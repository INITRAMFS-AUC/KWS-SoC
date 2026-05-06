# Conv1D Accelerator

Hardware accelerator for the int8 Conv1D layers in the KWS NNoM models.
Replaces NNoM's software `conv2d_run` for layers that meet the eligibility
criteria; falls back to the SW path otherwise.  Targets
`mel_compact_4blk_ch36`, `mel_compact_int8`, `mel_compact_int8_peak_norm`,
and `kws_lr` end-to-end via `make model-<name>-accel`.

## Files

| File                              | Role                                                      |
| --------------------------------- | --------------------------------------------------------- |
| [`DESIGN.md`](DESIGN.md)          | Full design write-up: register map, loop order, cycle accounting, fallback rules. Read this first. |
| `conv1d_accel.v`                  | RTL.  APB slave for control + AHB-Lite master for DMA.    |
| `dv/Makefile`                     | Verilator build for the standalone testbench.             |
| `dv/conv1d_accel_tb.v`            | Self-checking testbench (10 cases, randomized).           |
| `dv/tb_utils.vh`                  | Common TB macros (clock, reset, dump, finish).            |

## Quick start

Run the standalone testbench:

```
cd peris/conv1d_accel/dv && make sim
```

Expected: `ALL TESTS PASSED` (10 cases).

End-to-end run with the accelerated firmware against a 1-second clip:

```
make sim-verilator NO_JTAG=1 USE_MCYCLE_CSR=1 \
     FLASH=test/build/mel_compact_int8_xip_accel.bin \
     MIC=sim/down_0000_ds.hex EXTRA_ARGS="--cycles 80000000"
```

Look for `CYCLES_INFER:` near `1.08M` (vs ~46M for the SW baseline).

## Register map (APB base 0x4000_C000)

| Offset | Name        | Access | Bits / Notes                                                            |
| -----: | ----------- | ------ | ----------------------------------------------------------------------- |
| 0x00   | CTRL        | RW/RO  | [0]=start (W), [8]=busy (R), [9]=done (R)                               |
| 0x04   | SRC_ADDR    | RW     | Input feature-map address (SRAM)                                        |
| 0x08   | WT_ADDR     | RW     | Weight array address (SRAM or XIP flash)                                |
| 0x0C   | DST_ADDR    | RW     | Output feature-map address (SRAM)                                       |
| 0x10   | BS_ADDR     | RW     | Bias array address (int32, firmware pre-shifted)                        |
| 0x14   | CFG0        | RW     | [7:0]=C_in [15:8]=C_out [23:16]=K_w [31:24]=stride                      |
| 0x18   | CFG1        | RW     | [15:0]=W_in (W_out derived: (W_in − K_w) / stride + 1)                  |
| 0x1C   | SHIFT       | RW     | [4:0]=scalar output right-shift (legacy, used when SHIFT_ADDR=0)        |
| 0x20   | SHIFT_ADDR  | RW     | Per-channel shift byte array address (uint8_t [C_out])                  |

Firmware writes the addresses + CFG0/1 + SHIFT(_ADDR), pulses CTRL.start,
then polls CTRL.done.  See `test/common/accel_conv1d.h` for the inline
driver.

## Eligibility (when the HW path fires)

The shim in `test/common/nnom_conv1d_hw.c` falls back to the canonical SW
`conv2d` for any layer that violates these:

- HWC tensor layout, int8 input
- H = 1 (flat 1-D conv embedded in Conv2D shape)
- No dilation
- C_out ≤ 64 (matches `wt_buf` depth)
- PADDING_SAME handled in firmware: pre-pad input + zero-tail into a
  scratch buffer before calling `accel_conv1d()`.

## Known limitations

- **Input-misalignment regression marker (TC1, TC2)**.  When
  `stride × C_in` is not a multiple of 4, HSIZE_WORD reads cross word
  boundaries incorrectly.  `test/accel/accel_test.c` keeps two test
  cases that trip this so it doesn't silently re-emerge; production
  models all use C_in = 4 layers and aren't affected.

- **Per-channel SHIFT_ADDR is wired but unused** by every shipping
  model — exercise with a per-channel-quantized model before
  declaring the path validated.

- **Gate optimisations not yet applied** (4-MAC switching every cycle
  in S_MAC, weight-buffer write-enable always asserted).  Follow-up
  in `docs/TODO.md`.

## Provenance

Merged from `origin/NNOM_DMA_MUL-DIV-UNROLL_CONV_R` (RTL + TB) and
`origin/nnom-conv1d-accel` (DESIGN.md) on branch `merge-conv1d-accel`.
The two branches were independent attempts at the same problem; the
ping-pong + word-weight-burst optimisations from the CONV_R "better
accelerator" commit (d7df479) are the load-bearing perf wins.
