#!/usr/bin/env bash
# run_soc_batch.sh — Batch accuracy + cycle count validation for KWS-SoC
#
# Runs each of the 11 keyword audio hex files through the Verilator sim,
# collects DETECT:<pred>,<class> and CYCLES:<n> from UART output, reports
# accuracy and cycle counts.
#
# Usage:
#   bash scripts/run_soc_batch.sh \
#       --flash <fw.bin> \
#       --audio-dir <dir with *_0000.hex> \
#       --max-cycles <N> \
#       [--label <name>]
#
# Example (accel firmware, ~9 minutes for 11 clips):
#   cd kws_soc/
#   bash scripts/run_soc_batch.sh \
#       --flash test/build/mel_compact_4blk_ch36_xip_accel.bin \
#       --audio-dir sim \
#       --max-cycles 60000000 \
#       --label "accel"
#
# Example (SW-only baseline firmware, ~90 minutes for 11 clips):
#   bash scripts/run_soc_batch.sh \
#       --flash test/build/mel_compact_4blk_ch36_xip.bin \
#       --audio-dir sim \
#       --max-cycles 250000000 \
#       --label "sw_only"

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOC_DIR="$(dirname "$SCRIPT_DIR")"
VERILATOR_BIN="$SOC_DIR/build/verilator/Vkws_soc"

FLASH=""
AUDIO_DIR=""
MAX_CYCLES=60000000
LABEL="run"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --flash)       FLASH="$2";       shift 2 ;;
        --audio-dir)   AUDIO_DIR="$2";   shift 2 ;;
        --max-cycles)  MAX_CYCLES="$2";  shift 2 ;;
        --label)       LABEL="$2";       shift 2 ;;
        *) echo "Unknown argument: $1" >&2; exit 1 ;;
    esac
done

if [[ -z "$FLASH" || -z "$AUDIO_DIR" ]]; then
    echo "Usage: $0 --flash <fw.bin> --audio-dir <dir> [--max-cycles N] [--label name]" >&2
    exit 1
fi

if [[ ! -f "$VERILATOR_BIN" ]]; then
    echo "ERROR: Verilator binary not found: $VERILATOR_BIN" >&2
    echo "       Run: make sim_verilator (from $SOC_DIR)" >&2
    exit 1
fi

if [[ ! -f "$FLASH" ]]; then
    echo "ERROR: Flash binary not found: $FLASH" >&2
    exit 1
fi

CLASSES=(down go left no off on right stop up yes unknown)

echo "========================================================"
echo " KWS-SoC Verilator Batch Test: $LABEL"
echo "========================================================"
echo " Flash:      $FLASH ($(wc -c < "$FLASH") bytes)"
echo " Audio dir:  $AUDIO_DIR"
echo " Max cycles: $MAX_CYCLES"
echo "========================================================"
echo ""

declare -A results
total=0
correct=0
total_cycles=0
min_cycles=999999999
max_cycles_seen=0
failed=0

for cls in "${CLASSES[@]}"; do
    hex="$AUDIO_DIR/${cls}_0000.hex"
    if [[ ! -f "$hex" ]]; then
        echo "WARNING: audio file not found: $hex" >&2
        ((failed++)) || true
        continue
    fi

    printf "  %-8s → " "$cls"

    # Run the Verilator sim; firmware loops forever, so timeout kills it.
    # --no-jtag: boot from flash immediately, no OpenOCD needed.
    # Capture stdout (UART output), discard stderr (sim noise).
    uart_out=$(timeout 300 \
        "$VERILATOR_BIN" \
        --no-jtag \
        --flash "$FLASH" \
        --mic "$hex" \
        --cycles "$MAX_CYCLES" \
        2>/dev/null) || true

    # Parse first DETECT line: "DETECT:<index>,<name>"
    detect_line=$(echo "$uart_out" | grep -m1 '^DETECT:' || true)
    # Parse first CYCLES line: "CYCLES:<n>"
    cycles_line=$(echo "$uart_out" | grep -m1 '^CYCLES:' || true)

    if [[ -z "$detect_line" ]]; then
        echo "NO_DETECT (sim output below):"
        echo "$uart_out" | head -20
        ((failed++)) || true
        continue
    fi

    pred_class="${detect_line#DETECT:*,}"
    pred_class="${pred_class%%$'\r'}"

    cycles_val=""
    if [[ -n "$cycles_line" ]]; then
        cycles_val="${cycles_line#CYCLES:}"
        cycles_val="${cycles_val%%$'\r'}"
    fi

    is_correct=0
    if [[ "$pred_class" == "$cls" ]]; then
        is_correct=1
        ((correct++)) || true
        marker="✓"
    else
        marker="✗"
    fi
    ((total++)) || true

    results["$cls"]="$pred_class|$cycles_val|$is_correct"

    if [[ -n "$cycles_val" ]]; then
        printf "%-10s %s  [%s cycles]\n" "$pred_class" "$marker" "$cycles_val"
        ((total_cycles += cycles_val)) || true
        if (( cycles_val < min_cycles )); then min_cycles=$cycles_val; fi
        if (( cycles_val > max_cycles_seen )); then max_cycles_seen=$cycles_val; fi
    else
        printf "%-10s %s  [no CYCLES]\n" "$pred_class" "$marker"
    fi
done

echo ""
echo "========================================================"
echo " Results for: $LABEL"
echo "========================================================"
printf "  Accuracy:   %d / %d correct\n" "$correct" "$total"
if (( total > 0 )); then
    acc=$(awk "BEGIN { printf \"%.1f\", 100*$correct/$total }")
    echo "  Accuracy %: $acc%"
fi
if (( total > 0 && total_cycles > 0 )); then
    avg_cycles=$(( total_cycles / total ))
    clk_mhz=36
    avg_ms=$(awk "BEGIN { printf \"%.1f\", $avg_cycles / ($clk_mhz * 1000) }")
    echo ""
    echo "  Inference cycles (model_run only, excludes audio collection):"
    printf "    min: %d\n" "$min_cycles"
    printf "    max: %d\n" "$max_cycles_seen"
    printf "    avg: %d  (~%s ms @ %d MHz)\n" "$avg_cycles" "$avg_ms" "$clk_mhz"
fi
if (( failed > 0 )); then
    echo "  WARNING: $failed clips failed"
fi
echo "========================================================"
