# This Makefile is concerned with the simulation and quartus development workflows #
# initramfs, 2025-2026 #

# Root directory Makefile
HAZARD3_ROOT := ./Hazard3
include $(HAZARD3_ROOT)/project_paths.mk

## COMMON
TOP       := kws_soc
DOTF      := soc.f
ROOT_DIR  := $(shell pwd)

## SIM  
SIM_DIR	:= $(ROOT_DIR)/sim

# Use listfiles script to generate file list from .f file
FILE_LIST			:= $(shell python3 $(SCRIPTS)/listfiles -f flat $(DOTF))
FILE_LIST_VH 	:= $(shell python3 $(SCRIPTS)/listfiles -f flat --auto-vh $(DOTF))

# FPGA device family and party, set by env shell with defaults being the de-10 standard
FPGA_FAMILY ?= "Cyclone V"
FPGA_PART   ?= 5CSXFC6D6F31C6
FPGA_BOARD  ?= DE10S
FPGA_FAMILY_CLEAN := $(strip $(subst ",,$(FPGA_FAMILY)))

ifeq ($(FPGA_FAMILY_CLEAN), Cyclone IV E)
    # Cyclone IV Settings
    PLL_SRC       	:= quartus/ip/ALTPLL_25/ALTPLL_25.qip
    VERILOG_MACROS 	+= CYCLONE_IV=1
else
    # Default to Cyclone V
    PLL_SRC					:= quartus/ip/clock_pll_36/clock_pll_36.qip
    VERILOG_MACROS 	+= CYCLONE_V=1
endif
SRC_LIST_IP     		:= $(abspath $(PLL_SRC))

BUILD_DIR						:= build
## YOSYS VARS
YOSYS_CONFIG    		:= default
YOSYS_BUILD_DIR 		:= $(BUILD_DIR)/yosys
TBEXEC    					:= $(YOSYS_BUILD_DIR)/yosys/kws_soc_tb
CLANGXX   					:= clang++

## QUARTUS VARS
QUARTUS_DIR 				:= $(ROOT_DIR)/quartus
QUARTUS_PROJECT 		:= $(QUARTUS_DIR)/KWS-SoC
QUARTUS_SRC_DIR 		:= $(QUARTUS_DIR)/quartus_src_dir
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

# Clock config
# WARNING TODO: Must be set to 36 as current PLL setup does not allow for anything else
CLK_MHZ ?= 36

# 128k Memory
SRAM_DEPTH ?= 32768

# UART config
UART_BAUD_RATE ?= 115200
# The remaining are hardcoded in uart_mini and thus useless
UART_DATA_WIDTH := 8
UART_PARITY := N
UART_STOP_BITS := 1
UART_FLOW_CONTROL := 0

UART_CFLAGS := -DCLK_MHZ=$(CLK_MHZ) \
               -DUART_BAUD_RATE=$(UART_BAUD_RATE) \
               -DUART_DATA_WIDTH=$(UART_DATA_WIDTH) \
               -DUART_STOP_BITS=$(UART_STOP_BITS)


# Parity
ifeq ($(UART_PARITY), N)
    UART_CFLAGS += -DUART_PARITY_NONE
else ifeq ($(UART_PARITY), E)
    UART_CFLAGS += -DUART_PARITY_EVEN
else ifeq ($(UART_PARITY), O)
    UART_CFLAGS += -DUART_PARITY_ODD
else
    $(error Invalid UART_PARITY: $(UART_PARITY). Use N, E, or O)
endif

# Flow Control
ifeq ($(UART_FLOW_CONTROL), 1)
    UART_CFLAGS += -DUART_FLOW_CTRL_EN
endif

export GLOBAL_UART_CONFIG := $(UART_CFLAGS)

# important: these show be in PATH, locate your quartus installation
MAP := quartus_map
FIT := quartus_fit
ASM := quartus_asm
STA := quartus_sta
PGM := quartus_pgm
SH  := quartus_sh

.PHONY: clean lint map fit asm sta program test check_timing config sim_verilator sim_yosys

# Yosys synthesis command to generate CXXRTL C++ code
YOSYS_SYNTH_CMD += read_verilog -I$(HDL) -DCONFIG_HEADER="config_$(YOSYS_CONFIG).vh" $(FILE_LIST);
YOSYS_SYNTH_CMD += hierarchy -top $(TOP);
YOSYS_SYNTH_CMD += write_cxxrtl $(YOSYS_BUILD_DIR)/dut.cpp

$(YOSYS_BUILD_DIR)/dut.cpp: $(FILE_LIST) $(wildcard *.vh) $(DOTF)
	mkdir -p $(YOSYS_BUILD_DIR)
	yosys -p '$(YOSYS_SYNTH_CMD)'


$(TBEXEC): $(YOSYS_BUILD_DIR)/dut.cpp kws_soc_tb.cpp sim/i2s_mic_sim.cpp
	$(CLANGXX) -O3 -std=c++14 $(addprefix -D,$(CDEFINES)) $(UART_CFLAGS) \
		-I$(shell yosys-config --datdir)/include/backends/cxxrtl/runtime \
		-I$(YOSYS_BUILD_DIR) \
		sim/i2s_mic_sim.cpp kws_soc_tb.cpp -o $(TBEXEC)


# Helper target to run the cxxrtl testbench with default port
sim_yosys: $(TBEXEC)
	@echo "run $(TBEXEC) --help"
	@echo "When running run preferably over port 9824, else modify openocd/sim.cfg"

##### VERILATOR SIMULATION (fast) #####
 
# Verilator binary + default flags
VERILATOR ?= verilator
VERILATOR_FLAGS ?= -Wall -Wno-fatal --cc --trace
VERILATOR_BUILD_DIR ?= $(BUILD_DIR)/verilator
 
# ---------------------------------------------------------------------------
# VERILATOR_FLAGS — what each flag does and why it is here
# ---------------------------------------------------------------------------
#
# --cc              : C++ output mode (not SystemC).
#
# --trace           : Emit VCD tracing support.  Required by kws_soc_vpi.cpp
#                     (VerilatedVcdC).  Remove if you never use --vcd to save
#                     a small amount of per-cycle overhead.
#
#                     FASTER ALTERNATIVE: replace --trace with --trace-fst to
#                     emit FST (Fast Signal Trace) support instead.  FST files
#                     are ~10-50x smaller than VCD and write proportionally
#                     faster.  Requires changing kws_soc_vpi.cpp to include
#                     verilated_fst_c.h and use VerilatedFstC instead of
#                     VerilatedVcdC.
#
# --x-initial 0     : Initialise all X (uninitialised) bits to 0 instead of
#                     random.  Removes X-propagation tracking machinery from
#                     the generated C++, which measurably reduces eval() cost.
#
# -Wall -Wno-fatal  : Surface lint warnings without aborting the build.
#
# OPTIONAL ADDITIONS (uncomment to use):
#   --threads 4     : Multi-threaded simulation.  Can halve wall-clock time for
#                     designs that Verilator can partition.  Requires g++/clang
#                     to also see -lpthread; add it to LDFLAGS or CXXFLAGS.
#   --no-timing     : Disable timing-aware evaluation (default in --cc mode,
#                     listed here for clarity).
 
VERILATOR_FLAGS ?= -Wall -Wno-fatal --cc --trace --x-initial 0
 
# ---------------------------------------------------------------------------
# VERILATOR_CXXFLAGS — what each flag does and why it is here
# ---------------------------------------------------------------------------
#
# -O3               : Full optimisation.  Verilator's generated eval() is
#                     heavily inlined and benefits from auto-vectorisation.
#
# -march=native     : Allow the compiler to emit AVX/AVX2/etc. instructions
#                     for the host CPU.  Provides a further 10-30 % speedup
#                     on modern x86-64 machines for wide register-array accesses
#                     in the generated model.  Remove when cross-compiling or
#                     when the build and run hosts differ.
#
# -std=c++14        : Required by kws_soc_vpi.cpp.
#
# -I$(ROOT_DIR)     : kws_soc_vpi.cpp includes "sim/i2s_mic_sim.h" with a
#                     path relative to the project root.  Without this, the
#                     compiler (which runs from inside $(VERILATOR_BUILD_DIR)/)
#                     cannot resolve the path.
#
# $(UART_CFLAGS)    : -DCLK_MHZ, -DUART_BAUD_RATE, etc. — required by the
#                     #error guards in kws_soc_vpi.cpp.
 
VERILATOR_CXXFLAGS := $(UART_CFLAGS) -std=c++14 -O3 -march=native -I$(ROOT_DIR)
 
# ---------------------------------------------------------------------------
# Build rule
#
# -CFLAGS "..."     : Flags forwarded to the C++ compiler for ALL sources
#                     (including Verilator's own generated files).
#                     -I$(ROOT_DIR) here covers the verilator-invocation phase.
#                     -march=native here covers the generated model itself.
# ---------------------------------------------------------------------------
$(VERILATOR_BUILD_DIR)/Vkws_soc: $(FILE_LIST) kws_soc_vpi.cpp sim/i2s_mic_sim.cpp $(wildcard *.vh) $(DOTF)
	mkdir -p $(VERILATOR_BUILD_DIR)
	$(VERILATOR) $(VERILATOR_FLAGS) \
		--top-module $(TOP) \
		--Mdir $(VERILATOR_BUILD_DIR) \
		-CFLAGS "-I$(ROOT_DIR) -march=native" \
		--exe kws_soc_vpi.cpp sim/i2s_mic_sim.cpp \
		$(FILE_LIST) -I$(HDL)
	$(MAKE) -C $(VERILATOR_BUILD_DIR) -j -f V$(TOP).mk \
		CXXFLAGS='$(VERILATOR_CXXFLAGS)' V$(TOP)
 
.PHONY: sim-verilator sim-verilator-vcd sim-verilator-vcd-fast
sim_verilator: $(VERILATOR_BUILD_DIR)/Vkws_soc
	@echo "run ./$(VERILATOR_BUILD_DIR)/Vkws_soc --help"
	@echo "When running run preferably over port 9824, else modify openocd/sim.cfg"

lint:
	verilator --lint-only --top-module $(TOP) -I$(HDL) $(FILE_LIST)

###########################
##### QUARTUS Targets #####
###########################

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
	@echo "--- Synthesizing ---"
	$(MAP) $(QUARTUS_PROJECT) --verilog_macro="SRAM_DEPTH=$(SRAM_DEPTH)" \
        	$(foreach m,$(VERILOG_MACROS),--verilog_macro="$(m)")

# 2. Fitting (Place & Route)
$(FIT_RPT): $(MAP_RPT)
	@echo "--- Fitting ---"
	$(FIT) $(QUARTUS_PROJECT)

# 3. Bitstream Generation (Assembler)
$(ASM_RPT): $(FIT_RPT)
	@echo "--- Generating Bitstream ---"
	$(ASM) $(QUARTUS_PROJECT)

# 4
program: $(ASM_RPT)
	@echo "--- Programming FPGA ---"
ifeq ($(FPGA_BOARD), DE10S)
	@echo "--- Detecting Cable Index ---"
	$(eval CABLE_INDEX := $(shell jtagconfig -n | grep "DE-SoC" | head -n 1 | awk '{print $$1+0}'))

	@if [ -z "$(CABLE_INDEX)" ]; then \
		echo "Error: No DE-SoC cable found!"; \
		exit 1; \
	fi
	@echo "Using Cable Index: $(CABLE_INDEX)"
	@echo "--- Programming FPGA (Device 2) ---"
	$(PGM) -m jtag -c "$(CABLE_INDEX)" -o "p;$(SOF_FILE)@2"
else
	$(PGM) -c "USB-Blaster" -m JTAG -o "p;$(SOF_FILE)"
endif

test:
	$(MAKE) -C soc_test/c

# These just point to the real files above
.PHONY: config map fit asm
config: 	$(QSF_FILE)
map:    	$(MAP_RPT)
fit:    	$(FIT_RPT)
asm:    	$(ASM_RPT)

sta: $(FIT_RPT)
	@echo "--- Running Timing Analysis ---"
	$(STA) $(QUARTUS_PROJECT)

check_timing: sta
	@echo "--- Checking for Timing Violations ---"
	# Grep the summary for "Critical Warning" or negative slack
	# This is a simple check; for robust CI, parse the report files in output_files/
	$(SH) --tcl_eval "project_open $(QUARTUS_PROJECT); set x [get_timing_analysis_summary_results -model slow]; puts \$$x; project_close"

clean:: clean_sim clean_verilator clean_test clean_quartus
clean_sim:: clean_yosys clean_verilator

clean_test::
	$(MAKE) -C soc_test/c clean

clean_yosys::
	rm -rf $(YOSYS_BUILD_DIR) $(TBEXEC) *.vcd

clean_quartus::
	rm -rf $(QUARTUS_DIR)/db/ $(QUARTUS_DIR)/incremental_db/ $(QUARTUS_DIR)/output_files/ \
		$(QUARTUS_DIR)/*.qws $(QUARTUS_DIR)/*.sof $(QUARTUS_DIR)/*.pof $(QUARTUS_DIR)/*.rpt $(QUARTUS_DIR)/*.cdf \
		$(QUARTUS_DIR)/*.q*

clean_verilator::
	rm -rf $(VERILATOR_BUILD_DIR) *.vcd
