# Makefile for Verilator simulation of example_soc with JTAG
.PHONY: all verilate clean run

# Define the root of the KWS-SoC project for relative paths
KWS_ROOT := $(CURDIR)

# Python script for flattening .f files
FLATTEN_SCRIPT := Hazard3/scripts/listfiles
VERILATOR_SOURCES_F := verilator_sources.f
TOP_MODULE := example_soc

# C++ source files for Verilator (NO jtag_vpi.c!)
TESTBENCH_CPP := soc_jtag_wrapper.cpp \
                 $(KWS_ROOT)/jtag_vpi/jtagServer.cpp \
                 $(KWS_ROOT)/jtag_vpi/jtag_common.c

# Output executable name
SIM_EXE := sim_main

# JTAG server port
JTAG_PORT ?= 5555

# Default target
all: verilate

# Target to generate the flattened Verilog source list
$(VERILATOR_SOURCES_F): soc.f $(FLATTEN_SCRIPT)
	@echo "Generating flattened Verilog source list: $(VERILATOR_SOURCES_F)"
	@python3 $(FLATTEN_SCRIPT) -f flat -o $(KWS_ROOT)/$(VERILATOR_SOURCES_F) $(KWS_ROOT)/soc.f

# Target to run Verilator and build the simulation executable
verilate: $(VERILATOR_SOURCES_F) $(TESTBENCH_CPP)
	@echo "Running Verilator to build simulation for $(TOP_MODULE)..."
	verilator --cc -f $(VERILATOR_SOURCES_F) \
		--exe $(TESTBENCH_CPP) \
		--build \
		--top-module $(TOP_MODULE) \
		-o $(SIM_EXE) \
		-I$(KWS_ROOT)/Hazard3/hdl \
		-I$(KWS_ROOT)/jtag_vpi \
		-CFLAGS "-DVL_DEBUG -std=c++11 -I$(KWS_ROOT)/jtag_vpi" \
		-LDFLAGS "-lpthread" \
		-Wall -Wno-CASEINCOMPLETE -Wno-UNUSEDPARAM -Wno-PINMISSING \
		-Wno-WIDTHTRUNC -Wno-GENUNNAMED -Wno-PINCONNECTEMPTY \
		-Wno-UNUSEDSIGNAL -Wno-SYNCASYNCNET -Wno-WIDTHEXPAND \
		-Wno-UNDRIVEN -Wno-DECLFILENAME -Wno-LATCH -Wno-COMBDLY
	@echo "Verilator simulation built. Executable: obj_dir/$(SIM_EXE)"

# Target to run the simulation
run: verilate
	@echo "========================================="
	@echo "Starting Hazard3 JTAG Simulation"
	@echo "JTAG server will listen on port $(JTAG_PORT)"
	@echo "Connect with: openocd -f openocd_hazard3.cfg"
	@echo "========================================="
	./obj_dir/$(SIM_EXE)

# Target to clean up generated files
clean:
	@echo "Cleaning up Verilator generated files..."
	rm -rf obj_dir $(VERILATOR_SOURCES_F)

.PHONY: help
help:
	@echo "Makefile targets:"
	@echo "  all        - Build the simulation (default)"
	@echo "  verilate   - Run Verilator and build"
	@echo "  run        - Build and run simulation"
	@echo "  clean      - Remove generated files"
	@echo ""
	@echo "Environment variables:"
	@echo "  JTAG_PORT  - JTAG server port (default: 5555)"
