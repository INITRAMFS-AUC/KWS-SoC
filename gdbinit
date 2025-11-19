# Connect to OpenOCD right away
target extended-remote localhost:3333

# SRAM region: 128 KB at 0x00000000 (0x20000 bytes)
# Readable, writable, executable, cached
mem 0x00000000 0x00020000 rw

# Peripheral region: 64 KB starting at 0x40000000
# Timer at 0x40000000, UART at 0x40004000
# Readable, writable, but not executable, not cached
mem 0x40000000 0x40010000 rw

# Display memory regions
info mem

# convenience
set print pretty on
set print array on
set pagination off
