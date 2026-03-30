#include <stdint.h>
#include <stdio.h>
#include "uart.h"


#define I2S_BASE_ADDR 0x40008000

// how it works: struct members places sequentially in mem
// therefore they are offsetted automatically by C by their biggest type
// 32 bit (4byte offsets)
typedef struct {
    volatile uint32_t ID;       // Offset 0x0: I2S_ID Register (Read Only)
    volatile uint32_t CONF;     // Offset 0x4: I2S_CONF Register (Read/Write)
    volatile uint32_t FIFO;     // Offset 0x8: FIFO (Read Only)
} I2S_TypeDef;

#define I2S_HW ((I2S_TypeDef *) I2S_BASE_ADDR)

// --- Driver Functions ---
uint32_t i2s_get_id(void) {
    return I2S_HW->ID;
}

void i2s_configure(uint32_t clk_div, uint8_t int_en, uint8_t ws) {
    uint32_t conf_val = 0;
    
    // Shift values to their respective bit positions according to the table
    conf_val |= (clk_div << 8);         // Bits 31:8
    conf_val |= ((int_en & 0x1) << 4);  // Bit 4 (masked to ensure 1 bit)
    conf_val |= (ws & 0x1);             // Bit 0 (masked to ensure 1 bit)
    
    // Write to hardware
    I2S_HW->CONF = conf_val;
}

uint32_t i2s_read_fifo(void) {
    return I2S_HW->FIFO;
}

int main(void) {
    // smoke tewst
    uart_init();
    uint32_t id = i2s_get_id();
    // Example: Clock divider = 4, Interrupts Enabled, WS = 1
    uint8_t ws = 1;
    i2s_configure(4, 1, ws);

    uart_printf("[Smoke test] ID: 0x%x\n\n", id);
    
    
    // simple polling loop to read from FIFO
    while(1) {
        uint32_t audio_sample = i2s_read_fifo();
        i2s_configure(4, 1, ws);
        ws = ~ws;
        uart_printf("%x\n", audio_sample);
    }

    // TODO: figure out how to use its interrupt
    
    return 0;
}

