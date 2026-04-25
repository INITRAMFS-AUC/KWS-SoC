#!/usr/bin/env python3
"""Capture UART output from the FPGA. Flushes stale bytes first."""
import serial
import sys
import time

DEV = sys.argv[1] if len(sys.argv) > 1 else '/dev/ttyUSB0'
N   = int(sys.argv[2]) if len(sys.argv) > 2 else 532  # 2 rounds + sync

s = serial.Serial(DEV, 115200, timeout=120)

# Flush any stale bytes from kernel + driver buffers
s.reset_input_buffer()
time.sleep(0.5)
s.reset_input_buffer()

print(f"Reading up to {N} bytes from {DEV} (timeout 120s)...", file=sys.stderr)
data = s.read(N)
print(f"Got {len(data)} bytes", file=sys.stderr)
print()
for i in range(0, len(data), 16):
    row = data[i:i+16]
    hexs  = ' '.join(f'{b:02x}' for b in row)
    print(f'{i:04x}: {hexs}')
