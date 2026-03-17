# XIP Test Makefile
ROOT_DIR = ../..
XIP_HDL_DIR = $(ROOT_DIR)/xip
TESTBENCH_DIR = testbench
BUILD_DIR = build

# Tools
IVERILOG = iverilog
VVP = vvp

# Testbenches
TB_CACHE = ahbl_flash_ctrl_eb_cache_tb
TB_FLASH_CTRL = flash_ctrl_eb_tb
TB_RO_DMC = ro_dmc_tb

# HDL Sources
# TODO: Make ths read xip.f for the first few files
HDL_SRCS = \
	$(XIP_HDL_DIR)/ahbl_flash_ctrl_eb_cache.v \
	$(XIP_HDL_DIR)/flash_ctrl_eb.v \
	$(XIP_HDL_DIR)/ro_cache.v \
	$(XIP_HDL_DIR)/sst26wf080b.v

# Build targets
TB_CACHE_SIM = $(BUILD_DIR)/$(TB_CACHE).out
TB_FLASH_CTRL_SIM = $(BUILD_DIR)/$(TB_FLASH_CTRL).out
TB_RO_DMC_SIM = $(BUILD_DIR)/$(TB_RO_DMC).out

.PHONY: all clean run run-cache run-flash-ctrl run-ro-dmc

all: run-testbench

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build cache testbench
cache: $(BUILD_DIR) $(TB_CACHE_SIM)

$(TB_CACHE_SIM): $(BUILD_DIR) $(TESTBENCH_DIR)/$(TB_CACHE).v $(HDL_SRCS) $(TESTBENCH_DIR)/tb.vh
	$(IVERILOG) -o $@ \
		-I$(TESTBENCH_DIR) \
		$(TESTBENCH_DIR)/$(TB_CACHE).v \
		$(HDL_SRCS)
	@echo "Copy init.hex to build directory for simulation"
	cp $(TESTBENCH_DIR)/init.hex $(BUILD_DIR)/

# Build flash_ctrl testbench
flash_ctrl: $(BUILD_DIR) $(TB_FLASH_CTRL_SIM)

$(TB_FLASH_CTRL_SIM): $(BUILD_DIR) $(TESTBENCH_DIR)/$(TB_FLASH_CTRL).v $(HDL_SRCS) $(TESTBENCH_DIR)/tb.vh
	$(IVERILOG) -o $@ \
		-I$(TESTBENCH_DIR) \
		$(TESTBENCH_DIR)/$(TB_FLASH_CTRL).v \
		$(HDL_SRCS)
	@echo "Copy init.hex to build directory for simulation"
	cp $(TESTBENCH_DIR)/init.hex $(BUILD_DIR)/

# Build ro_dmc testbench
ro_dmc: $(BUILD_DIR) $(TB_RO_DMC_SIM)

$(TB_RO_DMC_SIM): $(BUILD_DIR) $(TESTBENCH_DIR)/$(TB_RO_DMC).v $(HDL_SRCS) $(TESTBENCH_DIR)/tb.vh
	$(IVERILOG) -o $@ \
		-I$(TESTBENCH_DIR) \
		$(TESTBENCH_DIR)/$(TB_RO_DMC).v \
		$(HDL_SRCS)
	@echo "Copy init.hex to build directory for simulation"
	cp $(TESTBENCH_DIR)/init.hex $(BUILD_DIR)/

# Run simulations
run-cache: $(TB_CACHE_SIM)
	cd $(BUILD_DIR) && $(VVP) $(TB_CACHE).out

run-flash-ctrl: $(TB_FLASH_CTRL_SIM)
	cd $(BUILD_DIR) && $(VVP) $(TB_FLASH_CTRL).out

run-ro-dmc: $(TB_RO_DMC_SIM)
	cd $(BUILD_DIR) && $(VVP) $(TB_RO_DMC).out

# Run all simulations
run-testbench: run-cache run-ro-dmc run-flash-ctrl

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
