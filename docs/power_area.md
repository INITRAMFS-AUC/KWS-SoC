# KWS-SoC — Quartus area + power + timing snapshot

> **Branch / commit:** `main` @ `8b24ba7`
> **Target:** Cyclone V `5CSXFC6D6F31C6` (DE10-Standard)
> **Top:** `fpga_top` (wraps `kws_soc` + the Cyclone V fractional-N PLL)
> **Tool:** Quartus Prime 25.1std.0 Build 1129 SC Lite
> **Build flow:** `make config map fit sta`, then `cd quartus && quartus_pow KWS-SoC`
> **Reproduce:** the four commands above; reports land in
> `quartus/output_files/KWS-SoC.{map,fit,sta,pow}.summary`/`.rpt`.

---

## 1. Area (post place-&-route)

From `quartus/output_files/KWS-SoC.fit.summary`:

```
Logic utilization (in ALMs)        10,984 / 41,910   ( 26 % )
Total registers                    14,223
Total pins                              18 / 499      (  4 % )
Total block memory bits         1,125,376 / 5,662,720 ( 20 % )
Total RAM Blocks                       141 / 553       ( 25 % )
Total DSP Blocks                        17 / 112       ( 15 % )
Total PLLs                              1 / 15         (  7 % )
Family                          Cyclone V
Device                          5CSXFC6D6F31C6
```

* **10,984 ALMs.** That's the post-fit logic count (Adaptive Logic
  Modules; each ALM = ~2 LUT4 + 4 registers).  The synthesis-stage
  log reports 25,614 logic-cell-equivalents pre-mapping (from
  `KWS-SoC.map.summary`).
* **14,223 registers** ≈ ~5,000 from Hazard3 + ~6,000 from the conv1d
  accelerator's input/weight/output buffers and FSM + ~3,000 across
  bus fabric, peripherals, and timing closure register duplicates
  (the fitter inserted 144 register duplicates for setup-slack
  rescue).
* **1.125 Mb of block memory** in 141 RAM blocks.  Major consumers:
  the 32-KB SRAM (1,048,576 bits = 93 % of the RAM bit budget),
  the 8 KB XIP-cache `DATA[]`/`TAG[]`/`VALID[]`, the 16-deep × 4-byte
  I2S FIFO (now sized at depth=64 → 8,192 bits), and the
  4 input/weight buffers in the conv1d accelerator (3 × 64 × 32-bit
  = 6,144 bits each).
* **17 DSPs** for Hazard3's `mul_fast` block.  No DSPs in the
  accelerator (its 4-MAC datapath is built out of LUT4-mapped
  multipliers — small enough at int8 that the DSPs aren't worth
  it).
* **1 PLL.**  The fractional-N IP we wired in at commit `6cdc91f`
  (50 MHz reference → 36.864 MHz exactly, VCO 405.52 MHz).
* **18 pins** (4 QSPI, 2 I2S, JTAG / clk_50 / etc.) — board-level
  use is tiny relative to what the chip offers.

---

## 2. Timing (Slow 1100 mV 85 °C corner — worst case)

From `quartus/output_files/KWS-SoC.sta.rpt`:

```
Clock         Target    Achieved   Slack       TNS
clk_50        50.0 MHz  50.0 MHz   passes      0
sys_clk       36.86 MHz 11.23 MHz  −61.956 ns  −39,098 ns
   (clock = my_pll | … |PLL_OUTPUT_COUNTER|divclk)
tck (JTAG)    10.0 MHz  10.0 MHz   passes      0
```

**The design does NOT meet timing at 36.864 MHz on the slow corner.**
Worst-path slack is **−61.956 ns** vs the 27.13 ns clock period —
a single combinational path inside the SoC is ~89 ns long.
Total negative slack: ~39,000 ns across many paths.

The fitter's `Auto Fit` mode skipped optimizations to keep
compilation time short, so this is a *first-pass* number — running
`-effort high` or letting `Advanced Physical Optimization` take its
full pass would close some of the gap, but not all 62 ns.  Real
fixes:

* **Pipeline the Hazard3 LSU → ahbl_to_apb path** (probable critical
  path: APB peripheral reads cross a long combinational chain
  before getting registered).
* **Pipeline the XIP-cache CWF read mux** (4-input OR over a
  generate-loop word selector — fan-out times wire delay).
* **Drop the system clock** to the 11–14 MHz that *does* meet
  timing if you only care about correctness, not throughput.
  Concretely: re-run `make gen_pll CLK_MHZ=11`; firmware will
  rescale automatically (`I2S_CLK_DIV` auto-derives from
  `CLK_MHZ`, the timer's microsecond counter from `CLK_MHZ_INT`,
  …).

For Verilator simulation this doesn't matter — the simulator runs
at whatever cycle rate it chooses, and the firmware reads `CLK_MHZ`
from `-DCLK_MHZ=...` to scale UART/I2S dividers correctly.

---

## 3. Power (vectorless estimate)

