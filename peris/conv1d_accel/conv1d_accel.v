`default_nettype none

// Conv1D hardware accelerator — int8 inputs/weights, int32 accumulator.
//
// APB slave for config registers, AHB-Lite master for DMA reads/writes.
// Processes one Conv1D layer per invocation. Input layout: NWC int8
// (batch=1 assumed). Valid-convolution output width: W_out = (W_in - K_w)/stride + 1.
// For same-padding, firmware must pre-pad the input buffer.
//
// Register map (base 0x4000_C000, byte-addressed via 16-bit paddr):
//   0x00  CTRL       [0]=start(w)  [8]=busy(r)  [9]=done(r)
//   0x04  SRC_ADDR   input feature map byte address (SRAM)
//   0x08  WT_ADDR    weight array byte address (SRAM or flash via XIP)
//   0x0C  DST_ADDR   output feature map byte address (SRAM)
//   0x10  BS_ADDR    bias array byte address (int32 array, firmware pre-shifted)
//   0x14  CFG0       [7:0]=C_in  [15:8]=C_out  [23:16]=K_w  [31:24]=stride
//   0x18  CFG1       [15:0]=W_in  (W_out derived: (W_in-K_w)/stride+1)
//   0x1C  SHIFT      [4:0]=scalar output right-shift (legacy / unused when SHIFT_ADDR set)
//   0x20  SHIFT_ADDR per-channel shift byte array address (uint8_t [C_out])
//
// Loop order: C_out (outer) x W_out (inner).
// Weights, bias and per-channel shift are loaded ONCE per output channel then
// reused across all output positions, amortising XIP flash read cost.
//
// AHB pipeline timing (r_wait flag):
//   Both SRAM and XIP cache have 1-cycle latency between address and HRDATA.
//   r_wait causes each data-capture state to skip the first hready=1 after an
//   address is issued.  For bursts the skip cycle advances haddr to keep the
//   pipeline primed; for single reads it is an unconditional stall.

module conv1d_accel (
    input  wire        clk,
    input  wire        rst_n,

    // APB slave (control registers)
    input  wire [15:0] paddr,
    input  wire        psel,
    input  wire        penable,
    input  wire        pwrite,
    input  wire [31:0] pwdata,
    output reg  [31:0] prdata,
    output wire        pready,
    output wire        pslverr,

    // AHB-Lite master (DMA reads/writes to SRAM and flash)
    output reg  [31:0] haddr,
    output reg  [ 2:0] hburst,
    output reg  [ 2:0] hsize,
    output reg  [ 1:0] htrans,
    output reg         hwrite,
    output reg  [31:0] hwdata,
    output wire [ 3:0] hprot,
    output wire        hmastlock,
    input  wire [31:0] hrdata,
    input  wire        hready,
    input  wire        hresp
);

    // -------------------------------------------------------------------------
    // AHB / APB constants

    localparam HTRANS_IDLE   = 2'b00;
    localparam HTRANS_NONSEQ = 2'b10;
    localparam HTRANS_SEQ    = 2'b11;
    localparam HBURST_SINGLE = 3'b000;
    localparam HBURST_INCR   = 3'b001;
    localparam HSIZE_BYTE    = 3'b000;
    localparam HSIZE_WORD    = 3'b010;

    assign hprot     = 4'b0011;  // data, privileged, non-cacheable, non-bufferable
    assign hmastlock = 1'b0;
    assign pready    = 1'b1;     // APB: no wait states
    assign pslverr   = 1'b0;

    // -------------------------------------------------------------------------
    // Config registers

    reg [31:0] r_src_addr, r_wt_addr, r_dst_addr, r_bs_addr, r_shift_addr;
    reg [ 7:0] r_c_in, r_c_out, r_k_w, r_stride;
    reg [15:0] r_w_in;
    reg [ 4:0] r_shift;     // legacy scalar shift (kept for compatibility)
    reg        r_busy, r_done;

    // APB write — paddr[5:2] selects register (covers 0x00–0x3C)
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            r_src_addr   <= 32'h0; r_wt_addr    <= 32'h0;
            r_dst_addr   <= 32'h0; r_bs_addr    <= 32'h0;
            r_shift_addr <= 32'h0;
            r_c_in <= 8'd1; r_c_out <= 8'd1; r_k_w <= 8'd1; r_stride <= 8'd1;
            r_w_in <= 16'd1; r_shift <= 5'd0;
        end else if (psel && penable && pwrite) begin
            case (paddr[5:2])
                4'd1: r_src_addr               <= pwdata;
                4'd2: r_wt_addr                <= pwdata;
                4'd3: r_dst_addr               <= pwdata;
                4'd4: r_bs_addr                <= pwdata;
                4'd5: {r_stride,r_k_w,r_c_out,r_c_in} <= pwdata;
                4'd6: r_w_in                   <= pwdata[15:0];
                4'd7: r_shift                  <= pwdata[4:0];
                4'd8: r_shift_addr             <= pwdata;
                default: ;
            endcase
        end
    end

    // APB read
    always @(*) begin
        case (paddr[5:2])
            4'd0: prdata = {22'b0, r_done, r_busy, 8'b0};
            4'd1: prdata = r_src_addr;
            4'd2: prdata = r_wt_addr;
            4'd3: prdata = r_dst_addr;
            4'd4: prdata = r_bs_addr;
            4'd5: prdata = {r_stride, r_k_w, r_c_out, r_c_in};
            4'd6: prdata = {16'b0, r_w_in};
            4'd7: prdata = {27'b0, r_shift};
            4'd8: prdata = r_shift_addr;
            default: prdata = 32'b0;
        endcase
    end

    // -------------------------------------------------------------------------
    // Internal buffers (64 words = 256 bytes each — covers K_w*C_in up to 256)

    reg [31:0] in_buf [0:63];
    reg [31:0] wt_buf [0:63];

    // -------------------------------------------------------------------------
    // Derived parameters (registered when start fires)

    reg [15:0] patch_bytes;   // K_w * C_in
    reg [ 5:0] patch_words;   // ceil(patch_bytes / 4)  — used for input reads
    reg [15:0] w_out_cnt;     // (W_in - K_w) / stride + 1
    reg [ 1:0] tail_bytes;    // patch_bytes[1:0] — bytes in last word (input reads)

    // Weight read byte-addressing (handles unaligned filter offsets)
    reg [ 1:0] r_wt_lane0;   // (r_wt_addr + c_pos*patch_bytes)[1:0] — byte lane of wt byte 0

    // Full weight base address for current filter (combinational)
    wire [31:0] w_wt_addr = r_wt_addr + ({24'b0, c_pos} * {16'b0, patch_bytes});

    // -------------------------------------------------------------------------
    // State machine
    //
    // Loop order: C_out outer, W_out inner.
    //   Per c_pos: load weights (S_WT_*), bias (S_BIAS_*), shift (S_SHIFT_*).
    //   Per w_pos: load input (S_IN_*), MAC, write result (S_WRITE_*).
    //   S_ADVANCE: increment w_pos; on rollover increment c_pos and reload weights.

    localparam S_IDLE       = 4'd0;
    localparam S_INIT       = 4'd1;
    localparam S_WT_ADDR    = 4'd4;   // issue AHB read address for wt_buf (first byte)
    localparam S_WT_DATA    = 4'd5;   // capture weight bytes
    localparam S_BIAS_ADDR  = 4'd7;   // read int32 bias[c_pos]
    localparam S_BIAS_DATA  = 4'd8;   // capture bias, then issue shift byte address
    localparam S_SHIFT_DATA = 4'd10;  // capture shift byte (address issued in S_BIAS_DATA)
    localparam S_IN_ADDR    = 4'd2;   // issue AHB read address for in_buf
    localparam S_IN_DATA    = 4'd3;   // capture hrdata into in_buf
    localparam S_MAC        = 4'd6;   // 4 MACs per cycle
    localparam S_WRITE_ADDR = 4'd11;  // write 1 byte result
    localparam S_WRITE_DATA = 4'd12;  // AHB write data phase
    localparam S_ADVANCE    = 4'd13;  // increment w_pos or c_pos
    localparam S_DONE       = 4'd14;

    reg [ 3:0] state;
    reg [15:0] w_pos;         // current output column position
    reg [ 7:0] c_pos;         // current output channel
    reg [ 6:0] buf_idx;       // word index (in-buf loads) or byte index (wt-buf loads)
    reg [ 5:0] mac_idx;       // word index during MAC phase
    reg signed [31:0] acc;
    reg signed [31:0] bias_val;   // held across all w_pos for current c_pos
    reg signed [ 7:0] result_byte;
    reg [ 4:0] cur_shift;     // per-channel shift (held across w_pos)
    reg [ 1:0] r_shift_lane;  // byte lane for current shift array element

    // AHB pipeline wait flag
    reg        r_wait;

    wire start_pulse = psel && penable && pwrite && (paddr[5:2] == 4'd0) && pwdata[0];

    // MAC datapath (combinational)
    wire signed [ 7:0] i0 = $signed(in_buf[mac_idx][ 7: 0]);
    wire signed [ 7:0] i1 = $signed(in_buf[mac_idx][15: 8]);
    wire signed [ 7:0] i2 = $signed(in_buf[mac_idx][23:16]);
    wire signed [ 7:0] i3 = $signed(in_buf[mac_idx][31:24]);
    wire signed [ 7:0] w0 = $signed(wt_buf[mac_idx][ 7: 0]);
    wire signed [ 7:0] w1 = $signed(wt_buf[mac_idx][15: 8]);
    wire signed [ 7:0] w2 = $signed(wt_buf[mac_idx][23:16]);
    wire signed [ 7:0] w3 = $signed(wt_buf[mac_idx][31:24]);
    wire signed [15:0] p0 = i0 * w0;
    wire signed [15:0] p1 = i1 * w1;
    wire signed [15:0] p2 = i2 * w2;
    wire signed [15:0] p3 = i3 * w3;
    wire signed [31:0] mac4 = {{16{p0[15]}},p0} + {{16{p1[15]}},p1}
                             + {{16{p2[15]}},p2} + {{16{p3[15]}},p3};

    // Shift+clip using per-channel cur_shift
    wire signed [31:0] shifted = (acc + bias_val) >>> cur_shift;
    wire signed [ 7:0] clipped = (shifted > 32'sd127)  ? 8'sd127  :
                                 (shifted < -32'sd128) ? -8'sd128 :
                                 shifted[7:0];

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state     <= S_IDLE;
            r_busy    <= 1'b0; r_done <= 1'b0;
            htrans    <= HTRANS_IDLE; hwrite <= 1'b0;
            hburst    <= HBURST_SINGLE; hsize <= HSIZE_WORD;
            haddr     <= 32'b0; hwdata <= 32'b0;
            w_pos     <= 16'b0; c_pos <= 8'b0;
            buf_idx   <= 7'b0; mac_idx <= 6'b0;
            r_wt_lane0 <= 2'b0; r_wait <= 1'b0;
            acc       <= 32'b0; bias_val <= 32'b0;
            cur_shift <= 5'b0; r_shift_lane <= 2'b0;
            patch_bytes <= 16'b0; patch_words <= 6'b0;
            w_out_cnt   <= 16'b0; tail_bytes <= 2'b0;
        end else begin
            case (state)

            // -----------------------------------------------------------------
            S_IDLE: begin
                htrans <= HTRANS_IDLE;
                if (start_pulse) begin
                    r_done <= 1'b0;
                    r_busy <= 1'b1;
                    state  <= S_INIT;
`ifdef ACCEL_DEBUG
                    $display("[ACCEL] START c_in=%0d c_out=%0d k_w=%0d stride=%0d w_in=%0d src=%08x wt=%08x dst=%08x bs=%08x sh=%08x",
                        r_c_in, r_c_out, r_k_w, r_stride, r_w_in,
                        r_src_addr, r_wt_addr, r_dst_addr, r_bs_addr, r_shift_addr);
