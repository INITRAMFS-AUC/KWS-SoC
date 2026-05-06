/*****************************************************************************\
|  xip_sample_player.v — AHB master that reads playback samples from XIP     |
|  flash and feeds them serially to the I2S receiver's sd input.              |
|  SPDX-License-Identifier: Apache-2.0                                        |
|                                                                             |
|  Data path (XIP_PLAYBACK=1):                                                |
|    firmware .rodata (in XIP flash @ 0x8001_0000) → AHB reads → SD pin       |
|                                                                             |
|  I2S Protocol (matches i2s_rx_core.v timing):                               |
|    - i2s_rx_core samples SD on SCK negedge, MSB-first                       |
|    - Bit 31 (MSB) is sampled on the SCK negedge immediately after WS 1→0    |
|    - 32 bits per channel, right channel ignored in MONO_MODE                |
|                                                                             |
|  When XIP_PLAYBACK is NOT defined, this module is not instantiated and      |
|  the I2S sd input comes directly from the external i2s_sd pin.              |
\*****************************************************************************/

`timescale 1ns / 1ns

module xip_sample_player #(
    // Base address in XIP space where playback_samples[] is placed by linker.
    // Must match the .playback_samples section address in test/common/link.ld
    parameter SAMPLE_XIP_ADDR = 32'h8001_0000,

    // Number of 32-bit words in the sample array.
    // Driven automatically by the XIP_N_SAMPLES Verilog macro, which the root
    // Makefile computes as PLAYBACK_SAMPLES_NUMBER * 8000 (clips × 8000 words/clip).
    // wav_to_hex.py writes PLAYBACK_SAMPLES_NUMBER to a .count sidecar so Make
    // can pick it up without manual editing.  Falls back to 8000 (one clip) when
    // XIP_PLAYBACK is not set or the macro is absent.
`ifdef XIP_N_SAMPLES
    parameter N_SAMPLES = `XIP_N_SAMPLES
`else
    parameter N_SAMPLES = 8000
`endif
) (
    input  wire        clk,       // System clock (e.g. 36 MHz FPGA / 12 MHz sim)
    input  wire        rst_n,     // Active-low reset

    // I2S timing references (outputs of apb_i2s_receiver)
    input  wire        sck_ref,   // SCK clock from I2S receiver
    input  wire        ws_ref,    // WS channel select

    // Serial data output (feeds apb_i2s_receiver's sd input)
    output reg         sd_out,

    // AHB-Lite master port (connects to crossbar master slot)
    output reg  [31:0] m_haddr,
    output reg         m_hwrite,
    output reg  [ 1:0] m_htrans,
    output reg  [ 2:0] m_hsize,
    input  wire [31:0] m_hrdata,
    input  wire        m_hready,
    input  wire        m_hresp
);

    // Initial debug - confirms module is instantiated
    initial begin
        $display("[XIP_PLAYER] Module instantiated: addr=%h samples=%0d",
                 SAMPLE_XIP_ADDR, N_SAMPLES);
    end

    // -----------------------------------------------------------------------
    // AHB master: continuous sequential reads from SAMPLE_XIP_ADDR
    // -----------------------------------------------------------------------
    reg [$clog2(N_SAMPLES)-1:0] sample_idx;
    reg [31:0]                  sample_buffer;
    reg                         buffer_valid;

    localparam AHB_ADDR = 1'b0;
    localparam AHB_DATA = 1'b1;
    reg ahb_state;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            m_haddr       <= SAMPLE_XIP_ADDR;
            m_hwrite      <= 1'b0;
            m_htrans      <= 2'b10;
            m_hsize       <= 3'b010;
            ahb_state     <= AHB_ADDR;
            sample_idx    <= {$clog2(N_SAMPLES){1'b0}};
            sample_buffer <= 32'h0;
            buffer_valid  <= 1'b0;
        end else begin
            case (ahb_state)
                AHB_ADDR: begin
                    m_htrans  <= 2'b00;
                    ahb_state <= AHB_DATA;
                end
                AHB_DATA: begin
                    if (m_hready) begin
                        sample_buffer <= m_hrdata;
                        buffer_valid  <= 1'b1;
                        sample_idx    <= (sample_idx == N_SAMPLES - 1)
                                       ? {$clog2(N_SAMPLES){1'b0}}
                                       : sample_idx + 1'b1;
                        m_haddr       <= (sample_idx == N_SAMPLES - 1)
                                       ? SAMPLE_XIP_ADDR
                                       : m_haddr + 32'd4;
                        m_htrans      <= 2'b10;
                        ahb_state     <= AHB_ADDR;
                    end
                end
            endcase
        end
    end

    // -----------------------------------------------------------------------
    // I2S SD serialization
    //
    // The i2s_rx_core samples SD on SCK negedge. We need to ensure SD is
    // stable before each SCK negedge, with bit 31 ready on the negedge
    // following WS 1→0.
    //
    // Strategy:
    //   - Detect WS 1→0 in system clock domain
    //   - On detection, load sample_buffer into shift register
    //   - Track bit position and shift on each SCK negedge
    //   - Drive SD combinationally from shift_reg[bit_pos]
    // -----------------------------------------------------------------------
    reg [31:0] shift_reg;
    reg [ 5:0] bit_pos;       // 31 = output bit 31, 0 = output bit 0, 63 = idle
    reg        ws_prev;
    reg        sck_prev;
    wire       ws_falling = ws_prev && !ws_ref;

    // Debug counter
    reg [31:0] debug_sck_count;
    reg        started;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            shift_reg    <= 32'h0;
            bit_pos      <= 6'd63;  // idle
            ws_prev      <= 1'b0;
            sck_prev     <= 1'b0;
            sd_out       <= 1'b0;
            debug_sck_count <= 32'd0;
            started      <= 1'b0;
        end else begin
            ws_prev  <= ws_ref;
            sck_prev <= sck_ref;

            // Count SCK edges for debug
            if (sck_prev && !sck_ref) begin
                debug_sck_count <= debug_sck_count + 1'b1;
            end

            // On WS 1→0 (start of left channel): load new sample
            if (ws_falling && buffer_valid) begin
                shift_reg <= sample_buffer;
                bit_pos   <= 6'd31;  // start with MSB
                if (!started) begin
                    started <= 1'b1;
                    $display("[XIP_PLAYER] t=%0t: First WS falling edge, sample=%h, sck_count=%0d",
                             $time, sample_buffer, debug_sck_count);
                end
            end
            // On SCK negedge: advance bit position
            else if (sck_prev && !sck_ref && bit_pos < 6'd32) begin
                bit_pos <= bit_pos - 1'b1;
            end

            // Drive SD combinationally based on current bit position
            if (bit_pos < 6'd32) begin
                sd_out <= shift_reg[bit_pos];
            end else begin
                sd_out <= 1'b0;  // idle
            end
        end
    end

    // Debug: report AHB activity
    reg [31:0] ahb_read_count;
    always @(posedge clk) begin
        if (m_hready && ahb_state == AHB_DATA) begin
            ahb_read_count <= ahb_read_count + 1'b1;
            if (ahb_read_count < 10) begin
                $display("[XIP_PLAYER] t=%0t: AHB read %0d: addr=%h data=%h",
                         $time, ahb_read_count, m_haddr, m_hrdata);
            end
        end
    end

endmodule
