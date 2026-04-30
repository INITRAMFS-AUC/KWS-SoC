.PHONY: clean
ROOT_MAKEFILE = ../Makefile
CROSS_COMPILE = riscv32-unknown-elf-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy

# Directories
BUILD_DIR    = build
COMMON_DIR   = common
SANITY_C_DIR = sanity_checks/c
UART_C_DIR   = uart/c
I2S_C_DIR    = i2s/c
XIP_C_DIR    = xip/c
XIP_DIR      = xip
DMA_C_DIR    = dma/c

MALLOC_C_DIR = malloc/c
MEMCPY_C_DIR = memcpy/c
MODEL_C_DIR  = model
STREAMING_MODEL_C_DIR = streaming_model
MEL_COMPACT_C_DIR = mel_compact
NNOM_DIR     = ../third_party/nnom
NNOM_INC_DIR = $(NNOM_DIR)/inc
NNOM_PORT_DIR = $(NNOM_DIR)/port

NNOM_SRCS_MODEL = $(NNOM_DIR)/src/core/nnom.c \
                  $(NNOM_DIR)/src/core/nnom_layers.c \
                  $(NNOM_DIR)/src/core/nnom_tensor.c \
                  $(NNOM_DIR)/src/core/nnom_utils.c \
                  $(NNOM_DIR)/src/layers/nnom_activation.c \
                  $(NNOM_DIR)/src/layers/nnom_avgpool.c \
                  $(NNOM_DIR)/src/layers/nnom_baselayer.c \
                  $(NNOM_DIR)/src/layers/nnom_conv2d.c \
                  $(NNOM_DIR)/src/layers/nnom_dense.c \
                  $(NNOM_DIR)/src/layers/nnom_flatten.c \
                  $(NNOM_DIR)/src/layers/nnom_global_pool.c \
                  $(NNOM_DIR)/src/layers/nnom_input.c \
                  $(NNOM_DIR)/src/layers/nnom_maxpool.c \
                  $(NNOM_DIR)/src/layers/nnom_output.c \
                  $(NNOM_DIR)/src/layers/nnom_reshape.c \
                  $(NNOM_DIR)/src/layers/nnom_softmax.c \
                  $(NNOM_DIR)/src/backends/nnom_local.c \
                  $(NNOM_DIR)/src/backends/nnom_local_q15.c

# Header include paths (relative to this Makefile's directory)
I2S_REGS_DIR  = ../peris/i2s/i2s_apb/i2s_itr2
UART_REGS_DIR = ../Hazard3/example_soc/libfpga/peris/uart
DMAC_FW_DIR   = ../peris/MS_DMAC_AHBL/fw

# Memory config
SRAM_DEPTH ?= 32768
SRAM_SIZE_BYTES = $(shell echo $$(($(SRAM_DEPTH) * 4)))

# Compiler flags
CFLAGS  = -march=rv32i_zicsr -mabi=ilp32 -g -c -MMD -MP
CFLAGS += $(GLOBAL_UART_CONFIG)
CFLAGS += -I$(I2S_REGS_DIR)
CFLAGS += -I$(UART_REGS_DIR)
CFLAGS += -I$(DMAC_FW_DIR)
CFLAGS += -I$(I2S_C_DIR)

# Linker flags:
#   SRAM_LDFLAGS – all sections (text/rodata/data/bss) in SRAM at 0x00000000.
#                  Required for GDB `load`; string literals live at their SRAM VMA.
#   XIP_LDFLAGS  – text+rodata in FLASH (0x80000000), data+bss in SRAM.
#                  Required for standalone boot; string literals live at their flash VMA.
SRAM_LDFLAGS = -T $(COMMON_DIR)/sram_link.ld --defsym=SRAM_SIZE=$(SRAM_SIZE_BYTES)
XIP_LDFLAGS  = -T $(COMMON_DIR)/link.ld      --defsym=SRAM_SIZE=$(SRAM_SIZE_BYTES)

