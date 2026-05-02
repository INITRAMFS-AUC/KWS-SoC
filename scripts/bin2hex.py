#!/usr/bin/env python3
"""Convert a raw RV32 .bin (little-endian) into a $readmemh-compatible
hex file: one 32-bit word per line, lowercase hex, no addresses or
comments.  Used by the SRAM_PRELOAD make knob to feed sram_sync."""
import struct
import sys

if len(sys.argv) != 3:
    sys.stderr.write(f"usage: {sys.argv[0]} input.bin output.hex\n")
    sys.exit(1)

with open(sys.argv[1], "rb") as f:
    data = f.read()
# pad up to a 4-byte boundary so the last word is whole
if len(data) % 4:
    data += b"\x00" * (4 - len(data) % 4)
with open(sys.argv[2], "w") as out:
    for i in range(0, len(data), 4):
        word = struct.unpack_from("<I", data, i)[0]
        out.write(f"{word:08x}\n")
print(f"  wrote {len(data) // 4} words to {sys.argv[2]}")
