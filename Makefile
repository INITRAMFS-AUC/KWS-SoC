# This Makefile is concerned with the simulation and quartus development workflows #
# initramfs, 2025-2026 #

# Root directory Makefile
HAZARD3_ROOT := ./Hazard3
include $(HAZARD3_ROOT)/project_paths.mk

# --- DYNAMIC PORT CONFIGURATION ---
# Generate unique ports per user based on Linux UID to avoid collisions
USER_ID := $(shell id -u 2>/dev/null || echo 1000)
SIM_PORT    ?= $(shell expr 9000 + $(USER_ID) % 1000)
GDB_PORT    ?= $(shell expr 10000 + $(USER_ID) % 1000)
TELNET_PORT ?= $(shell expr 11000 + $(USER_ID) % 1000)

# Export SIM_PORT so JimTcl inside openocd/sim.cfg can read it
export SIM_PORT GDB_PORT

## COMMON
TOP       := kws_soc
DOTF      := soc.f
ROOT_DIR  := $(shell pwd)

## SIM
SIM_DIR	:= $(ROOT_DIR)/sim

# Use a project-local listfiles script (kept in scripts/, not the Hazard3
# submodule). Upstream's listfiles dropped the `--auto-vh` flag we rely on
# for adding `include directories to Quartus's search path; vendoring the
# old version here keeps us decoupled from upstream Hazard3 churn.
LISTFILES		:= $(ROOT_DIR)/scripts/listfiles
FILE_LIST		:= $(shell python3 $(LISTFILES) -f flat $(DOTF))
FILE_LIST_VH 	:= $(shell python3 $(LISTFILES) -f flat --auto-vh $(DOTF))

