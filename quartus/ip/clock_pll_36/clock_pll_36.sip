set_global_assignment -entity "clock_pll_36" -library "lib_clock_pll_36" -name IP_TOOL_NAME "altera_pll"
set_global_assignment -entity "clock_pll_36" -library "lib_clock_pll_36" -name IP_TOOL_VERSION "25.1"
set_global_assignment -entity "clock_pll_36" -library "lib_clock_pll_36" -name IP_TOOL_ENV "mwpim"
set_global_assignment -library "lib_clock_pll_36" -name SPD_FILE [file join $::quartus(sip_path) "clock_pll_36.spd"]

set_global_assignment -library "lib_clock_pll_36" -name MISC_FILE [file join $::quartus(sip_path) "clock_pll_36_sim/clock_pll_36.vo"]
