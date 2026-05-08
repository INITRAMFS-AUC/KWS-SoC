#!/usr/bin/env bash
# Generate a Cyclone V fractional-N PLL via the Quartus CLI (qsys-script
# + qsys-generate).  Use this when the integer-N gen_pll.py can't hit the
# target frequency exactly — e.g. 36.864 MHz, which would land cfg_div=17
# at exactly 16,000 Hz raw I2S frame rate.
#
# Usage:
#     scripts/gen_pll_frac.sh <CLK_MHZ_FLOAT> [DEVICE_FAMILY] [DEVICE_PART]
#
# Example:
#     scripts/gen_pll_frac.sh 36.864
#     scripts/gen_pll_frac.sh 49.152 "Cyclone V" 5CSXFC6D6F31C6
#
# Output: quartus/ip/clock_pll_gen_frac/clock_pll_gen.{qsys,qip,v}
#
# Requires Quartus 25.1+ (qsys-script and qsys-generate under
# $QUARTUS_ROOTDIR/sopc_builder/bin/).
set -euo pipefail

CLK_MHZ=${1:-}
DEVICE_FAMILY=${2:-"Cyclone V"}
DEVICE_PART=${3:-"5CSXFC6D6F31C6"}

if [[ -z "$CLK_MHZ" ]]; then
    echo "usage: $0 <CLK_MHZ_FLOAT> [DEVICE_FAMILY] [DEVICE_PART]" >&2
    exit 2
fi

# Locate qsys-script / qsys-generate.  Quartus puts them under
# sopc_builder/bin (separate from the main quartus/bin used by the rest
# of the flow), so PATH-based which often misses them.
QUARTUS_ROOTDIR=${QUARTUS_ROOTDIR:-/home/public/Quartus-Installed/quartus}
QSYS_BIN="$QUARTUS_ROOTDIR/sopc_builder/bin"
if [[ ! -x "$QSYS_BIN/qsys-script" ]]; then
    echo "ERROR: qsys-script not found under $QSYS_BIN." >&2
    echo "       Set QUARTUS_ROOTDIR to your Quartus installation root." >&2
    exit 1
fi

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT_DIR="$ROOT_DIR/quartus/ip/clock_pll_gen_frac"
WORK_DIR=$(mktemp -d)
trap 'rm -rf "$WORK_DIR"' EXIT

echo "[gen_pll_frac] CLK_MHZ=$CLK_MHZ FAMILY='$DEVICE_FAMILY' PART=$DEVICE_PART"
echo "[gen_pll_frac] work=$WORK_DIR  out=$OUT_DIR"

# Tcl script: build a one-instance Qsys system around altera_pll in
# fractional-N mode.  Module is named clock_pll_gen so the rest of the
# repo can drop-in replace ALTPLL_25.
cat > "$WORK_DIR/gen.tcl" <<EOF
package require -exact qsys 25.1
create_system clock_pll_gen
set_project_property DEVICE_FAMILY  {$DEVICE_FAMILY}
set_project_property DEVICE         {$DEVICE_PART}

add_instance pll altera_pll
set_instance_parameter_value pll gui_reference_clock_frequency  50.0
set_instance_parameter_value pll gui_pll_mode                   {Fractional-N PLL}
set_instance_parameter_value pll gui_number_of_clocks           1
set_instance_parameter_value pll gui_operation_mode             direct
set_instance_parameter_value pll gui_use_locked                 1
set_instance_parameter_value pll gui_active_clk                 0
set_instance_parameter_value pll gui_output_clock_frequency0    $CLK_MHZ

add_interface refclk     clock sink
add_interface refclk_rst reset sink
add_interface outclk0    clock source
add_interface locked     conduit end

set_interface_property refclk     EXPORT_OF pll.refclk
set_interface_property refclk_rst EXPORT_OF pll.reset
set_interface_property outclk0    EXPORT_OF pll.outclk0
set_interface_property locked     EXPORT_OF pll.locked

save_system clock_pll_gen.qsys
EOF

# Run qsys-script from inside WORK_DIR so the .qsys lands there.
( cd "$WORK_DIR" && "$QSYS_BIN/qsys-script" --script=gen.tcl )

# Generate synthesis HDL.
"$QSYS_BIN/qsys-generate" "$WORK_DIR/clock_pll_gen.qsys" \
    --synthesis=VERILOG \
    --output-directory="$WORK_DIR/out" \
    --family="$DEVICE_FAMILY" \
    --part="$DEVICE_PART" \
    > "$WORK_DIR/qsys-gen.log" 2>&1 || {
    echo "ERROR: qsys-generate failed; see $WORK_DIR/qsys-gen.log" >&2
    cat "$WORK_DIR/qsys-gen.log" >&2
    exit 1
}

# Move the generated tree into place.
rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"
cp -r "$WORK_DIR/out/synthesis"/* "$OUT_DIR/"
cp "$WORK_DIR/clock_pll_gen.qsys" "$OUT_DIR/"

echo "[gen_pll_frac] wrote:"
ls -la "$OUT_DIR/" | head -20
echo ""
echo "[gen_pll_frac] To use, point your top-level instantiation at"
echo "  module clock_pll_gen (refclk_clk, refclk_rst_reset, outclk0_clk, locked_export);"
echo "and add the .qip to your project (already done by clock_pll_gen.qip in OUT_DIR)."
