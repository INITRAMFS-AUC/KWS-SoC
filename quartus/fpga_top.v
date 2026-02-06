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
    input  wire uart_rx
);

    // Instantiate the FPGA-Specific PLL
    wire sys_clk;
    // wire pll_locked; // Optional: Can use to hold reset if needed

    clock_pll_36 my_pll (
        .refclk   (clk_50),
        .rst      (!rst_n),  // Ensure polarity matches your PLL settings
        .outclk_0 (sys_clk),
        // .locked   (pll_locked)
    );

    kws_soc #(
        .DTM_TYPE   ("JTAG"),
        .SRAM_DEPTH (1 << 15),
        .CLK_MHZ    (36)      // Matches clock_pll_36 output // TODO: Make this linked to a global clk def instead of being a magic num
    ) soc_inst (
        .clk     (sys_clk),   // Connect PLL output to core input
        .rst_n   (rst_n ), // Safe Reset: Wait for PLL lock // Use & pll_locked for pll lock

        .tck     (tck),
        .trst_n  (trst_n),
        .tms     (tms),
        .tdi     (tdi),
        .tdo     (tdo),

        .uart_tx (uart_tx),
        .uart_rx (uart_rx)
    );

endmodule