`endif
                end
            end

            // -----------------------------------------------------------------
            S_INIT: begin
                patch_bytes <= r_k_w * r_c_in;
                patch_words <= 6'(32'({2'b0, r_k_w * r_c_in}) + 32'd3) >> 2;
                w_out_cnt   <= 16'((32'({16'b0, r_w_in}) - 32'({8'b0, r_k_w})) / 32'({8'b0, r_stride}) + 32'd1);
                tail_bytes  <= 2'({2'b0, r_k_w} * {2'b0, r_c_in});
                w_pos <= 16'b0;
                c_pos <= 8'b0;
                state <= S_WT_ADDR;   // begin outer loop: load weights for c_pos=0
            end

            // -----------------------------------------------------------------
            // Weight reads: HSIZE_BYTE burst for correct unaligned filter offsets.
            // Called once per c_pos (outer loop), weights reused for all w_pos.
            S_WT_ADDR: begin
                buf_idx    <= 7'b0;
                r_wt_lane0 <= w_wt_addr[1:0];
                haddr      <= w_wt_addr;
                htrans     <= HTRANS_NONSEQ;
                hburst     <= HBURST_INCR;
                hsize      <= HSIZE_BYTE;
                hwrite     <= 1'b0;
                r_wait     <= 1'b1;
                wt_buf[patch_words - 1] <= 32'b0;  // pre-zero last word
                state      <= S_WT_DATA;
            end

            S_WT_DATA: begin
                if (hready) begin
                    if (r_wait) begin
                        haddr  <= haddr + 32'd1;
                        htrans <= HTRANS_SEQ;
                        r_wait <= 1'b0;
                    end else begin
                        begin : wt_pack
                            reg [1:0] cur_lane;
                            reg [7:0] rdbyte;
                            cur_lane = r_wt_lane0 + buf_idx[1:0];
                            case (cur_lane)
                                2'b00: rdbyte = hrdata[ 7: 0];
                                2'b01: rdbyte = hrdata[15: 8];
                                2'b10: rdbyte = hrdata[23:16];
                                2'b11: rdbyte = hrdata[31:24];
                             endcase
                              case (buf_idx[1:0])
                                  2'b00: wt_buf[buf_idx[5:0]][ 7: 0] <= rdbyte;
                                  2'b01: wt_buf[buf_idx[5:0]][15: 8] <= rdbyte;
                                  2'b10: wt_buf[buf_idx[5:0]][23:16] <= rdbyte;
                                  2'b11: wt_buf[buf_idx[5:0]][31:24] <= rdbyte;
                             endcase
    `ifdef ACCEL_DEBUG
                        if (w_pos == 0 && c_pos < 2 && buf_idx < 4)
                                $display("[WT] c_pos=%0d byte[%0d]=0x%02x lane=%0d hrdata=0x%08x",
                                    c_pos, buf_idx, rdbyte, cur_lane, hrdata);
