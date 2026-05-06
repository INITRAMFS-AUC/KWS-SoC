# mel_compact_int8

NNoM weights for the `mel_compact_4blk_ch36` architecture quantized
end-to-end to int8 with the no-QAT float-new-noise pipeline.  Drop-in
substitute for the existing `mel_compact_4blk_ch36` build target;
same architecture, same `NNOM_STATIC_BUF_KB=36` budget.

## Files

| File                              | Role                       |
| --------------------------------- | -------------------------- |
| `mel_compact_int8_weights.h`      | NNoM-generated int8 weights, biases, and Q-format metadata. |

## Build

```
make model-mel-compact-int8           # SW baseline
make model-mel-compact-int8-accel     # HW conv1d accelerator path
```

## Cycle counts (sim, 36 MHz, no debounce)

| Variant                          | CYCLES_INFER |
| -------------------------------- | -----------: |
| `mel_compact_int8_xip`           | ~46M         |
| `mel_compact_int8_xip_accel`     | **1.08M**    |

(Same architecture as `mel_compact_4blk_ch36`; cycle counts match it.)

## Provenance

Imported from `origin/NNOM_DMA_MUL-DIV-UNROLL_CONV_R` as part of the
conv1d-accel merge.  Renamed from upstream's
`weights_no_qat_float_new_noise.h` to follow the `<name>_weights.h`
convention used elsewhere under `test/model/`.
