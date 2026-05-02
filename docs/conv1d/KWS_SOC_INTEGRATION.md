# KWS-SoC Integration Guide — Conv1D Accelerator

This guide explains how to drop the verified Conv1D accelerator into the KWS-SoC tree. The accelerator core, APB control wrapper, and SoC-facing top wrapper are all verified in simulation. Memory-side ports are abstract and require an SoC memory wrapper (scratchpad, AHB-Lite master, or DMA), as described below.

## A. Files to copy into KWS-SoC

Copy these files into the KWS-SoC repository:

- `rtl/conv1d_layer_accel.v` — Conv1D datapath (4-lane packed-int8, K=3).
- `rtl/apb_conv1d_layer_accel.v` — APB register wrapper around the core.
- `rtl/conv1d_accel_soc_wrapper.v` — SoC-facing top wrapper (single instantiation point).
- `sw/conv1d_accel_regs.h` — firmware register map and helpers.
- `sw/conv1d_accel_smoke_test.c` — firmware smoke-test template (optional).

## B. Suggested KWS-SoC location

Place the RTL under a peripheral subdirectory:

```
peris/conv1d/conv1d_layer_accel.v
peris/conv1d/apb_conv1d_layer_accel.v
peris/conv1d/conv1d_accel_soc_wrapper.v
```

The firmware header lives next to other peripheral headers in the firmware tree (the exact path depends on your firmware build):

```
sw/include/conv1d_accel_regs.h
sw/src/conv1d_accel_smoke_test.c
```

## C. Add to `peris/peris.f`

Append the three RTL files to your peripheral filelist:

```
file conv1d/conv1d_layer_accel.v
file conv1d/apb_conv1d_layer_accel.v
file conv1d/conv1d_accel_soc_wrapper.v
```

If your build flow uses `+define+` style overrides for the weight-stationary mode, add (recommended for KWS to reduce external weight bandwidth):

```
+define+USE_WEIGHT_BUFFER=1
```

The default without that define is the streaming nobuffer mode, which is also fully verified.

## D. Suggested APB base address

```
CONV1D_BASE = 0x4000_C000
```

The firmware header `sw/conv1d_accel_regs.h` defaults to this base. Override `CONV1D_BASE` at build time if your APB splitter places the slave somewhere else.

## E. APB splitter integration

The accelerator presents itself to the SoC as one more APB-3 peripheral, equivalent in shape to the existing timer / UART / I2S slaves. Add it to the SoC's APB mux/splitter exactly as you would any other slave:

- Connect `psel`, `penable`, `pwrite`, `paddr`, `pwdata`, `prdata`, `pready`, `pslverr` from the SoC APB mux/splitter.
- Drive `pclk` and `presetn` from the SoC peripheral clock and reset domain.

This guide does not claim exact signal packing for KWS-SoC's mux/splitter source, since that depends on the version of the SoC RTL you have in front of you. Wire the eight APB signals plus clock/reset using the same convention as your existing slaves.

### KWS-SoC Phase 5A/5B wiring status

The integration branch wires `conv1d_accel_soc_wrapper` into `kws_soc.v` as
the fourth APB splitter slave:

- timer: `0x4000_0000`
- I2S: `0x4000_8000`
- UART: `0x4000_4000`
- Conv1D accelerator: `0x4000_C000`

For APB-only bring-up, the accelerator memory-side ports are connected to a
safe zero-data stub: read-valid follows read-enable by one clock, read data is
zero, and writes are consumed/ignored. This supports ID/config readback smoke
tests only. It is not the final scratchpad or AHB/SRAM memory path.

For simulation/local bring-up, define `CONV1D_USE_SIM_SCRATCHPAD` to replace
that stub with `conv1d_scratchpad_mem`. The scratchpad is 32-bit
word-addressable, uses byte addresses on the accelerator side, has two
one-cycle read ports, and applies byte strobes on writes. This is a tiny-layer
validation path, not the production SoC memory wrapper.

`make run-conv1d-tiny-golden` preloads the scratchpad with:

- `INPUT_BASE = 0x0000_0000`
- `WEIGHT_BASE = 0x0000_0100`
- `BIAS_BASE = 0x0000_0200`
- `OUTPUT_BASE = 0x0000_0300`
- `input_len = 8`, `in_ch = 4`, `out_ch = 2`, `K = 3`
- `output_shift[0] = 0`, `output_shift[1] = 1`

Expected output bytes are:

| t | oc0 | oc1 |
|---|-----|-----|
| 0 | 10  | 4   |
| 1 | -2  | 4   |
| 2 | 0   | -3  |
| 3 | 1   | 7   |
| 4 | 12  | -2  |
| 5 | 5   | 0   |

