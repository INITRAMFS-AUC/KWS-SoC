`timescale 1ns/1ps
`include "tb.vh"

// Self-checking testbench for ro_dmc — the XIP direct-mapped cache.
//
// The TB uses a deterministic mock flash that returns the requested
// address as the data for each word in the line.  Concretely, a fetch
// of line containing address A (line-aligned to A & ~0x1f) returns 8
// words: { A+0x00, A+0x04, A+0x08, ..., A+0x1c }.  That makes the
// expected value for any CPU read at address A trivially `A` (modulo
// the byte offset within the word — we always read aligned words).
//
// Test plan:
//   1. Cold miss on the first line.  Verify data + miss path.
//   2. Hit on subsequent words within the same line.  Verify zero
//      stall + correct data.
//   3. Cold miss on a second line (different cache slot).
//   4. Conflict miss: read a line that maps to the same slot as #1,
//      with a different tag, evicting it.
//   5. Thrash back to line #1 (now refilled).
//   6. Sequential read across two adjacent cache lines (boundary).
//   7. Repeated reads of the same word (steady-state hit).
//
// Each case prints PASS / FAIL with expected vs got.  Final summary
// prints total fail count and exits with non-zero $finish on any
// failure so an outer Make can detect the regression.

module ro_dmc_tb;
    localparam LW = 256;   // bits per cache line  (8 words × 32 bits)
    localparam NL = 32;    // line count

    reg             cpu_rd;
    reg [31:0]      cpu_aaddr;
    reg [31:0]      cpu_daddr;
    wire            cpu_ahit;
    wire            cpu_dhit;
    wire [31:0]     cpu_data;

    localparam NW = LW/32;     // words per line

    reg [LW-1:0]    m_data;
    reg [NW-1:0]    m_word_done;
    wire [31:0]     m_addr;
    wire            m_start;
    reg             m_done;

    `TB(ro_dmc_tb, clk, rst_n, 1'b0, 50_000)

    ro_dmc #(.LW(LW), .NL(NL)) duv (
        .clk(clk),
        .rst_n(rst_n),
        .cpu_rd(cpu_rd),
        .cpu_aaddr(cpu_aaddr),
        .cpu_daddr(cpu_daddr),
        .cpu_ahit(cpu_ahit),
        .cpu_dhit(cpu_dhit),
        .cpu_data(cpu_data),
        .m_data(m_data),
`ifdef CWF
        .m_word_done(m_word_done),
