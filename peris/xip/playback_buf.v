/*****************************************************************************\
|  playback_buf.v — Dual-port playback sample buffer                         |
|  SPDX-License-Identifier: Apache-2.0                                        |
|                                                                             |
|  AHB slave port  : xip_loader writes samples; d-port may read for debug    |
|  Direct read port: i2s_sd_driver reads with single-cycle latency            |
|                                                                             |
|  Only instantiated when XIP_PLAYBACK is defined.                           |
\*****************************************************************************/

`ifdef XIP_N_SAMPLES
`define PB_N_SAMPLES `XIP_N_SAMPLES
`else
`define PB_N_SAMPLES 8000
`endif

module playback_buf #(
    parameter N_SAMPLES = `PB_N_SAMPLES,
    parameter W_ADDR    = 32,
    parameter W_DATA    = 32
) (
    input  wire             clk,
    input  wire             rst_n,

    // AHB-Lite slave port (crossbar slave 4 @ 0x2000_0000)
    input  wire             ahbls_hready,
    output wire             ahbls_hready_resp,
    output wire             ahbls_hresp,
    input  wire [W_ADDR-1:0] ahbls_haddr,
    input  wire             ahbls_hwrite,
    input  wire [1:0]       ahbls_htrans,
    input  wire [2:0]       ahbls_hsize,
    input  wire [2:0]       ahbls_hburst,
    input  wire [3:0]       ahbls_hprot,
    input  wire             ahbls_hmastlock,
    input  wire [W_DATA-1:0] ahbls_hwdata,
    output reg  [W_DATA-1:0] ahbls_hrdata,

    // Direct synchronous read port for i2s_sd_driver (1-cycle latency)
    input  wire [31:0]      direct_raddr,
    input  wire             direct_ren,
    output reg  [W_DATA-1:0] direct_rdata
);

    // -----------------------------------------------------------------------
    // Internal RAM — inferred as Cyclone V M10K SDP
    // -----------------------------------------------------------------------
    reg [31:0] mem [0:N_SAMPLES-1];

    // -----------------------------------------------------------------------
    // AHB slave — always-ready (no wait states)
    // -----------------------------------------------------------------------
    assign ahbls_hready_resp = 1'b1;
    assign ahbls_hresp       = 1'b0;

    // Capture address-phase attributes
    wire ahb_active = ahbls_hready && ahbls_htrans[1];
    wire [$clog2(N_SAMPLES)-1:0] ahb_word_addr = ahbls_haddr[$clog2(N_SAMPLES)+1:2];

    reg [$clog2(N_SAMPLES)-1:0] lat_addr;
    reg                         lat_write;
    reg                         lat_valid;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            lat_addr  <= {$clog2(N_SAMPLES){1'b0}};
            lat_write <= 1'b0;
            lat_valid <= 1'b0;
        end else begin
            lat_valid <= ahb_active;
            if (ahb_active) begin
                lat_addr  <= ahb_word_addr;
                lat_write <= ahbls_hwrite;
            end
        end
    end

    // Write (data phase)
    always @(posedge clk) begin
        if (lat_valid && lat_write)
            mem[lat_addr] <= ahbls_hwdata;
    end

    // AHB Read (data phase — data registered in address phase, stable in data phase)
    always @(posedge clk) begin
        if (ahb_active && !ahbls_hwrite)
            ahbls_hrdata <= mem[ahb_word_addr];
    end

    // -----------------------------------------------------------------------
    // Direct read port for i2s_sd_driver
    // -----------------------------------------------------------------------
    always @(posedge clk) begin
        if (direct_ren)
            direct_rdata <= mem[direct_raddr[$clog2(N_SAMPLES)-1:0]];
    end

endmodule