`endif
                        end
                        if (buf_idx == 7'(16'(patch_bytes) - 16'd1)) begin
                            htrans <= HTRANS_IDLE;
                            state  <= S_BIAS_ADDR;  // load bias for this c_pos
                        end else begin
                            buf_idx <= buf_idx + 7'd1;
                            haddr   <= haddr + 32'd1;
                            htrans  <= HTRANS_SEQ;
                        end
                    end
                end
            end

            // -----------------------------------------------------------------
            // Bias read (SRAM, single WORD read) — once per c_pos.
            S_BIAS_ADDR: begin
                haddr  <= r_bs_addr + (c_pos * 4);
                htrans <= HTRANS_NONSEQ;
                hburst <= HBURST_SINGLE;
                hsize  <= HSIZE_WORD;
                hwrite <= 1'b0;
                r_wait <= 1'b1;
                state  <= S_BIAS_DATA;
            end

            S_BIAS_DATA: begin
                if (hready) begin
                    if (r_wait) begin
                        r_wait <= 1'b0;
                    end else begin
                        bias_val <= $signed(hrdata);
                        haddr  <= r_shift_addr + {24'h0, c_pos};
                        htrans <= HTRANS_NONSEQ;
                        hburst <= HBURST_SINGLE;
                        hsize  <= HSIZE_BYTE;
                        hwrite <= 1'b0;
                        r_shift_lane <= r_shift_addr[1:0] + c_pos[1:0];
                        r_wait <= 1'b1;
                        state  <= S_SHIFT_DATA;
                    end
                end
            end

            // -----------------------------------------------------------------
            // Shift byte read — once per c_pos.
            S_SHIFT_DATA: begin
                if (hready) begin
                    if (r_wait) begin
                        r_wait <= 1'b0;
                    end else begin
                        case (r_shift_lane)
                            2'b00: cur_shift <= hrdata[ 4: 0];
                            2'b01: cur_shift <= hrdata[12: 8];
                            2'b10: cur_shift <= hrdata[20:16];
                            2'b11: cur_shift <= hrdata[28:24];
                        endcase
                        htrans <= HTRANS_IDLE;
                        // Begin inner loop: process all w_pos with these weights
                        acc   <= 32'b0;
                        state <= S_IN_ADDR;
                    end
                end
            end

            // -----------------------------------------------------------------
            // Input patch read (SRAM, HSIZE_WORD burst) — once per w_pos.
            S_IN_ADDR: begin
                buf_idx <= 7'b0;
                haddr   <= r_src_addr + (w_pos * r_stride * r_c_in) + 0;
                htrans  <= HTRANS_NONSEQ;
                hburst  <= HBURST_INCR;
                hsize   <= HSIZE_WORD;
                hwrite  <= 1'b0;
                r_wait  <= 1'b1;
                state   <= S_IN_DATA;
            end

            S_IN_DATA: begin
                if (hready) begin
                    if (r_wait) begin
                        haddr  <= haddr + 32'd4;
                        htrans <= HTRANS_SEQ;
                        r_wait <= 1'b0;
                    end else begin
                        if (buf_idx == 7'(6'(patch_words) - 6'd1)) begin
                            case (tail_bytes)
                                2'b01: in_buf[buf_idx[5:0]] <= {24'b0, hrdata[ 7:0]};
                                2'b10: in_buf[buf_idx[5:0]] <= {16'b0, hrdata[15:0]};
                                2'b11: in_buf[buf_idx[5:0]] <= { 8'b0, hrdata[23:0]};
                                2'b00: in_buf[buf_idx[5:0]] <= hrdata;
                            endcase
                            htrans  <= HTRANS_IDLE;
                            mac_idx <= 6'b0;
                            state   <= S_MAC;
                        end else begin
                            in_buf[buf_idx[5:0]] <= hrdata;
                            buf_idx <= buf_idx + 7'd1;
                            haddr   <= haddr + 32'd4;
                            htrans  <= HTRANS_SEQ;
                        end
                    end
                end
            end

            // -----------------------------------------------------------------
            S_MAC: begin
                htrans <= HTRANS_IDLE;
                acc    <= acc + mac4;
                if (mac_idx == patch_words - 1) begin
                    state <= S_WRITE_ADDR;
                end else begin
                    mac_idx <= mac_idx + 6'd1;
                end
            end

            // -----------------------------------------------------------------
            // Write 1-byte int8 result to dst[w_pos*C_out + c_pos].
            S_WRITE_ADDR: begin
`ifdef ACCEL_DEBUG
                if (w_pos < 2 && c_pos < 4)
                    $display("[ACCEL] w_pos=%0d c_pos=%0d acc=%0d bias=%0d shift=%0d clipped=%0d",
                        w_pos, c_pos, acc, bias_val, cur_shift, clipped);
