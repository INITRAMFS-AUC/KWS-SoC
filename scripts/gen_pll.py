#!/usr/bin/env python3
"""
Generate a Quartus ALTPLL wrapper for any integer output frequency.

Strategy: patch the four synthesis-critical defparam values in
quartus/ip/ALTPLL_25/ALTPLL_25.v (wizard-generated, proven base) and
write the result as quartus/ip/clock_pll_gen/clock_pll_gen.v.
Everything else (port wiring, unused-port declarations, retrieval info)
is left byte-for-byte identical to the wizard output.

Usage:
    python3 scripts/gen_pll.py --clk-mhz 36 --device-family "Cyclone V"
"""

import argparse
import math
import os
import re
import sys

SCRIPT_DIR   = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT    = os.path.dirname(SCRIPT_DIR)
TEMPLATE_V   = os.path.join(REPO_ROOT, "quartus/ip/ALTPLL_25/ALTPLL_25.v")
TEMPLATE_BB  = os.path.join(REPO_ROOT, "quartus/ip/ALTPLL_25/ALTPLL_25_bb.v")
OUT_DIR      = os.path.join(REPO_ROOT, "quartus/ip/clock_pll_gen")
MODULE_NAME  = "clock_pll_gen"
REF_MHZ      = 50


def compute_mc(clk_mhz):
    """Return (M, C) such that REF_MHZ * M / C == clk_mhz, in lowest terms."""
    g = math.gcd(clk_mhz, REF_MHZ)
    return clk_mhz // g, REF_MHZ // g


def patch(text, old_name, new_name, m, c, device_family, freq_mhz):
    """Apply all substitutions to the template text."""

    # --- Verilog body (synthesis-critical) ---
    text = re.sub(r'\bmodule\s+' + re.escape(old_name) + r'\b',
                  f'module {new_name}', text)
    text = re.sub(r'(altpll_component\.clk0_divide_by\s*=\s*)\d+',
                  rf'\g<1>{c}', text)
    text = re.sub(r'(altpll_component\.clk0_multiply_by\s*=\s*)\d+',
                  rf'\g<1>{m}', text)
    text = re.sub(r'(altpll_component\.intended_device_family\s*=\s*)"[^"]*"',
                  rf'\g<1>"{device_family}"', text)
    text = re.sub(r'(altpll_component\.lpm_hint\s*=\s*"CBX_MODULE_PREFIX=)' + re.escape(old_name) + r'"',
                  rf'\g<1>{new_name}"', text)

    # --- Retrieval-info comments (wizard bookkeeping only) ---
    text = re.sub(r'(CONSTANT: CLK0_DIVIDE_BY NUMERIC )"[^"]*"',
                  rf'\g<1>"{c}"', text)
    text = re.sub(r'(CONSTANT: CLK0_MULTIPLY_BY NUMERIC )"[^"]*"',
                  rf'\g<1>"{m}"', text)
    text = re.sub(r'(CONSTANT: INTENDED_DEVICE_FAMILY STRING )"[^"]*"',
                  rf'\g<1>"{device_family}"', text)
    text = re.sub(r'(PRIVATE: INTENDED_DEVICE_FAMILY STRING )"[^"]*"',
                  rf'\g<1>"{device_family}"', text)
    text = re.sub(r'(PRIVATE: OUTPUT_FREQ0 STRING )"[^"]*"',
                  rf'\g<1>"{freq_mhz:.8f}"', text)
    text = re.sub(r'(PRIVATE: EFF_OUTPUT_FREQ_VALUE0 STRING )"[^"]*"',
                  rf'\g<1>"{freq_mhz:.6f}"', text)

    # Rename all remaining old_name occurrences (GEN_FILE lines, file header comment, etc.)
    text = text.replace(old_name, new_name)

    return text


def write_qip(out_dir, module_name, device_family):
    qip_path = os.path.join(out_dir, f"{module_name}.qip")
    with open(qip_path, "w") as fh:
        fh.write(f'set_global_assignment -name IP_TOOL_NAME "ALTPLL"\n')
        fh.write(f'set_global_assignment -name IP_TOOL_VERSION "25.1"\n')
        fh.write(f'set_global_assignment -name IP_GENERATED_DEVICE_FAMILY "{{{device_family}}}"\n')
        fh.write(f'set_global_assignment -name VERILOG_FILE [file join $::quartus(qip_path) "{module_name}.v"]\n')
        fh.write(f'set_global_assignment -name MISC_FILE [file join $::quartus(qip_path) "{module_name}_bb.v"]\n')


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--clk-mhz",      type=int,   required=True,
                        help="Target output frequency in MHz (integer)")
    parser.add_argument("--device-family", default="Cyclone V",
                        help="Intel FPGA device family (default: 'Cyclone V')")
    args = parser.parse_args()

    clk_mhz = args.clk_mhz
    m, c = compute_mc(clk_mhz)
    f_check = REF_MHZ * m / c
    if abs(f_check - clk_mhz) > 0.001:
        sys.exit(f"ERROR: {clk_mhz} MHz is not exactly achievable from {REF_MHZ} MHz as an integer ratio")

    os.makedirs(OUT_DIR, exist_ok=True)

    old_name = "ALTPLL_25"

    for src, dst_name in [(TEMPLATE_V,  f"{MODULE_NAME}.v"),
                          (TEMPLATE_BB, f"{MODULE_NAME}_bb.v")]:
        with open(src) as fh:
            text = fh.read()
        text = patch(text, old_name, MODULE_NAME, m, c, args.device_family, float(clk_mhz))
        dst = os.path.join(OUT_DIR, dst_name)
        with open(dst, "w") as fh:
            fh.write(text)

    write_qip(OUT_DIR, MODULE_NAME, args.device_family)

    print(f"Generated {clk_mhz} MHz PLL (M={m}, C={c}, family={args.device_family!r}) → {OUT_DIR}/")


if __name__ == "__main__":
    main()
