`ifndef CLK_MHZ
    `define CLK_MHZ 36
`endif

module flash_ctrl_eb #(parameter LW = 256) (
    input  wire             clk,
    input  wire             rst_n,
    input  wire             start,
    output wire             done,
    input  wire [23:0]      A,
    output wire [LW-1:0]    D,
    output wire             csn,
    output wire             sck,
    output wire [3:0]       doe,
    output wire [3:0]       do,
    input  wire [3:0]       di
);
    localparam ENDCNT = 80 + LW/2;
    localparam CNTRW  = $clog2(ENDCNT + 1);

    // Hardcoded for simulation. Change to (30 * `CLK_MHZ) for physical FPGA synthesis
    localparam TRST_CYCLES = 2;

    reg [CNTRW-1:0] cntr;
    wire [CNTRW-2:0] bit_cntr = cntr[CNTRW-1:1];

    wire trans_off = (bit_cntr == 'd8) | (bit_cntr == 'd18) | (cntr == ENDCNT);
    wire trans_on  = (start) | (cntr == 'd19) | (cntr == 'd39);

    reg [15:0] delay_cntr;
    wire in_tRST_delay = (cntr == 'd38) && (delay_cntr < TRST_CYCLES);

    always @(posedge clk or negedge rst_n) begin
        if(!rst_n) delay_cntr <= 0;
        else if (in_tRST_delay) delay_cntr <= delay_cntr + 1;
        else delay_cntr <= 0;
    end

    reg run;
    always @(posedge clk or negedge rst_n)
        if(!rst_n) run <= 1'b0;
        else if(start) run <= 1'b1;
        else if(done) run <= 1'b0;

    reg first;
    always @(posedge clk or negedge rst_n)
        if(!rst_n) first <= 1'b1;
        else if(bit_cntr == 'd40) first <= 1'b0;

    reg csn_reg;
    assign csn = csn_reg;
    always @(posedge clk or negedge rst_n)
        if(!rst_n) csn_reg <= 'b1;
        else if(trans_on) csn_reg <= 1'b0;
        else if(trans_off) csn_reg <= 1'b1;

    always @(posedge clk or negedge rst_n)
        if(!rst_n) cntr <= 'b0;
        else if(start & first) cntr <= 'b0;
        else if(start & ~first) cntr <= 'd56; // CRM bypass
        else if(run & ~in_tRST_delay) begin
            if (~first && cntr == 75) cntr <= 'd80; // Skip dummy bytes in CRM
            else cntr <= cntr + 'b1;
        end

    // --- Glitch-Free Combinational Output ---
    reg [3:0] do_reg;
    always @* begin
        case(bit_cntr)
            // 0x66 (Reset Enable)
            6'd0 : do_reg = 4'b0000; 6'd1 : do_reg = 4'b0001;
            6'd2 : do_reg = 4'b0001; 6'd3 : do_reg = 4'b0000;
            6'd4 : do_reg = 4'b0000; 6'd5 : do_reg = 4'b0001;
            6'd6 : do_reg = 4'b0001; 6'd7 : do_reg = 4'b0000;

            // 0x99 (Reset)
            6'd10: do_reg = 4'b0001; 6'd11: do_reg = 4'b0000;
            6'd12: do_reg = 4'b0000; 6'd13: do_reg = 4'b0001;
            6'd14: do_reg = 4'b0001; 6'd15: do_reg = 4'b0000;
            6'd16: do_reg = 4'b0000; 6'd17: do_reg = 4'b0001;

            // 0xEB (Fast Read Quad I/O)
            6'd20: do_reg = 4'b0001; 6'd21: do_reg = 4'b0001;
            6'd22: do_reg = 4'b0001; 6'd23: do_reg = 4'b0000;
            6'd24: do_reg = 4'b0001; 6'd25: do_reg = 4'b0000;
            6'd26: do_reg = 4'b0001; 6'd27: do_reg = 4'b0001;

            // 4-bit Quad Address Phase
            6'd28: do_reg = A[23:20]; 6'd29: do_reg = A[19:16];
            6'd30: do_reg = A[15:12]; 6'd31: do_reg = A[11:8];
            6'd32: do_reg = A[7:4];   6'd33: do_reg = A[3:0];

            // Mode Byte (A0) for CRM
            6'd34: do_reg = 4'b1010;  6'd35: do_reg = 4'b0000;
            default: do_reg = 4'b0000;
        endcase
    end
    assign do = do_reg;

    // SECURITY: Prevent bus contention on SIO[1] (SO) during 1-bit SPI phases
    assign doe = (bit_cntr > 39) ? 4'h0 :             // Receive Phase: High-Z
                 (bit_cntr >= 28) ? 4'hF :            // Quad Phase: Drive all 4 pins
                 4'b1101;                             // Std SPI Phase: Float SIO[1]

    // --- Synchronous Data Capture ---
    reg [LW-1:0] data;
    reg [7:0] dbyte;

    always @(posedge clk or negedge rst_n) begin
        if(!rst_n) dbyte <= 'b0;
        else if(bit_cntr > 39 && cntr[0] == 1'b1) begin
            case (bit_cntr[0])
                0: dbyte[7:4] <= di;
                1: dbyte[3:0] <= di;
            endcase
        end
    end

    always @(posedge clk or negedge rst_n)
        if(!rst_n) data <= 'b0;
        else if(cntr > 80 && cntr[1:0] == 2'b0)
            data <= {dbyte, data[LW-1:8]};

    assign sck = cntr[0] & ~csn & (cntr > 0) & (cntr != 19) & (cntr != 39);
    assign done = (cntr > ENDCNT);
    assign D = data;
endmodule
