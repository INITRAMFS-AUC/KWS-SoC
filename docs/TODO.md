# KWS-SoC — Open work, ranked by importance

Priority ladder:
- **P0** — correctness / data integrity.
- **P1** — perf / power wins (≥10% inference cycles, ≥1 order of
  magnitude on idle-state energy).
- **P2** — cleanup that prevents future bugs (single source of truth,
  fragility removed).
- **P3** — developer velocity.

## P0 — correctness

- **ring_buf overflow**.  Trap handler silently drops samples once
  `ring_pos == SAMPLES_PER_CLIP`.  Inference takes longer than one
  capture window today, so every clip after the first is on
  **partial** audio.  Detect, or extend ring to 2× clip, or queue
  multiple input buffers.
- **`-fno-lto` hangs `nnom_model_create`**.  LTO build runs end-to-end;
  `-fno-lto` hangs after the boot banner.  Lost diagnostic GDB
  visibility; real bug somewhere in NNoM static-memory init that LTO
  is masking.

## P1 — perf / power

- **Voice-activity detector before model_run**.  Today every clip
  pays ~108M cycles of inference whether anyone is talking or not.
  Real deployment is mostly silence — gate with even a free
  energy-threshold VAD and inference power drops 90 %+.  Hook in
  `kws_bare_main.c` between `memcpy` and `model_run`.
- **Merge peris/i2s `nnom-quantize` branch (HW Q7 + downsample)**.
  A branch on the peris/i2s submodule does hardware-side Q7
  conversion + decimation.  After merge: drop the `>>16` extraction
  in the ISR, capture-leg bus traffic shrinks 4×.  Steps: identify
  branch, bump submodule, update `kws_soc.v` instantiation, update
  ISR / DMA configuration.
- **DMA drains valid I2S FIFO entries autonomously** (Plan B/C/D, not
  A).  Today the firmware burst is hardcoded and must match the I2S
  half-full threshold or every other 4-block in `ring_buf` is silently
  zero-padded.  I2S keeps interrupting at half-full; DMA reads
  whatever count is in the FIFO.  Three paths: (D) direct I2S→DMA
  wire, (C) level-triggered AHB drain, (B) DMA reads SIZE from I2S
  `fifo_count` register.
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
- **Gate `working-fpga-hazard3_core.patch` behind FPGA target**.
  Patch is FPGA-only (d-port→APB-bridge HWDATA race workaround,
  see `docs/2port_dport_bridge_bug.md`).  Empirically costs 0 cycles
  on sim — *not* a perf issue — but cleanliness wants it gated by
  `FPGA_PATCHES=1` so sim builds skip it.

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