# Shared compilation and link flags for all model (NNoM) targets.
# `-g` adds DWARF debug info to the ELF only — `objcopy -O binary` strips
# `.debug_*` so the runtime .bin (and the SRAM image we ship to flash)
# stays the exact same size.  Cost is purely ELF-on-disk + GDB working.
MODEL_CFLAGS = -march=rv32imac_zicsr_zifencei -mabi=ilp32 \
               -Os -flto -g -std=gnu99 \
               -ffunction-sections -fdata-sections \
               -falign-functions=4 \
               -DNNOM_USING_STATIC_MEMORY \
               -DNNOM_BARE_METAL \
               "-DNNOM_LOG(...)=" \
               -DCLK_MHZ=36 -DUART_BAUD_RATE=115200 \
               -DI2S_FIFO_DEPTH=8 \
               -I$(NNOM_INC_DIR) -I$(NNOM_PORT_DIR)

# Truly bare-metal link: drop newlib's libc / libgloss / libgcc entirely.
# `-nostdlib` = -nostartfiles + -nodefaultlibs.  We replace what we'd lose
# with project-local sources so we keep the real soft-float ABI:
#   common/memcpy.c, common/memset.c — gcc emits libcalls to these for
#                    non-trivial copy / fill sizes (NNoM moves activations).
#   common/intrinsics.c              — provides __ashrdi3, the only libgcc
#                    symbol the model build pulls in after --gc-sections
#                    (Conv2D 64-bit requantisation shift).  See that file
#                    for why we don't link the toolchain's libgcc.
# No -lm: no NNoM unit we compile calls into libm (int8 softmax is a LUT,
# no sqrt/exp/etc.).
# Net effect: minimal binary, real -mabi=ilp32 (4-byte stack alignment)
# instead of the libgcc-imposed ilp32d (16-byte alignment).
MODEL_LDFLAGS_COMMON = -march=rv32imac_zicsr_zifencei -mabi=ilp32 \
                       -nostdlib \
                       -Wl,--gc-sections
MODEL_LDFLAGS_SRAM   = $(MODEL_LDFLAGS_COMMON) -Wl,-T,$(COMMON_DIR)/sram_link.ld -Wl,--defsym=SRAM_SIZE=$(SRAM_SIZE_BYTES)
MODEL_LDFLAGS_XIP    = $(MODEL_LDFLAGS_COMMON) -Wl,-T,$(COMMON_DIR)/link.ld     -Wl,--defsym=SRAM_SIZE=$(SRAM_SIZE_BYTES)

# --- TARGET LISTS ---
# *_sram: GDB workflow — `make gdb`, then `load build/<test>_sram.elf`, then `continue`
SRAM_TARGETS = \
	$(BUILD_DIR)/inf_loop_sram.elf            $(BUILD_DIR)/inf_loop_sram.dis            $(BUILD_DIR)/inf_loop_sram.bin \
	$(BUILD_DIR)/hello_world_sram.elf         $(BUILD_DIR)/hello_world_sram.dis         $(BUILD_DIR)/hello_world_sram.bin \
	$(BUILD_DIR)/bridge_bug_test_sram.elf     $(BUILD_DIR)/bridge_bug_test_sram.dis     $(BUILD_DIR)/bridge_bug_test_sram.bin \
	$(BUILD_DIR)/snooper_test_sram.elf        $(BUILD_DIR)/snooper_test_sram.dis        $(BUILD_DIR)/snooper_test_sram.bin \
	$(BUILD_DIR)/readI2s_sram.elf             $(BUILD_DIR)/readI2s_sram.dis             $(BUILD_DIR)/readI2s_sram.bin \
	$(BUILD_DIR)/malloc_simple_test_sram.elf  $(BUILD_DIR)/malloc_simple_test_sram.dis  $(BUILD_DIR)/malloc_simple_test_sram.bin \
	$(BUILD_DIR)/malloc_torture_test_sram.elf $(BUILD_DIR)/malloc_torture_test_sram.dis $(BUILD_DIR)/malloc_torture_test_sram.bin \
	$(BUILD_DIR)/memcpy_simple_test_sram.elf  $(BUILD_DIR)/memcpy_simple_test_sram.dis  $(BUILD_DIR)/memcpy_simple_test_sram.bin \
	$(BUILD_DIR)/dma_test_sram.elf            $(BUILD_DIR)/dma_test_sram.dis            $(BUILD_DIR)/dma_test_sram.bin
