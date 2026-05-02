# 4-Lane Packed-INT8 Conv1D Accelerator for KWS-SoC

A small, verified Conv1D (kernel size 3) accelerator with a packed-int8 4-lane MAC datapath, an APB control wrapper, and an optional weight-stationary buffered mode that cuts external weight traffic by 30× on the realistic Test G benchmark.

## Project Summary

This repository implements a Conv1D layer accelerator targeted at a keyword-spotting SoC (KWS-SoC). The design combines:

- A 4-lane packed-int8 MAC datapath that processes four channels per 32-bit word.
- Signed int8 inputs and weights, signed int32 accumulation, with bias, arithmetic-right-shift requantization, optional ReLU, and int8 saturation.
- An APB register wrapper verified in simulation, ready to be connected to the KWS-SoC APB splitter.
- A compile-time-selectable **weight-stationary buffered mode** that preloads weights for one output channel into a local buffer and reuses them across all output time steps.
- Two streaming testbenches (original A-F and enhanced A-G with variable memory latency) plus an APB testbench, all passing.

## Motivation: Conv1D Was the KWS Bottleneck

In a small keyword-spotting model running on an MCU-class core, the Conv1D layers dominate both cycle count and memory traffic. Doing those MACs in software ties up the CPU and burns memory bandwidth on weight reads that are reused across time. A dedicated accelerator with a small register-file weight cache reduces both: the MAC work runs in dedicated lanes, and the weight tensor for each output channel is fetched from external memory only once instead of once per output time step.

## Architecture Overview

- Conv1D, kernel size `K = 3`, signed int8 inputs and weights, signed int32 accumulator.
- 4-lane packed datapath: each 32-bit word holds four signed int8 lanes.
- Pipelined MAC read/accumulate loop and merged write/next-output transition.
- Single-entry bias prefetch (nobuffer mode); per-oc cached bias (buffered mode).
- Two memory ports: `rd0` for inputs and biases, `rd1` for weights (or weight preload only, in buffered mode).
- APB register wrapper exposing `start`, `busy`, `done`, base addresses, sizes, and quantization settings.

The accelerator is parameterized by `LANES` (default 4) and `MAX_IN_CH` (default 64). The buffered mode allocates a local weight buffer sized for `MAX_IN_CH`.

## 4-Lane Packed Datapath

The key idea behind the packed datapath:

- One 32-bit input word = four signed int8 lanes (channel group of 4).
- One 32-bit weight word = four signed int8 lanes for the same channel group.
- Each MAC group computes four signed int8 × int8 products in parallel.
- The four products are reduced into a signed int32 sum.
- The sum is added to the running int32 accumulator.
- After all `K * channel_groups` groups, the accumulator goes through:
  1. Add bias (already preloaded).
  2. Arithmetic right shift by `out_shift`.
  3. Optional ReLU clamp to zero.
  4. Saturation clamp to `[-128, +127]` int8.
  5. Byte-strobed write back to memory.

Channel-group fanout: `channel_groups = ceil(in_ch / 4)`. When `in_ch` is not a multiple of 4, the trailing lanes are masked out so they contribute zero to the sum (tail-channel masking).

### Datapath Equation

```text
acc = bias[oc]
for k in 0..K-1:
    for g in 0..channel_groups-1:
        ic = g * 4
        acc += sum_4_lanes(input[t+k][ic..ic+3] * weight[oc][k][ic..ic+3])
output[t][oc] = clamp_int8((acc >>> out_shift) [, ReLU])
```

## Weight-Stationary Buffered Mode

Selected at compile time with `-DUSE_WEIGHT_BUFFER=1` (default `0` = original streaming mode).

### Why weights can be reused

In the streaming nobuffer mode, the weight word for each `(oc, k, channel_group)` is re-read from external memory for every output time step `t`. For Conv1D, those weights are identical across all `t` values for a given `oc`. Re-reading them is wasted external bandwidth.

### What the buffered mode does

For each output channel `oc`:

1. **Bias load**: read `bias[oc]` once via `rd0`, cache as `oc_bias`.
2. **Weight preload**: issue `K * channel_groups` `rd1` reads to fill a local register-file `weight_buf[k * channel_groups + group]` with the packed weight words for this `oc`.
3. **MAC over t**: for each output time step `t = 0 .. out_len-1`:
   - `acc <= oc_bias`
   - Issue `rd0` reads for input groups; on each `rd0_valid`, multiply input lanes by `weight_buf[resp_count]` and accumulate. `rd1` stays idle.
   - Write `output[t][oc]` after the last group.
