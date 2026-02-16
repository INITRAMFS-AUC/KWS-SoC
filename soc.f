# SoC integration file

file kws_soc.v
file kws_soc_config.vh

# CPU + debug components

list Hazard3/hdl/hazard3.f
list Hazard3/hdl/debug/dtm/hazard3_jtag_dtm.f
list Hazard3/hdl/debug/dm/hazard3_dm.f

# RISC-V timer

list Hazard3/example_soc/soc/peri/hazard3_riscv_timer.f

# Generic SoC components from libfpga

file Hazard3/example_soc/libfpga/common/reset_sync.v

list Hazard3/example_soc/libfpga/peris/uart/uart.f
list Hazard3/example_soc/libfpga/peris/spi_03h_xip/spi_03h_xip.f
list Hazard3/example_soc/libfpga/mem/ahb_cache.f
list Hazard3/example_soc/libfpga/mem/ahb_sync_sram.f

list Hazard3/example_soc/libfpga/busfabric/ahbl_crossbar.f
file Hazard3/example_soc/libfpga/busfabric/ahbl_to_apb.v
file Hazard3/example_soc/libfpga/busfabric/apb_splitter.v

list xip/xip.f
