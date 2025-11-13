# Makefile for Verilator simulation of example_soc

.PHONY: all verilate clean run

# Define the root of the KWS-SoC project for relative paths
KWS_ROOT := $(CURDIR)

# Python script for flattening .f files
FLATTEN_SCRIPT := Hazard3/scripts/listfiles
VERILATOR_SOURCES_F := verilator_sources.f

TOP_MODULE := example_soc

# C++ Wrapper file for verilator
TESTBENCH_CPP := jtag_server/jtag_main.cpp jtag_server/logger.cpp

#
# Output executable name
SIM_EXE := sim_main


# Default target
all: verilate run

# Target to generate the flattened Verilog source list
$(VERILATOR_SOURCES_F): soc.f $(FLATTEN_SCRIPT) $(shell find $(KWS_ROOT) -name "*.f")
	@echo "Generating flattened Verilog source list: $(VERILATOR_SOURCES_F)"
	@python3 $(FLATTEN_SCRIPT) -f flat -o $(KWS_ROOT)/$(VERILATOR_SOURCES_F) $(KWS_ROOT)/soc.f

# Target to run Verilator and build the simulation executable
verilate_no_warnings: $(VERILATOR_SOURCES_F) $(TESTBENCH_CPP)
	@echo "Running Verilator to build simulation for $(TOP_MODULE)..."
	verilator --cc -f $(VERILATOR_SOURCES_F) \
		--exe $(TESTBENCH_CPP) \
		--build \
		--vpi \
		--top-module $(TOP_MODULE) \
		-o $(SIM_EXE) \
		-I$(KWS_ROOT)/Hazard3/hdl \
		-CFLAGS "-DVL_DEBUG -std=c++11" \
		-Wall -Wno-CASEINCOMPLETE -Wno-UNUSEDPARAM -Wno-PINMISSING -Wno-WIDTHTRUNC -Wno-GENUNNAMED -Wno-PINCONNECTEMPTY -Wno-UNUSEDSIGNAL -Wno-SYNCASYNCNET -Wno-WIDTHEXPAND -Wno-UNDRIVEN -Wno-DECLFILENAME -Wno-LATCH -Wno-COMBDLY

	@echo "Verilator simulation built. Executable: obj_dir/$(SIM_EXE)"


verilate: $(VERILATOR_SOURCES_F) $(TESTBENCH_CPP)
	@echo "Running Verilator to build simulation for $(TOP_MODULE)..."
	verilator --cc -f $(VERILATOR_SOURCES_F) \
		--exe $(TESTBENCH_CPP) \
		--build \
		--vpi \
		--top-module $(TOP_MODULE) \
		-o $(SIM_EXE) \
		-I$(KWS_ROOT)/Hazard3/hdl \
		-CFLAGS "-DVL_DEBUG -std=c++11" \
		-Wall

	@echo "Verilator simulation built Wno Warnings. Executable: obj_dir/$(SIM_EXE)"

# Target to run the simulation
run: verilate
	@echo "Running simulation..."
	./obj_dir/$(SIM_EXE)
run_no_warnings: verilate_no_warnings
	@echo "Running simulation..."
	./obj_dir/$(SIM_EXE)

# Target to clean up generated files
clean:
	@echo "Cleaning up Verilator generated files..."
	rm -rf obj_dir $(VERILATOR_SOURCES_F) $(SIM_EXE)
