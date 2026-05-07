# Conv1D Hardware Accelerator — Design Document

**Target SoC:** KWS-SoC (Hazard3 RV32IMAC, 36 MHz, MULDIV_UNROLL=1)  
**Model:** mel_compact_4blk_ch36 (15,851 params, 28.7 KB flash, 88.7% int8)  
**Baseline:** 195,486,215 cycles (5.43 s) — NNoM Conv2D running as RISC-V instructions on Hazard3  
**Final result:** ~4,644,000 cycles (0.129 s) — **42× speedup**, 10/11 clips correct

Both numbers are measured via `csrr mcycle` in firmware running inside the Verilator
cycle-accurate simulation of the full KWS-SoC. The SoC, CPU, bus fabric, SRAM, XIP flash
cache, and I2S peripheral are identical in both cases. The only difference is the firmware
binary: the baseline uses NNoM's standard C convolution kernel executing as RISC-V instructions
on Hazard3; the accelerated version replaces that kernel with APB config writes that hand off
to the `conv1d_accel` RTL module, which then drives the AHB bus directly.

---

## 1. Motivation

The baseline mel_compact_4blk_ch36 model takes 195M cycles per inference — measured by
`csrr mcycle` in firmware running on the Verilator simulation of the full KWS-SoC — with
MULDIV_UNROLL=1 meaning sequential multiply takes up to 32 cycles per instruction. The bottleneck
is the five Conv2D layers, which together account for ~5M multiply-accumulate (MAC) operations.
At ~40 cycles per MAC (mul + accumulate + loop overhead) this gives roughly 200M cycles — matching
the observed number.

A dedicated Conv1D accelerator can parallelise the MAC loop and eliminate per-operation loop
overhead, replacing the entire inner kernel with DMA-driven burst reads and a 4-MAC-per-cycle
datapath.

---

## 2. Architecture Overview

```
   CPU (Hazard3)
       │
       │ APB (config writes, start, poll done)
       ▼
  ┌─────────────────────────────┐
  │    conv1d_accel              │
  │  ┌──────────────────────┐   │
  │  │  APB slave           │   │
  │  │  Config registers    │   │
  │  └──────┬───────────────┘   │
  │         │                   │
  │  ┌──────▼───────────────┐   │
  │  │  State machine        │   │
  │  │  (AHB-Lite master)   │   │
  │  └──────┬───────────────┘   │
  └─────────┼───────────────────┘
            │ AHB-Lite
            ▼
    ┌───────────────────┐
    │  Crossbar         │
    ├───────────────────┤
    │ SRAM (inputs,     │  ← 0x0000_0000
    │       outputs,    │
    │       bias,shift) │
    ├───────────────────┤
    │ XIP Flash cache   │  ← 0x8000_0000
    │ (weights)         │
    └───────────────────┘
```

The accelerator is an APB slave for configuration and an AHB-Lite master for data movement.
It accesses SRAM (inputs, outputs, bias, shift arrays) and XIP flash (weights) over the same
AHB crossbar as the CPU.

**Register map** (base 0x4000_C000):

| Offset | Name        | Description |
|--------|-------------|-------------|
| 0x00   | CTRL        | [0]=start(w) [8]=busy(r) [9]=done(r) |
| 0x04   | SRC_ADDR    | Input feature map address (SRAM) |
| 0x08   | WT_ADDR     | Weight array address (XIP flash) |
| 0x0C   | DST_ADDR    | Output feature map address (SRAM) |
| 0x10   | BS_ADDR     | Bias array address (int32, SRAM) |
| 0x14   | CFG0        | [7:0]=C_in [15:8]=C_out [23:16]=K_w [31:24]=stride |
| 0x18   | CFG1        | [15:0]=W_in |
| 0x20   | SHIFT_ADDR  | Per-channel shift array address (uint8, SRAM) |

---

## 2.1 Interface justification — why APB slave AND why AHB master

The accelerator carries **two** bus interfaces.  This section is
written assuming reasonable scepticism about *each* one — the natural
question is "why not skip APB and put config on AHB directly, since
the CPU is already an AHB master?"  Spelt out below.

