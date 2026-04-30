# 2-Port CPU d-port → APB Bridge Write Corruption (FPGA-only)

Status: **diagnosed, not yet fixed**.
Workaround: insert ≥4 register ops between any d-port load and a subsequent SW
to the APB bridge that consumes that load's result.

---

## Symptom

On Cyclone V (DE10-Standard) with `hazard3_cpu_2port` + `ahbl_crossbar` (N_MASTERS=2):

- All bytes written to the UART (APB bridge) come out as `0xD0`, regardless of source value
- Same firmware in Verilator simulation: works correctly
- 1-port CPU + N_MASTERS=1 crossbar: works correctly on FPGA
- STA timing closure: clean (+0.733 ns setup, +0.210 ns hold), no critical warnings

## Trigger conditions (all required)

1. **Sustained d-port traffic** preceding (e.g. 200000-iter `for(volatile uint32_t i = 0; i < N; i++)`)
2. **Tight `LBU c → SW UART_TX = c`** with no register ops between (the byte-load result is consumed as the next-instruction store data)
3. The SW target is a **slow slave** (APB bridge, multi-cycle data phase)
4. **Continuous pipelined execution** — the bug does NOT fire under JTAG single-step

Failing C pattern (typical):
```c
__attribute__((noinline))
void uart_putc(char c) {
    while (UART_FSTAT & UART_FSTAT_TXFULL);  // or any heavy d-port loop
    UART_TX = c;                              // gcc emits LBU c immediately followed by SW
}
```

## Confirmed workaround

Force gcc to emit register-ops between the byte load and the bridge SW:

```c
__attribute__((noinline))
void uart_putc(char c) {
    while (UART_FSTAT & UART_FSTAT_TXFULL);
    asm volatile (
        "nop\nnop\nnop\nnop\n"
        "lui  t1, 0x40004\n"
        "addi t1, t1, 12\n"
        "sw   %0, 0(t1)\n"
        :
        : "r" ((uint32_t)(unsigned char)c)
        : "t1", "memory"
    );
}
```

Verified: this prints `Hi!\r\n` cleanly. Disassembly shows ~6 register ops between the LBU of `c` and the SW to UART_TX.

## Diagnostic evidence

### Test taxonomy

| Test | Pattern | Result |
|------|---------|--------|
| A    | GDB SBA write to UART | ✅ |
| B    | Fixed-char firmware (no string read) | ❌ garbage |
| B2   | Halt after `uart_init`, GDB writes 'U' via SBA | ✅ |
| C    | Reduce clock (12 MHz, 4 MHz) | ❌ — not timing |
| E    | Single `UART_TX = 0x55`, no FSTAT | ✅ |
| F    | Loop of writes with delay (no FSTAT) | ❌ garbage at any delay |
| G    | Two back-to-back TX writes, no SRAM ops | ✅ |
| H    | APB-write, 1 SRAM-write, APB-write | ✅ |
| I    | APB-write, ~10 s register-only delay (no d-port), APB-write | ✅ |
| J    | APB-write, 4M d-port READS to SRAM, APB-write | ✅ |
| J'   | APB-write, 4M d-port WRITES to SRAM, APB-write | ✅ |
| K    | 200k LW+ADDI+SW to GLOBAL same address (asm), APB-write | ✅ |
| L    | Stack `for(volatile i=0; i<200k; i++)` in main, APB-write | ✅ |
| M    | Multiple (loop + APB write) sequences in main, no function call | ✅ |
| N    | Same pattern but inside a `noinline` function called from main | ❌ garbage |
| O    | Heavy loop, then `volatile char tmp='B'; UART_TX=tmp;` no function | ✅ (gcc inserted 3 reg ops between LBU and SW) |
| P    | Tight LBU→SW to bridge, NO heavy loop | ✅ |
| Q'   | Function with explicit asm forcing 4 NOPs between LBU and SW | ✅ |
| Path 3 | 256 bytes 0x00..0xFF via buggy pattern + log via main (clean SRAM SW) | UART = `0xD0` for all 256 bytes; `log_buf` correct (`0x00..0xFF`) |
| stepi | Halt at SW (PC=0x27c), inspect regs, single-step | a4=0x42 just before SW; SW emits `0x42` correctly under stepi |