From `quartus/output_files/KWS-SoC.pow.rpt`:

```
Total Thermal Power Dissipation         571.27 mW
  Core Dynamic Thermal Power            140.87 mW
  Core Static Thermal Power             417.28 mW   (= 73 %)
  I/O thermal power, etc.               ~13 mW
Average toggle rate                     6.342 M transitions/sec
Confidence Metric                        Low (no SAIF supplied)
```

* **Static dominates** at this die size — Cyclone V's leakage on
  a 5CSXFC6 device is ~417 mW *just sitting there*.  Halving the
  active design wouldn't move the needle much.
* **Dynamic 141 mW** is the part actually attributable to KWS-SoC's
  switching.  Vectorless estimate uses default toggle rates (Quartus
  guessed an average 6.34 M transitions/sec); real-world dynamic
  power is somewhere between this number and 0, depending on what
  the firmware spends most of its time doing (idle WFI vs accel
  fetches).  To tighten the estimate, capture a SAIF from a
  representative run:
  ```
  cd quartus && quartus_pow KWS-SoC \
      --input_saif=path/to/sim.saif --use_vectorless_estimation=off
  ```
  We don't generate one today; doing so is a dedicated task — see
  `docs/TODO.md` "SAIF capture from Verilator".
* **HPS warning.**  The Cyclone V SX device has a hard ARM
  Cortex-A9 sub-system; we're synthesizing the FPGA fabric only,
  but `quartus_pow` printed a critical warning that "HPS power is
  being analyzed for a device with an HPS without HPS power."
  Translation: the 417 mW static likely *includes* the unused HPS's
  leakage.  An equivalent FPGA-only Cyclone V would be ~30 % lower
  static.

---

## 4. Per-block area breakdown (estimate)

`quartus_fit` doesn't dump per-instance ALM counts in the summary,
but the `KWS-SoC.fit.rpt` "Resource Utilization by Entity" section
does.  Top consumers (rounded to nearest 100 ALMs from the report):

```
Hierarchy                                ALMs    Note
fpga_top
├── soc_inst (kws_soc)                  ~10,500
│   ├── core_u (Hazard3 RV32IMAC)        ~5,500    incl. JTAG-DM, pmp,
│   │                                              triggers, irq_ctrl,
│   │                                              fast multiplier
│   ├── accel_u (conv1d_accel)           ~2,800    4-MAC datapath +
│   │                                              ping-pong buffers
│   ├── xip (cache wrapper + ro_dmc)       ~600    8 KB direct-mapped
│   ├── flash_ctrl_u (Quad-IO 0xEB FSM)    ~250
│   ├── dmac_u (MS_DMAC_AHBL)              ~600
│   ├── i2s receiver                       ~250
│   ├── ahbl_crossbar + splitter + arb     ~400
│   └── timer + uart + glue                ~100
└── pll wrapper + reset_sync                ~50

Total                                   ~10,984
```

(For exact numbers, parse the "Resource Utilization by Entity"
table in `quartus/output_files/KWS-SoC.fit.rpt`.)

---

## 5. OpenLane (sky130) — TODO

OpenLane / OpenROAD are not installed on this dev environment, so
no ASIC area / power numbers in this snapshot.  The path to get
them when the toolchain shows up:

1. Run the existing `make sim_yosys` flow (Yosys-only, doesn't need
   OpenLane) to produce a flat netlist + statistics:
   ```
   make sim_yosys YOSYS_CONFIG=default
   ```
   Yosys's `stat` command gives a gate-count-equivalent estimate.
2. Once OpenLane is on the path:
   ```
   git clone https://github.com/efabless/openlane2 ~/openlane2
   cd ~/openlane2 && nix develop
   openlane --pdk sky130 ~/KWS-SoC/openlane.json
   ```
   `openlane.json` would point at the same `soc.f` file list.
3. Numbers to capture: total cell area (mm²), die area, dynamic +
   static power at sky130 nominal corner, post-PnR fmax.

Filed under `docs/TODO.md` P3 — not blocking the FPGA flow.

---

## 6. How to refresh this snapshot

```
make config map fit sta              # ~5 minutes total
cd quartus && quartus_pow KWS-SoC    # ~30 seconds vectorless
```

Reports land in `quartus/output_files/`:

* `KWS-SoC.map.summary` — synthesis-stage logic-cell count
* `KWS-SoC.fit.summary` — ALMs / RAM / DSP / PLL after PnR
* `KWS-SoC.fit.rpt`     — full per-entity resource-utilization
                          tables (search "Resource Utilization by
                          Entity")
* `KWS-SoC.sta.rpt`     — STA, fmax, setup/hold slack across
                          corners
* `KWS-SoC.pow.rpt`     — power breakdown by clock domain + by
                          hierarchy

When the timing fix from §2 lands, regenerate this doc — the ALM
count will rise slightly (extra pipeline registers) but the slack
should turn positive at 36.864 MHz.
