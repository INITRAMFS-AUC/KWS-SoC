ROOT_MAKEFILE = ../Makefile
CROSS_COMPILE = riscv32-unknown-elf-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy

# Model-only link driver. Keep the rest of the test workflow on raw ld.
MODEL_LINK = $(CC)

# Directories
BUILD_DIR    = build
COMMON_DIR   = common
SANITY_C_DIR = sanity_checks/c
UART_C_DIR   = uart/c
I2S_C_DIR    = i2s/c
XIP_C_DIR    = xip/c
XIP_DIR      = xip
MALLOC_C_DIR = malloc/c
MEMCPY_C_DIR = memcpy/c
MODEL_C_DIR  = model
NNOM_DIR     = ../third_party/nnom
NNOM_INC_DIR = $(NNOM_DIR)/inc
NNOM_PORT_DIR = $(NNOM_DIR)/port

NNOM_CORE_SRCS = $(wildcard $(NNOM_DIR)/src/core/*.c)
NNOM_LAYER_SRCS = $(wildcard $(NNOM_DIR)/src/layers/*.c)
NNOM_BACKEND_SRCS = $(NNOM_DIR)/src/backends/nnom_local.c $(NNOM_DIR)/src/backends/nnom_local_q15.c
NNOM_SRCS = $(NNOM_CORE_SRCS) $(NNOM_LAYER_SRCS) $(NNOM_BACKEND_SRCS)
NNOM_OBJS = $(patsubst $(NNOM_DIR)/src/%.c,$(BUILD_DIR)/nnom_%.o,$(NNOM_SRCS))


# Header include paths (relative to this Makefile's directory)
I2S_REGS_DIR  = ../peris/i2s/i2s_apb/i2s_itr2
UART_REGS_DIR = ../Hazard3/example_soc/libfpga/peris/uart

# Memory config
SRAM_DEPTH ?= 32768
SRAM_SIZE_BYTES = $(shell echo $$(($(SRAM_DEPTH) * 4)))

# Compiler flags
CFLAGS  = -march=rv32i_zicsr -mabi=ilp32 -g -c -MMD -MP
CFLAGS += $(GLOBAL_UART_CONFIG)
CFLAGS += -I$(I2S_REGS_DIR)
CFLAGS += -I$(UART_REGS_DIR)

# Linker flags:
#   SRAM_LDFLAGS – all sections (text/rodata/data/bss) in SRAM at 0x00000000.
#                  Required for GDB `load`; string literals live at their SRAM VMA.
#   XIP_LDFLAGS  – text+rodata in FLASH (0x80000000), data+bss in SRAM.
#                  Required for standalone boot; string literals live at their flash VMA.
SRAM_LDFLAGS = -T $(COMMON_DIR)/sram_link.ld --defsym=SRAM_SIZE=$(SRAM_SIZE_BYTES)
XIP_LDFLAGS  = -T $(COMMON_DIR)/link.ld      --defsym=SRAM_SIZE=$(SRAM_SIZE_BYTES)

# GCC-driver model link flags: pull compiler helper routines from libgcc
# without bringing full default runtime libraries.
MODEL_COMMON_GCC_LDFLAGS = -march=rv32i_zicsr -mabi=ilp32 -nostdlib -nostartfiles -nodefaultlibs -Wl,--gc-sections
MODEL_SRAM_GCC_LDFLAGS   = $(MODEL_COMMON_GCC_LDFLAGS) -Wl,-T,$(COMMON_DIR)/sram_link.ld -Wl,--defsym=SRAM_SIZE=$(SRAM_SIZE_BYTES)
MODEL_XIP_GCC_LDFLAGS    = $(MODEL_COMMON_GCC_LDFLAGS) -Wl,-T,$(COMMON_DIR)/link.ld -Wl,--defsym=SRAM_SIZE=$(SRAM_SIZE_BYTES)

# --- TARGET LISTS ---
# *_sram: GDB workflow — `make gdb`, then `load build/<test>_sram.elf`, then `continue`
SRAM_TARGETS = \
	$(BUILD_DIR)/inf_loop_sram.elf    $(BUILD_DIR)/inf_loop_sram.dis    $(BUILD_DIR)/inf_loop_sram.bin \
	$(BUILD_DIR)/hello_world_sram.elf $(BUILD_DIR)/hello_world_sram.dis $(BUILD_DIR)/hello_world_sram.bin \
	$(BUILD_DIR)/readI2s_sram.elf     $(BUILD_DIR)/readI2s_sram.dis     $(BUILD_DIR)/readI2s_sram.bin \
	$(BUILD_DIR)/malloc_simple_test_sram.elf  $(BUILD_DIR)/malloc_simple_test_sram.dis  $(BUILD_DIR)/malloc_simple_test_sram.bin \
	$(BUILD_DIR)/malloc_torture_test_sram.elf $(BUILD_DIR)/malloc_torture_test_sram.dis $(BUILD_DIR)/malloc_torture_test_sram.bin \
	$(BUILD_DIR)/memcpy_simple_test_sram.elf  $(BUILD_DIR)/memcpy_simple_test_sram.dis  $(BUILD_DIR)/memcpy_simple_test_sram.bin
	# $(BUILD_DIR)/strided_s16_nodil_sram.elf   $(BUILD_DIR)/strided_s16_nodil_sram.dis   $(BUILD_DIR)/strided_s16_nodil_sram.bin


# *_xip: standalone workflow — `NO_JTAG=1 make sim FLASH=test/build/<test>_xip.bin`
XIP_TARGETS = \
	$(BUILD_DIR)/hello_world_xip.elf     $(BUILD_DIR)/hello_world_xip.dis     $(BUILD_DIR)/hello_world_xip.bin \
	$(BUILD_DIR)/readI2s_xip.elf         $(BUILD_DIR)/readI2s_xip.dis         $(BUILD_DIR)/readI2s_xip.bin \
	$(BUILD_DIR)/flash_read_test_xip.elf $(BUILD_DIR)/flash_read_test_xip.dis $(BUILD_DIR)/flash_read_test_xip.bin \
	$(BUILD_DIR)/tests_xip.elf           $(BUILD_DIR)/tests_xip.dis           $(BUILD_DIR)/tests_xip.bin \
	$(BUILD_DIR)/malloc_simple_test_xip.elf  $(BUILD_DIR)/malloc_simple_test_xip.dis  $(BUILD_DIR)/malloc_simple_test_xip.bin \
	$(BUILD_DIR)/malloc_torture_test_xip.elf $(BUILD_DIR)/malloc_torture_test_xip.dis $(BUILD_DIR)/malloc_torture_test_xip.bin \
	$(BUILD_DIR)/memcpy_simple_test_xip.elf  $(BUILD_DIR)/memcpy_simple_test_xip.dis  $(BUILD_DIR)/memcpy_simple_test_xip.bin \
	$(BUILD_DIR)/strided_s16_nodil_xip.elf   $(BUILD_DIR)/strided_s16_nodil_xip.dis   $(BUILD_DIR)/strided_s16_nodil_xip.bin


ALL_TARGETS = $(SRAM_TARGETS) $(XIP_TARGETS)

all: $(BUILD_DIR) $(ALL_TARGETS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# --- ALIAS TARGETS ---
sram:   $(SRAM_TARGETS)
xip:    $(XIP_TARGETS)
sanity: $(BUILD_DIR)/inf_loop_sram.elf    $(BUILD_DIR)/inf_loop_sram.dis    $(BUILD_DIR)/inf_loop_sram.bin
uart:   $(BUILD_DIR)/hello_world_sram.elf $(BUILD_DIR)/hello_world_sram.dis $(BUILD_DIR)/hello_world_sram.bin \
        $(BUILD_DIR)/hello_world_xip.elf  $(BUILD_DIR)/hello_world_xip.dis  $(BUILD_DIR)/hello_world_xip.bin
i2s:    $(BUILD_DIR)/readI2s_sram.elf     $(BUILD_DIR)/readI2s_sram.dis     $(BUILD_DIR)/readI2s_sram.bin \
        $(BUILD_DIR)/readI2s_xip.elf      $(BUILD_DIR)/readI2s_xip.dis      $(BUILD_DIR)/readI2s_xip.bin
flash:  $(BUILD_DIR)/flash_read_test_xip.elf $(BUILD_DIR)/flash_read_test_xip.dis $(BUILD_DIR)/flash_read_test_xip.bin
malloc: $(BUILD_DIR)/malloc_simple_test_sram.elf  $(BUILD_DIR)/malloc_simple_test_sram.dis  $(BUILD_DIR)/malloc_simple_test_sram.bin \
	$(BUILD_DIR)/malloc_torture_test_sram.elf $(BUILD_DIR)/malloc_torture_test_sram.dis $(BUILD_DIR)/malloc_torture_test_sram.bin \
	$(BUILD_DIR)/malloc_simple_test_xip.elf   $(BUILD_DIR)/malloc_simple_test_xip.dis   $(BUILD_DIR)/malloc_simple_test_xip.bin \
	$(BUILD_DIR)/malloc_torture_test_xip.elf  $(BUILD_DIR)/malloc_torture_test_xip.dis  $(BUILD_DIR)/malloc_torture_test_xip.bin
malloc-simple: $(BUILD_DIR)/malloc_simple_test_sram.elf  $(BUILD_DIR)/malloc_simple_test_sram.dis  $(BUILD_DIR)/malloc_simple_test_sram.bin \
	       $(BUILD_DIR)/malloc_simple_test_xip.elf   $(BUILD_DIR)/malloc_simple_test_xip.dis   $(BUILD_DIR)/malloc_simple_test_xip.bin
malloc-torture: $(BUILD_DIR)/malloc_torture_test_sram.elf $(BUILD_DIR)/malloc_torture_test_sram.dis $(BUILD_DIR)/malloc_torture_test_sram.bin \
		$(BUILD_DIR)/malloc_torture_test_xip.elf  $(BUILD_DIR)/malloc_torture_test_xip.dis  $(BUILD_DIR)/malloc_torture_test_xip.bin
memcpy: $(BUILD_DIR)/memcpy_simple_test_sram.elf $(BUILD_DIR)/memcpy_simple_test_sram.dis $(BUILD_DIR)/memcpy_simple_test_sram.bin \
	$(BUILD_DIR)/memcpy_simple_test_xip.elf  $(BUILD_DIR)/memcpy_simple_test_xip.dis  $(BUILD_DIR)/memcpy_simple_test_xip.bin
model-strided-s16-nodil: $(BUILD_DIR)/strided_s16_nodil_xip.elf  $(BUILD_DIR)/strided_s16_nodil_xip.dis  $(BUILD_DIR)/strided_s16_nodil_xip.bin
# SRAM model artifacts intentionally disabled:
# $(BUILD_DIR)/strided_s16_nodil_sram.elf $(BUILD_DIR)/strided_s16_nodil_sram.dis $(BUILD_DIR)/strided_s16_nodil_sram.bin
xip-testbench:
	$(MAKE) -C $(XIP_DIR) run-testbench

# --- SRAM LINK RULES ---
$(BUILD_DIR)/inf_loop_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/inf_loop.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

$(BUILD_DIR)/hello_world_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/hello_world.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

$(BUILD_DIR)/readI2s_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/readI2s.o $(BUILD_DIR)/uart.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

$(BUILD_DIR)/malloc_simple_test_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/malloc_simple_test.o $(COMMON_DIR)/uart_utils.o $(BUILD_DIR)/malloc.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

$(BUILD_DIR)/malloc_torture_test_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/malloc_torture_test.o $(COMMON_DIR)/uart_utils.o $(BUILD_DIR)/malloc.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

$(BUILD_DIR)/memcpy_simple_test_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/memcpy_simple_test.o $(COMMON_DIR)/uart_utils.o $(BUILD_DIR)/memcpy.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

$(BUILD_DIR)/strided_s16_nodil_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/strided_s16_nodil_main.o $(COMMON_DIR)/uart_utils.o $(BUILD_DIR)/malloc.o $(BUILD_DIR)/memcpy.o $(BUILD_DIR)/memset.o $(NNOM_OBJS)
	$(MODEL_LINK) $(MODEL_SRAM_GCC_LDFLAGS) -o $@ $^ -lgcc

# --- XIP LINK RULES ---
$(BUILD_DIR)/hello_world_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/hello_world.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

$(BUILD_DIR)/readI2s_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/readI2s.o $(BUILD_DIR)/uart.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

$(BUILD_DIR)/flash_read_test_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/flash_read_test.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

$(BUILD_DIR)/tests_xip.elf: $(BUILD_DIR)/tests.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

$(BUILD_DIR)/malloc_simple_test_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/malloc_simple_test.o $(COMMON_DIR)/uart_utils.o $(BUILD_DIR)/malloc.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

$(BUILD_DIR)/malloc_torture_test_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/malloc_torture_test.o $(COMMON_DIR)/uart_utils.o $(BUILD_DIR)/malloc.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

$(BUILD_DIR)/memcpy_simple_test_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/memcpy_simple_test.o $(COMMON_DIR)/uart_utils.o $(BUILD_DIR)/memcpy.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

$(BUILD_DIR)/strided_s16_nodil_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/strided_s16_nodil_main.o $(COMMON_DIR)/uart_utils.o $(BUILD_DIR)/malloc.o $(BUILD_DIR)/memcpy.o $(BUILD_DIR)/memset.o $(NNOM_OBJS)
	$(MODEL_LINK) $(MODEL_XIP_GCC_LDFLAGS) -o $@ $^ -lgcc

# --- COMPILATION RULES ---
$(BUILD_DIR)/crt0.o: $(COMMON_DIR)/crt0.s $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/inf_loop.o: $(SANITY_C_DIR)/inf_loop.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/hello_world.o: $(UART_C_DIR)/hello_world.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/readI2s.o: $(I2S_C_DIR)/readI2s.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/uart.o: $(I2S_C_DIR)/uart.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/flash_read_test.o: $(XIP_C_DIR)/flash_read_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/tests.o: $(XIP_C_DIR)/tests.S $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/malloc_simple_test.o: $(MALLOC_C_DIR)/simple_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/malloc_torture_test.o: $(MALLOC_C_DIR)/torture_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/memcpy_simple_test.o: $(MEMCPY_C_DIR)/simple_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/strided_s16_nodil_main.o: $(MODEL_C_DIR)/strided_s16_nodil_main.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -I$(NNOM_INC_DIR) -I$(NNOM_PORT_DIR) -o $@ $<

$(BUILD_DIR)/nnom_%.o: $(NNOM_DIR)/src/%.c $(ROOT_MAKEFILE)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(NNOM_INC_DIR) -I$(NNOM_PORT_DIR) -o $@ $<

$(BUILD_DIR)/malloc.o: $(COMMON_DIR)/malloc.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/memcpy.o: $(COMMON_DIR)/memcpy.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/memset.o: $(COMMON_DIR)/memset.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/uart_utils.o: $(COMMON_DIR)/uart_utils.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

# --- BINARY & DISASSEMBLY RULES ---
$(BUILD_DIR)/%.dis: $(BUILD_DIR)/%.elf
	$(OBJDUMP) -d $< > $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD_DIR) *.o *.elf *.dis
	rm -f $(COMMON_DIR)/*.d
	$(MAKE) -C $(XIP_DIR) clean

.PHONY: all clean sram xip sanity uart i2s flash malloc malloc-simple malloc-torture memcpy model-strided-s16-nodil xip-testbench mel-compact

# Include dependency files
-include $(wildcard $(BUILD_DIR)/*.d)

# ── mel_compact_4blk_ch36 — 195M cycle baseline ──────────────────────────────
#
# Reproduces the confirmed 195,486,216 cycle inference result.
#
# SoC config: CSR_COUNTER=1, MULDIV_UNROLL=1, no accelerator (this branch).
# Model: mel_compact_4blk_ch36 seed=789, 88.7% int8, 26.2 KB flash.
#
# Build:
#   make mel-compact
#
# Run (from kws_soc/ root with Nix devshell active):
#   NO_JTAG=1 make sim-verilator \
#     FLASH=test/build/mel_compact_4blk_ch36_xip.bin \
#     MIC=sim/down_0000.hex
#
# Toolchain: riscv32-unknown-elf-gcc (rv32imac_zicsr_zifencei, ilp32)
# NNoM: third_party/nnom (INITRAMFS-AUC fork)

MEL_DIR = mel_compact

NNOM_SRCS_MEL = $(NNOM_DIR)/src/core/nnom.c \
                $(NNOM_DIR)/src/core/nnom_layers.c \
                $(NNOM_DIR)/src/core/nnom_tensor.c \
                $(NNOM_DIR)/src/core/nnom_utils.c \
                $(NNOM_DIR)/src/layers/nnom_activation.c \
                $(NNOM_DIR)/src/layers/nnom_avgpool.c \
                $(NNOM_DIR)/src/layers/nnom_baselayer.c \
                $(NNOM_DIR)/src/layers/nnom_conv2d.c \
                $(NNOM_DIR)/src/layers/nnom_dense.c \
                $(NNOM_DIR)/src/layers/nnom_flatten.c \
                $(NNOM_DIR)/src/layers/nnom_global_pool.c \
                $(NNOM_DIR)/src/layers/nnom_input.c \
                $(NNOM_DIR)/src/layers/nnom_maxpool.c \
                $(NNOM_DIR)/src/layers/nnom_output.c \
                $(NNOM_DIR)/src/layers/nnom_reshape.c \
                $(NNOM_DIR)/src/layers/nnom_softmax.c \
                $(NNOM_DIR)/src/backends/nnom_local.c \
                $(NNOM_DIR)/src/backends/nnom_local_q15.c

MEL_CFLAGS = -march=rv32imac_zicsr_zifencei -mabi=ilp32 \
             -Os -flto -std=gnu99 \
             -ffunction-sections -fdata-sections \
             -falign-functions=4 \
             -DNNOM_USING_STATIC_MEMORY \
             -DNNOM_BARE_METAL \
             "-DNNOM_LOG(...)=" \
             -DCLK_MHZ=36 -DUART_BAUD_RATE=115200 \
             -DI2S_FIFO_DEPTH=8 \
             -DNNOM_STATIC_BUF_KB=36 \
             -I$(NNOM_INC_DIR) -I$(NNOM_PORT_DIR) \
             -I$(MEL_DIR)

MEL_LDFLAGS = -march=rv32imac_zicsr_zifencei -mabi=ilp32 \
              -nostartfiles \
              -Wl,-T,$(COMMON_DIR)/link.ld \
              -Wl,--defsym=SRAM_SIZE=$(SRAM_SIZE_BYTES) \
              -Wl,--gc-sections \
              -lm -lgcc

$(BUILD_DIR)/mel_compact_4blk_ch36_xip.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/sbrk.c \
    $(MEL_DIR)/kws_bare.c \
    $(NNOM_SRCS_MEL) | $(BUILD_DIR)
	$(CC) $(MEL_CFLAGS) -o $@ $^ $(MEL_LDFLAGS)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/mel_compact_4blk_ch36_xip.bin
	$(CROSS_COMPILE)size $@

mel-compact: $(BUILD_DIR)/mel_compact_4blk_ch36_xip.elf
