# Makefile for example_soc CXXRTL testbench
# Root directory Makefile

# Include project paths from Hazard3
HAZARD3_ROOT := ./Hazard3
include $(HAZARD3_ROOT)/project_paths.mk

TOP       := example_soc
DOTF      := soc.f
CONFIG    := default
TBEXEC    := example_soc_tb

BUILD_DIR := build

# Use listfiles script to generate file list from .f file
FILE_LIST := $(shell python3 $(SCRIPTS)/listfiles -f flat $(DOTF))

# Note: clang++-18 has a >20x compile time regression, even at low
# optimisation levels. I have tried clang++-16 and clang++-17, both fine.
CLANGXX   := clang++

.PHONY: clean all lint run

all: $(TBEXEC)

# Yosys synthesis command to generate CXXRTL C++ code
SYNTH_CMD += read_verilog -I$(HDL) -DCONFIG_HEADER="config_$(CONFIG).vh" $(FILE_LIST);
SYNTH_CMD += hierarchy -top $(TOP);
SYNTH_CMD += write_cxxrtl $(BUILD_DIR)/dut.cpp

$(BUILD_DIR)/dut.cpp: $(FILE_LIST) $(wildcard *.vh) $(DOTF)
	mkdir -p $(BUILD_DIR)
	yosys -p '$(SYNTH_CMD)' 2>&1 | tee $(BUILD_DIR)/cxxrtl.log

clean::
	rm -rf $(BUILD_DIR) $(TBEXEC)

$(TBEXEC): $(BUILD_DIR)/dut.cpp example_soc_tb.cpp
	$(CLANGXX) -O3 -std=c++14 $(addprefix -D,$(CDEFINES)) \
		-I$(shell yosys-config --datdir)/include/backends/cxxrtl/runtime \
		-I$(BUILD_DIR) \
		example_soc_tb.cpp -o $(TBEXEC)

lint:
	verilator --lint-only --top-module $(TOP) -I$(HDL) $(FILE_LIST)

# Helper target to run the testbench with default port
run: $(TBEXEC)
	./$(TBEXEC) --port 9824

# Helper target to run with VCD dumping
run-vcd: $(TBEXEC)
	./$(TBEXEC) --port 9824 --vcd waves.vcd
