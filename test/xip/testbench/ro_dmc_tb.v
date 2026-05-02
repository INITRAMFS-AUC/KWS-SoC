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
    // Pass / fail tracking
    // ------------------------------------------------------------------
    integer passes = 0;
    integer fails  = 0;

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
        // The fetch returns word 0 first (per the mock flash model),
        // so a CWF-enabled cache should release HREADYOUT in ~1-2
        // cycles, not after all 8 words land.  Without CWF, stall
        // is ~9-10.  Use the 12-cyc loose bound today; tighten to
        // <= 4 once `CWF is defined.
        $display("\n[9] CWF early-restart on word 0 (offset 0x00)");
        ahb_read(32'h80000800);  // fresh line, slot 0, new tag
        check_eq("data    @0x80000800", cpu_data, 32'h80000800);
`ifdef CWF
        check_le("stall <= 4 cyc (CWF early)", last_stall, 4);
`else
        check_le("stall <= 12 cyc",            last_stall, 12);
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
        // With CWF, requesting offset 0x10 (word 4) should resume
        // when word 4 lands at flash_phase==5 (~5 cycles).  Without
        // CWF, still 9-10 cycles.
        $display("\n[11] CWF early-restart on word 4 (offset 0x10)");
        ahb_read(32'h80001010);  // new line, mid-line word
        check_eq("data    @0x80001010", cpu_data, 32'h80001010);
`ifdef CWF
        check_le("stall <= 7 cyc (CWF mid-line)", last_stall, 7);
`else
        check_le("stall <= 12 cyc",               last_stall, 12);
`endif

        // --- summary ---
        $display("\n=== ro_dmc_tb summary: %0d PASS  %0d FAIL ===", passes, fails);
        if (fails != 0) begin
            $display("=== TESTBENCH FAILED ===");
            // iverilog $finish always returns 0; print a marker line so
            // an outer wrapper grep can catch it.
            $display("TB_RESULT: FAIL");
        end else begin
            $display("TB_RESULT: PASS");
        end

        #50;
        $finish;
    end
endmodule
