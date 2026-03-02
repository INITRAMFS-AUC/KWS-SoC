# Clock and Reset
set_location_assignment PIN_AF14 -to clk_50 -comment Clock50
set_location_assignment PIN_AB30 -to rst_n  -comment "Switch 0"

# JTAG / Debug
set_location_assignment PIN_W15  -to tck    -comment "GPIO 0"
set_location_assignment PIN_AK2  -to tdi    -comment "GPIO 1"
set_location_assignment PIN_Y16  -to tdo    -comment "GPIO 2"
set_location_assignment PIN_AK3  -to tms    -comment "GPIO 3"
set_location_assignment PIN_AJ1  -to trst_n -comment "GPIO 4"

# UART
set_location_assignment PIN_AJ2  -to uart_rx -comment "GPIO 5"
set_location_assignment PIN_AH2  -to uart_tx -comment "GPIO 6"

# QSPI Flash
set_location_assignment PIN_AH3  -to xip_csn      -comment "GPIO 7"
set_location_assignment PIN_AH4  -to xip_sck      -comment "GPIO 8"
set_location_assignment PIN_AH5  -to flash_io[0]  -comment "GPIO 9"
set_location_assignment PIN_AG1  -to flash_io[1]  -comment "GPIO 10"
set_location_assignment PIN_AG2  -to flash_io[2]  -comment "GPIO 11"
set_location_assignment PIN_AG3  -to flash_io[3]  -comment "GPIO 12"

set_global_assignment -name STRATIX_DEVICE_IO_STANDARD "3.3-V LVTTL"
