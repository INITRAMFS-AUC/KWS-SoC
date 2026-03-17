// TODO: Next-Line Prefetching, Critical-Word-First / Early Restart, Software Cache Invalidation on firmware update
module ro_dmc #(parameter LW=32*16, NL=64) (
    input  wire             clk,
    input  wire             rst_n,

    // CPU/Bus Interface
    input  wire             cpu_rd,
    input  wire [31:0]      cpu_aaddr,
    input  wire [31:0]      cpu_daddr,
    output wire             cpu_ahit,
    output wire             cpu_dhit,
    output wire [31:0]      cpu_data,

    // Slow Memory Interface
    input  wire [LW-1:0]    m_data,
    output wire [31:0]      m_addr,
    output wire             m_start,
    input  wire             m_done
);

    localparam      LWB = LW/8;
    localparam      LFW = $clog2(NL);
    localparam      OFW = $clog2(LWB);
    localparam      TFW = 32 - LFW - OFW;
    localparam      LFS = $clog2(LWB);
    localparam      TFS = LFW + OFW;
    localparam      OFE = $clog2(LWB) - 1;
    localparam      LFE = OFE + LFW;
    localparam      TFE = 31;

    reg[LW-1:0]     DATA[NL-1:0];
    reg[TFW-1:0]    TAG[NL-1:0];
    reg             VALID[NL-1:0];

    // --- Safe Initialization for Sim & Synthesis ---
    integer i;
    initial begin
        for(i=0; i<NL; i=i+1) begin
            VALID[i] = 1'b0;
            TAG[i]   = 'b0;
            DATA[i]  = {LW{1'b0}};
        end
    end

    // --- Address Phase Signals ---
    wire [LFW-1:0]  aline_no = cpu_aaddr[LFE:LFS];
    wire [TFW-1:0]  atag     = cpu_aaddr[TFE:TFS];

    // --- Data Phase Signals ---
    wire [LFW-1:0]  dline_no = cpu_daddr[LFE:LFS];
    wire [TFW-1:0]  dtag     = cpu_daddr[TFE:TFS];
    wire [OFW-1:0]  doff     = cpu_daddr[OFW-1:0];

    // Hit Logic
    wire ahit = (TAG[aline_no] == atag) & (VALID[aline_no] == 1'b1);
    wire dhit = (TAG[dline_no] == dtag) & (VALID[dline_no] == 1'b1);

    // --- State Machine & Miss Handling ---
    localparam ST_IDLE  = 1'b0;
    localparam ST_FETCH = 1'b1;

    reg        state;
    reg [31:0] miss_addr_reg;
    reg        m_start_reg;  // NEW: Registered start pulse

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state         <= ST_IDLE;
            miss_addr_reg <= 32'b0;
            m_start_reg   <= 1'b0;
        end else begin
            case (state)
                ST_IDLE: begin
                    if (cpu_rd && !ahit) begin
                        state           <= ST_FETCH;
                        miss_addr_reg   <= cpu_aaddr;
                        VALID[aline_no] <= 1'b0;
                        m_start_reg     <= 1'b1; // Safely latch the start trigger
                    end
                end
                ST_FETCH: begin
                    m_start_reg <= 1'b0; // Instantly clear to create a 1-cycle pulse
                    if (m_done) begin
                        state                         <= ST_IDLE;
                        DATA[miss_addr_reg[LFE:LFS]]  <= m_data;
                        TAG[miss_addr_reg[LFE:LFS]]   <= miss_addr_reg[TFE:TFS];
                        VALID[miss_addr_reg[LFE:LFS]] <= 1'b1;
                    end
                end
            endcase
        end
    end

    assign cpu_ahit = ahit;
    assign cpu_dhit = dhit;
    assign m_start  = m_start_reg; // Driven by flip-flop now
    assign m_addr   = miss_addr_reg;

    // --- Read Logic ---
    localparam NW = LW/32;
    wire [31:0] words [NW-1:0];
    wire [LW-1:0] data = DATA[dline_no];
    wire [OFW-3:0] woff = doff[OFW-1:2];

    generate
        genvar gi;
        for(gi=0; gi<NW; gi=gi+1)  begin : GEN_WORDS
            assign words[gi] = data[gi*32+31:gi*32];
        end
    endgenerate

    assign cpu_data = words[woff];

endmodule
