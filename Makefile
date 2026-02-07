# This Makefile is concerned with the simulation and quartus development workflows #
# initramfs, 2025-2026 #

# Root directory Makefile
HAZARD3_ROOT := ./Hazard3
include $(HAZARD3_ROOT)/project_paths.mk

## COMMON
TOP       := kws_soc
DOTF      := soc.f

## YOSYS VARS
YOSYS_CONFIG    := default
TBEXEC    			:= kws_soc_tb
YOSYS_BUILD_DIR := yosys_build
CLANGXX   			:= clang++

## QUARTUS VARS
QUARTUS_DIR 		:= quartus
QUARTUS_PROJECT := $(QUARTUS_DIR)/KWS-SoC
QUARTUS_SRC_DIR := $(QUARTUS_DIR)/quartus_src_dir
QIP_FILE 				:= $(QUARTUS_SRC_DIR)/autogen_sources.qip
# Constraints
CONSTRAINTS_SRC ?= DE10_Constraints.tcl
TOP_FPGA        := $(QUARTUS_DIR)/fpga_top

# FPGA device family and party, set by env shell with defaults being the de-10 standard
FPGA_FAMILY ?= "Cyclone V"
FPGA_PART   ?= 5CSXFC6D6F31C6

# 128k Memory
SRAM_DEPTH ?= 32768

# important: these show be in PATH, locate your quartus installation
MAP := quartus_map
FIT := quartus_fit
ASM := quartus_asm
STA := quartus_sta
PGM := quartus_pgm
SH  := quartus_sh

# Use listfiles script to generate file list from .f file
FILE_LIST := $(shell python3 $(SCRIPTS)/listfiles -f flat $(DOTF))
FILE_LIST_NO_VH := $(shell python3 $(SCRIPTS)/listfiles -f flat --auto-vh $(DOTF))

.PHONY: clean all lint sim quartus_prep map fit asm sta program check_timing config

all: $(TBEXEC)

# Yosys synthesis command to generate CXXRTL C++ code
YOSYS_SYNTH_CMD += read_verilog -I$(HDL) -DCONFIG_HEADER="config_$(YOSYS_CONFIG).vh" $(FILE_LIST);
YOSYS_SYNTH_CMD += hierarchy -top $(TOP);
YOSYS_SYNTH_CMD += write_cxxrtl $(YOSYS_BUILD_DIR)/dut.cpp

$(YOSYS_BUILD_DIR)/dut.cpp: $(FILE_LIST) $(wildcard *.vh) $(DOTF)
	mkdir -p $(YOSYS_BUILD_DIR)
	yosys -p '$(YOSYS_SYNTH_CMD)' 2>&1 | tee $(YOSYS_BUILD_DIR)/cxxrtl.log


$(TBEXEC): $(YOSYS_BUILD_DIR)/dut.cpp kws_soc_tb.cpp
	$(CLANGXX) -O3 -std=c++14 $(addprefix -D,$(CDEFINES)) \
		-I$(shell yosys-config --datdir)/include/backends/cxxrtl/runtime \
		-I$(YOSYS_BUILD_DIR) \
		kws_soc_tb.cpp -o $(TBEXEC)

lint:
	verilator --lint-only --top-module $(TOP) -I$(HDL) $(FILE_LIST)

# Creates a working directory and symlinks all source files into it.
# We use abspath so the links remain valid when placed inside the subdir.
# Only Done on First Clone of Repo #
quartus_prep:
	# TODO: make this in a separate tcl script along with the config target
	#@mkdir -p $(QUARTUS_SRC_DIR)
	#@echo "# Auto-generated source list" > $(QIP_FILE)
	#$(foreach src,$(FILE_LIST_NO_VH), \
	#	ln -sf $(abspath $(src)) $(QUARTUS_SRC_DIR)/$(notdir $(src)); \
	#	f=$(notdir $(src)); \
	#	if [ "$${f##*.}" != "vh" ]; then \
	#		echo "set_global_assignment -name SYSTEMVERILOG_FILE [file join \$$::quartus(qip_path) $$f]" >> $(QIP_FILE); \
	#	fi; \
	#)
	#@echo "Quartus prepared: $(QIP_FILE) updated."
	mkdir -p $(QUARTUS_SRC_DIR)
	$(foreach src,$(FILE_LIST_NO_VH),ln -sf $(abspath $(src)) $(QUARTUS_SRC_DIR)/$(notdir $(src));)
	@echo "Quartus working directory prepared at: $(QUARTUS_SRC_DIR)"

# Helper target to run the cxxrtl testbench with default port
sim: $(TBEXEC)
	./$(TBEXEC) --port 9824

# Helper target to run cxxrtl testbench with VCD dumping
sim-vcd: $(TBEXEC)
	./$(TBEXEC) --port 9824 --vcd waves.vcd

config:
	@echo "--- Setting up Quartus Project ---"
	# We export these variables solely for the next command line
	export QUARTUS_PROJECT=$(QUARTUS_PROJECT); \
	export QUARTUS_FAMILY=$(FPGA_FAMILY); \
	export QUARTUS_DEVICE=$(FPGA_PART); \
	export QUARTUS_TOP_MODULE=$(TOP_FPGA); \
	export QUARTUS_SRCS="$(FILE_LIST_NO_VH)"; \
	export QUARTUS_CONSTRAINTS=$(CONSTRAINTS_SRC); \
	$(SH) -t $(QUARTUS_DIR)/setup_project.tcl

# 1. Synthesis (Map)
# This is where we inject the SRAM_DEPTH from the environment/makefile
map: config
	@echo "--- Synthesizing with SRAM_DEPTH=$(SRAM_DEPTH) ---"
	$(MAP) $(QUARTUS_PROJECT) --source=$(TOP_FPGA) --verilog_macro="SRAM_DEPTH=$(SRAM_DEPTH)"

# 2. Fitter (Place & Route)
# This stage automatically reads your .sdc and .qsf files
fit: map
	@echo "--- Fitting ---"
	$(FIT) $(QUARTUS_PROJECT)

# 3. Assembler (Generate Bitstream)
asm: fit
	@echo "--- Generating Bitstream (.sof) ---"
	$(ASM) $(QUARTUS_PROJECT)

# 4. Timing Analysis
# Updates the timing netlist
sta: fit
	@echo "--- Running Timing Analysis ---"
	$(STA) $(QUARTUS_PROJECT)

# 5. Check Timing (CI Helper)
# Fails the build if timing is not met
check_timing: sta
	@echo "--- Checking for Timing Violations ---"
	# Grep the summary for "Critical Warning" or negative slack
	# This is a simple check; for robust CI, parse the report files in output_files/
	$(SH) --tcl_eval "project_open $(QUARTUS_PROJECT); set x [get_timing_analysis_summary_results -model slow]; puts \$$x; project_close"

# 6. Programming
# JTAG mode, auto-detect cable. Replace 'USB-Blaster' with your specific cable name if needed.
program: asm
	@echo "--- Programming FPGA ---"
	$(PGM) -c "USB-Blaster" -m JTAG -o "p;output_files/$(QUARTUS_PROJECT).sof"

clean::
	rm -rf $(YOSYS_BUILD_DIR) $(TBEXEC) *.vcd \
				 $(QUARTUS_SRC_DIR) $(QUARTUS)/db/ $(QUARTUS)/incremental_db/ $(QUARTUS)/output_files/ \
				 $(QUARTUS)/*.qws $(QUARTUS)/*.sof $(QUARTUS)/*.pof $(QUARTUS)/*.rpt $(QUARTUS)/*.cdf
