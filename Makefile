# This Makefile is concerned with the simulation and quartus development workflows #
# initramfs, 2025-2026 #

# Root directory Makefile
HAZARD3_ROOT := ./Hazard3
include $(HAZARD3_ROOT)/project_paths.mk

## COMMON
TOP       := kws_soc
DOTF      := soc.f
ROOT_DIR  := $(shell pwd)

# Use listfiles script to generate file list from .f file
FILE_LIST 	 := $(shell python3 $(SCRIPTS)/listfiles -f flat $(DOTF))
FILE_LIST_VH := $(shell python3 $(SCRIPTS)/listfiles -f flat --auto-vh $(DOTF))
SRC_LIST_IP_REL := $(shell find quartus/ip -name "*.qip")
SRC_LIST_IP     := $(abspath $(SRC_LIST_IP_REL))

## YOSYS VARS
YOSYS_CONFIG    := default
TBEXEC    			:= kws_soc_tb
YOSYS_BUILD_DIR := yosys_build
CLANGXX   			:= clang++

## QUARTUS VARS
QUARTUS_DIR 				:= $(ROOT_DIR)/quartus
QUARTUS_PROJECT 		:= $(QUARTUS_DIR)/KWS-SoC
QUARTUS_SRC_DIR 		:= $(QUARTUS_DIR)/quartus_src_dir
QIP_COMPONENTS		  := $(shell find $(QUARTUS_DIR)/ip -name "*.qip")
ALL_QUARTUS_SRCS 		:= $(sort $(FILE_LIST_VH) $(SRC_LIST_IP))

# Sentinel files for Make to track
QSF_FILE  := $(QUARTUS_PROJECT).qsf
# Reports that indicate a stage is finished
MAP_RPT   := $(QUARTUS_DIR)/output_files/KWS-SoC.map.rpt
FIT_RPT   := $(QUARTUS_DIR)/output_files/KWS-SoC.fit.rpt
ASM_RPT   := $(QUARTUS_DIR)/output_files/KWS-SoC.asm.rpt
SOF_FILE  := $(QUARTUS_DIR)/output_files/KWS-SoC.sof

# Constraints
CONSTRAINTS_SRC ?= $(QUARTUS_DIR)/CycloneV/DE10_Constraints.tcl
TOP_FPGA        := fpga_top

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

# Helper target to run the cxxrtl testbench with default port
sim: $(TBEXEC)
	./$(TBEXEC) --port 9824

# Helper target to run cxxrtl testbench with VCD dumping
sim-vcd: $(TBEXEC)
	./$(TBEXEC) --port 9824 --vcd waves.vcd

# 0. Project Generation
$(QSF_FILE): $(QUARTUS_DIR)/setup_project.tcl Makefile
	@echo "--- Setting up Quartus Project ---"
	# We export these variables solely for the next command line
	export QUARTUS_PROJECT=$(QUARTUS_PROJECT); \
	export QUARTUS_FAMILY=$(FPGA_FAMILY); \
	export QUARTUS_DEVICE=$(FPGA_PART); \
	export QUARTUS_TOP_MODULE=$(TOP_FPGA); \
	export QUARTUS_SRCS="$(ALL_QUARTUS_SRCS)"; \
	export QUARTUS_CONSTRAINTS=$(CONSTRAINTS_SRC); \
	export NPROC=$(shell nproc); \
	$(SH) -t $(QUARTUS_DIR)/setup_project.tcl

# 1. Synthesis (Map)
$(MAP_RPT): $(QSF_FILE) $(ALL_QUARTUS_SRCS)
	@echo "--- Synthesizing with SRAM_DEPTH=$(SRAM_DEPTH) ---"
	$(MAP) $(QUARTUS_PROJECT) --verilog_macro="SRAM_DEPTH=$(SRAM_DEPTH)"

# 2. Fitting (Place & Route)
$(FIT_RPT): $(MAP_RPT)
	@echo "--- Fitting ---"
	$(FIT) $(QUARTUS_PROJECT)

# 3. Bitstream Generation (Assembler)
$(ASM_RPT): $(FIT_RPT)
	@echo "--- Generating Bitstream ---"
	$(ASM) $(QUARTUS_PROJECT)

# These just point to the real files above
.PHONY: config map fit asm prog
config: $(QSF_FILE)
map:    $(MAP_RPT)
fit:    $(FIT_RPT)
asm:    $(ASM_RPT)

sta: fit
	@echo "--- Running Timing Analysis ---"
	$(STA) $(QUARTUS_PROJECT)

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
