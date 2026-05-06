#!/usr/bin/env python3
"""
Convert a Spike-format batched int8 audio dataset into a single-clip C array
that the dump firmware (test/spikedebug/kws_spike_repro.c) can link in.

Layout of the input (matches what test/spikedebug/kws_spike_debug.c reads):
  bytes  0 ..  3   int32  n_clips        (LE)
  bytes  4 .. 131  uint8  labels[128]
  bytes 132 ..     int8   audio[n_clips * 8000]

We extract clip 0 (first 8000 int8 samples after the header) and emit:

  /* Auto-generated from <bin> (clip 0) */
  #include <stdint.h>
  const int8_t spike_clip[8000] = { ... };

Usage:
  python3 spike_bin_to_c_array.py <input.bin> <output.c> [clip_idx]

clip_idx defaults to 0 — pass another integer to dump a different clip from
the same dataset without re-recording.
"""

import os
import sys

HEADER_BYTES   = 4 + 128            # int32 n_clips + uint8 labels[128]
SAMPLES_PER_CLIP = 8000


def convert(bin_path: str, c_path: str, clip_idx: int = 0) -> None:
    if not os.path.isfile(bin_path):
        print(f"ERROR: {bin_path} not found", file=sys.stderr)
        sys.exit(1)

    with open(bin_path, "rb") as f:
        f.seek(HEADER_BYTES + clip_idx * SAMPLES_PER_CLIP)
        raw = f.read(SAMPLES_PER_CLIP)

    if len(raw) != SAMPLES_PER_CLIP:
        print(
            f"ERROR: short read at clip {clip_idx} "
            f"(got {len(raw)} bytes, need {SAMPLES_PER_CLIP})",
            file=sys.stderr,
        )
        sys.exit(1)

    # Reinterpret unsigned bytes as int8.
    samples = [b - 256 if b > 127 else b for b in raw]

    with open(c_path, "w") as out:
        out.write(
            "/* Auto-generated from {bin} (clip {idx}) — DO NOT EDIT */\n"
            "#include <stdint.h>\n"
            "\n"
            "const int8_t spike_clip[{n}] = {{\n".format(
                bin=os.path.basename(bin_path), idx=clip_idx, n=SAMPLES_PER_CLIP
            )
        )
        # 16 samples per line keeps the file readable without wasting space.
        for i in range(0, SAMPLES_PER_CLIP, 16):
            chunk = samples[i : i + 16]
            out.write("    " + ", ".join(f"{v:4d}" for v in chunk) + ",\n")
        out.write("};\n")

    print(f"spike_bin_to_c_array: clip {clip_idx} ({SAMPLES_PER_CLIP} samples) → {c_path}")


if __name__ == "__main__":
    if len(sys.argv) not in (3, 4):
        print(f"Usage: {sys.argv[0]} <input.bin> <output.c> [clip_idx]", file=sys.stderr)
        sys.exit(1)
    idx = int(sys.argv[3]) if len(sys.argv) == 4 else 0
    convert(sys.argv[1], sys.argv[2], idx)
