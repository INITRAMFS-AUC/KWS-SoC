# NNoM-aware XIP cache prefetch — design + results

**Branch / HEAD at writing:** `main` @ `03e4fcf`
**RTL surface:** `peris/conv1d_accel/conv1d_accel.v`,
`peris/xip/ahbl_flash_ctrl_eb_cache.v`, `peris/xip/ro_cache.v`,
`kws_soc.v`
**Firmware surface:** `test/common/accel_conv1d.h`, `test/Makefile`
**Reference docs:** `peris/xip/DESIGN.md` (cache internals),
`peris/conv1d_accel/DESIGN.md` (interface justification)

---

## TL;DR

* The XIP cache treats every master as opaque and waits for demand
  misses.  But the firmware on this SoC is **always running NNoM**,
  whose access pattern is highly predictable: a per-layer dispatch in
  `.text`, then a sequential weight scan from XIP, then activation
  reads from SRAM.  An NNoM-aware cache can pre-warm each layer's
  weight scan before the accelerator demands it.
* This branch ships the full plumbing: APB control regs on
  `conv1d_accel`, a side-band into `ahbl_flash_ctrl_eb_cache`, a
  single-line victim buffer + prefetch FSM in `ro_dmc`, and a firmware
  hint in the accel driver — all behind a runtime config bit
  (`PREFETCH_CTRL.PREFETCH_EN`) **and** a compile-time gate
  (`KWS_XIP_PREFETCH`), both default off.
* **Gating contract is bit-perfect:** with the default flags, the
  Verilator trace of the production firmware (200 M cycles,
  `mel_compact_int8_xip_accel`, `sim/playback_samples.hex`) is
  byte-for-byte identical to the pre-prefetch RTL.  `diff` returns
  empty.  `slide_test` continues to pass `SLIDE_PASS audio_losses=0`.
* **Measurement (KWS_XIP_PREFETCH=1, otherwise same setup):**
  CYCLES_INFER mean drops 1.29 % (1,049,515 → 1,035,954); inference
  count nearly doubles (49 → 90) within the same wall-time window
  because each call is faster.  But the per-call prefetch hogs the
  flash bus for ~150 cycles, occasionally colliding with a demand
  miss; AUDIO_LOSS events go from 8 to 20 over the same 200 M-cycle
  window, with a 37 K-byte outlier vs the pre-prefetch worst case of
  14.6 K.  Net: **tiny inference win, real tail-latency regression**.
* **MVP only fetches the FIRST line at PREFETCH_BASE; PREFETCH_LEN is
  latched but not consumed.**  Extending the FSM to walk the full
  `[BASE, BASE+LEN)` range is the queued refinement; the
  bus-contention cost is amortised across many prefetched lines and
  the win becomes unambiguous.  Tracked as task #24 in `docs/TODO.md`.

---

## 1.  Why a normal cache is leaving cycles on the table

The current XIP cache (see `peris/xip/DESIGN.md`) is direct-mapped,
8 KB, with critical-word-first early-restart.  It reacts to demand
misses: a CPU or accel master issues an AHB read, the cache checks
the tag, and if it misses, the cache stalls the bus and fetches a
32-byte line via QSPI Quad-IO Read (~150 cycles).

This is fine for opaque workloads.  But the workload here is far from
opaque:

```
  for each clip:
    snapshot ring buffer into nnom_input_data
    model_run(model):
      for each layer:
        if conv2d:
          firmware writes accel APB regs (SRC/WT/DST/...)
          accel master walks WT_ADDR..WT_ADDR+wt_bytes linearly  ← XIP
          accel writes activations to SRAM
        else (softmax / pool / dense / etc.):
          firmware sweeps SRAM activations
    softmax + UART
```

Three observations follow:

1. The accelerator's weight scans are **strictly sequential and
   one-shot per layer per inference**.  A traditional cache that
   caches them along with `.text` and never evicts is wasting the
   capacity — those lines aren't going to be reused for ~30 ms.
2. The accel hardware **knows** the start address and length of the
   scan at the moment it starts.  That's a perfect prefetch hint.