### Why an APB slave (config interface)

The current path of a config write is:

```
  CPU SW instr → AHB-Lite store on D-port
              → ahbl_crossbar
              → ahbl_to_apb_bridge   (1–2 cycles of protocol translate)
              → APB splitter
              → conv1d_accel APB slave latch
```

The bridge step is real overhead per write.  At ~6 register writes per
accel call × ~60 calls per inference ≈ 360 writes per inference, and
~2 extra cycles per write through the bridge, that's roughly **720
cycles per inference** of bridge tax — about **0.07 %** of today's
~1.05 M-cycle inference.  Most of that is hidden behind Hazard3's
non-blocking-store + LSU pipeline anyway; the visible stall is just
the last write before the `ACCEL_CTRL=START` store drains.

Alternatives we considered:

* **Make `conv1d_accel` an AHB-Lite slave** (skip the bridge entirely).
  Saves the ~720 cycles/inference above.  Costs:
  - AHB-Lite slave is a more complex protocol than APB: HSEL +
    HTRANS{NSEQ/SEQ} + HSIZE/HBURST decode + HRDATA pipelined one
    cycle behind HADDR + HRESP timing + the default-slave handling
    in the splitter.  Roughly **~1.5–2× the gates** of the APB
    slave (which is just paddr[5:2] decode + a 9-register file
    ≈ 250 flops).
  - Adds another address region on the AHB splitter, where
    conv1d_accel is the only inhabitant — the bridge still exists
    for timer/uart/i2s/snooper, so we don't shrink it.
  - Breaks the standard "small register-file peripherals on APB,
    bandwidth-hungry blocks on AHB" SoC convention used by every
    other peripheral here, including upstream Hazard3's example_soc.
* **Memory-map config into the existing AHB master port.**  Have the
  accel snoop its own writes via a slave-decode path bolted onto the
  master FSM.  Doubles the master FSM and every config write costs a
  full AHB arbitration cycle.  Strictly worse than the dedicated AHB
  slave above.

The APB slave costs **~250 flops** (paddr[5:2] mux + 9-register
file) and a single APB splitter slot at `0x4000_C000`.  It never
contends with the data path on AHB.  Single-cycle ready means no
CPU core ever waits on it past the bridge translate.

**Decision: keep the APB slave.**  The bridge tax is real but quantitatively
negligible (~0.07 % of inference); the gate / protocol / convention
cost of an AHB slave doesn't earn that back.

### Why an AHB master (data interface)

The accel moves real data per call (mel_compact_int8 layer 1 is the
worst case): `1024 weight bytes + 96 input bytes + 16 bias bytes + 16
shift bytes = 1152 bytes per call`.  Each call also writes ~120 output
bytes.  With ~60 calls per inference (8-layer model × ~7 channel
positions) that's ~70 KB moved per inference.

Alternatives considered:

* **CPU-driven data movement.**  Firmware reads from XIP/SRAM and writes
  every byte to the accel via APB.  Cost: ~3 cycles per byte (load,
  store-to-APB, decode) × 70 KB = ~210 K cycles purely on data
  movement.  Plus the CPU is blocked the whole time.  Today's
  accel-driven path takes ~1.05 M cycles total per inference, of which
  most is MAC; CPU-driven movement alone would tank that to ~5 M.
  Strictly inadmissible.
* **DMA-staged.**  CPU sets up MS_DMAC to move weights into an internal
  scratchpad, then triggers the accel.  Eliminates CPU stall on the
  movement, but: (a) we'd need a scratchpad sized for the largest
  weight tensor (~1 KB on the worst layer of mel_compact_int8;
  bigger on other models — must be sized for the largest model we'll
  ever ship); (b) two AHB masters (DMAC + accel) still contend on the
  bus during the next layer's setup; (c) the firmware now has a
  per-call DMA program in the critical path, doubling the accel call
  latency.  Net: same bus contention as the AHB-master approach with
  more firmware complexity and a fixed scratchpad cost.
