set_global_assignment -name STRATIX_DEVICE_IO_STANDARD "3.3-V LVTTL"

# Clock and Reset
set_location_assignment PIN_AF14 -to clk_50 -comment Clock50
set_location_assignment PIN_AB30 -to rst_n  -comment "Switch 0"

# JTAG / Debug
set_location_assignment PIN_AG1  -to tck    -comment "GPIO 10"
set_location_assignment PIN_AJ1  -to tdi    -comment "GPIO 4"
set_location_assignment PIN_AH4  -to tdo    -comment "GPIO 8"
set_location_assignment PIN_AH2  -to tms    -comment "GPIO 6"
set_location_assignment PIN_Y16  -to trst_n -comment "GPIO 2"

# UART
set_location_assignment PIN_AK2  -to uart_rx -comment "GPIO 1"
set_location_assignment PIN_W15  -to uart_tx -comment "GPIO 0"

# I2S
set_location_assignment PIN_AG7 -to i2s_ws  -comment "GPIO Connection [15]"
set_location_assignment PIN_AF4 -to i2s_sd  -comment "GPIO Connection [17]"
set_location_assignment PIN_AG5 -to i2s_sck -comment "GPIO Connection [13]"

# QSPI Flash Pin Assignments
set_location_assignment PIN_AD7  -to xip_csn      -comment "GPIO [27]"
set_location_assignment PIN_AE11 -to xip_sck      -comment "GPIO [25]"
set_location_assignment PIN_AC12 -to flash_io[0]  -comment "GPIO [33]"
set_location_assignment PIN_AD10 -to flash_io[1]  -comment "GPIO [29]"
set_location_assignment PIN_AD12 -to flash_io[2]  -comment "GPIO [31]"
set_location_assignment PIN_AA12 -to flash_io[3]  -comment "GPIO [35]"


set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to tms
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to tdi
# XIP

# Pull-up on Chip Select to ensure the flash stays deselected during boot/idle
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to xip_csn

# Pull-up on IO2 (WP#) and IO3 (HOLD#) to prevent suspended states or
# accidental write protection while operating in Standard 1-bit SPI mode
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to flash_io[2]
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to flash_io[3]

# (Optional but recommended) Pull-ups on IO0 and IO1 for bus stability
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to flash_io[0]
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to flash_io[1]

