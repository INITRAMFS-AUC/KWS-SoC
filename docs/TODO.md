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

- **Test-stimulus provenance unknown — classification regression
  deferred**.  After bumping `peris/i2s` to the protocol-correct
  RX core (skips the leading Z SCK cycle, captures the 24 audio
  bits at `shifter[23:0]`), every keyword in `sim/<key>_0000.hex`
  classifies as `unknown`.  Root cause is a stimulus-vs-receiver
  alignment offset: the existing hex files use the legacy
  `(int32_t)q7 << 16` convention which puts the q7 byte at bits
  [23:16] — under the new RX that maps to `audio[16:9]`, not the
  byte the firmware reads with `(int8_t)(fifo >> 16)`.  Two open
  questions block the fix: (a) what was the original audio source
  (16-bit Google Speech Commands, real 24-bit INMP441, something
  else), and (b) what byte slice was the model trained on.  Until
  we can answer both, regenerating hexes via
  `scripts/realign_q7_hex.py` is just guesswork.  Stimuli stay as-is
  for cycle-count regression; classification correctness gets
  validated end-to-end on real INMP441 silicon when that path is
  brought up.

- **`peris/i2s` `q8-sel-flag` branch needs to be pushed upstream**.
  The submodule pointer in this repo is at the local commit
  `4335f2f` (adds the compile-time `Q8_SEL` flag for byte selection
  under WIDTH=8).  Until the branch is pushed to the i2s remote, a
  fresh `git submodule update --init` on someone else's checkout
  will fail to find the SHA.  Push to `origin` on the i2s remote
  and verify cloning works from scratch.

- **Conv1D accelerator input-misalignment limitation (TC1/TC2)**.
  `test/accel/accel_test.c` runs three hand-written cases through
  `accel_conv1d.h`.  TC1 (C_in=1, C_out=2, K_w=3, stride=1, W_in=5)
  and TC2 (C_in=1, C_out=1, K_w=65, stride=1, W_in=68) fail because
  HSIZE_WORD input reads aren't word-aligned when stride×C_in is not
  a multiple of 4.  TC3 (C_in=4) passes — the production
  `mel_compact_4blk_ch36` and `mel_compact_int8` paths only exercise
  C_in=4 layers, so this is latent until a model with C_in not %4
  reaches the accelerator.  Fix: detect mis-alignment in the
  accelerator, fall back to HSIZE_BYTE reads (slower but correct).
  Fail-fast in `nnom_conv1d_hw.c` is the firmware-side stopgap.

## Deferred (waiting on external dep)

- **ring_buf overflow / lost audio between clips — RE-EVALUATE NOW
  THAT ACCELERATOR LANDED**.  Inference is now ~1.08M cycles for
  `mel_compact_int8_xip_accel` (vs ~46M SW), well inside one capture
  window, so the gap that motivated this entry is closed for the
  accelerated path.  Confirm under real workloads (non-back-to-back
  inferences, sliding window enabled per the entry below) and close
  this when verified.  SW-only models still have the gap.

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
- **Conv1D accelerator landed — `mel_compact_int8_xip_accel` at
  1.08M cycles**.  Merged on branch `merge-conv1d-accel` (this PR).
  RTL: `peris/conv1d_accel/conv1d_accel.v` (4-MAC datapath, ping-
  pong input buffers, word-aligned weight bursts).  Firmware:
  `test/common/nnom_conv1d_hw.c` shim replaces NNoM's `conv2d_run`.
  Build target: `make model-mel-compact-int8-accel`.  Verified
  CYCLES_INFER:
      mel_compact_int8_xip       (SW)          ~46M
      mel_compact_int8_xip_accel (HW conv1d)   1.08M  (≥ 42× speedup)
  perf gate met (target 1.09M).  Classification on existing test
  stimuli is `unknown` for all clips — see the test-stimulus
  provenance entry under P0.

- **Conv1D accel: clock/data gating opportunities (P1 follow-up)**.
  The 4-MAC datapath toggles every cycle in S_MAC regardless of
  `r_busy`; weight RF (`wt_buf`) is written every weight-load
  state.  Wrapping the multipliers in a state-derived clock-enable
  and gating `wt_buf` write-enable when `state ∉ {S_WT_DATA}` saves
  switching power on the inactive cycles.  Bias / per-channel
  shift registers latch only at `S_BIAS_DATA` / `S_SHIFT_DATA` —
  already inherently gated, but worth confirming with a power
  report on the next ASIC pass.  The debug counter `dbg_ctr` is
  now `\`ifdef ACCEL_DEBUG`-gated (no toggling 21-bit counter when
  undefined).

- **Per-channel `SHIFT_ADDR` is wired but unused**.  The accelerator
  exposes `SHIFT_ADDR` (0x20) for per-channel right-shift arrays,
  but every model in tree today shares one scalar shift across all
  C_out.  Exercise it with a per-channel-quantized model so the
  tested code path matches the synthesised one.  Otherwise the
  per-channel logic is silently dead until a future model needs it.

- **Flip on sliding-window inference now that the Conv1D accelerator
  is in tree.** Trigger: a `model_run` cycle count that fits comfortably
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

- **APB splitter slot map is hand-coded — make config-driven**.
  `kws_soc.v` now embeds an 80-bit `ADDR_MAP` / `ADDR_MASK` for
  five APB slots (timer / uart / i2s / accel / snooper).  Adding a
  6th slave or changing region sizes means editing the literals by
  hand.  Move the slot map into a Verilog header (or a small
  Python codegen) keyed by a peripheral list, the way `peris.f`
  already enumerates the peripheral RTL.  Same SSoT pattern as the
  RV_ARCH plumbing.

- **Conditional N_MASTERS in kws_soc.v duplicates the crossbar
  port-list**.  The `\`ifdef XIP_PLAYBACK` branch carries 5
  master concats (playback + accel + dmac + d + i); the `\`else`
  branch carries 4 (no playback).  ~14 lines duplicated per
  branch.  Fold into a tie-off pattern (`playback_*` always
  declared, hardwired to IDLE/0 when `XIP_PLAYBACK` undefined,
  `N_MASTERS=5` always) so the crossbar instance has one set of
  port assignments.  Tiny synthesis cost (extra dead inputs) for
  a much cleaner source.

- **Defer-or-drop the `Q8_SEL` Makefile knob if upstream commits
  the i2s `q8-sel-flag` branch**.  Today the parent project ships a
  `Q8_SEL=MSB|MID|LSB` knob that propagates through `VERILOG_MACROS`
  to the i2s submodule's compile-time byte selector.  If the
  submodule's `q8-sel-flag` branch is merged into i2s `main`, the
  flag becomes a project-local choice the user can make once and
  forget.  If we ever determine the byte slice that matches the
  trained model and pin it, drop the knob entirely.
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
