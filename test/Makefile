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
MEL_COMPACT_INT8_C_DIR = mel_compact_int8
NNOM_DIR     = ../third_party/nnom
NNOM_INC_DIR = $(NNOM_DIR)/inc
NNOM_PORT_DIR = $(NNOM_DIR)/port

# Per upstream NNoM README ("Accessing C files"): add inc/ and port/ to the
# include path and compile every src/*.c file.  --gc-sections drops layer
# implementations the active model doesn't reach, so we don't pay for them
# at runtime.  This is the canonical integration; hand-listing only the
# layers the current model uses is fragile (silent breakage when a future
# model needs depthwise conv, lambda, lstm/gru cells, concat, etc.).
#
# nnom_lstm_cell.c and nnom_gru_cell.c contain raw printf() debug calls;
# they should be stripped by --gc-sections because our int8 KWS path never
# reaches them.  If a future model that uses LSTM/GRU is added, those
# printfs must be silenced (e.g. wrap them in NNOM_LOG or stub printf in
# nnom_port.h).
NNOM_SRCS_MODEL = $(wildcard $(NNOM_DIR)/src/core/*.c) \
                  $(wildcard $(NNOM_DIR)/src/layers/*.c) \
                  $(wildcard $(NNOM_DIR)/src/backends/*.c)

# Header include paths (relative to this Makefile's directory)
I2S_REGS_DIR  = ../peris/i2s/i2s_apb/i2s_itr2
UART_REGS_DIR = ../Hazard3/example_soc/libfpga/peris/uart
DMAC_FW_DIR   = ../peris/MS_DMAC_AHBL/fw

# Memory config
SRAM_DEPTH ?= 32768
SRAM_SIZE_BYTES = $(shell echo $$(($(SRAM_DEPTH) * 4)))

# I2S FIFO depth flows from the root Makefile.  Until task #12 (autonomous
# DMA drain) lands, the DMA burst MUST equal the I2S half-full threshold
# (= FIFO_DEPTH / 2) or ring_buf gets silently zero-padded — see task #13.
# Derive the burst from the same source so the two can't desync.
# Defined here (before CFLAGS) so the readI2s test and any other I2S-aware
# binary picks up the same value the model build uses.
I2S_FIFO_DEPTH      ?= 16
I2S_DMA_BURST_WORDS := $(shell expr $(I2S_FIFO_DEPTH) / 2)

# Compiler flags
# RV_ARCH and RV_ABI are exported by the root Makefile and derived from
# hazard3_config.vh — flip an EXTENSION_* there and both gcc -march and
# the Verilog parameter elaboration update together (TODO #6 + #13).
# Defaults allow invoking targets directly from test/ without the root Makefile.
RV_ARCH ?= rv32imac_zicsr_zifencei_zba_zbb
RV_ABI  ?= ilp32
CFLAGS  = -march=$(RV_ARCH) -mabi=$(RV_ABI) -g -c -MMD -MP
CFLAGS += $(GLOBAL_UART_CONFIG)
CFLAGS += -I$(I2S_REGS_DIR)
CFLAGS += -I$(UART_REGS_DIR)
CFLAGS += -I$(DMAC_FW_DIR)
CFLAGS += -I$(I2S_C_DIR)
CFLAGS += -DI2S_DMA_BURST_WORDS=$(I2S_DMA_BURST_WORDS)

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
MODEL_CFLAGS = -march=$(RV_ARCH) -mabi=$(RV_ABI) \
               -Os -flto -g -std=gnu99 \
               -ffunction-sections -fdata-sections \
               -falign-functions=4 \
               -DNNOM_USING_STATIC_MEMORY \
               -DNNOM_BARE_METAL \
               "-DNNOM_LOG(...)=" \
               $(GLOBAL_UART_CONFIG) \
               -DI2S_DMA_BURST_WORDS=$(I2S_DMA_BURST_WORDS) \
               -I$(COMMON_DIR) \
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
MODEL_LDFLAGS_COMMON = -march=$(RV_ARCH) -mabi=$(RV_ABI) \
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
	$(BUILD_DIR)/dma_test_xip.elf                    $(BUILD_DIR)/dma_test_xip.dis                    $(BUILD_DIR)/dma_test_xip.bin \
	$(BUILD_DIR)/mel_compact_4blk_ch36_xip.elf       $(BUILD_DIR)/mel_compact_4blk_ch36_xip.dis       $(BUILD_DIR)/mel_compact_4blk_ch36_xip.bin \
	$(BUILD_DIR)/mel_compact_4blk_ch36_xip_accel.elf $(BUILD_DIR)/mel_compact_4blk_ch36_xip_accel.dis $(BUILD_DIR)/mel_compact_4blk_ch36_xip_accel.bin \
	$(BUILD_DIR)/mel_compact_int8_xip_accel.elf $(BUILD_DIR)/mel_compact_int8_xip_accel.dis $(BUILD_DIR)/mel_compact_int8_xip_accel.bin \
	$(BUILD_DIR)/accel_test_xip.elf                  $(BUILD_DIR)/accel_test_xip.dis                  $(BUILD_DIR)/accel_test_xip.bin

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
model-mel-compact-accel: $(BUILD_DIR)/mel_compact_4blk_ch36_xip_accel.elf $(BUILD_DIR)/mel_compact_4blk_ch36_xip_accel.dis $(BUILD_DIR)/mel_compact_4blk_ch36_xip_accel.bin
model-mel-compact-int8-accel: $(BUILD_DIR)/mel_compact_int8_xip_accel.elf $(BUILD_DIR)/mel_compact_int8_xip_accel.dis $(BUILD_DIR)/mel_compact_int8_xip_accel.bin
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

$(BUILD_DIR)/accel_test_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/accel_test.o $(BUILD_DIR)/uart_utils.o $(BUILD_DIR)/memset.o
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
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/accel_test.o: accel/accel_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -I$(COMMON_DIR) -o $@ $<

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

.PHONY: all clean sram xip sanity uart i2s flash malloc malloc-simple malloc-torture memcpy model-strided-s16-nodil model-streaming model-mel-compact model-mel-compact-accel model-mel-compact-int8-accel xip-testbench mel-compact mel-compact-accel mel-compact-int8-accel

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

# ── mel_compact_4blk_ch36 — Conv1D hardware accelerator variant ──────────────
#
# Uses --wrap=conv2d_run to redirect NNoM's conv2d_run to nnom_conv1d_hw.c,
# which calls the KWS-SoC Conv1D hardware accelerator at APB 0x4000_C000.
# Same firmware entry point (kws_bare_main.c) as the SW baseline.
# Build:  make mel-compact-accel   or   make model-mel-compact-accel

# NNoM sources without nnom_conv2d.c (replaced by the HW shim below).
NNOM_SRCS_MEL_ACCEL = \
    $(NNOM_DIR)/src/core/nnom.c \
    $(NNOM_DIR)/src/core/nnom_layers.c \
    $(NNOM_DIR)/src/core/nnom_tensor.c \
    $(NNOM_DIR)/src/core/nnom_utils.c \
    $(NNOM_DIR)/src/layers/nnom_activation.c \
    $(NNOM_DIR)/src/layers/nnom_avgpool.c \
    $(NNOM_DIR)/src/layers/nnom_baselayer.c \
    $(NNOM_DIR)/src/layers/nnom_dense.c \
    $(NNOM_DIR)/src/layers/nnom_flatten.c \
    $(NNOM_DIR)/src/layers/nnom_global_pool.c \
    $(NNOM_DIR)/src/layers/nnom_input.c \
    $(NNOM_DIR)/src/layers/nnom_maxpool.c \
    $(NNOM_DIR)/src/layers/nnom_output.c \
    $(NNOM_DIR)/src/layers/nnom_reshape.c \
    $(NNOM_DIR)/src/layers/nnom_softmax.c \
    $(NNOM_DIR)/src/backends/nnom_local.c \
    $(NNOM_DIR)/src/backends/nnom_local_q15.c \
    $(MEL_DIR)/nnom_conv1d_hw.c

MEL_ACCEL_CFLAGS = $(MODEL_CFLAGS) $(KWS_EXTRA_CFLAGS) \
                   -DNNOM_STATIC_BUF_KB=36 \
                   -DKWS_WEIGHTS_HEADER='"mel_compact_4blk_ch36_weights.h"' \
                   -DKWS_MODEL_NAME='"mel_compact_4blk_ch36_accel"' \
                   -I$(MEL_DIR)

$(BUILD_DIR)/mel_compact_4blk_ch36_xip_accel.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(COMMON_DIR)/kws_bare_main.c \
    $(NNOM_SRCS_MEL_ACCEL) | $(BUILD_DIR)
	$(CC) $(MEL_ACCEL_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_XIP)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/mel_compact_4blk_ch36_xip_accel.bin
	$(CROSS_COMPILE)size $@

mel-compact-accel: $(BUILD_DIR)/mel_compact_4blk_ch36_xip_accel.elf

# ── mel_compact_int8 — Conv1D hardware accelerator variant (int8 model) ──────
#
# Same as mel-compact-accel but uses the int8 weights from mel_compact_int8/
# Build:  make mel-compact-int8-accel   or   make model-mel-compact-int8-accel

MEL_INT8_ACCEL_CFLAGS = $(MODEL_CFLAGS) $(KWS_EXTRA_CFLAGS) \
                   -DNNOM_STATIC_BUF_KB=36 \
                   -DKWS_WEIGHTS_HEADER='"weights_no_qat_float_new_noise.h"' \
                   -DKWS_MODEL_NAME='"mel_compact_int8_accel"' \
                   -I$(MEL_COMPACT_INT8_C_DIR)

$(BUILD_DIR)/mel_compact_int8_xip_accel.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(COMMON_DIR)/kws_bare_main.c \
    $(NNOM_SRCS_MEL_ACCEL) | $(BUILD_DIR)
	$(CC) $(MEL_INT8_ACCEL_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_XIP)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/mel_compact_int8_xip_accel.bin
	$(CROSS_COMPILE)size $@

mel-compact-int8-accel: $(BUILD_DIR)/mel_compact_int8_xip_accel.elf
model-mel-compact-int8-accel: $(BUILD_DIR)/mel_compact_int8_xip_accel.elf
