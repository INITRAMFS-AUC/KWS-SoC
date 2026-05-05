#!/bin/bash
# Build kws_spike_debug for RISC-V and run on Spike
set -e

CC=/opt/riscv/gcc15/bin/riscv32-unknown-elf-gcc
SPIKE=/opt/riscv/bin/spike
PK=/opt/riscv/riscv32-unknown-elf/bin/pk
NNOM=/workspace/Desktop/kws-spike-validate/extern/nnom

NNOM_SRCS=$(find $NNOM/src/core $NNOM/src/layers $NNOM/src/backends -name '*.c')

$CC \
  -march=rv32imac_zicsr_zifencei -mabi=ilp32 \
  -O2 -std=c99 \
  -DNNOM_USING_STATIC_MEMORY \
  -DNNOM_BLOCK_NUM=16 \
  -I$NNOM/inc -I$NNOM/port \
  -I. \
  kws_spike_debug.c $NNOM_SRCS \
  -lm -o kws_spike_debug

echo "Build OK — binary: kws_spike_debug"
echo ""
echo "Run with:"
echo "  $SPIKE --isa=rv32imac_zicsr_zifencei -m256 $PK kws_spike_debug | tee activations.log"
