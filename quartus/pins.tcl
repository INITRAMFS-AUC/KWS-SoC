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
set_location_assignment PIN_AJ2  -to uart_rx -comment "Gpio 5"
set_location_assignment PIN_AH2  -to uart_tx -comment "gpio 6"
