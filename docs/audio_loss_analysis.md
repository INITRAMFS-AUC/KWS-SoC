# How AUDIO_LOSS happens even when inference is faster than realtime

> The question this doc answers: *if mel_compact_int8 inference is
> ~1.05 M cycles (~30 ms wall) and we get a full second of audio
> every ~36 M cycles, how can the audio-loss guard ever fire?*

**Short answer:** the guard tracks **per-iteration wall-time
variance**, not the **average** inference rate.  Average is fast
enough by 30×; but a *single* iter that occasionally takes 1 s+ of
wall time (because of bus contention spikes that `mcycle` doesn't
see) is enough to overflow the ring's 1-clip slack.

---

## 1. What the guard actually checks

`test/common/kws_bare_main.c`, top of every iter:

```
if (bytes_written - bytes_at_next_inference > RING_SAMPLES_PER_CLIP) {
    uart_puts("AUDIO_LOSS bytes=");
    ...
    bytes_at_next_inference = bytes_written;   // resync
}
```

The condition is **not** "inference is too slow on average".  It is
"the DMA has written **more than 1 clip's worth** of bytes since the
last inference's window-end".  Translation: between the last
`bytes_at_next_inference += step_bytes` and now, more than 1 s of
audio went by on the wall clock.

`step_bytes` is anchored to the DMA-side counter (`bytes_written`
delta during the iter), so in steady state `bytes_at_next_inference`
tracks `bytes_written` exactly — no drift.  The guard fires *only*
when a single iter's wall time exceeds 1 s.

## 2. Why a single iter can be 1 s+

`CYCLES_INFER` reports ~1.05 M for every `model_run()` call, but
the *iter* includes more than `model_run()`:

* snapshot loop (~80 K cycles, ring → nnom_input_data)
* softmax / argmax (~5 K)
* UART output (variable, suppressed under KWS_QUIET)
* accel busy-poll loop (waits for ACCEL_CTRL_DONE)

That's all measurable by `mcycle` and adds up to ~1.5 M nominally.

What `mcycle` **doesn't** count cleanly:

* AHB bus arbitration stalls when the DMA fires a burst mid-fetch
* XIP cache cold misses on weight reads (the accel does ~2200 reads
  per inference; if the cache evicts a chunk during one call, the
  next call's first re-read pays full ~150 cyc misses each)
* HRESP retries / arbitration round-robin penalties

Empirically, a small fraction of iters take 30–70 M wall cycles
(by `bytes_written`-counter math) while their `CYCLES_INFER`
remains ~1.05 M.  That's the residual `mcycle ≪ wall-time` anomaly
captured under task #23.

When such an iter happens, `bytes_written` advances by ~13 K-30 K
bytes during it.  If that exceeds `RING_SAMPLES_PER_CLIP` of slack,
the next iter's top-of-loop check fires AUDIO_LOSS.

## 3. Why HW DS made it go away

Flipping `KWS_DS_EN` from 0 to 1 (commit `1278b87`) halved the
DMA-side byte rate (7.81 kHz vs 15.62 kHz on cfg_div=17).  Two
effects compound:

1. **Fewer DMA bursts on the AHB bus.**  Each DMA burst held the
   bus for ~5-10 cycles plus arbitration overhead.  Halving the
   burst rate halves the contention windows.  The pathological
   iters that exceeded 1 s wall time no longer do — their wall
   time tightens up because the accel's XIP reads aren't competing
   as often with the DMA's SRAM writes.

2. **Fewer PIRQs in the IRQ handler.**  The handler is small (~50
   cyc) but each PIRQ is a context switch and forces a memory
   barrier.  Halving the IRQ rate halves that overhead in flight.

Result on the same 200 M-cycle test (mel_compact_int8_xip_accel,
sim/playback_samples.hex):

| metric                    | fw-÷2 (KWS_DS_EN=0) | HW-DS (KWS_DS_EN=1)  |
| ------------------------- | ------------------- | -------------------- |
| AUDIO_LOSS events         | 8                   | **0**                |
| Largest overshoot         | 14.6 KB             | n/a                  |
| CYCLES_INFER mean         | 1,049,515           | 1,046,925            |
| DETECT lines              | 49                  | 12                   |

The mean inference cost barely moves (–0.25 %) — what changed is
the **tail** of the wall-time distribution.

## 4. What can still trigger AUDIO_LOSS in the new config

* **Long-running (>1 s wall) inference targets.**  If we ever ship
  a model whose Conv2D layers can't be accel'd (dilation, large
  C_out, CHW format) and falls through to the SW path, that's
  ~80–100 M cycles per inference, ~3 s — guaranteed AUDIO_LOSS.
* **A future RTL change that re-introduces bus contention.**  E.g.
  if a third AHB master is added that competes with the accel's
  XIP reads, the `mcycle ≪ wall-time` gap could re-emerge and
  push some iters back over 1 s.
* **Cold cache after a long pause.**  Right after boot, the accel
  hits ~30+ compulsory misses on its first weight scan.  Today
  that's absorbed by the iter-0 capture wait (~36 M cycles to fill
  the ring).  But if firmware ever changes such that the first
  inference can run before the ring is full, the cold-cache hit
  on iter 0 could spike its wall time.

