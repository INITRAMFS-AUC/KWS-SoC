# mel_compact_4blk_ch36 — 195M Cycle Baseline

Reproduces the **~195–198M cycle** inference result on the
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
| Measured cycles (original) | 195,486,216 (±27) — using `/opt/nnom` majianjia@4d1d8fc |
| Measured cycles (this branch) | ~198,321,644 — using `nnom` INITRAMFS-AUC@733d72b |
| Wall-clock @ 36 MHz | ~5.43–5.51 s per inference |
| Test results | 8/10 correct (left→unknown, up→unknown) |

> **NNoM version note:** The original 195M measurement used `/opt/nnom` (majianjia/nnom
> commit `4d1d8fc`). This branch uses `nnom` (INITRAMFS-AUC/nnom fork,
> commit `733d72b`). The ~2.8M cycle difference (~1.4%) is due to minor divergence
> between the two NNoM versions. Layer implementations (conv2d, dense, etc.) are
> functionally equivalent; exact binary identity is not guaranteed.
> To reproduce the exact 195M figure, replace `nnom` with a checkout of
> majianjia/nnom at `4d1d8fc` (see NNoM version note in Prerequisites).

---

## Prerequisites

- **Nix devshell** — from the repo root: `nix develop` (provides Verilator, yosys, openocd)
- **RISC-V toolchain** — `riscv32-unknown-elf-gcc` must be on PATH
  - Tested with GCC 15 at `/opt/riscv/gcc15/bin/`; add to PATH or set `CROSS_COMPILE`
- **NNoM** — see note below on NNoM version

### NNoM version note

The 195M run used `/opt/nnom` installed from
[majianjia/nnom](https://github.com/majianjia/nnom) (commit `4d1d8fc`).

This branch builds against `nnom` which is the
[INITRAMFS-AUC/nnom](https://github.com/INITRAMFS-AUC/nnom) fork
(commit `733d72b`, v0.4.3-57). The fork may have minor divergence from
upstream. The NNoM layer implementations (conv2d, dense, etc.) that
matter for cycle count are expected to be equivalent, but exact binary
identity is not guaranteed.

To use the exact upstream version instead, replace `nnom`
with a checkout of `majianjia/nnom` at commit `4d1d8fc`.

### Submodule SSH workaround

The Hazard3 submodule (and its nested submodules) use SSH URLs
(`git@github.com:...`). If you do not have an SSH key configured for
GitHub, `git submodule update --init --recursive` will fail with a
permission error.

Override to HTTPS before running the recursive init:

```bash
# After cloning KWS-SoC (step 1 below), run these before step 1's submodule init:
git config submodule.Hazard3.url https://github.com/Wren6991/Hazard3.git
git config submodule.peris/i2s.url https://github.com/INITRAMFS-AUC/peris.git
git submodule update --init Hazard3 peris/i2s

# Then override Hazard3's own nested submodules:
cd Hazard3
git config submodule.example_soc/libfpga.url https://github.com/Wren6991/libfpga.git
git config submodule.scripts.url https://github.com/Wren6991/picorv32-scripts.git
git config submodule.test/formal/riscv-formal/riscv-formal.url https://github.com/YosysHQ/riscv-formal.git
git config submodule.test/sim/embench/embench-iot.url https://github.com/embench/embench-iot.git
git config submodule.test/sim/riscv-compliance/riscv-arch-test.url https://github.com/riscv-non-isa/riscv-arch-test.git
git config submodule.test/sim/riscv-tests/riscv-tests.url https://github.com/riscv-software-src/riscv-tests.git
git submodule update --init --recursive
cd ..

# Finally, init the NNoM submodule:
git submodule update --init --force nnom
```

---

## Steps to reproduce

### 1. Clone and initialise submodules

```bash
git clone https://github.com/INITRAMFS-AUC/KWS-SoC.git
cd KWS-SoC
git checkout nnom-195m-baseline
git submodule update --init --recursive
```

> **No SSH key?** See the "Submodule SSH workaround" section in Prerequisites above
> and apply the HTTPS overrides before running the recursive init.

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
| `test/common/kws_bare_main.c` | `I2S_CLK_DIV=35` (was 70); L/R stereo discard in IRQ; `csrw 0x320, zero` clears mcountinhibit; `csrr mcycle` before/after `model_run()`.  (Was `test/mel_compact/kws_bare.c` before the unified-firmware refactor.) |
| `test/model/mel_compact_4blk_ch36/mel_compact_4blk_ch36_weights.h` | Model weights (was `test/mel_compact/...` before the model-dir reorg.) |
| `test/common/sbrk.c` | Bare-metal syscall stubs for newlib (new file) |
| `test/Makefile` | `mel-compact` target added |
