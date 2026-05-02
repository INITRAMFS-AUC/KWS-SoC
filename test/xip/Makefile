IVERILOG ?= iverilog
VVP ?= vvp
BUILD_DIR ?= build
TESTBENCH_DIR ?= testbench
XIP_HDL_DIR ?= ../../peris/xip

# Testbenches
TB_CACHE      = ahbl_flash_ctrl_eb_cache_tb
TB_FLASH_CTRL = flash_ctrl_eb_tb
TB_RO_DMC     = ro_dmc_tb

HDL_SRCS = \
	$(XIP_HDL_DIR)/ahbl_flash_ctrl_eb_cache.v \
	$(XIP_HDL_DIR)/flash_ctrl_eb.v \
	$(XIP_HDL_DIR)/ro_cache.v \
	$(XIP_HDL_DIR)/sst26wf080b.v

# Build targets
TB_CACHE_SIM      = $(BUILD_DIR)/$(TB_CACHE).out
TB_FLASH_CTRL_SIM = $(BUILD_DIR)/$(TB_FLASH_CTRL).out
TB_RO_DMC_SIM     = $(BUILD_DIR)/$(TB_RO_DMC).out

.PHONY: all clean cache flash_ctrl ro_dmc ro_dmc_cwf run-cache run-flash-ctrl run-ro-dmc run-ro-dmc-cwf run-testbench

all: run-testbench

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Unified Pattern Rule
$(BUILD_DIR)/%.out: $(TESTBENCH_DIR)/%.v $(HDL_SRCS) | $(BUILD_DIR)
	$(IVERILOG) -o $@ -I$(TESTBENCH_DIR) $< $(HDL_SRCS)
	cp $(TESTBENCH_DIR)/init.hex $(BUILD_DIR)/

# CWF variant of ro_dmc TB — same source, +define+CWF so ro_cache.v
# pulls in the critical-word-first logic and the TB tightens its
# stall bounds.  Built into a separate output so both flavours can be
# kept side-by-side.
$(BUILD_DIR)/$(TB_RO_DMC)_cwf.out: $(TESTBENCH_DIR)/$(TB_RO_DMC).v $(HDL_SRCS) | $(BUILD_DIR)
	$(IVERILOG) -DCWF -o $@ -I$(TESTBENCH_DIR) $< $(HDL_SRCS)
	cp $(TESTBENCH_DIR)/init.hex $(BUILD_DIR)/

# Friendly alias targets
cache: $(TB_CACHE_SIM)
flash_ctrl: $(TB_FLASH_CTRL_SIM)
ro_dmc: $(TB_RO_DMC_SIM)
ro_dmc_cwf: $(BUILD_DIR)/$(TB_RO_DMC)_cwf.out

# Run targets
run-cache: $(TB_CACHE_SIM)
	cd $(BUILD_DIR) && $(VVP) $(TB_CACHE).out

run-flash-ctrl: $(TB_FLASH_CTRL_SIM)
	cd $(BUILD_DIR) && $(VVP) $(TB_FLASH_CTRL).out

run-ro-dmc: $(TB_RO_DMC_SIM)
	cd $(BUILD_DIR) && $(VVP) $(TB_RO_DMC).out

run-ro-dmc-cwf: $(BUILD_DIR)/$(TB_RO_DMC)_cwf.out
	cd $(BUILD_DIR) && $(VVP) $(TB_RO_DMC)_cwf.out

run-testbench: run-cache run-ro-dmc run-ro-dmc-cwf run-flash-ctrl

clean:
	rm -rf $(BUILD_DIR)