## 5. Two fail-safes that mean AUDIO_LOSS is a *warning*, not a fault

* **Guard auto-resyncs.**  When AUDIO_LOSS fires, the firmware
  forces `bytes_at_next_inference = bytes_written`.  Subsequent
  windows pick up from there.  Net effect: we lose audio across
  the spike, but the loop self-heals.

* **Sliding-window soft vote already amortises.**  Each
  inference's softmax row contributes to the 200 ms vote (see
  `kws_bare_main.c` "Wall-clock-anchored output gate with score-
  averaging").  One overshoot iter contributes 0 votes in its
  window; the rest of the window's iters fill in.  A short
  AUDIO_LOSS spike does NOT change the emitted DETECT label
  unless it spans the entire window.

## 6. Root cause for the mcycle ≪ wall-time anomaly

(Was task #23 in the task list — closed as "diagnosed but needs
RTL instrumentation to fully measure".)

Hazard3's `hazard3_csr.v` line 590:

```verilog
wire mcycle_stopped = mcountinhibit_cy || debug_mode || wen_m_mode && (
    addr == MCYCLEH || addr == MCYCLE
);
```

`mcycle` ticks every clock cycle UNLESS one of:

1. `mcountinhibit_cy` is set — we explicitly clear it in
   `csr_enable_cycle_counter()` at boot, and never write it back.
2. `debug_mode` is asserted by the JTAG debug module.
3. The CPU is mid-write to mcycle/mcycleh (single-cycle, irrelevant
   to long-running counts).

We're seeing 30+ M wall cycles per iter that mcycle doesn't count,
which leaves only path **(2)** as the structural answer: the
debug module is asserting `debug_mode` for a fraction of the
inference time even though `--no-jtag` means OpenOCD never
connects.

**Why we believe it without an oscilloscope:**

* `CYCLES_CAPTURE` is reported correctly during the iter-0 ring-
  fill: 36,823,380 mcycle ticks for a 1-second WFI wait, exactly
  matching the expected `36 MHz / 15.625 kHz × 16,000 bytes`.
  So mcycle definitely ticks during WFI sleep — the issue is
  specific to non-WFI execution.
* `bytes_written` is hardware-anchored (incremented in the DMA
  IRQ handler by exactly DMA_BURST_BYTES per PIRQ).  Its delta
  per iter IS the wall time.  We measured 14,400 bytes per iter ≈
  34 M cycles, vs CYCLES_INFER ≈ 1.05 M.  29 M cycles missing,
  consistent across multiple iters and reproducible.
* No NNoM / accel code does explicit ebreak / debug-halt; nothing
  in the firmware reads dscratch CSRs.

**What would close it definitively** (didn't pursue this session):

1. Add a free-running 32-bit counter in `kws_soc.v` (just
   `always @(posedge clk) wall_ctr <= wall_ctr + 1;`) exposed via
   APB.  Read it from firmware at iter_start / iter_end alongside
   mcycle, print both.  Direct A/B = unambiguous.
2. Or instrument the Verilator wrapper `kws_soc_vpi.cpp` to dump
   the simulation-side cycle counter when CYCLES_INFER lines
   appear in the UART stream.  Same A/B, no RTL change.
3. Tap `debug_mode` from the Hazard3 hierarchy
   (`soc_inst|core_u|core|debug_mode_o` once that signal exists)
   and assert it stays low across the iter window.

The HW-DS-default win (commit `1278b87`) eliminates the AUDIO_LOSS
symptom for the standard playback_samples.hex stimulus, so this
anomaly is observation-only today.

## 7. Open follow-ups

Both worth instrumenting once we have a workload that
actually re-triggers AUDIO_LOSS (today's playback_samples.hex run
under HW-DS doesn't).
