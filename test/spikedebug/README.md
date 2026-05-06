# Spike activation debug harness

Self-contained tool for layer-by-layer comparison between the int8
peak-norm KWS model running on **Spike** (the RISC-V ISS) and the same
model running on the SoC RTL.  Used to localise audio-pipeline / model
divergence to a specific layer rather than guessing from a final
classification mismatch.

Imported from `origin/NNOM_DMA_MUL-DIV-UNROLL_CONV_R` SHA `1e99305`.

## Files

| File                          | Role                                                                |
| ----------------------------- | ------------------------------------------------------------------- |
| `kws_spike_debug.c`           | Standalone NNoM harness — runs inference and dumps every layer's raw int8 output as hex (one byte per line) via NNoM layer callback. |
| `weights_int8norm.h`          | Same int8 peak-norm weights as `test/model/mel_compact_int8_peak_norm/`. |
| `down_audio_8k_spike.bin`     | Real INMP441 'Barbary' recording of the word "down" (8000 int8 Q7 samples) in batched format: `int32 n_clips`, `uint8 labels[128]`, `int8 audio[128*8000]`.  Only clip 0 is used. |
| `down_audio_16k.hex`          | Same audio as `..._spike.bin` but reformatted for the I2S sim mic (16000 line-per-frame hex, MSB-first SD-pattern). |
| `activations.log`             | Reference Spike output: 14 layers + `PRED:4`.  Re-run to regenerate; diff against hardware-sim layer dumps. |
| `build.sh`                    | Build script — uses `/opt/riscv/gcc15/bin/riscv32-unknown-elf-gcc` and `/opt/riscv/bin/spike` + the proxy kernel.  Edit the `NNOM` path to point at your local nnom checkout. |

## Quick start

```sh
bash build.sh
/opt/riscv/bin/spike --isa=rv32imac_zicsr_zifencei -m256 \
    /opt/riscv/riscv32-unknown-elf/bin/pk kws_spike_debug \
    | tee activations.log
```

The output format per layer is:

```
LAYER_<n> type=<name> shape=[d0,d1,...] q_dec=<val> n=<total_bytes>
<hex byte>            ← one per line, all output elements of that layer
...
PRED:<class_idx>
```

`q_dec` is the Q-format fractional bits (`value = int8 / 2^q_dec`).

## Why it's here

The conv1d-accel merge currently classifies every clip as `unknown`
because the existing `sim/<key>_0000.hex` test stimuli put the q7
sample at hex bits [23:16], but the protocol-correct `i2s_rx_core`
captures audio from hex bits [30:7] (skips the leading Z SCK cycle).
The two are off by 7 bit positions.  Diffing per-layer activations
between Spike (known-good) and the SoC RTL using this harness will
either confirm it's a stimulus-alignment issue or surface a different
root cause.  Spike's reference output (PRED:4 = "off" — though this
should be PRED:0 "down" for the down clip; the recording's actual
ground truth is captured in `activations.log`'s preamble).

## Provenance & open questions

- Imported from CONV_R; original audio is real INMP441 from the
  Barbary recording session (per the upstream commit message).
- The `down_audio_*` pair gives us a real-mic stimulus to validate
  against; once the audio path passes Spike-vs-RTL byte-for-byte at
  the first layer, the rest of the model should match.
