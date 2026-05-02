`timescale 1ns/1ps
// Testbench: APB scratchpad loader (SPAD_ADDR / SPAD_WDATA / SPAD_RDATA)
//
// Verifies that firmware can:
//   1. Write words into the simulation scratchpad via SPAD_ADDR + SPAD_WDATA.
//   2. Read them back via SPAD_RDATA (auto-increment path).
//   3. SPAD_CTRL bit[0] resets SPAD_ADDR to 0.
//   4. The same tiny Conv1D golden layer (same tensors as tb_kws_soc_conv1d_tiny_golden)
//      gives the same expected outputs when preloaded through the APB loader.

module tb_kws_soc_conv1d_spad_loader;

    parameter CLK_HALF = 5; // 100 MHz

    reg clk   = 0;
    reg rst_n = 0;

    always #CLK_HALF clk = ~clk;

    reg         psel    = 0;
    reg         penable = 0;
    reg         pwrite  = 0;
    reg [31:0]  paddr   = 0;
    reg [31:0]  pwdata  = 0;
    wire [31:0] prdata;
    wire        pready;
    wire        pslverr;

    wire        rd0_en;   wire [31:0] rd0_addr;
    wire [31:0] rd0_data; wire        rd0_valid;
    wire        rd1_en;   wire [31:0] rd1_addr;
    wire [31:0] rd1_data; wire        rd1_valid;
    wire        wr_en;    wire [31:0] wr_addr;
    wire [31:0] wr_data;  wire [3:0]  wr_strb;

    wire        spad_lpb_wr_en;
    wire [31:0] spad_lpb_wr_addr;
    wire [31:0] spad_lpb_wr_data;
    wire [3:0]  spad_lpb_wr_strb;
    wire [31:0] spad_lpb_rd_addr;
    wire [31:0] spad_lpb_rd_data;

    conv1d_accel_soc_wrapper #(
        .ADDR_W(32), .DATA_W(32), .MAX_IN_CH(64), .MAX_OUT_CH(64), .USE_WEIGHT_BUFFER(1)
    ) dut (
        .pclk(clk), .presetn(rst_n),
        .psel(psel), .penable(penable), .pwrite(pwrite),
        .paddr(paddr), .pwdata(pwdata),
        .prdata(prdata), .pready(pready), .pslverr(pslverr),

        .rd0_en(rd0_en), .rd0_addr(rd0_addr),
        .rd0_data(rd0_data), .rd0_valid(rd0_valid),
        .rd1_en(rd1_en), .rd1_addr(rd1_addr),
        .rd1_data(rd1_data), .rd1_valid(rd1_valid),
        .wr_en(wr_en), .wr_addr(wr_addr),
        .wr_data(wr_data), .wr_strb(wr_strb),

        .spad_lpb_wr_en(spad_lpb_wr_en),
        .spad_lpb_wr_addr(spad_lpb_wr_addr),
        .spad_lpb_wr_data(spad_lpb_wr_data),
        .spad_lpb_wr_strb(spad_lpb_wr_strb),
        .spad_lpb_rd_addr(spad_lpb_rd_addr),
        .spad_lpb_rd_data(spad_lpb_rd_data)
    );

    conv1d_scratchpad_mem #(.MEM_BYTES(8192)) spad (
        .clk(clk), .rst_n(rst_n),
        .rd0_en(rd0_en), .rd0_addr(rd0_addr),
        .rd0_data(rd0_data), .rd0_valid(rd0_valid),
        .rd1_en(rd1_en), .rd1_addr(rd1_addr),
        .rd1_data(rd1_data), .rd1_valid(rd1_valid),
        .wr_en(wr_en), .wr_addr(wr_addr),
        .wr_data(wr_data), .wr_strb(wr_strb),
        .lpb_wr_en(spad_lpb_wr_en),
        .lpb_wr_addr(spad_lpb_wr_addr),
        .lpb_wr_data(spad_lpb_wr_data),
        .lpb_wr_strb(spad_lpb_wr_strb),
        .lpb_rd_addr(spad_lpb_rd_addr),
        .lpb_rd_data(spad_lpb_rd_data)
    );

    // -----------------------------------------------------------------------
    // APB helpers
    // -----------------------------------------------------------------------
    task apb_write;
        input [31:0] addr;
        input [31:0] data;
        begin
            @(posedge clk); #1;
            psel = 1; pwrite = 1; paddr = addr; pwdata = data; penable = 0;
            @(posedge clk); #1;
            penable = 1;
            @(posedge clk); #1;
            psel = 0; penable = 0; pwrite = 0;
        end
    endtask

    task apb_read;
        input  [31:0] addr;
        output [31:0] data;
        begin
            @(posedge clk); #1;
            psel = 1; pwrite = 0; paddr = addr; penable = 0;
            @(posedge clk); #1;
            penable = 1;
            // prdata is combinatorial — sample it during the access phase
            // (before the posedge where spad_addr auto-increments)
            @(negedge clk);
            data = prdata;
            @(posedge clk); #1;
            psel = 0; penable = 0;
        end
    endtask

    // Helper: pack four signed 8-bit values into one 32-bit word, LSB-first
    function [31:0] pack4;
        input signed [7:0] b0, b1, b2, b3;
        begin
            pack4 = {b3[7:0], b2[7:0], b1[7:0], b0[7:0]};
        end
    endfunction

    // -----------------------------------------------------------------------
    // Register offsets
    // -----------------------------------------------------------------------
    localparam CONV1D_INPUT_BASE   = 32'h0C;
    localparam CONV1D_WEIGHT_BASE  = 32'h10;
    localparam CONV1D_BIAS_BASE    = 32'h14;
    localparam CONV1D_OUTPUT_BASE  = 32'h18;
    localparam CONV1D_INPUT_LEN    = 32'h1C;
    localparam CONV1D_IN_CH        = 32'h20;
    localparam CONV1D_OUT_CH       = 32'h24;
    localparam CONV1D_QUANT        = 32'h28;
    localparam CONV1D_QUANT_INDEX  = 32'h2C;
    localparam CONV1D_SHIFT_DATA   = 32'h30;
    localparam CONV1D_CTRL         = 32'h04;
    localparam CONV1D_STATUS       = 32'h08;

    localparam SPAD_ADDR  = 32'h34;
    localparam SPAD_WDATA = 32'h38;
    localparam SPAD_RDATA = 32'h3C;
    localparam SPAD_CTRL  = 32'h40;

    // Same tensor addresses as tb_kws_soc_conv1d_tiny_golden
    localparam INPUT_BASE  = 32'h0000_0000;
    localparam WEIGHT_BASE = 32'h0000_0100;
    localparam BIAS_BASE   = 32'h0000_0200;
    localparam OUTPUT_BASE = 32'h0000_0300;

    integer failures = 0;
    reg [31:0] rdata;

    initial begin
        $display("=== Conv1D APB scratchpad loader testbench ===");
        rst_n = 0;
        repeat(4) @(posedge clk);
        rst_n = 1;
        repeat(2) @(posedge clk);

        // ==================================================================
        // Part 1: write/read-back smoke test
        // ==================================================================
        $display("Part 1: write/read-back smoke");

        apb_write(SPAD_CTRL, 32'h1);          // reset SPAD_ADDR to 0
        apb_write(SPAD_ADDR, 32'h0);

        apb_write(SPAD_WDATA, 32'hDEADBEEF);
        apb_write(SPAD_WDATA, 32'hCAFEBABE);
        apb_write(SPAD_WDATA, 32'hA5A5A5A5);
        apb_write(SPAD_WDATA, 32'h12345678);

        // SPAD_ADDR should have auto-incremented to 16
        apb_read(SPAD_ADDR, rdata);
        if (rdata !== 32'd16) begin
            $display("FAIL Part1: SPAD_ADDR after 4 writes = %0d, expected 16", rdata);
            failures = failures + 1;
        end

        // Reset and read back
        apb_write(SPAD_CTRL, 32'h1);
        apb_read(SPAD_RDATA, rdata);
        if (rdata !== 32'hDEADBEEF) begin
            $display("FAIL Part1 word0: got %08X expected DEADBEEF", rdata);
            failures = failures + 1;
        end
        apb_read(SPAD_RDATA, rdata);
        if (rdata !== 32'hCAFEBABE) begin
            $display("FAIL Part1 word1: got %08X expected CAFEBABE", rdata);
            failures = failures + 1;
        end
        apb_read(SPAD_RDATA, rdata);
        if (rdata !== 32'hA5A5A5A5) begin
            $display("FAIL Part1 word2: got %08X expected A5A5A5A5", rdata);
            failures = failures + 1;
        end
        apb_read(SPAD_RDATA, rdata);
        if (rdata !== 32'h12345678) begin
            $display("FAIL Part1 word3: got %08X expected 12345678", rdata);
            failures = failures + 1;
        end
        if (failures == 0) $display("Part 1 PASS");

        // ==================================================================
        // Part 2: tiny Conv1D golden via APB loader
        //
        // Uses IDENTICAL tensors to tb_kws_soc_conv1d_tiny_golden so the
        // expected outputs are the same verified golden values.
        //
        //   input_len=8, in_ch=4, out_ch=2, K=3, out_len=6
        //   shifts: oc0=0, oc1=1; relu_en=0; bias: oc0=0, oc1=1
        // ==================================================================
        $display("Part 2: tiny Conv1D golden via APB loader");

        // --- Write inputs (8 time steps × 4 channels, 8 words) ---
        // Same values as tiny golden: input_data[0..31]
        apb_write(SPAD_ADDR, INPUT_BASE);
        apb_write(SPAD_WDATA, pack4( 1,  2,  3,  4)); // t0
        apb_write(SPAD_WDATA, pack4( 2,  1,  0, -1)); // t1
        apb_write(SPAD_WDATA, pack4(-1,  1,  2,  0)); // t2
        apb_write(SPAD_WDATA, pack4( 3, -2,  1,  2)); // t3
        apb_write(SPAD_WDATA, pack4( 0,  1, -3,  2)); // t4
        apb_write(SPAD_WDATA, pack4( 1,  0,  2, -2)); // t5
        apb_write(SPAD_WDATA, pack4( 2,  2, -1,  1)); // t6
        apb_write(SPAD_WDATA, pack4(-2,  3,  0,  1)); // t7

        // --- Write weights (2 oc × 3 k × 4 ic = 6 words) ---
        // Same values as tiny golden: weight_data[0..23]
        apb_write(SPAD_ADDR, WEIGHT_BASE);
        apb_write(SPAD_WDATA, pack4( 1,  0, -1,  2)); // oc0,k0
        apb_write(SPAD_WDATA, pack4( 0,  1,  1,  0)); // oc0,k1
        apb_write(SPAD_WDATA, pack4(-1,  2,  0,  1)); // oc0,k2
        apb_write(SPAD_WDATA, pack4( 2, -1,  0,  1)); // oc1,k0
        apb_write(SPAD_WDATA, pack4( 1,  1, -1,  0)); // oc1,k1
        apb_write(SPAD_WDATA, pack4( 0, -2,  1,  1)); // oc1,k2

        // --- Write biases (2 out channels → 2 × 32-bit words) ---
        // bias_data[0]=0, bias_data[1]=1 (same as tiny golden)
        apb_write(SPAD_ADDR, BIAS_BASE);
        apb_write(SPAD_WDATA, 32'd0); // oc0 bias = 0
        apb_write(SPAD_WDATA, 32'd1); // oc1 bias = 1

        // --- Program accelerator registers ---
        apb_write(CONV1D_INPUT_BASE,  INPUT_BASE);
        apb_write(CONV1D_WEIGHT_BASE, WEIGHT_BASE);
        apb_write(CONV1D_BIAS_BASE,   BIAS_BASE);
        apb_write(CONV1D_OUTPUT_BASE, OUTPUT_BASE);
        apb_write(CONV1D_INPUT_LEN,   32'd8);
        apb_write(CONV1D_IN_CH,       32'd4);
        apb_write(CONV1D_OUT_CH,      32'd2);
        apb_write(CONV1D_QUANT,       32'h00); // global shift=0, reload_all (overridden below)

        // Per-output-channel shifts: oc0=0, oc1=1
        apb_write(CONV1D_QUANT_INDEX, 32'd0); apb_write(CONV1D_SHIFT_DATA, 32'd0);
        apb_write(CONV1D_QUANT_INDEX, 32'd1); apb_write(CONV1D_SHIFT_DATA, 32'd1);

        // --- Start ---
        apb_write(CONV1D_CTRL, 32'h1);

        // --- Poll done ---
        begin : poll_done
            integer timeout;
            timeout = 0;
            rdata = 0;
            while (!(rdata & 32'h2) && timeout < 10000) begin
                apb_read(CONV1D_STATUS, rdata);
                timeout = timeout + 1;
            end
            if (timeout >= 10000) begin
                $display("FAIL Part2: timeout waiting for done");
                failures = failures + 1;
                disable poll_done;
            end
        end

        // --- Read and verify outputs ---
        // Output layout: out_ch=2 bytes per time step, contiguous.
        // Each 32-bit word at OUTPUT_BASE+4*n contains two time steps:
        //   word[7:0]   = oc0 @ t(2n)
        //   word[15:8]  = oc1 @ t(2n)
        //   word[23:16] = oc0 @ t(2n+1)
        //   word[31:24] = oc1 @ t(2n+1)
        //
        // Expected: t0=(10,4), t1=(-2,4), t2=(0,-3), t3=(1,7), t4=(12,-2), t5=(5,0)
        begin : verify_outputs
            reg [31:0] w0, w1, w2;
            integer got;

            apb_write(SPAD_ADDR, OUTPUT_BASE);
            apb_read(SPAD_RDATA, w0); // t0,t1
            apb_read(SPAD_RDATA, w1); // t2,t3
            apb_read(SPAD_RDATA, w2); // t4,t5

            // t0
            got = $signed(w0[7:0]);
            if (got !== 10) begin $display("FAIL Part2 t=0 oc0: got %0d exp 10", got); failures=failures+1; end
            got = $signed(w0[15:8]);
            if (got !== 4) begin $display("FAIL Part2 t=0 oc1: got %0d exp 4",  got); failures=failures+1; end
            // t1
            got = $signed(w0[23:16]);
            if (got !== -2) begin $display("FAIL Part2 t=1 oc0: got %0d exp -2", got); failures=failures+1; end
            got = $signed(w0[31:24]);
            if (got !== 4) begin $display("FAIL Part2 t=1 oc1: got %0d exp 4",  got); failures=failures+1; end
            // t2
            got = $signed(w1[7:0]);
            if (got !== 0) begin $display("FAIL Part2 t=2 oc0: got %0d exp 0",  got); failures=failures+1; end
            got = $signed(w1[15:8]);
            if (got !== -3) begin $display("FAIL Part2 t=2 oc1: got %0d exp -3", got); failures=failures+1; end
            // t3
            got = $signed(w1[23:16]);
            if (got !== 1) begin $display("FAIL Part2 t=3 oc0: got %0d exp 1",  got); failures=failures+1; end
            got = $signed(w1[31:24]);
            if (got !== 7) begin $display("FAIL Part2 t=3 oc1: got %0d exp 7",  got); failures=failures+1; end
            // t4
            got = $signed(w2[7:0]);
            if (got !== 12) begin $display("FAIL Part2 t=4 oc0: got %0d exp 12", got); failures=failures+1; end
            got = $signed(w2[15:8]);
            if (got !== -2) begin $display("FAIL Part2 t=4 oc1: got %0d exp -2", got); failures=failures+1; end
            // t5
            got = $signed(w2[23:16]);
            if (got !== 5) begin $display("FAIL Part2 t=5 oc0: got %0d exp 5",  got); failures=failures+1; end
            got = $signed(w2[31:24]);
            if (got !== 0) begin $display("FAIL Part2 t=5 oc1: got %0d exp 0",  got); failures=failures+1; end
        end

        // ==================================================================
        // Final verdict
        // ==================================================================
        if (failures == 0)
            $display("PASS: Conv1D APB scratchpad loader test");
        else
            $display("FAIL: %0d failure(s) in APB scratchpad loader test", failures);

        $finish;
    end

    initial begin
        #500000;
        $display("FAIL: simulation timeout");
        $finish;
    end

endmodule
