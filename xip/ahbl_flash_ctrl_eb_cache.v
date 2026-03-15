`timescale 1ns / 1ps

module ahbl_flash_ctrl_eb_cache #(parameter LW=256, NL=32) (
    input   wire                HCLK,
    input   wire                HRESETn,
    input   wire                HSEL,
    input   wire [31:0]         HADDR,
    input   wire [1:0]          HTRANS,
    input   wire                HWRITE,
    input   wire                HREADY,
    output  wire                HREADYOUT,
    output  wire [31:0]         HRDATA,
    output  wire                HRESP,      // Added for AHB-Lite Compliance

    // External Interface to Quad I/O
    output wire                 csn,
    output wire                 sck,
    output wire [3:0]           doe,
    output wire [3:0]           do,
    input  wire [3:0]           di
);

    // AHB-Lite OKAY Response
    assign HRESP = 1'b0;

    reg [31:0]   addr_reg;
    reg [2:0]    state;
    reg          start_reg;
    reg [LW-1:0] data_reg;

    // 4-Phase Handshake States
    localparam IDLE       = 3'd0;
    localparam START_CMD  = 3'd1;
    localparam WAIT_LOW   = 3'd2;
    localparam WAIT_HIGH  = 3'd3;
    localparam DONE       = 3'd4;

    wire flash_done;
    wire [LW-1:0] flash_data_bus;

    // Request is valid ONLY for Reads
    wire valid_req = HSEL && HTRANS[1] && !HWRITE && HREADY;

    always @(posedge HCLK or negedge HRESETn) begin
        if (!HRESETn) begin
            state     <= IDLE;
            start_reg <= 1'b0;
            addr_reg  <= 32'b0;
            data_reg  <= {LW{1'b0}};
        end else begin
            case (state)
                IDLE: begin
                    if (valid_req) begin
                        addr_reg  <= HADDR;
                        start_reg <= 1'b1;
                        state     <= START_CMD;
                    end
                end

                START_CMD: begin
                    start_reg <= 1'b0; // Pulse start for exactly 1 cycle
                    state     <= WAIT_LOW;
                end

                WAIT_LOW: begin
                    // Wait for Flash Controller to acknowledge the start
                    // by clearing its 'done' flag
                    if (!flash_done) begin
                        state <= WAIT_HIGH;
                    end
                end

                WAIT_HIGH: begin
                    // Now wait for the actual fetch to finish
                    if (flash_done) begin
                        data_reg <= flash_data_bus;
                        state    <= DONE;
                    end
                end

                DONE: begin
                    // Wait for Master to sample the data
                    if (HREADY) state <= IDLE;
                end

                default: state <= IDLE;
            endcase
        end
    end

    // --- Clean Word Alignment ---
    wire [2:0]  word_idx = addr_reg[4:2];
    assign HRDATA = data_reg[word_idx * 32 +: 32];

    // Stall the bus when not IDLE and not presenting DONE data
    assign HREADYOUT = (state == IDLE) || (state == DONE);

    // Flash Controller
    flash_ctrl_eb #(.LW(LW)) flash_ctrl (
        .clk(HCLK),
        .rst_n(HRESETn),
        .start(start_reg),
        .done(flash_done),
        .A({addr_reg[23:5], 5'b00000}),
        .D(flash_data_bus),
        .csn(csn), .sck(sck), .doe(doe), .do(do), .di(di)
    );

endmodule
