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
# Each NNoM model lives in its own subdir under test/model/.  Per the
# repo-reorg in TODO #7, the `test/{model,mel_compact,streaming_model}`
# scatter was consolidated to:
#
#   test/model/strided_s16_nodil/      strided_s16_nodil_weights.h
#   test/model/mel_compact_4blk_ch36/  mel_compact_4blk_ch36_weights.h + README
#   test/model/streaming/              kws_bare_stream.c + kws_stream_b.{c,h}
MODEL_DIR             = model
STRIDED_MODEL_C_DIR   = $(MODEL_DIR)/strided_s16_nodil
MEL_COMPACT_C_DIR     = $(MODEL_DIR)/mel_compact_4blk_ch36
MEL_INT8_C_DIR        = $(MODEL_DIR)/mel_compact_int8
MEL_INT8_PI_C_DIR     = $(MODEL_DIR)/mel_compact_int8_pi
MEL_INT8_PEAK_NORM_C_DIR = $(MODEL_DIR)/mel_compact_int8_peak_norm
MEL_INT8_PI_KLD_C_DIR = $(MODEL_DIR)/mel_compact_int8_pi_kld
MEL_4CMD_C_DIR        = $(MODEL_DIR)/mel_compact_4cmd
KWS_LR_C_DIR          = $(MODEL_DIR)/kws_lr
STREAMING_MODEL_C_DIR = $(MODEL_DIR)/streaming
ACCEL_C_DIR           = accel
NNOM_DIR     = ../nnom
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

# NNoM source set for the Conv1D-accelerated builds.  Two changes vs the
# baseline NNOM_SRCS_MODEL:
#   1. Drop nnom_conv2d.c so its conv2d_run symbol doesn't collide with
#      common/nnom_conv1d_hw.c's HW-shim conv2d_run; the linker then resolves
#      every call directly into the shim (no -Wl,--wrap needed — LTO can
#      inline through the call-site, which the wrap form blocks).
#   2. Trim the layer list to what the int8 KWS models actually traverse
#      (no LSTM/GRU/concat/dw/dilated/etc.).  --gc-sections would drop them
#      anyway, but keeping the source list minimal cuts compile time and
#      keeps any debug `printf` in the unused layers from being elaborated.
NNOM_SRCS_ACCEL = \
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
    $(COMMON_DIR)/nnom_conv1d_hw.c

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
CFLAGS  = -march=$(RV_ARCH) -mabi=$(RV_ABI) -g -c -MMD -MP
CFLAGS += $(GLOBAL_UART_CONFIG)
CFLAGS += -I$(I2S_REGS_DIR)
CFLAGS += -I$(UART_REGS_DIR)
CFLAGS += -I$(DMAC_FW_DIR)
CFLAGS += -I$(I2S_C_DIR)
CFLAGS += -DI2S_DMA_BURST_WORDS=$(I2S_DMA_BURST_WORDS)

# XIP audio playback flow (gated by root Makefile XIP_PLAYBACK=1).
# When enabled, scripts/hex_to_c_array.py emits test/build/playback_samples.c,
# which is compiled and linked into every XIP firmware so that a 1-second
# (or N-second when XIP_N_SAMPLES is set) audio buffer is placed in flash at
# 0x8001_0000 — read by peris/xip/xip_sample_player.v.
XIP_PLAYBACK        ?= 0
PLAYBACK_SAMPLES_C  ?=
ifeq ($(XIP_PLAYBACK),1)
  PLAYBACK_OBJ := $(BUILD_DIR)/playback_samples.o
else
  PLAYBACK_OBJ :=
endif

# Linker flags:
#   SRAM_LDFLAGS – all sections (text/rodata/data/bss) in SRAM at 0x00000000.
#                  Required for GDB `load`; string literals live at their SRAM VMA.
#   XIP_LDFLAGS  – text+rodata in FLASH (0x80000000), data+bss in SRAM.
#                  Required for standalone boot; string literals live at their flash VMA.
SRAM_LDFLAGS = -T $(COMMON_DIR)/sram_link.ld --defsym=SRAM_SIZE=$(SRAM_SIZE_BYTES)
XIP_LDFLAGS  = -T $(COMMON_DIR)/link.ld      --defsym=SRAM_SIZE=$(SRAM_SIZE_BYTES) $(PLAYBACK_OBJ)

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
MODEL_LDFLAGS_XIP    = $(MODEL_LDFLAGS_COMMON) -Wl,-T,$(COMMON_DIR)/link.ld     -Wl,--defsym=SRAM_SIZE=$(SRAM_SIZE_BYTES) $(PLAYBACK_OBJ)

# Accelerator builds use plain MODEL_LDFLAGS_XIP — symbol substitution for
# conv2d_run happens by simply omitting nnom_conv2d.c from NNOM_SRCS_ACCEL,
# which lets common/nnom_conv1d_hw.c's definition resolve naturally.
# (Earlier revisions used -Wl,--wrap=conv2d_run; that form blocks LTO from
# inlining through the call-site and added ~800K cycles of inference time.)

