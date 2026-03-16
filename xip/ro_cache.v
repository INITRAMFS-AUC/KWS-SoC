module ro_dmc #(parameter LW=32*16, NL=64) (
    input  wire             clk,
    input  wire             rst_n,

    // CPU/Bus Interface
    input  wire             cpu_rd,
    input  wire [31:0]      cpu_aaddr,
    input  wire [31:0]      cpu_daddr,
    output wire             cpu_hit,
    output wire [31:0]      cpu_data,

    // Slow Memory Interface
    input  wire [LW-1:0]    m_data,
    output wire [31:0]      m_addr,  // Changed to 32 bits to match standard address
    output wire             m_start,
    input  wire             m_done
);

    localparam      LWB = LW/8;
    localparam      LFW = $clog2(NL);
    localparam      OFW = $clog2(LWB);
    localparam      TFW = 32 - LFW - OFW;
    localparam      OFS = 0;
    localparam      LFS = $clog2(LWB);
    localparam      TFS = LFW + OFW;
    localparam      OFE = $clog2(LWB) - 1;
    localparam      LFE = OFE + LFW;
    localparam      TFE = 31;

    reg[LW-1:0]     DATA[NL-1:0];
    reg[TFW-1:0]    TAG[NL-1:0];
    reg             VALID[NL-1:0];

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

    // --- Miss Handling & Update Logic ---
    // Safely latch the address that caused the miss so we update the correct line later
    reg [31:0] miss_addr_reg;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) miss_addr_reg <= 32'b0;
        else if (m_start) miss_addr_reg <= cpu_aaddr;
    end

    wire [LFW-1:0] mline_no = miss_addr_reg[LFE:LFS];
    wire [TFW-1:0] mtag     = miss_addr_reg[TFE:TFS];

    integer i;
    always@(posedge clk, negedge rst_n) begin
        if(!rst_n) begin
            for(i=0; i<NL; i=i+1) begin
                VALID[i] <= 1'b0;
                TAG[i] <= 'b0;
            end
        end else if(m_done) begin
            DATA[mline_no]  <= m_data;
            VALID[mline_no] <= 1'b1;
            TAG[mline_no]   <= mtag;
        end
    end

    assign cpu_hit = cpu_rd ? ahit : dhit;
    assign m_start = cpu_rd & ~ahit;
    assign m_addr  = cpu_aaddr;

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