### What stepi proved

At PC = 0x27c (the failing `sw a4, 0(a5)` in `buggy_write`) with `c=0x42`:
- `$a4 = 0x42` (correct)
- `$a5 = 0x4000400c` (correct UART_TX address)
- Memory at `[$s0-33] = 0x42` (c stored correctly to stack)
- Memory at `[$s0-20] = 0x00030d40` (i=200000, expected loop-exit value)
- After `stepi`: UART receives `0x42`

So:
- The CPU register file holds the right value
- SRAM contents are correct
- The store, executed alone, emits the right byte
- The corruption only happens when the SW's data phase is concurrent with the previous LBU's data phase under continuous pipelined execution

## Root-cause class (without bus-signal observation)

Bug requires the specific overlap of:
- LBU c's data phase (cycle K-1: SRAM ack, mw_result update)
- SW UART_TX's address phase (cycle K-1: bridge sees haddr/htrans)
- SW UART_TX's data phase (cycle K: bridge captures pwdata at end of S_WR0)
- AND sustained d-port pipeline state from the heavy loop

Static analysis of `hazard3_core.v` and `hazard3_cpu_2port.v` does not reveal a functional defect; STA is clean; Verilator passes. The bug only manifests in synthesized hardware under steady-state pipelined execution.

Without ChipScope/SignalTap (not available in this build), we can't observe the exact corrupted signal. Candidates remaining:
1. `mw_result` updating one cycle late vs. expected during pipelined LBU→SW
2. Combinatorial glitch on the splitter `dst_hwdata` broadcast path
3. Bridge `pwdata` sampling at S_WR0 catching an in-transit value
4. Synthesis-specific behavior of the bypass mux not reproduced by Verilator

## Constants observed

- All buggy bytes are `0xD0`, regardless of source value (256 distinct inputs all produce `0xD0`)
- The value `0xD0` does not appear directly in any obvious place (loop counter `i` ends at `200000 = 0x00030D40`; bytes `0x40, 0x0D, 0x03, 0x00`, no `0xD0`)

## Tests we did not run

- SignalTap (Quartus ILA) — no license/setup available
- Custom debug peripheral that latches the last few bridge transactions into APB-readable registers — feasible but not built

## Investigation log (2026-04-26)

### Confirmed via continued investigation

1. **Bug persists after pulling upstream Hazard3** (HEAD `8af9929`). It is not
   specific to our fork's altera-compat patches.
2. **Bug persists after replacing all local `busfabric/*.v` with upstream
   libfpga's versions** (different state machine in `ahbl_to_apb.v`, AHB5
   atomics ports in arbiter/splitter/crossbar). It is not in the bus fabric.
3. **Bug persists with dual-port (1R1W) SRAM** (`USE_1R1W=1`,
   `HAS_WRITE_BUFFER=0`). It is not a single-port SRAM hazard.
4. **`REDUCED_BYPASS=1` makes the bug go away.** This points the finger at the
   M-stage store-data bypass network in `hazard3_core.v`.

### Smoking-gun byte value

The corrupted byte is consistently `0xD0` regardless of source value. From a
gdb halt at the failing SW (`sw a4, 0(a5)`) immediately after the loop:

```
$a4 = 0x42       (correct, matches the c argument we passed)
$a5 = 0x4000400c (correct UART_TX address)
$s0 = 0x1ffd0    (frame pointer)  ← byte[7:0] = 0xD0
$sp = 0x1ffa0
mem[$s0-33] = 0x42
mem[$s0-20] = 0x00030d40 (i = 200000)
```

