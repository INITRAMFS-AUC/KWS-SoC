/*
 * conv1d_accel_regs.h
 *
 * Firmware register map and helpers for the Conv1D accelerator's APB
 * control wrapper. Drop this header into the KWS-SoC firmware tree and
 * include it from the keyword-spotting application.
 *
 * The base address below is the suggested KWS-SoC mapping; override
 * CONV1D_BASE before including this header (or via build flags) if your
 * APB splitter places the slave somewhere else.
 */
#ifndef CONV1D_ACCEL_REGS_H
#define CONV1D_ACCEL_REGS_H

#include <stdint.h>

#ifndef CONV1D_BASE
#define CONV1D_BASE         0x4000C000u
#endif

/* Register offsets (bytes) */
#define CONV1D_ID           0x00u   /* RO: ID/version (expect 0x12345678) */
#define CONV1D_CTRL         0x04u   /* WO: bit[0] = start                  */
#define CONV1D_STATUS       0x08u   /* RO: bit[0] = busy, bit[1] = done    */
#define CONV1D_INPUT_BASE   0x0Cu   /* RW: input tensor base address       */
#define CONV1D_WEIGHT_BASE  0x10u   /* RW: weight tensor base address      */
#define CONV1D_BIAS_BASE    0x14u   /* RW: bias tensor base address        */
#define CONV1D_OUTPUT_BASE  0x18u   /* RW: output tensor base address      */
#define CONV1D_INPUT_LEN    0x1Cu   /* RW: input length (samples)          */
#define CONV1D_IN_CH        0x20u   /* RW: input channels                  */
#define CONV1D_OUT_CH       0x24u   /* RW: output channels                 */
#define CONV1D_QUANT        0x28u   /* RW: [4:0]=out_shift, [5]=relu_en    */
#define CONV1D_QUANT_INDEX  0x2Cu   /* RW: [7:0]=output channel index      */
#define CONV1D_QUANT_SHIFT_DATA \
                            0x30u   /* RW: [4:0]=shift for selected index  */

/* Scratchpad APB loader registers (firmware preload/readback) */
#define CONV1D_SPAD_ADDR    0x34u   /* RW: scratchpad byte addr (auto-inc) */
#define CONV1D_SPAD_WDATA   0x38u   /* WO: write word to scratchpad        */
#define CONV1D_SPAD_RDATA   0x3Cu   /* RO: read word from scratchpad       */
#define CONV1D_SPAD_CTRL    0x40u   /* WO: bit[0]=reset SPAD_ADDR to 0     */

/* Bit definitions */
#define CONV1D_CTRL_START        (1u << 0)
#define CONV1D_STATUS_BUSY       (1u << 0)
#define CONV1D_STATUS_DONE       (1u << 1)
#define CONV1D_QUANT_SHIFT_MASK  0x1Fu        /* [4:0] */
#define CONV1D_QUANT_RELU_EN     (1u << 5)

/* Expected ID register value (from APB wrapper) */
#define CONV1D_EXPECTED_ID  0x12345678u

/* ------------------------------------------------------------------ */
/* MMIO helpers                                                        */
/* ------------------------------------------------------------------ */

#ifdef CONV1D_ACCEL_HOST_TEST
enum { CONV1D_HOST_REG_WORDS = 16 };
static uint32_t conv1d_host_regs[CONV1D_HOST_REG_WORDS];
#endif

static inline void conv1d_write_reg(uint32_t offset, uint32_t value)
{
#ifdef CONV1D_ACCEL_HOST_TEST
    uint32_t index = offset >> 2;

    if (index >= CONV1D_HOST_REG_WORDS)
        return;

    conv1d_host_regs[index] = value;

    if (offset == CONV1D_CTRL && (value & CONV1D_CTRL_START))
        conv1d_host_regs[CONV1D_STATUS >> 2] = CONV1D_STATUS_DONE;
#else
    volatile uint32_t *reg = (volatile uint32_t *)(uintptr_t)(CONV1D_BASE + offset);
    *reg = value;
#endif
}

static inline uint32_t conv1d_read_reg(uint32_t offset)
{
#ifdef CONV1D_ACCEL_HOST_TEST
    uint32_t index = offset >> 2;

    if (offset == CONV1D_ID)
        return CONV1D_EXPECTED_ID;

    if (index >= CONV1D_HOST_REG_WORDS)
        return 0;

    return conv1d_host_regs[index];
#else
    volatile uint32_t *reg = (volatile uint32_t *)(uintptr_t)(CONV1D_BASE + offset);
    return *reg;
#endif
}

static inline void conv1d_start(void)
{
    conv1d_write_reg(CONV1D_CTRL, CONV1D_CTRL_START);
}

static inline int conv1d_done(void)
{
    return (conv1d_read_reg(CONV1D_STATUS) & CONV1D_STATUS_DONE) ? 1 : 0;
}

