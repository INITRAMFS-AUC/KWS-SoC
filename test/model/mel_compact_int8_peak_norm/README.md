# mel_compact_int8_peak_norm

NNoM weights for the `mel_compact_4blk_ch36` architecture, fine-tuned
on **Barbary INMP441 mic data** using the int8 peak-normalisation
pipeline (`>> 8` then scale to int8 range).  Same architecture and
same `NNOM_STATIC_BUF_KB=36` budget as the other `mel_compact_*`
variants — the only difference is the trained weight values.

## Files

| File                                       | Role                       |
| ------------------------------------------ | -------------------------- |
| `mel_compact_int8_peak_norm_weights.h`     | NNoM-generated int8 weights for the peak-normalised pipeline. |

## Reported accuracy

| Eval set            | Accuracy                                                       |
| ------------------- | -------------------------------------------------------------- |
| Barbary mic data    | 95 %                                                           |
| GSCD Spike eval     | 80.7 %                                                         |

Numbers from the upstream commit message
(`origin/NNOM_DMA_MUL-DIV-UNROLL_CONV_R` SHA `e812f19`); not yet
re-verified post-merge.

## Build

A build target hasn't been wired into `test/Makefile` yet — the
weights file is staged here so the `_weights.h` header is in tree.
To build, copy the existing `mel_compact_int8` rule and point
`KWS_WEIGHTS_HEADER` at this file:

```make
MEL_INT8_PN_CFLAGS = $(MODEL_CFLAGS) $(KWS_EXTRA_CFLAGS) \
                     -DNNOM_STATIC_BUF_KB=36 \
                     -DKWS_WEIGHTS_HEADER='"mel_compact_int8_peak_norm_weights.h"' \
                     -DKWS_MODEL_NAME='"mel_compact_int8_peak_norm"' \
                     -I$(MODEL_DIR)/mel_compact_int8_peak_norm
```

## Provenance

Imported from `origin/NNOM_DMA_MUL-DIV-UNROLL_CONV_R` SHA `e812f19`
("Add int8 peak-norm KWS model weights").  Renamed from upstream's
`weights_int8norm.h` to follow the `<name>_weights.h` convention.
