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
//   --- NNoM-aware XIP-cache prefetch hint (gated, default OFF) ---
//   See peris/xip/DESIGN.md §"NNoM-aware cache" for the motivation +
//   gating contract.  All three regs reset to 0; prefetch_en=0 means
//   the cache wrapper observes the hint signals as inert (it must
//   hold any prefetch logic in reset).  This commit only adds the
//   register surface + side-band outputs; the cache-side consumer
//   lands in a follow-up commit.
//
//   0x24  PREFETCH_CTRL   [0]=PREFETCH_EN.  When 0 (reset default),
//                         all prefetch behaviour is suppressed; the
//                         XIP cache must remain byte-for-byte
//                         identical to a non-NNoM build.
//   0x28  PREFETCH_BASE   start byte address (line-aligned recommended).
//   0x2C  PREFETCH_LEN    bytes to prefetch.  Zero = no work.
//
// Loop order: C_out (outer) x W_out (inner).
// Weights, bias and per-channel shift are loaded ONCE per output channel then
// reused across all output positions, amortising XIP flash read cost.
//
// Speed improvements over baseline:
//   1. Weight WORD reads: when patch_bytes is a multiple of 4 and wt_addr is
//      word-aligned, weights are fetched as 32-bit words (4 bytes/cycle instead
//      of 1). Reduces weight-load cycles by ~4x.
//   2. Ping-pong input buffers + overlapped load/MAC: while MAC executes on the
//      active input buffer, the AHB master pre-fetches the NEXT w_pos's patch
//      into the idle buffer. Steady-state inner-loop time becomes
//      max(load_cycles, MAC_cycles)+write, down from load+MAC+write.
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
    input  wire        hresp,

    // ─── NNoM-aware XIP-cache prefetch hint side-band ──────────────────
    //
    // Drives the optional NNoM-aware prefetch path in
    // `peris/xip/ro_cache.v` (see peris/xip/DESIGN.md §"NNoM-aware
    // cache").  All three signals are *gated by* prefetch_en:
    //   prefetch_en   = PREFETCH_CTRL[0].  Reset 0 → all hint logic
    //                   in the cache must hold in reset; the cache
    //                   sees no functional change.
    //   prefetch_base = byte address to start prefetching at.
    //   prefetch_len  = byte length to prefetch (line-aligned read by
    //                   the cache).  Zero = "no work to do".
    //
    // Today these are just driven from the APB regs (firmware sets
    // them, hint stays static).  A future refinement will fire them
    // automatically from the start-of-call FSM transition (a deferred
    // step — keeps this commit small + keeps APB visibility for
    // debug/A-B testing).
    output wire        prefetch_en,
    output wire [31:0] prefetch_base,
    output wire [31:0] prefetch_len
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

    assign hprot     = 4'b0011;
    assign hmastlock = 1'b0;
    assign pready    = 1'b1;
    assign pslverr   = 1'b0;

    // -------------------------------------------------------------------------
    // Config registers

    reg [31:0] r_src_addr, r_wt_addr, r_dst_addr, r_bs_addr, r_shift_addr;
    reg [ 7:0] r_c_in, r_c_out, r_k_w, r_stride;
    reg [15:0] r_w_in;
    reg [ 4:0] r_shift;
    reg        r_busy, r_done;

    // NNoM-aware XIP-cache prefetch hint regs.  Side-band-only — these
    // do NOT participate in the conv1d datapath.  See module port
    // comment for semantics; gated by r_prefetch_en (reset 0).
    reg        r_prefetch_en;
    reg [31:0] r_prefetch_base;
    reg [31:0] r_prefetch_len;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            r_src_addr   <= 32'h0; r_wt_addr    <= 32'h0;
            r_dst_addr   <= 32'h0; r_bs_addr    <= 32'h0;
            r_shift_addr <= 32'h0;
            r_c_in <= 8'd1; r_c_out <= 8'd1; r_k_w <= 8'd1; r_stride <= 8'd1;
            r_w_in <= 16'd1; r_shift <= 5'd0;
            r_prefetch_en   <= 1'b0;
            r_prefetch_base <= 32'h0;
            r_prefetch_len  <= 32'h0;
        end else if (psel && penable && pwrite) begin
            case (paddr[5:2])
                4'd1:  r_src_addr               <= pwdata;
                4'd2:  r_wt_addr                <= pwdata;
                4'd3:  r_dst_addr               <= pwdata;
                4'd4:  r_bs_addr                <= pwdata;
                4'd5:  {r_stride,r_k_w,r_c_out,r_c_in} <= pwdata;
                4'd6:  r_w_in                   <= pwdata[15:0];
                4'd7:  r_shift                  <= pwdata[4:0];
                4'd8:  r_shift_addr             <= pwdata;
                4'd9:  r_prefetch_en            <= pwdata[0];
                4'd10: r_prefetch_base          <= pwdata;
                4'd11: r_prefetch_len           <= pwdata;
                default: ;
            endcase
        end
    end

    always @(*) begin
        case (paddr[5:2])
            4'd0:  prdata = {22'b0, r_done, r_busy, 8'b0};
            4'd1:  prdata = r_src_addr;
            4'd2:  prdata = r_wt_addr;
            4'd3:  prdata = r_dst_addr;
            4'd4:  prdata = r_bs_addr;
            4'd5:  prdata = {r_stride, r_k_w, r_c_out, r_c_in};
            4'd6:  prdata = {16'b0, r_w_in};
            4'd7:  prdata = {27'b0, r_shift};
            4'd8:  prdata = r_shift_addr;
            4'd9:  prdata = {31'b0, r_prefetch_en};
            4'd10: prdata = r_prefetch_base;
            4'd11: prdata = r_prefetch_len;
            default: prdata = 32'b0;
        endcase
    end

    // Side-band outputs to the XIP cache wrapper.  When the gate
    // (r_prefetch_en) is 0, prefetch_base / prefetch_len are forced
    // low so the cache cannot accidentally interpret a stale
    // configuration as a real hint.  This satisfies the
    // "behaviourally invisible when disabled" contract from
    // peris/xip/DESIGN.md.
    assign prefetch_en   = r_prefetch_en;
    assign prefetch_base = r_prefetch_en ? r_prefetch_base : 32'h0;
    assign prefetch_len  = r_prefetch_en ? r_prefetch_len  : 32'h0;

    // -------------------------------------------------------------------------
    // Internal buffers
    //
    // PING-PONG: in_buf0 / in_buf1 alternate as MAC source and AHB load target.
    //   buf_sel=0 → MAC reads in_buf0, AHB fills in_buf1 (and vice-versa).
    //
    // wt_buf: weight buffer, loaded once per c_pos and reused for all w_pos.

    reg [31:0] in_buf0 [0:63];
    reg [31:0] in_buf1 [0:63];
    reg [31:0] wt_buf  [0:63];

    // -------------------------------------------------------------------------
    // Derived parameters (registered when start fires)

    reg [15:0] patch_bytes;
    reg [ 5:0] patch_words;
    reg [15:0] w_out_cnt;
    reg [ 1:0] tail_bytes;

    reg [ 1:0] r_wt_lane0;
    wire [31:0] w_wt_addr = r_wt_addr + ({24'b0, c_pos} * {16'b0, patch_bytes});

    // -------------------------------------------------------------------------
    // State machine
    //
    // Loop order: C_out outer, W_out inner.
    //   Per c_pos: load weights (S_WT_*), bias (S_BIAS_*), shift (S_SHIFT_*).
    //   First w_pos: load input (S_IN_ADDR/S_IN_DATA).
    //   Steady-state w_pos: S_MAC_OVLP — MAC on active buf + AHB fill of inactive buf.
    //   Last w_pos: S_MAC only (no next load needed).
    //   S_ADVANCE: increment w_pos; on rollover increment c_pos.

    localparam S_IDLE       = 4'd0;
    localparam S_INIT       = 4'd1;
    localparam S_IN_ADDR    = 4'd2;
    localparam S_IN_DATA    = 4'd3;
    localparam S_WT_ADDR    = 4'd4;
    localparam S_WT_DATA    = 4'd5;   // weight BYTE reads (fallback for misaligned/partial)
    localparam S_MAC        = 4'd6;   // MAC only (first/last w_pos, or w_out_cnt==1)
    localparam S_BIAS_ADDR  = 4'd7;
    localparam S_BIAS_DATA  = 4'd8;
    localparam S_WT_DATA_W  = 4'd9;   // weight WORD reads (fast path: aligned, full words)
    localparam S_SHIFT_DATA = 4'd10;
    localparam S_WRITE_ADDR = 4'd11;
    localparam S_WRITE_DATA = 4'd12;
    localparam S_ADVANCE    = 4'd13;
    localparam S_DONE       = 4'd14;
    localparam S_MAC_OVLP   = 4'd15;  // overlapped MAC (active buf) + load (inactive buf)

    reg [ 3:0] state;
    reg [15:0] w_pos;
    reg [ 7:0] c_pos;
    reg [ 6:0] buf_idx;
    reg [ 5:0] mac_idx;
    reg [ 5:0] wt_word_idx;       // word counter for S_WT_DATA_W
    reg signed [31:0] acc;
    reg signed [31:0] bias_val;
    reg signed [ 7:0] result_byte;
    reg [ 4:0] cur_shift;
    reg [ 1:0] r_shift_lane;
    reg        r_wait;

    // Ping-pong control
    reg        buf_sel;   // 0: MAC←buf0, load→buf1;  1: MAC←buf1, load→buf0
    reg        mac_done;  // set when MAC phase completes inside S_MAC_OVLP
    reg        load_done; // set when AHB load phase completes inside S_MAC_OVLP

    wire start_pulse = psel && penable && pwrite && (paddr[5:2] == 4'd0) && pwdata[0];

    // -------------------------------------------------------------------------
    // MAC datapath — reads from active buffer selected by buf_sel

    wire [31:0] mac_in_word = buf_sel ? in_buf1[mac_idx] : in_buf0[mac_idx];
    wire signed [ 7:0] i0 = $signed(mac_in_word[ 7: 0]);
    wire signed [ 7:0] i1 = $signed(mac_in_word[15: 8]);
    wire signed [ 7:0] i2 = $signed(mac_in_word[23:16]);
    wire signed [ 7:0] i3 = $signed(mac_in_word[31:24]);
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

    wire signed [31:0] shifted = (acc + bias_val) >>> cur_shift;
    wire signed [ 7:0] clipped = (shifted > 32'sd127)  ?  8'sd127  :
                                 (shifted < -32'sd128) ? -8'sd128 :
                                 shifted[7:0];

    // -------------------------------------------------------------------------
    // Main FSM

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state      <= S_IDLE;
            r_busy     <= 1'b0; r_done <= 1'b0;
            htrans     <= HTRANS_IDLE; hwrite <= 1'b0;
            hburst     <= HBURST_SINGLE; hsize <= HSIZE_WORD;
            haddr      <= 32'b0; hwdata <= 32'b0;
            w_pos      <= 16'b0; c_pos <= 8'b0;
            buf_idx    <= 7'b0; mac_idx <= 6'b0; wt_word_idx <= 6'b0;
            r_wt_lane0 <= 2'b0; r_wait <= 1'b0;
            acc        <= 32'b0; bias_val <= 32'b0;
            cur_shift  <= 5'b0; r_shift_lane <= 2'b0;
            patch_bytes <= 16'b0; patch_words <= 6'b0;
            w_out_cnt  <= 16'b0; tail_bytes <= 2'b0;
            buf_sel    <= 1'b0;
            mac_done   <= 1'b0; load_done <= 1'b0;
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
                patch_words <= ((r_k_w * r_c_in) + 3) >> 2;
                w_out_cnt   <= (r_w_in - r_k_w) / r_stride + 16'd1;
                tail_bytes  <= (r_k_w * r_c_in) & 2'b11;
                w_pos   <= 16'b0;
                c_pos   <= 8'b0;
                buf_sel <= 1'b0;
                state   <= S_WT_ADDR;
            end

            // -----------------------------------------------------------------
            // Weight reads — fast WORD path when patch_bytes%4==0 and addr aligned.
            // Byte path is the original fallback for misaligned/partial cases.

            S_WT_ADDR: begin
                r_wt_lane0 <= w_wt_addr[1:0];
                haddr      <= w_wt_addr;
                htrans     <= HTRANS_NONSEQ;
                hburst     <= HBURST_INCR;
                hwrite     <= 1'b0;
                r_wait     <= 1'b1;

                if (w_wt_addr[1:0] == 2'b00 && patch_bytes[1:0] == 2'b00) begin
                    // Fast path: WORD reads, patch guaranteed full words, addr aligned
                    hsize       <= HSIZE_WORD;
                    wt_word_idx <= 6'b0;
                    state       <= S_WT_DATA_W;
                end else begin
                    // Fallback: BYTE reads (handles any alignment/length)
                    buf_idx <= 7'b0;
                    hsize   <= HSIZE_BYTE;
                    wt_buf[patch_words - 1] <= 32'b0;  // pre-zero last word
                    state   <= S_WT_DATA;
                end
            end

            // Fast weight load: one wt_buf word per AHB data cycle
            S_WT_DATA_W: begin
                if (hready) begin
                    if (r_wait) begin
                        haddr  <= haddr + 32'd4;
                        htrans <= HTRANS_SEQ;
                        r_wait <= 1'b0;
                    end else begin
                        wt_buf[wt_word_idx] <= hrdata;
                        if (wt_word_idx == patch_words - 6'd1) begin
                            htrans <= HTRANS_IDLE;
                            state  <= S_BIAS_ADDR;
                        end else begin
                            wt_word_idx <= wt_word_idx + 6'd1;
                            haddr       <= haddr + 32'd4;
                            htrans      <= HTRANS_SEQ;
                        end
                    end
                end
            end

            // Fallback weight load: byte-by-byte (original behaviour)
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
                                2'b00: wt_buf[buf_idx[6:2]][ 7: 0] <= rdbyte;
                                2'b01: wt_buf[buf_idx[6:2]][15: 8] <= rdbyte;
                                2'b10: wt_buf[buf_idx[6:2]][23:16] <= rdbyte;
                                2'b11: wt_buf[buf_idx[6:2]][31:24] <= rdbyte;
                            endcase
`ifdef ACCEL_DEBUG
                            if (w_pos == 0 && c_pos < 2 && buf_idx < 4)
                                $display("[WT] c_pos=%0d byte[%0d]=0x%02x lane=%0d hrdata=0x%08x",
                                    c_pos, buf_idx, rdbyte, cur_lane, hrdata);
`endif
                        end
                        if (buf_idx == patch_bytes - 7'd1) begin
                            htrans <= HTRANS_IDLE;
                            state  <= S_BIAS_ADDR;
                        end else begin
                            buf_idx <= buf_idx + 7'd1;
                            haddr   <= haddr + 32'd1;
                            htrans  <= HTRANS_SEQ;
                        end
                    end
                end
            end

            // -----------------------------------------------------------------
            // Bias read (SRAM, single WORD) — once per c_pos.

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
                        acc    <= 32'b0;
                        state  <= S_IN_ADDR;
                    end
                end
            end

            // -----------------------------------------------------------------
            // Input patch read — used only for the FIRST w_pos of each c_pos.
            // Subsequent w_pos patches are pre-fetched inside S_MAC_OVLP.

            S_IN_ADDR: begin
                buf_idx <= 7'b0;
                haddr   <= r_src_addr + (w_pos * {8'b0, r_stride} * {8'b0, r_c_in});
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
                        // Write to active buffer (buf_sel=0 at c_pos start)
                        if (!buf_sel) in_buf0[buf_idx] <= hrdata;
                        else          in_buf1[buf_idx] <= hrdata;

                        if (buf_idx == patch_words - 7'd1) begin
                            mac_idx <= 6'b0;

                            if (w_out_cnt > 16'd1) begin
                                // Pre-load w_pos=1 into the inactive buffer while
                                // we MAC on the just-filled active buffer.
                                haddr   <= r_src_addr + ({8'b0, r_stride} * {8'b0, r_c_in});
                                htrans  <= HTRANS_NONSEQ;
                                hburst  <= HBURST_INCR;
                                hsize   <= HSIZE_WORD;
                                hwrite  <= 1'b0;
                                buf_idx <= 7'b0;
                                r_wait  <= 1'b1;
                                mac_done  <= 1'b0;
                                load_done <= 1'b0;
                                state   <= S_MAC_OVLP;
                            end else begin
                                // Only one output position — just MAC, no preload
                                htrans <= HTRANS_IDLE;
                                state  <= S_MAC;
                            end
                        end else begin
                            buf_idx <= buf_idx + 7'd1;
                            haddr   <= haddr + 32'd4;
                            htrans  <= HTRANS_SEQ;
                        end
                    end
                end
            end

            // -----------------------------------------------------------------
            // S_MAC: plain MAC, no concurrent load.
            // Used for w_out_cnt==1, or the very last w_pos.

            S_MAC: begin
                htrans <= HTRANS_IDLE;
                acc    <= acc + mac4;
                if (mac_idx == patch_words - 6'd1) begin
                    state <= S_WRITE_ADDR;
                end else begin
                    mac_idx <= mac_idx + 6'd1;
                end
            end

            // -----------------------------------------------------------------
            // S_MAC_OVLP: overlapped MAC (active buf) + AHB load (inactive buf).
            //
            // Entered from:
            //   S_IN_DATA  (first w_pos: issues NONSEQ for w_pos=1 before entering)
            //   S_ADVANCE  (subsequent: issues NONSEQ for w_pos+2 before entering)
            //
            // MAC runs every cycle until mac_done.
            // AHB load runs when hready, respecting r_wait, until load_done.
            // Exits to S_WRITE_ADDR one cycle after both mac_done and load_done.

            S_MAC_OVLP: begin
                // ---- MAC side ----
                if (!mac_done) begin
                    acc <= acc + mac4;
                    if (mac_idx == patch_words - 6'd1)
                        mac_done <= 1'b1;
                    else
                        mac_idx <= mac_idx + 6'd1;
                end

                // ---- AHB load side (fills inactive buffer) ----
                if (!load_done && hready) begin
                    if (r_wait) begin
                        haddr  <= haddr + 32'd4;
                        htrans <= HTRANS_SEQ;
                        r_wait <= 1'b0;
                    end else begin
                        // Write hrdata into the INACTIVE buffer (!buf_sel)
                        if (buf_sel) in_buf0[buf_idx] <= hrdata;
                        else         in_buf1[buf_idx] <= hrdata;

                        if (buf_idx == patch_words - 7'd1) begin
                            load_done <= 1'b1;
                            htrans    <= HTRANS_IDLE;
                        end else begin
                            buf_idx <= buf_idx + 7'd1;
                            haddr   <= haddr + 32'd4;
                            htrans  <= HTRANS_SEQ;
                        end
                    end
                end

                // Exit one cycle after both phases complete
                if (mac_done && load_done)
                    state <= S_WRITE_ADDR;
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
                haddr  <= r_dst_addr + (w_pos * {8'b0, r_c_out}) + c_pos;
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
            // S_ADVANCE: increment w_pos; on rollover increment c_pos.
            //
            // For the next inner-loop iteration:
            //   - If w_pos+2 exists: flip buf_sel, issue NONSEQ for w_pos+2 load,
            //     reset MAC counters → S_MAC_OVLP (overlap MAC w_pos+1 + load w_pos+2)
            //   - If w_pos+1 is the last: flip buf_sel, reset MAC counters → S_MAC
            //   - If w_pos was the last: c_pos increment → S_WT_ADDR or S_DONE

            S_ADVANCE: begin
                if (w_pos < w_out_cnt - 16'd1) begin
                    // More w_pos remain for this c_pos
                    w_pos   <= w_pos + 16'd1;
                    acc     <= 32'b0;
                    buf_sel <= !buf_sel;   // loaded buffer becomes active

                    if (w_pos + 16'd1 < w_out_cnt - 16'd1) begin
                        // w_pos+2 exists: issue load now, overlap with MAC on w_pos+1
                        haddr   <= r_src_addr +
                                   ((w_pos + 16'd2) * {8'b0, r_stride} * {8'b0, r_c_in});
                        htrans  <= HTRANS_NONSEQ;
                        hburst  <= HBURST_INCR;
                        hsize   <= HSIZE_WORD;
                        hwrite  <= 1'b0;
                        buf_idx <= 7'b0;
                        r_wait  <= 1'b1;
                        mac_idx <= 6'b0;
                        mac_done  <= 1'b0;
                        load_done <= 1'b0;
                        state   <= S_MAC_OVLP;
                    end else begin
                        // w_pos+1 is the last — just MAC, no preload
                        mac_idx <= 6'b0;
                        state   <= S_MAC;
                    end
                end else begin
                    // All w_pos done for this c_pos
                    w_pos <= 16'b0;
                    if (c_pos < r_c_out - 8'd1) begin
                        c_pos   <= c_pos + 8'd1;
                        acc     <= 32'b0;
                        buf_sel <= 1'b0;   // reset ping-pong for new c_pos
                        state   <= S_WT_ADDR;
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
                $display("[ACCEL] still busy: state=%0d w_pos=%0d c_pos=%0d buf_idx=%0d mac_idx=%0d haddr=%08x htrans=%0d hready=%0d r_wait=%0d buf_sel=%0d mac_done=%0d load_done=%0d",
                    state, w_pos, c_pos, buf_idx, mac_idx, haddr, htrans, hready, r_wait, buf_sel, mac_done, load_done);
        end
    end
`endif

endmodule
`default_nettype wire
