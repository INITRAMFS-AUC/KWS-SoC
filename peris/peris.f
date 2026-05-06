# I2S
file i2s/i2s_apb/i2s_itr2/i2s_rx_core.v
file i2s/i2s_apb/i2s_itr2/apb_i2s_receiver.v
file i2s/i2s_apb/i2s_itr2/i2s_regs.v

# XIP
file xip/ahbl_flash_ctrl_eb_cache.v
file xip/flash_ctrl_eb.v
file xip/ro_cache.v
# XIP sample player (XIP_PLAYBACK only — module instantiated under `ifdef in kws_soc.v).
# Always compiled so synthesis sees the source even when the gate is off.
file xip/xip_sample_player.v

# DMA Controller
file MS_DMAC_AHBL/hdl/rtl/MS_DMAC_AHBL.pp.v

# Conv1D accelerator
file conv1d_accel/conv1d_accel.v

# Bus snooper (debug, always compiled; only instantiated under `ifdef DEBUG_SNOOPER)
file snooper/bus_snooper.v