# --- TARGET LISTS ---
# *_sram: GDB workflow — `make gdb`, then `load build/<test>_sram.elf`, then `continue`
SRAM_TARGETS = \
	$(BUILD_DIR)/inf_loop_sram.elf            $(BUILD_DIR)/inf_loop_sram.dis            $(BUILD_DIR)/inf_loop_sram.bin \
	$(BUILD_DIR)/hello_world_sram.elf         $(BUILD_DIR)/hello_world_sram.dis         $(BUILD_DIR)/hello_world_sram.bin \
	$(BUILD_DIR)/bridge_bug_test_sram.elf     $(BUILD_DIR)/bridge_bug_test_sram.dis     $(BUILD_DIR)/bridge_bug_test_sram.bin \
	$(BUILD_DIR)/snooper_test_sram.elf        $(BUILD_DIR)/snooper_test_sram.dis        $(BUILD_DIR)/snooper_test_sram.bin \
	$(BUILD_DIR)/readI2s_sram.elf             $(BUILD_DIR)/readI2s_sram.dis             $(BUILD_DIR)/readI2s_sram.bin \
	$(BUILD_DIR)/recordI2s_sram.elf           $(BUILD_DIR)/recordI2s_sram.dis           $(BUILD_DIR)/recordI2s_sram.bin \
	$(BUILD_DIR)/dataflow_test_sram.elf       $(BUILD_DIR)/dataflow_test_sram.dis       $(BUILD_DIR)/dataflow_test_sram.bin \
	$(BUILD_DIR)/slide_test_sram.elf          $(BUILD_DIR)/slide_test_sram.dis          $(BUILD_DIR)/slide_test_sram.bin \
	$(BUILD_DIR)/malloc_simple_test_sram.elf  $(BUILD_DIR)/malloc_simple_test_sram.dis  $(BUILD_DIR)/malloc_simple_test_sram.bin \
	$(BUILD_DIR)/malloc_torture_test_sram.elf $(BUILD_DIR)/malloc_torture_test_sram.dis $(BUILD_DIR)/malloc_torture_test_sram.bin \
	$(BUILD_DIR)/memcpy_simple_test_sram.elf  $(BUILD_DIR)/memcpy_simple_test_sram.dis  $(BUILD_DIR)/memcpy_simple_test_sram.bin \
	$(BUILD_DIR)/dma_test_sram.elf            $(BUILD_DIR)/dma_test_sram.dis            $(BUILD_DIR)/dma_test_sram.bin \
	$(BUILD_DIR)/strided_s16_nodil_sram.elf   $(BUILD_DIR)/strided_s16_nodil_sram.dis   $(BUILD_DIR)/strided_s16_nodil_sram.bin \
	$(BUILD_DIR)/streaming_model_sram.elf     $(BUILD_DIR)/streaming_model_sram.dis     $(BUILD_DIR)/streaming_model_sram.bin \
	$(BUILD_DIR)/mel_compact_4blk_ch36_sram.elf $(BUILD_DIR)/mel_compact_4blk_ch36_sram.dis $(BUILD_DIR)/mel_compact_4blk_ch36_sram.bin
# Model SRAM artifacts: same source set as their _xip siblings, linked
# with sram_link.ld (everything in SRAM at 0x00000000).  Useful as a
# zero-XIP-latency baseline — the difference vs the _xip CYCLES_INFER
# is a clean lower bound on what cache / prefetch work can recover.
# Whether these fit at SRAM_DEPTH=$(SRAM_DEPTH) words depends on the
# model: mel_compact_4blk_ch36 (~28 KB text + ~53 KB bss) fits comfortably
# at the default 128 KB; larger weights may overflow — check `size` output.

