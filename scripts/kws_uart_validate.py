#!/usr/bin/env python3
"""
kws_uart_validate.py — Validate KWS inference accuracy from UART output.

Reads DETECT lines from the KWS UART output, aligns them against the expected
class sequence from wav_to_hex.py's labels file, and reports accuracy.

Validation strategy
-------------------
Each one-second audio clip produces exactly one DETECT line.  The XIP sample
player loops through the clips in order, so inference N maps to clip
N % len(labels).  The expected class for each inference is therefore
labels[N % len(labels)].

Because of firmware debouncing (KWS_DEBOUNCE_COUNT), a newly-encountered class
outputs "unknown" for the first (count-1) inferences before the real class is
confirmed.  The script simulates this to tell "unknown" responses apart:

  CORRECT  — actual == expected class for that clip position
  UNKNOWN  — actual == "unknown" (debounce warm-up; excluded from accuracy)
  WRONG    — actual is a real class AND differs from expected

Accuracy = CORRECT / (CORRECT + WRONG)

If the same class repeats across consecutive clips (e.g. two "go" clips in a
row), the debounce counter continues — no spurious "unknown" between them.
"""

import argparse
import re
import sys
import time
from collections import defaultdict

DETECT_RE = re.compile(r"^DETECT:(\w+),\s*(\d+)\s*$")

CLASS_NAMES = [
    "down", "go", "left", "no", "off",
    "on", "right", "stop", "up", "yes", "unknown",
]


def load_labels(path):
    with open(path) as f:
        labels = [l.strip() for l in f if l.strip()]
    if not labels:
        sys.exit(f"Error: no labels found in '{path}'")
    unknown_labels = [l for l in labels if l not in CLASS_NAMES]
    if unknown_labels:
        print(f"Warning: unrecognised class names in labels file: {unknown_labels}",
              file=sys.stderr)
    return labels


def simulate_debounce(labels, n_inferences, debounce_count):
    """Return what the firmware's DETECT line *should* print for each inference."""
    out = []
    last = None
    streak = 0
    for i in range(n_inferences):
        cls = labels[i % len(labels)]
        if cls == last:
            streak += 1
        else:
            last = cls
            streak = 1
        out.append(cls if streak >= debounce_count else "unknown")
    return out


def validate(actuals, labels, debounce_count):
    n = len(actuals)
    debounced_expected = simulate_debounce(labels, n, debounce_count)

    per_clip = defaultdict(lambda: {"correct": 0, "wrong": 0, "unknown": 0})
    rows = []
    correct = wrong = unknown_count = 0

    for i, actual in enumerate(actuals):
        clip_idx = i % len(labels)
        true_class = labels[clip_idx]
        expected_detect = debounced_expected[i]

        if actual == true_class:
            status = "OK"
            correct += 1
            per_clip[clip_idx]["correct"] += 1
        elif actual == "unknown":
            status = "--"
            unknown_count += 1
            per_clip[clip_idx]["unknown"] += 1
        else:
            status = "WRONG"
            wrong += 1
            per_clip[clip_idx]["wrong"] += 1

        rows.append((i, clip_idx, true_class, expected_detect, actual, status))

    return rows, correct, wrong, unknown_count, per_clip


def print_results(rows, labels, correct, wrong, unknown_count, per_clip, debounce_count):
    total = correct + wrong + unknown_count
    scored = correct + wrong

    print()
    print(f"{'#':>5}  {'Clip':>4}  {'True class':12}  {'Exp. DETECT':12}  {'Actual':12}  Result")
    print("-" * 68)
    for inf_num, clip_idx, true_class, exp_detect, actual, status in rows:
        marker = "  <-- WRONG" if status == "WRONG" else ""
        print(f"{inf_num:>5}  {clip_idx:>4}  {true_class:12}  {exp_detect:12}  {actual:12}  {status}{marker}")

    print()
    print("=" * 68)
    print(f"Total inferences : {total}")
    print(f"  Correct        : {correct}")
    print(f"  Wrong          : {wrong}")
    print(f"  Unknown        : {unknown_count}  (debounce warm-up, excluded from accuracy)")
    print()
    if scored > 0:
        accuracy = correct / scored * 100
        print(f"Accuracy (correct / (correct + wrong)) : {accuracy:.1f}%")
    else:
        print("No scored inferences — all outputs were 'unknown'.")
        print(f"Try increasing the test duration or setting KWS_DEBOUNCE_COUNT=1.")
    print()

    print(f"Per-clip breakdown  (KWS_DEBOUNCE_COUNT={debounce_count}):")
    print(f"  {'Clip':>4}  {'Class':12}  {'Correct':>8}  {'Wrong':>6}  {'Unknown':>8}  {'Rate':>7}")
    for clip_idx in range(len(labels)):
        d = per_clip[clip_idx]
        scored_clip = d["correct"] + d["wrong"]
        rate = f"{d['correct']/scored_clip*100:.1f}%" if scored_clip else "  n/a"
        print(f"  {clip_idx:>4}  {labels[clip_idx]:12}  {d['correct']:>8}  "
              f"{d['wrong']:>6}  {d['unknown']:>8}  {rate:>7}")