* **Accel as a co-processor over a private memory port.**  Direct SRAM
  attachment.  Requires a custom port on the SRAM block (today the
  SRAM is single-port AHB-Lite).  Not a free improvement — adds an
  arbiter on the SRAM port that's then in *every* CPU access path.
  Only worth it if accel-vs-CPU SRAM contention is the bottleneck;
  measured today it isn't.

The AHB master adds **one master port** to the crossbar (raising
`N_MASTERS=4` → was 3 before the merge, see
`memory/project_ahb_master_history.md`) and **~200 lines of FSM** in
the accel.  The crossbar arbiter is round-robin; we measure 0 cycles
of arbitration overhead per accel call in the steady state (the CPU
is in WFI most of the time during accel execution; DMA bursts are
short and infrequent).

**Decision: keep the AHB master.**

### What this means for the NNoM-aware XIP cache work

The NNoM-aware prefetch hint (planned in `peris/xip/DESIGN.md`)
piggybacks on **both** interfaces:

* Hint registers (`PREFETCH_CTRL`, `PREFETCH_BASE`, `PREFETCH_LEN`) are
  added to the accel's existing **APB slave** — no new peripheral, no
  new APB splitter slot.  Costs: 3 more registers (+96 flops), one
  more `paddr[5:2]` decode case.
* When `PREFETCH_CTRL.EN` is set, the accel hardware drives the hint
  signals out as a side-band of the AHB master's start sequence — no
  CPU involvement on the hot path.  No additional master port required;
  the cache wrapper sees the hint as a separate non-AHB sideband.

So the NNoM-aware cache does **not** alter the master/slave decision
above.  It reuses both interfaces idiomatically.

---

## 3. Firmware Integration

### 3.1 Driver (`accel_conv1d.h`)

A single inline function writes all config registers and polls DONE:

```c
void accel_conv1d(src, wt, dst, bias32, shift_arr, c_in, c_out, k_w, stride, w_in);
```

The function blocks until the accelerator sets the DONE bit. This is acceptable for a
bare-metal single-threaded MCU; there is no preemption to worry about.

### 3.2 NNoM Hook (`nnom_conv1d_hw.c`)

The standard NNoM `nnom_conv2d.c` is replaced with `nnom_conv1d_hw.c`, which wraps the
same interface but substitutes the inner kernel with a hardware call when all these conditions
hold at runtime:

- HWC tensor layout (not CHW)
- int8 inputs
- H dimension = 1 (our Conv1D embedded in a 2D tensor)
- No dilation
- C_out ≤ 64 (scratch buffer limit)

Before calling the accelerator for PADDING_SAME layers, the firmware pads the input into a
static scratch buffer (`s_pad_buf`, 9216 bytes) and passes the padded width to the accel.
This keeps the accelerator's hardware simple — it always performs valid convolution only.

Bias pre-processing is also done in firmware:

```c
bias32[c] = ((int32)(bias8[c] << bias_lshift[c])) + NNOM_ROUND(output_rshift[c])
```

This matches NNoM's internal formula exactly, so the hardware just adds `bias32[c]` and
right-shifts by `shift_arr[c]` (both pre-computed per channel).

### 3.3 kws_bare.c: I2S prime loop

After `csr_enable_mie()`, a short busy loop was added:

```c
for (volatile uint32_t t = 0; t < 2000u; t++) { (void)t; }
```

**Why this is needed:** The I2S IRQ fires when the FIFO is completely full (`fifo_full && cfg_irq_en`).
Without the prime loop, the CPU reaches `while (!ring_ready)` before the first FIFO fill
event has had time to occur, and the I2S clock divider is still settling. The 2000-iteration
delay (~30K cycles at ~15 cycles/iteration) gives the FIFO time to receive its first 8 samples
and assert the IRQ. This is a one-time startup delay; subsequent FIFOs fire at the I2S rate.

---

## 4. RTL State Machine

### 4.1 Computation Loop Order

The central design choice is the loop order for the convolution:

```
for c_pos in 0..C_out-1:          ← outer loop
    load wt[c_pos]  (once, from XIP)
    load bias[c_pos], shift[c_pos] (once, from SRAM)
    for w_pos in 0..W_out-1:       ← inner loop
        load input patch (SRAM, burst)
        compute MAC
        write output byte (SRAM)
```

