#!/usr/bin/env bash
# Aggregate Quartus warnings/errors across map/fit/sta/asm reports.
# Usage: scripts/collect_quartus_warnings.sh [output_path]
# Default output: quartus/output_files/all_warnings.txt
set -uo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT_DIR="$ROOT/quartus/output_files"
OUT_FILE="${1:-$OUT_DIR/all_warnings.txt}"

if [ ! -d "$OUT_DIR" ]; then
  echo "ERROR: $OUT_DIR not found. Run 'make map fit sta' first." >&2
  exit 1
fi

# Reports in stage order.
STAGES=(map fit sta asm)

{
  echo "# Quartus Warnings & Errors — generated $(date -Iseconds)"
  echo "# Source: $OUT_DIR"
  echo

  # 1. Per-stage summary counts.
  echo "## Summary"
  for s in "${STAGES[@]}"; do
    f="$OUT_DIR/KWS-SoC.$s.rpt"
    [ -f "$f" ] || { echo "- $s: MISSING"; continue; }
    err=$(grep -cE '^Error \(' "$f" || true)
    cri=$(grep -cE '^Critical Warning \(' "$f" || true)
    war=$(grep -cE '^Warning \(' "$f" || true)
    inf=$(grep -cE '^Info \(' "$f" || true)
    printf -- "- %-3s: %4d errors, %4d critical, %4d warnings, %d info\n" "$s" "$err" "$cri" "$war" "$inf"
  done
  echo

  # 2. Histogram of warning codes per stage (top 20).
  echo "## Top warning codes per stage"
  for s in "${STAGES[@]}"; do
    f="$OUT_DIR/KWS-SoC.$s.rpt"
    [ -f "$f" ] || continue
    echo "### $s"
    grep -oE '^(Critical Warning|Warning|Error) \([0-9]+\)' "$f" 2>/dev/null \
      | sort | uniq -c | sort -rn | head -20
    echo
  done

  # 3. Errors (full bodies).
  echo "## Errors (full)"
  for s in "${STAGES[@]}"; do
    f="$OUT_DIR/KWS-SoC.$s.rpt"
    [ -f "$f" ] || continue
    if grep -qE '^Error \(' "$f"; then
      echo "### $s"
      awk '/^Error \(/{p=1} p; /^$/{p=0}' "$f"
    fi
  done
  echo

  # 4. Critical warnings (full bodies).
  echo "## Critical Warnings (full)"
  for s in "${STAGES[@]}"; do
    f="$OUT_DIR/KWS-SoC.$s.rpt"
    [ -f "$f" ] || continue
    if grep -qE '^Critical Warning \(' "$f"; then
      echo "### $s"
      awk '/^Critical Warning \(/{p=1} p; /^$/{p=0}' "$f"
      echo
    fi
  done
  echo

  # 5. Warnings of high diagnostic value: latches, inferred memory, async, X, missing nets,
  #    multi-driven, race, simulation mismatch, missing constraints.
  echo "## Suspicious Warnings (filtered)"
  PATTERNS='latch|inferred|infer\s+|asynchron|cross[- ]clock|multi-?driv|combinational loop|simulation|race|undefin|x[ -]propagat|hold\s+slack|setup\s+slack|missed|missing\s+(input|net|connection|reset)|ungrouped|undriven|not connected|connected to|may not match|never assigned|no driver|stuck at'
  for s in "${STAGES[@]}"; do
    f="$OUT_DIR/KWS-SoC.$s.rpt"
    [ -f "$f" ] || continue
    if grep -iE "^(Critical Warning|Warning|Error) .*($PATTERNS)" "$f" >/dev/null; then
      echo "### $s"
      grep -iE "^(Critical Warning|Warning|Error) .*($PATTERNS)" "$f" | sort -u
      echo
    fi
  done
  echo

  # 6. All warnings touching the bug-suspect file set.
  echo "## Warnings touching bug-suspect files (hazard3_core, cpu_2port, ahbl_*, ahb_sync_sram)"
  SUSPECT='hazard3_core\.v|hazard3_cpu_2port\.v|hazard3_csr\.v|ahbl_arbiter\.v|ahbl_splitter\.v|ahbl_crossbar\.v|ahbl_to_apb\.v|ahb_sync_sram\.v|sram_sync\.v|hazard3_frontend\.v|hazard3_decode\.v'
  for s in "${STAGES[@]}"; do
    f="$OUT_DIR/KWS-SoC.$s.rpt"
    [ -f "$f" ] || continue
    if grep -E "^(Critical Warning|Warning|Error) .*($SUSPECT)" "$f" >/dev/null; then
      echo "### $s"
      grep -E "^(Critical Warning|Warning|Error) .*($SUSPECT)" "$f" | sort -u
      echo
    fi
  done
  echo

  # 7. Suppressed messages (smsg).
  smsg="$OUT_DIR/KWS-SoC.map.smsg"
  if [ -f "$smsg" ] && [ -s "$smsg" ]; then
    echo "## Suppressed messages (map.smsg)"
    grep -E '^(Critical Warning|Warning|Error)' "$smsg" | sort -u
    echo
  fi

} > "$OUT_FILE"

echo "Wrote $OUT_FILE"
echo "Lines: $(wc -l < "$OUT_FILE")"
