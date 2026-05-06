#!/usr/bin/env python3
"""
Interactive configurator for the KWS-SoC build / sim / FPGA flows.

Walks the user through:
  1. Mode selection (Simulation vs FPGA)
  2. Per-knob prompts (description, default, optional choices) for every
     environment variable that affects the chosen flow
  3. Target selection (which `make` recipe to invoke)
  4. Confirmation — prints the resolved `make` command before running it
  5. For the FPGA flow: pauses after `asm` so the user can program the
     on-board QSPI Flash with their firmware .bin BEFORE `make program`
     re-loads the FPGA bitstream

Run:
  python3 scripts/configure.py            # interactive
  python3 scripts/configure.py --list     # dump the knob table and exit
  python3 scripts/configure.py --dry-run  # walk the prompts but never invoke make

No third-party deps; stdlib only.
"""

from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Optional


REPO_ROOT = Path(__file__).resolve().parent.parent


# ── Knob table ──────────────────────────────────────────────────────────────
# Single source of truth.  Add a new row to expose a new env var to the
# wizard.  flow ∈ {"common", "sim", "fpga"}.

@dataclass
class Knob:
    name: str
    default: str
    descr: str
    flow: str
    choices: Optional[list[str]] = None


KNOBS: list[Knob] = [
    # ── Common to both flows ────────────────────────────────────────────────
    Knob("CLK_MHZ",            "36",
         "PLL target clock frequency in MHz.  Used by both Verilator and the "
         "FPGA PLL IP (re-run `make gen_pll` after a change). Available 25, 36, 40, 50, 100",
         "common"),
    Knob("SRAM_DEPTH",         "32768",
         "Internal SRAM depth in 32-bit words (32768 = 128 KiB).",
         "common"),
    Knob("UART_BAUD_RATE",     "115200",
         "UART baud rate.  Match this on your terminal / minicom session.",
         "common"),
    Knob("I2S_FIFO_DEPTH",     "16",
         "I2S receiver hardware FIFO depth (single source of truth for both "
         "the Verilog parameter and the firmware DMA burst size).",
         "common"),
    Knob("I2S_CLK_DIV",        "6",
         "Mic raw-rate divider: rate = CLK_MHZ*1e6 / (128*(div+1)).  "
         "36 MHz / div=6 → ~40 kHz raw → ~13.4 kHz post HW÷3.",
         "common"),
    Knob("I2S_WIDTH_MODE",     "",
         "Sample width override for recordI2s firmware: leave blank to use "
         "the kws_bare_main.c default; or set I2S_CONF_WIDTH_8 / _16 / _32.",
         "common",
         choices=["", "I2S_CONF_WIDTH_8", "I2S_CONF_WIDTH_16", "I2S_CONF_WIDTH_32"]),
    Knob("KWS_DEBOUNCE_COUNT", "2",
         "Consecutive identical predictions required before a DETECT line is "
         "emitted (1 = no debouncing).",
         "common"),
    Knob("USE_MCYCLE_CSR",     "0",
         "1 = firmware reads the mcycle CSR and prints CYCLES_CAPTURE / "
         "CYCLES_INFER / CYCLES_TOTAL per clip.",
         "common",
         choices=["0", "1"]),
    Knob("KWS_AGC_DISABLE",    "0",
         "1 = bypass the firmware-side rolling peak normaliser (AGC).  "
         "Useful when feeding pre-normalised inputs.",
         "common",
         choices=["0", "1"]),
    Knob("KWS_AGC_RELEASE_SHIFT", "",
         "AGC release exponent (peak -= peak >> SHIFT each sample).  "
         "Default 10 (~128 ms @ 8 kHz) — leave blank to keep it.",
         "common"),
    Knob("KWS_AGC_FLOOR",      "",
         "AGC floor — minimum peak value (prevents /0 + absurd boost on "
         "silence).  Default 8 — leave blank to keep it.",
         "common"),
    Knob("KWS_QUIET",          "0",
         "1 = only emit a per-clip log line when the post-debounce label is "
         "NOT 'unknown'.  Use for long noisy captures.",
         "common",
         choices=["0", "1"]),
    Knob("KWS_MODEL_HZ",       "",
         "Model-input sample rate.  Leave blank for the firmware's default "
         "(8000 for the legacy int8 model, 16000 for peak-norm).",
         "common",
         choices=["", "8000", "16000"]),
    Knob("RECORD_N_SAMPLES",   "",
         "recordI2s only: number of samples to capture before halting.  "
         "Leave blank to use the firmware's compile-time default.",
         "common"),

    # ── Simulation-only ─────────────────────────────────────────────────────
    Knob("FLASH",              "",
         "Path to the .bin to load into the simulated XIP flash.  Required "
         "for sim-verilator.  e.g. test/build/recordI2s_xip.bin",
         "sim"),
    Knob("MIC",                "",
         "Path to the .hex audio file fed into the simulated I2S mic BFM.  "
         "e.g. sim/playback_samples.hex",
         "sim"),
    Knob("CYCLES",             "",
         "Hard sim cycle limit.  Blank = run forever (until SIGINT).",
         "sim"),
    Knob("NO_JTAG",            "0",
         "1 = run the sim standalone without OpenOCD/GDB; CPU boots from "
         "flash immediately (requires FLASH=…).",
         "sim",
         choices=["0", "1"]),
    Knob("TRACE_FORMAT",       "FST",
         "Waveform format when invoking sim-verilator-vcd.  FST is fastest, "
         "VCD is widest tool support, SAIF is for power analysis.",
         "sim",
         choices=["FST", "VCD", "SAIF"]),
    Knob("XIP_DEBUG",          "0",
         "1 = enable XIP-flash $display + tb prints.",
         "sim",
         choices=["0", "1"]),
    Knob("I2S_DEBUG",          "0",
         "1 = enable I2S receiver $display + tb prints.",
         "sim",
         choices=["0", "1"]),
    Knob("ACCEL_DEBUG",        "0",
         "1 = enable Conv1D accelerator $display.",
         "sim",
         choices=["0", "1"]),
    Knob("UART_DEBUG",         "0",
         "1 = enable UART decoder verbose mode in the testbench.",
         "sim",
         choices=["0", "1"]),
    Knob("XIP_PLAYBACK",       "0",
         "1 = embed a hex audio file at XIP 0x8001_0000 and synthesize an "
         "I2S sample player into the design (post-FPGA validation aid).",
         "sim",
         choices=["0", "1"]),
    Knob("PLAYBACK_SAMPLES_HEX", "sim/playback_samples.hex",
         "Audio hex used when XIP_PLAYBACK=1.",
         "sim"),

    # ── FPGA-only ───────────────────────────────────────────────────────────
    Knob("FPGA_FAMILY",        '"Cyclone V"',
         "Quartus device family.  Quoted because the string contains a space.",
         "fpga"),
    Knob("FPGA_PART",          "5CSXFC6D6F31C6",
         "Quartus device part number.",
         "fpga"),
    Knob("FPGA_BOARD",         "DE10S",
         "Board hint — affects the program target's JTAG cable autodetect.",
         "fpga"),
    Knob("CONSTRAINTS_SRC",    "quartus/CycloneV/DE10_Constraints.tcl",
         "Path to the TCL constraints file applied during fit.",
         "fpga"),
    Knob("DEBUG_SNOOPER",      "",
         "1 = synthesize the bus-snooper debug peripheral at 0x4000_C000.  "
         "Costs M10K / area; off in production builds.",
         "fpga",
         choices=["", "1"]),
]


