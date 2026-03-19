# Connect to OpenOCD right away
target extended-remote localhost:3333

python
import os
import gdb

# Read SRAM_DEPTH from env, default to 32768 if not set
depth = int(os.environ.get('SRAM_DEPTH', '32768'))
size_bytes = depth * 4

# 0x00000000 to size_bytes, rw
gdb.execute(f"mem 0x00000000 {hex(size_bytes)} rw")
gdb.execute("mem 0x40000000 0x40010000 rw")
gdb.execute("mem 0x80000000 0x80010000 rw")

end

# Display memory regions
info mem

# convenience
set print pretty on
set print array on
set pagination off