Passing this test proves APB decode plus memory-side reads/writes plus Conv1D
datapath operation through the SoC-level integration path. It still does not
claim live KWS `model_run()` acceleration.

## F. Memory-side integration options

The accelerator's memory ports are intentionally abstract:

- `rd0_en` / `rd0_addr` / `rd0_data` / `rd0_valid` — input + bias reads.
- `rd1_en` / `rd1_addr` / `rd1_data` / `rd1_valid` — weight reads (preload only, in buffered mode).
- `wr_en` / `wr_addr` / `wr_data` / `wr_strb` — output byte-strobed writes.

Three integration paths are supported:

1. **Local scratchpad memory wrapper** *(recommended for first integration)*. Wrap the ports with a small dual-port SRAM block that the CPU pre-loads via DMA or direct stores before each layer. Simplest functional bring-up; lowest integration risk.
2. **AHB-Lite master wrapper**. Wrap each port with an AHB-Lite master that arbitrates onto the SoC fabric. Adds latency-tolerance requirements (already verified up to `MEM_LATENCY=3` in simulation) and bus arbitration concerns.
3. **DMA preload/writeback wrapper**. Have a DMA engine stage tensors into a scratchpad before `start` and write outputs back after `done`. Highest peak throughput; most integration work.

Recommended first integration path: **APB control + local scratchpad memory wrapper**. Once functional, evolve to AHB-Lite or DMA-backed data movement.

## G. Integration milestones

1. **Read ID from firmware** — confirm APB plumbing works. Read `CONV1D_ID`, expect `0x12345678`.
2. **Write config registers** — write `INPUT_BASE`, `WEIGHT_BASE`, `BIAS_BASE`, `OUTPUT_BASE`, `INPUT_LEN`, `IN_CH`, `OUT_CH`, `QUANT`. For NNoM per-axis quantized layers, also load `QUANT_INDEX`/`QUANT_SHIFT_DATA` for each output channel. Read back at least one to confirm.
3. **Start accelerator and observe done** — write `CTRL.start`, poll `STATUS.done`. With memory ports tied off this validates only the control path; expect zero useful work.
4. **Connect fake/local memory and run tiny layer** — wire memory ports to a scratchpad, place a small tensor (e.g. `input_len=8`, `in_ch=4`, `out_ch=2`), run, and compare the output bytes against a golden software reference.
5. **Connect SRAM/AHB** — replace the scratchpad with the real SRAM/AHB wrapper. Re-run the tiny layer end-to-end.
6. **Run real KWS Conv1D layer** — load the production model's first Conv1D weights/biases, point the registers at them, and confirm bit-equivalent output to your software reference.

Each milestone is independently verifiable; failures bisect cleanly.

## H. Honest limitations

- The APB control wrapper is **verified in simulation only** (full A-G test suite at `MEM_LATENCY = 1, 2, 3`, both `USE_WEIGHT_BUFFER=0` and `USE_WEIGHT_BUFFER=1`, plus a dedicated APB testbench and a SoC-wrapper testbench).
- **Memory-side ports are abstract** — every result in this repo uses a behavioral memory model in the testbench. They need a real SoC memory wrapper (scratchpad / AHB-Lite / DMA) before they will move data in silicon.
- **Full AHB/SRAM connection is future work**. The variable-latency simulation gives confidence the datapath tolerates `MEM_LATENCY=3`, but a real bus has arbitration, contention, and clock-crossing concerns not modelled here.
- `MAX_IN_CH = 64` for the buffered mode's local weight buffer. The testbench refuses to run buffered mode beyond that.
- The `USE_WEIGHT_BUFFER` parameter on `conv1d_accel_soc_wrapper` is documentation/metadata; the runtime mode is selected by the `\`USE_WEIGHT_BUFFER` build define on the inner core. Set both consistently in your synthesis flow.

### Per-channel quantization shifts

NNoM Conv2D_1/2/3 use per-output-channel `output_shift` arrays, so scalar
`QUANT[4:0]` is not sufficient for bit-exact replacement. The accelerator now
supports both modes:

- `QUANT` at `0x28`: scalar fallback, `[4:0]=out_shift`, `[5]=relu_en`. A scalar
  write seeds every per-channel shift slot with the same value.
- `QUANT_INDEX` at `0x2C`: output-channel index for the next shift write.
- `QUANT_SHIFT_DATA` at `0x30`: writes `[4:0]` into the selected per-channel
  shift slot.

