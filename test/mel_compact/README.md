# mel_compact_4blk_ch36 — 195M Cycle Baseline

Reproduces the confirmed **195,486,216 cycle** inference result on the
Hazard3 RV32IMAC SoC Verilator simulation.

| Parameter | Value |
|-----------|-------|
| Model | mel_compact_4blk_ch36 (seed 789) |
| Accuracy | 88.7% int8 (1000 samples on Spike) |
| Flash | 26.2 KB |
| SoC clock | 36 MHz |
| MULDIV_UNROLL | 1 (iterative multiplier) |
| CSR_COUNTER | 1 (mcycle enabled) |
| Accelerator | none |
| Measured cycles | ~195,486,216 (±27 across 10 clips) |
| Wall-clock @ 36 MHz | ~5.43 s per inference |
| Test results | 8/10 correct (left→unknown, up→unknown) |

---

## Prerequisites

- **Nix devshell** — from the repo root: `nix develop` (provides Verilator, yosys, openocd)
- **RISC-V toolchain** — `riscv32-unknown-elf-gcc` must be on PATH
  - Tested with GCC 15 at `/opt/riscv/gcc15/bin/`; add to PATH or set `CROSS_COMPILE`
- **NNoM** — see note below on NNoM version

### NNoM version note

The 195M run used `/opt/nnom` installed from
[majianjia/nnom](https://github.com/majianjia/nnom) (commit `4d1d8fc`).

This branch builds against `third_party/nnom` which is the
[INITRAMFS-AUC/nnom](https://github.com/INITRAMFS-AUC/nnom) fork
(commit `733d72b`, v0.4.3-57). The fork may have minor divergence from
upstream. The NNoM layer implementations (conv2d, dense, etc.) that
matter for cycle count are expected to be equivalent, but exact binary
identity is not guaranteed.

To use the exact upstream version instead, replace `third_party/nnom`
with a checkout of `majianjia/nnom` at commit `4d1d8fc`.

---

## Steps to reproduce

### 1. Clone and initialise submodules

```bash
git clone https://github.com/INITRAMFS-AUC/KWS-SoC.git
cd KWS-SoC
git checkout nnom-195m-baseline
git submodule update --init --recursive
```

### 2. Build the firmware

```bash
cd test
make mel-compact
# Produces: test/build/mel_compact_4blk_ch36_xip.bin
cd ..
```

### 3. Build the Verilator simulation

```bash
nix develop
make build/verilator/Vkws_soc
```

### 4. Run a test clip

```bash
make sim-verilator NO_JTAG=1 \
  FLASH=test/build/mel_compact_4blk_ch36_xip.bin \
  MIC=sim/down_0000.hex
```

Available test clips in `sim/`:
`down_0000.hex`, `go_0000.hex`, `left_0000.hex`, `no_0000.hex`,
`off_0000.hex`, `on_0000.hex`, `right_0000.hex`, `stop_0000.hex`,
`up_0000.hex`, `yes_0000.hex`

### 5. Expected output

The firmware prints over UART. Look for:

```
KWS bare-metal firmware (mel_compact_4blk_ch36, NNoM int8)
Model loaded
I2S started
POLL: ... OK
DETECT:0,down
IRQS:1000
CYCLES:195486216
```

The `CYCLES:` value should be ~195,486,216 (±27 cycle variation observed
across clips due to branch alignment).

---

## Known misclassifications (8/10 correct)

| Clip | Result |
|------|--------|
| down | ✓ down |
| go | ✓ go |
| left | ✗ unknown |
| no | ✓ no |
| off | ✓ off |
| on | ✓ on |
| right | ✓ right |
| stop | ✓ stop |
| up | ✗ unknown |
| yes | ✓ yes |

`left` and `up` are misclassified as `unknown` — this is a model
accuracy issue, not a firmware or SoC bug.

---

## What was changed from the repo NNOM branch

| File | Change |
|------|--------|
| `hazard3_config.vh` | `CSR_COUNTER=1` (was 0) — enables `mcycle` CSR |
| `test/mel_compact/kws_bare.c` | `I2S_CLK_DIV=35` (was 70); L/R stereo discard in IRQ; `csrw 0x320, zero` clears mcountinhibit; `csrr mcycle` before/after `model_run()` |
| `test/mel_compact/mel_compact_4blk_ch36_weights.h` | Model weights (new file) |
| `test/common/sbrk.c` | Bare-metal syscall stubs for newlib (new file) |
| `test/Makefile` | `mel-compact` target added |
