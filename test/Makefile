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
DMA_C_DIR    = dma/c

# Header include paths (relative to this Makefile's directory)
I2S_REGS_DIR  = ../peris/i2s/i2s_apb/i2s_itr2
UART_REGS_DIR = ../Hazard3/example_soc/libfpga/peris/uart
DMAC_FW_DIR   = ../peris/MS_DMAC_AHBL/fw

# Memory config
SRAM_DEPTH ?= 32768
SRAM_SIZE_BYTES = $(shell echo $$(($(SRAM_DEPTH) * 4)))

# Compiler flags
CFLAGS  = -march=rv32i_zicsr -mabi=ilp32 -g -c -MMD -MP
CFLAGS += $(GLOBAL_UART_CONFIG)
CFLAGS += -I$(I2S_REGS_DIR)
CFLAGS += -I$(UART_REGS_DIR)
CFLAGS += -I$(DMAC_FW_DIR)
CFLAGS += -I$(I2S_C_DIR)

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
	$(BUILD_DIR)/memcpy_simple_test_sram.elf  $(BUILD_DIR)/memcpy_simple_test_sram.dis  $(BUILD_DIR)/memcpy_simple_test_sram.bin \
	# $(BUILD_DIR)/strided_s16_nodil_sram.elf   $(BUILD_DIR)/strided_s16_nodil_sram.dis   $(BUILD_DIR)/strided_s16_nodil_sram.bin
	$(BUILD_DIR)/dma_test_sram.elf    $(BUILD_DIR)/dma_test_sram.dis    $(BUILD_DIR)/dma_test_sram.bin

# *_xip: standalone workflow — `NO_JTAG=1 make sim FLASH=test/build/<test>_xip.bin`
XIP_TARGETS = \
	$(BUILD_DIR)/hello_world_xip.elf     $(BUILD_DIR)/hello_world_xip.dis     $(BUILD_DIR)/hello_world_xip.bin \
	$(BUILD_DIR)/readI2s_xip.elf         $(BUILD_DIR)/readI2s_xip.dis         $(BUILD_DIR)/readI2s_xip.bin \
	$(BUILD_DIR)/flash_read_test_xip.elf $(BUILD_DIR)/flash_read_test_xip.dis $(BUILD_DIR)/flash_read_test_xip.bin \
	$(BUILD_DIR)/tests_xip.elf           $(BUILD_DIR)/tests_xip.dis           $(BUILD_DIR)/tests_xip.bin \
	$(BUILD_DIR)/malloc_simple_test_xip.elf  $(BUILD_DIR)/malloc_simple_test_xip.dis  $(BUILD_DIR)/malloc_simple_test_xip.bin \
	$(BUILD_DIR)/malloc_torture_test_xip.elf $(BUILD_DIR)/malloc_torture_test_xip.dis $(BUILD_DIR)/malloc_torture_test_xip.bin \
	$(BUILD_DIR)/memcpy_simple_test_xip.elf  $(BUILD_DIR)/memcpy_simple_test_xip.dis  $(BUILD_DIR)/memcpy_simple_test_xip.bin \
	$(BUILD_DIR)/strided_s16_nodil_xip.elf   $(BUILD_DIR)/strided_s16_nodil_xip.dis   $(BUILD_DIR)/strided_s16_nodil_xip.bin \
	$(BUILD_DIR)/dma_test_xip.elf        $(BUILD_DIR)/dma_test_xip.dis        $(BUILD_DIR)/dma_test_xip.bin 

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
dma:    $(BUILD_DIR)/dma_test_sram.elf    $(BUILD_DIR)/dma_test_sram.dis    $(BUILD_DIR)/dma_test_sram.bin \
        $(BUILD_DIR)/dma_test_xip.elf     $(BUILD_DIR)/dma_test_xip.dis     $(BUILD_DIR)/dma_test_xip.bin
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

$(BUILD_DIR)/dma_test_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/dma_test.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/MS_DMAC_AHBL.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

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

$(BUILD_DIR)/dma_test_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/dma_test.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/MS_DMAC_AHBL.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

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
$(BUILD_DIR)/dma_test.o: $(DMA_C_DIR)/dma_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/MS_DMAC_AHBL.o: $(DMAC_FW_DIR)/MS_DMAC_AHBL.c $(ROOT_MAKEFILE)
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

.PHONY: all clean sram xip sanity uart i2s flash malloc malloc-simple malloc-torture memcpy model-strided-s16-nodil xip-testbench

# Include dependency files
-include $(wildcard $(BUILD_DIR)/*.d)
