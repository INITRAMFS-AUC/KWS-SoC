#!/usr/bin/env python3
"""Realign legacy `q7 << 16` I2S sim stimuli to the protocol-correct
`q7 << 23` placement.

Background
----------
A `sim/<key>_0000.hex` line is one 32-bit I2S frame: the bits are emitted
MSB-first across 32 SCK cycles, and the I2S RX core (after commit ee863e3
in peris/i2s) interprets the frame per the INMP441 protocol:

    bit 31         high-Z (1 SCK cycle, dropped)
    bits 30 .. 7   24 audio bits, MSB-first  → captured into shifter[23:0]
    bits  6 .. 0   high-Z (7 trailing SCK cycles, dropped)

Legacy stimuli were authored when the RX captured all 32 bits verbatim,
so the q7 (8-bit Q7 fixed-point) sample was placed at hex bits [23:16]
via `(int32_t)q7 << 16`.  Under the protocol-correct RX, that position
maps to audio[16:9] — i.e., the *middle* of the 24-bit audio window —
not the byte the firmware reads with `(int8_t)(fifo >> 16)`.

This script rewrites each line so the q7 lands at hex bits [30:23]
(audio[23:16]), the byte the model is trained on:

    new = ((old >> 16) & 0xFF) << 23

Z-bit positions (hex[31] and hex[6:0]) are written as zero.

Usage
-----
    python3 scripts/realign_q7_hex.py sim/down_0000.hex
    python3 scripts/realign_q7_hex.py sim/down_0000.hex sim/down_0000_p.hex
    python3 scripts/realign_q7_hex.py --in-place sim/*.hex
"""

import argparse
import pathlib
import sys


def realign_word(old: int) -> int:
    """Take a legacy `q7 << 16` 32-bit word and re-place q7 at bits [30:23].

    The Z-bit positions (hex[31] and hex[6:0]) are zeroed; the receiver
    ignores them anyway.  Sign-extension above hex[30] is intentionally
    dropped — it's a Z bit per the I2S protocol.
    """
    q7 = (old >> 16) & 0xFF
    return (q7 << 23) & 0xFFFFFFFF


def realign_file(in_path: pathlib.Path, out_path: pathlib.Path) -> int:
    converted = 0
    with in_path.open("r") as fin, out_path.open("w") as fout:
        for raw in fin:
            line = raw.rstrip("\n")
            stripped = line.strip()
            if not stripped:
                fout.write(line + "\n")
                continue
            if stripped.startswith(("#", "/")):
                # Update the format banner if we recognise it
                if "q7 << 16" in line:
                    line = (line.replace("q7 << 16", "q7 << 23")
                                .replace("Q7 left-aligned in 32-bit I2S word",
                                         "Q7 left-aligned in 24-bit audio (bits [30:23] of I2S frame)"))
                fout.write(line + "\n")
                continue
            try:
                old = int(stripped, 16)
            except ValueError:
                fout.write(line + "\n")
                continue
            fout.write(f"{realign_word(old):08X}\n")
            converted += 1
    return converted


def main(argv):
    p = argparse.ArgumentParser(description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--in-place", action="store_true",
                   help="rewrite files in place (otherwise writes _p.hex sibling)")
    p.add_argument("paths", nargs="+", type=pathlib.Path)
    args = p.parse_args(argv[1:])

    for path in args.paths:
        if not path.exists():
            print(f"warning: skipping missing {path}", file=sys.stderr)
            continue
        out = path if args.in_place else path.with_name(path.stem + "_p" + path.suffix)
        if out.resolve() == path.resolve() and not args.in_place:
            print(f"error: {out} would clobber {path}; use --in-place", file=sys.stderr)
            return 1
        n = realign_file(path, out)
        print(f"{path} -> {out}: {n} samples realigned")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
