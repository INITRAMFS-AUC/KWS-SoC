/*****************************************************************************\
|  xip_loader.v — One-shot AHB master: XIP flash → playback_buf SRAM        |
|  SPDX-License-Identifier: Apache-2.0                                        |
|                                                                             |
|  Runs once after reset. Reads N_SAMPLES 32-bit words from XIP flash        |
|  (SAMPLE_XIP_ADDR) and writes them sequentially to the playback SRAM       |
|  (PB_SRAM_ADDR). Asserts load_done when the transfer is complete.          |
|                                                                             |
|  Only instantiated when XIP_PLAYBACK is defined.                           |
\*****************************************************************************/

`ifdef XIP_N_SAMPLES
`define XL_N_SAMPLES `XIP_N_SAMPLES
`else
`define XL_N_SAMPLES 8000
`endif

module xip_loader #(
    parameter SAMPLE_XIP_ADDR = 32'h8001_0000,
    parameter PB_SRAM_ADDR    = 32'h2000_0000,
    parameter N_SAMPLES       = `XL_N_SAMPLES
) (
    input  wire        clk,
    input  wire        rst_n,

    output reg         load_done,

    // AHB-Lite master port (crossbar master 4)
    output reg  [31:0] m_haddr,
    output reg         m_hwrite,
    output reg  [ 1:0] m_htrans,
    output reg  [ 2:0] m_hsize,
    output reg  [31:0] m_hwdata,
    input  wire [31:0] m_hrdata,
    input  wire        m_hready,
    input  wire        m_hresp
);

    localparam [2:0]
        ST_IDLE      = 3'd0,
        ST_XIP_ADDR  = 3'd1,
        ST_XIP_DATA  = 3'd2,
        ST_SRAM_ADDR = 3'd3,
        ST_SRAM_DATA = 3'd4,
        ST_DONE      = 3'd5;

    reg [2:0]                    state;
    reg [$clog2(N_SAMPLES)-1:0]  idx;
    reg [31:0]                   data_buf;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state     <= ST_IDLE;
            idx       <= {$clog2(N_SAMPLES){1'b0}};
            data_buf  <= 32'h0;
            load_done <= 1'b0;
            m_haddr   <= 32'h0;
            m_hwrite  <= 1'b0;
            m_htrans  <= 2'b00;
            m_hsize   <= 3'b010;
            m_hwdata  <= 32'h0;
        end else begin
            case (state)
                ST_IDLE: begin
                    // Begin immediately after reset
                    state    <= ST_XIP_ADDR;
                end

                ST_XIP_ADDR: begin
                    // Address phase: issue read to XIP
                    m_haddr  <= SAMPLE_XIP_ADDR + {{(30-$clog2(N_SAMPLES)){1'b0}}, idx, 2'b00};
                    m_hwrite <= 1'b0;
                    m_htrans <= 2'b10; // NONSEQ
                    m_hsize  <= 3'b010; // 32-bit
                    state    <= ST_XIP_DATA;
                end

                ST_XIP_DATA: begin
                    // Data phase: hold IDLE, wait for hready
                    m_htrans <= 2'b00;
                    if (m_hready) begin
                        data_buf <= m_hrdata;
                        // Issue SRAM write address phase immediately
                        m_haddr  <= PB_SRAM_ADDR + {{(30-$clog2(N_SAMPLES)){1'b0}}, idx, 2'b00};
                        m_hwrite <= 1'b1;
                        m_htrans <= 2'b10; // NONSEQ
                        m_hsize  <= 3'b010;
                        state    <= ST_SRAM_DATA;
                    end
                end

                ST_SRAM_DATA: begin
                    // Data phase for SRAM write (playback_buf is always-ready)
                    m_hwdata <= data_buf;
                    m_htrans <= 2'b00;
                    if (m_hready) begin
                        if (idx == N_SAMPLES - 1) begin
                            state <= ST_DONE;
                        end else begin
                            idx   <= idx + 1'b1;
                            state <= ST_XIP_ADDR;
                        end
                    end
                end

                ST_DONE: begin
                    m_htrans  <= 2'b00;
                    m_hwrite  <= 1'b0;
                    load_done <= 1'b1;
                end

                default: state <= ST_IDLE;
            endcase
        end
    end

endmodule
