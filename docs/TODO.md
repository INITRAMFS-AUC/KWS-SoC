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

- **DMA-direct-to-NNoM-input + LSB=oldest Q8 pack (Option 1).**
  Eliminate `ring_buf`, `dma_batch`, the ISR byte-unpack, and the
  end-of-clip memcpy in one pass.  Steps: (a) RTL one-line change in
  `apb_i2s_receiver.v` to pack `{byte3, byte2, byte1, byte0}` with
  byte0 (oldest) at LSB so DMA writes land in time order on
  little-endian RV32; (b) point DMA `daddr` directly at
  `nnom_input_data`; (c) ISR shrinks to `DMAC->daddr += burst*4`
  per PIRQ.  Saves ~66K cycles/clip (ISR 290→55 cycles × 250) plus
  8 KB SRAM (`ring_buf` gone).  Doesn't reduce IRQ count — the DMA
  resets `CNTR` between frames so DADDR must advance per frame.
- **Investigate Q8 ISR cycle regression** *(captured for follow-up
  if it survives the Option 1 rewrite — likely subsumed)*.  Audit
  (2026-05-02) found switching to HW Q8_EN packing made
  `CYCLES_INFER` ~1.7M cycles *worse* (108.13M → 109.86M,
  steady-state clip 2) even though IRQ count drops 4× (1000 → 250
  per clip).  Each ISR now does 4× more byte stores per IRQ.
  Hypothesis: store-buffer / d-port back pressure during ISR while
  DMA is still draining the FIFO.  Re-measure after Option 1.
- **HW skip-R-slot SCK in I2S MONO_MODE** (peris/i2s Phase B).
  Today `apb_i2s_receiver` toggles SCK during the R slot even though
  MONO_MODE discards R data.  Half of all SCK transitions burn power
  for nothing.  Patch the SCK generator to gate toggles on
  `ws_reg == 0` (L slot) when MONO_MODE && cfg_q8_en.  Needs a
  proper testbench (Phase C) — current sim has no I2S-protocol
  assertions.
- **Merge peris/i2s `nnom-quantize` branch (HW Q7 + downsample)**.
  Phase A done (firmware uses Q8_EN, ISR unpacks 4 samples/word
  MSB-first, IRQ count 4× lower).  Phase B (HW skip-R-slot) and
  Phase C (testbench) tracked separately above.
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

## P2 — cleanup

- **Single source of truth for I2S FIFO depth + DMA burst**.
  `I2S_FIFO_DEPTH` lives in `kws_soc.v` *and* `test/Makefile`; drift
  silently corrupts ring_buf.  Half-fixed (Makefile derives burst);
  finish by driving the Verilog parameter from the same root-Makefile
  variable via VERILOG_MACROS.
- **Reorganise repo layout**.  (1) Consolidate every NNoM model under
  `test/model/<model_name>/` (today: `test/model/`, `test/mel_compact/`,
  `test/streaming_model/`).  (2) Move `third_party/nnom` to `./nnom`
  next to `busfabric/`, `peris/`, etc. — it's not really third_party.
  Atomic restructure commit, `git mv` preserves history.
<!-- Done: working-fpga patch gated behind FPGA_PATCHES=1 in
     scripts/apply_patches.sh; sim builds leave Hazard3 clean. -->

## P3 — developer velocity

- **Pipe root Makefile values into test/Makefile**.  `test/Makefile`
  hardcodes `CLK_MHZ=36`, `UART_BAUD_RATE=115200`.  Root Makefile
  already exports `GLOBAL_UART_CONFIG`; have test/Makefile inherit so
  one change propagates.
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
