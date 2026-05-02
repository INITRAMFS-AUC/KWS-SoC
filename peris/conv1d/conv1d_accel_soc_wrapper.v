// SoC-facing top wrapper for the Conv1D accelerator.
//
// This module is the single integration point for KWS-SoC. It exposes:
//   - An APB-3 slave for CPU-facing control/config (pclk/presetn naming).
//   - An abstract memory-side interface (rd0/rd1/wr) that an SoC integrator
//     wraps with a local scratchpad, AHB-Lite master, or DMA preload/writeback.
//
// USE_WEIGHT_BUFFER selection:
//   The underlying datapath selects between the streaming nobuffer mode and
//   the weight-stationary buffered mode at compile time via the
//   `USE_WEIGHT_BUFFER preprocessor define (default 0). The parameter on this
//   wrapper is informational/documentation only; toggle the build define
//   (`-DUSE_WEIGHT_BUFFER=1`) to change modes. The parameter is preserved so
//   downstream tooling can record the chosen mode without editing RTL.
`timescale 1ns/1ps

module conv1d_accel_soc_wrapper #(
    parameter integer ADDR_W            = 32,
    parameter integer DATA_W            = 32,
    parameter integer MAX_IN_CH         = 64,
    parameter integer MAX_OUT_CH        = 64,
    parameter integer USE_WEIGHT_BUFFER = 1
)(
    // APB-3 slave (CPU-facing control/config)
    input  wire                  pclk,
    input  wire                  presetn,
    input  wire                  psel,
    input  wire                  penable,
    input  wire                  pwrite,
    input  wire [31:0]           paddr,
    input  wire [31:0]           pwdata,
    output wire [31:0]           prdata,
    output wire                  pready,
    output wire                  pslverr,

    // Memory-side abstract interface (SoC integrator wraps these)
    output wire                  rd0_en,
    output wire [ADDR_W-1:0]     rd0_addr,
    input  wire [DATA_W-1:0]     rd0_data,
    input  wire                  rd0_valid,

    output wire                  rd1_en,
    output wire [ADDR_W-1:0]     rd1_addr,
    input  wire [DATA_W-1:0]     rd1_data,
    input  wire                  rd1_valid,

    output wire                  wr_en,
    output wire [ADDR_W-1:0]     wr_addr,
    output wire [DATA_W-1:0]     wr_data,
    output wire [3:0]            wr_strb,

    // APB scratchpad loader port (firmware preload/readback)
    output wire                  spad_lpb_wr_en,
    output wire [ADDR_W-1:0]     spad_lpb_wr_addr,
    output wire [DATA_W-1:0]     spad_lpb_wr_data,
    output wire [3:0]            spad_lpb_wr_strb,
    output wire [ADDR_W-1:0]     spad_lpb_rd_addr,
    input  wire [DATA_W-1:0]     spad_lpb_rd_data
);

    // Tag the parameter so synthesis tooling can flag a mismatch between the
    // RTL build define and the SoC integration intent. Pure-information,
    // no functional effect.
    // synthesis translate_off
    initial begin
        $display("[conv1d_accel_soc_wrapper] USE_WEIGHT_BUFFER param = %0d, MAX_IN_CH = %0d, MAX_OUT_CH = %0d",
                 USE_WEIGHT_BUFFER, MAX_IN_CH, MAX_OUT_CH);
    end
    // synthesis translate_on

    apb_conv1d_layer_accel #(
        .ADDR_W   (ADDR_W),
        .DATA_W   (DATA_W),
        .MAX_IN_CH(MAX_IN_CH),
        .MAX_OUT_CH(MAX_OUT_CH)
    ) u_apb_core (
        .clk        (pclk),
        .rst_n      (presetn),

        .paddr      (paddr),
        .psel       (psel),
        .penable    (penable),
        .pwrite     (pwrite),
        .pwdata     (pwdata),
        .prdata     (prdata),
        .pready     (pready),
        .pslverr    (pslverr),

        // The inner APB wrapper exposes these for visibility/testbench use.
        // At the SoC top level they are internal-only; leave unconnected.
        .start       (),
        .busy        (),
        .done        (),
        .input_base  (),
        .weight_base (),
        .bias_base   (),
        .output_base (),
        .input_len   (),
        .in_ch       (),
        .out_ch      (),
        .out_shift   (),
        .relu_en     (),

        .rd0_en      (rd0_en),
        .rd0_addr    (rd0_addr),
        .rd0_data    (rd0_data),
        .rd0_valid   (rd0_valid),

        .rd1_en      (rd1_en),
        .rd1_addr    (rd1_addr),
        .rd1_data    (rd1_data),
        .rd1_valid   (rd1_valid),

        .wr_en       (wr_en),
        .wr_addr     (wr_addr),
        .wr_data     (wr_data),
        .wr_strb     (wr_strb),

        .spad_lpb_wr_en   (spad_lpb_wr_en),
        .spad_lpb_wr_addr (spad_lpb_wr_addr),
        .spad_lpb_wr_data (spad_lpb_wr_data),
        .spad_lpb_wr_strb (spad_lpb_wr_strb),
        .spad_lpb_rd_addr (spad_lpb_rd_addr),
        .spad_lpb_rd_data (spad_lpb_rd_data)
    );

endmodule