# SRAM model artifacts intentionally disabled:
# $(BUILD_DIR)/strided_s16_nodil_sram.elf $(BUILD_DIR)/strided_s16_nodil_sram.dis $(BUILD_DIR)/strided_s16_nodil_sram.bin
# $(BUILD_DIR)/streaming_model_sram.elf $(BUILD_DIR)/streaming_model_sram.dis $(BUILD_DIR)/streaming_model_sram.bin
# $(BUILD_DIR)/mel_compact_sram.elf $(BUILD_DIR)/mel_compact_sram.dis $(BUILD_DIR)/mel_compact_sram.bin

# *_xip: standalone workflow — `NO_JTAG=1 make sim FLASH=test/build/<test>_xip.bin`
XIP_TARGETS = \
	$(BUILD_DIR)/hello_world_xip.elf     $(BUILD_DIR)/hello_world_xip.dis     $(BUILD_DIR)/hello_world_xip.bin \
	$(BUILD_DIR)/readI2s_xip.elf         $(BUILD_DIR)/readI2s_xip.dis         $(BUILD_DIR)/readI2s_xip.bin \
	$(BUILD_DIR)/flash_read_test_xip.elf       $(BUILD_DIR)/flash_read_test_xip.dis       $(BUILD_DIR)/flash_read_test_xip.bin \
	$(BUILD_DIR)/snooper_test_xip.elf          $(BUILD_DIR)/snooper_test_xip.dis          $(BUILD_DIR)/snooper_test_xip.bin \
	$(BUILD_DIR)/tests_xip.elf                 $(BUILD_DIR)/tests_xip.dis                 $(BUILD_DIR)/tests_xip.bin \
	$(BUILD_DIR)/malloc_simple_test_xip.elf    $(BUILD_DIR)/malloc_simple_test_xip.dis    $(BUILD_DIR)/malloc_simple_test_xip.bin \
	$(BUILD_DIR)/malloc_torture_test_xip.elf   $(BUILD_DIR)/malloc_torture_test_xip.dis   $(BUILD_DIR)/malloc_torture_test_xip.bin \
	$(BUILD_DIR)/memcpy_simple_test_xip.elf    $(BUILD_DIR)/memcpy_simple_test_xip.dis    $(BUILD_DIR)/memcpy_simple_test_xip.bin \
	$(BUILD_DIR)/strided_s16_nodil_xip.elf     $(BUILD_DIR)/strided_s16_nodil_xip.dis     $(BUILD_DIR)/strided_s16_nodil_xip.bin \
	$(BUILD_DIR)/streaming_model_xip.elf       $(BUILD_DIR)/streaming_model_xip.dis       $(BUILD_DIR)/streaming_model_xip.bin \
	$(BUILD_DIR)/dma_test_xip.elf              $(BUILD_DIR)/dma_test_xip.dis              $(BUILD_DIR)/dma_test_xip.bin \
	$(BUILD_DIR)/mel_compact_4blk_ch36_xip.elf $(BUILD_DIR)/mel_compact_4blk_ch36_xip.dis $(BUILD_DIR)/mel_compact_4blk_ch36_xip.bin

ALL_TARGETS = $(SRAM_TARGETS) $(XIP_TARGETS)

