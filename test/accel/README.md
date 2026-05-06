# Standalone Conv1D Accelerator Test

Pure-SRAM unit test for `peris/conv1d_accel/conv1d_accel.v` — no I2S, no
NNoM, no DMA.  Programs the accelerator with hand-rolled input / weight
buffers via the inline driver in `test/common/accel_conv1d.h`, polls
`done`, and compares the output against a SW reference.

## Build and run

```
make test
make sim-verilator NO_JTAG=1 \
     FLASH=test/build/accel_test_xip.bin \
     MIC=sim/down_0000_ds.hex \
     EXTRA_ARGS="--cycles 30000000"
```

Expected output:

```
ACCEL_TEST start
FAIL TC1 out[2]=14 exp=20
FAIL TC2 out[1]=-4 exp=3
PASS TC3
ACCEL_TEST done
```

## What each case covers

| Case | C_in | C_out | K_w | stride | W_in | Status                                                     |
| ---: | ---: | ----: | --: | -----: | ---: | ---------------------------------------------------------- |
| TC1  | 1    | 2     | 3   | 1      | 5    | **FAIL** — known-failing input-misalignment regression marker |
| TC2  | 1    | 1     | 65  | 1      | 68   | **FAIL** — same root cause as TC1                          |
| TC3  | 4    | 4     | 3   | 1      | 6    | **PASS** — exercises the production C_in=4 path             |

TC1 / TC2 fail because the accelerator issues HSIZE_WORD reads on
input addresses that aren't 4-aligned when `stride × C_in` is not a
multiple of 4.  All shipping KWS models use C_in = 4 layers, so this
is latent in production today — the failing cases are kept here so a
future fix has an obvious regression target.  See the entry in
`docs/TODO.md` and the limitation note in `peris/conv1d_accel/README.md`.

## Why it lives outside the model build

The standalone test is a `--cycles 30M` smoke check that the
accelerator's MAC + bias + shift + write path works end-to-end on
known-good inputs.  Failures here narrow the bug surface to the RTL
or the firmware driver before the much heavier model runs.  Run this
first when bisecting an inference-side regression.
