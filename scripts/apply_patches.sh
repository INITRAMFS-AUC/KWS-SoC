#!/usr/bin/env bash
# Apply (or revert) project-local patches to submodule files.
#
# This branch (debug-snooper) carries one patch — see patches/ directory.
# The patch is applied to the Hazard3 submodule worktree on every build,
# idempotently. The submodule's tracked SHA is unchanged so future
# `git submodule update` / upstream pulls don't conflict.
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

[ -d "$PATCH_DIR" ] || exit 0

shopt -s nullglob
for patch in "$PATCH_DIR"/*.patch; do
    name="$(basename "$patch")"
    case "$name" in
        debug-snooper-hazard3_taps.patch)
            target="$HAZARD3"
            ;;
        debug-snooper-sram_wait.patch)
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
