`timescale 1ns/1ps

module conv1d_scratchpad_mem #(
    parameter integer MEM_BYTES = 8192
)(
    input  wire        clk,
    input  wire        rst_n,

    input  wire        rd0_en,
    input  wire [31:0] rd0_addr,
    output reg  [31:0] rd0_data,
    output reg         rd0_valid,

    input  wire        rd1_en,
    input  wire [31:0] rd1_addr,
    output reg  [31:0] rd1_data,
    output reg         rd1_valid,

    input  wire        wr_en,
    input  wire [31:0] wr_addr,
    input  wire [31:0] wr_data,
    input  wire [3:0]  wr_strb
);
    localparam integer WORDS = MEM_BYTES / 4;
    localparam integer ADDR_BITS = $clog2(MEM_BYTES);

    reg [31:0] mem [0:WORDS-1];

    function integer word_index;
        input [31:0] addr;
        begin
            word_index = addr[ADDR_BITS-1:2];
        end
    endfunction

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            rd0_data <= 32'd0;
            rd1_data <= 32'd0;
            rd0_valid <= 1'b0;
            rd1_valid <= 1'b0;
        end else begin
            rd0_valid <= rd0_en;
            rd1_valid <= rd1_en;

            if (rd0_en)
                rd0_data <= mem[word_index(rd0_addr)];

            if (rd1_en)
                rd1_data <= mem[word_index(rd1_addr)];

            if (wr_en) begin
                if (wr_strb[0]) mem[word_index(wr_addr)][7:0]   <= wr_data[7:0];
                if (wr_strb[1]) mem[word_index(wr_addr)][15:8]  <= wr_data[15:8];
                if (wr_strb[2]) mem[word_index(wr_addr)][23:16] <= wr_data[23:16];
                if (wr_strb[3]) mem[word_index(wr_addr)][31:24] <= wr_data[31:24];
            end
        end
    end

`ifdef SIM
    task write_word;
        input [31:0] addr;
        input [31:0] data;
        begin
            mem[word_index(addr)] = data;
        end
    endtask

    task read_word;
        input [31:0] addr;
        output [31:0] data;
        begin
            data = mem[word_index(addr)];
        end
    endtask
`endif
endmodule