# ── Sim target presets ──────────────────────────────────────────────────────
# Discovered targets (Makefile + test/Makefile) most users care about.

SIM_TARGETS: list[tuple[str, str]] = [
    ("test",                                           "Build all firmware .bins under test/build/"),
    ("sim_verilator",                                  "Build the Verilator simulator only (no run)"),
    ("sim-verilator",                                  "Build firmware + simulator + RUN with FLASH/MIC/CYCLES"),
    ("sim-verilator-vcd",                              "sim-verilator but also dump waves.<TRACE_FORMAT>"),
    ("test-dma",                                       "Build the DMA test firmware only"),
    ("test-mel-compact",                               "Build mel_compact_4blk_ch36 (SW Conv2D)"),
    ("test-mel-compact-accel",                         "Build mel_compact_4blk_ch36 (HW Conv1D accel)"),
    ("test-mel-compact-int8-accel",                    "Build mel_compact_int8 (HW accel)"),
    ("test-mel-compact-int8-peak-norm-accel",          "Build the int8 peak-norm production firmware"),
    ("test-mel-compact-int8-peak-norm-dump",           "Build the spike-diff dump firmware (memcpy mode)"),
    ("test-mel-compact-int8-peak-norm-dump-full",      "Build the spike-diff dump firmware (full I2S pipeline)"),
    ("clean",                                          "Wipe every build artefact (sim + test + Quartus)"),
    ("clean_verilator",                                "Wipe just build/verilator"),
    ("clean_test",                                     "Wipe just test/build"),
]

