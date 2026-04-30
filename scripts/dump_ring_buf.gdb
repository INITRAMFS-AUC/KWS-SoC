# GDB helper: dump kws_bare_main.c's ring_buf in the sim/*.hex audio format.
#
# Pairs with the KWS_DEBUG_DUMP_FIRST_CLIP build option in kws_bare_main.c,
# which inserts an `ebreak` after the FIRST full ring_buf is captured and
# before memcpy / inference.  At that point ring_buf[0..7999] holds the Q7
# samples that the DMA + ISR delivered for the model to consume.
#
# This script re-emits each Q7 byte in the same 32-bit hex form that
# i2s_mic_sim feeds in:
#
#     fifo_word = ((int16_t)q7) << 16          # sign-extended high 16 bits
#
# so the dumped file can be `diff`'d directly against the input hex
# (e.g. sim/down_0000.hex) and any sample-level corruption shows up
# immediately.
#
# Build the firmware with the dump-clip break:
#     make -C test STRIDED_CFLAGS+=' -DKWS_DEBUG_DUMP_FIRST_CLIP=1' \
#         model-strided-s16-nodil
#
# Use from a GDB session attached to the SoC:
#     (gdb) source scripts/dump_ring_buf.gdb
#     (gdb) continue                # let the firmware run until the ebreak
#     (gdb) dump_ring_buf /tmp/ring_dump.hex
#     (gdb) shell diff /tmp/ring_dump.hex sim/down_0000.hex | head
#
# Non-interactive (Verilator / cxxrtl + openocd-sim already running):
#     riscv32-unknown-elf-gdb -batch \
#         -ex 'target extended-remote :$(GDB_PORT)' \
#         -ex 'load test/build/strided_s16_nodil_xip.elf' \
#         -ex 'continue' \
#         -ex 'source scripts/dump_ring_buf.gdb' \
#         -ex 'dump_ring_buf /tmp/ring_dump.hex'

define dump_ring_buf
  if $argc < 1
    printf "usage: dump_ring_buf <output-file>\n"
  else
    # Sanity: ring_pos should be SAMPLES_PER_CLIP (=8000) at the dump point.
    printf "ring_pos = %d (expect 8000), i2s_irq_count = %d\n", \
           ring_pos, i2s_irq_count

    set logging file $arg0
    set logging overwrite on
    set logging redirect on
    set logging on
    printf "# ring_buf dump (post-DMA capture)\n"
    printf "# 8000 samples, int8 Q7 reconstructed into the I2S FIFO word.\n"
    printf "# word = ((int16_t)q7) << 16  -- matches sim/*.hex / i2s_mic_sim\n"

    set $base = (signed char *)&ring_buf
    set $i = 0
    while $i < 8000
      # ring_buf[i] is int8_t — GDB sign-extends to int automatically.
      set $q7   = $base[$i]
      set $word = ($q7 << 16) & 0xffff0000
      printf "%08X\n", $word
      set $i = $i + 1
    end
    set logging off

    printf "ring_buf -> %s (8000 lines, hex-word format)\n", "$arg0"
  end
end

document dump_ring_buf
Dump ring_buf as 8000 hex words in the same format as sim/*.hex (the
audio fed by i2s_mic_sim).  Diff the result against the input hex to
catch DMA/ISR sample corruption.

Usage:
  (gdb) dump_ring_buf <output-file>
end
