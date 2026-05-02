`timescale 1ns/1ps

module tb_apb_conv1d_layer_accel;
`ifndef USE_PER_OC_SHIFT
`define USE_PER_OC_SHIFT 1
`endif

    localparam integer TB_USE_PER_OC_SHIFT = `USE_PER_OC_SHIFT;

    reg clk;
    reg rst_n;

    reg [31:0] paddr;
    reg psel;
    reg penable;
    reg pwrite;
    reg [31:0] pwdata;
    wire [31:0] prdata;
    wire pready;
    wire pslverr;

    wire start;
    wire busy;
    wire done;
    wire [31:0] input_base;
    wire [31:0] weight_base;
    wire [31:0] bias_base;
    wire [31:0] output_base;
    wire [15:0] input_len;
    wire [15:0] in_ch;
    wire [15:0] out_ch;
    wire [4:0] out_shift;
    wire relu_en;

    wire rd0_en;
    wire [31:0] rd0_addr;
    reg [31:0] rd0_data;
    reg rd0_valid;
    wire rd1_en;
    wire [31:0] rd1_addr;
    reg [31:0] rd1_data;
    reg rd1_valid;
    wire wr_en;
    wire [31:0] wr_addr;
    wire [31:0] wr_data;
    wire [3:0] wr_strb;

    reg [31:0] mem [0:1023];
    integer i;
    integer failures;

    apb_conv1d_layer_accel #(
        .ADDR_W(32),
        .DATA_W(32),
        .MAX_IN_CH(64),
        .MAX_OUT_CH(64)
    ) dut (
        .clk(clk),
        .rst_n(rst_n),
        .paddr(paddr),
        .psel(psel),
        .penable(penable),
        .pwrite(pwrite),
        .pwdata(pwdata),
        .prdata(prdata),
        .pready(pready),
        .pslverr(pslverr),
        .start(start),
        .busy(busy),
        .done(done),
        .input_base(input_base),
        .weight_base(weight_base),
        .bias_base(bias_base),
        .output_base(output_base),
        .input_len(input_len),
        .in_ch(in_ch),
        .out_ch(out_ch),
        .out_shift(out_shift),
        .relu_en(relu_en),
        .rd0_en(rd0_en),
        .rd0_addr(rd0_addr),
        .rd0_data(rd0_data),
        .rd0_valid(rd0_valid),
        .rd1_en(rd1_en),
        .rd1_addr(rd1_addr),
        .rd1_data(rd1_data),
        .rd1_valid(rd1_valid),
        .wr_en(wr_en),
        .wr_addr(wr_addr),
        .wr_data(wr_data),
        .wr_strb(wr_strb)
    );

    initial begin
        clk = 1'b0;
        forever #5 clk = ~clk;
    end

    always @(posedge clk) begin
        rd0_valid <= rd0_en;
        rd0_data <= mem[rd0_addr[31:2]];
        rd1_valid <= rd1_en;
        rd1_data <= mem[rd1_addr[31:2]];

        if (wr_en) begin
            if (wr_strb[0]) mem[wr_addr[31:2]][7:0] <= wr_data[7:0];
            if (wr_strb[1]) mem[wr_addr[31:2]][15:8] <= wr_data[15:8];
            if (wr_strb[2]) mem[wr_addr[31:2]][23:16] <= wr_data[23:16];
            if (wr_strb[3]) mem[wr_addr[31:2]][31:24] <= wr_data[31:24];
        end
    end

    task apb_write;
        input [31:0] addr;
        input [31:0] data;
        begin
            @(posedge clk);
            paddr <= addr;
            pwdata <= data;
            pwrite <= 1'b1;
            psel <= 1'b1;
            penable <= 1'b0;
            @(posedge clk);
            penable <= 1'b1;
            @(posedge clk);
            psel <= 1'b0;
            penable <= 1'b0;
            pwrite <= 1'b0;
        end
    endtask

    task expect_s8;
        input integer lane;
        input signed [7:0] got;
        input signed [7:0] exp;
        begin
            if (got !== exp) begin
                $display("FAIL: output lane %0d got=%0d exp=%0d", lane, got, exp);
                failures = failures + 1;
            end
        end
    endtask

    initial begin
        failures = 0;
        rst_n = 1'b0;
        paddr = 32'd0;
        psel = 1'b0;
        penable = 1'b0;
        pwrite = 1'b0;
        pwdata = 32'd0;
        rd0_data = 32'd0;
        rd0_valid = 1'b0;
        rd1_data = 32'd0;
        rd1_valid = 1'b0;

        for (i = 0; i < 1024; i = i + 1)
            mem[i] = 32'd0;

        // Input: three K positions, four channels, all ones.
        mem[0] = 32'h01010101;
        mem[1] = 32'h01010101;
        mem[2] = 32'h01010101;

        // Weights: four output channels, three K positions, four lanes, all ones.
        for (i = 0; i < 12; i = i + 1)
            mem[(32'd512 >> 2) + i] = 32'h01010101;

        // Bias: zero for all four output channels.
        mem[(32'd2048 >> 2)] = 32'd0;
        mem[(32'd2048 >> 2) + 1] = 32'd0;
        mem[(32'd2048 >> 2) + 2] = 32'd0;
        mem[(32'd2048 >> 2) + 3] = 32'd0;

        repeat (4) @(posedge clk);
        rst_n = 1'b1;

        apb_write(32'h0C, 32'd0);     // INPUT_BASE
        apb_write(32'h10, 32'd512);   // WEIGHT_BASE
        apb_write(32'h14, 32'd2048);  // BIAS_BASE
        apb_write(32'h18, 32'd3072);  // OUTPUT_BASE
        apb_write(32'h1C, 32'd3);     // INPUT_LEN
        apb_write(32'h20, 32'd4);     // IN_CH
        apb_write(32'h24, 32'd4);     // OUT_CH
        apb_write(32'h28, 32'd0);     // scalar shift 0, relu disabled

        apb_write(32'h2C, 32'd0);
        apb_write(32'h30, 32'd0);
        apb_write(32'h2C, 32'd1);
        apb_write(32'h30, 32'd1);
        apb_write(32'h2C, 32'd2);
        apb_write(32'h30, 32'd2);
        apb_write(32'h2C, 32'd3);
        apb_write(32'h30, 32'd3);

        apb_write(32'h04, 32'd1);     // CTRL.start

        for (i = 0; i < 500 && !done; i = i + 1)
            @(posedge clk);

        if (!done) begin
            $display("FAIL: accelerator did not assert done");
            failures = failures + 1;
        end

        // Raw MAC sum is 3 K * 4 lanes = 12. Per-OC shifts: 0, 1, 2, 3.
        // With USE_PER_OC_SHIFT=0, the scalar QUANT shift remains the fallback.
        expect_s8(0, mem[(32'd3072 >> 2)][7:0], 8'sd12);
        if (TB_USE_PER_OC_SHIFT != 0) begin
            expect_s8(1, mem[(32'd3072 >> 2)][15:8], 8'sd6);
            expect_s8(2, mem[(32'd3072 >> 2)][23:16], 8'sd3);
            expect_s8(3, mem[(32'd3072 >> 2)][31:24], 8'sd1);
        end else begin
            expect_s8(1, mem[(32'd3072 >> 2)][15:8], 8'sd12);
            expect_s8(2, mem[(32'd3072 >> 2)][23:16], 8'sd12);
            expect_s8(3, mem[(32'd3072 >> 2)][31:24], 8'sd12);
        end

        if (failures == 0) begin
            $display("PASS: APB per-output-channel shift mapping works");
            $finish;
        end

        $display("FAIL: APB per-output-channel shift failures=%0d", failures);
        $finish_and_return(1);
    end
endmodule
