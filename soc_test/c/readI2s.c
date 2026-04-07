#include <stdint.h>
#include <stdio.h>
#include "uart.h"


#define I2S_BASE_ADDR 0x40005000

#define I2S_ID   (*(volatile uint32_t *)(I2S_BASE_ADDR + 0x00))
#define I2S_CONF (*(volatile uint32_t *)(I2S_BASE_ADDR + 0x04))
#define I2S_FIFO (*(volatile uint32_t *)(I2S_BASE_ADDR + 0x08))

// --- Driver Functions ---
uint32_t i2s_get_id(void) {
    return I2S_ID;
}

void i2s_configure(uint32_t clk_div, uint8_t int_en) {
    uint32_t conf_val = 0;
    
    // Shift values to their respective bit positions according to the table
    conf_val |= (clk_div << 8);         // Bits 31:8
    conf_val |= ((int_en & 0x1) << 4);  // Bit 4 (masked to ensure 1 bit)
    
    // Write to hardware
    I2S_CONF = conf_val;
}

uint32_t i2s_read_fifo(void) {
    return I2S_FIFO;
}

int main(void) {
    // smoke tewst
    uart_init();
    uint32_t id = i2s_get_id();
    
    // Example: Clock divider = 4, Interrupts Enabled
    i2s_configure(4, 0);

    uart_printf("[Smoke test] ID: 0x%x\n\n", id);
    
    
    // simple polling loop to read from FIFO
    while(1) {
        uint32_t audio_sample = i2s_read_fifo();
        uart_printf("%x\n", audio_sample);
    }

    // TODO: figure out how to use its interrupt
    
    return 0;
}

