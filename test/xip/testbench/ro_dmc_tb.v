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

    reg [LW-1:0]    m_data;
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
        .m_addr(m_addr),
        .m_start(m_start),
        .m_done(m_done)
    );

    // ------------------------------------------------------------------
    // Mock flash: 5-cycle latency per line, returns address-as-data so
    // the expected word value is just the address itself.
    // ------------------------------------------------------------------
    reg [3:0] flash_delay;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            m_done      <= 1'b0;
            m_data      <= {LW{1'b0}};
            flash_delay <= 4'd0;
        end else begin
            if (m_start && flash_delay == 0) begin
                flash_delay <= 4'd5;
                m_done      <= 1'b0;
            end else if (flash_delay > 1) begin
                flash_delay <= flash_delay - 1'b1;
                m_done      <= 1'b0;
            end else if (flash_delay == 1) begin
                flash_delay <= 4'd0;
                m_done      <= 1'b1;
                // 8 words: the line's base address through base + 0x1c.
                m_data      <= { m_addr + 32'h1c, m_addr + 32'h18,
                                 m_addr + 32'h14, m_addr + 32'h10,
                                 m_addr + 32'h0c, m_addr + 32'h08,
                                 m_addr + 32'h04, m_addr };
            end else begin
                m_done <= 1'b0;
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
