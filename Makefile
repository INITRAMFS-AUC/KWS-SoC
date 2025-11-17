# Makefile for Verilator simulation of example_soc


# Define the root of the KWS-SoC project for relative paths
KWS_ROOT := $(CURDIR)

# Python script for flattening .f files
FLATTEN_SCRIPT := Hazard3/scripts/listfiles
VERILATOR_SOURCES_F := verilator_sources.f
DOF := soc.f
BUILD_DIR := build-$(patsubst %.f,%,$(DOTF))


TOP := example_soc
HDL := $(KWS_ROOT)/Hazard3/hdl
CONFIG := default

.PHONY: clean all lint

all: $(TBEXEC)

$(VERILATOR_SOURCES_F): soc.f $(FLATTEN_SCRIPT) $(shell find $(KWS_ROOT) -name "*.f")
	@echo "Generating flattened Verilog source list: $(VERILATOR_SOURCES_F)"
	@python3 $(FLATTEN_SCRIPT) -f flat -o $(KWS_ROOT)/$(VERILATOR_SOURCES_F) $(KWS_ROOT)/soc.f


SYNTH_CMD += read_verilog -I $(HDL) -DCONFIG_HEADER="config_$(CONFIG).vh" $(VERILATOR_SOURCES_F);
SYNTH_CMD += hierarchy -top $(TOP);
SYNTH_CMD += write_cxxrtl $(BUILD_DIR)/dut.cpp

$(BUILD_DIR)/dut.cpp: $(VERILATOR_SOURCES_F) $(wildcard *.vh)
	mkdir -p $(BUILD_DIR)
	yosys -p '$(SYNTH_CMD)' 2>&1 > $(BUILD_DIR)/cxxrtl.log

clean::
	rm -rf $(BUILD_DIR) $(TBEXEC)

$(TBEXEC): $(BUILD_DIR)/dut.cpp example_soc_tb.cpp
	$(CLANGXX) -O3 -std=c++14 $(addprefix -D,$(CDEFINES) $(CDEFINES_$(DOTF))) -I $(shell yosys-config --datdir)/include/backends/cxxrtl/runtime -I $(BUILD_DIR) example_soc_tb.cpp -o $(TBEXEC)

lint:
	verilator --lint-only --top-module $(TOP) -I$(HDL) $(VERILATOR_SOURCES_F)