# ── FPGA target sequences ───────────────────────────────────────────────────

FPGA_SEQUENCES: list[tuple[str, list[str], bool]] = [
    # (label, list of make targets, pause-for-flash-before-program?)
    ("Stop after fit",                  ["gen_pll", "config", "map", "fit"],            False),
    ("Stop after asm (bitstream ready)",["gen_pll", "config", "map", "fit", "asm"],     False),
    ("Full flow (gen_pll → … → program, with pause to program QSPI Flash)",
                                        ["gen_pll", "config", "map", "fit", "asm", "program"], True),
    ("Program only (skip synth — bitstream must already be built)", ["program"],        True),
]


# ── Prompt helpers ──────────────────────────────────────────────────────────

def _strip(s: str) -> str:
    return s.strip()


def ask(prompt: str, default: str = "") -> str:
    """Prompt with a default; blank input returns the default."""
    suffix = f" [{default}]" if default else ""
    try:
        raw = input(f"{prompt}{suffix}: ")
    except EOFError:
        return default
    raw = _strip(raw)
    return raw if raw else default


def ask_choice(prompt: str, choices: list[str], default_idx: int = 0) -> str:
    while True:
        print(prompt)
        for i, c in enumerate(choices, 1):
            label = c if c else "(unset)"
            mark = "*" if i - 1 == default_idx else " "
            print(f"  [{i}]{mark} {label}")
        raw = ask(f"choice 1-{len(choices)}", str(default_idx + 1))
        try:
            n = int(raw)
            if 1 <= n <= len(choices):
                return choices[n - 1]
        except ValueError:
            pass
        print("  invalid choice — try again\n")


def ask_yn(prompt: str, default_yes: bool = True) -> bool:
    sfx = "[Y/n]" if default_yes else "[y/N]"
    raw = ask(f"{prompt} {sfx}").lower()
    if not raw:
        return default_yes
    return raw[0] == "y"


def prompt_knob(idx: int, total: int, k: Knob, current_default: str) -> str:
    """Show a single knob with description; return chosen value (str)."""
    print()
    print(f"[{idx}/{total}] {k.name}   ({k.flow})")
    for line in _wrap(k.descr, 76):
        print(f"       {line}")
    if k.choices is not None:
        # Find current default's index
        try:
            di = k.choices.index(current_default)
        except ValueError:
            di = 0
        return ask_choice(f"       value? (default = {current_default!r})",
                          k.choices, default_idx=di)
    return ask(f"       value?", current_default)


def _wrap(text: str, width: int) -> list[str]:
    out, line = [], ""
    for word in text.split():
        if line and len(line) + 1 + len(word) > width:
            out.append(line); line = word
        else:
            line = word if not line else f"{line} {word}"
    if line:
        out.append(line)
    return out


# ── Make invocation ─────────────────────────────────────────────────────────

def compose_make_args(values: dict[str, str], target: str) -> list[str]:
    """Return argv for `make TARGET VAR=value …`, dropping blank values."""
    argv = ["make"]
    if target:
        argv.append(target)
    for k, v in values.items():
        if v == "":
            continue
        argv.append(f"{k}={v}")
    return argv


def shellish(argv: list[str]) -> str:
    """Render argv as a copy-pasteable shell command."""
    parts = []
    for a in argv:
        if a and re.fullmatch(r"[A-Za-z0-9_./=:+-]+", a):
            parts.append(a)
        else:
            parts.append("'" + a.replace("'", "'\\''") + "'")
    return " ".join(parts)


def run_make(argv: list[str], dry_run: bool) -> int:
    print()
    print("==>", shellish(argv))
    if dry_run:
        print("    (dry-run — not invoked)")
        return 0
    print()
    return subprocess.run(argv, cwd=REPO_ROOT).returncode


# ── Flows ───────────────────────────────────────────────────────────────────