# *_xip: standalone workflow — `NO_JTAG=1 make sim FLASH=test/build/<test>_xip.bin`
XIP_TARGETS = \
	$(BUILD_DIR)/hello_world_xip.elf     $(BUILD_DIR)/hello_world_xip.dis     $(BUILD_DIR)/hello_world_xip.bin \
	$(BUILD_DIR)/readI2s_xip.elf         $(BUILD_DIR)/readI2s_xip.dis         $(BUILD_DIR)/readI2s_xip.bin \
	$(BUILD_DIR)/recordI2s_xip.elf       $(BUILD_DIR)/recordI2s_xip.dis       $(BUILD_DIR)/recordI2s_xip.bin \
	$(BUILD_DIR)/dataflow_test_xip.elf   $(BUILD_DIR)/dataflow_test_xip.dis   $(BUILD_DIR)/dataflow_test_xip.bin \
	$(BUILD_DIR)/slide_test_xip.elf      $(BUILD_DIR)/slide_test_xip.dis      $(BUILD_DIR)/slide_test_xip.bin \
	$(BUILD_DIR)/flash_read_test_xip.elf       $(BUILD_DIR)/flash_read_test_xip.dis       $(BUILD_DIR)/flash_read_test_xip.bin \
	$(BUILD_DIR)/snooper_test_xip.elf          $(BUILD_DIR)/snooper_test_xip.dis          $(BUILD_DIR)/snooper_test_xip.bin \
	$(BUILD_DIR)/tests_xip.elf                 $(BUILD_DIR)/tests_xip.dis                 $(BUILD_DIR)/tests_xip.bin \
	$(BUILD_DIR)/malloc_simple_test_xip.elf    $(BUILD_DIR)/malloc_simple_test_xip.dis    $(BUILD_DIR)/malloc_simple_test_xip.bin \
	$(BUILD_DIR)/malloc_torture_test_xip.elf   $(BUILD_DIR)/malloc_torture_test_xip.dis   $(BUILD_DIR)/malloc_torture_test_xip.bin \
	$(BUILD_DIR)/memcpy_simple_test_xip.elf    $(BUILD_DIR)/memcpy_simple_test_xip.dis    $(BUILD_DIR)/memcpy_simple_test_xip.bin \
	$(BUILD_DIR)/strided_s16_nodil_xip.elf     $(BUILD_DIR)/strided_s16_nodil_xip.dis     $(BUILD_DIR)/strided_s16_nodil_xip.bin \
	$(BUILD_DIR)/streaming_model_xip.elf       $(BUILD_DIR)/streaming_model_xip.dis       $(BUILD_DIR)/streaming_model_xip.bin \
	$(BUILD_DIR)/dma_test_xip.elf              $(BUILD_DIR)/dma_test_xip.dis              $(BUILD_DIR)/dma_test_xip.bin \
	$(BUILD_DIR)/mel_compact_4blk_ch36_xip.elf       $(BUILD_DIR)/mel_compact_4blk_ch36_xip.dis       $(BUILD_DIR)/mel_compact_4blk_ch36_xip.bin \
	$(BUILD_DIR)/mel_compact_4blk_ch36_xip_accel.elf $(BUILD_DIR)/mel_compact_4blk_ch36_xip_accel.dis $(BUILD_DIR)/mel_compact_4blk_ch36_xip_accel.bin \
	$(BUILD_DIR)/mel_compact_int8_xip.elf            $(BUILD_DIR)/mel_compact_int8_xip.dis            $(BUILD_DIR)/mel_compact_int8_xip.bin \
	$(BUILD_DIR)/mel_compact_int8_xip_accel.elf      $(BUILD_DIR)/mel_compact_int8_xip_accel.dis      $(BUILD_DIR)/mel_compact_int8_xip_accel.bin \
	$(BUILD_DIR)/mel_compact_int8_pi_xip_accel.elf   $(BUILD_DIR)/mel_compact_int8_pi_xip_accel.dis   $(BUILD_DIR)/mel_compact_int8_pi_xip_accel.bin \
	$(BUILD_DIR)/mel_compact_int8_pi_kld_xip_accel.elf $(BUILD_DIR)/mel_compact_int8_pi_kld_xip_accel.dis $(BUILD_DIR)/mel_compact_int8_pi_kld_xip_accel.bin \
	$(BUILD_DIR)/mel_compact_int8_peak_norm_xip_accel.elf $(BUILD_DIR)/mel_compact_int8_peak_norm_xip_accel.dis $(BUILD_DIR)/mel_compact_int8_peak_norm_xip_accel.bin \
	$(BUILD_DIR)/kws_lr_xip.elf                      $(BUILD_DIR)/kws_lr_xip.dis                      $(BUILD_DIR)/kws_lr_xip.bin \
	$(BUILD_DIR)/kws_lr_xip_accel.elf                $(BUILD_DIR)/kws_lr_xip_accel.dis                $(BUILD_DIR)/kws_lr_xip_accel.bin \
	$(BUILD_DIR)/accel_test_xip.elf                  $(BUILD_DIR)/accel_test_xip.dis                  $(BUILD_DIR)/accel_test_xip.bin

ALL_TARGETS = $(SRAM_TARGETS) $(XIP_TARGETS)

all: $(BUILD_DIR) $(PLAYBACK_OBJ) $(ALL_TARGETS)

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
record-i2s: $(BUILD_DIR)/recordI2s_sram.elf $(BUILD_DIR)/recordI2s_sram.dis $(BUILD_DIR)/recordI2s_sram.bin \
            $(BUILD_DIR)/recordI2s_xip.elf  $(BUILD_DIR)/recordI2s_xip.dis  $(BUILD_DIR)/recordI2s_xip.bin
dataflow-test: $(BUILD_DIR)/dataflow_test_sram.elf $(BUILD_DIR)/dataflow_test_sram.dis $(BUILD_DIR)/dataflow_test_sram.bin \
               $(BUILD_DIR)/dataflow_test_xip.elf  $(BUILD_DIR)/dataflow_test_xip.dis  $(BUILD_DIR)/dataflow_test_xip.bin
slide-test: $(BUILD_DIR)/slide_test_sram.elf $(BUILD_DIR)/slide_test_sram.dis $(BUILD_DIR)/slide_test_sram.bin \
            $(BUILD_DIR)/slide_test_xip.elf  $(BUILD_DIR)/slide_test_xip.dis  $(BUILD_DIR)/slide_test_xip.bin
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
model-strided-s16-nodil: \
	$(BUILD_DIR)/strided_s16_nodil_xip.elf  $(BUILD_DIR)/strided_s16_nodil_xip.dis  $(BUILD_DIR)/strided_s16_nodil_xip.bin \
	$(BUILD_DIR)/strided_s16_nodil_sram.elf $(BUILD_DIR)/strided_s16_nodil_sram.dis $(BUILD_DIR)/strided_s16_nodil_sram.bin
model-streaming: \
	$(BUILD_DIR)/streaming_model_xip.elf  $(BUILD_DIR)/streaming_model_xip.dis  $(BUILD_DIR)/streaming_model_xip.bin \
	$(BUILD_DIR)/streaming_model_sram.elf $(BUILD_DIR)/streaming_model_sram.dis $(BUILD_DIR)/streaming_model_sram.bin
model-mel-compact: \
	$(BUILD_DIR)/mel_compact_4blk_ch36_xip.elf  $(BUILD_DIR)/mel_compact_4blk_ch36_xip.dis  $(BUILD_DIR)/mel_compact_4blk_ch36_xip.bin \
	$(BUILD_DIR)/mel_compact_4blk_ch36_sram.elf $(BUILD_DIR)/mel_compact_4blk_ch36_sram.dis $(BUILD_DIR)/mel_compact_4blk_ch36_sram.bin
model-mel-compact-accel: \
	$(BUILD_DIR)/mel_compact_4blk_ch36_xip_accel.elf $(BUILD_DIR)/mel_compact_4blk_ch36_xip_accel.dis $(BUILD_DIR)/mel_compact_4blk_ch36_xip_accel.bin