def read_from_serial(port, baud, n_target, timeout_s, flush_s=0.5):
    try:
        import serial
    except ImportError:
        sys.exit("pyserial not installed — run: pip install pyserial")

    actuals = []
    print(f"Opening {port} at {baud} baud …", file=sys.stderr)
    with serial.Serial(port, baud, timeout=timeout_s) as ser:
        ser.reset_input_buffer()
        time.sleep(flush_s)
        ser.reset_input_buffer()

        if n_target:
            print(f"Capturing {n_target} DETECT lines (Ctrl-C to stop early) …",
                  file=sys.stderr)
        else:
            print("Capturing DETECT lines until Ctrl-C …", file=sys.stderr)

        last_rx = time.monotonic()
        try:
            while True:
                raw = ser.readline()
                if not raw:
                    if time.monotonic() - last_rx > timeout_s:
                        print(f"\nTimeout: no data for {timeout_s}s.", file=sys.stderr)
                        break
                    continue
                last_rx = time.monotonic()
                line = raw.decode("ascii", errors="replace").strip()
                m = DETECT_RE.match(line)
                if m:
                    actuals.append(m.group(1))
                    suffix = f"/{n_target}" if n_target else ""
                    print(f"  [{len(actuals):>4}{suffix}] {line}", flush=True)
                    if n_target and len(actuals) >= n_target:
                        break
        except KeyboardInterrupt:
            print("\nStopped by user.", file=sys.stderr)

    return actuals


def read_from_log(path):
    actuals = []
    with open(path) as f:
        for line in f:
            m = DETECT_RE.match(line.strip())
            if m:
                actuals.append(m.group(1))
    print(f"Parsed {len(actuals)} DETECT lines from '{path}'", file=sys.stderr)
    return actuals


def main():
    p = argparse.ArgumentParser(
        description="Validate KWS UART DETECT output against wav_to_hex.py label ordering",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
examples:
  # Live capture — 1 full loop of a 5-clip test set on /dev/ttyUSB0
  python3 scripts/kws_uart_validate.py -l sim/labels.txt

  # Different port and two loops
  python3 scripts/kws_uart_validate.py -l sim/labels.txt -p /dev/ttyACM0 --loops 2

  # Replay from a saved UART log (e.g. captured with scripts/uart_capture.py)
  python3 scripts/kws_uart_validate.py -l sim/labels.txt --log uart.log

  # Match firmware built with KWS_DEBOUNCE_COUNT=3
  python3 scripts/kws_uart_validate.py -l sim/labels.txt --debounce 3

  # Run until Ctrl-C (--loops 0) then score whatever was captured
  python3 scripts/kws_uart_validate.py -l sim/labels.txt --loops 0
""",
    )
    p.add_argument("-l", "--labels", required=True,
                   help="Labels file from wav_to_hex.py (one class name per line)")
    p.add_argument("-p", "--port", default="/dev/ttyUSB0",
                   help="Serial port (default: /dev/ttyUSB0)")
    p.add_argument("-b", "--baud", type=int, default=115200,
                   help="Baud rate (default: 115200)")
    p.add_argument("-n", "--loops", type=int, default=1,
                   help="Full playback loops to capture; 0 = run until Ctrl-C (default: 1)")
    p.add_argument("-t", "--timeout", type=float, default=15.0,
                   help="Seconds to wait for UART data before giving up (default: 15)")
    p.add_argument("-d", "--debounce", type=int, default=2,
                   help="KWS_DEBOUNCE_COUNT used in firmware (default: 2)")
    p.add_argument("--log",
                   help="Read from a saved UART log instead of the serial port")
    args = p.parse_args()

    labels = load_labels(args.labels)
    print(f"Labels ({len(labels)} clips): {labels}")

    n_target = len(labels) * args.loops if args.loops > 0 else 0

    if args.log:
        actuals = read_from_log(args.log)
    else:
        actuals = read_from_serial(args.port, args.baud, n_target, args.timeout)

    if not actuals:
        sys.exit("No DETECT lines captured — nothing to validate.")

    rows, correct, wrong, unknown_count, per_clip = validate(
        actuals, labels, args.debounce)

    print_results(rows, labels, correct, wrong, unknown_count, per_clip, args.debounce)


if __name__ == "__main__":
    main()
