IVERILOG ?= iverilog
VVP ?= vvp
BUILD_DIR ?= build
TESTBENCH_DIR ?= testbench
XIP_HDL_DIR ?= ../../xip

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

.PHONY: all clean cache flash_ctrl ro_dmc run-cache run-flash-ctrl run-ro-dmc run-testbench

all: run-testbench

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Unified Pattern Rule
$(BUILD_DIR)/%.out: $(TESTBENCH_DIR)/%.v $(HDL_SRCS) | $(BUILD_DIR)
	$(IVERILOG) -o $@ -I$(TESTBENCH_DIR) $< $(HDL_SRCS)
	cp $(TESTBENCH_DIR)/init.hex $(BUILD_DIR)/

# Friendly alias targets
cache: $(TB_CACHE_SIM)
flash_ctrl: $(TB_FLASH_CTRL_SIM)
ro_dmc: $(TB_RO_DMC_SIM)

# Run targets
run-cache: $(TB_CACHE_SIM)
	cd $(BUILD_DIR) && $(VVP) $(TB_CACHE).out

run-flash-ctrl: $(TB_FLASH_CTRL_SIM)
	cd $(BUILD_DIR) && $(VVP) $(TB_FLASH_CTRL).out

run-ro-dmc: $(TB_RO_DMC_SIM)
	cd $(BUILD_DIR) && $(VVP) $(TB_RO_DMC).out

run-testbench: run-cache run-ro-dmc run-flash-ctrl

clean:
	rm -rf $(BUILD_DIR)