model-mel-compact-int8: \
	$(BUILD_DIR)/mel_compact_int8_xip.elf       $(BUILD_DIR)/mel_compact_int8_xip.dis       $(BUILD_DIR)/mel_compact_int8_xip.bin
model-mel-compact-int8-accel: \
	$(BUILD_DIR)/mel_compact_int8_xip_accel.elf $(BUILD_DIR)/mel_compact_int8_xip_accel.dis $(BUILD_DIR)/mel_compact_int8_xip_accel.bin
model-mel-compact-int8-pi-accel: \
	$(BUILD_DIR)/mel_compact_int8_pi_xip_accel.elf $(BUILD_DIR)/mel_compact_int8_pi_xip_accel.dis $(BUILD_DIR)/mel_compact_int8_pi_xip_accel.bin
model-mel-compact-int8-pi-kld-accel: \
	$(BUILD_DIR)/mel_compact_int8_pi_kld_xip_accel.elf $(BUILD_DIR)/mel_compact_int8_pi_kld_xip_accel.dis $(BUILD_DIR)/mel_compact_int8_pi_kld_xip_accel.bin
model-mel-compact-int8-peak-norm-accel: \
	$(BUILD_DIR)/mel_compact_int8_peak_norm_xip_accel.elf $(BUILD_DIR)/mel_compact_int8_peak_norm_xip_accel.dis $(BUILD_DIR)/mel_compact_int8_peak_norm_xip_accel.bin
model-mel-compact-int8-peak-norm-dump: \
	$(BUILD_DIR)/mel_compact_int8_peak_norm_dump_xip_accel.elf $(BUILD_DIR)/mel_compact_int8_peak_norm_dump_xip_accel.dis $(BUILD_DIR)/mel_compact_int8_peak_norm_dump_xip_accel.bin
model-mel-compact-int8-peak-norm-dump-full: \
	$(BUILD_DIR)/mel_compact_int8_peak_norm_dump_full_xip_accel.elf $(BUILD_DIR)/mel_compact_int8_peak_norm_dump_full_xip_accel.dis $(BUILD_DIR)/mel_compact_int8_peak_norm_dump_full_xip_accel.bin
model-kws-lr: \
	$(BUILD_DIR)/kws_lr_xip.elf                 $(BUILD_DIR)/kws_lr_xip.dis                 $(BUILD_DIR)/kws_lr_xip.bin
model-kws-lr-accel: \
	$(BUILD_DIR)/kws_lr_xip_accel.elf           $(BUILD_DIR)/kws_lr_xip_accel.dis           $(BUILD_DIR)/kws_lr_xip_accel.bin
accel-test: \
	$(BUILD_DIR)/accel_test_xip.elf             $(BUILD_DIR)/accel_test_xip.dis             $(BUILD_DIR)/accel_test_xip.bin
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

$(BUILD_DIR)/recordI2s_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/recordI2s.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/MS_DMAC_AHBL.o
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

$(BUILD_DIR)/recordI2s_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/recordI2s.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/MS_DMAC_AHBL.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

$(BUILD_DIR)/recordI2s.o: $(I2S_C_DIR)/recordI2s.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<

# ── Dataflow self-test (mic → I2S → DMA → snap[]) ────────────────────────────
# Drives the counter stimulus from sim/counter_16k.hex through the same
# capture path kws_bare_main.c uses, then byte-checks the snapshot against
# the expected pattern.  Prints DATAFLOW_PASS or DATAFLOW_FAIL i=N
# got=0xXX exp=0xYY on the first mismatch — pin-points which chain link
# (mic, RX core, WIDTH=8 byte select, FIFO pack, DMA, ÷2 snapshot) failed.
$(BUILD_DIR)/dataflow_test.o: $(I2S_C_DIR)/dataflow_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) $(if $(I2S_CLK_DIV),-DI2S_CLK_DIV=$(I2S_CLK_DIV),) -o $@ $<

$(BUILD_DIR)/dataflow_test_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/dataflow_test.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/MS_DMAC_AHBL.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

$(BUILD_DIR)/dataflow_test_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/dataflow_test.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/MS_DMAC_AHBL.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

# ── Sliding-window self-test ────────────────────────────────────────────────
# Drives sim/counter_32k.hex through the full capture path, runs N
# simulated inferences with a configurable cycle budget (default ≈ accel),
# and verifies every snapshot's first/last byte matches the expected
# counter pattern after the dynamic step.  Pin-points whether sliding
# preserves data integrity across all iterations.
SIM_INFERENCE_CYCLES ?=
SLIDE_ITERATIONS     ?=

SLIDE_TEST_CFLAGS = $(if $(I2S_CLK_DIV),-DI2S_CLK_DIV=$(I2S_CLK_DIV),) \
                    $(if $(SIM_INFERENCE_CYCLES),-DSIM_INFERENCE_CYCLES=$(SIM_INFERENCE_CYCLES),) \
                    $(if $(SLIDE_ITERATIONS),-DSLIDE_ITERATIONS=$(SLIDE_ITERATIONS),)

$(BUILD_DIR)/slide_test.o: $(I2S_C_DIR)/slide_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) $(SLIDE_TEST_CFLAGS) -o $@ $<

$(BUILD_DIR)/slide_test_sram.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/slide_test.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/MS_DMAC_AHBL.o
	$(LD) $(SRAM_LDFLAGS) -o $@ $^