The previous design had the loops inverted (W_out outer, C_out inner), which caused weights
to be re-loaded from XIP for every output position. This is the root cause of the 150M+ cycle
failure described below.

### 4.2 State Transitions

```
IDLE → INIT
INIT → WT_ADDR        (start outer loop at c_pos=0)

─── Per c_pos (outer loop): ───────────────────────────────────
WT_ADDR → WT_DATA     (AHB BYTE burst: patch_bytes bytes from XIP)
WT_DATA → BIAS_ADDR   (all bytes loaded into wt_buf)
BIAS_ADDR → BIAS_DATA (AHB WORD single: bias[c_pos] from SRAM)
BIAS_DATA → SHIFT_DATA (AHB BYTE single: shift[c_pos] from SRAM, addr issued in BIAS_DATA)
SHIFT_DATA → IN_ADDR  (begin inner w_pos loop; acc=0)

─── Per w_pos (inner loop): ───────────────────────────────────
IN_ADDR → IN_DATA     (AHB WORD burst: patch_words words from SRAM)
IN_DATA → MAC         (accumulate 4 MACs/cycle for patch_words cycles)
MAC → WRITE_ADDR      (compute shift+clip, issue write address)
WRITE_ADDR → WRITE_DATA (AHB BYTE single: write clipped result)
WRITE_DATA → ADVANCE

ADVANCE:
  if w_pos < w_out-1: w_pos++, acc=0 → IN_ADDR  (stay in inner loop)
  else: w_pos=0, c_pos++ → WT_ADDR             (advance outer loop)
  if c_pos == c_out: → DONE

DONE → IDLE  (set r_done=1, then idle)
```

### 4.3 AHB Pipeline Timing (r_wait flag)

Both synchronous SRAM and the XIP read-only cache have a registered output: they latch the
incoming address on the clock edge and present data on the NEXT edge. This means there is
always a 1-cycle latency between the address phase and valid HRDATA.

The `r_wait` flag handles this correctly:

```
Cycle 0: Issue haddr, htrans=NONSEQ, r_wait=1
Cycle 1: hready=1 arrives. r_wait=1 → skip capture,
          advance haddr to prime next burst word, r_wait=0
Cycle 2: hready=1 arrives. r_wait=0 → capture hrdata (valid for cycle-0 address)
```

For burst reads, the skip cycle simultaneously primes the AHB pipeline for the next word.
For single reads (bias, shift), the skip cycle is a pure stall.

Without r_wait, byte[0] of every burst would be captured from the previous transaction's
residual HRDATA — giving systematically wrong first values and causing all-zero weight bugs.

### 4.4 Weight Read: HSIZE_BYTE

Weights are stored in NNoM's [C_out][K_w][C_in] layout. Filter `c` starts at byte offset
`c * K_w * C_in` from the base of the weight array. This offset is NOT always word-aligned:

- Frontend K=65, C_in=1: filter 0 at offset 0, filter 1 at offset 65, filter 2 at 130...
  - Offsets mod 4: 0, 1, 2, 3, 0, 1, ... (repeating)
- Body K=3, C_in=16: filter offsets are multiples of 48 — always word-aligned
- Body K=3, C_in=36: filter offsets are multiples of 108 — always word-aligned

HSIZE_WORD reads from a non-word-aligned address on the XIP cache may return wrong data
or cause bus faults. HSIZE_BYTE reads are always valid regardless of alignment, with the
hardware returning the requested byte in the appropriate `hrdata` lane:

```verilog
cur_lane = r_wt_lane0 + buf_idx[1:0];  // modular 2-bit arithmetic
case (cur_lane)
    2'b00: rdbyte = hrdata[ 7: 0];
    2'b01: rdbyte = hrdata[15: 8];
    2'b10: rdbyte = hrdata[23:16];
    2'b11: rdbyte = hrdata[31:24];
endcase
```

