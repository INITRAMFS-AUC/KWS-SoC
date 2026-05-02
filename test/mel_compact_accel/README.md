# mel_compact_4blk_ch36 — Conv1D Hardware Accelerator

Reproduces the **~4.6M cycle** inference result with the Conv1D hardware
accelerator, achieving a **42× speedup** over the 195M cycle SW baseline.

| Parameter | Value |
|-----------|-------|
| Model | mel_compact_4blk_ch36 (seed 789) |
| Accuracy | 88.7% int8 (10/11 correct on SoC sim) |
| Flash (this binary) | 28.7 KB |
| SoC clock | 36 MHz |
| MULDIV_UNROLL | 1 (iterative multiplier) |
| CSR_COUNTER | 1 (mcycle enabled) |
| Accelerator | conv1d_accel at APB 0x4000\_C000, AHB master port 2 |
| SW baseline cycles | 195,486,216 (~5.43 s @ 36 MHz) |
| Accel inference cycles | ~4,638,000–4,644,000 (varies by NNoM build) |
| Speedup | **~42×** |
| Wall-clock @ 36 MHz | ~0.129 s per inference |
| Test results | 10/11 correct (up→unknown; same failure as SW baseline) |

> **Cycle count note:** The reference measurement (`~4,644,000`) was taken with the
> pre-built firmware from `kws_firmware/`. Building from `test/Makefile` via
> `make mel-compact-accel` produces `~4,638,000` due to minor LTO/linker
> differences. Both are in the same ~4.6M range and represent the same 42×
> speedup. The firmware source and model weights are identical in both cases.

---

## How the accelerator works

The Conv1D hardware accelerator (`peris/conv1d_accel/conv1d_accel.v`) is an
APB slave / AHB-Lite master that performs all five Conv2D layers in hardware.

Key design decision: **C_out-outer loop order** — weights are loaded from XIP
flash once per output channel and reused across all output positions. This
eliminates XIP cache thrashing (the root cause of the 150M+ cycle failure in an
earlier W_out-outer design).

The firmware (`nnom_conv1d_hw.c`) intercepts NNoM's `conv2d_run` via
`--wrap=conv2d_run` and delegates to the hardware when the layer is compatible
(HWC layout, int8, H=1, no dilation, C_out ≤ 64). Padding is handled in
firmware before calling the accelerator; the hardware performs valid
convolution only.

For the full design rationale, state machine, AHB timing, and performance
analysis see `peris/conv1d_accel/DESIGN.md`.

---

## Prerequisites

- **Nix devshell** — from the repo root: `nix develop`
  (provides Verilator 5.x, yosys, clang++, openocd)
- **RISC-V toolchain** — `riscv32-unknown-elf-gcc` must be on PATH
  - Tested with GCC 15 at `/opt/riscv/gcc15/bin/`
- **NNoM submodule** — `third_party/nnom` (INITRAMFS-AUC fork, commit `733d72b`)

### Clone and set up submodules

```bash
git clone <repo-url> kws_soc
cd kws_soc
git checkout nnom-conv1d-accel
```

All submodule URLs in this branch use HTTPS (no SSH key required):

```bash
git submodule update --init Hazard3 peris/i2s
cd Hazard3
git submodule update --init --recursive
cd ..
git submodule update --init --force third_party/nnom
```

---

## Build

### 1. Build the Verilator simulation

From the repo root with the Nix devshell active:

```bash
make sim_verilator
```

This produces `build/verilator/Vkws_soc`.

### 2. Build the accelerated firmware

```bash
cd test/
make mel-compact-accel
# Produces: test/build/mel_compact_4blk_ch36_xip_accel.bin (~28.7 KB)
```

To also build the SW-only baseline for comparison:

```bash
make mel-compact
# Produces: test/build/mel_compact_4blk_ch36_xip.bin
```

---

## Run

### Single clip

```bash
# From kws_soc/ root:
NO_JTAG=1 make sim-verilator \
  FLASH=test/build/mel_compact_4blk_ch36_xip_accel.bin \
  MIC=sim/yes_0000.hex \
  CYCLES=60000000
```

Expected UART output:
```
KWS bare-metal firmware (mel_compact_4blk_ch36, NNoM int8)
Model loaded
I2S started
DETECT:9,yes
CYCLES:4638161
```

The `CYCLES:` value is measured by `csrr mcycle` around `model_run()` in
firmware — it reflects the real cycle-accurate RTL simulation. Audio
collection (~36M cycles) is **not** included in this number.

### All 11 keywords — batch accuracy and cycle report

```bash
bash scripts/run_soc_batch.sh \
  --flash test/build/mel_compact_4blk_ch36_xip_accel.bin \
  --audio-dir sim \
  --max-cycles 60000000 \
  --label accel
```

Expected output (~9 minutes wall-clock):
```
  down     → down       ✓  [~4638000 cycles]
  go       → go         ✓  [~4638000 cycles]
  left     → left       ✓  [~4638000 cycles]
  ...
  up       → unknown    ✗  [~4638000 cycles]
  yes      → yes        ✓  [~4638000 cycles]

  ACCURACY: 10 / 11  (90.9%)
  avg: ~4638000 cycles  (~0.129 s @ 36 MHz)
```

### SW-only baseline for comparison

```bash
bash scripts/run_soc_batch.sh \
  --flash test/build/mel_compact_4blk_ch36_xip.bin \
  --audio-dir sim \
  --max-cycles 250000000 \
  --label sw_only
```

Expected: 10/11 correct, ~195M cycles per inference (~90 minutes wall-clock).

---

## Audio files

`sim/` contains one hex file per keyword class (11 total):

```
sim/down_0000.hex   sim/go_0000.hex    sim/left_0000.hex
sim/no_0000.hex     sim/off_0000.hex   sim/on_0000.hex
sim/right_0000.hex  sim/stop_0000.hex  sim/unknown_0000.hex
sim/up_0000.hex     sim/yes_0000.hex
```

Each file is a raw int8 Q7 audio clip (8000 samples) converted to 32-bit
I2S words (`word = (int32_t)q7 << 16`). These are the `_0000` (first)
example of each class from the Google Speech Commands v0.02 test split.

The `up` clip is misclassified as `unknown` in both SW and accel modes —
this is a genuine model limitation, not an accelerator bug.

---

## File map

| File | Purpose |
|------|---------|
| `peris/conv1d_accel/conv1d_accel.v` | Accelerator RTL (APB slave + AHB-Lite master) |
| `peris/conv1d_accel/DESIGN.md` | Full design doc: architecture, state machine, performance analysis |
| `kws_soc.v` | Top-level SoC: accel integrated into AHB crossbar + APB splitter |
| `soc.f` | File list: adds `peris/conv1d_accel/conv1d_accel.v` |
| `test/mel_compact_accel/kws_bare.c` | Bare-metal firmware: I2S audio → model_run → UART output |
| `test/mel_compact_accel/mel_compact_4blk_ch36_weights.h` | NNoM int8 model weights |
| `test/mel_compact_accel/nnom_conv1d_hw.c` | NNoM conv2d_run replacement that calls the HW accel |
| `test/mel_compact_accel/accel_conv1d.h` | Hardware driver: APB register writes + DONE polling |
| `test/Makefile` | Build targets: `mel-compact-accel` and `mel-compact` (SW baseline) |
| `scripts/run_soc_batch.sh` | Batch test: runs all 11 clips, reports accuracy + cycle counts |
| `sim/*_0000.hex` | I2S audio stimulus for all 11 keyword classes |
