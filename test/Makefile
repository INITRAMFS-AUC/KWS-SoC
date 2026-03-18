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

# TODO: decide if a fallback makes sense or an error does
SRAM_DEPTH ?= 32768
SRAM_SIZE_BYTES = $(shell echo $$(($(SRAM_DEPTH) * 4)))

# Use rv32i to match the assembly example
CFLAGS = -march=rv32i_zicsr -g -c -MMD -MP
CFLAGS += $(GLOBAL_UART_CONFIG)
LDFLAGS = -T $(COMMON_DIR)/link.ld --defsym=SRAM_SIZE=$(SRAM_SIZE_BYTES)

# Targets
SANITY_TARGETS = $(BUILD_DIR)/inf_loop.elf $(BUILD_DIR)/inf_loop.dis
UART_TARGETS = $(BUILD_DIR)/hello_world.elf $(BUILD_DIR)/hello_world.dis
FLASH_TARGETS = $(BUILD_DIR)/flash_read_test.elf $(BUILD_DIR)/flash_read_test.dis $(BUILD_DIR)/flash_read_test.bin
RAW_BIN_TARGETS = $(BUILD_DIR)/tests.elf $(BUILD_DIR)/tests.dis $(BUILD_DIR)/tests.bin
ALL_TARGETS = $(SANITY_TARGETS) $(UART_TARGETS) $(FLASH_TARGETS) $(RAW_BIN_TARGETS)

all: $(BUILD_DIR) $(ALL_TARGETS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Sanity check targets
sanity: $(SANITY_TARGETS)

# UART targets
uart: $(UART_TARGETS)

# Flash (XIP) targets
flash: $(FLASH_TARGETS)

# XIP tests - Verilog simulations (auto-builds and runs)
xip-testbench:
	$(MAKE) -C $(XIP_DIR) run-testbench

# Build rules
$(BUILD_DIR)/inf_loop.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/inf_loop.o
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/hello_world.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/hello_world.o
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/flash_read_test.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/flash_read_test.o
	$(LD) $(LDFLAGS) -o $@ $^

# Object file rules
$(BUILD_DIR)/crt0.o: $(COMMON_DIR)/crt0.s $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/inf_loop.o: $(SANITY_C_DIR)/inf_loop.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/hello_world.o: $(UART_C_DIR)/hello_world.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/flash_read_test.o: $(XIP_C_DIR)/flash_read_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

# Disassembly rules
$(BUILD_DIR)/inf_loop.dis: $(BUILD_DIR)/inf_loop.elf
	$(OBJDUMP) -d $< > $@

$(BUILD_DIR)/hello_world.dis: $(BUILD_DIR)/hello_world.elf
	$(OBJDUMP) -d $< > $@

$(BUILD_DIR)/flash_read_test.dis: $(BUILD_DIR)/flash_read_test.elf
	$(OBJDUMP) -d $< > $@

$(BUILD_DIR)/tests.o: $(XIP_C_DIR)/tests.S $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/tests.elf: $(BUILD_DIR)/tests.o
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/tests.dis: $(BUILD_DIR)/tests.elf
	$(OBJDUMP) -d $< > $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O binary $< $@

# Clean up
clean:
	rm -rf $(BUILD_DIR) *.o *.elf *.dis
	$(MAKE) -C $(XIP_DIR) clean

# Phony targets
.PHONY: all clean sanity uart flash xip

-include $(wildcard $(BUILD_DIR)/*.d)
