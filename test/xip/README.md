
> **⚠️ WARNING:**
>
> These simulation tests require an external simulated flash memory to be connected to the KWS SoC, either through a Verilog module or a C++ simulation bridge. If no external flash is present, the test will not work as intended. We are still figuring out how to do this. See this [issue](https://github.com/INITRAMFS-AUC/KWS-SoC/issues/8).
>
> **Behavior if no flash is connected:**
>
> The test will repeatedly print the test header and first test label, like this:
>
> ```
> ========================================
> Flash Memory Integration Test
> ========================================
>
> [TEST 1] Basic Flash Read @ 0x00
> 
> ========================================
> Flash Memory Integration Test
> ========================================
>
> [TEST 1] Basic Flash Read @ 0x00
> ... (repeats)
> ```
>
> The reason for this repeated reset is not fully understood, but it is likely due to the CPU stalling or being reset internally when it cannot access the flash.
>
> For the correct **Expected Output** see end of this README.md

# XIP (Execute In Place) / Flash Integration Tests

This directory contains tests for the flash controller integration.

## Flash Read Test (`c/flash_read_test.c`)

A C test that verifies flash memory integration by reading data from the flash controller and validating it via UART output.

### Building the Test

```bash
cd test
make flash
```

or 

inside the root directory (KWS-SoC) run
```bash
make test
```

This builds all tests including the `flash_read_test.c`

### Test Description

The test performs:
1. **Basic Flash Read** - Reads first word from flash base address (0x80000000)
2. **Sequential Reads** - Tests cache line filling with consecutive reads
3. **Cache Hit Test** - Re-reads to verify cache operation
4. **Different Cache Line** - Accesses another cache line to test miss handling
5. **Random Access** - Non-sequential access pattern

## Pre-loading Data into Flash

You must find a way to pre-load the flash with `test/xip/c/init.hex`.

### Flash Init File Format (`init.hex`)

The file uses a simple hex format:
```
@AAAAAA    ← Byte address in hex
DD         ← Data byte 0
DD         ← Data byte 1
...
@BBBBBB    ← Next address section
DD
...
```

**Important:**
- Each line after `@address` is ONE byte in hex
- Addresses are byte addresses  
- Data is in little-endian format for 32-bit word reads
- Flash model internal memory is byte-addressed array

### Example: Loading 0x12FEDCBA at address 0x00

```
@000000
BA    ← byte at 0x00 (LSB of word)
DC    ← byte at 0x01
FE    ← byte at 0x02
12    ← byte at 0x03 (MSB of word)
```

When CPU executes: `uint32_t val = *(uint32_t*)0x80000000;`  
Result: `val == 0x12FEDCBA`

### Current Test Data (`init.hex`)

- **0x00-0x03:** 0x12FEDCBA
- **0x04-0x07:** 0xA7A6A5A4
- **0x08-0x0B:** 0xBBBAB9B8
- **0x100-0x103:** 0x03020100


## Expected Output

```
========================================
Flash Memory Integration Test
========================================

[TEST 1] Basic Flash Read @ 0x00
  Expected: 0x12FEDCBA
  Read:     0x12FEDCBA
  Result:   PASS

[TEST 2] Sequential Reads (Cache Fill)
  Word 1 - Expected: 0xA7A6A5A4, Read: 0xA7A6A5A4 - PASS
  Word 2 - Expected: 0xBBBAB9B8, Read: 0xBBBAB9B8 - PASS

[TEST 3] Cache Hit Test (Re-read)
  Re-reading word 0: 0x12FEDCBA - PASS

[TEST 4] Different Cache Line Access
  Reading from offset 256: 0x03020100 - PASS

[TEST 5] Random Access Pattern
  Read words at index 10, 5, 15 - PASS

========================================
ALL TESTS PASSED!
Flash integration successful.
========================================
```

## Testbench-Only Simulations

This directory also contains Verilog-only testbenches in `testbench/`:
- `ahbl_flash_ctrl_eb_cache_tb.v` - Full controller + cache testbench
- `flash_ctrl_eb_tb.v` - Flash controller only
- `ro_dmc_tb.v` - Cache only

Run with:
```bash
cd test/xip
make run
```

These are standalone hardware simulations that don't require the full SoC.