`endif
                result_byte <= clipped;
                hwdata <= {4{clipped}};
                haddr  <= r_dst_addr + ({16'b0, w_pos} * {16'b0, r_c_out}) + {24'b0, c_pos};
                htrans <= HTRANS_NONSEQ;
                hburst <= HBURST_SINGLE;
                hsize  <= HSIZE_BYTE;
                hwrite <= 1'b1;
                state  <= S_WRITE_DATA;
            end

            S_WRITE_DATA: begin
                if (hready) begin
                    hwrite <= 1'b0;
                    htrans <= HTRANS_IDLE;
                    state  <= S_ADVANCE;
                end
            end

            // -----------------------------------------------------------------
            // Inner loop (w_pos) advances first; outer loop (c_pos) on rollover.
            S_ADVANCE: begin
                if (w_pos < w_out_cnt - 16'd1) begin
                    // More output positions for this filter — stay in inner loop.
                    w_pos <= w_pos + 16'd1;
                    acc   <= 32'b0;
                    state <= S_IN_ADDR;
                end else begin
                    // All w_pos done for this c_pos.
                    w_pos <= 16'b0;
                    if (c_pos < r_c_out - 8'd1) begin
                        // Next output channel — reload weights, bias, shift.
                        c_pos <= c_pos + 8'd1;
                        acc   <= 32'b0;
                        state <= S_WT_ADDR;
                    end else begin
                        state <= S_DONE;
                    end
                end
            end

            // -----------------------------------------------------------------
            S_DONE: begin
                htrans <= HTRANS_IDLE;
                r_busy <= 1'b0;
                r_done <= 1'b1;
                state  <= S_IDLE;
`ifdef ACCEL_DEBUG
                $display("[ACCEL] DONE");
`endif
            end

            default: state <= S_IDLE;
            endcase
        end
    end

`ifdef ACCEL_DEBUG
    reg [20:0] dbg_ctr;
    always @(posedge clk) begin
        if (!r_busy) dbg_ctr <= 21'b0;
        else begin
            dbg_ctr <= dbg_ctr + 21'd1;
            if (dbg_ctr == 21'd1999999)
                $display("[ACCEL] still busy: state=%0d w_pos=%0d c_pos=%0d buf_idx=%0d mac_idx=%0d haddr=%08x htrans=%0d hready=%0d r_wait=%0d",
                    state, w_pos, c_pos, buf_idx, mac_idx, haddr, htrans, hready, r_wait);
        end
    end
`endif

endmodule
`default_nettype wire
