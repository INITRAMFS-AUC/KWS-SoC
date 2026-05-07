// TODO: Next-Line Prefetching
// TODO: Software Cache Invalidation on firmware update (latent until OTA exists)
// TODO: Program-aware FSM prefetch — far future, since we know the call graph
//
// Critical-Word-First / Early Restart is always compiled in.  ro_dmc
// uses the per-word `m_word_done` ready bitmap from flash_ctrl_eb to
// release HREADYOUT to the CPU as soon as the requested word lands,
// instead of waiting for the full line.  See the dhit_cwf / ahit_cwf
// logic and the rising-edge-latched `fwv` register below.
//
// Set XIP_CWF_DEBUG=1 in the root Makefile to compile in the per-cycle
// MISS / HIT / DONE / TICK / SUM $display lines (Verilog define
// CWF_DEBUG).  Off by default — production builds get the perf logic
// without the trace volume.
//
// Validate any change with
//   make -C test/xip run-ro-dmc        # CWF self-checking TB
module ro_dmc #(parameter LW=32*16, NL=64) (
    input  wire             clk,
    input  wire             rst_n,

    // CPU/Bus Interface
    input  wire             cpu_rd,
    input  wire [31:0]      cpu_aaddr,
    input  wire [31:0]      cpu_daddr,
    // dphase_active from the wrapper.  Used by the FSM to detect a
    // *deferred* miss: when CWF early-restart commits the master, the
    // master may issue a new address phase mid-fetch that cpu_rd
    // can't catch (HREADY drops as soon as the new dhit is 0).  After
    // the current fetch completes, the cache must look at cpu_daddr
    // and kick off the miss for that held data phase, otherwise it
    // deadlocks (cpu_rd stays 0 because HREADY=0; HREADY=0 because no
    // hit; no hit because we never fetched the line).
    input  wire             cpu_dvalid,
    output wire             cpu_ahit,
    output wire             cpu_dhit,
    output wire [31:0]      cpu_data,

    // Slow Memory Interface.  m_word_done[K] is the per-word ready
    // bitmap from flash_ctrl_eb (level — bits stay high once set,
    // cleared on the next `start`).
    input  wire [LW-1:0]    m_data,
    input  wire [LW/32-1:0] m_word_done,
    output wire [31:0]      m_addr,
    output wire             m_start,
    input  wire             m_done,

    // NNoM-aware prefetch hint side-band (gated; see DESIGN.md).
    // When `prefetch_en` = 0 (default), this module behaves byte-
    // for-byte as before — `pf_pending` can never become 1, the
    // victim buffer's `pf_valid` is forced low downstream, and the
    // hit-detection mux for the buffer is AND-gated with
    // `prefetch_en` so it can never fire either.  All flop toggling
    // attributable to the prefetch path is suppressed when the bit
    // is clear.  The 54/54-pass XIP cache TB is run in the
    // prefetch_en=0 default; no TB updates needed.
    input  wire             prefetch_en,
    input  wire [31:0]      prefetch_base,
    input  wire [31:0]      prefetch_len
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

    // ──────────────────────────────────────────────────────────────────
    // NNoM-aware prefetch — single-line victim buffer (MVP).
    // ──────────────────────────────────────────────────────────────────
    // Holds ONE prefetched line, separate from the main DATA[] array.
    // Filled when the FSM is in ST_FETCH for a prefetch (`fetch_is_pf`),
    // never evicted by the main cache (no eviction at all in MVP — the
    // line stays valid until the next prefetch hint replaces it).
    //
    // Flow:
    //   1. `prefetch_en` rises while `prefetch_len > 0`  →  `pf_pending`
    //      latches at 1 with `prefetch_base` captured.
    //   2. Next time the main FSM is in ST_IDLE with no demand miss,
    //      it issues `m_start` for `pf_addr_capture` and sets
    //      `fetch_is_pf = 1` for the duration of the fetch.
    //   3. On `m_done`, the fetched line is committed to the victim
    //      buffer (`pf_data / pf_tag / pf_line / pf_valid`).
    //   4. Demand reads on that line short-circuit through the buffer
    //      via `ahit_pf` / `dhit_pf` — gated by `prefetch_en` so the
    //      buffer is invisible when the user disables prefetch.
    //
    // Prefetch-during-fetch policy: CWF early-restart is suppressed
    // for prefetch fetches.  The mid-fetch `m_data` mirror only writes
    // DATA[] when `fetch_is_pf == 0`, so the main cache never sees
    // the prefetched line.  That's adequate for MVP — prefetch is a
    // "before we need it" operation, the ~150-cycle miss penalty is
    // hidden by inference doing other work in parallel.
    //
    // Demand miss arriving while a prefetch is in flight: the main
    // FSM is busy in ST_FETCH; the demand hits the standard "deferred
    // miss" rescue path after the prefetch completes (one extra ~150-
    // cycle stall — same penalty as if the prefetch hadn't started,
    // but the prefetched line is now warm for subsequent hits).
    //
    // MVP scope: ignores `prefetch_len`, only fetches the FIRST line
    // at `prefetch_base`.  A multi-line walk over `[base, base+len)`
    // is the next refinement.

    reg            pf_pending;        // hint queued, waiting for FSM IDLE
    reg [31:0]     pf_addr_capture;   // address from the most recent hint
    reg            fetch_is_pf;       // current ST_FETCH is for prefetch
    reg            prefetch_en_d;     // rising-edge detect

    reg [LW-1:0]   pf_data;           // single-line victim buffer
    reg [TFW-1:0]  pf_tag;
    reg [LFW-1:0]  pf_line;
    reg            pf_valid;

    // Edge-detect prefetch_en rising with non-zero length.  We don't
    // re-trigger on a level signal because firmware writes the hint
    // once per accel call.  When `prefetch_en` is held high across
    // calls, the firmware can simply pulse it (write 0 then 1) to
    // re-arm; a future refinement can treat any write to BASE/LEN as
    // a re-arm trigger.
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            prefetch_en_d   <= 1'b0;
            pf_pending      <= 1'b0;
            pf_addr_capture <= 32'b0;
        end else begin
            prefetch_en_d <= prefetch_en;
            if (prefetch_en && !prefetch_en_d && (|prefetch_len)) begin
                // Fresh hint — latch the base address and arm.
                pf_pending      <= 1'b1;
                pf_addr_capture <= prefetch_base;
            end else if (state == ST_IDLE && pf_pending && !(cpu_rd && !ahit)
                                          && !(cpu_dvalid && !dhit)) begin
                // The FSM is about to dispatch this prefetch (see the
                // ST_IDLE branch below), so clear pf_pending now.
                pf_pending <= 1'b0;
            end
        end
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state         <= ST_IDLE;
            miss_addr_reg <= 32'b0;
            m_start_reg   <= 1'b0;
            fetch_is_pf   <= 1'b0;
            pf_valid      <= 1'b0;
            pf_data       <= {LW{1'b0}};
            pf_tag        <= {TFW{1'b0}};
            pf_line       <= {LFW{1'b0}};
        end else begin
            case (state)
                ST_IDLE: begin
                    if (cpu_rd && !ahit) begin
                        // Normal path: fresh address phase that misses.
                        state           <= ST_FETCH;
                        miss_addr_reg   <= cpu_aaddr;
                        VALID[aline_no] <= 1'b0;
                        fetch_line_reg  <= aline_no;
                        fetch_tag_reg   <= atag;
                        m_start_reg     <= 1'b1;
                        fetch_is_pf     <= 1'b0;
                    end else if (cpu_dvalid && !dhit) begin
                        // Deferred-miss path (CWF rescue): no fresh
                        // address phase right now, but the data phase
                        // is parked on a line we haven't fetched.
                        // This happens when CWF early-restarts and the
                        // master issues a new address mid-fetch that
                        // the FSM couldn't queue (state was ST_FETCH).
                        // After the current fetch completed, cpu_rd
                        // is held low by HREADY=0 (which depends on
                        // dhit, which depends on having the line).
                        // Without this branch the SoC deadlocks; with
                        // it we treat the held data phase as the next
                        // miss.  Fetches cpu_daddr's line.
                        state             <= ST_FETCH;
                        miss_addr_reg     <= cpu_daddr;
                        VALID[dline_no]   <= 1'b0;
                        fetch_line_reg    <= dline_no;
                        fetch_tag_reg     <= dtag;
                        m_start_reg       <= 1'b1;
                        fetch_is_pf       <= 1'b0;
                    end else if (pf_pending) begin
                        // Prefetch path: lower priority than demand
                        // misses (covered by the two `else if`s above).
                        // Fires only when the FSM is otherwise idle.
                        state             <= ST_FETCH;
                        miss_addr_reg     <= pf_addr_capture;
                        // Don't touch VALID[] / fetch_line_reg /
                        // fetch_tag_reg in the same way as a demand
                        // miss — those drive the main-cache hit logic
                        // and the CWF mid-fetch path.  For prefetch we
                        // suppress CWF entirely, but we still need
                        // fetch_line_reg/fetch_tag_reg to be sensible
                        // values to avoid accidental CWF hits on
                        // unrelated lines (the AND-gate with
                        // !fetch_is_pf takes care of that downstream).
                        fetch_line_reg    <= pf_addr_capture[LFE:LFS];
                        fetch_tag_reg     <= pf_addr_capture[TFE:TFS];
                        m_start_reg       <= 1'b1;
                        fetch_is_pf       <= 1'b1;
                    end
                end
                ST_FETCH: begin
                    m_start_reg <= 1'b0;
                    // CWF mid-fetch fill: mirror m_data into the line
                    // every cycle.  flash_ctrl_eb writes each byte at
                    // its final position as it arrives, so the slot
                    // for word K becomes correct exactly when fwv[K]
                    // is set.  Single writer per DATA slot so the
                    // m_done branch below can leave DATA alone.
                    //
                    // Suppressed for prefetch fetches so prefetched
                    // lines never pollute the main cache (the whole
                    // point of the victim buffer).
                    if (!fetch_is_pf) begin
                        DATA[fetch_line_reg] <= m_data;
                    end
                    if (m_done) begin
                        state <= ST_IDLE;
                        if (fetch_is_pf) begin
                            // Commit the prefetched line into the
                            // victim buffer.  pf_valid becomes 1 here
                            // and remains 1 until the next prefetch
                            // hint overwrites the buffer.  The
                            // downstream hit-detection AND-gate with
                            // `prefetch_en` ensures EN=0 makes this
                            // dead silicon.
                            pf_data  <= m_data;
                            pf_tag   <= miss_addr_reg[TFE:TFS];
                            pf_line  <= miss_addr_reg[LFE:LFS];
                            pf_valid <= 1'b1;
                        end else begin
                            TAG[miss_addr_reg[LFE:LFS]]   <= miss_addr_reg[TFE:TFS];
                            VALID[miss_addr_reg[LFE:LFS]] <= 1'b1;
                        end
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
            // Reset fwv on EITHER miss-entry path — both the normal
            // (cpu_rd && !ahit) and the deferred (cpu_dvalid && !dhit)
            // branches kick off a new fetch, so both must wipe the
            // previous fetch's accumulated bits.  Missing the second
            // case meant CWF returned stale all-1s for the line being
            // fetched and the firmware locked up after a few cycles
            // of executing garbage.
            if (state == ST_IDLE && ((cpu_rd && !ahit) ||
                                     (cpu_dvalid && !dhit)))
                fwv <= {(LW/32){1'b0}};
            else if (state == ST_FETCH)
                fwv <= fwv | m_word_done_rise;
        end
    end

    // ------------------------------------------------------------------
    // Hit logic
    // ------------------------------------------------------------------
    wire ahit_full = (TAG[aline_no] == atag) & VALID[aline_no];
    wire dhit_full = (TAG[dline_no] == dtag) & VALID[dline_no];

    // CWF (early-restart) hit: mid-fetch on this exact line, the tag
    // matches what we latched at the miss, and the requested word's
    // bit is set in fwv (= rising-edge-filtered m_word_done).
    //
    // Suppressed when the in-flight fetch is a prefetch
    // (`fetch_is_pf`): prefetched lines land in pf_data on m_done,
    // not in DATA[], so a mid-fetch CWF hit would return stale
    // DATA[] for that line.  Demand misses that race with a
    // prefetch take the deferred-miss path post-prefetch.
    wire [NW_LOG-1:0] aword = cpu_aaddr[OFW-1:2];
    wire [NW_LOG-1:0] dword = cpu_daddr[OFW-1:2];
    wire ahit_cwf = (state == ST_FETCH) & !fetch_is_pf
                  & (aline_no == fetch_line_reg) & (atag == fetch_tag_reg)
                  & fwv[aword];
    wire dhit_cwf = (state == ST_FETCH) & !fetch_is_pf
                  & (dline_no == fetch_line_reg) & (dtag == fetch_tag_reg)
                  & fwv[dword];

    // NNoM-aware victim-buffer hit.  AND-gated with `prefetch_en` so
    // that when the user clears the enable bit at runtime, the
    // buffer becomes invisible — even if a previously-prefetched
    // line is still sitting in pf_data with pf_valid=1, no demand
    // read can hit it.  This is what makes the "byte-for-byte
    // identical when EN=0" gating contract enforceable.
    wire ahit_pf = prefetch_en & pf_valid
                 & (aline_no == pf_line) & (atag == pf_tag);
    wire dhit_pf = prefetch_en & pf_valid
                 & (dline_no == pf_line) & (dtag == pf_tag);

    wire ahit = ahit_full | ahit_cwf | ahit_pf;
    wire dhit = dhit_full | dhit_cwf | dhit_pf;

    assign cpu_ahit = ahit;
    assign cpu_dhit = dhit;
    assign m_start  = m_start_reg;
    assign m_addr   = miss_addr_reg;

`ifdef CWF_DEBUG
    integer cwf_hits = 0;
    integer total_misses = 0;
    integer dones = 0;
    integer cyc = 0;
    always @(posedge clk) begin
        cyc = cyc + 1;
        if (cpu_rd && !ahit && state == ST_IDLE) begin
            total_misses = total_misses + 1;
            if (total_misses < 30)
                $display("[CWF_DBG MISS #%0d] cyc=%0d aaddr=%h",
                         total_misses, cyc, cpu_aaddr);
        end
        if (dhit_cwf && cpu_rd) begin
            cwf_hits = cwf_hits + 1;
            if (cwf_hits < 30)
                $display("[CWF_DBG HIT  #%0d] cyc=%0d daddr=%h dword=%0d fwv=%h",
                         cwf_hits, cyc, cpu_daddr, dword, fwv);
        end
        if (m_done) begin
            dones = dones + 1;
            if (dones < 30)
                $display("[CWF_DBG DONE #%0d] cyc=%0d fetch_line=%0d",
                         dones, cyc, fetch_line_reg);
        end
        if (dones >= 1 && cyc % 100000 == 0)
            $display("[CWF_DBG TICK] cyc=%0d state=%b cpu_rd=%b cpu_aaddr=%h cpu_dhit=%b",
                     cyc, state, cpu_rd, cpu_aaddr, cpu_dhit);
        // Coarse summary every 1M cycles so we can see how many CWF
        // hits we accumulate per inference (~46M cyc) — useful for
        // understanding why end-to-end speedup is small even when the
        // mechanism works.
        if (cyc % 1000000 == 0)
            $display("[CWF_DBG SUM ] cyc=%0d misses=%0d cwf_hits=%0d dones=%0d",
                     cyc, total_misses, cwf_hits, dones);
    end
`endif

    // --- Read Logic ---
    localparam NW = LW/32;
    wire [31:0] words [NW-1:0];

    // Source the line-wide read from the victim buffer when the
    // prefetch hit fires; otherwise use the main cache as before.
    // dhit_pf is already AND-gated with prefetch_en, so this mux
    // collapses to exactly `DATA[dline_no]` when prefetch is
    // disabled — no extra mux level in synthesis (the constant
    // 0 prunes).
    wire [LW-1:0] data = dhit_pf ? pf_data : DATA[dline_no];
    wire [OFW-3:0] woff = doff[OFW-1:2];

    generate
        genvar gi;
        for(gi=0; gi<NW; gi=gi+1)  begin : GEN_WORDS
            assign words[gi] = data[gi*32+31:gi*32];
        end
    endgenerate

    assign cpu_data = words[woff];

endmodule
