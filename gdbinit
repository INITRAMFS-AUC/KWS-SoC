python
import os
import gdb


gdb_port = os.environ.get('GDB_PORT', '3333')
print(f"Connecting to OpenOCD on port {gdb_port}...")
gdb.execute(f"target extended-remote localhost:{gdb_port}")

# Read SRAM_DEPTH from env, default to 32768 if not set
depth = int(os.environ.get('SRAM_DEPTH', '32768'))
size_bytes = depth * 4

# 0x00000000 to size_bytes, rw (SRAM)
gdb.execute(f"mem 0x00000000 {hex(size_bytes)} rw")

# 0x40000000 to 0x40010000, rw (Peripherals)
gdb.execute("mem 0x40000000 0x40010000 rw")

# 0x80000000 to 0x81000000, ro (16MB physical Flash)
gdb.execute("mem 0x80000000 0x81000000 ro")

end

# Display memory regions
info mem

# convenience
set print pretty on
set print array on
set pagination off
