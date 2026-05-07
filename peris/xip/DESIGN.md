# XIP cache (`ro_dmc` + `ahbl_flash_ctrl_eb_cache` + `flash_ctrl_eb`)

The XIP (eXecute-In-Place) cache lives between the AHB bus fabric and the
QSPI flash controller.  All firmware lives in flash; without this cache,
every CPU instruction fetch and every accel weight read would pay the
full QSPI miss penalty (~150 cycles per 32-byte line at 36 MHz CLK,
flash running 4-bit IO @ 1 SCK = 8 bits, command 0xEB Quad-IO Read).

This document is the long-form reference for what the cache is, what
optimizations it currently carries, and where the open knobs are.  It is
maintained alongside `peris/xip/ro_cache.v` (the cache RTL) and
`peris/xip/ahbl_flash_ctrl_eb_cache.v` (the AHB-protocol wrapper + miss
FSM).  Empirical numbers come from `mel_compact_4blk_ch36_xip` /
`mel_compact_int8_xip_accel` running under Verilator at 36 MHz CLK.

## Topology

```
  CPU i-port ─┐
  CPU d-port ─┤
  DMAC      ─┼─ ahbl_crossbar / arbiter ─→ xip slot @ 0x8000_0000
  conv1d_accel ─┘                           │
                                            ▼
                            ahbl_flash_ctrl_eb_cache
                                            │
                       ┌────────────────────┴───────────────────┐
                       │                                        │
                  ro_dmc (cache)                       flash_ctrl_eb (FSM)
                  └─────────── line transfer ──────────┘
                                            │
                                       QSPI flash (0xEB)
```

* **Master count**: 4 today (CPU i, CPU d, DMAC, conv1d_accel) + the
  optional XIP_PLAYBACK player as a 5th.  Every read access from any of
  them, in the address range `0x8000_0000–0x8FFF_FFFF`, lands in the XIP
  cache.
* **Slave**: a single AHBL slave port presented by the wrapper.

## Cache parameters (today)

| Parameter | Value          | Notes                                       |
| --------- | -------------- | ------------------------------------------- |
| `LW`      | `32 * 8` = 256 | Bits per cache line → **32-byte line**.     |
| `NL`      | 256            | Number of lines → **8 KB total cache**.     |
| Mapping   | direct-mapped  | One way; tag + valid per line.              |
| Address   | { tag, line, off } where `line[7:0]`, `off[4:0]`. |

Set in `kws_soc.v:783-786`:

```verilog
ahbl_flash_ctrl_eb_cache #(
    .LW(32*8),
    .NL(256)
) xip ( ... );
```

## Active optimizations

Every optimization listed here is **currently compiled in**.  Any
remaining alternatives discussed below are deliberately turned off (and
why).

### 1. Direct-mapped tag/valid cache (baseline)

* Simple, single-cycle hit detection: `ahit = (TAG[line]==tag) & VALID[line]`.
* Tag width = `32 - LFW - OFW` = `32 - 8 - 5` = 19 bits per line.
* `VALID[]` is reset to 0 on `!rst_n` so post-reset reads always miss
  (compulsory).
* No write-back / dirty bits — cache is **read-only** (the firmware is
  in flash; the accel never writes flash).

### 2. Critical-Word-First (CWF) / early restart

`ro_cache.v:152-219`.  Default in every build (no compile-time gate).

The QSPI flash controller `flash_ctrl_eb` returns words within a 32-byte
line **as they arrive on SCK**, not all at once.  It exposes a
per-word ready bitmap (`m_word_done[LW/32-1:0]`) that lights bits
left-to-right as the line fills.

`ro_dmc` watches this bitmap and releases `HREADYOUT` to the CPU as soon
as the **requested word** lands, instead of waiting for the full line.
The remaining words continue filling in the background.  See `dhit_cwf`
and `ahit_cwf` plus the `fwv` (fetch-word-valid) latch.

* Measured: **−1.23% CYCLES_INFER** on `mel_compact_4blk_ch36_xip`
  (45.84M → 45.28M, commit `ed8732d`).
* Modest because most i-fetches target word 0 of the line (sequential
  code), where CWF saves nothing.  The CWF self-checking TB sweeps all
  8 word offsets and shows **−34 % stall** for non-zero offsets.

### 3. Deferred-miss rescue (CWF correctness fix)

`ro_cache.v:112-131`.  Without this, CWF deadlocks the bus.

When CWF early-restarts the master mid-fetch, the master may issue a
**fresh address phase** (e.g. the next instruction the CPU prefetches)
that lands on a line we haven't fetched yet.  HREADY drops, the new
`cpu_rd` pulse never fires (HREADY=0 gates it), and the FSM is in
ST_FETCH so it can't see the new request via the normal path.

