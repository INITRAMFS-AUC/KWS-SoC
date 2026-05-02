`timescale 1ns/1ps

module tb_kws_soc_conv1d_apb_decode;
    localparam CONV1D_ID           = 16'hc000;
    localparam CONV1D_INPUT_BASE   = 16'hc00c;
    localparam CONV1D_WEIGHT_BASE  = 16'hc010;
    localparam CONV1D_BIAS_BASE    = 16'hc014;
    localparam CONV1D_OUTPUT_BASE  = 16'hc018;
    localparam CONV1D_INPUT_LEN    = 16'hc01c;
    localparam CONV1D_IN_CH        = 16'hc020;
    localparam CONV1D_OUT_CH       = 16'hc024;
    localparam CONV1D_QUANT_INDEX  = 16'hc02c;
    localparam CONV1D_SHIFT_DATA   = 16'hc030;
    localparam EXPECTED_ID         = 32'h1234_5678;

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
    reg         conv1d_rd0_valid;
    wire        conv1d_rd1_en;
    wire [31:0] conv1d_rd1_addr;
    reg         conv1d_rd1_valid;
    wire        conv1d_wr_en;
    wire [31:0] conv1d_wr_addr;
    wire [31:0] conv1d_wr_data;
    wire [3:0]  conv1d_wr_strb;

    wire conv1d_mem_stub_unused = conv1d_wr_en
                                | conv1d_wr_strb[0]
                                | ^conv1d_rd0_addr
                                | ^conv1d_rd1_addr
                                | ^conv1d_wr_addr
                                | ^conv1d_wr_data;

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
        .rd0_data (32'd0),
        .rd0_valid(conv1d_rd0_valid),

        .rd1_en   (conv1d_rd1_en),
        .rd1_addr (conv1d_rd1_addr),
        .rd1_data (32'd0),
        .rd1_valid(conv1d_rd1_valid),

        .wr_en  (conv1d_wr_en),
        .wr_addr(conv1d_wr_addr),
        .wr_data(conv1d_wr_data),
        .wr_strb(conv1d_wr_strb)
    );

    always #5 clk = ~clk;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            conv1d_rd0_valid <= 1'b0;
            conv1d_rd1_valid <= 1'b0;
        end else begin
            conv1d_rd0_valid <= conv1d_rd0_en;
            conv1d_rd1_valid <= conv1d_rd1_en;
        end
    end

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

    task expect_read;
        input [15:0] addr;
        input [31:0] expected;
        input [8*24-1:0] name;
        reg [31:0] got;
        begin
            apb_read(addr, got);
            if (got !== expected) begin
                $display("FAIL: %0s got=0x%08x expected=0x%08x", name, got, expected);
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

        repeat (4) @(posedge clk);
        rst_n = 1'b1;
        repeat (2) @(posedge clk);

        expect_read(CONV1D_ID, EXPECTED_ID, "ID");

        apb_write(CONV1D_INPUT_BASE, 32'h2000_0000);
        apb_write(CONV1D_WEIGHT_BASE, 32'h2000_0200);
        apb_write(CONV1D_BIAS_BASE, 32'h2000_0800);
        apb_write(CONV1D_OUTPUT_BASE, 32'h2000_0c00);
        apb_write(CONV1D_INPUT_LEN, 32'd8);
        apb_write(CONV1D_IN_CH, 32'd4);
        apb_write(CONV1D_OUT_CH, 32'd2);
        apb_write(CONV1D_QUANT_INDEX, 32'd1);
        apb_write(CONV1D_SHIFT_DATA, 32'd4);

        expect_read(CONV1D_INPUT_BASE, 32'h2000_0000, "INPUT_BASE");
        expect_read(CONV1D_WEIGHT_BASE, 32'h2000_0200, "WEIGHT_BASE");
        expect_read(CONV1D_BIAS_BASE, 32'h2000_0800, "BIAS_BASE");
        expect_read(CONV1D_OUTPUT_BASE, 32'h2000_0c00, "OUTPUT_BASE");
        expect_read(CONV1D_INPUT_LEN, 32'd8, "INPUT_LEN");
        expect_read(CONV1D_IN_CH, 32'd4, "IN_CH");
        expect_read(CONV1D_OUT_CH, 32'd2, "OUT_CH");
        expect_read(CONV1D_QUANT_INDEX, 32'd1, "QUANT_INDEX");
        expect_read(CONV1D_SHIFT_DATA, 32'd4, "QUANT_SHIFT_DATA");

        $display("PASS: KWS-SoC Conv1D APB decode ID/control smoke works");
        $finish;
    end
endmodule
