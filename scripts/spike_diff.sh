#!/usr/bin/env bash
# spike_diff.sh — build the layer-dump firmware, run it in Verilator, and
# diff the per-layer hex stream against test/spikedebug/activations.log.
#
# Two modes:
#   default ("memcpy")  — feed Spike's clip0 directly into nnom_input_data;
#                         the I2S/DMA path is bypassed.  Validates the
#                         model + accelerator only.
#   "full"              — feed down_audio_16k.hex through the I2S BFM, run
#                         the production audio path (DMA → snapshot → model)
#                         with AGC OFF (since the source isn't peak-
#                         normalised).  Validates the I2S + quantise +
#                         DMA + model chain end-to-end.
#
# Usage:
#   ./scripts/spike_diff.sh                       # memcpy mode, default cycles
#   ./scripts/spike_diff.sh 1500000000            # memcpy, custom cycles
#   ./scripts/spike_diff.sh full                  # full pipeline, default cycles
#   ./scripts/spike_diff.sh full 1500000000       # full pipeline, custom cycles
#   CYCLES=… FW_LOG=… ./scripts/spike_diff.sh …   # env overrides
#
# Exit 0 on bit-exact match, 1 otherwise (prints the first 40 diff lines).

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$REPO_ROOT"

# Argument parsing: optional first arg "full", then optional cycle count.
MODE="memcpy"
if [[ "${1:-}" == "full" ]]; then
  MODE="full"
  shift
fi

CYCLES="${1:-${CYCLES:-400000000}}"
SPIKE_LOG="test/spikedebug/activations.log"

if [[ "$MODE" == "full" ]]; then
  FW_BIN="test/build/mel_compact_int8_peak_norm_dump_full_xip_accel.bin"
  MIC_HEX="test/spikedebug/down_audio_16k_mic.hex"
  FW_LOG="${FW_LOG:-/tmp/fw_layers_full.log}"
  BUILD_TARGET="test-mel-compact-int8-peak-norm-dump-full"
  MIC_ARG=( --mic "$MIC_HEX" )
else
  FW_BIN="test/build/mel_compact_int8_peak_norm_dump_xip_accel.bin"
  FW_LOG="${FW_LOG:-/tmp/fw_layers.log}"
  BUILD_TARGET="test-mel-compact-int8-peak-norm-dump"
  MIC_ARG=()
fi

echo "==> ($MODE) building dump firmware"
make "$BUILD_TARGET" >/dev/null

if [[ ! -x build/verilator/Vkws_soc ]]; then
  echo "==> building Verilator simulator"
  make sim_verilator >/dev/null
fi

echo "==> running sim ($CYCLES cycles, ~$((CYCLES/36000000))s of mic time at 36 MHz)"
./build/verilator/Vkws_soc --no-jtag \
    --flash "$FW_BIN" \
    "${MIC_ARG[@]}" \
    --cycles "$CYCLES" 2>/dev/null \
  | tr -d '\r' >| "$FW_LOG"

echo "==> diff vs $SPIKE_LOG"
DIFF_OUT="$(diff \
    <(awk '/^LAYER_0 /,/^PRED:/' "$FW_LOG") \
    <(awk '/^LAYER_0 /,/^PRED:/' "$SPIKE_LOG") || true)"

if [[ -z "$DIFF_OUT" ]]; then
  fw_pred="$(grep '^PRED:' "$FW_LOG"  | head -1)"
  sp_pred="$(grep '^PRED:' "$SPIKE_LOG" | head -1)"
  echo
  echo "PASS — firmware output matches Spike byte-for-byte"
  echo "  firmware: $fw_pred"
  echo "  spike   : $sp_pred"
  exit 0
fi

if ! grep -q '^PRED:' "$FW_LOG"; then
  echo
  echo "FAIL — sim did not reach PRED line.  Re-run with more cycles, e.g.:"
  echo "  $0 $MODE $((CYCLES * 2))"
  echo
  echo "Last 5 lines of firmware log:"
  tail -5 "$FW_LOG"
  exit 1
fi

# First differing layer — useful summary for the full-pipeline mode where
# divergence is expected and we want to know *where* it starts.
first_diff_layer="$(awk '
    BEGIN { layer = -1 }
    /^LAYER_[0-9]+ / { match($0, /^LAYER_[0-9]+/); layer = substr($0, RSTART+6, RLENGTH-6) }
    NR == 1 { in_diff = 1 }
    /^[<>]/ && layer >= 0 { print layer; exit }
' <(diff <(awk '/^LAYER_0 /,/^PRED:/' "$FW_LOG") \
         <(awk '/^LAYER_0 /,/^PRED:/' "$SPIKE_LOG")) || true)"

# Simpler fallback: walk both files, find first non-matching line, report
# the LAYER_<n> header most recently seen above it.
first_diff_summary="$(diff <(awk '/^LAYER_0 /,/^PRED:/' "$FW_LOG") \
                           <(awk '/^LAYER_0 /,/^PRED:/' "$SPIKE_LOG") \
                       | head -1)"

fw_pred="$(grep '^PRED:' "$FW_LOG"  | head -1)"
sp_pred="$(grep '^PRED:' "$SPIKE_LOG" | head -1)"

echo
if [[ "$fw_pred" == "$sp_pred" ]]; then
  echo "PRED MATCH but per-layer activations DIFFER"
else
  echo "FAIL — different prediction"
fi
echo "  firmware: $fw_pred"
echo "  spike   : $sp_pred"
[[ -n "$first_diff_summary" ]] && echo "  first diff hunk header: $first_diff_summary"
echo "  total diff lines: $(echo "$DIFF_OUT" | wc -l)"
echo
if [[ "$MODE" == "full" ]]; then
  echo "Note: in 'full' mode the input is down_audio_16k.hex (NOT peak-normalised),"
  echo "so per-layer activations are EXPECTED to differ from Spike's pre-normalised"
  echo "clip0.  A matching PRED above is the meaningful pass criterion for this mode."
fi
echo
echo "First 40 diff lines:"
echo "$DIFF_OUT" | head -40
echo "..."
echo "(full firmware log: $FW_LOG)"
[[ "$fw_pred" == "$sp_pred" ]] && exit 0 || exit 1
