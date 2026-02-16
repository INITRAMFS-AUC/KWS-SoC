ROOT_MAKEFILE = ../Makefile
CROSS_COMPILE = riscv32-unknown-elf-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump

# Directories
BUILD_DIR = build
COMMON_DIR = common
SANITY_C_DIR = sanity_checks/c
UART_C_DIR = uart/c
XIP_C_DIR = xip/c
XIP_DIR = xip

# Use rv32i to match the assembly example
CFLAGS = -march=rv32i -g -c
CFLAGS += $(GLOBAL_UART_CONFIG)
LDFLAGS = -T $(COMMON_DIR)/link.ld

# Targets
SANITY_TARGETS = $(BUILD_DIR)/inf_loop.elf $(BUILD_DIR)/inf_loop.dis
UART_TARGETS = $(BUILD_DIR)/hello_world.elf $(BUILD_DIR)/hello_world.dis
FLASH_TARGETS = $(BUILD_DIR)/flash_read_test.elf $(BUILD_DIR)/flash_read_test.dis
ALL_TARGETS = $(SANITY_TARGETS) $(UART_TARGETS) $(FLASH_TARGETS)

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

# Clean up
clean:
	rm -rf $(BUILD_DIR) *.o *.elf *.dis
	$(MAKE) -C $(XIP_DIR) clean

# Phony targets
.PHONY: all clean sanity uart flash xip
