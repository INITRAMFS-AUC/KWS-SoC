# KWS-SoC — Open work, ranked by importance

Priority ladder:
- **P0** — correctness / data integrity.
- **P1** — perf / power wins (≥10% inference cycles, ≥1 order of
  magnitude on idle-state energy).
- **P2** — cleanup that prevents future bugs (single source of truth,
  fragility removed).
- **P3** — developer velocity.

## P0 — correctness

- **`-fno-lto` hangs `nnom_model_create`**.  LTO build runs end-to-end;
  `-fno-lto` hangs after the boot banner.  Lost diagnostic GDB
  visibility; real bug somewhere in NNoM static-memory init that LTO
  is masking.

## Deferred (waiting on external dep)

- **ring_buf overflow / lost audio between clips**.  Trap handler
  silently drops samples once `ring_pos == SAMPLES_PER_CLIP`; today
  inference takes ~3× longer than one capture window so we lose ~3
  clips of audio between every processed clip.  **Deferred while the
  team's Conv1D accelerator lands** — once inference fits inside one
  capture window, the gap closes naturally and most of this becomes
  moot.  Re-evaluate after accelerator merge; if inference still
  > capture, do double-buffering (ring = 2× clip).

## P1 — perf / power

- **Profile why Zba+Zbb regressed mel_compact 9.5%.**  Flipping
  EXTENSION_ZBA / EXTENSION_ZBB on in hazard3_config.vh (and
  rebuilding both Verilator and the firmware via the new RV_ARCH
  pipeline) shrank the text section 220 bytes and produced ~30
  bitmanip instructions in the binary, but CYCLES_INFER went from
  105.0M to 115.7M (reproducible across both sim clips, DETECT
  preserved).  Hazard3's barrel shifter + ALU result mux look
  single-cycle on paper, so the most likely culprit is gcc's
  instruction scheduling with the bitmanip path producing hot-loop
  sequences that miss Hazard3's forwarding bypass.  Plan: disasm a
  hot conv kernel in both builds, diff the schedules, and try
  Zba-only vs Zbb-only to isolate.  If the cause is gcc-side, the
  workaround might be `-fno-tree-vectorize` or layer-specific
  pragmas; if it's Hazard3-side, file an upstream issue.
- **HW skip-R-slot SCK in I2S MONO_MODE** (peris/i2s Phase B).
  Today `apb_i2s_receiver` toggles SCK during the R slot even though
  MONO_MODE discards R data.  Half of all SCK transitions burn power
  for nothing.  Patch the SCK generator to gate toggles on
  `ws_reg == 0` (L slot) when MONO_MODE && cfg_q8_en.  Needs a
  proper testbench (Phase C) — current sim has no I2S-protocol
  assertions.
- **Merge peris/i2s `nnom-quantize` branch (HW Q7 + downsample)**.
  Done.  Phase A (firmware uses Q8_EN, IRQ count 4× lower) and
  Option 1 (LSB-oldest pack + DMA-into-audio_ring + lean ISR +
  sliding-window-ready main loop) both landed; CYCLES_INFER
  109.86M → 105.0M and CYCLES_CAPTURE matches the audio rate
  exactly.  Phase B (HW skip-R-slot) and Phase C (testbench)
  tracked separately above.
- **Flip on sliding-window inference once the Conv1D accelerator
  lands.** Trigger: a `model_run` cycle count that fits comfortably
  inside the 20-40 ms / 720K-1.44M-cycle window we want to slide
  by.  Action, in order:
    1. Bump `KWS_RING_SAMPLES` from 8192 to 16384 in
       `test/common/kws_bare_main.c` (or pass `-DKWS_RING_SAMPLES=16384`
       from the model build) — gives the DMA 800+ samples of
       headroom past the active inference snapshot.
    2. Drop `KWS_STEP_SAMPLES` from `SAMPLES_PER_CLIP` (8000) to
       160 (20 ms) or 320 (40 ms).  Pick whichever still leaves
       slack between consecutive inferences in the new budget.
    3. Re-run `mel_compact_4blk_ch36` on `sim/down_0000.hex` and
       confirm: (a) DETECT is still `0,down` on the relevant
       window(s); (b) consecutive `CYCLES_INFER` values are
       within a few percent of each other; (c) `CYCLES_CAPTURE`
       between inferences is roughly `KWS_STEP_SAMPLES` audio
       periods (~20-40 ms ≈ 720K-1.44M cycles).
    4. Add a deliberate "word straddling clip boundary" sim
       fixture under `sim/` and verify the sliding window catches
       it where the old non-overlapping pipeline missed it.
  Until then, today's defaults (STEP=8000, RING=8192) reduce to
  back-to-back full-clip inferences with no overlap, which is the
  best you can do with a 4 s `model_run`.