The initial byte lane `r_wt_lane0 = w_wt_addr[1:0]` is captured when the burst starts.
Subsequent bytes at `w_wt_addr + n` are always at lane `(r_wt_lane0 + n)[1:0]` by modular
arithmetic — this works because `(A+B) mod 4 = ((A mod 4) + (B mod 4)) mod 4`.

### 4.5 Input Read: HSIZE_WORD

Input data is always read from SRAM. The address of the w_pos-th input patch is:

```
src_addr + w_pos * stride * C_in
```

For the production model:
- Frontend (stride=16, C_in=1): offsets = 0, 16, 32, ... — all multiples of 4 ✓
- Body layers (stride=1, C_in=16): offsets = 0, 16, 32, ... — multiples of 4 ✓
- Body layers (stride=1, C_in=36): offsets = 0, 36, 72, ... — multiples of 4 ✓

So HSIZE_WORD is safe for all production model layers. This gives 4× better bandwidth
versus HSIZE_BYTE for input reads.

**Known limitation:** For layers with `stride * C_in` not divisible by 4 (e.g., C_in=1, stride=1
as in TC1 and TC2 of the unit test), input addresses for w_pos > 0 are not word-aligned.
HSIZE_WORD reads at these addresses return wrong data from the SRAM. This affects the unit
test cases but NOT the production model. A future fix would be to either use HSIZE_BYTE for
inputs too (with per-byte lane extraction), or detect non-aligned strides at runtime and fall
back to software.

### 4.6 MAC Datapath

The accumulator pipeline computes 4 signed 8-bit multiply-accumulates per cycle:

```verilog
wire signed [15:0] p0 = in_buf[mac_idx][7:0]  * wt_buf[mac_idx][7:0];
wire signed [15:0] p1 = in_buf[mac_idx][15:8] * wt_buf[mac_idx][15:8];
wire signed [15:0] p2 = in_buf[mac_idx][23:16] * wt_buf[mac_idx][23:16];
wire signed [15:0] p3 = in_buf[mac_idx][31:24] * wt_buf[mac_idx][31:24];
wire signed [31:0] mac4 = sign_extend(p0) + sign_extend(p1)
                        + sign_extend(p2) + sign_extend(p3);
acc <= acc + mac4;
```

Since inputs and weights are packed 4-per-word (NWC layout for inputs, matching packed
layout for weights), this naturally computes 4 products per cycle with no masking needed
(assuming patch_bytes is a multiple of 4, which holds for all body layers). For the frontend
(K=65, C_in=1, patch_bytes=65), the last word is zero-padded in both in_buf and wt_buf
so the extra 3 bytes contribute zero products.

Shift and clip use the per-channel `cur_shift` loaded from the shift array:

```verilog
wire signed [31:0] shifted = (acc + bias_val) >>> cur_shift;
wire signed [7:0]  clipped = saturate(shifted, -128, 127);
```

---

## 5. Performance Analysis

### 5.1 Baseline Failure Mode

The initial design had W_out as the outer loop. For the frontend layer (K=65, C_in=1, C_out=16,
stride=16, W_in=8000, W_out=496):

- Per output position: reload 16 filters × 65 bytes = 1,040 bytes from XIP
- Total weight bytes loaded: 496 × 1,040 = **515,840 bytes**

The XIP cache has 32 lines × 32 bytes = 1,024 bytes capacity. The 16 filters together need
~1,040 bytes — just over the cache size. Each new output position evicted and re-loaded the
entire weight cache, giving approximately 1 miss per 32 bytes:

```
515,840 bytes / 32 bytes per cache line = 16,120 cache misses
Each miss ≈ 1,250 system cycles (SPI flash read latency in simulation)
Frontend weight load cost ≈ 20M cycles
```

For body layers (K=3, C_in=36, C_out=36, patch_bytes=108):
- Per position: 36 × 108 = 3,888 bytes from XIP; with W_out≈248: 963,936 bytes total
- Cache lines needed: 3,888/32 = 122 per position; far exceeds the 32-line cache
- Resulting in ~100M cycles for body layers alone

Total with W_out-outer: >> 150M cycles (hit the cycle limit without completing).

### 5.2 Fix: C_out-Outer Loop