# FPGA device family and party, set by env shell with defaults being the de-10 standard
FPGA_FAMILY ?= "Cyclone V"
FPGA_PART   ?= 5CSXFC6D6F31C6
FPGA_BOARD  ?= DE10S
FPGA_FAMILY_CLEAN := $(strip $(subst ",,$(FPGA_FAMILY)))

GEN_PLL_QIP         := quartus/ip/clock_pll_gen/clock_pll_gen.qip
SRC_LIST_IP         := $(abspath $(GEN_PLL_QIP))

BUILD_DIR						:= build
## YOSYS VARS
YOSYS_CONFIG    		:= default
YOSYS_BUILD_DIR 		:= $(BUILD_DIR)/yosys
TBEXEC    					:= $(YOSYS_BUILD_DIR)/kws_soc_tb
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

# Clock config — change CLK_MHZ here (or on the command line) to any integer MHz
# achievable from 50 MHz (e.g. 25, 36, 40, 50, 100).
# Run `make gen_pll` first when changing frequency, then `make clean_quartus map`.
CLK_MHZ ?= 36

# 128k Memory
SRAM_DEPTH ?= 32768

export SRAM_DEPTH := $(SRAM_DEPTH)

# I2S receiver hardware FIFO depth — single source of truth for both
# Verilog (apb_i2s_receiver's FIFO_DEPTH parameter, threshold = N/2) and
# firmware (DMA burst = N/2, derived in test/Makefile).  Until task #12
# lands (autonomous DMA drain), the firmware burst MUST equal the half-
# full threshold or every other 4-block in ring_buf is silently zero-
# padded by FIFO-empty reads — keep both knobs derived from this one.
I2S_FIFO_DEPTH ?= 16
export I2S_FIFO_DEPTH

# DMA Controller base address (slave registers at 0x6000_0000)
DMAC_BASE_ADDR ?= 0x60000000

# MS_DMAC_AHBL.pp.v is the pre-expanded form (no ahbl_util.vh dependency)
DMAC_RTL_DIR := $(ROOT_DIR)/peris/MS_DMAC_AHBL/hdl/rtl

# UART config
UART_BAUD_RATE ?= 115200
# The remaining are hardcoded in uart_mini and thus useless
UART_DATA_WIDTH := 8
UART_PARITY := N
UART_STOP_BITS := 1
UART_FLOW_CONTROL := 0

# Verilog-macro form (NAME=VALUE, no -D prefix) — used by Quartus, Yosys, Verilator
UART_VERILOG_MACROS := CLK_MHZ=$(CLK_MHZ) \
                       UART_BAUD_RATE=$(UART_BAUD_RATE) \
                       UART_DATA_WIDTH=$(UART_DATA_WIDTH) \
                       UART_STOP_BITS=$(UART_STOP_BITS) \
                       I2S_FIFO_DEPTH=$(I2S_FIFO_DEPTH)

# Parity
ifeq ($(UART_PARITY), N)
    UART_VERILOG_MACROS += UART_PARITY_NONE
else ifeq ($(UART_PARITY), E)
    UART_VERILOG_MACROS += UART_PARITY_EVEN
else ifeq ($(UART_PARITY), O)
    UART_VERILOG_MACROS += UART_PARITY_ODD
else
    $(error Invalid UART_PARITY: $(UART_PARITY). Use N, E, or O)
endif

# Flow Control
ifeq ($(UART_FLOW_CONTROL), 1)
    UART_VERILOG_MACROS += UART_FLOW_CTRL_EN
endif

# C-preprocessor form (for firmware and kws_soc_vpi.cpp)
UART_CFLAGS := $(addprefix -D,$(UART_VERILOG_MACROS))

# Propagate to all RTL tools (Quartus, Yosys, Verilator Verilog elaboration)
VERILOG_MACROS += $(UART_VERILOG_MACROS)

# Optional bus-snooper debug peripheral. Set DEBUG_SNOOPER=1 in the
# environment (e.g. `make DEBUG_SNOOPER=1 sim-verilator …`) to:
#   - define DEBUG_SNOOPER for Verilog elaboration (instantiates the
#     snooper at 0x4000_C000, switches apb_splitter to N_SLAVES=4)
#   - tell scripts/apply_patches.sh to apply the debug-snooper-* patches
#     (CPU dbg_* output ports + ahb_sync_sram EXTRA_RD_WAIT parameter)
# Default off — production builds do not pay the snooper's M10K / area cost.
DEBUG_SNOOPER ?=
ifneq ($(DEBUG_SNOOPER),)
  VERILOG_MACROS += DEBUG_SNOOPER
  export DEBUG_SNOOPER
endif

export GLOBAL_UART_CONFIG := $(UART_CFLAGS)

# important: these show be in PATH, locate your quartus installation
MAP := quartus_map
FIT := quartus_fit
ASM := quartus_asm
STA := quartus_sta
PGM := quartus_pgm
SH  := quartus_sh

.PHONY: clean all lint sim sim-vcd sim_yosys sim_verilator sim-verilator-vcd \
        map fit asm sta program test test-xip testbench check_timing config \
        openocd-sim openocd-hw gdb telnet gen_pll \
        clean_sim clean_yosys clean_verilator clean_test clean_quartus \
        synth_sky130 clean_sky130 \
        gls gls_saif clean_gls

all: $(TBEXEC) test

# Yosys synthesis command to generate CXXRTL C++ code
YOSYS_SYNTH_CMD += read_verilog -I$(HDL) -I$(DMAC_RTL_DIR) -DSRAM_DEPTH=$(SRAM_DEPTH) $(foreach m,$(VERILOG_MACROS),-D$(m)) -DSIMULATION=1 -DCONFIG_HEADER="config_$(YOSYS_CONFIG).vh" $(XIP_DEBUG_VFLAG) $(FILE_LIST);
YOSYS_SYNTH_CMD += hierarchy -top $(TOP);
YOSYS_SYNTH_CMD += write_cxxrtl $(YOSYS_BUILD_DIR)/dut.cpp

## SKY130 ASIC SYNTHESIS VARS
# PDK root: set SKYWATER_PATH in your shell (default shown for reference)
SKYWATER_PATH       ?= $(pwd)/skywater-pdk/.ciel/sky130A
SKY130_SC_LIB       := sky130_fd_sc_hd
SKY130_CORNER       ?= tt_025C_1v80
SKY130_LIB          := $(SKYWATER_PATH)/libs.ref/$(SKY130_SC_LIB)/lib/$(SKY130_SC_LIB)__$(SKY130_CORNER).lib
SKY130_BUILD_DIR    := $(BUILD_DIR)/yosys/synth
SKY130_NETLIST      := $(SKY130_BUILD_DIR)/kws_soc_sky130.v

SKY130_SYNTH_SCRIPT := $(SKY130_BUILD_DIR)/synth_sky130.ys

$(SKY130_NETLIST): $(FILE_LIST) $(wildcard *.vh) $(DOTF) $(GEN_PARAMS_VH)
	@echo "--- Synthesizing kws_soc for SkyWater 130nm ($(SKY130_CORNER)) ---"
	@echo "    Liberty: $(SKY130_LIB)"
	@test -f "$(SKY130_LIB)" || (echo "ERROR: Liberty file not found: $(SKY130_LIB)"; exit 1)
	mkdir -p $(SKY130_BUILD_DIR)
	@printf '%s\n' \
		'read_verilog -I$(HDL) -DSRAM_DEPTH=$(SRAM_DEPTH) -DCLK_MHZ=$(CLK_MHZ) -DCONFIG_HEADER="config_$(YOSYS_CONFIG).vh" $(FILE_LIST)' \
		'hierarchy -check -top $(TOP)' \
		'proc; opt' \
		'memory; opt' \
		'fsm; opt' \
		'techmap; opt' \
		'dfflibmap -liberty $(SKY130_LIB)' \
		'abc -liberty $(SKY130_LIB)' \
		'clean' \
		'write_verilog $(SKY130_NETLIST)' \
		> $(SKY130_SYNTH_SCRIPT)
	yosys $(SKY130_SYNTH_SCRIPT)

synth_sky130: $(SKY130_NETLIST)
	@echo "--- Gate-level netlist written to $(SKY130_NETLIST) ---"

clean_sky130:
	rm -rf $(SKY130_BUILD_DIR)

## GATE-LEVEL SIMULATION (Verilator + sky130 cells → full-depth SAIF for OpenSTA)
#
# Reuses kws_soc_vpi.cpp (flash model, I2S, UART decode) compiled against the
# gate-level netlist so every internal net is tracked.  The binary is always
# built with -DTRACE_SAIF regardless of the TRACE_FORMAT setting above.
#
# Usage:
#   make gls_saif FLASH=test/fw.bin CYCLES=5000000
#   → produces build/gls/kws_soc_gate.saif, ready for OpenSTA read_saif

GLS_BUILD_DIR      := $(BUILD_DIR)/gls
SKY130_VERILOG_DIR := $(SKYWATER_PATH)/libs.ref/$(SKY130_SC_LIB)/verilog
SKY130_CELL_V      := $(SKY130_VERILOG_DIR)/$(SKY130_SC_LIB).v
# primitives.v uses Verilog-1995 UDP tables which Verilator cannot elaborate.
# sky130_udp_rtl.v provides drop-in module equivalents for all UDP primitives.
SKY130_UDP_RTL     := $(ROOT_DIR)/sim/sky130_udp_rtl.v
GLS_CELL_SRCS      := $(SKY130_UDP_RTL) $(SKY130_CELL_V)
GLS_SAIF_OUT       ?= $(GLS_BUILD_DIR)/kws_soc_gate.saif
# --output-split-cfuncs 500 : split the flat eval() across many small TUs so
#   no single compile unit blows memory.
# -O1 (not -O3) : keeps per-TU compile memory safe while giving ~5-10x faster
#   simulation than -O0.
# --threads $(GLS_THREADS) : Verilator statically partitions the eval graph;
#   set to 1 if the netlist is too flat for good partitioning.
GLS_THREADS        ?= 8
GLS_JOBS           ?= $(shell nproc)
GLS_CXXFLAGS       := $(UART_CFLAGS) -DTRACE_SAIF -std=c++14 -O1 -I$(ROOT_DIR) -lpthread

$(GLS_BUILD_DIR)/V$(TOP): $(SKY130_NETLIST) $(SKY130_CELL_V) $(SKY130_UDP_RTL) \
                           kws_soc_vpi.cpp sim/flashsim.cpp sim/i2s_mic_sim.cpp \
                           $(wildcard *.vh)
	@test -f "$(SKY130_CELL_V)" || \
	    (echo "ERROR: sky130 Verilog cells not found: $(SKY130_CELL_V)"; exit 1)
	@test -f "$(SKY130_NETLIST)" || \
	    (echo "ERROR: gate-level netlist not found — run 'make synth_sky130' first"; exit 1)
	mkdir -p $(GLS_BUILD_DIR)
	$(VERILATOR) -Wall -Wno-fatal --cc --no-timing --x-initial 0 \
	    --threads $(GLS_THREADS) \
	    --output-split-cfuncs 500 \
	    --top-module $(TOP) \
	    --Mdir $(GLS_BUILD_DIR) \
	    -DFUNCTIONAL -DUNIT_DELAY= \
	    -CFLAGS "-I$(ROOT_DIR)" \
	    --exe $(ROOT_DIR)/kws_soc_vpi.cpp \
	          $(ROOT_DIR)/sim/flashsim.cpp \
	          $(ROOT_DIR)/sim/i2s_mic_sim.cpp \
	    $(GLS_CELL_SRCS) $(SKY130_NETLIST) \
	    -I$(ROOT_DIR) -I$(SKY130_VERILOG_DIR)
	$(MAKE) -C $(GLS_BUILD_DIR) -j$(GLS_JOBS) -f V$(TOP).mk \
	    CXXFLAGS='$(GLS_CXXFLAGS)' V$(TOP)

gls: $(GLS_BUILD_DIR)/V$(TOP)
	@echo "--- GLS binary ready: $(GLS_BUILD_DIR)/V$(TOP) ---"
	@echo "Run: $(GLS_BUILD_DIR)/V$(TOP) --no-jtag --flash <fw.bin> --cycles <N> --waves <out.saif>"

gls_saif: $(GLS_BUILD_DIR)/V$(TOP)
	@test -n "$(FLASH)" || \
	    (echo "ERROR: FLASH=path/to/fw.bin is required  (e.g. make gls_saif FLASH=test/fw.bin CYCLES=5000000)"; exit 1)
	$(GLS_BUILD_DIR)/V$(TOP) --no-jtag $(FLASH_ARG) \
	    $(MIC_ARG) $(CYCLES_ARG) --waves $(GLS_SAIF_OUT) $(EXTRA_ARGS)
	@echo "--- Gate-level SAIF written to $(GLS_SAIF_OUT) ---"

clean_gls:
	rm -rf $(GLS_BUILD_DIR)

# --- GENERATED FILES ---
HAZARD3_CONFIG  :=$(ROOT_DIR)/hazard3_config.vh
GEN_PARAMS_VH   := hazard3_instantiation_params.vh

# --- PROJECT-LOCAL SUBMODULE PATCHES ---
# Patches in patches/*.patch are applied to the Hazard3 submodule (and the
# nested libfpga submodule) at build time. Idempotent — running twice does
# nothing on the second pass. The submodules' tracked SHAs are unchanged so
# `git submodule update` / upstream pulls remain conflict-free. `make clean`
# reverts the patches so the worktree is left pristine.
.PHONY: apply_patches revert_patches
apply_patches:
	@bash $(ROOT_DIR)/scripts/apply_patches.sh

revert_patches:
	@bash $(ROOT_DIR)/scripts/apply_patches.sh --revert

# Hook every build path through apply_patches via the params .vh file
$(GEN_PARAMS_VH): $(HAZARD3_CONFIG) | apply_patches
	@echo "--- Generating Instantiation Parameters ---"
	python3 scripts/gen_inst_params.py $< $@


$(YOSYS_BUILD_DIR)/dut.cpp: $(FILE_LIST) $(wildcard *.vh) $(DOTF) $(GEN_PARAMS_VH)
	mkdir -p $(YOSYS_BUILD_DIR)
	yosys -p '$(YOSYS_SYNTH_CMD)'


# Build the cxxrtl testbench: links both the flash simulator and the I2S mic simulator
$(TBEXEC): $(YOSYS_BUILD_DIR)/dut.cpp kws_soc_tb.cpp sim/flashsim.cpp sim/flashsim.h sim/i2s_mic_sim.cpp
	$(CLANGXX) -O3 -std=c++14 $(addprefix -D,$(CDEFINES)) $(UART_CFLAGS) \
		-I$(shell yosys-config --datdir)/include/backends/cxxrtl/runtime \
		-I$(YOSYS_BUILD_DIR) \
		-Isim \
		sim/flashsim.cpp sim/i2s_mic_sim.cpp kws_soc_tb.cpp -o $(TBEXEC)

# Helper target to run the cxxrtl testbench with default port
sim_yosys: $(TBEXEC)
	@echo "run $(TBEXEC) --help"
	@echo "When running run preferably over port $(SIM_PORT), else modify openocd/sim.cfg"

# Helper target to run the cxxrtl testbench with default port
SIM_PORT_ARG = $(if $(filter 1,$(NO_JTAG)),,--port $(SIM_PORT))

sim: $(TBEXEC) test
	./$(TBEXEC) $(SIM_PORT_ARG) $(NO_JTAG_ARG) $(FLASH_ARG) $(MIC_ARG) $(XIP_DEBUG_ARG) $(I2S_DEBUG_ARG) $(UART_DEBUG_ARG) $(CYCLES_ARG) $(EXTRA_ARGS)

# Helper target to run cxxrtl testbench with VCD dumping
sim-vcd: $(TBEXEC) test
	./$(TBEXEC) $(SIM_PORT_ARG) $(NO_JTAG_ARG) --vcd waves.vcd $(FLASH_ARG) $(MIC_ARG) $(XIP_DEBUG_ARG) $(I2S_DEBUG_ARG) $(UART_DEBUG_ARG) $(CYCLES_ARG) $(EXTRA_ARGS)

##### VERILATOR SIMULATION (fast) #####

# Verilator binary + default flags
VERILATOR ?= verilator
VERILATOR_BUILD_DIR ?= $(BUILD_DIR)/verilator

# ---------------------------------------------------------------------------
# Trace format: FST (default) or VCD
#
#   make sim_verilator                      → FST (smaller, faster)
#   TRACE_FORMAT=VCD  make sim_verilator    → VCD (classic, wider tool support)
#   TRACE_FORMAT=SAIF make sim_verilator    → SAIF toggle-activity file for power analysis
# ---------------------------------------------------------------------------
TRACE_FORMAT ?= FST

ifeq ($(TRACE_FORMAT),SAIF)
  VERILATOR_TRACE_FLAG :=           # no Verilator waveform file — SaifWriter tracks natively
  TRACE_CFLAGS         := -DTRACE_SAIF
  TRACE_EXT            := saif
else ifeq ($(TRACE_FORMAT),VCD)
  VERILATOR_TRACE_FLAG := --trace
  TRACE_CFLAGS         := -DTRACE_VCD
  TRACE_EXT            := vcd
else
  VERILATOR_TRACE_FLAG := --trace-fst
  TRACE_CFLAGS         :=
  TRACE_EXT            := fst
endif

# ---------------------------------------------------------------------------
# VERILATOR_FLAGS — what each flag does and why it is here
# ---------------------------------------------------------------------------
#
# --cc              : C++ output mode (not SystemC).
#
# $(VERILATOR_TRACE_FLAG) : Controlled by TRACE_FORMAT (default FST).
#                     --trace-fst emits FST support (VerilatedFstC).
#                     --trace     emits VCD support (VerilatedVcdC).
#                     kws_soc_vpi.cpp selects the matching class via
#                     the TRACE_VCD preprocessor define.
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

VERILATOR_FLAGS := -Wall -Wno-fatal --cc $(VERILATOR_TRACE_FLAG) --x-initial 0

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

VERILATOR_CXXFLAGS := $(UART_CFLAGS) $(TRACE_CFLAGS) -std=c++14 -O3 -march=native -I$(ROOT_DIR)

# ---------------------------------------------------------------------------
# Build rule
#
# -CFLAGS "..."     : Flags forwarded to the C++ compiler for ALL sources
#                     (including Verilator's own generated files).
#                     -I$(ROOT_DIR) here covers the verilator-invocation phase.
#                     -march=native here covers the generated model itself.
# ---------------------------------------------------------------------------
$(VERILATOR_BUILD_DIR)/Vkws_soc: $(FILE_LIST) kws_soc_vpi.cpp sim/flashsim.cpp sim/flashsim.h sim/i2s_mic_sim.cpp $(wildcard *.vh) $(DOTF)
	mkdir -p $(VERILATOR_BUILD_DIR)
	$(VERILATOR) $(VERILATOR_FLAGS) \
		--top-module $(TOP) \
		--Mdir $(VERILATOR_BUILD_DIR) \
		-CFLAGS "-I$(ROOT_DIR) -march=native" \
		--exe $(ROOT_DIR)/kws_soc_vpi.cpp $(ROOT_DIR)/sim/flashsim.cpp $(ROOT_DIR)/sim/i2s_mic_sim.cpp \
		$(FILE_LIST) -I$(ROOT_DIR) -I$(HDL) -I$(DMAC_RTL_DIR) -DSRAM_DEPTH=$(SRAM_DEPTH) $(foreach m,$(VERILOG_MACROS),-D$(m)) $(XIP_DEBUG_VFLAG)
	$(MAKE) -C $(VERILATOR_BUILD_DIR) -j -f V$(TOP).mk \
		CXXFLAGS='$(VERILATOR_CXXFLAGS)' V$(TOP)

.PHONY: sim-verilator sim-verilator-vcd sim-verilator-vcd-fast
sim_verilator: $(VERILATOR_BUILD_DIR)/Vkws_soc
	@echo "run ./$(VERILATOR_BUILD_DIR)/Vkws_soc --help"
	@echo "When running run preferably over port $(SIM_PORT), else modify openocd/sim.cfg"

sim-verilator: $(VERILATOR_BUILD_DIR)/Vkws_soc test
	./$(VERILATOR_BUILD_DIR)/Vkws_soc $(SIM_PORT_ARG) $(NO_JTAG_ARG) $(FLASH_ARG) $(MIC_ARG) $(XIP_DEBUG_ARG) $(I2S_DEBUG_ARG) $(UART_DEBUG_ARG) $(CYCLES_ARG) $(EXTRA_ARGS)

sim-verilator-vcd: $(VERILATOR_BUILD_DIR)/Vkws_soc test
	./$(VERILATOR_BUILD_DIR)/Vkws_soc $(SIM_PORT_ARG) $(NO_JTAG_ARG) --waves waves.$(TRACE_EXT) $(FLASH_ARG) $(MIC_ARG) $(XIP_DEBUG_ARG) $(I2S_DEBUG_ARG) $(UART_DEBUG_ARG) $(CYCLES_ARG) $(EXTRA_ARGS)

lint:
	verilator --lint-only -Wno-fatal --top-module $(TOP) -I$(HDL) -I$(DMAC_RTL_DIR) -DSRAM_DEPTH=$(SRAM_DEPTH) $(foreach m,$(VERILOG_MACROS),-D$(m)) $(FILE_LIST)

lint_fpga:
	verilator --lint-only -Wno-fatal --top-module $(TOP_FPGA) -I$(HDL) -I$(DMAC_RTL_DIR) -DSRAM_DEPTH=$(SRAM_DEPTH) $(foreach m,$(VERILOG_MACROS),-D$(m)) $(FILE_LIST)

# Allow passing a flash binary via `make sim FLASH=path/to/fw.bin`
FLASH ?=
FLASH_ARG = $(if $(FLASH),--flash $(FLASH),)

# Allow passing an I2S audio hex file via `make sim MIC=sim/debug_audio.hex`
MIC ?=
MIC_ARG = $(if $(MIC),--mic $(MIC),)

# Debug flags — setting XIP_DEBUG=1 or I2S_DEBUG=1 on the make command line
# enables both the RTL $display (via Verilog define) and C++ prints (via
# runtime flag) in whichever simulator you invoke.  No need to separately
# pass --xip-debug / --i2s-debug via EXTRA_ARGS.
XIP_DEBUG ?= 0
I2S_DEBUG ?= 0

ifeq ($(XIP_DEBUG),1)
  XIP_DEBUG_VFLAG := -DXIP_DEBUG
  XIP_DEBUG_ARG   := --xip-debug
else
  XIP_DEBUG_VFLAG :=
  XIP_DEBUG_ARG   :=
endif

ifeq ($(I2S_DEBUG),1)
  I2S_DEBUG_ARG := --i2s-debug
else
  I2S_DEBUG_ARG :=
endif

UART_DEBUG ?= 0

ifeq ($(UART_DEBUG),1)
  UART_DEBUG_ARG := --uart-debug
else
  UART_DEBUG_ARG :=
endif

# NO_JTAG=1 — run standalone without OpenOCD/GDB.
# CPU boots from flash immediately (requires FLASH=path/to/fw.bin).
# Useful to smoke-test UART output without setting up a debug session.
NO_JTAG ?= 0

ifeq ($(NO_JTAG),1)
  NO_JTAG_ARG := --no-jtag
else
  NO_JTAG_ARG :=
endif

CYCLES ?=
CYCLES_ARG = $(if $(CYCLES),--cycles $(CYCLES),)

###########################
##### QUARTUS Targets #####
###########################

# Generate (or regenerate) the PLL IP for the current CLK_MHZ / FPGA_FAMILY.
# Re-run manually when changing CLK_MHZ: `make gen_pll`
.PHONY: gen_pll
gen_pll:
	@echo "--- Generating PLL: $(CLK_MHZ) MHz ($(FPGA_FAMILY_CLEAN)) ---"
	python3 scripts/gen_pll.py --clk-mhz $(CLK_MHZ) --device-family "$(FPGA_FAMILY_CLEAN)"

# 0. Project Generation
$(QSF_FILE): $(QUARTUS_DIR)/setup_project.tcl Makefile
	@echo "--- Generating PLL: $(CLK_MHZ) MHz ($(FPGA_FAMILY_CLEAN)) ---"
	python3 scripts/gen_pll.py --clk-mhz $(CLK_MHZ) --device-family "$(FPGA_FAMILY_CLEAN)"
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
$(MAP_RPT): $(QSF_FILE) $(ALL_QUARTUS_SRCS) $(GEN_PARAMS_VH)
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
	$(MAKE) -C test

test-dma:
	$(MAKE) -C test dma

testbench:
	# TODO: Make a python script that runs all testbenches using vvp and checks their output and gives a report
	$(MAKE) -C test xip-testbench

test-xip: testbench

# These just point to the real files above
.PHONY: config map fit asm
config: 	$(QSF_FILE)
map:    	$(MAP_RPT)
fit:    	$(FIT_RPT)
# TODO: asm does not correctly detect what to rerun for example sometimes it is necessary to recompile one module before attempting the top one
asm:    	$(ASM_RPT)

sta: $(FIT_RPT)
	@echo "--- Running Timing Analysis ---"
	$(STA) $(QUARTUS_PROJECT)

check_timing: sta
	@echo "--- Checking for Timing Violations ---"
	# Grep the summary for "Critical Warning" or negative slack
	# This is a simple check; for robust CI, parse the report files in output_files/
	$(SH) --tcl_eval "project_open $(QUARTUS_PROJECT); set x [get_timing_analysis_summary_results -model slow]; puts \$$x; project_close"

openocd-sim:
	@echo "Starting OpenOCD (Simulation)..."
	@echo " -> Sim Port:    $(SIM_PORT)"
	@echo " -> GDB Port:    $(GDB_PORT)"
	@echo " -> Telnet Port: $(TELNET_PORT)"
	riscv-openocd -c "gdb_port $(GDB_PORT)" -c "telnet_port $(TELNET_PORT)" -c "tcl_port disabled" -f openocd/sim.cfg

openocd-hw:
	@echo "Starting OpenOCD (Hardware)..."
	@echo " -> GDB Port:    $(GDB_PORT)"
	@echo " -> Telnet Port: $(TELNET_PORT)"
	riscv-openocd -c "gdb_port $(GDB_PORT)" -c "telnet_port $(TELNET_PORT)" -c "tcl_port disabled" -f openocd/picodriver.cfg

gdb:
	riscv32-unknown-elf-gdb -x gdbinit

telnet:
	telnet localhost $(TELNET_PORT)

clean:: clean_sim clean_test clean_quartus revert_patches

clean_sim:: clean_yosys clean_verilator

clean_test::
	$(MAKE) -C test clean

clean_yosys::
	rm -rf $(TBEXEC) *.vcd
	rm -f $(GEN_PARAMS_VH)

clean_quartus::
	rm -rf $(QUARTUS_SRC_DIR) $(QUARTUS_DIR)/db/ $(QUARTUS_DIR)/incremental_db/ $(QUARTUS_DIR)/output_files/ \
		$(QUARTUS_DIR)/*.qws $(QUARTUS_DIR)/*.sof $(QUARTUS_DIR)/*.pof $(QUARTUS_DIR)/*.rpt $(QUARTUS_DIR)/*.cdf \
		$(QUARTUS_DIR)/*.qsf $(QUARTUS_DIR)/*.qpf $(QUARTUS_DIR)/*.qws $(QUARTUS_DIR)/*dump.txt \
		$(QUARTUS_DIR)/ip/clock_pll_gen/

clean_verilator::
	rm -rf $(VERILATOR_BUILD_DIR) *.vcd *.fst
