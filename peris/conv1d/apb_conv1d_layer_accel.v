// APB Register Wrapper for Conv1D Layer Accelerator
`timescale 1ns/1ps

module apb_conv1d_layer_accel #(
    parameter ADDR_W = 32,
    parameter DATA_W = 32,
    parameter integer MAX_IN_CH = 64,
    parameter integer MAX_OUT_CH = 64
)(
    // Clock & Reset
    input  wire                  clk,
    input  wire                  rst_n,

    // APB Interface
    input  wire [31:0]           paddr,
    input  wire                  psel,
    input  wire                  penable,
    input  wire                  pwrite,
    input  wire [31:0]           pwdata,
    output reg  [31:0]           prdata,
    output reg                   pready,
    output reg                   pslverr,

    // Accelerator Control Interface
    output reg                   start,
    output reg                   busy,
    output reg                   done,

    output reg  [ADDR_W-1:0]     input_base,
    output reg  [ADDR_W-1:0]     weight_base,
    output reg  [ADDR_W-1:0]     bias_base,
    output reg  [ADDR_W-1:0]     output_base,

    output reg  [15:0]           input_len,
    output reg  [15:0]           in_ch,
    output reg  [15:0]           out_ch,

    output reg  [4:0]            out_shift,
    output reg                   relu_en,

    // Memory Interface (read/write ports delegated to testbench)
    output wire                  rd0_en,
    output wire [ADDR_W-1:0]     rd0_addr,
    input  wire [DATA_W-1:0]     rd0_data,
    input  wire                  rd0_valid,

    output wire                  rd1_en,
    output wire [ADDR_W-1:0]     rd1_addr,
    input  wire [DATA_W-1:0]     rd1_data,
    input  wire                  rd1_valid,

    output wire                  wr_en,
    output wire [ADDR_W-1:0]     wr_addr,
    output wire [DATA_W-1:0]     wr_data,
    output wire [3:0]            wr_strb,

    // Scratchpad APB loader port (firmware preload/readback)
    // 0x34: SPAD_ADDR  — byte address (auto-inc on WDATA write / RDATA read)
    // 0x38: SPAD_WDATA — write 32-bit word to scratchpad, advance SPAD_ADDR
    // 0x3C: SPAD_RDATA — read 32-bit word from scratchpad, advance SPAD_ADDR
    // 0x40: SPAD_CTRL  — bit[0]=reset SPAD_ADDR to 0
    output wire                  spad_lpb_wr_en,
    output wire [ADDR_W-1:0]     spad_lpb_wr_addr,
    output wire [DATA_W-1:0]     spad_lpb_wr_data,
    output wire [3:0]            spad_lpb_wr_strb,
    output wire [ADDR_W-1:0]     spad_lpb_rd_addr,
    input  wire [DATA_W-1:0]     spad_lpb_rd_data
);

    // APB Register map (memory-mapped)
    // 0x00: ID/VERSION (RO)
    // 0x04: CTRL (WO) - bit[0]=start
    // 0x08: STATUS (RO) - bit[0]=busy, bit[1]=done
    // 0x0C: INPUT_BASE (RW)
    // 0x10: WEIGHT_BASE (RW)
    // 0x14: BIAS_BASE (RW)
    // 0x18: OUTPUT_BASE (RW)
    // 0x1C: INPUT_LEN (RW)
    // 0x20: IN_CH (RW)
    // 0x24: OUT_CH (RW)
    // 0x28: QUANT (RW) - [4:0]=out_shift, [5]=relu_en
    // 0x2C: QUANT_INDEX (RW) - [7:0]=output channel index
    // 0x30: QUANT_SHIFT_DATA (RW) - write [4:0] to selected output channel
    // 0x34: SPAD_ADDR (RW)  - scratchpad byte address (auto-inc on WDATA/RDATA)
    // 0x38: SPAD_WDATA (WO) - write word to scratchpad at SPAD_ADDR, advance by 4
    // 0x3C: SPAD_RDATA (RO) - read word from scratchpad at SPAD_ADDR, advance by 4
    // 0x40: SPAD_CTRL (WO)  - bit[0]=reset SPAD_ADDR to 0

    reg [31:0] id_reg;
    reg [31:0] ctrl_reg;
    reg [7:0] quant_index;
    reg [4:0] quant_shift_data;
    reg out_shift_load_all;
    reg out_shift_we;
    reg [31:0] spad_addr;

    wire core_busy;
    wire core_done;
    wire core_rd0_en;
    wire [ADDR_W-1:0] core_rd0_addr;
    wire core_rd1_en;
    wire [ADDR_W-1:0] core_rd1_addr;
    wire core_wr_en;
    wire [ADDR_W-1:0] core_wr_addr;
    wire [DATA_W-1:0] core_wr_data;
    wire [3:0] core_wr_strb;

    assign rd0_en = core_rd0_en;
    assign rd0_addr = core_rd0_addr;
    assign rd1_en = core_rd1_en;
    assign rd1_addr = core_rd1_addr;
    assign wr_en = core_wr_en;
    assign wr_addr = core_wr_addr;
    assign wr_data = core_wr_data;
    assign wr_strb = core_wr_strb;

    // Scratchpad loader: combinatorial write enable fires during APB access phase
    assign spad_lpb_wr_en   = psel & penable & pwrite & (paddr[7:0] == 8'h38);
    assign spad_lpb_wr_addr = spad_addr;
    assign spad_lpb_wr_data = pwdata;
    assign spad_lpb_wr_strb = 4'hF;
    assign spad_lpb_rd_addr = spad_addr;

    conv1d_layer_accel #(
        .ADDR_W(ADDR_W),
        .DATA_W(DATA_W),
        .LANES(4),
        .MAX_IN_CH(MAX_IN_CH),
        .MAX_OUT_CH(MAX_OUT_CH)
    ) core (
        .clk(clk),
        .rst_n(rst_n),
        .start(start),
        .busy(core_busy),
        .done(core_done),
        .input_base(input_base),
        .weight_base(weight_base),
        .bias_base(bias_base),
        .output_base(output_base),
        .input_len(input_len),
        .in_ch(in_ch),
        .out_ch(out_ch),
        .out_shift(out_shift),
        .relu_en(relu_en),
        .out_shift_load_all(out_shift_load_all),
        .out_shift_we(out_shift_we),
        .out_shift_index(quant_index),
        .out_shift_data(quant_shift_data),
        .rd0_en(core_rd0_en),
        .rd0_addr(core_rd0_addr),
        .rd0_data(rd0_data),
        .rd0_valid(rd0_valid),
        .rd1_en(core_rd1_en),
        .rd1_addr(core_rd1_addr),
        .rd1_data(rd1_data),
        .rd1_valid(rd1_valid),
        .wr_en(core_wr_en),
        .wr_addr(core_wr_addr),
        .wr_data(core_wr_data),
        .wr_strb(core_wr_strb)
    );

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            start <= 1'b0;
            id_reg <= 32'h12345678;  // Example ID
            ctrl_reg <= 32'd0;
            pready <= 1'b1;
            pslverr <= 1'b0;
            busy <= 1'b0;
            done <= 1'b0;

            input_base <= 32'd0;
            weight_base <= 32'd512;
            bias_base <= 32'd2048;
            output_base <= 32'd3072;

            input_len <= 16'd0;
            in_ch <= 16'd0;
            out_ch <= 16'd0;
            out_shift <= 5'd0;
            relu_en <= 1'b0;
            quant_index <= 8'd0;
            quant_shift_data <= 5'd0;
            out_shift_load_all <= 1'b0;
            out_shift_we <= 1'b0;
            spad_addr <= 32'd0;
        end else begin
            // Default: clear single-cycle signals
            start <= 1'b0;
            out_shift_load_all <= 1'b0;
            out_shift_we <= 1'b0;

            busy <= core_busy;
            if (start) begin
                done <= 1'b0;
            end else if (core_done) begin
                done <= 1'b1;
            end
            ctrl_reg[0] <= start;

            // APB transaction (Write)
            if (psel && penable && pwrite) begin
                case (paddr[7:0])
                    8'h04: begin  // CTRL
                        ctrl_reg <= pwdata;
                        if (pwdata[0]) start <= 1'b1;
                    end
                    8'h0C: input_base <= pwdata;
                    8'h10: weight_base <= pwdata;
                    8'h14: bias_base <= pwdata;
                    8'h18: output_base <= pwdata;
                    8'h1C: input_len <= pwdata[15:0];
                    8'h20: in_ch <= pwdata[15:0];
                    8'h24: out_ch <= pwdata[15:0];
                    8'h28: begin
                        out_shift <= pwdata[4:0];
                        relu_en <= pwdata[5];
                        quant_shift_data <= pwdata[4:0];
                        out_shift_load_all <= 1'b1;
                    end
                    8'h2C: quant_index <= pwdata[7:0];
                    8'h30: begin
                        quant_shift_data <= pwdata[4:0];
                        out_shift_we <= 1'b1;
                    end
                    8'h34: spad_addr <= pwdata;
                    8'h38: spad_addr <= spad_addr + 32'd4; // write is combinatorial above
                    8'h3C: ; // SPAD_RDATA is read-only; silent no-op on write
                    8'h40: if (pwdata[0]) spad_addr <= 32'd0;
                    default: pslverr <= 1'b1;
                endcase
            end

            // SPAD_RDATA read auto-increments the scratchpad pointer
            if (psel && penable && !pwrite && paddr[7:0] == 8'h3C)
                spad_addr <= spad_addr + 32'd4;
        end
    end

    // prdata must be valid combinatorially during the APB access phase so that
    // the AHB-to-APB bridge can capture hrdata <= prdata at the same clock edge.
    always @(*) begin
        case (paddr[7:0])
            8'h00: prdata = id_reg;
            8'h04: prdata = ctrl_reg;
            8'h08: prdata = {30'd0, done | core_done, core_busy};
            8'h0C: prdata = input_base;
            8'h10: prdata = weight_base;
            8'h14: prdata = bias_base;
            8'h18: prdata = output_base;
            8'h1C: prdata = {{16{1'b0}}, input_len};
            8'h20: prdata = {{16{1'b0}}, in_ch};
            8'h24: prdata = {{16{1'b0}}, out_ch};
            8'h28: prdata = {{26{1'b0}}, relu_en, out_shift};
            8'h2C: prdata = {{24{1'b0}}, quant_index};
            8'h30: prdata = {{27{1'b0}}, quant_shift_data};
            8'h34: prdata = spad_addr;
            8'h38: prdata = 32'd0;
            8'h3C: prdata = spad_lpb_rd_data;
            8'h40: prdata = 32'd0;
            default: prdata = 32'd0;
        endcase
    end

endmodule
