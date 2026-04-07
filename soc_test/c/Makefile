# 1. Guard against direct invocation
# If GLOBAL_UART_CONFIG isn't set, tell the user to use the root Makefile
ifndef GLOBAL_UART_CONFIG
$(error GLOBAL_UART_CONFIG is missing. Please build by running 'make test' from the project root)
endif

I2S_REGS_DIR = ../../peris/i2s/i2s_apb/i2s_itr2
ROOT_MAKEFILE = ../../Makefile
CROSS_COMPILE = riscv32-unknown-elf-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump

# 2. Compiler and Linker Flags
# -march=rv32i -mabi=ilp32 ensures standard 32-bit RISC-V integer instructions
CFLAGS = -march=rv32i -mabi=ilp32 -g -c
CFLAGS += $(GLOBAL_UART_CONFIG)
CFLAGS += -I$(I2S_REGS_DIR)

# It's generally better to let GCC drive the linker rather than using LD directly.
# -nostartfiles prevents the compiler from linking standard system startup files.
# -Wl,-T,link.ld passes the linker script explicitly.
LDFLAGS = -nostartfiles -Wl,-T,link.ld 

# 3. Targets
all: inf_loop.elf inf_loop.dis hello_world.elf hello_world.dis readI2s.elf readI2s.dis

# Link step (using CC instead of LD)
inf_loop.elf: crt0.o inf_loop.o
	$(CC) $(LDFLAGS) -o $@ $^

hello_world.elf: crt0.o hello_world.o
	$(CC) $(LDFLAGS) -o $@ $^

readI2s.elf: crt0.s readI2s.o uart.o
	$(CC) $(LDFLAGS) -o $@ $^

# Compile steps
crt0.o: crt0.s $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

inf_loop.o: inf_loop.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

hello_world.o: hello_world.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

readI2s.o: readI2s.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

uart.o: uart.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

# Disassembly steps
inf_loop.dis: inf_loop.elf
	$(OBJDUMP) -d $< > $@

hello_world.dis: hello_world.elf
	$(OBJDUMP) -d $< > $@

readI2s.dis: readI2s.elf
	$(OBJDUMP) -d $< > $@

clean:
	rm -f *.o *.elf *.dis
