#!/usr/bin/env python3
"""
Convert an I2S audio hex file into a C array for XIP sample playback.

Usage: python3 hex_to_c_array.py <input.hex> <output.c>

The hex file format is the same as sim/down_0000.hex — one 32-bit hex
word per line, with optional comment lines starting with #.

The output C file defines playback_samples[] in the .playback_samples
linker section, which the xip_sample_player reads from 0x8001_0000.
"""

import sys


def convert(hex_path, c_path):
    samples = []
    with open(hex_path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            val = int(line, 16)
            samples.append(val)

    if not samples:
        print("ERROR: no samples found in", hex_path, file=sys.stderr)
        sys.exit(1)

    with open(c_path, "w") as f:
        f.write(
            "/* Auto-generated playback samples — DO NOT EDIT */\n"
            "/* Source: {} */\n"
            "/* {} samples, {} bytes */\n"
            '\n'
            '#include <stdint.h>\n'
            '\n'
            '__attribute__((section(".playback_samples"), aligned(4)))\n'
            "const uint32_t playback_samples[{}] = {{\n".format(
                hex_path, len(samples), len(samples) * 4, len(samples)
            )
        )
        for i, val in enumerate(samples):
            f.write("    0x{:08X},\n".format(val))
        f.write("};\n")

    print(
        "hex_to_c_array: {} samples → {} ({})".format(
            len(samples), c_path, hex_path
        )
    )


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: {} <input.hex> <output.c>".format(sys.argv[0]))
        sys.exit(1)
    convert(sys.argv[1], sys.argv[2])
