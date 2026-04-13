# XIP (Execute In Place) / Flash Integration Tests

This directory contains tests for the flash controller integration.

## Flash Smoke Test (`c/flash_read_test.c`)

A minimal XIP smoke test that verifies the CPU can boot from flash and that crt0's
`.data` section copy (flash LMA → SRAM VMA) works correctly.  `main()` reads and
writes an initialised variable in SRAM, then loops.  There is no UART output; use
GDB to inspect `my_initialized_var` and `my_zeroed_var`.

### Building

From the project root:
```bash
make test
```

Or from the `test/` directory:
```bash
cd test
make flash
```

This builds `test/build/flash_read_test_xip.elf` and `test/build/flash_read_test_xip.bin`.

### Running standalone (no GDB)

The CPU's `RESET_VECTOR` is `0x80000000`, so it boots from flash immediately — no
`set $pc` needed.

```bash
NO_JTAG=1 make sim-verilator FLASH=test/build/flash_read_test_xip.bin
```

The testbench will print the heartbeat line and exit at `--cycles` limit.  No UART
output is expected; a clean run means the CPU booted from flash without faulting.

### Running with GDB (inspect variables)

```bash
# Terminal 1 — sim
make sim-verilator FLASH=test/build/flash_read_test_xip.bin

# Terminal 2 — OpenOCD
riscv-openocd -f openocd/sim.cfg

# Terminal 3 — GDB
riscv32-unknown-elf-gdb -x gdbinit
```

In GDB:
```gdb
file test/build/flash_read_test_xip.elf   # load symbols only (flash is read-only)
continue
# Ctrl-C to halt, then:
print my_initialized_var    # should be 43 (42 + 1)
print my_zeroed_var         # should be 43
```

`gdbinit` marks `0x80000000–0x81000000` as `ro` so GDB `load` will not attempt to
overwrite the pre-loaded flash content.

## Testbench-Only Simulations

This directory also contains Verilog-only testbenches in `testbench/` that exercise
the flash controller and cache in isolation (no full SoC required):

- `ahbl_flash_ctrl_eb_cache_tb.v` — Full controller + cache testbench
- `flash_ctrl_eb_tb.v` — Flash controller only
- `ro_dmc_tb.v` — Cache only

Run with:
```bash
cd test/xip
make run
```

Or from the project root:
```bash
make test-xip
```

### Flash Init File Format (`init.hex`)

The Verilog testbenches use `init.hex` to pre-load flash memory:

```
@AAAAAA    ← Byte address in hex
DD         ← Data byte 0
DD         ← Data byte 1
...
```

Each line after `@address` is one byte in hex; addresses are byte addresses;
data is little-endian for 32-bit word reads.
