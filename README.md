# FPGA prototyping on Quartus

1.  Shell Vars You need:

| Variable | desc | Makefile Default |
| --- | --- | --- | 
| `quartus_*` | These are quartus CLI commands such as `quartus_map`, `quartus_sh`. They are extremely important to have them in your `PATH` variable, locate your quartus Installation. | **No Default** |
| `FPGA_FAMILY` | The family of the FPGA chip | "Cyclone V" |
| `FPGA_PART` | The part model of the FPGA chip | 5CSXFC6D6F31C6N |

shell config template can be found at `soc_conf.sh`, which is just a bunch of export statements

2. Quartus CLI Development Workflow

| Target | desc | depends on |
| --- | --- | --- |
| `make config`      | Quartus Project Generation         | -        |
| `make map`         | Synthesis                         | `config` |
| `make fit`         | Fitting                           | `map`    |
| `make asm`         | Bit Stream generation             | `fit`    |
| `make sta`         | Static Timing Analysis            | `fit`    |
| `make check_timing`| Timing Violations                 | `sta`    |
| `make program`     | Programming FPGA                  | `asm`    |
| `make test`        | Build all C/ASM firmware (see test/Makefile) | - |
| `make test-xip`    | Build and run XIP Verilog simulations         | - |
| `make clean`       | Clean all build artifacts                     | - |

>[!NOTE]
> **For GUI development:**
> You can just open the .qsf file generated in `quartus/` after running `make config`

>[!IMPORTANT]
> 1. If you change Global SoC config parameters such as `SRAM_DEPTH` you need to recompile the project
>  so Run `make clean` and start over
> 2. Programming Your FPGA will be done through USB-Blaster and JTAG chain, if your setup is different modify the `program` target in the Makefile
> 3. Make knows that previous steps in EDA flow should not be re-done, i.e, if you ran `map` stage you do not need to redo it if you run `fit` stage.

# SoC `cxxrtl` Example (kws_soc)
Implementation for a Simulated SoC model on `cxxrtl` with a jtag bitbanging wrapper testbench for `openocd` to connect to.

## Building

### Prerequisites

1. First Install Yosys

Ubuntu (apt)
```
sudo apt-get install yosys
```

ArchLinux Based Systems
```
sudo pacman -S yosys
```
> Or build it from source version `b1569de5`, as per Hazard3 Repo instructions


2. Make sure you have riscv-openocd built from source, as per Hazard3 repo instructions:
```
cd /tmp
git clone https://github.com/riscv/riscv-openocd.git
cd riscv-openocd
./bootstrap
# Prefix is optional
./configure --enable-remote-bitbang --enable-ftdi --program-prefix=riscv-
make -j $(nproc)
sudo make install
```

>[!WARNING]
> This might not go smoothly, so if it doesn't go smoothly either try to build as best you can by either modifying the Makefiles generated and/or building its submodules separately or contacting me.


3. Make sure you have a riscv32 gcc14-based gnu toolchain installed with the following steps as per Hazard3 repo instructions:

- Prerequisites for your system

Ubuntu 24

```bash
# Prerequisites for Ubuntu 24.04
sudo apt install autoconf automake autotools-dev curl python3 python3-pip libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev ninja-build git cmake libglib2.0-dev libslirp-dev
```