$(BUILD_DIR)/slide_test_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/slide_test.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/MS_DMAC_AHBL.o
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

$(BUILD_DIR)/strided_s16_nodil_sram.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(COMMON_DIR)/kws_bare_main.c \
    $(NNOM_SRCS_MODEL) | $(BUILD_DIR)
	$(CC) $(STRIDED_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_SRAM)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/strided_s16_nodil_sram.bin
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

$(BUILD_DIR)/streaming_model_sram.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(STREAMING_MODEL_C_DIR)/kws_bare_stream.c \
    $(STREAMING_MODEL_C_DIR)/kws_stream_b.c \
    $(NNOM_SRCS_MODEL) | $(BUILD_DIR)
	$(CC) $(STREAMING_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_SRAM)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/streaming_model_sram.bin
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

.PHONY: all clean sram xip sanity uart i2s record-i2s dataflow-test slide-test flash malloc malloc-simple malloc-torture memcpy \
        model-strided-s16-nodil model-streaming model-mel-compact model-mel-compact-accel \
        model-mel-compact-int8 model-mel-compact-int8-accel \
        model-mel-compact-int8-pi-accel \
        model-mel-compact-int8-pi-kld-accel \
        model-mel-compact-int8-peak-norm-accel \
        model-mel-compact-int8-peak-norm-dump \
        model-mel-compact-int8-peak-norm-dump-full \
        mel-compact-int8-pi-accel mel-compact-int8-pi-kld-accel \
        mel-compact-int8-peak-norm-accel \
        mel-compact-int8-peak-norm-dump mel-compact-int8-peak-norm-dump-full \
        model-kws-lr model-kws-lr-accel accel-test xip-testbench

