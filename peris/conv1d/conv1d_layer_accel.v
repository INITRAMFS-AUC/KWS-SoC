`timescale 1ns/1ps

`ifndef USE_WEIGHT_BUFFER
`define USE_WEIGHT_BUFFER 0
`endif

`ifndef USE_PER_OC_SHIFT
`define USE_PER_OC_SHIFT 1
`endif

module conv1d_layer_accel #(
    parameter integer ADDR_W = 32,
    parameter integer DATA_W = 32,
    parameter integer LANES  = 4,
    parameter integer MAX_IN_CH = 64,
    parameter integer MAX_OUT_CH = 64
)(
    input  wire clk,
    input  wire rst_n,

    input  wire start,
    output reg  busy,
    output reg  done,

    input  wire [ADDR_W-1:0] input_base,
    input  wire [ADDR_W-1:0] weight_base,
    input  wire [ADDR_W-1:0] bias_base,
    input  wire [ADDR_W-1:0] output_base,

    input  wire [15:0] input_len,
    input  wire [15:0] in_ch,
    input  wire [15:0] out_ch,

    input  wire [4:0]  out_shift,
    input  wire        relu_en,
    input  wire        out_shift_load_all,
    input  wire        out_shift_we,
    input  wire [7:0]  out_shift_index,
    input  wire [4:0]  out_shift_data,

    output reg                 rd0_en,
    output reg  [ADDR_W-1:0]   rd0_addr,
    input  wire [DATA_W-1:0]   rd0_data,
    input  wire                rd0_valid,

    output reg                 rd1_en,
    output reg  [ADDR_W-1:0]   rd1_addr,
    input  wire [DATA_W-1:0]   rd1_data,
    input  wire                rd1_valid,

    output reg                 wr_en,
    output reg  [ADDR_W-1:0]   wr_addr,
    output reg  [DATA_W-1:0]   wr_data,
    output reg  [3:0]          wr_strb
);

    localparam K = 3;
    localparam LG_LANES = $clog2(LANES);
    localparam integer USE_WEIGHT_BUFFER_MODE = `USE_WEIGHT_BUFFER;
    localparam integer USE_PER_OC_SHIFT_MODE = `USE_PER_OC_SHIFT;
    localparam integer MAX_CH_GROUPS = (MAX_IN_CH + LANES - 1) / LANES;
    localparam integer WEIGHT_WORDS_PER_OC_MAX = K * MAX_CH_GROUPS;

    // Original (nobuffer) FSM states
    localparam S_IDLE      = 4'd0;
    localparam S_BIAS_REQ  = 4'd1;
    localparam S_BIAS_WAIT = 4'd2;
    localparam S_MAC_RUN   = 4'd3;
    localparam S_WRITE     = 4'd5;
    localparam S_DONE      = 4'd7;

    // Buffered FSM states (oc-outer loop, weight-stationary per oc)
    localparam S_BUF_BIAS_REQ  = 4'd8;
    localparam S_BUF_BIAS_WAIT = 4'd9;
    localparam S_WPRELOAD      = 4'd10;
    localparam S_BUF_MAC_PRE   = 4'd11;
    localparam S_BUF_MAC_RUN   = 4'd12;
    localparam S_BUF_WRITE     = 4'd13;

    reg [3:0] state;
    reg [15:0] t, oc, out_len;
    reg signed [31:0] acc;
    reg signed [31:0] oc_bias;
    reg [15:0] req_count;
    reg [15:0] resp_count;
    reg [15:0] w_req_count;
    reg [15:0] w_resp_count;
    reg signed [31:0] prefetched_bias;
    reg prefetched_bias_valid;
    reg prefetch_pending;
    reg [15:0] prefetched_t;
    reg [15:0] prefetched_oc;

    reg [31:0] weight_buf [0:WEIGHT_WORDS_PER_OC_MAX-1];
    reg [4:0] out_shift_buf [0:MAX_OUT_CH-1];

    reg [ADDR_W-1:0] in_byte_addr;
    reg [ADDR_W-1:0] wt_byte_addr;
    reg [1:0] in_sel;
    reg [1:0] wt_sel;

    wire [15:0] in_ch_groups = (in_ch + (LANES-1)) >> LG_LANES;
    wire [15:0] in_ch_groups_nz = (in_ch_groups == 0) ? 16'd1 : in_ch_groups;
    wire [15:0] total_groups = K * in_ch_groups;

    // MAC indexing (input read schedule)
    wire [15:0] req_k     = req_count / in_ch_groups_nz;
    wire [15:0] req_group = req_count % in_ch_groups_nz;
    wire [15:0] req_ic_base = req_group << LG_LANES;

    wire [15:0] resp_group = resp_count % in_ch_groups_nz;
    wire [15:0] resp_ic_base = resp_group << LG_LANES;

    // Weight preload indexing (rd1 schedule for buffered mode)
    wire [15:0] w_req_k     = w_req_count / in_ch_groups_nz;
    wire [15:0] w_req_group = w_req_count % in_ch_groups_nz;

    wire [ADDR_W-1:0] calc_in_byte_addr = input_base + ((((t + req_k) * in_ch_groups) + req_group) << LG_LANES);
    wire [ADDR_W-1:0] calc_wt_byte_addr = weight_base + (((((oc * K) + req_k) * in_ch_groups) + req_group) << LG_LANES);
    wire [ADDR_W-1:0] calc_w_preload_addr = weight_base + (((((oc * K) + w_req_k) * in_ch_groups) + w_req_group) << LG_LANES);
    wire [ADDR_W-1:0] calc_out_byte_addr = output_base + ((t * out_ch) + oc);

    function signed [7:0] pick_s8;
        input [31:0] word;
        input [1:0] sel;
        begin
            case (sel)
                2'd0: pick_s8 = word[7:0];
                2'd1: pick_s8 = word[15:8];
                2'd2: pick_s8 = word[23:16];
                default: pick_s8 = word[31:24];
            endcase
        end
    endfunction

    function [31:0] put_s8_word;
        input signed [7:0] val;
        input [1:0] sel;
        begin
            case (sel)
                2'd0: put_s8_word = {24'd0, val};
                2'd1: put_s8_word = {16'd0, val, 8'd0};
                2'd2: put_s8_word = {8'd0, val, 16'd0};
                default: put_s8_word = {val, 24'd0};
            endcase
        end
    endfunction

    function [3:0] byte_strobe;
        input [1:0] sel;
        begin
            case (sel)
                2'd0: byte_strobe = 4'b0001;
                2'd1: byte_strobe = 4'b0010;
                2'd2: byte_strobe = 4'b0100;
                default: byte_strobe = 4'b1000;
            endcase
        end
    endfunction

    function signed [7:0] clamp_s8;
        input signed [31:0] x;
        begin
            if (x > 32'sd127) clamp_s8 = 8'sd127;
            else if (x < -32'sd128) clamp_s8 = -8'sd128;
            else clamp_s8 = x[7:0];
        end
    endfunction

    integer shift_i;

    function [4:0] effective_out_shift;
        input [15:0] oc_idx;
        begin
            if (USE_PER_OC_SHIFT_MODE != 0 && oc_idx < MAX_OUT_CH)
                effective_out_shift = out_shift_buf[oc_idx];
            else
                effective_out_shift = out_shift;
        end
    endfunction

    wire signed [31:0] shifted_raw = acc >>> effective_out_shift(oc);
    wire signed [31:0] shifted_relu = (relu_en && shifted_raw < 0) ? 32'sd0 : shifted_raw;
    wire signed [7:0]  out_s8 = clamp_s8(shifted_relu);

    wire signed [7:0] x0_s8 = pick_s8(rd0_data, in_sel);
    wire signed [7:0] x1_s8 = pick_s8(rd0_data, in_sel + 2'd1);
    wire signed [7:0] x2_s8 = pick_s8(rd0_data, in_sel + 2'd2);
    wire signed [7:0] x3_s8 = pick_s8(rd0_data, in_sel + 2'd3);

    // Nobuffer-mode weights come from rd1
    wire signed [7:0] w0_s8 = pick_s8(rd1_data, wt_sel);
    wire signed [7:0] w1_s8 = pick_s8(rd1_data, wt_sel + 2'd1);
    wire signed [7:0] w2_s8 = pick_s8(rd1_data, wt_sel + 2'd2);
    wire signed [7:0] w3_s8 = pick_s8(rd1_data, wt_sel + 2'd3);

    // Buffered-mode weights come from local weight_buf indexed by resp_count
    wire [31:0] buf_weight_word = weight_buf[resp_count];
    wire signed [7:0] bw0_s8 = pick_s8(buf_weight_word, wt_sel);
    wire signed [7:0] bw1_s8 = pick_s8(buf_weight_word, wt_sel + 2'd1);
    wire signed [7:0] bw2_s8 = pick_s8(buf_weight_word, wt_sel + 2'd2);
    wire signed [7:0] bw3_s8 = pick_s8(buf_weight_word, wt_sel + 2'd3);

    wire [15:0] lanes_valid = in_ch - resp_ic_base;
    wire use_l0 = (lanes_valid > 0);
    wire use_l1 = (lanes_valid > 1) && (LANES > 1);
    wire use_l2 = (lanes_valid > 2) && (LANES > 2);
    wire use_l3 = (lanes_valid > 3) && (LANES > 3);

    wire signed [31:0] lane0_prod = use_l0 ? ($signed(x0_s8) * $signed(w0_s8)) : 32'sd0;
    wire signed [31:0] lane1_prod = use_l1 ? ($signed(x1_s8) * $signed(w1_s8)) : 32'sd0;
    wire signed [31:0] lane2_prod = use_l2 ? ($signed(x2_s8) * $signed(w2_s8)) : 32'sd0;
    wire signed [31:0] lane3_prod = use_l3 ? ($signed(x3_s8) * $signed(w3_s8)) : 32'sd0;

    wire signed [31:0] lane_sum = lane0_prod + lane1_prod + lane2_prod + lane3_prod;

    wire signed [31:0] blane0_prod = use_l0 ? ($signed(x0_s8) * $signed(bw0_s8)) : 32'sd0;
    wire signed [31:0] blane1_prod = use_l1 ? ($signed(x1_s8) * $signed(bw1_s8)) : 32'sd0;
    wire signed [31:0] blane2_prod = use_l2 ? ($signed(x2_s8) * $signed(bw2_s8)) : 32'sd0;
    wire signed [31:0] blane3_prod = use_l3 ? ($signed(x3_s8) * $signed(bw3_s8)) : 32'sd0;

    wire signed [31:0] lane_sum_buf = blane0_prod + blane1_prod + blane2_prod + blane3_prod;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state <= S_IDLE;
            busy <= 1'b0;
            done <= 1'b0;
            rd0_en <= 1'b0;
            rd1_en <= 1'b0;
            rd0_addr <= 0;
            rd1_addr <= 0;
            wr_en <= 1'b0;
            wr_addr <= 0;
            wr_data <= 0;
            wr_strb <= 0;
            t <= 0; oc <= 0; out_len <= 0; acc <= 0;
            oc_bias <= 0;
            req_count <= 0;
            resp_count <= 0;
            w_req_count <= 0;
            w_resp_count <= 0;
            prefetched_bias <= 0;
            prefetched_bias_valid <= 1'b0;
            prefetch_pending <= 1'b0;
            prefetched_t <= 0;
            prefetched_oc <= 0;
            in_byte_addr <= 0; wt_byte_addr <= 0; in_sel <= 0; wt_sel <= 0;
            for (shift_i = 0; shift_i < MAX_OUT_CH; shift_i = shift_i + 1) begin
                out_shift_buf[shift_i] <= 5'd0;
            end
        end else begin
            done <= 1'b0;
            rd0_en <= 1'b0;
            rd1_en <= 1'b0;
            wr_en <= 1'b0;
            wr_strb <= 4'b0000;

            if (out_shift_load_all) begin
                for (shift_i = 0; shift_i < MAX_OUT_CH; shift_i = shift_i + 1) begin
                    out_shift_buf[shift_i] <= out_shift_data;
                end
            end

            if (out_shift_we && out_shift_index < MAX_OUT_CH) begin
                out_shift_buf[out_shift_index] <= out_shift_data;
            end

            if (USE_WEIGHT_BUFFER_MODE != 0) begin
                // ============================================================
                // Buffered mode: oc-outer loop, weight-stationary per oc
                // ============================================================
                case (state)
                    S_IDLE: begin
                        busy <= 1'b0;
                        if (start) begin
                            busy <= 1'b1;
                            t <= 0; oc <= 0;
                            out_len <= (input_len >= K) ? (input_len - K + 1'b1) : 16'd0;
                            state <= S_BUF_BIAS_REQ;
                        end
                    end

                    S_BUF_BIAS_REQ: begin
                        if (out_len == 0 || in_ch == 0 || out_ch == 0) begin
                            state <= S_DONE;
                        end else begin
                            rd0_en <= 1'b1;
                            rd0_addr <= bias_base + {14'd0, oc, 2'b00};
                            state <= S_BUF_BIAS_WAIT;
                        end
                    end

                    S_BUF_BIAS_WAIT: begin
                        if (rd0_valid) begin
                            oc_bias <= rd0_data;
                            w_req_count <= 0;
                            w_resp_count <= 0;
                            state <= S_WPRELOAD;
                        end
                    end

                    S_WPRELOAD: begin
                        if (w_req_count < total_groups) begin
                            rd1_en <= 1'b1;
                            rd1_addr <= {calc_w_preload_addr[ADDR_W-1:2], 2'b00};
                            w_req_count <= w_req_count + 1'b1;
                        end
                        if ((w_resp_count < total_groups) && rd1_valid) begin
                            weight_buf[w_resp_count] <= rd1_data;
                            if (w_resp_count + 1'b1 >= total_groups) begin
                                t <= 0;
                                state <= S_BUF_MAC_PRE;
                            end
                            w_resp_count <= w_resp_count + 1'b1;
                        end
                    end

                    S_BUF_MAC_PRE: begin
                        acc <= oc_bias;
                        req_count <= 0;
                        resp_count <= 0;
                        state <= S_BUF_MAC_RUN;
                    end

                    S_BUF_MAC_RUN: begin
                        in_sel <= 2'd0;
                        wt_sel <= 2'd0;

                        if (req_count < total_groups) begin
                            in_byte_addr <= calc_in_byte_addr;
                            rd0_en <= 1'b1;
                            rd0_addr <= {calc_in_byte_addr[ADDR_W-1:2], 2'b00};
                            req_count <= req_count + 1'b1;
                        end

                        if ((resp_count < total_groups) && rd0_valid) begin
                            acc <= acc + lane_sum_buf;
                            if (resp_count + 1'b1 >= total_groups) begin
                                state <= S_BUF_WRITE;
                            end
                            resp_count <= resp_count + 1'b1;
                        end
                    end

                    S_BUF_WRITE: begin
                        wr_en <= 1'b1;
                        wr_addr <= {calc_out_byte_addr[ADDR_W-1:2], 2'b00};
                        wr_data <= put_s8_word(out_s8, calc_out_byte_addr[1:0]);
                        wr_strb <= byte_strobe(calc_out_byte_addr[1:0]);

                        if (t == out_len - 1'b1) begin
                            // Finished all t for this oc; advance oc
                            if (oc == out_ch - 1'b1) begin
                                state <= S_DONE;
                            end else begin
                                oc <= oc + 1'b1;
                                state <= S_BUF_BIAS_REQ;
                            end
                        end else begin
                            // Same oc, advance t and rerun MAC with cached bias and weights
                            t <= t + 1'b1;
                            state <= S_BUF_MAC_PRE;
                        end
                    end

                    S_DONE: begin
                        busy <= 1'b0;
                        done <= 1'b1;
                        state <= S_IDLE;
                    end

                    default: state <= S_IDLE;
                endcase
            end else begin
                // ============================================================
                // Original (nobuffer) FSM, preserved verbatim
                // ============================================================
                case (state)
                    S_IDLE: begin
                        busy <= 1'b0;
                        if (start) begin
                            busy <= 1'b1;
                            t <= 0; oc <= 0;
                            out_len <= (input_len >= K) ? (input_len - K + 1'b1) : 16'd0;
                            prefetched_bias_valid <= 1'b0;
                            prefetch_pending <= 1'b0;
                            state <= S_BIAS_REQ;
                        end
                    end

                    S_BIAS_REQ: begin
                        if (out_len == 0 || in_ch == 0 || out_ch == 0) begin
                            state <= S_DONE;
                        end else if (prefetched_bias_valid && (prefetched_t == t) && (prefetched_oc == oc)) begin
                            acc <= prefetched_bias;
                            prefetched_bias_valid <= 1'b0;
                            req_count <= 0;
                            resp_count <= 0;
                            state <= S_MAC_RUN;
                        end else begin
                            rd0_en <= 1'b1;
                            rd0_addr <= bias_base + {14'd0, oc, 2'b00};
                            state <= S_BIAS_WAIT;
                        end
                    end

                    S_BIAS_WAIT: begin
                        if (prefetched_bias_valid && (prefetched_t == t) && (prefetched_oc == oc)) begin
                            acc <= prefetched_bias;
                            prefetched_bias_valid <= 1'b0;
                            req_count <= 0;
                            resp_count <= 0;
                            state <= S_MAC_RUN;
                        end else if (prefetch_pending && rd0_valid) begin
                            prefetch_pending <= 1'b0;
                            prefetched_bias <= rd0_data;
                            prefetched_bias_valid <= 1'b1;

                            if ((prefetched_t == t) && (prefetched_oc == oc)) begin
                                acc <= rd0_data;
                                prefetched_bias_valid <= 1'b0;
                                req_count <= 0;
                                resp_count <= 0;
                                state <= S_MAC_RUN;
                            end
                        end else if (rd0_valid) begin
                            acc <= rd0_data;
                            req_count <= 0;
                            resp_count <= 0;
                            state <= S_MAC_RUN;
                        end
                    end

                    S_MAC_RUN: begin
                        in_sel <= 2'd0;
                        wt_sel <= 2'd0;

                        if (req_count < total_groups) begin
                            in_byte_addr <= calc_in_byte_addr;
                            wt_byte_addr <= calc_wt_byte_addr;

                            rd0_en <= 1'b1;
                            rd0_addr <= {calc_in_byte_addr[ADDR_W-1:2], 2'b00};

                            rd1_en <= 1'b1;
                            rd1_addr <= {calc_wt_byte_addr[ADDR_W-1:2], 2'b00};

                            req_count <= req_count + 1'b1;
                        end

                        if ((resp_count < total_groups) && rd0_valid && rd1_valid) begin
                            acc <= acc + lane_sum;
                            if (resp_count + 1'b1 >= total_groups) begin
                                state <= S_WRITE;
                            end
                            resp_count <= resp_count + 1'b1;
                        end
                    end

                    S_WRITE: begin
                        wr_en <= 1'b1;
                        wr_addr <= {calc_out_byte_addr[ADDR_W-1:2], 2'b00};
                        wr_data <= put_s8_word(out_s8, calc_out_byte_addr[1:0]);
                        wr_strb <= byte_strobe(calc_out_byte_addr[1:0]);
                        if (oc == out_ch - 1'b1) begin
                            if (t == out_len - 1'b1) begin
                                state <= S_DONE;
                            end else begin
                                t <= t + 1'b1;
                                oc <= 0;

                                rd0_en <= 1'b1;
                                rd0_addr <= bias_base;
                                prefetch_pending <= 1'b1;
                                prefetched_t <= t + 1'b1;
                                prefetched_oc <= 0;

                                state <= S_BIAS_WAIT;
                            end
                        end else begin
                            oc <= oc + 1'b1;

                            rd0_en <= 1'b1;
                            rd0_addr <= bias_base + {14'd0, (oc + 1'b1), 2'b00};
                            prefetch_pending <= 1'b1;
                            prefetched_t <= t;
                            prefetched_oc <= oc + 1'b1;

                            state <= S_BIAS_WAIT;
                        end
                    end

                    S_DONE: begin
                        busy <= 1'b0;
                        done <= 1'b1;
                        state <= S_IDLE;
                    end

                    default: state <= S_IDLE;
                endcase
            end
        end
    end

endmodule
