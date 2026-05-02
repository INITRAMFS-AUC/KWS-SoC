// TODO: Next-Line Prefetching
// TODO: Software Cache Invalidation on firmware update (latent until OTA exists)
// TODO: Program-aware FSM prefetch — far future, since we know the call graph
//
// Critical-Word-First / Early Restart: enabled by `+define+CWF (default
// off in production builds today).  When defined, ro_dmc uses the
// per-word `m_word_done` ready bitmap from flash_ctrl_eb to release
// HREADYOUT to the CPU as soon as the requested word lands, instead of
// waiting for the full line.  See the dhit_cwf logic and the
// rising-edge-latched `fwv` register below.  Validate any change with
//   make -C test/xip run-ro-dmc        # baseline
//   make -C test/xip run-ro-dmc-cwf    # CWF-enabled (tighter bounds)
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

    // Slow Memory Interface.  m_word_done[K] is the per-word ready
    // bitmap from flash_ctrl_eb (level — bits stay high once set,
    // cleared on the next `start`).  Always present in the port list
    // even without CWF so the wrapper / SoC build doesn't have to
    // conditionally rewire; synth prunes the wire when CWF is off.
    input  wire [LW-1:0]    m_data,
    input  wire [LW/32-1:0] m_word_done,
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

    // --- State Machine & Miss Handling ---
    localparam ST_IDLE  = 1'b0;
    localparam ST_FETCH = 1'b1;

    reg        state;
    reg [31:0] miss_addr_reg;
    reg        m_start_reg;

    // The line/tag currently being fetched (latched at miss).  CWF
    // hit path keys off these without needing TAG[NL] to be
    // pre-published; that keeps the full-line hit logic untouched.
    reg [LFW-1:0] fetch_line_reg;
    reg [TFW-1:0] fetch_tag_reg;

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
                        fetch_line_reg  <= aline_no;
                        fetch_tag_reg   <= atag;
                        m_start_reg     <= 1'b1;
                    end
                end
                ST_FETCH: begin
                    m_start_reg <= 1'b0;
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

    // ------------------------------------------------------------------
    // CWF (critical-word-first / early restart)
    // ------------------------------------------------------------------
    // m_word_done from flash_ctrl_eb is a *level* — bits stay high
    // after each word lands and only clear on the next `start`.  The
    // start propagates from ro_dmc through the wrapper FSM (~3 cycles)
    // before flash_ctrl_eb actually clears them, so the first few
    // cycles of ST_FETCH still see the previous fetch's bits.  If we
    // trusted m_word_done directly, the second miss would publish
    // stale data through the CWF hit path before the new fetch
    // started — the TB's stale-bit regression test (case 10) catches
    // exactly this bug.
    //
    // Solution: latch our own per-word valid (`fwv`) that resets on
    // miss entry and OR-accumulates only the 0->1 rising edges of
    // m_word_done.  When fwv is fresh (post-reset and post-flash-clear),
    // every set bit corresponds to a real word arrival of the current
    // fetch, so dhit_cwf is safe.
    //
    // Without `+define+CWF this whole block is a no-op except for an
    // unused fwv register that synth prunes — keeps a single source
    // of truth for the cache module while the perf optimisation is
    // gated.
`ifdef CWF
    localparam NW_LOG = $clog2(LW/32);

    reg  [LW/32-1:0] m_word_done_d;
    wire [LW/32-1:0] m_word_done_rise = m_word_done & ~m_word_done_d;
    reg  [LW/32-1:0] fwv;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            m_word_done_d <= {(LW/32){1'b0}};
            fwv           <= {(LW/32){1'b0}};
        end else begin
            m_word_done_d <= m_word_done;
            if (state == ST_IDLE && cpu_rd && !ahit)
                fwv <= {(LW/32){1'b0}};
            else if (state == ST_FETCH)
                fwv <= fwv | m_word_done_rise;
        end
    end

    // Mid-fetch DATA fill: drive DATA[fetch_line_reg] from m_data
    // every cycle of ST_FETCH.  flash_ctrl_eb writes each byte at its
    // final position as it arrives, so DATA[fetch_line_reg][K*32+:32]
    // becomes correct exactly when fwv[K] is set — never before.
    // After m_done the ST_FETCH branch above re-writes the full
    // m_data into DATA, which is a no-op in steady state but keeps
    // the array authoritative even if the caller tweaks m_data after
    // m_done.
    always @(posedge clk) begin
        if (state == ST_FETCH) begin
            DATA[fetch_line_reg] <= m_data;
        end
    end
`endif

    // ------------------------------------------------------------------
    // Hit logic
    // ------------------------------------------------------------------
    wire ahit_full = (TAG[aline_no] == atag) & VALID[aline_no];
    wire dhit_full = (TAG[dline_no] == dtag) & VALID[dline_no];

`ifdef CWF
    // CWF (early-restart) hit: mid-fetch on this exact line, the tag
    // matches what we latched at the miss, and the requested word's
    // bit is set in fwv (= rising-edge-filtered m_word_done).
    wire [NW_LOG-1:0] aword = cpu_aaddr[OFW-1:2];
    wire [NW_LOG-1:0] dword = cpu_daddr[OFW-1:2];
    wire ahit_cwf = (state == ST_FETCH) & (aline_no == fetch_line_reg) &
                    (atag == fetch_tag_reg) & fwv[aword];
    wire dhit_cwf = (state == ST_FETCH) & (dline_no == fetch_line_reg) &
                    (dtag == fetch_tag_reg) & fwv[dword];
    wire ahit = ahit_full | ahit_cwf;
    wire dhit = dhit_full | dhit_cwf;
`else
    wire ahit = ahit_full;
    wire dhit = dhit_full;
`endif

    assign cpu_ahit = ahit;
    assign cpu_dhit = dhit;
    assign m_start  = m_start_reg;
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
