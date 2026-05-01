#!/usr/bin/env bash
# Apply (or revert) project-local patches to submodule files.
#
# This branch carries the union of working-fpga and debug-snooper patches:
#   * working-fpga-hazard3_core.patch — the production fix that suppresses
#     the FPGA-only d-port → APB bridge HWDATA corruption (forces
#     x_stall_on_raw=1). Targets Hazard3/hdl/.
#   * debug-snooper-hazard3_taps.patch — debug taps in hazard3_core /
#     hazard3_cpu_2port / hazard3_cpu_1port that the bus_snooper
#     peripheral consumes. Targets Hazard3/hdl/.
#   * debug-snooper-sram_wait.patch — opt-in EXTRA_RD_WAIT parameter on
#     ahb_sync_sram, used to reproduce the FPGA-only symptom in
#     Verilator (default 0 in kws_soc.v, off). Targets the nested
#     Hazard3/example_soc/libfpga submodule.
#
# Patches apply / revert to the submodule worktree on every build,
# idempotently. The submodules' tracked SHAs are unchanged, so
# `git submodule update` / upstream pulls remain conflict-free.
#
# Usage:
#   scripts/apply_patches.sh            # apply  (idempotent)
#   scripts/apply_patches.sh --revert   # revert (idempotent)
#
# The Makefile runs this before any build target (apply mode) and from
# `make clean` (revert mode) so the working tree is left pristine.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PATCH_DIR="$ROOT/patches"
HAZARD3="$ROOT/Hazard3"
LIBFPGA="$ROOT/Hazard3/example_soc/libfpga"

REVERT=0
case "${1:-}" in
    --revert) REVERT=1 ;;
    "")       REVERT=0 ;;
    *)        echo "usage: $0 [--revert]" >&2; exit 2 ;;
esac

# debug-snooper-* patches are opt-in. Set DEBUG_SNOOPER=1 in the environment
# (typically via `make DEBUG_SNOOPER=1 …`) to apply them. They add output
# taps to hazard3_core / hazard3_cpu_*port and an EXTRA_RD_WAIT parameter
# to ahb_sync_sram, all of which are only useful when the bus_snooper
# peripheral is instantiated (also gated by `DEBUG_SNOOPER` in kws_soc.v).
DEBUG_SNOOPER="${DEBUG_SNOOPER:-}"

# working-fpga-hazard3_core.patch is FPGA-only (workaround for the d-port
# → APB-bridge HWDATA corruption documented in
# docs/2port_dport_bridge_bug.md).  On Verilator that race doesn't
# reproduce so the patch's stall is pure sim-side overhead — and verified
# empirically to cost 0 cycles on mel inference, so skipping it on sim is
# pure cleanliness.  Set FPGA_PATCHES=1 (or just FPGA=1, common in our
# Quartus targets) to apply it.  Default off keeps the Hazard3 submodule
# clean for sim workflows.
FPGA_PATCHES="${FPGA_PATCHES:-${FPGA:-}}"

[ -d "$PATCH_DIR" ] || exit 0

shopt -s nullglob
for patch in "$PATCH_DIR"/*.patch; do
    name="$(basename "$patch")"
    case "$name" in
        working-fpga-hazard3_core.patch)
            if [ -z "$FPGA_PATCHES" ]; then
                # Skip silently in apply mode; in revert mode we still
                # want to revert in case the user toggled FPGA_PATCHES
                # off after a previous FPGA build.
                if [ $REVERT -eq 0 ]; then continue; fi
            fi
            target="$HAZARD3"
            ;;
        debug-snooper-hazard3_taps.patch)
            if [ -z "$DEBUG_SNOOPER" ]; then
                # Skip silently in apply mode; in revert mode we still
                # want to revert in case it was previously applied.
                if [ $REVERT -eq 0 ]; then continue; fi
            fi
            target="$HAZARD3"
            ;;
        debug-snooper-sram_wait.patch)
            if [ -z "$DEBUG_SNOOPER" ]; then
                if [ $REVERT -eq 0 ]; then continue; fi
            fi
            # libfpga is a nested submodule of Hazard3, with its own git tree
            target="$LIBFPGA"
            ;;
        *)
            echo "[patches] $name has no known target, skipping" >&2
            continue
            ;;
    esac

    if [ $REVERT -eq 1 ]; then
        # Revert mode: only revert if currently applied; idempotent if not.
        if git -C "$target" apply --reverse --check "$patch" 2>/dev/null; then
            git -C "$target" apply --reverse "$patch"
            echo "[patches] reverted $name"
        fi
    else
        # Apply mode: idempotent — skip if already applied.
        if git -C "$target" apply --reverse --check "$patch" 2>/dev/null; then
            continue
        fi
        if git -C "$target" apply --check "$patch" 2>/dev/null; then
            git -C "$target" apply "$patch"
            echo "[patches] applied $name"
        else
            echo "[patches] $name no longer applies cleanly to $target" >&2
            echo "[patches] either the file already differs or upstream changed the area." >&2
            exit 1
        fi
    fi
done
