# Timing constraints for KWS-SoC on DE10-Standard (Cyclone V)

# 50 MHz board oscillator
create_clock -name clk_50 -period 20.000 [get_ports clk_50]

# JTAG TCK — separate async domain
create_clock -name tck -period 100.000 [get_ports tck]

# Derive all PLL output clocks (creates sys_clk at 36 MHz from clk_50 via my_pll)
derive_pll_clocks
derive_clock_uncertainty

# Separate async clock domains:
#   group 1 — all clocks in the system domain (50 MHz board osc + any PLL outputs)
#   group 2 — JTAG TCK (totally unrelated to sys domain)
# Using remove_from_collection makes this portable across PLL types (altera_pll
# vs altpll) so we don't hard-code the PLL output counter path.
set_clock_groups -asynchronous \
    -group [remove_from_collection [get_clocks *] [get_clocks tck]] \
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