```bash
cd /tmp
git clone https://github.com/riscv/riscv-gnu-toolchain
cd riscv-gnu-toolchain
git clone --depth=1 https://github.com/gcc-mirror/gcc gcc-14 -b releases/gcc-14
./configure --with-gcc-src=$(pwd)/gcc-14 --prefix=/opt/riscv/gcc14-no-zcmp --with-arch=rv32ia_zicsr --with-abi=ilp32 --with-multilib-generator="rv32i-ilp32--;rv32im-ilp32--;rv32ia-ilp32--;rv32ima-ilp32--;rv32ic-ilp32--;rv32imc-ilp32--;rv32iac-ilp32--;rv32imac-ilp32--;rv32i_zicsr-ilp32--;rv32im_zicsr-ilp32--;rv32ia_zicsr-ilp32--;rv32ima_zicsr-ilp32--;rv32ic_zicsr-ilp32--;rv32imc_zicsr-ilp32--;rv32iac_zicsr-ilp32--;rv32imac_zicsr-ilp32--;rv32i_zicsr_zifencei-ilp32--;rv32im_zicsr_zifencei-ilp32--;rv32ia_zicsr_zifencei-ilp32--;rv32ima_zicsr_zifencei-ilp32--;rv32ic_zicsr_zifencei-ilp32--;rv32imc_zicsr_zifencei-ilp32--;rv32iac_zicsr_zifencei-ilp32--;rv32imac_zicsr_zifencei-ilp32--;rv32im_zba_zbb_zbs-ilp32--;rv32ima_zba_zbb_zbs-ilp32--;rv32imc_zba_zbb_zbs-ilp32--;rv32imac_zba_zbb_zbs-ilp32--;rv32im_zicsr_zba_zbb_zbs-ilp32--;rv32ima_zicsr_zba_zbb_zbs-ilp32--;rv32imc_zicsr_zba_zbb_zbs-ilp32--;rv32imac_zicsr_zba_zbb_zbs-ilp32--;rv32im_zicsr_zifencei_zba_zbb_zbs-ilp32--;rv32ima_zicsr_zifencei_zba_zbb_zbs-ilp32--;rv32imc_zicsr_zifencei_zba_zbb_zbs-ilp32--;rv32imac_zicsr_zifencei_zba_zbb_zbs-ilp32--;rv32im_zba_zbb_zbs_zbkb-ilp32--;rv32ima_zba_zbb_zbs_zbkb-ilp32--;rv32imc_zba_zbb_zbs_zbkb-ilp32--;rv32imac_zba_zbb_zbs_zbkb-ilp32--;rv32im_zicsr_zba_zbb_zbs_zbkb-ilp32--;rv32ima_zicsr_zba_zbb_zbs_zbkb-ilp32--;rv32imc_zicsr_zba_zbb_zbs_zbkb-ilp32--;rv32imac_zicsr_zba_zbb_zbs_zbkb-ilp32--;rv32im_zicsr_zifencei_zba_zbb_zbs_zbkb-ilp32--;rv32ima_zicsr_zifencei_zba_zbb_zbs_zbkb-ilp32--;rv32imc_zicsr_zifencei_zba_zbb_zbs_zbkb-ilp32--;rv32imac_zicsr_zifencei_zba_zbb_zbs_zbkb-ilp32--;rv32im_zba_zbb_zbc_zbs_zbkb-ilp32--;rv32ima_zba_zbb_zbc_zbs_zbkb-ilp32--;rv32imc_zba_zbb_zbc_zbs_zbkb-ilp32--;rv32imac_zba_zbb_zbc_zbs_zbkb-ilp32--;rv32im_zicsr_zba_zbb_zbc_zbs_zbkb-ilp32--;rv32ima_zicsr_zba_zbb_zbc_zbs_zbkb-ilp32--;rv32imc_zicsr_zba_zbb_zbc_zbs_zbkb-ilp32--;rv32imac_zicsr_zba_zbb_zbc_zbs_zbkb-ilp32--;rv32im_zicsr_zifencei_zba_zbb_zbc_zbs_zbkb-ilp32--;rv32ima_zicsr_zifencei_zba_zbb_zbc_zbs_zbkb-ilp32--;rv32imc_zicsr_zifencei_zba_zbb_zbc_zbs_zbkb-ilp32--;rv32imac_zicsr_zifencei_zba_zbb_zbc_zbs_zbkb-ilp32--;rv32i_zca-ilp32--;rv32im_zca-ilp32--;rv32ia_zca-ilp32--;rv32ima_zca-ilp32--;rv32i_zicsr_zca-ilp32--;rv32im_zicsr_zca-ilp32--;rv32ia_zicsr_zca-ilp32--;rv32ima_zicsr_zca-ilp32--;rv32i_zicsr_zifencei_zca-ilp32--;rv32im_zicsr_zifencei_zca-ilp32--;rv32ia_zicsr_zifencei_zca-ilp32--;rv32ima_zicsr_zifencei_zca-ilp32--;rv32im_zba_zbb_zbs_zca-ilp32--;rv32ima_zba_zbb_zbs_zca-ilp32--;rv32im_zicsr_zba_zbb_zbs_zca-ilp32--;rv32ima_zicsr_zba_zbb_zbs_zca-ilp32--;rv32im_zicsr_zifencei_zba_zbb_zbs_zca-ilp32--;rv32ima_zicsr_zifencei_zba_zbb_zbs_zca-ilp32--;rv32im_zba_zbb_zbs_zbkb_zca-ilp32--;rv32ima_zba_zbb_zbs_zbkb_zca-ilp32--;rv32im_zicsr_zba_zbb_zbs_zbkb_zca-ilp32--;rv32ima_zicsr_zba_zbb_zbs_zbkb_zca-ilp32--;rv32im_zicsr_zifencei_zba_zbb_zbs_zbkb_zca-ilp32--;rv32ima_zicsr_zifencei_zba_zbb_zbs_zbkb_zca-ilp32--;rv32im_zba_zbb_zbc_zbs_zbkb_zca-ilp32--;rv32ima_zba_zbb_zbc_zbs_zbkb_zca-ilp32--;rv32im_zicsr_zba_zbb_zbc_zbs_zbkb_zca-ilp32--;rv32ima_zicsr_zba_zbb_zbc_zbs_zbkb_zca-ilp32--;rv32im_zicsr_zifencei_zba_zbb_zbc_zbs_zbkb_zca-ilp32--;rv32ima_zicsr_zifencei_zba_zbb_zbc_zbs_zbkb_zca-ilp32--;rv32i_zca_zcb-ilp32--;rv32im_zca_zcb-ilp32--;rv32ia_zca_zcb-ilp32--;rv32ima_zca_zcb-ilp32--;rv32i_zicsr_zca_zcb-ilp32--;rv32im_zicsr_zca_zcb-ilp32--;rv32ia_zicsr_zca_zcb-ilp32--;rv32ima_zicsr_zca_zcb-ilp32--;rv32i_zicsr_zifencei_zca_zcb-ilp32--;rv32im_zicsr_zifencei_zca_zcb-ilp32--;rv32ia_zicsr_zifencei_zca_zcb-ilp32--;rv32ima_zicsr_zifencei_zca_zcb-ilp32--;rv32im_zba_zbb_zbs_zca_zcb-ilp32--;rv32ima_zba_zbb_zbs_zca_zcb-ilp32--;rv32im_zicsr_zba_zbb_zbs_zca_zcb-ilp32--;rv32ima_zicsr_zba_zbb_zbs_zca_zcb-ilp32--;rv32im_zicsr_zifencei_zba_zbb_zbs_zca_zcb-ilp32--;rv32ima_zicsr_zifencei_zba_zbb_zbs_zca_zcb-ilp32--;rv32im_zba_zbb_zbs_zbkb_zca_zcb-ilp32--;rv32ima_zba_zbb_zbs_zbkb_zca_zcb-ilp32--;rv32im_zicsr_zba_zbb_zbs_zbkb_zca_zcb-ilp32--;rv32ima_zicsr_zba_zbb_zbs_zbkb_zca_zcb-ilp32--;rv32im_zicsr_zifencei_zba_zbb_zbs_zbkb_zca_zcb-ilp32--;rv32ima_zicsr_zifencei_zba_zbb_zbs_zbkb_zca_zcb-ilp32--;rv32im_zba_zbb_zbc_zbs_zbkb_zca_zcb-ilp32--;rv32ima_zba_zbb_zbc_zbs_zbkb_zca_zcb-ilp32--;rv32im_zicsr_zba_zbb_zbc_zbs_zbkb_zca_zcb-ilp32--;rv32ima_zicsr_zba_zbb_zbc_zbs_zbkb_zca_zcb-ilp32--;rv32im_zicsr_zifencei_zba_zbb_zbc_zbs_zbkb_zca_zcb-ilp32--;rv32ima_zicsr_zifencei_zba_zbb_zbc_zbs_zbkb_zca_zcb-ilp32--"
sudo mkdir -p /opt/riscv/gcc14-no-zcmp
sudo chown $(whoami) /opt/riscv/gcc14-no-zcmp
make -j $(nproc)
```

