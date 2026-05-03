# ============================================================
# Verilator simulation Makefile for conv1d_accel
# Requires Verilator v5.x (--timing support)
# ============================================================

RTL_SRC := ../conv1d_accel.v
TB_SRC  := conv1d_accel_tb.v
SIM_DIR := sim
TOP     := conv1d_accel_tb
EXE     := V$(TOP)

VFLAGS := \
	--binary \
	--timing \
	--trace \
	--top-module $(TOP) \
	-I. \
	-Wno-TIMESCALEMOD \
	-Wno-STMTDLY \
	-Wno-MULTIDRIVEN \
	-Wno-UNDRIVEN \
	-Wno-WIDTH

.PHONY: all sim waves clean

all: sim

$(SIM_DIR)/$(EXE): $(RTL_SRC) $(TB_SRC) tb_utils.vh
	mkdir -p $(SIM_DIR)
	verilator $(VFLAGS) --Mdir $(SIM_DIR) -o $(EXE) $(RTL_SRC) $(TB_SRC)

sim: $(SIM_DIR)/$(EXE)
	cd $(SIM_DIR) && ./$(EXE)

waves:
	gtkwave $(SIM_DIR)/conv1d_accel_tb.vcd &

clean:
	rm -rf $(SIM_DIR)
