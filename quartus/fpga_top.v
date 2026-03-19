`default_nettype none
`include "kws_soc_config.vh"

module fpga_top (
    // Physical FPGA Pins (Must match pins.tcl)
    input wire  clk_50,
    input wire  rst_n,

    // JTAG
    input  wire tck,
    input  wire trst_n,
    input  wire tms,
    input  wire tdi,
    output wire tdo,

    // UART
    output wire uart_tx,
    input  wire uart_rx,
    input  wire sd,
    output wire ws,
    output wire sck
);

    // Instantiate the FPGA-Specific PLL
    wire sys_clk;
    // Optional: Can use to hold reset if needed
    wire pll_locked;

    `ifdef CYCLONE_V
        clock_pll_36 my_pll (
            .refclk   (clk_50),      
            .rst      (!rst_n),
            .outclk_0 (sys_clk),  
            .locked   (pll_locked)
        );
    `elsif CYCLONE_IV
        ALTPLL_25 my_pll (
            .inclk0 (clk_50),      
            .c0     (sys_clk),  
            .locked (pll_locked)
        );
    `else
        initial $error("No PLL defined for this architecture!");
    `endif

    kws_soc #(
        .DTM_TYPE   (`DTM_TYPE),
        .SRAM_DEPTH (`SRAM_DEPTH),
        .CLK_MHZ    (`CLK_MHZ)      	// Matches clock_pll_36 output // TODO: Make this linked to a global clk def instead of being a magic num
    ) soc_inst (
        .clk     (sys_clk),   	        // Connect PLL output to core input
        .rst_n   (rst_n),	            // Safe Reset: Wait for PLL lock // Use & pll_locked for pll lock

        .tck     (tck),
        .trst_n  (trst_n),
        .tms     (tms),
        .tdi     (tdi),
        .tdo     (tdo),

        .uart_tx (uart_tx),
        .uart_rx (uart_rx),
        .sd      (sd),
        .ws_out  (ws),
        .sck_out (sck)
    );

endmodule
