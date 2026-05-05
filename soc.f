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
# Skip libfpga/mem/ahb_cache.f — we use our own ro_dmc (peris/xip/ro_cache.v),
# never libfpga's ahb_cache_readonly / ahb_cache_writeback / cache_mem_*.
# sram_sync.v is still pulled in via ahb_sync_sram.f below.
# Side benefit: avoids compiling ahb_cache_writeback.v which has an upstream
# stray `;;` that Quartus (correctly) rejects but other tools accept silently.
list Hazard3/example_soc/libfpga/mem/ahb_sync_sram.f

list busfabric/busfabric.f

# IO and XIP peripherals

list peris/peris.f

# XIP sample playback (elaborated only when XIP_PLAYBACK is defined)
file peris/xip/playback_buf.v
file peris/xip/xip_loader.v
file peris/xip/i2s_sd_driver.v

# Conv1D hardware accelerator

file peris/conv1d_accel/conv1d_accel.v