4. Advance to the next `oc`.

Output address layout `output[t][oc] = output_base + (t * out_ch + oc)` is unchanged — only the loop traversal order changes (`oc` outer, `t` inner). Tail masking, signed multiply, int32 accumulation, `out_shift`, ReLU, saturation, and bias semantics are preserved bit-for-bit.

### Buffer size

```text
MAX_IN_CH               = 64           (parameter, RTL default)
MAX_CH_GROUPS           = 16           (= ceil(MAX_IN_CH / 4))
WEIGHT_WORDS_PER_OC_MAX = 48           (= K * MAX_CH_GROUPS)
```

The enhanced testbench checks every `set_cfg` against `MAX_IN_CH` in buffered mode and fails the run with a clear message if a configuration would overflow `weight_buf`. This is a sim-time guard; running at `in_ch <= 64` is supported.

## APB Control Wrapper

`apb_conv1d_layer_accel.v` wraps the accelerator core and exposes a minimal APB-3 register slave interface. The wrapper holds the base addresses, sizes, and quantization configuration, drives `start` from the CPU, and reports `busy` / `done` back via the status register. `STATUS.done` is sticky until the next `start`, making polled-completion firmware simple and safe.

## Tensor Layout

Memory is byte-addressed. The packed layout treats four consecutive int8 channels as one 32-bit word:

```text
input[t][ic]      = input_base  + ((t * channel_groups) << 2) + (ic & 3)
                    where channel_groups = ceil(in_ch / 4)
weight[oc][k][ic] = weight_base + ((((oc * K) + k) * channel_groups) << 2) + (ic & 3)
bias[oc]          = bias_base   + oc * 4         (signed int32)
output[t][oc]     = output_base + (t * out_ch) + oc   (signed int8)
```

## Register Map

The verified APB wrapper exposes the following registers:

| Address | Name           | Access | Description |
|---|---|---|---|
| `0x00`  | `ID`           | RO     | Wrapper ID/version (`0x12345678`) |
| `0x04`  | `CTRL`         | WO     | `bit[0] = start` |
| `0x08`  | `STATUS`       | RO     | `bit[0] = busy`, `bit[1] = done` (sticky until next start) |
| `0x0C`  | `INPUT_BASE`   | RW     | Input tensor base address |
| `0x10`  | `WEIGHT_BASE`  | RW     | Weight tensor base address |
| `0x14`  | `BIAS_BASE`    | RW     | Bias tensor base address |
| `0x18`  | `OUTPUT_BASE`  | RW     | Output tensor base address |
| `0x1C`  | `INPUT_LEN`    | RW     | Input length (samples) |
| `0x20`  | `IN_CH`        | RW     | Input channels |
| `0x24`  | `OUT_CH`       | RW     | Output channels |
| `0x28`  | `QUANT`        | RW     | `bit[4:0] = out_shift`, `bit[5] = relu_en` |

## Verification Strategy

Three independent simulation paths are maintained, plus a buffered-vs-nobuffer split:

1. **Original A-F testbench** (`tb/tb_conv1d_layer_accel.v`) — backward-compatible single-cycle-latency functional coverage.
2. **Enhanced A-G testbench** (`tb/tb_conv1d_layer_accel_enhanced.v`) — variable-latency memory model, detailed cycle accounting, mode-aware MAC-active metric, traffic counters, and a `MAX_IN_CH` guard. Compiled twice: nobuffer (`-DUSE_WEIGHT_BUFFER=0`) and buffered (`-DUSE_WEIGHT_BUFFER=1`).
3. **APB wrapper testbench** (`tb/tb_apb_conv1d_layer_accel.v`) — exercises the CPU-facing register/control path. Also compiled in both modes.

Compile-time switches: `-DMEM_LATENCY=1|2|3` and `-DUSE_WEIGHT_BUFFER=0|1`.

## Test Cases A-G

| Test | Purpose                                  | `input_len` | `in_ch` | `out_ch` | `out_shift` | `relu` |
|------|------------------------------------------|------------:|--------:|---------:|------------:|:------:|
| A    | Full lane usage                          | 6           | 4       | 2        | 0           | off    |
| B    | Tail-lane masking                        | 6           | 5       | 2        | 0           | off    |
| C    | Single-channel sanity                    | 5           | 1       | 1        | 0           | off    |
| D    | Requantization shift                     | 6           | 4       | 1        | 2           | off    |
| E    | ReLU clamp                               | 5           | 4       | 1        | 0           | on     |
| F    | Saturation clamp `±127/-128`             | 5           | 4       | 2        | 0           | off    |
| G    | Realistic benchmark (deterministic PRNG) | 32          | 16      | 8        | 1           | on     |

