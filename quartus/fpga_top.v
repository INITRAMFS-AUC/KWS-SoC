`default_nettype none
`include "kws_soc_config.vh"

// TODO: rst signal based on jtag needs to reach external flash somehow
module fpga_top (
    // Physical FPGA Pins (Must match pins.tcl)
    input wire          clk_50,
    input wire          rst_n,

    // JTAG
    input  wire         tck,
    input  wire         trst_n,
    input  wire         tms,
    input  wire         tdi,
    output wire         tdo,

    // UART
    output wire         uart_tx,
    input  wire         uart_rx,

    // I2S
    input  wire         i2s_sd,
    output wire         i2s_ws,
    output wire         i2s_sck,

	// XIP
	output wire         xip_csn,
    output wire         xip_sck,
    inout  wire [3:0]   flash_io   // Bidirectional QSPI data bus (IO0-IO3)
);

    // Instantiate the generated PLL (produced by scripts/gen_pll.py)
    wire sys_clk;
    wire pll_locked;

    clock_pll_gen my_pll (
        .inclk0 (clk_50),
        .areset (!rst_n),
        .c0     (sys_clk),
        .locked (pll_locked)
    );


    // Tristate buffers: merge internal do/doe/di onto the physical inout pads
    wire [3:0] xip_doe;
    wire [3:0] xip_do;
    wire [3:0] flash_di_int; // Internal wires for flash data input

    genvar i;
    generate
        for (i = 0; i < 4; i = i + 1) begin : qspi_io_buf
            assign flash_io[i]    = xip_doe[i] ? xip_do[i] : 1'bz; // doe=1 → FPGA drives, doe=0 → High-Z (Flash drives)
            assign flash_di_int[i] = flash_io[i];
        end
    endgenerate

    kws_soc #(
        .DTM_TYPE   (`DTM_TYPE),
        .SRAM_DEPTH (`SRAM_DEPTH),
        .CLK_MHZ    (`CLK_MHZ),      	// Matches clock_pll_36 output // TODO: Make this linked to a global clk def instead of being a magic num
        `include "hazard3_instantiation_params.vh"
    ) soc_inst (
        .clk            (sys_clk),      // Connect PLL output to core input
        .rst_n          (rst_n & pll_locked), // Hold reset until PLL locks and sys_clk is stable

        .tck            (tck),
        .trst_n         (trst_n),
        .tms            (tms),
        .tdi            (tdi),
        .tdo            (tdo),

        .uart_tx        (uart_tx),
        .uart_rx        (uart_rx),
        
        .i2s_sd             (i2s_sd),
        .i2s_ws_out         (i2s_ws),
        .i2s_sck_out        (i2s_sck),

        .xip_csn        (xip_csn),
        .xip_sck        (xip_sck),
        .xip_doe        (xip_doe),
        .xip_do         (xip_do),
        .flash_di       (flash_di_int)
    );

endmodule