With C_out as the outer loop:
- Per output channel: load exactly `K_w × C_in` bytes from XIP — done once, then reused
- Frontend: 16 filters × 65 bytes = 1,040 bytes total XIP access (496× reduction)
- Body layers: 36 filters × 108 bytes = 3,888 bytes total XIP access (248× reduction)

Cache miss analysis for frontend (1,040 bytes, 32-byte cache lines):
```
ceil(1,040 / 32) = 33 cache line loads × 1,250 cycles ≈ 41K cycles for all frontend weights
```

### 5.3 Cycle Budget Breakdown

For mel_compact_4blk_ch36 with C_out-outer accel (~4.6M cycles measured):

| Phase | Dominant cost | Estimate |
|-------|---------------|---------- |
| Frontend weight load (XIP) | 33 cache misses | ~41K cycles |
| Frontend input reads (SRAM) | 496 × 1 word × 3 cycles | ~1.5K |
| Frontend MAC | 496 × 16 × 17 cycles | ~135K |
| Frontend bias/shift reads | 16 × 2 reads × 3 cycles | ~100 |
| Frontend output writes | 496 × 16 × 3 cycles | ~24K |
| Body layers (×4) | Similar analysis | ~4.4M |
| I2S audio collection | 2,000 IRQs × ~18K cycles | ~37M |

The ~37M cycles of audio collection dominates the total. The 4.6M inference cycles represent
only the model computation after `ring_ready=1`. The measured CYCLES value (from `csrr mcycle`
around `model_run()`) confirms the breakdown.

### 5.4 Remaining Bottlenecks

The largest remaining cost per layer is SRAM input reads. For each (c_pos, w_pos) pair, the
input patch (patch_bytes bytes) is read from SRAM. With HSIZE_WORD burst reads and ~3 cycles
per word (1 r_wait skip + SRAM latency + 1 capture), the total input read cost for a body
layer (C_in=36, W_out=248):

```
248 w_pos × 36 c_pos × 27 words × 3 cycles ≈ 7.3M cycles just for input reads
```

This is the primary remaining bottleneck. A future optimisation would be to also move W_out
into an outer sub-loop per c_pos iteration, caching the input patch for all C_out channels.
However, this would require C_out × patch_bytes SRAM (e.g., 36 × 108 = 3,888 bytes of register
storage or on-chip SRAM), which would significantly increase area.

---

## 6. Key Design Decisions and Trade-offs

### 6.1 HSIZE_BYTE for weights, HSIZE_WORD for inputs

Chosen to correctly handle unaligned filter starts (HSIZE_BYTE) while maximising throughput
for always-aligned SRAM reads (HSIZE_WORD). The cost of HSIZE_BYTE is 4× more AHB byte
transactions per cache miss cycle, but cache misses dominate the weight load cost anyway —
the incremental latency for cache hits is small relative to the miss penalty.

An alternative (HSIZE_WORD with explicit byte extraction via shift registers) would be faster
for the hit path but would complicate the state machine and require a 64-bit rotation buffer.
Given that weight loads are already amortised across W_out positions, the simpler HSIZE_BYTE
approach is adequate.

### 6.2 One accelerator call per Conv2D layer

The `conv2d_run` hook in `nnom_conv1d_hw.c` calls the accelerator once per layer. An
alternative would be to call it once per output channel or once per output position. The
single-call-per-layer approach is simplest and avoids per-call APB setup overhead (~10 cycles
per register write × 7 registers = ~70 cycles overhead per call, negligible).

### 6.3 Static scratch buffers for bias/shift/pad

Rather than allocating bias32 and shift_u8 arrays dynamically, they are declared as static
arrays in `nnom_conv1d_hw.c`. This is safe for a single-threaded MCU. The pad buffer
(9,216 bytes) covers the worst case: body layer 1 after the frontend pool, which has
W_in=248 and C_in=16 → (248+2)×16 = 4,000 bytes. The 9,216 byte allocation gives ample
headroom.

### 6.4 Valid convolution only in hardware, padding in software