# Include dependency files
-include $(wildcard $(BUILD_DIR)/*.d)

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
# USE_MCYCLE_CSR=1 by default — Hazard3 inhibits mcycle out of reset, so any
# code that does `csrr mcycle` (notably the dynamic sliding-window step in
# kws_bare_main.c) returns a frozen value with the counter inhibited.  The
# firmware now uses mcycle for correctness-critical timing, not just for
# debug prints, so the safe default is ON.  Set USE_MCYCLE_CSR=0 explicitly
# only for cycle-counter-free measurement runs.
USE_MCYCLE_CSR           ?= 1
KWS_EXTRA_CFLAGS         :=
ifeq ($(KWS_DEBUG_DUMP_FIRST_CLIP),1)
  KWS_EXTRA_CFLAGS += -DKWS_DEBUG_DUMP_FIRST_CLIP=1
endif
ifeq ($(USE_MCYCLE_CSR),1)
  KWS_EXTRA_CFLAGS += -DUSE_MCYCLE_CSR=1
endif
ifneq ($(KWS_DEBOUNCE_COUNT),)
  KWS_EXTRA_CFLAGS += -DKWS_DEBOUNCE_COUNT=$(KWS_DEBOUNCE_COUNT)
endif
# AGC is OPT-IN here (polarity reversed from upstream NDMCR_AWR_DEV which made
# it default-on via KWS_AGC_DISABLE).  Pass KWS_AGC_ENABLE=1 to turn on the
# rolling peak-normalization path; KWS_AGC_RELEASE_SHIFT and KWS_AGC_FLOOR
# tune its envelope.
ifeq ($(KWS_AGC_ENABLE),1)
  KWS_EXTRA_CFLAGS += -DKWS_AGC_ENABLE=1
endif
ifneq ($(KWS_AGC_RELEASE_SHIFT),)
  KWS_EXTRA_CFLAGS += -DKWS_AGC_RELEASE_SHIFT=$(KWS_AGC_RELEASE_SHIFT)
endif
ifneq ($(KWS_AGC_FLOOR),)
  KWS_EXTRA_CFLAGS += -DKWS_AGC_FLOOR=$(KWS_AGC_FLOOR)
endif
# Suppress per-clip output when the post-debounce label is "unknown".
# Useful for long captures of mostly silence — the UART only prints when
# the model actually detects a keyword.
ifeq ($(KWS_QUIET),1)
  KWS_EXTRA_CFLAGS += -DKWS_QUIET=1
endif
# Disable sliding-window overlap: each inference covers a disjoint 1-second
# slice of audio (step_bytes pinned to RING_SAMPLES_PER_CLIP).  Default is
# the dynamic overlapping window.
ifeq ($(KWS_NO_OVERLAP),1)
  KWS_EXTRA_CFLAGS += -DKWS_NO_OVERLAP=1
endif
# Firmware-side upsampling: compensate the mic's real rate (15.625 kHz at
# cfg_div=17 on 36 MHz) up to the model's training rate (KWS_MODEL_HZ).
# Default off.  KWS_RING_HZ_REAL overrides the assumed real rate.
ifeq ($(KWS_UPSAMPLE),1)
  KWS_EXTRA_CFLAGS += -DKWS_UPSAMPLE=1
endif
ifneq ($(KWS_RING_HZ_REAL),)
  KWS_EXTRA_CFLAGS += -DKWS_RING_HZ_REAL=$(KWS_RING_HZ_REAL)
endif
# NNoM-aware XIP-cache prefetch hint: opt-in.  When 1, the accel's
# nnom_conv1d_hw shim writes PREFETCH_BASE/LEN/CTRL on every layer
# call so the cache pre-warms the weight-scan starting line.  RTL
# stays gated behind the same APB enable bit, so KWS_XIP_PREFETCH=0
# (default) is byte-for-byte identical to a no-prefetch build.
ifeq ($(KWS_XIP_PREFETCH),1)
  KWS_EXTRA_CFLAGS += -DKWS_XIP_PREFETCH=1
endif
MEL_CFLAGS = $(MODEL_CFLAGS) $(KWS_EXTRA_CFLAGS) \
             -DNNOM_STATIC_BUF_KB=36 \
             -DKWS_WEIGHTS_HEADER='"mel_compact_4blk_ch36_weights.h"' \
             -DKWS_MODEL_NAME='"mel_compact_4blk_ch36"' \
             -I$(MEL_COMPACT_C_DIR)

STRIDED_CFLAGS = $(MODEL_CFLAGS) $(KWS_EXTRA_CFLAGS) \
                 -DNNOM_STATIC_BUF_KB=52 \
                 -DKWS_WEIGHTS_HEADER='"strided_s16_nodil_weights.h"' \
                 -DKWS_MODEL_NAME='"strided_s16_nodil"' \
                 -I$(STRIDED_MODEL_C_DIR)

STREAMING_CFLAGS = $(MODEL_CFLAGS) $(KWS_EXTRA_CFLAGS) \
                   -DNNOM_STATIC_BUF_KB=36 \
                   -I$(STREAMING_MODEL_C_DIR) -I$(STRIDED_MODEL_C_DIR)

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

$(BUILD_DIR)/mel_compact_4blk_ch36_sram.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(COMMON_DIR)/kws_bare_main.c \
    $(NNOM_SRCS_MODEL) | $(BUILD_DIR)
	$(CC) $(MEL_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_SRAM)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/mel_compact_4blk_ch36_sram.bin
	$(CROSS_COMPILE)size $@

mel-compact: \
	$(BUILD_DIR)/mel_compact_4blk_ch36_xip.elf \
	$(BUILD_DIR)/mel_compact_4blk_ch36_sram.elf

# ── Conv1D-accelerated mel_compact (HW shim via --wrap=conv2d_run) ───────────
$(BUILD_DIR)/mel_compact_4blk_ch36_xip_accel.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(COMMON_DIR)/kws_bare_main.c \
    $(NNOM_SRCS_ACCEL) | $(BUILD_DIR)
	$(CC) $(MEL_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_XIP)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/mel_compact_4blk_ch36_xip_accel.bin
	$(CROSS_COMPILE)size $@

# ── mel_compact_int8 (SW + accel variants) ──────────────────────────────────
MEL_INT8_CFLAGS = $(MODEL_CFLAGS) $(KWS_EXTRA_CFLAGS) \
                  -DNNOM_STATIC_BUF_KB=36 \
                  -DKWS_WEIGHTS_HEADER='"mel_compact_int8_weights.h"' \
                  -DKWS_MODEL_NAME='"mel_compact_int8"' \
                  -I$(MEL_INT8_C_DIR)

$(BUILD_DIR)/mel_compact_int8_xip.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(COMMON_DIR)/kws_bare_main.c \
    $(NNOM_SRCS_MODEL) | $(BUILD_DIR)
	$(CC) $(MEL_INT8_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_XIP)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/mel_compact_int8_xip.bin
	$(CROSS_COMPILE)size $@

$(BUILD_DIR)/mel_compact_int8_xip_accel.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(COMMON_DIR)/kws_bare_main.c \
    $(NNOM_SRCS_ACCEL) | $(BUILD_DIR)
	$(CC) $(MEL_INT8_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_XIP)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/mel_compact_int8_xip_accel.bin
	$(CROSS_COMPILE)size $@

mel-compact-int8: $(BUILD_DIR)/mel_compact_int8_xip.elf
mel-compact-int8-accel: $(BUILD_DIR)/mel_compact_int8_xip_accel.elf

# ── mel_compact_int8_pi — int8 model fine-tuned on Barbary mic data (Pi pipeline)
#    Same audio path as mel_compact_int8 (8 kHz model, 16 kHz mic + firmware ÷2).
MEL_INT8_PI_CFLAGS = $(MODEL_CFLAGS) $(KWS_EXTRA_CFLAGS) \
                     -DNNOM_STATIC_BUF_KB=36 \
                     -DKWS_WEIGHTS_HEADER='"mel_compact_int8_pi.h"' \
                     -DKWS_MODEL_NAME='"mel_compact_int8_pi"' \
                     -I$(MEL_INT8_PI_C_DIR)

$(BUILD_DIR)/mel_compact_int8_pi_xip_accel.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(COMMON_DIR)/kws_bare_main.c \
    $(NNOM_SRCS_ACCEL) | $(BUILD_DIR)
	$(CC) $(MEL_INT8_PI_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_XIP)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/mel_compact_int8_pi_xip_accel.bin
	$(CROSS_COMPILE)size $@

mel-compact-int8-pi-accel: $(BUILD_DIR)/mel_compact_int8_pi_xip_accel.elf

# ── mel_compact_int8_pi_kld — Barbary int8 model, pi-only KLD calibration ───
#    Same audio path as mel_compact_int8_pi (8 kHz model, 16 kHz mic + ÷2).
MEL_INT8_PI_KLD_CFLAGS = $(MODEL_CFLAGS) $(KWS_EXTRA_CFLAGS) \
                     -DNNOM_STATIC_BUF_KB=36 \
                     -DKWS_WEIGHTS_HEADER='"mel_compact_int8_pi_kld_weights.h"' \
                     -DKWS_MODEL_NAME='"mel_compact_int8_pi_kld"' \
                     -I$(MEL_INT8_PI_KLD_C_DIR)

$(BUILD_DIR)/mel_compact_int8_pi_kld_xip_accel.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(COMMON_DIR)/kws_bare_main.c \
    $(NNOM_SRCS_ACCEL) | $(BUILD_DIR)
	$(CC) $(MEL_INT8_PI_KLD_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_XIP)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/mel_compact_int8_pi_kld_xip_accel.bin
	$(CROSS_COMPILE)size $@

mel-compact-int8-pi-kld-accel: $(BUILD_DIR)/mel_compact_int8_pi_kld_xip_accel.elf

# ── mel_compact_4cmd — 4-command model (go/left/right/stop) ─────────────────
#    Same audio path as mel_compact_int8_pi_kld (8 kHz, peak-norm pipeline).
#    NUM_CLASSES=5 and class names are embedded in the weights header.
MEL_4CMD_CFLAGS = $(MODEL_CFLAGS) $(KWS_EXTRA_CFLAGS) \
                  -DNNOM_STATIC_BUF_KB=36 \
                  -DKWS_WEIGHTS_HEADER='"weights_4cmd.h"' \
                  -DKWS_MODEL_NAME='"mel_compact_4cmd"' \
                  -I$(MEL_4CMD_C_DIR)

$(BUILD_DIR)/mel_compact_4cmd_xip_accel.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(COMMON_DIR)/kws_bare_main.c \
    $(NNOM_SRCS_ACCEL) | $(BUILD_DIR)
	$(CC) $(MEL_4CMD_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_XIP)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/mel_compact_4cmd_xip_accel.bin
	$(CROSS_COMPILE)size $@

mel-compact-4cmd-accel: $(BUILD_DIR)/mel_compact_4cmd_xip_accel.elf

# ── mel_compact_int8_peak_norm — int8 model + rolling peak-normalisation ────
#
# Fine-tuned on Barbary (INMP441) data using the int8 peak-norm pipeline.
# Like mel-compact-int8-accel BUT: KWS_DS_EN=0 (no HW downsample; the mic
# already runs at 16 kHz raw → 16 kHz ring) and the firmware ÷2 in
# kws_bare_main.c brings it to the model's 8 kHz expectation.  Firmware
# AGC is opt-in via KWS_AGC_ENABLE=1 (the upstream "default-on" polarity is
# inverted here per merge decision).
# Build:  make mel-compact-int8-peak-norm-accel
MEL_INT8_PEAK_NORM_ACCEL_CFLAGS = $(MODEL_CFLAGS) $(KWS_EXTRA_CFLAGS) \
                   -DNNOM_STATIC_BUF_KB=36 \
                   -DKWS_WEIGHTS_HEADER='"mel_compact_int8_peak_norm_weights.h"' \
                   -DKWS_MODEL_NAME='"mel_compact_int8_peak_norm_accel"' \
                   -DKWS_DS_EN=0 \
                   -DKWS_MODEL_HZ=8000 \
                   -I$(MEL_INT8_PEAK_NORM_C_DIR)

$(BUILD_DIR)/mel_compact_int8_peak_norm_xip_accel.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(COMMON_DIR)/kws_bare_main.c \
    $(NNOM_SRCS_ACCEL) | $(BUILD_DIR)
	$(CC) $(MEL_INT8_PEAK_NORM_ACCEL_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_XIP)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/mel_compact_int8_peak_norm_xip_accel.bin
	$(CROSS_COMPILE)size $@

mel-compact-int8-peak-norm-accel: $(BUILD_DIR)/mel_compact_int8_peak_norm_xip_accel.elf

# ── mel_compact_int8_peak_norm — Spike-reproduction layer-dump variant ─────
#
# Same model + accelerator path as -peak-norm-accel, but replaces
# kws_bare_main.c with kws_spike_repro.c, which embeds a single 8 kHz int8
# clip from Spike (via spike_clip_data.c) and dumps every layer's output
# bytes as `printf("%02x\n", ...)` lines.  Diff its UART output against
# test/spikedebug/activations.log to isolate where the SoC diverges from
# Spike's pure-SW reference.
SPIKE_DEBUG_DIR  := spikedebug
SPIKE_AUDIO_BIN  := $(SPIKE_DEBUG_DIR)/down_audio_8k_spike.bin
SPIKE_CLIP_C     := $(SPIKE_DEBUG_DIR)/spike_clip_data.c
SPIKE_REPRO_C    := $(SPIKE_DEBUG_DIR)/kws_spike_repro.c

# Generated from the audio binary; kept in-tree if already up to date so we
# don't force python on every build, but we list the recipe for completeness.
$(SPIKE_CLIP_C): $(SPIKE_AUDIO_BIN) ../scripts/spike_bin_to_c_array.py
	python3 ../scripts/spike_bin_to_c_array.py $(SPIKE_AUDIO_BIN) $@

MEL_INT8_PEAK_NORM_DUMP_CFLAGS = $(MODEL_CFLAGS) $(KWS_EXTRA_CFLAGS) \
                   -DNNOM_STATIC_BUF_KB=36 \
                   -DKWS_WEIGHTS_HEADER='"mel_compact_int8_peak_norm_weights.h"' \
                   -DKWS_MODEL_NAME='"mel_compact_int8_peak_norm_dump"' \
                   -I$(MEL_INT8_PEAK_NORM_C_DIR)

$(BUILD_DIR)/mel_compact_int8_peak_norm_dump_xip_accel.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(SPIKE_REPRO_C) \
    $(SPIKE_CLIP_C) \
    $(NNOM_SRCS_ACCEL) | $(BUILD_DIR)
	$(CC) $(MEL_INT8_PEAK_NORM_DUMP_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_XIP)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/mel_compact_int8_peak_norm_dump_xip_accel.bin
	$(CROSS_COMPILE)size $@

mel-compact-int8-peak-norm-dump: $(BUILD_DIR)/mel_compact_int8_peak_norm_dump_xip_accel.elf

# ── mel_compact_int8_peak_norm — full-pipeline single-shot dump variant ────
#
# Production audio path (kws_bare_main.c) under KWS_DUMP_LAYERS=1 — captures
# one ring's worth of audio via I2S/DMA, snapshots into nnom_input_data,
# runs model_run once with a layer-dump callback registered (LF-only
# "%02x" bytes — same wire format as test/spikedebug/activations.log),
# prints PRED, halts.  KWS_AGC_ENABLE forced 0 so the firmware doesn't
# warp the input distribution before we compare against Spike's pre-
# normalised clip.
MEL_INT8_PEAK_NORM_DUMP_FULL_CFLAGS = $(MODEL_CFLAGS) $(KWS_EXTRA_CFLAGS) \
                   -DNNOM_STATIC_BUF_KB=36 \
                   -DKWS_WEIGHTS_HEADER='"mel_compact_int8_peak_norm_weights.h"' \
                   -DKWS_MODEL_NAME='"mel_compact_int8_peak_norm_dump_full"' \
                   -DKWS_DS_EN=0 \
                   -DKWS_MODEL_HZ=8000 \
                   -DKWS_DUMP_LAYERS=1 \
                   -DKWS_AGC_ENABLE=0 \
                   -I$(MEL_INT8_PEAK_NORM_C_DIR)

$(BUILD_DIR)/mel_compact_int8_peak_norm_dump_full_xip_accel.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(COMMON_DIR)/kws_bare_main.c \
    $(NNOM_SRCS_ACCEL) | $(BUILD_DIR)
	$(CC) $(MEL_INT8_PEAK_NORM_DUMP_FULL_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_XIP)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/mel_compact_int8_peak_norm_dump_full_xip_accel.bin
	$(CROSS_COMPILE)size $@

mel-compact-int8-peak-norm-dump-full: $(BUILD_DIR)/mel_compact_int8_peak_norm_dump_full_xip_accel.elf

# Mic-BFM hex generated from down_audio_16k.hex for the full-pipeline test
# (each int16 sample placed at mic[30:15], triplicated for HW ÷3).
SPIKE_AUDIO_16K_HEX  := $(SPIKE_DEBUG_DIR)/down_audio_16k.hex
SPIKE_AUDIO_MIC_HEX  := $(SPIKE_DEBUG_DIR)/down_audio_16k_mic.hex

$(SPIKE_AUDIO_MIC_HEX): $(SPIKE_AUDIO_16K_HEX) ../scripts/spike16k_to_mic_hex.py
	python3 ../scripts/spike16k_to_mic_hex.py $(SPIKE_AUDIO_16K_HEX) $@

# ── kws_lr (SW + accel variants) ────────────────────────────────────────────
KWS_LR_CFLAGS = $(MODEL_CFLAGS) $(KWS_EXTRA_CFLAGS) \
                -DNNOM_STATIC_BUF_KB=36 \
                -DKWS_WEIGHTS_HEADER='"lr_weights.h"' \
                -DKWS_MODEL_NAME='"kws_lr"' \
                -I$(KWS_LR_C_DIR)

$(BUILD_DIR)/kws_lr_xip.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(COMMON_DIR)/kws_bare_main.c \
    $(NNOM_SRCS_MODEL) | $(BUILD_DIR)
	$(CC) $(KWS_LR_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_XIP)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/kws_lr_xip.bin
	$(CROSS_COMPILE)size $@

$(BUILD_DIR)/kws_lr_xip_accel.elf: \
    $(COMMON_DIR)/crt0.s \
    $(COMMON_DIR)/memcpy.c \
    $(COMMON_DIR)/memset.c \
    $(COMMON_DIR)/intrinsics.c \
    $(COMMON_DIR)/kws_bare_main.c \
    $(NNOM_SRCS_ACCEL) | $(BUILD_DIR)
	$(CC) $(KWS_LR_CFLAGS) -o $@ $^ $(MODEL_LDFLAGS_XIP)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/kws_lr_xip_accel.bin
	$(CROSS_COMPILE)size $@

# ── Standalone Conv1D accelerator unit test ─────────────────────────────────
# Pure SRAM test (no I2S, no NNoM): exercises 3 hand-written conv1d cases
# directly through the APB driver in test/common/accel_conv1d.h.  TC1/TC2
# fail with HSIZE_WORD on stride×C_in non-aligned to 4 — known accelerator
# limitation, kept as a documented regression marker (see test/accel/README.md).
$(BUILD_DIR)/accel_test.o: $(ACCEL_C_DIR)/accel_test.c $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -I$(COMMON_DIR) -o $@ $<

$(BUILD_DIR)/accel_test_xip.elf: $(BUILD_DIR)/crt0.o $(BUILD_DIR)/accel_test.o $(COMMON_DIR)/uart_utils.o $(BUILD_DIR)/memset.o
	$(LD) $(XIP_LDFLAGS) -o $@ $^

# ── XIP audio playback firmware glue (only built when XIP_PLAYBACK=1) ────────
# scripts/hex_to_c_array.py emits PLAYBACK_SAMPLES_C; root Makefile wires it.
$(BUILD_DIR)/playback_samples.o: $(PLAYBACK_SAMPLES_C) $(ROOT_MAKEFILE)
	$(CC) $(CFLAGS) -o $@ $<
