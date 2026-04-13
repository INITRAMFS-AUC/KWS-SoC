#include <stdint.h>
#include "uart.h"
#include "i2s_regs.h"

#define I2S_BASE_ADDR 0x40008000
#define I2S ((i2s_hw_t *)I2S_BASE_ADDR)
#define I2S_FIFO_DEPTH 8

uint32_t i2s_get_id(void) {
    return I2S->id;
}

void i2s_configure(uint32_t clk_div, uint8_t int_en) {
    uint32_t conf_val = 0;
    conf_val |= ((clk_div << I2S_CONF_DIV_LSB) & I2S_CONF_DIV_MASK);
    conf_val |= ((int_en  << I2S_CONF_IRQ_EN_LSB) & I2S_CONF_IRQ_EN_MASK);
    I2S->conf = conf_val;
}

void __attribute__((interrupt("machine"))) i2s_isr(void) {
    for (int i = 0; i < I2S_FIFO_DEPTH; i++) {
        uint32_t sample = I2S->fifo;
        uart_printf("%x\r\n", sample);
    }
}

static inline void csr_set_mtvec(void (*handler)(void)) {
    asm volatile ("csrw mtvec, %0" :: "r"((uintptr_t)handler));
}

static inline void csr_enable_meie(void) {
    asm volatile ("csrs mie, %0" :: "r"(1u << 11));
}

static inline void csr_enable_mie(void) {
    asm volatile ("csrsi mstatus, 8");
}

static inline void wfi(void) {
    asm volatile ("wfi");
}

int main(void) {
    uart_init();
    uint32_t id = i2s_get_id();
    uart_printf("[Smoke test] ID: 0x%x\r\n\r\n", id);
    csr_set_mtvec(i2s_isr);
    csr_enable_meie();
    i2s_configure(4, 1);
    csr_enable_mie();
    while (1) { wfi(); }
    return 0;
}