The rescue: at the end of every fetch, if `cpu_dvalid && !dhit` (a
parked data phase that doesn't have its line), kick off a fresh fetch
using `cpu_daddr` instead of waiting for a `cpu_rd` that will never
come.

Required for CWF + AHB master correctness; the testbench TB10 case
explicitly stresses this.

### 4. Stale `m_word_done` filtering (CWF correctness fix)

`ro_cache.v:170-195`.

`flash_ctrl_eb`'s `m_word_done` is **level**, not edge — bits stay high
after each word lands and only clear on the next `start`.  But `start`
takes ~3 cycles to propagate from `ro_dmc` through the wrapper FSM and
back into `flash_ctrl_eb`, so the first few cycles of a fresh ST_FETCH
still see the **previous** fetch's bits.

If we trusted `m_word_done` directly, the second miss would publish
**stale** data through the CWF hit path before the new fetch even
started.  TB10's stale-bit case catches this.

Solution: latch our own `fwv[LW/32-1:0]`:
* Reset on miss-entry (both the normal and deferred-miss branches).
* OR-accumulate only the **rising edges** of `m_word_done`.

When `fwv` is fresh, every set bit corresponds to a real word arrival of
the *current* fetch, so `dhit_cwf` is safe.

### 5. Quad-IO Read (`flash_ctrl_eb` mode 0xEB)

Outside `ro_dmc` itself but on the same critical path.  `flash_ctrl_eb`
issues the **0xEB** command (Quad I/O Read), which sends 4 bits per SCK
edge across the QSPI data lines.  4× the data rate of single-line SPI.

* SCK is held at CLK/2 (~18 MHz at 36 MHz CLK).
* 32-byte line = 256 bits ÷ 8 bits/SCK = 32 SCK cycles, plus ~10 cycles
  of address/dummy phases.  At CLK/2 SCK, ~84 CLK cycles per miss in
  steady state — close to the ~155 cyc miss penalty observed (the
  delta is start/stop overhead and ST_WAIT_LOW/HIGH FSM transitions in
  the wrapper).

### 6. Per-word ready bitmap published mid-fetch

`flash_ctrl_eb:18`.  `word_done[K]` is set when **byte 4K+3** (the last
byte of word K) lands in `D` and stays high until the next `start`.
This is the level-bitmap that `ro_dmc` filters in optimization #4.
Adds `LW/32` flops to `flash_ctrl_eb` but is essentially free —
required to make CWF possible at all.

## Tried and rejected

### Next-line prefetch (chained) — **REVERTED 2026-05-03**

`docs/TODO.md` carries the post-mortem.  Implementation was a chained-
prefetch in `ro_dmc`: on every demand miss, also prefetch line+1
opportunistically.  TB-validated with 54/54 PASS, prefetched lines hit
at 0 stall.

Sweeping `(NL, prefetch)` over `NL ∈ {32, 64, 128, 256}` on
`mel_compact_4blk_ch36`, steady-state CYCLES_INFER:

```
  NL= 32   off ~99.9M    on  >300M  (no infer in 300M cyc)
  NL= 64   off ~77.9M    on ~113.7M (+46 %)
  NL=128   off ~52.7M    on  ~63.3M (+20 %)
  NL=256   off ~46.3M    on  ~50.7M  (+9 %)
```

* **Smaller cache, worse prefetch.**  Prefetch evicts a hot line on
  every miss; the next miss on the evicted line triggers another
  prefetch that evicts more hot lines — a self-feeding eviction
  cascade that scales inversely with cache capacity.
* The ~3.5 % spatial-locality win never pays for the bus contention +
  premature evictions.

Reverted.  Don't re-try without one of:

(a) A **victim-buffer-style** prefetch that doesn't disturb the main
    cache (so prefetches can never evict anything).
(b) **Abort-able** in-flight QSPI fetches, so a demand miss can pre-empt
    a wasted prefetch.
(c) **Confidence prediction** — only prefetch when the last K accesses
    were sequential (i.e. NOT during accel weight scans, which look
    sequential but are one-shot per layer).

## Open follow-ups (in `docs/TODO.md`)

These are *not* implemented yet but are documented sketches:

### A. AHB burst support (HBURST=INCRx)

`busfabric/ahbl_splitter.v:26`, `busfabric/ahbl_arbiter.v:29`.

Today every line fetch is 8 single-beat AHB transactions.  HBURST=INCRx
would let the cache issue **one** burst transaction for the whole 32-byte
line.  Estimated win: miss penalty ~155 cyc → ~50 cyc, plus less bus
arbitration churn.  Pairs cleanly with cache improvements.

### B. Program-aware FSM prefetch

`peris/xip/ro_cache.v:1-3` (TODO comment), `docs/TODO.md`.

The firmware code is **fixed** at flash-program time.  The cache doesn't
need to be a generic adaptive structure — a small FSM that knows the
program (or just walks `.text` sequentially at boot to warm the cache)
can drive the miss rate to the compulsory floor.

* At `NL=1024` (32 KB cache, fits all hot text) we already measure
  **0 % miss rate / 41M CYCLES_INFER**.  So this optimization is
  effectively "right-size the cache to the program" — relevant when
  BRAM budget can't grow that far on ASIC.

### C. Software cache invalidation hook

`docs/TODO.md` P2.  Latent until the firmware is updated in-place
(OTA).  Requires an APB register that bulk-clears `VALID[]`.

## Proposed: NNoM-aware cache (in-flight, this branch)

The current cache treats every master as opaque and reacts to demand
misses.  But the firmware on this SoC is **always running NNoM**, and
NNoM has a very predictable access pattern:

```
  1. CPU runs the per-layer dispatch loop in .text (small, hot, repeat).
  2. For each Conv2D layer:
       a. CPU writes the accel APB regs (~6 stores).
       b. accel master scans WT_ADDR..WT_ADDR+wt_bytes linearly (XIP).
       c. CPU spins in the busy-poll loop in .text (small, hot, repeat).
       d. accel writes activations to SRAM.
  3. For each non-Conv2D layer (softmax, pool, etc.):
       a. CPU runs nnom_local_*.c routines from .text.
       b. CPU sweeps activations from SRAM (no XIP cost).
       c. CPU reads/writes weights from XIP if present (rare).
```

Implications:

* **`.text` is reused across iterations**; pinning hot inference code
  (or just ensuring it never gets evicted by transient weight scans)
  removes most steady-state instruction misses.
* **Weights are read sequentially, ONCE per layer per inference**.
  Spatial locality is perfect *within* a layer scan but the line is
  consumed once and then untouched until the next inference, ~30 ms
  later.  A traditional cache wastes capacity caching used-once lines.
* **Activations are in SRAM**, not XIP — out of scope for this cache.

Two natural design points fall out:

**(1) Hint-driven sequential prefetch ("NNoM weight prefetcher").**
APB-writable `(base, length)` tells the cache "I'm about to scan
contiguously".  The cache prefetches those lines into a **separate
small region** (so it can't evict `.text`), and demand misses on the
prefetched lines cost zero.  Firmware (or, ideally, the conv1d_accel
hardware itself) writes the hint as part of the accel start sequence.

* Pros: matches NNoM's actual access pattern exactly.  Doesn't need
  associativity changes.
* Cons: adds a dedicated prefetch buffer (4-8 lines = 128-256 B); needs
  hint plumbing through the accel.

**(2) 2-way set-associative + LRU.**
Doubles effective capacity for the conflict-miss case (e.g. inference
loop and softmax LUT both hashing to the same line).  With 8 KB total
and direct mapping today, two .text lines that happen to share line
index collide on every loop iteration.

* Pros: generic; no firmware/RTL changes elsewhere.
* Cons: tag+LRU storage doubles; hit-detection mux adds a level; no
  silver bullet against the *sequential-then-discarded* weight scan
  pattern (still pollutes both ways).

**Recommendation: do (1) first.**  It's tightly coupled to the actual
program and addresses the root cause (transient weight scans evicting
hot text).  (2) is a generic improvement we can stack on top later.

### Gating: `XIP_PREFETCH_EN` config bit (mandatory)

The NNoM-aware logic is **always opt-in**.  Default behaviour is the
current cache exactly — no extra storage paths, no hint side-effects,
no observable change for any non-NNoM workload (XIP playback,
flash_read_test, the standalone XIP testbench).  Flip enabled via:

* APB-writable bit `XIP_CACHE_CTRL.PREFETCH_EN` (bit 0 of a new
  control register exposed on the cache wrapper's APB face), **or**
* Compile-time gate `\`define XIP_PREFETCH_EN_DEFAULT_1` for boards
  where the bit should be on at reset.

When the bit is 0:

* Hint writes (`PREFETCH_BASE`, `PREFETCH_LEN`) are accepted but
  produce no fetches.
* The prefetch buffer is held in reset (no flops toggle).
* The hit-detection path is exactly today's `ahit_full | ahit_cwf`
  (no extra mux level).

This is a mandatory requirement — the new behaviour must be
behaviourally invisible when the bit is clear, and must keep the
existing 54/54-PASS XIP-cache TB green untouched.

### Implementation steps (in this branch)

1. Add a victim-style **weight-prefetch buffer** (parameterized depth)
   inside `ro_dmc` that holds prefetched weight lines separately from
   the main cache.  Held in reset when `XIP_CACHE_CTRL.PREFETCH_EN` = 0.
2. Add the APB control surface: `XIP_CACHE_CTRL` (bit 0 = enable),
   `PREFETCH_BASE`, `PREFETCH_LEN`.  Co-located with the cache wrapper.
3. Wire `conv1d_accel` so the hint fires automatically when
   `ACCEL_CTRL_START` is written — but only when the cache enable bit
   is set; otherwise the accel writes nothing.
4. Re-run the existing XIP-cache TB suite unmodified; confirm 54/54
   still passes with `XIP_CACHE_CTRL.PREFETCH_EN` = 0 (the default).
5. Sweep CYCLES_INFER + miss-count delta vs the current cache with
   the bit set, and record in this doc.