Now do not forget to add the lib/binaries to your `PATH` variable
```bash
export PATH="$PATH:/opt/riscv/gcc14-no-zcmp/bin"
```
> You can add them to your `.rc` file if needed, if you are using `bash` shell for example add the export statement above to your `~/.bashrc` file.


4. (optional) Install verilator for linting

Ubuntu (apt)
```
sudo apt install verilator
```

ArchLinux Based Systems
```
sudo pacman -S verilator
```

### Building the `cxxrtl` simulation

To build the simulation executable run:
```bash
make sim_yosys
```

>[!NOTE] 
> This will result in compiling RTL into C++ code and compiling `kws_soc_tb.cpp`, the output of which is the executable `./build/yosys/kws_soc_tb` this acts as your testbench and as a jtag server for `risv-openocd` to connect to.
> More on `cxxrtl` [here](https://yosyshq.readthedocs.io/projects/yosys/en/0.38/cmd/write_cxxrtl.html).


## Running C/ASM Tests

All C tests are organized under `test/`. Each test is compiled in **two variants**:

| Binary suffix | Linked for | How to run |
| --- | --- | --- |
| `_sram` | SRAM — all code+data at `0x00000000` | GDB `load` into a running simulation |
| `_xip`  | XIP flash — code+rodata at `0x80000000`, data in SRAM | `--no-jtag --flash` standalone mode |

The distinction matters because string literals and function pointers are linked at their **VMA**.
Loading a `_sram` binary into flash and booting the CPU from `0x80000000` would point all `.rodata`
accesses at the wrong addresses and produce no output.

| Source directory | Test | `_sram` (GDB) | `_xip` (standalone) |
| --- | --- | --- | --- |
| `test/sanity_checks/c/` | `inf_loop.c` | `build/inf_loop_sram.elf` | — |
| `test/uart/c/` | `hello_world.c` | `build/hello_world_sram.elf` | `build/hello_world_xip.bin` |
| `test/i2s/c/` | `readI2s.c` | `build/readI2s_sram.elf` | `build/readI2s_xip.bin` |
| `test/xip/c/` | `flash_read_test.c` | — | `build/flash_read_test_xip.bin` |

To build all tests:

```bash
make test
```

Or build individual categories:

```bash
cd test
make sanity   # inf_loop (SRAM only)
make uart     # hello_world SRAM + XIP
make i2s      # readI2s SRAM + XIP
make flash    # flash_read_test XIP only
make sram     # all SRAM variants
make xip      # all XIP variants
```

Build artifacts are placed in `test/build/`.

### Standalone (no-GDB) mode

XIP binaries can be run without OpenOCD or GDB using `--no-jtag`.
The CPU boots from `RESET_VECTOR = 0x80000000` immediately:

```bash
# UART hello world — standalone
NO_JTAG=1 make sim-verilator FLASH=test/build/hello_world_xip.bin

# I2S smoke test — standalone with mic data
NO_JTAG=1 make sim-verilator FLASH=test/build/readI2s_xip.bin MIC=sim/debug_audio.hex

# Flash/XIP smoke test — standalone
NO_JTAG=1 make sim-verilator FLASH=test/build/flash_read_test_xip.bin
```

Add `EXTRA_ARGS="--cycles N"` to limit simulation length.

### I2S Microphone Test

The `readI2s.c` test exercises the APB I2S receiver peripheral at `0x40008000`.
It configures the I2S clock divider, enables the IRQ, waits for the FIFO to
fill, and prints received samples over UART.

> **Note — cxxrtl limitation:** The I2S interrupt test does **not** work with
> the cxxrtl (`make sim`) testbench. `i2s_rx_core` uses `always @(negedge sck)`
> where `sck` is an internally-generated fabric register. cxxrtl caches
> edge-detection variables at the start of `eval()` before register `.next`
> values are updated, so the negedge of `sck_reg` is never detected — the FIFO
> never fills and the IRQ never fires. Use the **Verilator** simulation
> (`make sim_verilator`) for I2S interrupt testing; Verilator handles
> internally-generated clocks correctly.

**Standalone (XIP) — no GDB required:**
```bash
NO_JTAG=1 make sim-verilator FLASH=test/build/readI2s_xip.bin MIC=sim/debug_audio.hex
```

**GDB workflow (SRAM binary):**

1. Build:
```bash
make test
```

2. Run the **Verilator** simulation with an optional audio hex file:
```bash
make sim-verilator MIC=<audio.hex>
```
If `--mic` is omitted the mic outputs zeros.

3. Connect OpenOCD and GDB as usual, then:
```gdb
file test/build/readI2s_sram.elf
load
continue
```

### XIP Flash Smoke Test

`flash_read_test.c` is a minimal XIP smoke test: crt0 copies the `.data` section
from flash to SRAM, then `main()` reads and writes an initialised variable in SRAM.
It verifies that the CPU can boot from flash and that the data copy works, but
produces no UART output.  Attach GDB to inspect variables if needed.

**Standalone:**
```bash
NO_JTAG=1 make sim-verilator FLASH=test/build/flash_read_test_xip.bin
```

**With GDB (symbols only — flash is read-only):**
```bash
make sim-verilator FLASH=test/build/flash_read_test_xip.bin
# then in GDB:
file test/build/flash_read_test_xip.elf
continue
```

See `test/xip/README.md` for the Verilog-only XIP component testbenches.

## Running XIP (Execute-In-Place) Verilog Simulations

You can build and run the XIP Verilog testbenches (adapted from the shalan/SoC-Lab repo) using:

From the project root:
```bash
make test-xip
```
This will:
- Build the XIP Verilog testbench and all required HDL files
- Copy the required `init.hex` to the simulation directory for memory initialization.
- Run the simulation using `vvp`

Simulation output and temporary files are placed in `test/xip/build/`.

You can also run from the test directory:
```bash
cd test
make xip
```
Or from the XIP test directory for more control:
```bash
cd test/xip
make run         # Build and run the default cache testbench
make run-cache   # Build and run the cache testbench
make run-flash-ctrl # Build and run the flash controller testbench
make run-ro-dmc  # Build and run the ro_dmc testbench
```

run `./build/yosys/kws_soc_tb --help` for options

### Building the `verilator` VPI-based simulation

```bash
make sim_verilator
```
Produces `./build/verilator/Vkws_soc`. Run `./build/verilator/Vkws_soc --help`
for all options.

**Run targets** (mirror the cxxrtl `sim` / `sim-vcd` targets):

| Target | What it does |
| --- | --- |
| `make sim-verilator` | Run Verilator sim on port `$(SIM_PORT)` (or standalone with `NO_JTAG=1`) |
| `make sim-verilator-vcd` | Run + dump waveforms to `waves.fst` (or `.vcd` with `TRACE_FORMAT=VCD`) |
| `make sim` | Run cxxrtl sim |
| `make sim-vcd` | Run cxxrtl sim + dump `waves.vcd` |

All four run targets honour these Make variables:

| Variable | Example | Description |
| --- | --- | --- |
| `FLASH` | `FLASH=test/build/flash_read_test_xip.bin` | Pre-load a binary into QSPI flash |
| `MIC` | `MIC=sim/debug_audio.hex` | Feed audio samples to the I2S mic model |
| `NO_JTAG` | `NO_JTAG=1` | Standalone mode — no OpenOCD/GDB needed; CPU boots from flash immediately |
| `EXTRA_ARGS` | `EXTRA_ARGS="--cycles 50000000"` | Pass extra flags directly to the testbench binary |
| `TRACE_FORMAT` | `TRACE_FORMAT=VCD` | Verilator only: choose `VCD` instead of `FST` |

Examples:
```bash
# UART hello world — standalone XIP
NO_JTAG=1 make sim-verilator FLASH=test/build/hello_world_xip.bin

# I2S interrupt test with waveform capture in Verilator
make sim-verilator-vcd MIC=sim/debug_audio.hex

# XIP flash smoke test — standalone
NO_JTAG=1 make sim-verilator FLASH=test/build/flash_read_test_xip.bin

# Verilator, VCD format, flash + mic
make sim-verilator-vcd FLASH=test/build/flash_read_test_xip.bin MIC=audio.hex TRACE_FORMAT=VCD
```

## Running the SoC (GDB workflow)

You will need three terminals open.

1. To run the JTAG server in terminal 1, from the project root use one of the
   Make run targets (recommended) or invoke the binary directly:

```bash
# SRAM-based tests (cxxrtl or Verilator)
make sim                              # cxxrtl
make sim-verilator                    # Verilator

# XIP/flash test
make sim-verilator FLASH=test/build/flash_read_test_xip.bin

# I2S smoke test (ID read only) — cxxrtl is fine for this
make sim MIC=sim/debug_audio.hex

# I2S interrupt test — Verilator required (see I2S section above)
make sim-verilator MIC=sim/debug_audio.hex

# Capture waveforms (FST by default; TRACE_FORMAT=VCD for classic VCD)
make sim-vcd MIC=sim/debug_audio.hex
make sim-verilator-vcd MIC=sim/debug_audio.hex
```

>[!NOTE]
> `SIM_PORT` defaults to `9000 + (UID % 1000)` so multiple users on the same
> host don't collide.  Override with `make sim SIM_PORT=9824`.
> The testbench also accepts `--cycles <n>` to limit simulation length.


You should see this output:
```
Waiting for connection on port <port>
```

2. Now run `riscv-openocd` in project root, in terminal 2:

```
riscv-openocd -f openocd/sim.cfg
```

You should see this output in terminal 2 (`riscv-openocd` terminal):

```
Open On-Chip Debugger 0.12.0+dev-04404-geb01c632a (2025-11-08-20:11)
Licensed under GNU GPL v2
For bug reports, read
	http://openocd.org/doc/doxygen/bugs.html
DEPRECATED! use 'gdb report_data_abort', not 'gdb_report_data_abort'
Info : Initializing remote_bitbang driver
Info : Connecting to localhost:9824
Info : remote_bitbang driver initialized
Info : Note: The adapter "remote_bitbang" doesn't support configurable speed
Info : JTAG tap: hazard3.cpu tap/device found: 0xdeadbeef (mfg: 0x777 (Fabric of Truth Inc), part: 0xeadb, ver: 0xd)
Info : [hazard3.cpu] datacount=1 progbufsize=2
Info : [hazard3.cpu] Examined RISC-V core
Info : [hazard3.cpu]  XLEN=32, misa=0x40001105
[hazard3.cpu] Target successfully examined.
Info : [hazard3.cpu] Examination succeed
Info : [hazard3.cpu] starting gdb server on 3333
Info : Listening on port 3333 for gdb connections
hazard3.cpu halted due to debug-request.
Info : Listening on port 6666 for tcl connections
Info : Listening on port 4444 for telnet connections
```

You should see `Connected` in terminal 1 (`kws_soc_tb` terminal).

3. Now run `riscv32-unknown-elf-gdb -x gdbinit` in the third terminal and run the following command:

In terminal 2 (`riscv-openocd`) you should see this output:

```
Info : accepting 'gdb' connection on tcp/3333
hazard3.cpu halted due to debug-request.
```

## Running Example Assembly Code


1. First compile the assembly code:
```bash
riscv32-unknown-elf-as -march=rv32i -g -o test/common/inf_loop.o test/common/inf_loop.s
```
2. Now Link the object file with the linker script, to generate an ELF file:
```bash
riscv32-unknown-elf-ld -T test/common/inf_loop.ld -o test/build/inf_loop_sram.elf test/common/inf_loop.o
```
3. Run the remote debugging session as mentioned in the previous section expect when running gdb run:
```bash
riscv32-unknown-elf-gdb -x gdbinit
```
4. Load your ELF in GDB:
```gdb
file test/build/inf_loop_sram.elf
load
```

## Running Example C Code

### Infinite Loop

1. First compile the C code using the provided Makefile:

```
make test
```
2. Run the remote debugging session as mentioned in the "Running the SoC" section, except when running gdb run:
```bash
riscv32-unknown-elf-gdb -x gdbinit
```
3. Load your ELF in GDB:
```gdb
file test/build/inf_loop_sram.elf
load
```

Now your C code is loaded and you can start debugging.

### Hello World

**Standalone (no GDB):**
```bash
NO_JTAG=1 make sim-verilator FLASH=test/build/hello_world_xip.bin
```
Expected output on the testbench terminal:
```
Hello World!
Hello World!
...
```

**GDB workflow:**

1. First compile the C code using the provided Makefile:
```bash
make test
```
2. Run the remote debugging session as mentioned in the "Running the SoC" section, except when running gdb run:
```bash
riscv32-unknown-elf-gdb -x gdbinit
```
3. Load the SRAM-linked ELF in GDB:
```gdb
file test/build/hello_world_sram.elf
load
continue
```

4. You can put breakpoints, or just run the program to find the uart output on the testbench terminal:

```
Waiting for connection on port 9824
Connected
Hello World!
```

## Full Verilator sim flow

```bash
# One-shot rebuild — RTL changes touched receiver, regs, kws_soc.v.
make clean

# Build all firmware binaries (test/build/*.bin) with the model's int8 path.
# I2S_CLK_DIV is the only knob that changes between sim and FPGA frequencies.
# CLK_MHZ defaults to 36; UART_BAUD_RATE defaults to 115200; I2S_FIFO_DEPTH=16.
# kws_bare_main.c hard-codes WIDTH=8 itself, so no I2S_WIDTH_MODE here.
make test \
    I2S_CLK_DIV=5            # 36 MHz / (128*6) = 46.875 kHz raw → 15.6 kHz ring → 7.8 kHz model

# Build the Verilator simulator with matching CLK_MHZ / UART_BAUD_RATE.
make sim_verilator           # consumes the same CLK_MHZ and UART_BAUD_RATE the firmware used

# Run the model firmware end-to-end.
make sim-verilator NO_JTAG=1 \
    FLASH=test/build/mel_compact_int8_xip_accel.bin \
    MIC=sim/playback_samples.hex \
    I2S_CLK_DIV=5            # repeat overrides — sim-verilator depends on `test`, which would otherwise rebuild with defaults
```

For just `recordI2s` (separate test, doesn't touch the model build):

```bash
make test \
    I2S_CLK_DIV=5 \
    RECORD_N_SAMPLES=8000 \
    I2S_WIDTH_MODE=I2S_CONF_WIDTH_8     # match the int8 path

make sim-verilator NO_JTAG=1 \
    FLASH=test/build/recordI2s_xip.bin \
    MIC=sim/playback_samples.hex \
    I2S_CLK_DIV=5 RECORD_N_SAMPLES=8000 I2S_WIDTH_MODE=I2S_CONF_WIDTH_8
```

## Synthesis (Quartus)

```bash
make clean

# Build the firmware first — Quartus bakes the .bin into the bitstream's flash image.
# Same I2S_CLK_DIV applies; CLK_MHZ defaults to 36 (DE10S PLL target).
make test I2S_CLK_DIV=5

# (Re)generate the PLL IP only when CLK_MHZ changes. Skip if 36 MHz is fine.
# make gen_pll CLK_MHZ=36 FPGA_FAMILY="Cyclone V"

# Synth → fit → assemble → SOF.
make config map fit asm \
    CLK_MHZ=36 \
    FPGA_FAMILY="Cyclone V" \
    FPGA_PART=5CSXFC6D6F31C6 \
    FPGA_BOARD=DE10S \
    I2S_CLK_DIV=5

# Static timing report (optional but recommended on first synth):
make sta check_timing
```

## Currently best FPGA run configuration

```bash
make clean && make test-mel-compact-int8-peak-norm-accel I2S_CLK_DIV=17
```