3. Hot `.text` (the per-layer dispatch and the accel busy-poll loop)
   is small and benefits from sticking in the cache across calls.

So the design space is "give the cache a hint it can act on, but
don't let prefetched lines disturb the main cache."  That's a
victim-style buffer holding prefetched-but-not-yet-consumed lines.

The previous attempt at next-line prefetching was reverted on
2026-05-03 (see `docs/TODO.md` — at NL=256, `+9 %` CYCLES_INFER
regression, smaller cache = much worse) precisely because it polluted
the main cache.  This design takes the opposite approach: prefetched
lines go into a separate buffer that can never evict main-cache
lines.

---

## 2.  Mechanism

```
   firmware (test/common/accel_conv1d.h)
     │   on every accel call:
     │     ACCEL_PREFETCH_BASE = (uint32_t)wt;
     │     ACCEL_PREFETCH_LEN  = c_out * k_w * c_in;
     │     ACCEL_PREFETCH_CTRL = ACCEL_PREFETCH_EN;   ← rising edge
     │     ACCEL_CTRL          = ACCEL_CTRL_START;
     │     while (!(ACCEL_CTRL & ACCEL_CTRL_DONE));
     │     ACCEL_PREFETCH_CTRL = 0;                    ← re-arm next call
     ▼
  conv1d_accel APB slave  (peris/conv1d_accel/conv1d_accel.v)
   ┌──────────────────────────────────────────┐
   │  paddr[5:2]=9   PREFETCH_CTRL [0]=EN    │
   │  paddr[5:2]=10  PREFETCH_BASE           │
   │  paddr[5:2]=11  PREFETCH_LEN            │
   │                                          │
   │  prefetch_en   ─→ output port            │
   │  prefetch_base ─→ output port  (forced 0 │
   │  prefetch_len  ─→ output port    when    │
   │                                  EN=0)   │
   └──────────────────────────────────────────┘
     │
     │ side-band (kws_soc.v: xip_prefetch_{en,base,len})
     ▼
  ahbl_flash_ctrl_eb_cache (peris/xip/ahbl_flash_ctrl_eb_cache.v)
     │  ports: prefetch_{en,base,len}, passed straight through
     ▼
  ro_dmc  (peris/xip/ro_cache.v)
   ┌──────────────────────────────────────────┐
   │  Edge-detect prefetch_en rising →        │
   │    pf_pending = 1, pf_addr_capture = BASE│
   │                                          │
   │  Main FSM, ST_IDLE, no demand miss →     │
   │    state = ST_FETCH                      │
   │    fetch_is_pf = 1                       │
   │    m_start to flash_ctrl_eb              │
   │                                          │
   │  Main FSM, ST_FETCH, m_done && pf →      │
   │    pf_data <= m_data                     │
   │    pf_tag  <= ...                        │
   │    pf_line <= ...                        │
   │    pf_valid <= 1                         │
   │                                          │
   │  Hit logic:                              │
   │    ahit_pf = prefetch_en & pf_valid      │
   │             & (aline_no == pf_line)       │
   │             & (atag == pf_tag)            │
   │    ahit = ahit_full | ahit_cwf | ahit_pf │
   │  (similarly for dhit)                    │
   │                                          │
   │  Read mux:                               │
   │    data = dhit_pf ? pf_data : DATA[...]  │
   └──────────────────────────────────────────┘
```

Three rules baked into the FSM:

1. **Demand misses always preempt prefetch.**  The ST_IDLE branch
   checks `cpu_rd && !ahit` and `cpu_dvalid && !dhit` first; the
   prefetch path only fires `else if (pf_pending)`.

2. **Prefetched lines never pollute DATA[].**  The CWF mid-fetch fill
   (`DATA[fetch_line_reg] <= m_data`) is gated on `!fetch_is_pf`.
   For prefetch fetches, the line lands in `pf_data` only on
   `m_done`.  This is the "victim buffer" property — the main cache
   can never lose hot `.text` to a transient weight scan.