- **Validate readI2s Q8 testbench end-to-end**.  Merged the NNOM
  branch's `test/i2s/c/readI2s.c` (compares HW-packed bytes against
  `sim/debug_audio.hex` via uart_printf).  Boot + banner work; the
  IRQ-driven check loop never reaches `done` in an 80M-cycle sim
  window.  Suspect: (a) `i2s_mic_sim` stops feeding after 234 samples
  so FIFO never gets back to half-full, or (b) `clk_div=4` paired with
  the sim's SCK-driven sample tick produces no FIFO writes.  Step
  through one IRQ in waveforms and pin down which.
- **Voice-activity detector before model_run**.  Today every clip
  pays ~108M cycles of inference whether anyone is talking or not.
  Real deployment is mostly silence — gate with even a free
  energy-threshold VAD and inference power drops 90 %+.  Hook in
  `kws_bare_main.c` between `memcpy` and `model_run`.
- **DMA drains valid I2S FIFO entries autonomously** (Plan B/C/D, not
  A).  Today the firmware burst is hardcoded and must match the I2S
  half-full threshold or every other 4-block in `ring_buf` is silently
  zero-padded.  I2S keeps interrupting at half-full; DMA reads
  whatever count is in the FIFO.  Three paths: (D) direct I2S→DMA
  wire, (C) level-triggered AHB drain, (B) DMA reads SIZE from I2S
  `fifo_count` register.
- **I2S becomes its own AHB master, autonomous drain to SRAM
  (long-term, Option 2).**  End-state for the audio capture path:
  `apb_i2s_receiver` grows an AHB-Lite master port and writes
  packed Q8 samples directly into a SRAM ring region with no DMA
  peripheral, no `dmac_irq`, no per-frame ISR.  The CPU only sees
  one IRQ per clip (when the ring head crosses the clip boundary).
  Eliminates 250 pipeline flushes during inference, frees the DMA
  for non-audio work, removes the I2S/DMA burst-size coupling
  altogether (TODO #13 disappears).  Bigger RTL change — third
  master on `ahbl_crossbar` (NUM_MASTERS=3), arbitration retest,
  and a dedicated FIFO-drain FSM in the I2S block — so deferred
  until the Conv1D accelerator work brings the inference budget
  down enough that the per-IRQ pipe-flush savings actually move
  the needle.  Lands naturally as part of that effort because it
  also wants its own master port.
- **Reduce DMA cost on FIFO-empty (zero) reads**.  Wasted AHB cycles
  + dynamic power when the burst overruns available samples.
  Subsumed by the autonomous-drain task; keep separate so a faster
  power-gate fix (gate `dst_hwdata` toggling on `fifo_empty`) doesn't
  block on the bigger redesign.
- **XIP cache: critical-word-first** is now always-on in `ro_dmc` —
  shipped in commit ed8732d for −1.23 % CYCLES_INFER (45.84M → 45.28M
  on mel_compact_4blk_ch36).  Modest because most i-fetches target
  word 0 (sequential code) where CWF saves nothing; the standalone
  TB sweep across all 8 word offsets shows −34 % stall.
- **XIP cache: next-line prefetch — TRIED, REGRESSES AT EVERY NL (2026-05-03)**.
  Implemented chained-prefetch in `ro_dmc` (TB-validated, 54/54 PASS,
  prefetched lines hit at 0 stall).  Swept (NL, prefetch) over
  NL ∈ {32, 64, 128, 256} on mel_compact_4blk_ch36.  Steady-state
  CYCLES_INFER:

      NL=32  off  ~99.9M    on  >300M (no infer in 300M cyc)
      NL=64  off  ~77.9M    on  ~113.7M  (+46 %)
      NL=128 off  ~52.7M    on  ~63.3M   (+20 %)
      NL=256 off  ~46.3M    on  ~50.7M   (+9 %)

  Counter-intuitive: the **smaller the cache, the WORSE prefetch is**.
  Bus-fetch counts went +25 % (NL=32) → +81 % (NL=256) and CYCLES_INFER
  regressed in every case; at NL=32 the CPU never finishes inference
  in 300M cycles.  Mechanism: prefetch evicts a hot line on every
  miss, the CPU's next miss on the evicted line triggers another
  prefetch that evicts more hot lines — a self-feeding eviction
  cascade that gets worse as the cache shrinks.  The ~3.5 % spatial-
  locality win never pays for the bus contention + premature evictions.
  Reverted.  Don't re-try without one of: (a) a victim-buffer-style
  prefetch that doesn't disturb the main cache (so prefetches can't
  evict anything), (b) abort-able in-flight QSPI fetches (so a
  demand miss can pre-empt a wasted prefetch), or (c) confidence
  prediction (only prefetch when the last K accesses were sequential).
