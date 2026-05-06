#!/usr/bin/env python3
"""Double an I2S mic hex stimulus by interleaving a zero word between each sample.

Use this when the SoC is configured with cfg_div=17 + I2S_CONF_DS_EN=1, which
keeps the firmware-visible sample rate at 8 kHz but doubles the SCK rate (and
therefore the mic-feeder consumption rate in sim/i2s_mic_sim.cpp).  The DS_EN
hardware drops every *second* left-channel sample, so to feed the same audio
content into the FIFO we interleave a zero between each real sample:

    input :  s0      s1      s2      s3   ...
    output:  s0   0  s1   0  s2   0  s3   ...

After DS_EN drops every other sample the firmware captures s0, s1, s2, s3, ...
identically to the cfg_div=35 path, just over the same wall-clock seconds.

Usage:
    python3 scripts/double_hex.py sim/down_0000.hex sim/down_0000_ds.hex
    python3 scripts/double_hex.py sim/down_0000.hex            # writes _ds.hex sibling
"""

import sys
import pathlib


def double_hex(in_path: pathlib.Path, out_path: pathlib.Path) -> int:
    """Write `out_path` = each non-blank/non-comment line of `in_path` followed
    by a zero word.  Comments and blank lines are preserved unchanged.
    Returns the number of real samples written (= half the data lines)."""
    written = 0
    with in_path.open("r") as fin, out_path.open("w") as fout:
        for raw in fin:
            line = raw.rstrip("\n")
            stripped = line.strip()
            if not stripped or stripped.startswith(("#", "/")):
                # Pass through comments / blank lines verbatim
                fout.write(line + "\n")
                continue
            fout.write(line + "\n")
            fout.write("00000000\n")
            written += 1
    return written


def main(argv):
    if len(argv) not in (2, 3):
        print(__doc__, file=sys.stderr)
        return 2
    in_path = pathlib.Path(argv[1])
    if not in_path.exists():
        print(f"error: input not found: {in_path}", file=sys.stderr)
        return 1
    if len(argv) == 3:
        out_path = pathlib.Path(argv[2])
    else:
        out_path = in_path.with_name(in_path.stem + "_ds" + in_path.suffix)
    if out_path.resolve() == in_path.resolve():
        print("error: input and output paths must differ", file=sys.stderr)
        return 1
    n = double_hex(in_path, out_path)
    print(f"{in_path} -> {out_path}: {n} real samples (×2 with interleaved zeros)")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