Each test compares the accelerator output against an independent golden model in the testbench.

## Variable-Latency Memory Testing

The enhanced testbench includes a parameterized memory delay model (shift-register chain on each read port). Builds at `MEM_LATENCY = 1, 2, 3` are run in sequence; the same A-G tests pass at every latency for both nobuffer and buffered modes.

Per-port traffic is reported per test: `rd0_req` (input + bias requests), `rd1_req` (weight requests), `Write` cycles, `MAC_Active`, `Total_Busy`, `Theory`, and `Overhead`.

## APB Wrapper Testing

The APB testbench programs the accelerator end-to-end through APB writes (no shortcuts), polls `STATUS.done`, and checks the output memory against an independent golden model. It runs in both nobuffer and buffered builds.

## Performance / Traffic Results

### Configuration of Test G

```text
input_len     = 32
output_len    = 30
in_ch         = 16
out_ch        = 8
K             = 3
total outputs = 240
theoretical MAC groups       = 2880     (out_len * out_ch * K * channel_groups)
scalar 1-lane theoretical    = 11520    (out_len * out_ch * K * in_ch)
ideal 4-lane packed speedup  = 4×
```

### Test G Traffic / Cycle Comparison

| Mode     | Latency | Busy  | Theoretical MAC Groups | External Weight Reads (rd1) | Correctness |
|----------|--------:|------:|-----------------------:|----------------------------:|:-----------:|
| Nobuffer | 1       | 4562  | 2880                   | 2880                        | PASS        |
| Nobuffer | 2       | 5042  | 2880                   | 2880                        | PASS        |
| Nobuffer | 3       | 5522  | 2880                   | 2880                        | PASS        |
| Buffered | 1       | 4233  | 2880                   | 96                          | PASS        |
| Buffered | 2       | 4489  | 2880                   | 96                          | PASS        |
| Buffered | 3       | 4745  | 2880                   | 96                          | PASS        |

### What the buffered mode buys you

- **30× fewer external weight reads on Test G** (2880 → 96). This is the headline benefit and is invariant across memory latency settings.
- **It does not mean a 30× total speedup.** In abstract simulation with single-cycle-latency, ideal `rd1` ports, the cycle-count improvement is modest:
  - `MEM_LATENCY=1`: 4562 → 4233 (≈7.2% faster)
  - `MEM_LATENCY=2`: 5042 → 4489 (≈11.0% faster)
  - `MEM_LATENCY=3`: 5522 → 4745 (≈14.1% faster)
- On a real AHB/SRAM with non-trivial latency, bus arbitration, or contention, replacing 2880 external weight transactions with 96 should translate into a noticeably larger real-world gain than the abstract-simulation cycle delta suggests, because the saved transactions free up the shared interconnect for input traffic and other masters.

### MAC-Active metric (mode-aware)

The enhanced testbench reports a mode-aware `MAC_Active` count so it equals the theoretical MAC-group count in both modes:

- **Nobuffer**: cycles where `rd0_valid && rd1_valid` (input and weight delivered together).
- **Buffered**: cycles where `rd0_valid` (input delivered), minus the `out_ch` bias-load cycles. Weights come from the local buffer, so `rd1` is intentionally idle during MAC.

This avoids the previously misleading "MAC_Active = 0" in buffered runs.

## SoC Integration Readiness

The accelerator is packaged for KWS-SoC integration with a verified APB-facing wrapper and a documented memory-side integration path. Specifically:

- **SoC-facing wrapper added** — `rtl/conv1d_accel_soc_wrapper.v` exposes a single APB-3 slave (`pclk`/`presetn` naming) plus the abstract `rd0`/`rd1`/`wr` memory ports. This is the only module KWS-SoC needs to instantiate.
- **Wrapper testbench added** — `tb/tb_conv1d_accel_soc_wrapper.v` drives the SoC top end-to-end through APB only: ID readback, register programming, `CTRL.start`, sticky-`STATUS.done` polling, and golden-model output check. Covered by `make run-soc-wrapper`.
- **Firmware register header added** — `sw/conv1d_accel_regs.h` provides register offsets, bit definitions, and inline helpers (`conv1d_write_reg`, `conv1d_read_reg`, `conv1d_start`, `conv1d_done`, `conv1d_busy`, `conv1d_run_layer`). Default base address `CONV1D_BASE = 0x4000_C000` matches the suggested KWS-SoC mapping; override at build time if needed.
- **Firmware smoke test template added** — `sw/conv1d_accel_smoke_test.c` is a copy-and-customize template for KWS-SoC firmware (ID check → configure → start → poll done → optional output verify).
- **KWS-SoC integration guide added** — `docs/KWS_SOC_INTEGRATION.md` walks through file copy, `peris.f` updates, APB splitter wiring, the three memory-side integration options (local scratchpad, AHB-Lite master, DMA), and milestone-by-milestone bring-up.
- **APB control path verified** — APB wrapper testbench, SoC wrapper testbench, and full A-G regression at `MEM_LATENCY = 1, 2, 3` in both `USE_WEIGHT_BUFFER` modes all pass.
- **Memory side prepared for AHB/SRAM wrapper** — the abstract memory interface is the integration boundary; latency tolerance up to `MEM_LATENCY=3` is verified.
- **Full KWS-SoC integration remains the next step** — physical APB-splitter wiring, real SRAM/AHB connection, and on-chip bring-up are explicitly out of scope of this repo.

