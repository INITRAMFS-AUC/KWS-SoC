# Timing constraints for KWS-SoC on DE10-Standard (Cyclone V)

# 50 MHz board oscillator
create_clock -name clk_50 -period 20.000 [get_ports clk_50]

# JTAG TCK — separate async domain
create_clock -name tck -period 100.000 [get_ports tck]

# Derive all PLL output clocks (creates sys_clk at 36 MHz from clk_50 via my_pll)
derive_pll_clocks
derive_clock_uncertainty

# Separate async clock domains (clk_50 vs tck; sys_clk derived from clk_50 via PLL)
set_clock_groups -asynchronous \
    -group [get_clocks {clk_50}] \
    -group [get_clocks {tck}]

# Async reset — no timing path needed
set_false_path -from [get_ports rst_n]

# GPIO I/O pins have no board-level timing model; relax to avoid unconstrained I/O warnings
set_false_path -to   [get_ports {uart_tx xip_csn xip_sck flash_io[*] i2s_ws i2s_sck tdo}]
set_false_path -from [get_ports {uart_rx i2s_sd flash_io[*] tdi tms trst_n}]