- **XIP cache: program-aware FSM prefetch** (TODO at
  `peris/xip/ro_cache.v:2`, also raised in conversation 2026-05-02).
  The firmware code is fixed and known at flash-program time, so
  the cache doesn't need to be a generic adaptive structure — a
  small FSM that knows the call graph (or just walks flash
  sequentially at boot to warm the cache) can drive miss rate to
  the compulsory floor.  At NL=1024 (32 KB, fits all .text) we
  already measure 0 % miss rate and 41M CYCLES_INFER, so this is
  effectively "right-size the cache to the program" — the FSM
  variant matters when BRAM budget can't grow that far on ASIC.
  Pair with the cache invalidation P2 entry below if the firmware
  is ever updated in-place.
- **AHB burst support in `ahbl_splitter` and `ahbl_arbiter`** (TODOs
  at `busfabric/ahbl_splitter.v:26` and `busfabric/ahbl_arbiter.v:29`).
  Today both modules treat each beat as an independent transaction.
  HBURST=INCRx would let the QSPI cache fetch a full 32 B line as a
  single bus transaction (vs 8 single-beat reads today) — measurable
  saving on miss penalty (~155 cyc → ~50 cyc per miss) and bus
  arbitration churn.  Pair with the cache improvements above; both
  attack the same bottleneck from different sides.

## P2 — cleanup

<!-- Done: I2S_FIFO_DEPTH lives in the root Makefile, exported to
     test/Makefile (?=) and pushed into the Verilog elaboration via
     UART_VERILOG_MACROS / VERILOG_MACROS so kws_soc.v's
     `apb_i2s_receiver` instance reads it through `I2S_FIFO_DEPTH.
     Same source drives I2S_DMA_BURST_WORDS in test/Makefile. -->
<!-- Done: hazard3_config.vh is the single source of truth for the
     RV32 ISA and enabled extensions; the root Makefile greps the
     EXTENSION_* parameters out of it to compose RV_ARCH (= gcc
     -march), and exports RV_ARCH / RV_ABI for test/Makefile to
     consume.  Flip an extension by editing hazard3_config.vh once
     and both the CPU build and the firmware compile pick it up. -->
<!-- Done: every NNoM model now lives under test/model/<name>/ —
     test/model/strided_s16_nodil/, test/model/mel_compact_4blk_ch36/,
     test/model/streaming/.  test/Makefile dir vars updated; build
     identical (mel_compact_4blk_ch36_xip.elf same 28380/53228 size).
     Renames done with git mv so blame/log follow.  Part (1) of
     TODO #7. -->
<!-- Done: third_party/nnom moved to ./nnom alongside busfabric/,
     peris/, etc.  .gitmodules path updated, test/Makefile NNOM_DIR
     re-anchored, README + nnom_port.h doc strings updated.  Build
     verified across all three model targets.  Part (2) of TODO #7. -->
<!-- TODO #7 fully done: model dirs consolidated under
     test/model/<name>/ AND nnom moved out of third_party/. -->

- **Software cache invalidation hook on firmware update** (TODO at
  `peris/xip/ro_cache.v:1`).  `ro_dmc` only invalidates a line when
  the cache controller fetches it; if the firmware ever writes new
  bytes to the flash region currently mirrored in the cache,
  subsequent fetches return stale data.  Today nobody writes flash
  at runtime, so it's latent — but the bootloader / OTA path will
  expose it.  Add a write-1-to-clear "invalidate-all" CSR (or a
  per-line invalidate range) in the cache and call it after any
  flash write from firmware.
- **`ahbl_flash_ctrl_eb_cache` parameters config-dependent** (TODO
  at `kws_soc.v:668`).  The `LW(32*8) NL(256)` literal in the
  instantiation should come from the same `kws_soc_config.vh` /
  Verilog macros pipeline that already drives `SRAM_DEPTH`,
  `CLK_MHZ`, `I2S_FIFO_DEPTH` etc., so an FPGA build can opt into
  NL=1024 (32 KB, fits all .text) and an ASIC build can dial back
  to NL=128 (4 KB) without editing the source.  See the existing
  `RV_ARCH` plumbing for the pattern.
- **Drop the `xip_hsel_internal` splitter workaround** (TODO at
  `kws_soc.v:676`).  The XIP slave currently re-derives its own
  HSEL from the AHB address because the splitter doesn't honour the
  master's HSEL cleanly.  Fix the splitter to pass HSEL through and
  remove the workaround wire.
- **`fpga_top.v` CLK_MHZ duplication** (TODO at `quartus/fpga_top.v:60`).
  The `CLK_MHZ` parameter on the `kws_soc` instantiation is a magic
  number that has to track the PLL.  Wire it from the same root
  Makefile knob that already drives `\`CLK_MHZ` in
  `UART_VERILOG_MACROS` and feeds the kws_soc parameter elsewhere.