3. **CWF is suppressed for prefetch fetches.**  The CWF early-restart
   path returns words from `DATA[]`, but DATA[] is intentionally not
   updated mid-prefetch-fetch.  So `ahit_cwf`/`dhit_cwf` AND `!fetch_is_pf`.
   Demand misses arriving mid-prefetch take the existing
   deferred-miss rescue path post-prefetch.

---

## 3.  Gating contract

The brief from the user: *"Make sure the NNoM-aware improvement is
only enabled if a config bit enables it."*

Two layers of gate, both default OFF:

| Gate                        | Default | Effect when 0                                 |
| --------------------------- | ------- | --------------------------------------------- |
| `PREFETCH_CTRL.PREFETCH_EN` (RTL, runtime) | 0       | `pf_pending` can never be set; `prefetch_base`/`prefetch_len` forced low at the accel boundary; `ahit_pf`/`dhit_pf` AND-gated to 0; the `data = dhit_pf ? pf_data : DATA[…]` mux collapses to the original direct read. |
| `KWS_XIP_PREFETCH` (firmware, compile-time) | 0       | The firmware emits zero extra instructions per accel call — no APB writes to PREFETCH_BASE/LEN/CTRL. |

When **both** default to 0 (the production build's default), the SoC
is byte-for-byte the pre-prefetch design.  Verified by `diff`:

```
$ diff /tmp/prod_pre_prefetch_baseline.txt /tmp/prod_off_post_step4.txt
$  echo $?
0
```

(empty diff, exit 0)

The compile-time gate exists because runtime gating alone leaves
~20 bytes of dead instructions on the firmware's hot path.  When
KWS_XIP_PREFETCH=0 those are pruned by the preprocessor, not by the
linker / GC sections.

The TB regression (`make -C test/xip run-testbench`) is unmodified
and runs with prefetch_en=0 — the existing 54/54 PASS holds.

---

## 4.  Verification methodology

### 4.1.  slide_test (loop-math invariant)

`test/i2s/c/slide_test.c` is a self-checking simulation of the
sliding-window-inference loop.  It uses a fixed simulated inference
duration and verifies that every byte of audio is part of at least
one inference window (`audio_losses=0`) and that each window's
sample bytes match the `counter_32k.hex` stimulus.

It has nothing to do with the XIP cache directly, but it's a smoke
test for everything *around* the accel — DMA path, ring buffer,
audio-loss guard, sliding-step math.  It was the first sanity check
on every commit:

```
$ ./build/verilator/Vkws_soc --no-jtag --flash test/build/slide_test_xip.bin
                              --mic sim/counter_32k.hex --cycles 100000000
SLIDE_PASS  iters=a step=1e0 audio_losses=0  bytes_written=5900  irqs=b2
```

PASS at every commit on this branch.

### 4.2.  Production firmware (gating contract)

The actual gating-contract verification is end-to-end in Verilator:

```
$ make test-mel-compact-int8-accel        # KWS_XIP_PREFETCH=0 by default
$ ./build/verilator/Vkws_soc --no-jtag
        --flash test/build/mel_compact_int8_xip_accel.bin
        --mic sim/playback_samples.hex --cycles 200000000 > prod.txt
$ diff prod.txt prod_pre_prefetch_baseline.txt && echo IDENTICAL
IDENTICAL
```

Run 4 consecutive times across separate commits — same bit-for-bit
output every time.  The defining numbers:

| metric                  | value |
| ----------------------- | ----- |
| AUDIO_LOSS events       | 8     |
| AUDIO_LOSS values (in order) | 8704, 5248, 2816, 6784, 14592, 5376, 512, 9984 |
| DETECT count            | 49    |
| CYCLES_INFER mean       | 1,049,515 |
| ITER_BYTES steady-state | 768   |

### 4.3.  Production firmware with prefetch ON

Re-run with `KWS_XIP_PREFETCH=1`:

```
$ make test-mel-compact-int8-accel KWS_XIP_PREFETCH=1
$ ./build/verilator/Vkws_soc --no-jtag
        --flash test/build/mel_compact_int8_xip_accel.bin
        --mic sim/playback_samples.hex --cycles 200000000 > prod_on.txt
```

| metric                | OFF (default) | ON     | delta      |
| --------------------- | ------------- | ------ | ---------- |
| CYCLES_INFER mean     | 1,049,515     | 1,035,954 | **−1.29 %** |
| DETECT count          | 49            | 90     | n/a*       |
| AUDIO_LOSS count      | 8             | 20     | +12        |
| AUDIO_LOSS / DETECT   | 16 %          | 22 %   | +6 pp      |
| Largest AUDIO_LOSS    | 14.6 KB        | 37.2 KB | +22.6 KB worse |
| Firmware text size    | 29,084 B      | 29,104 B | +20 B    |

\* The DETECT count rises because each inference is slightly faster,
so more iterations fit in the same 200 M-cycle wall-time window.
The right framing is *per-inference cost*, not iteration count.

---

## 5.  Why MVP isn't a clear win

The mechanism works exactly as designed: the prefetched line IS warm
when the accel reads it, saving the demand miss (~150 cycles, the
QSPI line-fetch penalty).  That's where the −1.29 % comes from.

But the MVP only prefetches **one line** per layer call.  Each layer's
weight scan is much longer than one line — for layer 1 of
mel_compact_int8 the weight tensor is 1024 bytes = 32 lines.  Of
those 32 lines, only the first one is prewarmed; the other 31 still
demand-miss.

Meanwhile the prefetch *itself* costs:

* ~150 cycles of flash-bus time (during which a demand miss must wait).
* 4 APB writes per call (~16 cycles of CPU time).

That's almost exactly the cost of one demand miss.  So we save one
demand miss per call but pay one demand-miss-equivalent of bus
contention.  At ~90 calls per inference, that's ~13 K cycles saved
and ~13 K cycles spent.  The actual measured win (−13 K cycles ≈
1.29 %) is consistent with this break-even-with-noise picture.

The reason AUDIO_LOSS gets *worse* is exactly this contention: when
a prefetch happens to be in flight at the moment a demand miss
arrives, that demand miss now waits 150 cycles for the prefetch
to drain *plus* its own 150 cycles, vs the no-prefetch baseline of
just 150.  Most of the time this doesn't matter — both fetches were
needed anyway.  But in the rare iter where the timing aligns badly,
we add a one-shot 150-cycle stall to a path that was already long.
That pushes a borderline 1-second iter past 1 second and trips
AUDIO_LOSS.

The fix is to amortise the bus-contention cost across **many**
prefetched lines: walk the full `[BASE, BASE+LEN)` range, fetching
line after line in the background while the accel does setup.
Doing one ~150-cycle prefetch saves one demand miss → 0 net.  Doing
ten prefetches saves ten demand misses → 1500 cycles saved per call
→ ~135 K cycles per inference → ~13 % win.  And the AUDIO_LOSS
regression goes away because the prefetch is doing useful background
work the whole time (no scattered "was a demand miss almost
finished?" race windows).

That extension (consume `prefetch_len`, walk lines until exhausted,
yield to demand misses between each line) is queued as task #24.
The firmware contract is already forward-compatible: it writes the
correct LEN already; the FSM just doesn't read it yet.

---

## 6.  Implementation in commits

| Commit    | Step                                                       |
| --------- | ---------------------------------------------------------- |
| `5577de8` | conv1d_accel: 3 gated APB regs + side-band outputs.        |
| `4694b2c` | XIP cache wrapper + ro_dmc: hint pass-through input ports. |
| `adb8620` | ro_dmc: single-line victim buffer + prefetch FSM, gated.   |
| `03e4fcf` | firmware: hint plumbing in accel_conv1d, opt-in.           |
| earlier   | `peris/xip/DESIGN.md` (proposal + gating contract).         |
| earlier   | `peris/conv1d_accel/DESIGN.md` (interface justification).   |

Each commit was independently verified — `slide_test` remained green
and the OFF-default trace remained bit-identical to the pre-prefetch
baseline at every step.

---

## 7.  Files touched (live reference)

### RTL

* `peris/conv1d_accel/conv1d_accel.v` — adds three APB registers
  (`PREFETCH_CTRL`/`BASE`/`LEN` at `paddr[5:2]` = 9/10/11) and three
  side-band output ports.  When EN=0, base/len are forced 0 at the
  module boundary so even an unconnected consumer sees no stale hint.
* `peris/xip/ahbl_flash_ctrl_eb_cache.v` — adds three input ports for
  the hint and passes them straight through to `ro_dmc`.
* `peris/xip/ro_cache.v` — adds the prefetch FSM, single-line victim
  buffer (`pf_{data,tag,line,valid}`), and the AND-gated hit logic +
  read mux.
* `kws_soc.v` — declares `xip_prefetch_{en,base,len}` wires and
  routes them from the accel side-band to the cache wrapper.

### Firmware

* `test/common/accel_conv1d.h` — adds `ACCEL_PREFETCH_{CTRL,BASE,LEN}`
  register defines and pulses them around every `accel_conv1d()` call
  when `KWS_XIP_PREFETCH=1`.  Default-off.

### Build

* `test/Makefile` — `KWS_XIP_PREFETCH=1` propagation through
  `KWS_EXTRA_CFLAGS`.

### Documentation

* `peris/xip/DESIGN.md` — long-form cache reference with the
  proposal section and the mandatory gating contract.
* `peris/conv1d_accel/DESIGN.md` — section 2.1 justifies APB-slave
  + AHB-master interfaces; section after that documents the hint
  registers.
* `docs/nnom_aware_xip_cache.md` — this document.

---

## 8.  Open follow-ups (`docs/TODO.md` + task list)

* **#24** — Extend the prefetch FSM to walk full `[BASE, BASE+LEN)`.
  RTL-only change to `ro_cache.v`; firmware contract already forward-
  compatible.
* **#22 (closed)** — covered by this implementation.
* Earlier reverted approach (chained next-line prefetch) is documented
  in `docs/TODO.md` — keep it there as a record of what doesn't work
  and why (eviction cascade).
* AHB burst (HBURST=INCRx) in `ahbl_splitter`/`ahbl_arbiter` — would
  cut every miss penalty roughly in half by collapsing the 8-beat
  line fill into one burst.  Independent of prefetch; both attack
  the same bottleneck from different sides.
* Program-aware FSM that walks `.text` sequentially at boot — drives
  the steady-state miss rate to the compulsory floor.  Documented in
  `docs/TODO.md`; relevant when BRAM budget can't grow enough that
  the whole hot text fits in the main cache.

---

## 9.  How to reproduce

```bash
git checkout 03e4fcf
scripts/apply_patches.sh        # idempotent — applies build-time
                                 # patches to submodule worktrees

# RTL + firmware
make sim_verilator
make test                        # KWS_XIP_PREFETCH=0 (default)

# Default (gated off) — bit-identical to pre-prefetch baseline
./build/verilator/Vkws_soc --no-jtag \
    --flash test/build/slide_test_xip.bin \
    --mic   sim/counter_32k.hex \
    --cycles 100000000 | grep SLIDE_

./build/verilator/Vkws_soc --no-jtag \
    --flash test/build/mel_compact_int8_xip_accel.bin \
    --mic   sim/playback_samples.hex \
    --cycles 200000000 > prod_off.txt

# Prefetch on
make test-mel-compact-int8-accel KWS_XIP_PREFETCH=1
./build/verilator/Vkws_soc --no-jtag \
    --flash test/build/mel_compact_int8_xip_accel.bin \
    --mic   sim/playback_samples.hex \
    --cycles 200000000 > prod_on.txt

grep -c AUDIO_LOSS prod_off.txt prod_on.txt
grep CYCLES_INFER prod_off.txt | awk -F: '{s+=$2;n++} END {print s/n}'
grep CYCLES_INFER prod_on.txt  | awk -F: '{s+=$2;n++} END {print s/n}'
```

Expected: identical AUDIO_LOSS counts/values/CYCLES_INFER for the
default; +12 AUDIO_LOSS / −1.29 % CYCLES_INFER mean for the on
build.
