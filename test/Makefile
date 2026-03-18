ROOT_MAKEFILE = ../Makefile
CROSS_COMPILE = riscv32-unknown-elf-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy

# Directories
BUILD_DIR = build
COMMON_DIR = common
SANITY_C_DIR = sanity_checks/c
UART_C_DIR = uart/c
XIP_C_DIR = xip/c
XIP_DIR = xip

# Memory config
SRAM_DEPTH ?= 32768
SRAM_SIZE_BYTES = $(shell echo $$(($(SRAM_DEPTH) * 4)))

# Compiler & Linker flags
CFLAGS = -march=rv32i_zicsr -g -c -MMD -MP
CFLAGS += $(GLOBAL_UART_CONFIG)
LDFLAGS = -T $(COMMON_DIR)/link.ld --defsym=SRAM_SIZE=$(SRAM_SIZE_BYTES)

# --- TARGET LISTS (Added .bin targets here) ---
SANITY_TARGETS = $(BUILD_DIR)/inf_loop.elf $(BUILD_DIR)/inf_loop.dis $(BUILD_DIR)/inf_loop.bin
UART_TARGETS = $(BUILD_DIR)/hello_world.elf $(BUILD_DIR)/hello_world.dis $(BUILD_DIR)/hello_world.bin
FLASH_TARGETS = $(BUILD_DIR)/flash_read_test.elf $(BUILD_DIR)/flash_read_test.dis $(BUILD_DIR)/flash_read_test.bin
RAW_BIN_TARGETS = $(BUILD_DIR)/tests.elf $(BUILD_DIR)/tests.dis $(BUILD_DIR)/tests.bin
ALL_TARGETS = $(SANITY_TARGETS) $(UART_TARGETS) $(FLASH_TARGETS) $(RAW_BIN_TARGETS)

all: $(BUILD_DIR) $(ALL_TARGETS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Alias targets
sanity: $(SANITY_TARGETS)
uart: $(UART_TARGETS)
flash: $(FLASH_TARGETS)
xip-testbench:
	$(MAKE) -C $(XIP_DIR) run-testbench

# --- UNIFIED LINKING RULES ---
# Standard C files linked with crt0.o
$(BUILD_DIR)/%.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/%.o
	$(LD) $(LDFLAGS) -o $@ $^

# The raw assembly test is linked without crt0.o
$(BUILD_DIR)/tests.elf: $(BUILD_DIR)/tests.o
	$(LD) $(LDFLAGS) -o $@ $^

# --- EXPLICIT COMPILATION RULES ---
$(BUILD_DIR)/crt0.o: $(COMMON_DIR)/crt0.s $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/inf_loop.o: $(SANITY_C_DIR)/inf_loop.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/hello_world.o: $(UART_C_DIR)/hello_world.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/flash_read_test.o: $(XIP_C_DIR)/flash_read_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/tests.o: $(XIP_C_DIR)/tests.S $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

# --- BINARY & DISASSEMBLY RULES ---
$(BUILD_DIR)/%.dis: $(BUILD_DIR)/%.elf
	$(OBJDUMP) -d $< > $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD_DIR) *.o *.elf *.dis
	$(MAKE) -C $(XIP_DIR) clean

.PHONY: all clean sanity uart flash xip xip-testbench

# Include dependency files
-include $(wildcard $(BUILD_DIR)/*.d)