static inline int conv1d_busy(void)
{
    return (conv1d_read_reg(CONV1D_STATUS) & CONV1D_STATUS_BUSY) ? 1 : 0;
}

static inline void conv1d_write_output_shift(int oc, uint32_t shift)
{
    if (oc < 0)
        return;

    conv1d_write_reg(CONV1D_QUANT_INDEX, (uint32_t)oc & 0xFFu);
    conv1d_write_reg(CONV1D_QUANT_SHIFT_DATA, shift & CONV1D_QUANT_SHIFT_MASK);
}

static inline void conv1d_load_output_shifts(const int8_t *shifts, int out_ch)
{
    int oc;

    if (shifts == NULL || out_ch <= 0)
        return;

    for (oc = 0; oc < out_ch; oc++)
        conv1d_write_output_shift(oc, (uint32_t)shifts[oc]);
}

static inline void conv1d_load_output_shifts_u8(const uint8_t *shifts, int out_ch)
{
    int oc;

    if (shifts == NULL || out_ch <= 0)
        return;

    for (oc = 0; oc < out_ch; oc++)
        conv1d_write_output_shift(oc, shifts[oc]);
}

/* ------------------------------------------------------------------ */
/* Scratchpad APB loader helpers (sim-scratchpad path)                */
/* ------------------------------------------------------------------ */

static inline void conv1d_spad_reset(void)
{
    conv1d_write_reg(CONV1D_SPAD_CTRL, 1u);
}

static inline void conv1d_spad_set_addr(uint32_t byte_addr)
{
    conv1d_write_reg(CONV1D_SPAD_ADDR, byte_addr);
}

static inline void conv1d_spad_write_word(uint32_t word)
{
    conv1d_write_reg(CONV1D_SPAD_WDATA, word);
}

static inline uint32_t conv1d_spad_read_word(void)
{
    return conv1d_read_reg(CONV1D_SPAD_RDATA);
}

/* Write a single signed 8-bit value at the given byte address.
 * The 8-bit value is packed into the correct byte lane of a 32-bit word.
 * Only the target byte is updated (full-word write with the value in the
 * correct lane; the scratchpad stores byte lanes individually via wr_strb,
 * but the APB loader always writes full words — caller packs as needed). */
static inline void conv1d_spad_write_s8_at(uint32_t byte_addr, int8_t val)
{
    uint32_t word_addr = byte_addr & ~3u;
    uint32_t lane      = byte_addr & 3u;
    uint32_t word      = (uint32_t)(uint8_t)val << (lane * 8u);
    conv1d_spad_set_addr(word_addr);
    conv1d_spad_write_word(word);
}

static inline int8_t conv1d_spad_read_s8_at(uint32_t byte_addr)
{
    uint32_t word_addr = byte_addr & ~3u;
    uint32_t lane      = byte_addr & 3u;
    conv1d_spad_set_addr(word_addr);
    return (int8_t)((conv1d_spad_read_word() >> (lane * 8u)) & 0xFFu);
}

/*
 * Run a Conv1D layer end-to-end. Configures all programmable registers,
 * fires CTRL.start, and busy-polls until STATUS.done is observed.
 *
 * Caller is responsible for placing input, weight, bias, and output
 * tensors at the addresses passed in, and for ensuring those addresses
 * are reachable by the accelerator's memory-side ports in the SoC.
 *
 * Returns 0 on success.
 */
static inline int conv1d_run_layer(uint32_t input_base,
                                   uint32_t weight_base,
                                   uint32_t bias_base,
                                   uint32_t output_base,
                                   uint16_t input_len,
                                   uint16_t in_ch,
                                   uint16_t out_ch,
                                   uint8_t  out_shift,
                                   int      relu_en)
{
    uint32_t quant = ((uint32_t)out_shift & CONV1D_QUANT_SHIFT_MASK)
                   | (relu_en ? CONV1D_QUANT_RELU_EN : 0u);

    conv1d_write_reg(CONV1D_INPUT_BASE,  input_base);
    conv1d_write_reg(CONV1D_WEIGHT_BASE, weight_base);
    conv1d_write_reg(CONV1D_BIAS_BASE,   bias_base);
    conv1d_write_reg(CONV1D_OUTPUT_BASE, output_base);
    conv1d_write_reg(CONV1D_INPUT_LEN,   (uint32_t)input_len);
    conv1d_write_reg(CONV1D_IN_CH,       (uint32_t)in_ch);
    conv1d_write_reg(CONV1D_OUT_CH,      (uint32_t)out_ch);
    conv1d_write_reg(CONV1D_QUANT,       quant);

    conv1d_start();

    while (!conv1d_done()) {
        /* spin; firmware can replace this with a WFI/yield as needed */
    }

    return 0;
}

#endif /* CONV1D_ACCEL_REGS_H */
