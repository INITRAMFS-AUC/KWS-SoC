# Timing constraints for KWS-SoC on DE10-Standard (Cyclone V)

# 50 MHz board oscillator
create_clock -name clk_50 -period 20.000 [get_ports clk_50]

# JTAG TCK — separate async domain
create_clock -name tck -period 100.000 [get_ports tck]

# Derive all PLL output clocks (creates sys_clk at 36 MHz from clk_50 via my_pll)
derive_pll_clocks
derive_clock_uncertainty

# Separate async clock domains:
#   group 1 — sys domain: 50 MHz board osc + 36 MHz PLL output (related clocks)
#   group 2 — JTAG TCK (totally unrelated to sys domain)
# Without sys_clk in a group the analyzer treats tck<->sys_clk as synchronous,
# inflating clock uncertainty by ~7 ns and creating false violations.
set_clock_groups -asynchronous \
    -group [get_clocks { clk_50 my_pll|clock_pll_36_inst|altera_pll_i|general[0].gpll~PLL_OUTPUT_COUNTER|divclk }] \
    -group [get_clocks { tck }]

# Async reset paths — no timing check needed.
# set_false_path -from [get_ports rst_n] only covers the pin; pll_locked is
# an internal PLL signal that also drives async clears and is not a port.
# False-path all async clear/preset pins globally (standard Quartus practice).
set_false_path -from [get_ports rst_n]
set_false_path -to [get_pins -compatibility_mode {*|clrn}]
set_false_path -to [get_pins -compatibility_mode {*|prn}]

# GPIO I/O pins have no board-level timing model; relax to avoid unconstrained I/O warnings
set_false_path -to   [get_ports {uart_tx xip_csn xip_sck flash_io[*] i2s_ws i2s_sck tdo}]
set_false_path -from [get_ports {uart_rx i2s_sd flash_io[*] tdi tms trst_n}]
