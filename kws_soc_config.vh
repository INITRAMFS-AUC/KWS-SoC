`ifndef SRAM_DEPTH
    `define SRAM_DEPTH (1 << 7)  // Default 16 kwords -> 64 kB
`endif

// TODO: rename this to ellude to the timer MHZ
`ifndef CLK_MHZ
    `define CLK_MHZ 12            // For Timer timebase
`endif

`ifndef DTM_TYPE
    `define DTM_TYPE "JTAG"       // JTAG or ECP5
`endif
