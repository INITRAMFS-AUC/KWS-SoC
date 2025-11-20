# SoC `cxxrtl` Example
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
make
```

>[!NOTE] 
> This will result in compiling RTL into C++ code and compiling `example_soc_tb.cpp`, the output of which is the executable `example_soc_tb` this acts as your testbench and as a jtag server for `risv-openocd` to connect to.
> More on `cxxrtl` [here](https://yosyshq.readthedocs.io/projects/yosys/en/0.38/cmd/write_cxxrtl.html).


## Running the SoC

You will need three terminals open.

1. To run the jtag server by default in project root, in terminal 1, run:
```
make run
```

You should see this output:
```
./example_soc_tb --port 9824
Waiting for connection on port 9824
```

2. Now run `riscv-openocd` in project root, in terminal 2:

```
riscv-openocd -f openocd.cfg
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

You should see `Connected` in terminal 1 (`example_soc_tb` terminal).

3. Now run `riscv32-unknown-elf-gdb` in the third terminal and run the following command:

```
target extended-remote localhost:3333
```

In terminal 2 (`riscv-openocd`) you should see this output:

```
Info : accepting 'gdb' connection on tcp/3333
hazard3.cpu halted due to debug-request.
```

## Running Example Assembly Code

1. First compile the assembly code:

```
riscv32-unknown-elf-as -march=rv32i -g -o soc_test/asm/inf_loop.o soc_test/asm/inf_loop.s
```

2. Now Link the object file with the linker script, to generate an ELF file:
```
riscv32-unknown-elf-ld -T soc_test/asm/inf_loop.ld -o soc_test/asm/inf_loop.elf soc_test/asm/inf_loop.o
```

3. Run the remote debugging session as mentioned in the previous section expect when running gdb run:
```
riscv32-unknown-elf-gdb -x gdbinit
```

4. Load your ELF in GDB:

```
file soc_test/asm/inf_loop.elf
load
```


## Running Example C Code

1. First compile the C code using the provided Makefile:

```
cd soc_test/c
make
cd ../..
```

2. Run the remote debugging session as mentioned in the "Running the SoC" section, except when running gdb run:
```
riscv32-unknown-elf-gdb -x gdbinit
```

3. Load your ELF in GDB:

```
file soc_test/c/inf_loop.elf
load
```

Now your C code is loaded and you can start debugging.
