/*****************************************************************************\
|  i2s_sd_driver.v — I2S SD serializer from playback_buf SRAM               |
|  SPDX-License-Identifier: Apache-2.0                                        |
|                                                                             |
|  Reads 32-bit samples from playback_buf via its direct synchronous read    |
|  port and serializes them MSB-first onto sd_out, synchronised to the       |
|  I2S SCK/WS timing outputs of apb_i2s_receiver.                            |
|                                                                             |
|  Timing:                                                                    |
|    - Activates only after load_done is asserted                            |
|    - On WS 1→0 (left-channel start): reads next sample from SRAM          |
|    - On each SCK negedge: shifts out one bit MSB-first                     |
|    - Loops continuously (sample_idx wraps at N_SAMPLES-1)                  |
|                                                                             |
|  Only instantiated when XIP_PLAYBACK is defined.                           |
\*****************************************************************************/

`ifdef XIP_N_SAMPLES
`define ISD_N_SAMPLES `XIP_N_SAMPLES
`else
`define ISD_N_SAMPLES 8000
`endif

module i2s_sd_driver #(
    parameter N_SAMPLES = `ISD_N_SAMPLES
) (
    input  wire        clk,
    input  wire        rst_n,

    input  wire        load_done,

    // I2S timing references from apb_i2s_receiver
    input  wire        sck_ref,
    input  wire        ws_ref,

    // Serial data output → apb_i2s_receiver sd input
    output reg         sd_out,

    // Direct read port to playback_buf
    output reg  [31:0] rd_addr,
    output reg         rd_en,
    input  wire [31:0] rd_data
);

    reg [$clog2(N_SAMPLES)-1:0] sample_idx;
    reg [31:0]                  shift_reg;
    reg [ 5:0]                  bit_pos;    // 31=MSB, counts down; 63=idle

    reg                         ws_prev;
    reg                         sck_prev;
    reg                         pending_load; // shift_reg load pending after SRAM read

    wire ws_falling  = ws_prev  && !ws_ref;
    wire sck_falling = sck_prev && !sck_ref;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            sample_idx   <= {$clog2(N_SAMPLES){1'b0}};
            shift_reg    <= 32'h0;
            bit_pos      <= 6'd63; // idle
            ws_prev      <= 1'b0;
            sck_prev     <= 1'b0;
            sd_out       <= 1'b0;
            rd_addr      <= 32'h0;
            rd_en        <= 1'b0;
            pending_load <= 1'b0;
        end else begin
            ws_prev  <= ws_ref;
            sck_prev <= sck_ref;
            rd_en    <= 1'b0; // default: no read

            // Latch SRAM data one cycle after the read request
            if (pending_load) begin
                shift_reg    <= rd_data;
                bit_pos      <= 6'd31;
                pending_load <= 1'b0;
            end

            if (load_done) begin
                // On WS 1→0: request next sample from SRAM
                if (ws_falling) begin
                    rd_addr      <= {{(32-$clog2(N_SAMPLES)){1'b0}}, sample_idx};
                    rd_en        <= 1'b1;
                    pending_load <= 1'b1;
                    sample_idx   <= (sample_idx == N_SAMPLES - 1)
                                    ? {$clog2(N_SAMPLES){1'b0}}
                                    : sample_idx + 1'b1;
                end

                // On SCK negedge: advance bit pointer
                if (sck_falling && bit_pos < 6'd32) begin
                    bit_pos <= bit_pos - 1'b1;
                end
            end

            // Drive SD from current bit position
            if (load_done && bit_pos < 6'd32)
                sd_out <= shift_reg[bit_pos[4:0]];
            else
                sd_out <= 1'b0;
        end
    end

endmodule