The correct claim today is: **"The accelerator is packaged for KWS-SoC integration with a verified APB-facing wrapper and documented memory-side integration path."**

## How to Run

```bash
make clean
make run-original              # A-F, original tb (nobuffer)
make run-enhanced-all-lat      # A-G, nobuffer, MEM_LATENCY = 1, 2, 3
make run-apb                   # APB wrapper (nobuffer)
make run-enhanced-buffered     # A-G, weight-stationary buffered, MEM_LATENCY = 1, 2, 3
make run-apb-buffered          # APB wrapper (buffered)
make run-soc-wrapper           # SoC top wrapper (APB + abstract memory, buffered)
make run-all                   # Aggregate of every target above
```

Individual targets:

```bash
make run-enhanced-lat1
make run-enhanced-lat2
make run-enhanced-lat3
make run-enhanced-buffered-lat1
make run-enhanced-buffered-lat2
make run-enhanced-buffered-lat3
```

## Limitations

- `MAX_IN_CH = 64` (sized for the buffered weight buffer). The enhanced testbench fails fast with a clear message if a configuration would overflow.
- The buffered mode buffers weights only; inputs are still streamed every output time step. Input buffering would further reduce traffic but is out of scope for this submission.
- Memory ports are abstract in simulation. Real performance depends on the final memory subsystem (AHB/SRAM/DMA).
- The KWS-SoC APB-splitter integration is not yet wired up in this repository.
- `MAC_Active` in the enhanced table is a derived TB metric, not an internal RTL counter; it is the fairest single number we can report from external signals alone.

## Future Work

- Connect the verified APB wrapper to the KWS-SoC APB splitter.
- Wire the accelerator memory ports into a real on-chip SRAM or DMA-fed scratchpad.
- Add an input ping-pong buffer for further memory traffic reduction.
- Multi-entry bias prefetch or fused bias+weight preload, if profiling on the real bus shows a benefit.
- Synthesis run on the target FPGA/ASIC flow to characterize area/frequency.

## Final Status

All tests pass in both memory modes:

- `run-original` — **PASS**
- `run-enhanced-all-lat` (MEM_LATENCY = 1, 2, 3, nobuffer) — **PASS**
- `run-apb` (nobuffer) — **PASS**
- `run-enhanced-buffered` (MEM_LATENCY = 1, 2, 3, buffered) — **PASS**
- `run-apb-buffered` (buffered) — **PASS**
- `run-soc-wrapper` (SoC top wrapper, buffered) — **PASS**

The accelerator core, the APB control wrapper, the SoC top wrapper, the variable-latency memory verification, and the weight-stationary buffered mode are all verified in simulation. The Conv1D accelerator is packaged for KWS-SoC integration with a verified APB-facing wrapper and a documented memory-side integration path, with a 30× external-weight-traffic reduction available by setting `USE_WEIGHT_BUFFER=1` at build time.
## Kernel Size Choice

The current accelerator is specialized for `KERNEL_SIZE = 3`.

This choice was made for three reasons:

1. **Practicality and verification risk**  
   The project goal was to accelerate the dominant Conv1D bottleneck quickly and reliably. A fixed K=3 datapath allowed us to implement, optimize, and fully verify the accelerator under multiple modes: baseline, buffered, APB-controlled, and variable-latency memory.

2. **Common Conv1D/CNN usage**  
   Small kernels such as K=3 are common in convolutional neural networks because they reduce computation and parameter cost while still capturing local temporal context.

3. **Efficient hardware mapping**  
   For Conv1D, the MAC work per output is:

   ```text
   MACs per output = K × input_channels