For NNoM Conv2D replacement, prepare bias as
`(bias[oc] << bias_shift[oc]) + NNOM_ROUND(output_shift[oc])`, load
`output_shift[oc]` through the per-channel registers, and keep `relu_en = 0`
because ReLU remains a separate NNoM layer.

### Firmware APB call path

The bridge helper `conv1d_accel_run_prepared_layer()` programs the APB wrapper
for an already-prepared K=3 Conv1D layer. It writes:

- `INPUT_BASE`, `WEIGHT_BASE`, `BIAS_BASE`, `OUTPUT_BASE`
- `INPUT_LEN`, `IN_CH`, `OUT_CH`
- scalar `QUANT` with `out_shift = 0`, `relu_en = 0`
- one per-channel shift entry per output channel via
  `QUANT_INDEX`/`QUANT_SHIFT_DATA`
- `CTRL.start`, followed by a bounded poll of `STATUS.done`

Fallback is returned without touching live inference when any required pointer
is null, a pointer is not 4-byte aligned, `input_len < 3`, `in_ch` or `out_ch`
exceeds 64, an output shift does not fit the 5-bit hardware field, or the done
poll exceeds `CONV1D_ACCEL_TIMEOUT` (default `1000000` iterations).

The APB call path is intentionally not wired into `model_run()` yet. The scratchpad
loader path (`SPAD_ADDR`/`SPAD_WDATA`/`SPAD_RDATA`) provides a firmware-visible
preload/readback route without an AHB master. The firmware test
`test/conv1d/c/conv1d_accel_tiny_golden_fw.c` exercises this path end-to-end:

- Preloads input, weights, and bias through APB scratchpad loader registers
- Configures all APB control registers and per-channel output shifts
- Fires `CTRL.start` and polls `STATUS.done`
- Reads output through `SPAD_RDATA` and compares against the verified golden values

Build with `make -C test conv1d-tiny-fw` (requires `riscv32-unknown-elf`). The
equivalent RTL testbench runs without a toolchain: `make run-conv1d-spad-loader`.

The correct integration claim today is:

> The accelerator is packaged for KWS-SoC integration with a verified APB-facing wrapper, an APB scratchpad loader for firmware tensor preload/readback, and a firmware tiny golden test that exercises the complete control path.

## I. Tests requiring another device

The following tests are **not run on this development machine** because they require
`riscv32-unknown-elf-gcc` and Verilator (or the repo's full CXXRTL/Verilator SoC
simulator). Run them on a device with the RISC-V toolchain and Verilator installed.

**No full `model_run()` acceleration result is claimed until these pass.**

---

### 1. Firmware tiny golden test

```bash
# Build firmware ELF and binary
make -C test conv1d-tiny-fw

# Run in SoC simulation (no JTAG, 500k cycle budget)
make sim-verilator FLASH=test/build/conv1d_tiny_golden_fw_xip.bin NO_JTAG=1 CYCLES=500000
```

Expected UART output:

```
CONV1D_ID:PASS
CONV1D_TINY_FW:PASS
```

This proves the APB scratchpad loader path from real firmware code running on
the Hazard3 RISC-V core.

---

### 2. Baseline full `model_run()` cycle measurement (no accelerator)

```bash
make clean
make -C test mel-compact USE_MCYCLE_CSR=1
make sim-verilator FLASH=test/build/mel_compact_4blk_ch36_xip.bin \
    MIC=sim/down_0000.hex NO_JTAG=1
```

Expected UART output on first clip (exact numbers will vary by clock/model):

```
DETECT:0,down
CYCLES_CAPTURE:<N>
CYCLES_INFER:<N>
CYCLES_TOTAL:<N>
CYCLES:<N>
```

Record `CYCLES_INFER` as the baseline before any hardware acceleration.

---

### 3. Guarded accelerator integration (future — not yet implemented)

Once the `USE_CONV1D_ACCEL` bridge is wired into `kws_bare_main.c`:

```bash
make clean
make -C test mel-compact USE_MCYCLE_CSR=1 USE_CONV1D_ACCEL=1
make sim-verilator FLASH=test/build/mel_compact_4blk_ch36_xip.bin \
    MIC=sim/down_0000.hex NO_JTAG=1
```

Expected UART output: same detection result as baseline, with `CYCLES_INFER`
reduced. Compare `CYCLES_INFER` with and without `USE_CONV1D_ACCEL=1` for the
speedup figure.

**This step is not yet implemented.** `kws_bare_main.c` is unmodified; the bridge
exists (`test/conv1d/c/conv1d_accel_nnom_bridge.c`) but is not yet called from the
live inference path.