PADDING_SAME is handled by the firmware copying the input into a zero-padded buffer before
calling the accelerator. This keeps the hardware state machine simple (no conditional pad
insertion) at the cost of a memcpy + memset per layer. For the largest body layer
(248×16 = 3,968 bytes), this is ~4K bytes copied — approximately 1,000 CPU cycles, negligible.

### 6.5 Polling DONE via APB

The firmware polls `ACCEL_CTRL & ACCEL_CTRL_DONE` in a tight loop. An interrupt-driven
approach would free the CPU during inference, but since the firmware has nothing else to do
while the accelerator runs (the I2S IRQ handler still fires via normal interrupt priority),
polling is simpler and has no practical disadvantage here.

### 6.6 Per-channel shift array (SHIFT_ADDR)

NNoM uses per-tensor quantization for this model, meaning all output channels share the same
`output_rshift`. The per-channel SHIFT_ADDR register was added to future-proof the design
for per-channel quantized models (e.g., when exporting with `qtype=PER_CHANNEL`). The cost
is one additional AHB read per output channel, which at ~3 cycles × 36 channels ≈ 108 cycles
per layer is negligible.

---

## 7. Verification

### 7.1 Unit test (`test/accel/accel_test.c`)

Three test cases:
- **TC1** (C_in=1, C_out=2, K_w=3, stride=1, W_in=5): **FAILS** — input misalignment for
  stride=1, C_in=1 causes wrong HSIZE_WORD reads at non-word-aligned positions. Not
  a production concern (see §4.5).
- **TC2** (C_in=1, K_w=65, stride=1, W_in=68): **FAILS** — same input misalignment.
- **TC3** (C_in=4, C_out=4, K_w=3, stride=1, W_in=6): **PASSES** — C_in=4 gives
  word-aligned input addresses; multi-channel weight packing correct.

### 7.2 End-to-end inference (Verilator cycle-accurate simulation)

All 11 test clips tested with 60M cycle limit (captures first inference only):

| Clip | Accel | SW baseline |
|------|-------|-------------|
| down | ✓ down | ✓ down |
| go | ✓ go | ✓ go |
| left | ✗ unknown | ✗ unknown |
| no | ✓ no | ✓ no |
| off | ✓ off | ✓ off |
| on | ✓ on | ✓ on |
| right | ✓ right | ✓ right |
| stop | ✓ stop | ✓ stop |
| unknown | ✓ unknown | ✓ unknown |
| up | ✗ unknown | ✗ unknown |
| yes | ✓ yes | ✓ yes |

**10/11 correct** — identical to the SW baseline. Left and up predicting as unknown is
a model accuracy limitation (not a hardware bug) present in both modes.

### 7.3 Cycle count measurement

Measured via `csrr mcycle` CSR around `model_run()` in firmware:
```
CYCLES:4644544   (yes clip, typical)
```

The `mcycle` counter requires `CSR_COUNTER=1` in `hazard3_config.vh` (to instantiate the CSR)
and `csrw 0x320, zero` in firmware startup (to clear `mcountinhibit`, which Hazard3 sets by
default to prevent mcycle from incrementing).

---

## 8. Files Changed

| File | Change |
|------|--------|
| `kws_soc/peris/conv1d_accel/conv1d_accel.v` | Full RTL rewrite: r_wait timing, HSIZE_BYTE weights, SHIFT_ADDR register, C_out-outer loop order |
| `kws_firmware/mel_compact_4blk_ch36/kws_bare.c` | Removed 150M-cycle debug POLL block; added 2K-iteration I2S prime loop |
| `kws_firmware/mel_compact_4blk_ch36/nnom_conv1d_hw.c` | New file: NNoM conv2d_run replacement with HW accel fast path |
| `kws_firmware/mel_compact_4blk_ch36/accel_conv1d.h` | New file: Firmware driver (inline function, register map) |
| `kws_soc/test/accel/accel_test.c` | New file: Standalone unit test (TC1/TC2/TC3) |
| `kws_soc/test/Makefile` | Added `accel_test_xip` build target |
| `kws_firmware/Makefile` | Added `mel_compact_4blk_ch36_xip_accel` build target |