all: $(BUILD_DIR) $(ALL_TARGETS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# --- ALIAS TARGETS ---
sram:   $(SRAM_TARGETS)
xip:    $(XIP_TARGETS)
sanity: $(BUILD_DIR)/inf_loop_sram.elf    $(BUILD_DIR)/inf_loop_sram.dis    $(BUILD_DIR)/inf_loop_sram.bin
uart:   $(BUILD_DIR)/hello_world_sram.elf $(BUILD_DIR)/hello_world_sram.dis $(BUILD_DIR)/hello_world_sram.bin \
        $(BUILD_DIR)/hello_world_xip.elf  $(BUILD_DIR)/hello_world_xip.dis  $(BUILD_DIR)/hello_world_xip.bin
i2s:    $(BUILD_DIR)/readI2s_sram.elf     $(BUILD_DIR)/readI2s_sram.dis     $(BUILD_DIR)/readI2s_sram.bin \
        $(BUILD_DIR)/readI2s_xip.elf      $(BUILD_DIR)/readI2s_xip.dis      $(BUILD_DIR)/readI2s_xip.bin
flash:  $(BUILD_DIR)/flash_read_test_xip.elf $(BUILD_DIR)/flash_read_test_xip.dis $(BUILD_DIR)/flash_read_test_xip.bin
malloc: $(BUILD_DIR)/malloc_simple_test_sram.elf  $(BUILD_DIR)/malloc_simple_test_sram.dis  $(BUILD_DIR)/malloc_simple_test_sram.bin \
	$(BUILD_DIR)/malloc_torture_test_sram.elf $(BUILD_DIR)/malloc_torture_test_sram.dis $(BUILD_DIR)/malloc_torture_test_sram.bin \
	$(BUILD_DIR)/malloc_simple_test_xip.elf   $(BUILD_DIR)/malloc_simple_test_xip.dis   $(BUILD_DIR)/malloc_simple_test_xip.bin \
	$(BUILD_DIR)/malloc_torture_test_xip.elf  $(BUILD_DIR)/malloc_torture_test_xip.dis  $(BUILD_DIR)/malloc_torture_test_xip.bin
malloc-simple: $(BUILD_DIR)/malloc_simple_test_sram.elf  $(BUILD_DIR)/malloc_simple_test_sram.dis  $(BUILD_DIR)/malloc_simple_test_sram.bin \
	       $(BUILD_DIR)/malloc_simple_test_xip.elf   $(BUILD_DIR)/malloc_simple_test_xip.dis   $(BUILD_DIR)/malloc_simple_test_xip.bin
malloc-torture: $(BUILD_DIR)/malloc_torture_test_sram.elf $(BUILD_DIR)/malloc_torture_test_sram.dis $(BUILD_DIR)/malloc_torture_test_sram.bin \
		$(BUILD_DIR)/malloc_torture_test_xip.elf  $(BUILD_DIR)/malloc_torture_test_xip.dis  $(BUILD_DIR)/malloc_torture_test_xip.bin
memcpy: $(BUILD_DIR)/memcpy_simple_test_sram.elf $(BUILD_DIR)/memcpy_simple_test_sram.dis $(BUILD_DIR)/memcpy_simple_test_sram.bin \
	$(BUILD_DIR)/memcpy_simple_test_xip.elf  $(BUILD_DIR)/memcpy_simple_test_xip.dis  $(BUILD_DIR)/memcpy_simple_test_xip.bin
model-strided-s16-nodil: $(BUILD_DIR)/strided_s16_nodil_xip.elf  $(BUILD_DIR)/strided_s16_nodil_xip.dis  $(BUILD_DIR)/strided_s16_nodil_xip.bin
model-streaming: $(BUILD_DIR)/streaming_model_xip.elf $(BUILD_DIR)/streaming_model_xip.dis $(BUILD_DIR)/streaming_model_xip.bin
model-mel-compact: $(BUILD_DIR)/mel_compact_4blk_ch36_xip.elf $(BUILD_DIR)/mel_compact_4blk_ch36_xip.dis $(BUILD_DIR)/mel_compact_4blk_ch36_xip.bin
# SRAM model artifacts intentionally disabled:
# $(BUILD_DIR)/strided_s16_nodil_sram.elf $(BUILD_DIR)/strided_s16_nodil_sram.dis $(BUILD_DIR)/strided_s16_nodil_sram.bin
dma:    $(BUILD_DIR)/dma_test_sram.elf    $(BUILD_DIR)/dma_test_sram.dis    $(BUILD_DIR)/dma_test_sram.bin \
        $(BUILD_DIR)/dma_test_xip.elf     $(BUILD_DIR)/dma_test_xip.dis     $(BUILD_DIR)/dma_test_xip.bin
xip-testbench:
	$(MAKE) -C $(XIP_DIR) run-testbench

# --- SRAM LINK RULES ---
$(BUILD_DIR)/inf_loop_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/inf_loop.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

$(BUILD_DIR)/hello_world_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/hello_world.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

$(BUILD_DIR)/bridge_bug_test_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/bridge_bug_test.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

$(BUILD_DIR)/snooper_test_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/snooper_test.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

$(BUILD_DIR)/readI2s_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/readI2s.o $(BUILD_DIR)/uart.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

$(BUILD_DIR)/malloc_simple_test_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/malloc_simple_test.o $(COMMON_DIR)/uart_utils.o $(BUILD_DIR)/malloc.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

$(BUILD_DIR)/malloc_torture_test_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/malloc_torture_test.o $(COMMON_DIR)/uart_utils.o $(BUILD_DIR)/malloc.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

$(BUILD_DIR)/memcpy_simple_test_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/memcpy_simple_test.o $(COMMON_DIR)/uart_utils.o $(BUILD_DIR)/memcpy.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

$(BUILD_DIR)/dma_test_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/dma_test.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/MS_DMAC_AHBL.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

# --- XIP LINK RULES ---
$(BUILD_DIR)/hello_world_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/hello_world.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

$(BUILD_DIR)/readI2s_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/readI2s.o $(BUILD_DIR)/uart.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

$(BUILD_DIR)/flash_read_test_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/flash_read_test.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

$(BUILD_DIR)/snooper_test_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/snooper_test.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

$(BUILD_DIR)/tests_xip.elf: $(BUILD_DIR)/tests.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

$(BUILD_DIR)/malloc_simple_test_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/malloc_simple_test.o $(COMMON_DIR)/uart_utils.o $(BUILD_DIR)/malloc.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

$(BUILD_DIR)/malloc_torture_test_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/malloc_torture_test.o $(COMMON_DIR)/uart_utils.o $(BUILD_DIR)/malloc.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

$(BUILD_DIR)/memcpy_simple_test_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/memcpy_simple_test.o $(COMMON_DIR)/uart_utils.o $(BUILD_DIR)/memcpy.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

$(BUILD_DIR)/strided_s16_nodil_xip.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(COMMON_DIR)/kws_bare_main.c \
    $(NNOM_SRCS_MODEL) | $(BUILD_DIR)
	$(CC) $(STRIDED_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_XIP)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/strided_s16_nodil_xip.bin
	$(CROSS_COMPILE)size $@

$(BUILD_DIR)/streaming_model_xip.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(STREAMING_MODEL_C_DIR)/kws_bare_stream.c \
    $(STREAMING_MODEL_C_DIR)/kws_stream_b.c \
    $(NNOM_SRCS_MODEL) | $(BUILD_DIR)
	$(CC) $(STREAMING_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_XIP)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/streaming_model_xip.bin
	$(CROSS_COMPILE)size $@

$(BUILD_DIR)/dma_test_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/dma_test.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/MS_DMAC_AHBL.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

# --- COMPILATION RULES ---
$(BUILD_DIR)/crt0.o: $(COMMON_DIR)/crt0.s $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/inf_loop.o: $(SANITY_C_DIR)/inf_loop.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/hello_world.o: $(UART_C_DIR)/hello_world.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/bridge_bug_test.o: $(UART_C_DIR)/bridge_bug_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/snooper_test.o: $(UART_C_DIR)/snooper_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/readI2s.o: $(I2S_C_DIR)/readI2s.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/uart.o: $(I2S_C_DIR)/uart.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/flash_read_test.o: $(XIP_C_DIR)/flash_read_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/tests.o: $(XIP_C_DIR)/tests.S $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/malloc_simple_test.o: $(MALLOC_C_DIR)/simple_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/malloc_torture_test.o: $(MALLOC_C_DIR)/torture_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/memcpy_simple_test.o: $(MEMCPY_C_DIR)/simple_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/nnom_%.o: $(NNOM_DIR)/src/%.c $(ROOT_MAKEFILE)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(NNOM_INC_DIR) -I$(NNOM_PORT_DIR) -o $@ $<

$(BUILD_DIR)/malloc.o: $(COMMON_DIR)/malloc.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/memcpy.o: $(COMMON_DIR)/memcpy.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/memset.o: $(COMMON_DIR)/memset.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/uart_utils.o: $(COMMON_DIR)/uart_utils.c $(ROOT_MAKEFILE)

$(BUILD_DIR)/dma_test.o: $(DMA_C_DIR)/dma_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/MS_DMAC_AHBL.o: $(DMAC_FW_DIR)/MS_DMAC_AHBL.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

# --- BINARY & DISASSEMBLY RULES ---
$(BUILD_DIR)/%.dis: $(BUILD_DIR)/%.elf
	$(OBJDUMP) -d $< > $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD_DIR) *.o *.elf *.dis
	rm -f $(COMMON_DIR)/*.d
	$(MAKE) -C $(XIP_DIR) clean

.PHONY: all clean sram xip sanity uart i2s flash malloc malloc-simple malloc-torture memcpy model-strided-s16-nodil model-streaming model-mel-compact xip-testbench

# Include dependency files
-include $(wildcard $(BUILD_DIR)/*.d)

MEL_DIR = mel_compact

# All KWS NNoM models share test/common/kws_bare_main.c.  Per-model knobs
# (weights header, model name, NNoM activation buffer size in KB) are passed
# via -D so the only thing changing between targets is the model itself —
# cycle counts compare apples-to-apples.

# Debug knobs (set on the make command line):
#   KWS_DEBUG_DUMP_FIRST_CLIP=1
#       Insert an `ebreak` in kws_bare_main.c after the first full ring_buf
#       capture; pair with scripts/dump_ring_buf.gdb.  ring_buf / ring_pos
#       / i2s_irq_count carry __attribute__((used)) so GDB sees them under
#       LTO too — no need to drop -flto.
#   USE_MCYCLE_CSR=1
#       Enable the in-firmware mcycle-CSR cycle counter and "CYCLES:"
#       UART line.  Off by default — VPI testbenches measure cycles more
#       accurately and without running extra firmware code.
KWS_DEBUG_DUMP_FIRST_CLIP ?= 0
USE_MCYCLE_CSR           ?= 0
KWS_EXTRA_CFLAGS         :=
ifeq ($(KWS_DEBUG_DUMP_FIRST_CLIP),1)
  KWS_EXTRA_CFLAGS += -DKWS_DEBUG_DUMP_FIRST_CLIP=1
endif
ifeq ($(USE_MCYCLE_CSR),1)
  KWS_EXTRA_CFLAGS += -DUSE_MCYCLE_CSR=1
endif
MEL_CFLAGS = $(MODEL_CFLAGS) $(KWS_EXTRA_CFLAGS) \
             -DNNOM_STATIC_BUF_KB=36 \
             -DKWS_WEIGHTS_HEADER='"mel_compact_4blk_ch36_weights.h"' \
             -DKWS_MODEL_NAME='"mel_compact_4blk_ch36"' \
             -I$(MEL_DIR)

STRIDED_CFLAGS = $(MODEL_CFLAGS) $(KWS_EXTRA_CFLAGS) \
                 -DNNOM_STATIC_BUF_KB=52 \
                 -DKWS_WEIGHTS_HEADER='"strided_s16_nodil_weights.h"' \
                 -DKWS_MODEL_NAME='"strided_s16_nodil"' \
                 -I$(MODEL_C_DIR)

STREAMING_CFLAGS = $(MODEL_CFLAGS) $(KWS_EXTRA_CFLAGS) \
                   -DNNOM_STATIC_BUF_KB=36 \
                   -I$(STREAMING_MODEL_C_DIR) -I$(MODEL_C_DIR)

$(BUILD_DIR)/mel_compact_4blk_ch36_xip.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(COMMON_DIR)/kws_bare_main.c \
    $(NNOM_SRCS_MODEL) | $(BUILD_DIR)
	$(CC) $(MEL_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_XIP)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/mel_compact_4blk_ch36_xip.bin
	$(CROSS_COMPILE)size $@

mel-compact: $(BUILD_DIR)/mel_compact_4blk_ch36_xip.elf