- **Drop the `kws_soc_tb.cpp` "delete TODO" comment** (TODO at
  `kws_soc_tb.cpp:25`).  Either delete the comment if the
  surrounding code is fine, or convert to an `#error` if the macro
  it references really must be defined.  Whichever way, no more
  `// TODO: Delete todo`.
<!-- Done: working-fpga patch gated behind FPGA_PATCHES=1 in
     scripts/apply_patches.sh; sim builds leave Hazard3 clean. -->

## P3 — developer velocity

<!-- Done: test/Makefile MODEL_CFLAGS now picks up CLK_MHZ /
     UART_BAUD_RATE via $(GLOBAL_UART_CONFIG) instead of hardcoded
     `-DCLK_MHZ=36 -DUART_BAUD_RATE=115200`.  Same source as the
     non-model CFLAGS already used. -->
- **VPI cycle counter halt-on-ebreak**.  Hazard3's `mcycle` CSR pauses
  on debug halt; the Verilator VPI's wallclock counter does not, so
  any measurement bracketing a debug halt is inflated by GDB time.
  Detect `dbg_halted` in the VPI and pause its counter accordingly.
- **Speed up Verilator simulation**.  Wire `--threads N` (Verilator's
  static graph partitioning) with `-lpthread` in CXXFLAGS, default
  `nproc/2`.  Goal: ≥2× wall-clock for a 1-second-of-audio run.
- **Speed up OpenSTA**.  Check whether forks support real
  parallelism; parallelise corner runs (tt/ss/ff) in separate
  processes; batch SDC checks per session.  Low priority — only
  matters for long ASIC iteration loops.
- **`flashsim` — handle reset commands** (TODO at
  `sim/flashsim.cpp:446`).  The simulated QSPI flash currently
  ignores reset commands.  Real GD25 / W25 parts respect them, so
  any firmware flow that resets the flash mid-test (e.g. OTA path
  validation) silently diverges from real hardware.  Implement
  the reset cmd; ~10 lines.
- **Adaptive QSPI SCK divider** (TODO at
  `peris/xip/flash_ctrl_eb.v:1`).  Today the divider is fixed at /2
  of the system clock.  At 36 MHz that gives 18 MHz SCK — within
  GD25 spec.  At higher system clocks (FPGA can run >100 MHz with
  smaller models) the SCK exceeds flash spec and reads silently
  corrupt.  Make the divider a parameter driven by the same root
  Makefile that exports `CLK_MHZ`, with a runtime sanity check
  against the configured flash part's max SCK.
- **CRM 2-cycle XIP read** (TODO at `peris/xip/flash_ctrl_eb.v:67`).
  Continuous Read Mode lets a flash respond in 2 dummy cycles
  instead of the full 8, doubling sustained read throughput.  The
  controller assumes CRM but the GD25 part on the DE10 doesn't
  support the truncated form, so we run with the long path.  Add a
  parameter to select the timing per-part; will pay back roughly
  half the per-miss QSPI cost (~155 cyc → ~80 cyc on miss).
- **JTAG-driven flash reset reaches external pin** (TODO at
  `quartus/fpga_top.v:4`).  Today a `dmihardreset_req` from JTAG
  resets the SoC but doesn't toggle the flash CSn / RESETn pin, so
  the QSPI part can be left in an indeterminate state across
  debug halts.  Wire the SoC-level reset out to the flash reset
  pin (via Quartus IO assignment).
- **Pythonic testbench harness** (TODO at `Makefile:603`).  The
  `xip-testbench` target runs Verilog testbenches via vvp and
  prints raw output; needs a wrapper that runs them all, parses
  PASS/FAIL, and emits a single report.  Low priority — current
  workflow inspects each TB by hand.
- **`asm` target dependency tracking** (TODO at `Makefile:613`).
  The `asm` target sometimes misses recompilation order — needs a
  proper dependency graph between Quartus stages.