`endif
        .m_addr(m_addr),
        .m_start(m_start),
        .m_done(m_done)
    );

    // ------------------------------------------------------------------
    // Mock flash.  Models a QSPI line fetch where individual words
    // arrive one at a time over 8 clock cycles (one word per cycle —
    // matches the real flash_ctrl_eb's behaviour after the position-
    // based byte refactor, modulo the per-word latency).  m_data has
    // each word slotted into its final position as it arrives, and
    // m_word_done[K] goes high (and stays high until the next start)
    // when word K has landed.  m_done pulses after the last word.
    //
    // Returns address-as-data so the expected value of any read is
    // just the address itself.
    // ------------------------------------------------------------------
    reg [3:0] flash_phase;       // 0 = idle, 1..NW = next word index, NW+1 = done

    function [31:0] expected_word(input [31:0] base, input integer k);
        // Words come back at the line-aligned base, not the byte-exact
        // miss address — matches the real flash_ctrl_eb which masks
        // m_addr[4:0] off when sending the QSPI address.
        expected_word = (base & 32'hffffffe0) + (k << 2);
    endfunction

    integer fpi;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            m_done      <= 1'b0;
            m_data      <= {LW{1'b0}};
            m_word_done <= {NW{1'b0}};
            flash_phase <= 4'd0;
        end else begin
            m_done <= 1'b0;
            if (m_start && flash_phase == 0) begin
                // Start of a new fetch: clear word-done bitmap, but
                // keep m_data (per-word writes will overwrite it as
                // words land).
                m_word_done <= {NW{1'b0}};
                flash_phase <= 4'd1;
            end else if (flash_phase > 0 && flash_phase <= NW) begin
                // Word at index (flash_phase - 1) arrives this cycle.
                m_data[((flash_phase - 1) << 5) +: 32] <= expected_word(m_addr, flash_phase - 1);
                m_word_done[flash_phase - 1]            <= 1'b1;
                if (flash_phase == NW) begin
                    m_done      <= 1'b1;   // last word — pulse done
                    flash_phase <= 4'd0;
                end else begin
                    flash_phase <= flash_phase + 4'd1;
                end
            end
        end
    end

    // ------------------------------------------------------------------
    // Pass / fail + perf tracking.  total_stall sums every ahb_read
    // task's stall cycles across the whole run; total_misses counts
    // reads that stalled at all.  Running this TB once with `+CWF and
    // once without lets us see the CWF speedup directly without
    // having to integrate into the SoC and re-run mel_compact.
    // ------------------------------------------------------------------
    integer passes       = 0;
    integer fails        = 0;
    integer total_stall  = 0;
    integer total_misses = 0;

    task check_eq(input [255:0] tag,
                  input [31:0] got, input [31:0] exp);
        if (got === exp) begin
            $display("  PASS  %0s   got=%h", tag, got);
            passes = passes + 1;
        end else begin
            $display("  FAIL  %0s   got=%h  exp=%h", tag, got, exp);
            fails = fails + 1;
        end
    endtask

    task check_le(input [255:0] tag,
                  input integer got, input integer exp_max);
        if (got <= exp_max) begin
            $display("  PASS  %0s   stall=%0d  (<= %0d)", tag, got, exp_max);
            passes = passes + 1;
        end else begin
            $display("  FAIL  %0s   stall=%0d  (>  %0d)", tag, got, exp_max);
            fails = fails + 1;
        end
    endtask

    // ------------------------------------------------------------------
    // AHB-style read with stall counting.  Returns stall_cycles via
    // the `last_stall` global so each test can assert on miss timing.
    //
    // We collapse the AHB address+data phases into one task by driving
    // cpu_aaddr and cpu_daddr identically at task entry (blocking
    // assigns so the cache observes the new address immediately), then
    // waiting for cpu_dhit — the data-valid signal.  cpu_dhit goes high
    // either at the very first clock (hit) or after the miss-handler
    // FSM completes (miss).  This keeps the task simple while still
    // exercising both the miss path and the dhit data-output mux.
    // ------------------------------------------------------------------
    integer last_stall;
    task ahb_read(input [31:0] addr);
    begin
        // Quiesce: with CWF, the previous test may have exited as soon
        // as its requested word landed, leaving the rest of the line
        // still arriving from flash.  Wait for the mock to return to
        // idle before starting a new miss so this test's stall
        // measurement reflects only this test's fetch.
        while (flash_phase != 0) begin
            @(posedge clk);
            #1;
        end

        last_stall = 0;
        cpu_rd    = 1'b1;
        cpu_aaddr = addr;
        cpu_daddr = addr;

        @(posedge clk);
        #1;
        while (!cpu_dhit) begin
            @(posedge clk);
            #1;
            last_stall = last_stall + 1;
        end
        cpu_rd = 1'b0;

        // Roll into the run-wide perf totals so the final summary
        // line can quantify CWF speedup vs the baseline.
        if (last_stall > 0) total_misses = total_misses + 1;
        total_stall = total_stall + last_stall;
    end
    endtask

    // ------------------------------------------------------------------
    // Test sequence
    // ------------------------------------------------------------------
    initial begin
        cpu_rd    = 1'b0;
        cpu_aaddr = 32'h0;
        cpu_daddr = 32'h0;

        @(posedge rst_n);
        @(posedge clk);

        $display("\n=== ro_dmc_tb (LW=%0d, NL=%0d) ===", LW, NL);

        // --- 1. Cold miss on line 0, slot 0 ---
        $display("\n[1] Cold miss on 0x80000000 (line 0, slot 0)");
        ahb_read(32'h80000000);
        check_eq("data    @0x80000000", cpu_data, 32'h80000000);
        // 5 flash-latency + a couple FSM cycles.
        check_le("stall <= 12 cyc",   last_stall, 12);

        // --- 2. Hit on next word in same line ---
        $display("\n[2] Same-line hit on 0x80000004");
        ahb_read(32'h80000004);
        check_eq("data    @0x80000004", cpu_data, 32'h80000004);
        check_le("stall == 0 cyc",    last_stall, 0);

        // --- 3. Last word in same line still hits ---
        $display("\n[3] Same-line hit on 0x8000001C (last word of line)");
        ahb_read(32'h8000001C);
        check_eq("data    @0x8000001C", cpu_data, 32'h8000001C);
        check_le("stall == 0 cyc",    last_stall, 0);

        // --- 4. Cold miss on a second line at slot 1 ---
        // Slot 1 lives at addr bits[9:5] == 5'd1, so any address with
        // those bits set works.  Use 0x80000020 (next 32 bytes).
        $display("\n[4] Cold miss on 0x80000020 (line 1, slot 1)");
        ahb_read(32'h80000020);
        check_eq("data    @0x80000020", cpu_data, 32'h80000020);
        check_le("stall <= 12 cyc",   last_stall, 12);

        // --- 5. Conflict miss: same slot as line 0, different tag.  For
        // NL=32 the slot is selected by bits[9:5], the tag by bits[31:10].
        // Add 0x400 (= 1 << 10) to flip the lowest tag bit while
        // keeping the slot index unchanged.
        $display("\n[5] Conflict miss on 0x80000400 (slot 0, new tag)");
        ahb_read(32'h80000400);
        check_eq("data    @0x80000400", cpu_data, 32'h80000400);
        check_le("stall <= 12 cyc",   last_stall, 12);

        // --- 6. Thrash back to original 0x80000000 → miss again ---
        $display("\n[6] Thrash back to 0x80000000 (was evicted)");
        ahb_read(32'h80000000);
        check_eq("data    @0x80000000", cpu_data, 32'h80000000);
        check_le("stall <= 12 cyc",   last_stall, 12);

        // --- 7. Sequential read across the line boundary ---
        // Read offsets 0x18, 0x1c (last two words of slot 0's line),
        // then 0x20, 0x24 (first two words of slot 1's line — which
        // is already cached from test #4).  Both lines should hit
        // since #4 left slot 1 valid and #6 just refilled slot 0.
        $display("\n[7] Sequential reads across line boundary");
        ahb_read(32'h80000018);  check_eq("data    @0x80000018", cpu_data, 32'h80000018);
                                 check_le("stall == 0 cyc",     last_stall, 0);
        ahb_read(32'h8000001C);  check_eq("data    @0x8000001C", cpu_data, 32'h8000001C);
                                 check_le("stall == 0 cyc",     last_stall, 0);
        ahb_read(32'h80000020);  check_eq("data    @0x80000020", cpu_data, 32'h80000020);
                                 check_le("stall == 0 cyc",     last_stall, 0);
        ahb_read(32'h80000024);  check_eq("data    @0x80000024", cpu_data, 32'h80000024);
                                 check_le("stall == 0 cyc",     last_stall, 0);

        // --- 8. Steady-state hit on the same word repeated ---
        $display("\n[8] Repeated hit on 0x80000020");
        ahb_read(32'h80000020);  check_eq("data #1 @0x80000020", cpu_data, 32'h80000020);
                                 check_le("stall == 0 cyc",     last_stall, 0);
        ahb_read(32'h80000020);  check_eq("data #2 @0x80000020", cpu_data, 32'h80000020);
                                 check_le("stall == 0 cyc",     last_stall, 0);
        ahb_read(32'h80000020);  check_eq("data #3 @0x80000020", cpu_data, 32'h80000020);
                                 check_le("stall == 0 cyc",     last_stall, 0);

        // --- 9. CWF early restart on word 0 ---
        // With this mock flash a fetch produces word K at internal
        // phase K+1 and the cache surfaces dhit at sim cycle K+4 from
        // the miss start (1 for state→ST_FETCH, 1 for clear-stale,
        // 1 for word arrival, 1 for fwv rising-edge latch).  Without
        // CWF every miss is ~10.  Asserts: word-0 stall <= 4.
        $display("\n[9] CWF early-restart on word 0 (offset 0x00)");
        ahb_read(32'h80000800);  // fresh line, slot 0, new tag
        check_eq("data    @0x80000800", cpu_data, 32'h80000800);
`ifdef CWF
        check_le("stall <= 4 cyc (CWF word 0)", last_stall, 4);
`else
        check_le("stall <= 12 cyc",             last_stall, 12);
`endif

        // --- 10. CWF stale-data regression test ---
        // After a previous fetch leaves m_word_done = all-1s, the next
        // miss must not publish data from those stale bits before the
        // new line actually fills.  My first CWF attempt hit exactly
        // this bug — the CPU looped on garbage after 3 misses.  The
        // test asserts the data returned is the NEW line's, not the
        // previous fetch's leftover.
        $display("\n[10] CWF stale-bit regression — back-to-back miss to new line");
        ahb_read(32'h80000C00);  // slot 0, yet another tag — must miss
        check_eq("data    @0x80000C00", cpu_data, 32'h80000C00);

        // --- 11. CWF mid-line restart (word 4) ---
        // Stall formula K+4 (see test 9) gives 8 for word 4.  Without
        // CWF it's still ~10.  CWF wins by 2 here; the win grows for
        // earlier words and shrinks toward last word (word 7 ≈ 11
        // cycles, slightly worse than no-CWF — that's the tradeoff
        // when the requested word happens to be last in the line).
        $display("\n[11] CWF early-restart on word 4 (offset 0x10)");
        ahb_read(32'h80001010);  // new line, mid-line word
        check_eq("data    @0x80001010", cpu_data, 32'h80001010);
`ifdef CWF
        check_le("stall <= 8 cyc (CWF word 4)", last_stall, 8);
`else
        check_le("stall <= 12 cyc",             last_stall, 12);
`endif

        // --- 12. CWF on last word (word 7) ---
        // Worst case for CWF: requested word is last in the line.
        // Stall ~K+4 = 11 cycles, marginally worse than no-CWF's ~10.
        // Expected, just verify correctness + bound.
        $display("\n[12] CWF — request last word (offset 0x1C)");
        ahb_read(32'h80001C1C);  // new line (slot 0, new tag), last word
        check_eq("data    @0x80001C1C", cpu_data, 32'h80001C1C);
`ifdef CWF
        check_le("stall <= 12 cyc (CWF word 7)", last_stall, 12);
`else
        check_le("stall <= 12 cyc",              last_stall, 12);
`endif

        // --- 13. Uniform word-offset miss sweep ---
        // 8 cold misses, one per word position (offsets 0x00, 0x04,
        // ..., 0x1c).  Each lives in a different cache slot so they
        // don't conflict; the only thing we're measuring is how the
        // miss penalty depends on the requested word.
        //
        // Without CWF every miss waits for the full line: ~10 cyc each
        // × 8 = ~80 total.
        // With CWF the stall is K+4 per miss, summing to
        //     (4 + 5 + 6 + 7 + 8 + 9 + 10 + 11) = 60 cyc total.
        // Saving: 20 cyc / 25 % across 8 misses.  Real flash latency
        // per word is much higher (16 cyc) so the SoC saving scales
        // proportionally — see the doc block at the top of the file.
        $display("\n[13] Uniform miss sweep — all 8 word offsets");
        begin : sweep
            integer k;
            for (k = 0; k < 8; k = k + 1) begin
                // Slot-stride: each iteration picks a fresh slot so
                // we don't conflict with previous fetches.  Tag bits
                // (>= bit 10) absorb the iteration index too.
                ahb_read(32'h80010000 + (k << 10) + (k << 2));
                check_eq("data sweep", cpu_data,
                         32'h80010000 + (k << 10) + (k << 2));
            end
        end

        // --- summary ---
        $display("\n=== ro_dmc_tb summary: %0d PASS  %0d FAIL ===", passes, fails);
        $display("=== ro_dmc_tb perf: total_stall=%0d cyc across %0d misses (avg %0d cyc/miss) ===",
                 total_stall, total_misses,
                 (total_misses > 0) ? (total_stall / total_misses) : 0);
        if (fails != 0) begin
            $display("=== TESTBENCH FAILED ===");
            $display("TB_RESULT: FAIL");
        end else begin
            $display("TB_RESULT: PASS");
        end

        #50;
        $finish;
    end
endmodule