def collect_values(flow: str) -> dict[str, str]:
    """Walk every relevant Knob and return user-chosen values."""
    relevant = [k for k in KNOBS if k.flow in ("common", flow)]
    chosen: dict[str, str] = {}
    print()
    print(f"── {flow.upper()} configuration ─────────────────────────────────")
    print(f"   Press Enter to accept the default in [brackets].")
    print(f"   Type '?' to re-print the description, 'q' to abort.")
    for i, k in enumerate(relevant, 1):
        while True:
            v = prompt_knob(i, len(relevant), k, k.default)
            if v == "?":
                continue
            if v == "q":
                print("aborted by user.")
                sys.exit(130)
            chosen[k.name] = v
            break
    return chosen


def sim_flow(values: dict[str, str], dry_run: bool) -> int:
    print()
    print("── Simulation target ─────────────────────────────────────────")
    labels = [f"{name}  — {desc}" for name, desc in SIM_TARGETS]
    pick = ask_choice("Choose a make target:", labels, default_idx=2)
    target = SIM_TARGETS[labels.index(pick)][0]

    argv = compose_make_args(values, target)
    print()
    print("Resolved command:")
    print(" ", shellish(argv))
    if not ask_yn("proceed?", default_yes=True):
        print("aborted by user.")
        return 130
    return run_make(argv, dry_run)


def flash_pause() -> None:
    print()
    print("=" * 72)
    print("  BITSTREAM READY: quartus/output_files/KWS-SoC.sof")
    print()
    print("  Now program the on-board QSPI Flash with your firmware .bin")
    print("  (e.g. test/build/recordI2s_xip.bin) using the vendor's flash-")
    print("  loader tool.  Power-cycle the board so XIP boots from the new")
    print("  image, then return here and press Enter to invoke `make program`")
    print("  (which re-loads the FPGA bitstream over JTAG).")
    print()
    print("  Ctrl-C to abort without programming the FPGA.")
    print("=" * 72)
    try:
        input("Press Enter when Flash is programmed and the board is ready...")
    except (EOFError, KeyboardInterrupt):
        print("\naborted by user.")
        sys.exit(130)


def fpga_flow(values: dict[str, str], dry_run: bool) -> int:
    print()
    print("── FPGA flow ────────────────────────────────────────────────")
    labels = [lbl for lbl, _, _ in FPGA_SEQUENCES]
    pick = ask_choice("Choose a sequence:", labels, default_idx=2)
    seq_idx = labels.index(pick)
    targets, pause = FPGA_SEQUENCES[seq_idx][1], FPGA_SEQUENCES[seq_idx][2]

    print()
    print("Sequence:")
    for t in targets:
        print(f"   make {t}")
    if pause:
        print("   <pause: flash QSPI on-board>")
        print("   make program  ← already in the list above; pause inserted before it")
    if not ask_yn("proceed?", default_yes=True):
        print("aborted by user.")
        return 130

    for t in targets:
        if pause and t == "program":
            flash_pause()
        rc = run_make(compose_make_args(values, t), dry_run)
        if rc != 0:
            print(f"\nmake {t} failed (exit {rc}); aborting sequence.")
            return rc
    return 0


# ── Main ────────────────────────────────────────────────────────────────────

def list_knobs() -> None:
    for flow in ("common", "sim", "fpga"):
        print(f"# {flow.upper()}")
        for k in KNOBS:
            if k.flow != flow:
                continue
            choices = f"  choices={k.choices}" if k.choices else ""
            print(f"  {k.name:<24} default={k.default!r}{choices}")
            print(f"      {k.descr}")
        print()


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--list",   action="store_true", help="dump the knob table and exit")
    p.add_argument("--dry-run", action="store_true",
                   help="walk the prompts but never invoke make")
    args = p.parse_args()

    if args.list:
        list_knobs()
        return 0

    print("KWS-SoC interactive configurator")
    print(f"  repo root: {REPO_ROOT}")
    flow_choice = ask_choice(
        "Select flow:",
        ["Simulation (Verilator)", "FPGA (Quartus → board)", "Cancel"],
        default_idx=0,
    )
    if flow_choice.startswith("Cancel"):
        return 0

    flow = "sim" if flow_choice.startswith("Simulation") else "fpga"
    values = collect_values(flow)

    if flow == "sim":
        return sim_flow(values, args.dry_run)
    return fpga_flow(values, args.dry_run)


if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print("\naborted by user.")
        sys.exit(130)
