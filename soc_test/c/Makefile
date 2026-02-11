ROOT_MAKEFILE = ../../Makefile
CROSS_COMPILE = riscv32-unknown-elf-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump

# Use rv32i to match the assembly example
CFLAGS = -march=rv32i -g -c
CFLAGS += $(GLOBAL_UART_CONFIG)
LDFLAGS = -T link.ld

all: inf_loop.elf inf_loop.dis hello_world.elf hello_world.dis

inf_loop.elf: crt0.o inf_loop.o
	$(LD) $(LDFLAGS) -o $@ $^

hello_world.elf: crt0.o hello_world.o
	$(LD) $(LDFLAGS) -o $@ $^

crt0.o: crt0.s $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

inf_loop.o: inf_loop.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

hello_world.o: hello_world.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

inf_loop.dis: inf_loop.elf
	$(OBJDUMP) -d $< > $@

hello_world.dis: hello_world.elf
	$(OBJDUMP) -d $< > $@

clean:
	rm -f *.o *.elf *.dis
