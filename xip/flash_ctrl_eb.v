// TODO: current we have a clk divider of 2, but a very fast clk would mean it would be too fast for flash
// so a param for prescalar and a ODDR for Clock Forwarding is needed
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

    localparam integer TRST_CYCLES = 20 * `CLK_MHZ;
    // --- FSM States ---
    localparam IDLE      = 4'd0,
               CMD_66    = 4'd1,
               WAIT_66   = 4'd2,
               CMD_99    = 4'd3,
               TRST_WAIT = 4'd4,
               CMD_EB    = 4'd5,
               ADDR      = 4'd6,
               MODE      = 4'd7,
               DUMMY     = 4'd8,
               DATA      = 4'd9,
               DONE_WAIT = 4'd10; // Clean SPI Mode 0 termination

    reg [3:0] state, next_state;
    reg [11:0] phase_tick;
    reg is_first;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) is_first <= 1'b1;
        else if (state == DATA && phase_tick == 127) is_first <= 1'b0;
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) state <= IDLE;
        else state <= next_state;
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) phase_tick <= 0;
        else if (state != next_state) phase_tick <= 0;
        else if (state != IDLE) phase_tick <= phase_tick + 1;
    end

    // --- State Transitions ---
    always @* begin
        next_state = state;
        case (state)
            IDLE:      if (start) next_state = is_first ? CMD_66 : ADDR;
            CMD_66:    if (phase_tick == 15) next_state = WAIT_66;
            WAIT_66:   if (phase_tick == 3)  next_state = CMD_99;
            CMD_99:    if (phase_tick == 15) next_state = TRST_WAIT;
            TRST_WAIT: if (phase_tick == TRST_CYCLES - 1)  next_state = CMD_EB;
            CMD_EB:    if (phase_tick == 15) next_state = ADDR;
            ADDR:      if (phase_tick == 11) next_state = MODE;
            MODE:      if (phase_tick == 3)  next_state = DUMMY;

            // FIX: Cold Boot uses 4 dummy clocks (7 ticks). CRM uses 2 dummy clocks (3 ticks).
            DUMMY:     if (phase_tick == (is_first ? 7 : 3)) next_state = DATA;

            DATA:      if (phase_tick == 127) next_state = DONE_WAIT;

            // Allow SCK to fall cleanly and hold CSN high to reset flash pipeline
            DONE_WAIT: if (phase_tick == 3)   next_state = IDLE;
            default:   next_state = IDLE;
        endcase
    end

    // --- Glitch-Free Combinational Output ---
    reg [3:0] do_reg;
    always @* begin
        do_reg = 4'b0000;
        case (state)
            CMD_66: do_reg[0] = (8'h66 >> (7 - phase_tick[3:1])) & 1;
            CMD_99: do_reg[0] = (8'h99 >> (7 - phase_tick[3:1])) & 1;
            CMD_EB: do_reg[0] = (8'hEB >> (7 - phase_tick[3:1])) & 1;

            ADDR: begin
                case (phase_tick[3:1])
                    0: do_reg = A[23:20]; 1: do_reg = A[19:16];
                    2: do_reg = A[15:12]; 3: do_reg = A[11:8];
                    4: do_reg = A[7:4];   5: do_reg = A[3:0];
                    default: do_reg = 0;
                endcase
            end
            MODE: begin
                case (phase_tick[3:1])
                    0: do_reg = 4'hA; 1: do_reg = 4'h0;
                    default: do_reg = 0;
                endcase
            end
            default: do_reg = 4'b0000;
        endcase
    end
    assign do = do_reg;

    assign doe = (state == CMD_66 || state == CMD_99 || state == CMD_EB) ? 4'b1101 :
                 (state == ADDR || state == MODE) ? 4'b1111 : 4'b0000;

    // CSN is pulled high during DONE_WAIT for a clean termination
    wire state_active = (state == CMD_66) || (state == CMD_99) || (state == CMD_EB) ||
                        (state == ADDR) || (state == MODE) || (state == DUMMY) || (state == DATA);
    assign csn = ~state_active;
    assign sck = state_active ? phase_tick[0] : 1'b0;

    // --- Synchronous Data Capture ---
    reg [7:0] dbyte;
    reg [LW-1:0] data_reg;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            dbyte <= 0;
            data_reg <= 0;
        end else if (state == DATA && phase_tick[0] == 1'b1) begin
            if (phase_tick[1] == 1'b0) begin
                dbyte[7:4] <= di;
            end else begin
                data_reg <= {dbyte[7:4], di, data_reg[LW-1:8]};
            end
        end
    end

    // --- Control Handshake ---
    reg done_reg;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) done_reg <= 0;
        else if (start) done_reg <= 0;
        else if (state == DONE_WAIT && phase_tick == 3) done_reg <= 1;
    end

    assign done = done_reg;
    assign D = data_reg;


endmodule
