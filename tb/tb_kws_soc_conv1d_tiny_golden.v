`timescale 1ns/1ps

module tb_kws_soc_conv1d_tiny_golden;
    localparam CONV1D_CTRL        = 16'hc004;
    localparam CONV1D_STATUS      = 16'hc008;
    localparam CONV1D_INPUT_BASE  = 16'hc00c;
    localparam CONV1D_WEIGHT_BASE = 16'hc010;
    localparam CONV1D_BIAS_BASE   = 16'hc014;
    localparam CONV1D_OUTPUT_BASE = 16'hc018;
    localparam CONV1D_INPUT_LEN   = 16'hc01c;
    localparam CONV1D_IN_CH       = 16'hc020;
    localparam CONV1D_OUT_CH      = 16'hc024;
    localparam CONV1D_QUANT       = 16'hc028;
    localparam CONV1D_QUANT_INDEX = 16'hc02c;
    localparam CONV1D_SHIFT_DATA  = 16'hc030;

    localparam INPUT_BASE  = 32'h0000_0000;
    localparam WEIGHT_BASE = 32'h0000_0100;
    localparam BIAS_BASE   = 32'h0000_0200;
    localparam OUTPUT_BASE = 32'h0000_0300;
    localparam INPUT_LEN   = 8;
    localparam IN_CH       = 4;
    localparam OUT_CH      = 2;
    localparam OUT_LEN     = INPUT_LEN - 3 + 1;

    reg clk;
    reg rst_n;

    reg         bridge_psel;
    reg         bridge_penable;
    reg         bridge_pwrite;
    reg [15:0]  bridge_paddr;
    reg [31:0]  bridge_pwdata;
    wire        bridge_pready;
    wire [31:0] bridge_prdata;
    wire        bridge_pslverr;

    wire        timer_psel;
    wire        i2s_psel;
    wire        uart_psel;
    wire        timer_penable;
    wire        i2s_penable;
    wire        uart_penable;
    wire        timer_pwrite;
    wire        i2s_pwrite;
    wire        uart_pwrite;
    wire [15:0] timer_paddr;
    wire [15:0] i2s_paddr;
    wire [15:0] uart_paddr;
    wire [31:0] timer_pwdata;
    wire [31:0] i2s_pwdata;
    wire [31:0] uart_pwdata;

    wire        conv1d_psel;
    wire        conv1d_penable;
    wire        conv1d_pwrite;
    wire [15:0] conv1d_paddr;
    wire [31:0] conv1d_pwdata;
    wire [31:0] conv1d_prdata;
    wire        conv1d_pready;
    wire        conv1d_pslverr;

    wire        conv1d_rd0_en;
    wire [31:0] conv1d_rd0_addr;
    wire [31:0] conv1d_rd0_data;
    wire        conv1d_rd0_valid;
    wire        conv1d_rd1_en;
    wire [31:0] conv1d_rd1_addr;
    wire [31:0] conv1d_rd1_data;
    wire        conv1d_rd1_valid;
    wire        conv1d_wr_en;
    wire [31:0] conv1d_wr_addr;
    wire [31:0] conv1d_wr_data;
    wire [3:0]  conv1d_wr_strb;

    reg signed [7:0] input_data [0:INPUT_LEN*IN_CH-1];
    reg signed [7:0] weight_data [0:OUT_CH*3*IN_CH-1];
    reg signed [31:0] bias_data [0:OUT_CH-1];
    reg [4:0] shift_data [0:OUT_CH-1];
    reg signed [7:0] expected [0:OUT_LEN*OUT_CH-1];

    integer failures;

    apb_splitter #(
        .N_SLAVES (4),
        .W_ADDR   (16),
        .ADDR_MAP (64'hc000_4000_8000_0000),
        .ADDR_MASK(64'hc000_c000_c000_c000)
    ) dut_splitter (
        .apbs_paddr   (bridge_paddr),
        .apbs_psel    (bridge_psel),
        .apbs_penable (bridge_penable),
        .apbs_pwrite  (bridge_pwrite),
        .apbs_pwdata  (bridge_pwdata),
        .apbs_pready  (bridge_pready),
        .apbs_prdata  (bridge_prdata),
        .apbs_pslverr (bridge_pslverr),

        .apbm_paddr   ({conv1d_paddr, uart_paddr, i2s_paddr, timer_paddr}),
        .apbm_psel    ({conv1d_psel, uart_psel, i2s_psel, timer_psel}),
        .apbm_penable ({conv1d_penable, uart_penable, i2s_penable, timer_penable}),
        .apbm_pwrite  ({conv1d_pwrite, uart_pwrite, i2s_pwrite, timer_pwrite}),
        .apbm_pwdata  ({conv1d_pwdata, uart_pwdata, i2s_pwdata, timer_pwdata}),
        .apbm_pready  ({conv1d_pready, 1'b1, 1'b1, 1'b1}),
        .apbm_prdata  ({conv1d_prdata, 32'd0, 32'd0, 32'd0}),
        .apbm_pslverr ({conv1d_pslverr, 1'b0, 1'b0, 1'b0})
    );

    conv1d_accel_soc_wrapper conv1d_u (
        .pclk   (clk),
        .presetn(rst_n),
        .psel   (conv1d_psel),
        .penable(conv1d_penable),
        .pwrite (conv1d_pwrite),
        .paddr  ({16'd0, conv1d_paddr}),
        .pwdata (conv1d_pwdata),
        .prdata (conv1d_prdata),
        .pready (conv1d_pready),
        .pslverr(conv1d_pslverr),

        .rd0_en   (conv1d_rd0_en),
        .rd0_addr (conv1d_rd0_addr),
        .rd0_data (conv1d_rd0_data),
        .rd0_valid(conv1d_rd0_valid),

        .rd1_en   (conv1d_rd1_en),
        .rd1_addr (conv1d_rd1_addr),
        .rd1_data (conv1d_rd1_data),
        .rd1_valid(conv1d_rd1_valid),

        .wr_en  (conv1d_wr_en),
        .wr_addr(conv1d_wr_addr),
        .wr_data(conv1d_wr_data),
        .wr_strb(conv1d_wr_strb)
    );

    conv1d_scratchpad_mem #(
        .MEM_BYTES(8192)
    ) scratchpad_u (
        .clk      (clk),
        .rst_n    (rst_n),
        .rd0_en   (conv1d_rd0_en),
        .rd0_addr (conv1d_rd0_addr),
        .rd0_data (conv1d_rd0_data),
        .rd0_valid(conv1d_rd0_valid),
        .rd1_en   (conv1d_rd1_en),
        .rd1_addr (conv1d_rd1_addr),
        .rd1_data (conv1d_rd1_data),
        .rd1_valid(conv1d_rd1_valid),
        .wr_en    (conv1d_wr_en),
        .wr_addr  (conv1d_wr_addr),
        .wr_data  (conv1d_wr_data),
        .wr_strb  (conv1d_wr_strb)
    );

    always #5 clk = ~clk;

    function [31:0] pack4;
        input signed [7:0] b0;
        input signed [7:0] b1;
        input signed [7:0] b2;
        input signed [7:0] b3;
        begin
            pack4 = {b3[7:0], b2[7:0], b1[7:0], b0[7:0]};
        end
    endfunction

    function signed [7:0] clamp_s8;
        input signed [31:0] value;
        begin
            if (value > 32'sd127) clamp_s8 = 8'sd127;
            else if (value < -32'sd128) clamp_s8 = -8'sd128;
            else clamp_s8 = value[7:0];
        end
    endfunction

    function signed [7:0] pick_byte;
        input [31:0] word;
        input [1:0] lane;
        begin
            case (lane)
                2'd0: pick_byte = word[7:0];
                2'd1: pick_byte = word[15:8];
                2'd2: pick_byte = word[23:16];
                default: pick_byte = word[31:24];
            endcase
        end
    endfunction

    task apb_write;
        input [15:0] addr;
        input [31:0] data;
        begin
            @(posedge clk);
            bridge_psel <= 1'b1;
            bridge_penable <= 1'b0;
            bridge_pwrite <= 1'b1;
            bridge_paddr <= addr;
            bridge_pwdata <= data;
            @(posedge clk);
            bridge_penable <= 1'b1;
            @(posedge clk);
            bridge_psel <= 1'b0;
            bridge_penable <= 1'b0;
            bridge_pwrite <= 1'b0;
        end
    endtask

    task apb_read;
        input [15:0] addr;
        output [31:0] data;
        begin
            @(posedge clk);
            bridge_psel <= 1'b1;
            bridge_penable <= 1'b0;
            bridge_pwrite <= 1'b0;
            bridge_paddr <= addr;
            @(posedge clk);
            bridge_penable <= 1'b1;
            @(posedge clk);
            @(posedge clk);
            data = bridge_prdata;
            bridge_psel <= 1'b0;
            bridge_penable <= 1'b0;
        end
    endtask

    task init_tensors;
        begin
            input_data[0]  =  1; input_data[1]  =  2; input_data[2]  =  3; input_data[3]  =  4;
            input_data[4]  =  2; input_data[5]  =  1; input_data[6]  =  0; input_data[7]  = -1;
            input_data[8]  = -1; input_data[9]  =  1; input_data[10] =  2; input_data[11] =  0;
            input_data[12] =  3; input_data[13] = -2; input_data[14] =  1; input_data[15] =  2;
            input_data[16] =  0; input_data[17] =  1; input_data[18] = -3; input_data[19] =  2;
            input_data[20] =  1; input_data[21] =  0; input_data[22] =  2; input_data[23] = -2;
            input_data[24] =  2; input_data[25] =  2; input_data[26] = -1; input_data[27] =  1;
            input_data[28] = -2; input_data[29] =  3; input_data[30] =  0; input_data[31] =  1;

            weight_data[0]  =  1; weight_data[1]  =  0; weight_data[2]  = -1; weight_data[3]  =  2;
            weight_data[4]  =  0; weight_data[5]  =  1; weight_data[6]  =  1; weight_data[7]  =  0;
            weight_data[8]  = -1; weight_data[9]  =  2; weight_data[10] =  0; weight_data[11] =  1;
            weight_data[12] =  2; weight_data[13] = -1; weight_data[14] =  0; weight_data[15] =  1;
            weight_data[16] =  1; weight_data[17] =  1; weight_data[18] = -1; weight_data[19] =  0;
            weight_data[20] =  0; weight_data[21] = -2; weight_data[22] =  1; weight_data[23] =  1;

            bias_data[0] = 0;
            bias_data[1] = 1;
            shift_data[0] = 0;
            shift_data[1] = 1;
        end
    endtask

    task compute_expected;
        integer t;
        integer oc;
        integer k;
        integer ic;
        integer acc;
        integer idx;
        begin
            for (t = 0; t < OUT_LEN; t = t + 1) begin
                for (oc = 0; oc < OUT_CH; oc = oc + 1) begin
                    acc = bias_data[oc];
                    for (k = 0; k < 3; k = k + 1) begin
                        for (ic = 0; ic < IN_CH; ic = ic + 1) begin
                            idx = ((oc * 3 + k) * IN_CH) + ic;
                            acc = acc + (input_data[(t + k) * IN_CH + ic] * weight_data[idx]);
                        end
                    end
                    expected[t * OUT_CH + oc] = clamp_s8(acc >>> shift_data[oc]);
                end
            end
        end
    endtask

    task preload_scratchpad;
        integer t;
        integer oc;
        integer k;
        integer base;
        begin
            for (t = 0; t < INPUT_LEN; t = t + 1) begin
                base = t * IN_CH;
                scratchpad_u.write_word(INPUT_BASE + (t * 4),
                                        pack4(input_data[base],
                                              input_data[base + 1],
                                              input_data[base + 2],
                                              input_data[base + 3]));
            end

            for (oc = 0; oc < OUT_CH; oc = oc + 1) begin
                for (k = 0; k < 3; k = k + 1) begin
                    base = ((oc * 3 + k) * IN_CH);
                    scratchpad_u.write_word(WEIGHT_BASE + ((oc * 3 + k) * 4),
                                            pack4(weight_data[base],
                                                  weight_data[base + 1],
                                                  weight_data[base + 2],
                                                  weight_data[base + 3]));
                end
            end

            scratchpad_u.write_word(BIAS_BASE + 0, bias_data[0]);
            scratchpad_u.write_word(BIAS_BASE + 4, bias_data[1]);
        end
    endtask

    task start_and_wait_done;
        integer timeout;
        reg [31:0] status;
        begin
            apb_write(CONV1D_CTRL, 32'd1);
            status = 32'd0;
            for (timeout = 0; timeout < 5000 && status[1] == 1'b0; timeout = timeout + 1) begin
                apb_read(CONV1D_STATUS, status);
            end

            if (status[1] != 1'b1) begin
                $display("FAIL: timeout waiting for Conv1D done, status=0x%08x", status);
                $finish;
            end
        end
    endtask

    task check_outputs;
        integer t;
        integer oc;
        integer byte_addr;
        reg [31:0] word;
        reg signed [7:0] got;
        begin
            failures = 0;
            $display("Tiny Conv1D expected outputs:");
            for (t = 0; t < OUT_LEN; t = t + 1) begin
                $display("  t%0d: oc0=%0d oc1=%0d",
                         t,
                         expected[t * OUT_CH],
                         expected[t * OUT_CH + 1]);
            end

            for (t = 0; t < OUT_LEN; t = t + 1) begin
                for (oc = 0; oc < OUT_CH; oc = oc + 1) begin
                    byte_addr = OUTPUT_BASE + (t * OUT_CH) + oc;
                    scratchpad_u.read_word({byte_addr[31:2], 2'b00}, word);
                    got = pick_byte(word, byte_addr[1:0]);
                    if (got !== expected[t * OUT_CH + oc]) begin
                        $display("FAIL: output t=%0d oc=%0d got=%0d expected=%0d word=0x%08x",
                                 t, oc, got, expected[t * OUT_CH + oc], word);
                        failures = failures + 1;
                    end
                end
            end

            if (failures != 0) begin
                $display("FAIL: tiny Conv1D mismatches=%0d", failures);
                $finish;
            end
        end
    endtask

    initial begin
        clk = 1'b0;
        rst_n = 1'b0;
        bridge_psel = 1'b0;
        bridge_penable = 1'b0;
        bridge_pwrite = 1'b0;
        bridge_paddr = 16'd0;
        bridge_pwdata = 32'd0;
        failures = 0;

        init_tensors();
        compute_expected();
        preload_scratchpad();

        repeat (4) @(posedge clk);
        rst_n = 1'b1;
        repeat (4) @(posedge clk);

        apb_write(CONV1D_INPUT_BASE, INPUT_BASE);
        apb_write(CONV1D_WEIGHT_BASE, WEIGHT_BASE);
        apb_write(CONV1D_BIAS_BASE, BIAS_BASE);
        apb_write(CONV1D_OUTPUT_BASE, OUTPUT_BASE);
        apb_write(CONV1D_INPUT_LEN, INPUT_LEN);
        apb_write(CONV1D_IN_CH, IN_CH);
        apb_write(CONV1D_OUT_CH, OUT_CH);
        apb_write(CONV1D_QUANT, 32'd0);
        apb_write(CONV1D_QUANT_INDEX, 32'd0);
        apb_write(CONV1D_SHIFT_DATA, shift_data[0]);
        apb_write(CONV1D_QUANT_INDEX, 32'd1);
        apb_write(CONV1D_SHIFT_DATA, shift_data[1]);

        start_and_wait_done();
        check_outputs();

        $display("PASS: KWS-SoC Conv1D scratchpad tiny golden test works");
        $finish;
    end
endmodule
