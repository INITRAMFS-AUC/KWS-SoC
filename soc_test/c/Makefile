CROSS_COMPILE = riscv32-unknown-elf-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump

# Use rv32i to match the assembly example
CFLAGS = -march=rv32i -g -c
LDFLAGS = -T link.ld

all: inf_loop.elf inf_loop.dis

inf_loop.elf: crt0.o inf_loop.o
	$(LD) $(LDFLAGS) -o $@ $^

crt0.o: crt0.s
	$(CC) $(CFLAGS) -o $@ $<

inf_loop.o: inf_loop.c
	$(CC) $(CFLAGS) -o $@ $<

inf_loop.dis: inf_loop.elf
	$(OBJDUMP) -d $< > $@

clean:
	rm -f *.o *.elf *.dis