`s0[7:0] == 0xD0` matches the corrupted byte exactly. For a load instruction,
`xm_result <= x_alu_result = x_op_a + x_op_b = s0 + 0 = s0`. So `xm_result` for
the LBU is **the LBU's address**, and that address happens to have low byte
`0xD0`.

The SW in cycle K *should* pull `m_wdata` from `mw_result` (LBU's loaded data,
e.g. `0x42`) via the M-stage bypass. Instead it pulls from `xm_result` (LBU's
address `0x1FFD0`), so the bridge captures `0x0001FFD0` and the UART TX FIFO
takes byte[7:0] = `0xD0`. The pattern repeats for every byte we attempt to
send through the failing path.

### Why Verilator hides it

Verilator (and any cycle-accurate simulator) treats combinational logic as
resolving instantaneously between clock edges. At the posedge ending the
LBU's M cycle:
- `mw_result <= m_result` (LBU's loaded data)
- `mw_rd <= xm_rd = 14` (a4)
- `xm_result <= s0` (forwarded from LBU.xm_result via X-stage bypass)

All update atomically. In the next cycle, the M-stage bypass mux sees
`xm_rs2(=14) == mw_rd(=14)` and picks `mw_result` correctly.

### Why FPGA exposes it

On real silicon, signals propagate with non-zero delays. The data input to
the `mw_rd` FF goes through a long combinational path:

```
SRAM_hready_resp
  → splitter src_hready_resp
  → d_hready (back to CPU)
  → bus_dph_ready_d
  → m_bus_stall
  → m_reg_wen_if_nonzero
  → mux that selects between xm_rd and mw_rd_q
  → mw_rd FF data input
```

If this path's value isn't stable by the FF's setup window — *for the
specific input pattern of a heavy d-port loop into a tight LBU→SW* — the
FF captures the previous-instruction's value and the bypass condition
evaluates to false, falling through to `xm_result`.

### Why clock frequency does not matter

Setup-time slack scales with clock period. At 4 MHz (250 ns) we have orders
of magnitude more slack than needed; if it were a setup violation, lowering
the clock would absolutely fix it. **It does not.** Therefore this is not a
setup-time issue.

Hold-time slack does NOT scale with clock period. STA reports the worst hold
slack at +0.226 ns — passing, but very tight. Hold violations and
synthesis-specific issues (e.g. don't-care optimizations of the casez default
`32'hxxxx_xxxx` for stores in `m_rdata_pick_sext`) are independent of clock
frequency, which matches our observation.

### Why 1-port works

`hazard3_cpu_1port.v` shares one bus between instruction fetch and load/store.
LBU's data phase and SW's address phase **cannot overlap** — the bus
serializes them. There is therefore at least one extra cycle between LBU
retiring from M and SW entering M, during which `mw_rd`/`mw_result` settle
unconditionally. The race never opens.

`hazard3_cpu_2port.v` has separate i-port and d-port. The d-port pipelines
LBU's data phase with SW's address phase on the same cycle (AHB pipelining).
The X→M transition for SW happens at the **same** posedge that LBU completes M.
The race window opens.

### What we ruled out
- Wiring/parameters (audited line-by-line)
- Bus fabric (replaced with upstream libfpga; bug persists)
- Hazard3 core (upstream HEAD; bug persists)
- SRAM single-port hazards (1R1W; bug persists)
- Setup-time violations (bug persists at 4 MHz)
- Compiler artifacts (bug reproduces with hand-written inline asm)

### What pinpoints the cause

1. `REDUCED_BYPASS=1` (which removes the M-stage bypass entirely and forces
   load-use stalls) makes the bug go away.
2. Inserting ≥4 NOPs / register ops between LBU and SW in firmware makes the
   bug go away (because gcc's normal scheduling already provides this gap).
3. Single-stepping (`stepi`) the SW instruction makes it write the right byte
   even on FPGA — because the halt drains the pipeline and the LBU's M
   completion is not concurrent with SW's M entry on resume.

These three together localize the bug to the **M-stage bypass network in
`hazard3_core.v`**, specifically the path leading to the `mw_rd` /
`mw_result` capture FFs, when the prior M-stage instruction is a load that
just retired and the current M-stage instruction is a store consuming that
load's destination.

### Targeted RTL fix (verified to work)

Removing the **store exception in `x_stall_on_raw`** at `hazard3_core.v:467`
makes the bug go away while preserving every other forwarding path.

Original code:
```verilog
end else if (|xm_rd && xm_rd == d_rs2) begin
    // Store data can be bypassed in M. Any other instructions must stall.
    x_stall_on_raw = !(d_memop == MEMOP_SW || d_memop == MEMOP_SH || d_memop == MEMOP_SB);
end
```

Patched code:
```verilog
end else if (|xm_rd && xm_rd == d_rs2) begin
    x_stall_on_raw = 1'b1;        // always stall, no store exception
end
```

This change forces a 1-cycle stall when a store's RS2 is the destination of
an in-flight load. After the stall, the load is fully retired (regfile
written), and the store reads RS2 from the regfile normally — no bypass mux,
no race.

### Performance impact

The stall fires only when the **previous** instruction is a load whose
destination register is **immediately consumed** by the **current** store as
its data operand. For typical compiler-emitted code this is rare, because
optimisers schedule a register-using instruction between LBU and a dependent
SW (load-use scheduling has been a standard optimization for decades).

Empirically the bug was only triggered by the contrived
`for(volatile uint32_t i = 0; i < N; i++); UART_TX = c;` pattern in
`uart_putc(c)`-like functions, where:
- the heavy d-port loop populates the pipeline state needed to expose the
  race
- gcc emits `lbu c, -X(s0); sw c, 0(uart_tx_addr);` with no register op
  between (because c is a function parameter loaded from the stack, and the
  literal MMIO address has already been computed)

In normal code the stall almost never fires, so the dynamic-cycle cost is
near-zero.

### What we know vs. what's hypothesis (without bus-signal observation)

**Known for sure:**
1. The bridge captures `pwdata = 0x????1FD0`, the UART takes byte[7:0] = `0xD0`.
   This value is `s0[7:0]` (frame pointer's low byte). For a load,
   `xm_result = x_alu_result = s0 + 0 = s0`. So `xm_result` is the source of
   the leaked value.
2. `m_wdata` for the SW is falling through to `xm_result` instead of the
   intended `mw_result`. Therefore the bypass condition
   `xm_rs2 == mw_rd && |mw_rd` is evaluating to FALSE on FPGA when it should
   evaluate to TRUE.
3. Disabling the bypass via `REDUCED_BYPASS=1` fixes it.
4. Inserting NOPs (or any reg-op) between LBU and SW in firmware fixes it.
5. Setting `x_stall_on_raw = 1'b1` for the LBU→SW pattern (option B) fixes it.
6. Single-stepping (`stepi`) the failing SW always emits the right byte —
   because the halt drains the pipeline and the LBU/SW concurrency that
   feeds the race is gone.

**Hypothesis (most likely cause, not directly observed):**
Either `mw_rd` or `mw_result` (or both) fail to latch the LBU's destination
at the right posedge on FPGA. The candidate mechanism is the long
combinational enable path
`SRAM_hready_resp → splitter → src_hready_resp → d_hready → bus_dph_ready_d
→ m_bus_stall → m_reg_wen_if_nonzero → mw_rd/mw_result FF gate`
producing an enable that is unstable around the FF's setup/hold window.
This is a bug specific to the 2-port pipeline because in 1-port LBU's data
phase and SW's address phase don't overlap (bus-sharing serializes them),
giving the enable an extra cycle to settle.

**Definitive confirmation requires** observing `mw_rd`, `mw_result`,
`m_wdata`, and `bus_wdata_d` in the failing cycle. Without SignalTap we plan
to build a small APB-mapped "bus snooper" peripheral that captures these
signals into a ring buffer readable via JTAG SBA.

## Plan: pull upstream Hazard3 + libfpga

### Audit of our forks (2026-04)

Our Hazard3 fork (pinned `7949618`, base `787da131` from Dec 2024, vs upstream HEAD `8af9929` from Mar 2026):

1. **`hdl/hazard3_instr_decompress.v`**: `uop_ctr` generate block commented out.
   Causes W(10858) "uop_ctr used but never assigned". **Inactive in our build**
   (`EXTENSION_ZCMP=0`, the consumer expressions all gate on `|EXTENSION_ZCMP`).
   Patch is broken but harmless. **Discard.**

2. **`example_soc/soc/peri/hazard3_riscv_timer.v`**: added an undriven
   `wire tick_nrz`, hooked the synchronizer to it instead of the `tick` input,
   and renamed `tick_nrz_sync_prev` → `tick_nrz_prev`. **Inactive in our build**
   (`TICK_IS_NRZ=0` default, the kws_soc instantiation does not override it; the
   else-branch `assign tick_event = tick;` is taken). Patch is broken but
   harmless. **Discard.**

3. **`.gitmodules`**: URLs changed to internal SSH mirrors at
   `github.com:INITRAMFS-AUC/`. Pure convenience. **Keep or revert — preference**.

4. **Submodule pointer bumps**: carry the libfpga patch (and a `scripts` bump).

Our libfpga fork (pinned `cb52ad5`, base `9d50e12`, vs upstream HEAD `964ceca`):

1. **`mem/ahb_cache_writeback.v`**: removed a stray `;;` (double semicolon).
   Trivial cleanup; could be upstreamed as a one-liner. **Discard / send PR.**

### Conclusion

**No HDL patch in either fork is needed for our SoC.** Both broken patches sit
in dead code paths; the libfpga patch is cosmetic.

### Upstream pull executed (2026-04-26)

- Hazard3 submodule moved to upstream HEAD `8af9929` (Mar 2026)
- libfpga submodule moved to upstream HEAD `964ceca` (Dec 2025)
- Re-applied a single benign fix in libfpga: removed `};;` (double semicolon) at
  `mem/ahb_cache_writeback.v:539` (Quartus rejects double semicolons; upstream
  accepts it). This is the only local diff against upstream.
- `hazard3_config.vh` updated for the new parameter list (added `EXTENSION_E`,
  `EXTENSION_ZBKX`, `EXTENSION_ZCLSD`, `EXTENSION_ZILSD`, `PMP_MATCH_NAPOT`,
  `PMP_MATCH_TOR`; removed `MIMPID_VAL` and `MHARTID_VAL` which became ports).
- `kws_soc.v` updated for the cpu_2port port list change (removed
  `.MIMPID_VAL` / `.MHARTID_VAL` overrides; added `mhartid_val`,
  `eco_version`, `fence_i_vld`, `fence_d_vld`, `fence_rdy` port connections).
- Quartus build clean (0 errors, 0 critical, 116 warnings vs prev 118).

### Bug status after upstream pull

**The d-port bridge corruption bug PERSISTS in upstream Hazard3.** Re-running
the exact Path-3 test (4 × 0x55 via clean_write, then 256 bytes 0x00..0xFF via
buggy_write):

```
0000: 55 55 55 55 d0 d0 d0 d0 d0 d0 d0 d0 d0 d0 d0 d0
...
```

Identical signature to before. So this is a real upstream bug, not specific to
our fork's altera-compat patches. We are likely the first to exercise this
exact (heavy d-port loop + tight LBU→SW to slow APB slave) pattern on a
2-port build with a multi-cycle bridge.

### Next steps

- **Short-term (today):** apply the firmware workaround (`clean_write`-style
  inline asm) in `uart_putc` and any other function that emits a tight load+SW
  pattern to a slow slave.
- **Medium-term:** file an issue / minimum reproducer with upstream
  (Wren6991/Hazard3) using our test taxonomy. Include the exact failing C
  function, disassembly, and the constant-0xD0 corruption signature.
- **Long-term:** build the small "bridge transaction snooper" debug peripheral
  to observe `bus_wdata_d` and `apbm_pwdata` directly via JTAG SBA, since
  SignalTap is not available.

## Files relevant to the bug

| Path | Role |
|------|------|
| `Hazard3/hdl/hazard3_cpu_2port.v` | 2-port CPU wrapper; `bus_active_dph_d`, `bus_hold_aph`, d-port arbiter |
| `Hazard3/hdl/hazard3_core.v` | Core; `bus_wdata_d` / `m_wdata` / `mw_result` forwarding logic |
| `busfabric/ahbl_arbiter.v` | N:1 arbiter per slave |
| `busfabric/ahbl_splitter.v` | 1:N splitter per master; `slave_sel_d`, `dst_hwdata = {N{src_hwdata}}` |
| `busfabric/ahbl_crossbar.v` | Crossbar wrapper; `CONN_MATRIX = 6'b111_101` |
| `busfabric/ahbl_to_apb.v` | APB bridge; `pwdata <= ahbls_hwdata` at end of S_WR0 |
| `Hazard3/example_soc/libfpga/mem/ahb_sync_sram.v` | SRAM with write buffer |
| `Hazard3/example_soc/libfpga/mem/sram_sync.v` | Inferred-BRAM SRAM body |

## Reproducer firmware

`test/uart/c/hello_world.c` was iteratively reduced to confirm each test in the table above. The most concise repro:

```c
__attribute__((noinline))
void buggy_write(char c) {
    for (volatile uint32_t i = 0; i < 200000; i++);
    UART_TX = c;
}
int main() {
    uart_init();
    while (1) buggy_write(0x42);
}
```

UART output: stream of `0xD0`.

---

## Bus-snooper investigation (2026-04-28)

We built the bus snooper proposed in "Next steps" above and ran the
`snooper_test` firmware on FPGA, in Verilator, and in Verilator with an
artificially-induced 1-cycle SRAM read wait state. Findings below.

The infrastructure (`peris/snooper/bus_snooper.v`, `test/uart/c/snooper_test.c`,
`scripts/dump_snooper.gdb`, the d-port and CPU-internal taps via
`patches/debug-snooper-hazard3_taps.patch`) lives on the `debug-snooper`
branch and is preserved here as a reusable debug peripheral. It is *not*
instantiated on `main` by default; bring it back as needed.

### What the snooper captured

For each `buggy_write(0x42)` call, the FPGA ring buffer shows **two**
bridge transactions to `0x4000_400c`, two cycles apart:

| field | TX1 (the spurious one) | TX2 (the "real" SW@1c4) |
|---|---|---|
| `bridge_haddr[15:0]` | `0x400c` | `0x400c` |
| `dport_haddr` | `0x4000_400c` | `0x4000_400c` |
| `bridge_hwdata` | `0x0001_ffe0` (= a stack address) | `0x42` |
| `m_wdata` | `0x0001_ffe0` | `0x42` |
| `xm_rs2`, `mw_rd` | `0`, `15` (no bypass) | `14`, `14` (bypass fires) |
| `xm_memop` (M-stage) | `04` (LBU) | `04` (LBU) |
| `m_bus_stall` | `1` | `0` |

Both transactions have the *same* `dport_haddr`, so the crossbar is not
misrouting — the CPU's d-port itself is presenting two aphases to the
bridge. The first fires while M-stage is still on the LBU
(`m_bus_stall=1`); m_wdata at that moment is whatever the bypass mux
produces for the LBU's M-stage state, which is the LBU's own
*address-calc result* (the load address, not the loaded byte). Low byte
`0xE0`/`0xD0` is what we see in UART output as the "garbage" byte.

The second transaction fires after M-stage advances to the SW; the bypass
condition `xm_rs2 == mw_rd` (both = `a4` = 14) is true, so
`m_wdata = mw_result = 0x42`, the correct data.

The clean_write entries (with the asm-block NOP separation) emit one
transaction per call with correct data.

### Reproducing in Verilator

Unmodified Verilator runs show **8 ring entries** (4 clean + 4 buggy
correct, no spurious mid-buggy). Identical RTL, identical firmware, and
yet sim is clean and FPGA isn't — so the trigger is something Quartus /
the M10K does that the behavioural `sram_sync` doesn't model.

Adding a single hready=0 wait state on every SRAM read in
`ahb_sync_sram` (parameter `EXTRA_RD_WAIT=1`, off by default; see
`patches/debug-snooper-sram_wait.patch`) is sufficient to reproduce the
12-entry FPGA pattern *and* the corrupted UART output in Verilator. So
the FPGA-only mechanism is timing on the d-port read response, and the
behavioural SRAM model just happens to be one cycle faster than M10K's
realised path.

### Why the proper fix is hard from the snooper alone

We attempted a minimal fix in `hazard3_core.v`: gate `bus_aph_req_d` so
the same X-stage instruction's aphase fires exactly once
(`reg x_aph_done`, set on `bus_aph_ready_d`, cleared on `!x_stall`).
**It did not work.** Re-tracing the failing scenario with the wait
state:

- At the cycle the SRAM wait state ends, `m_bus_stall` drops to 0.
- `x_stall` already includes the term `bus_aph_req_d && !bus_aph_ready_d`,
  which goes to 0 the same cycle the aphase is accepted.
- So `!x_stall` becomes 1 in the same cycle as `bus_aph_ready_d=1`, and
  the reset branch wins the race in the always-block — `x_aph_done`
  never latches.

Beyond that, the snooper's two-entries-per-buggy-write is partly a
**snooper-side double-fire**: the snooper's `bridge_hready` input is
`dst_hready` from the crossbar (= the master's broadcast hready), which
transitions `0→1` once when the SRAM wait-state ends *with the splitter's
`slave_sel_d` still pointing at SRAM* and again when the bridge actually
finishes its dphase. That gives two snooper commits for one bridge
transaction, with two different `m_wdata` snapshots. Yet UART still
receives the wrong byte, so the bridge *must* be sampling `pwdata` from
the wrong cycle as well. We could not pin down which cycle without a
waveform.

### What we'd do next, if the bug becomes worth more time

1. `make sim-verilator-vcd FLASH=test/build/snooper_test_xip.bin NO_JTAG=1`
   and inspect the cycle when `apb_bridge_u.apb_state == S_WR0`.
2. The single resolving question: *what is `cpu.core.m_wdata` at that
   cycle?* If `0x42`, the bridge is mis-capturing — fix lives in
   `ahbl_to_apb` or the splitter's slave-select handoff. If
   `0x0001_ffd0` (the stack address), the M-stage hasn't advanced when
   bridge captures — fix lives in the CPU's pipeline-advance / aph_req
   logic, but the gating condition must be different from what we tried.

### Decision: ship the working-fpga workaround

The `working-fpga` branch's `x_stall_on_raw = 1'b1` change in
`hazard3_core.v` (force a one-cycle X-stage stall on every load →
store-data RAW pair) is verified correct on FPGA and costs at most one
extra cycle per such pair, only on the rare patterns that exercise it.
That cost is far smaller than the time we have already spent looking
for the optimal fix, and it keeps the SoC shippable without further RTL
risk. The snooper, taps, and reproducer infrastructure stay in tree as
debug tools so a future investigator (us or upstream) can pick this up
with the diagnostic ladder already built.
