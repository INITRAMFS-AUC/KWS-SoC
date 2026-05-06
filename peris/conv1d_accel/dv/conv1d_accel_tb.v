`timescale 1ns/1ps
`default_nettype none

`include "tb_utils.vh"

// Self-checking randomised testbench for conv1d_accel.
//
// Memory map (flat 64 KB array, 16-bit byte-addressed):
//   0x1000  SRC  — input feature map  (NWC int8)
//   0x2000  WT   — weights            (C_out × K_w × C_in int8)
//   0x3000  BS   — bias               (C_out × int32 LE)
//   0x4000  SH   — per-channel shift  (C_out × uint8)
//   0x5000  DST  — output feature map (W_out × C_out int8)
//
// AHB slave model:
//   Address phase is captured on every accepted beat (htrans[1]=1 && hready).
//   Read data is driven combinatorially from the WORD-ALIGNED capture address,
//   matching the one-cycle pipeline latency the DUT expects (r_wait mechanism).
//   Byte writes use the lane selected by haddr[1:0].

module conv1d_accel_tb;

    // -----------------------------------------------------------------------
    // Clock, reset, dump, timeout
    `TB_CLK(clk, 20)
    `TB_SRSTN(rst_n, clk, 100)
    `TB_DUMP("conv1d_accel_tb.vcd", conv1d_accel_tb, 0)
    `TB_FINISH(10_000_000)

    // -----------------------------------------------------------------------
    // APB master interface (TB → DUT slave)
    reg  [15:0] paddr;
    reg         psel, penable, pwrite;
    reg  [31:0] pwdata;
    wire [31:0] prdata;

    // -----------------------------------------------------------------------
    // AHB-Lite slave interface (DUT master → TB)
    wire [31:0] haddr;
    wire [ 2:0] hburst, hsize;
    wire [ 1:0] htrans;
    wire        hwrite;
    wire [31:0] hwdata;
    reg  [31:0] hrdata;
    reg         hready;
    wire        hresp;

    assign hresp = 1'b0;

    // -----------------------------------------------------------------------
    // DUT
    conv1d_accel DUT (
        .clk(clk), .rst_n(rst_n),
        .paddr(paddr),   .psel(psel),     .penable(penable),
        .pwrite(pwrite), .pwdata(pwdata),
        .prdata(prdata), .pready(),       .pslverr(),
        .haddr(haddr),   .hburst(hburst), .hsize(hsize),
        .htrans(htrans), .hwrite(hwrite), .hwdata(hwdata),
        .hprot(),        .hmastlock(),
        .hrdata(hrdata), .hready(hready), .hresp(hresp)
    );

    // -----------------------------------------------------------------------
    // Flat 64 KB behavioral memory
    reg [7:0] mem [0:65535];

    localparam [31:0] SRC_BASE = 32'h0000_1000;
    localparam [31:0] WT_BASE  = 32'h0000_2000;
    localparam [31:0] BS_BASE  = 32'h0000_3000;
    localparam [31:0] SH_BASE  = 32'h0000_4000;
    localparam [31:0] DST_BASE = 32'h0000_5000;

    // -----------------------------------------------------------------------
    // AHB-Lite slave model

    reg [31:0] ap_addr;
    reg        ap_wr, ap_valid;
    reg [ 2:0] ap_sz;

    // Capture address/control phase on every accepted beat
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            ap_valid <= 1'b0;
        end else if (hready) begin
            ap_valid <= htrans[1];   // NONSEQ or SEQ
            ap_addr  <= haddr;
            ap_wr    <= hwrite;
            ap_sz    <= hsize;
        end
    end

    // Combinatorial read — two modes based on transfer size.
    //
    // HSIZE_WORD: return bytes from the exact byte address (unaligned support).
    //   The DUT issues WORD reads at w_pos*stride*C_in which may not be word-
    //   aligned, and stores hrdata[7:0] as patch byte 0, so the slave must
    //   deliver bytes from that exact offset (Cortex-M AHB matrix behaviour).
    //
    // HSIZE_BYTE: return the full word at the word-aligned address.
    //   Weight and shift reads use BYTE size and explicitly select the lane
    //   via haddr[1:0], so the byte must sit in its natural lane position.
    always @(*) begin
        hrdata = 32'hDEAD_BEEF;
        if (ap_valid && !ap_wr) begin
            if (ap_sz == 3'b010)
                hrdata = {mem[ap_addr[15:0]+3], mem[ap_addr[15:0]+2],
                          mem[ap_addr[15:0]+1], mem[ap_addr[15:0]+0]};
            else
                hrdata = {mem[{ap_addr[15:2], 2'b11}],
                          mem[{ap_addr[15:2], 2'b10}],
                          mem[{ap_addr[15:2], 2'b01}],
                          mem[{ap_addr[15:2], 2'b00}]};
        end
    end

    // Write: capture hwdata on data phase (one cycle after address phase)
    always @(posedge clk) begin
        if (ap_valid && ap_wr && hready) begin
            case (ap_sz)
                3'b000: begin   // BYTE — select lane from address[1:0]
                    case (ap_addr[1:0])
                        2'b00: mem[ap_addr[15:0]]   <= hwdata[ 7: 0];
                        2'b01: mem[ap_addr[15:0]]   <= hwdata[15: 8];
                        2'b10: mem[ap_addr[15:0]]   <= hwdata[23:16];
                        2'b11: mem[ap_addr[15:0]]   <= hwdata[31:24];
                    endcase
                end
                3'b010: begin   // WORD
                    mem[ap_addr[15:0]+0] <= hwdata[ 7: 0];
                    mem[ap_addr[15:0]+1] <= hwdata[15: 8];
                    mem[ap_addr[15:0]+2] <= hwdata[23:16];
                    mem[ap_addr[15:0]+3] <= hwdata[31:24];
                end
                default: ;
            endcase
        end
    end

    // -----------------------------------------------------------------------
    // APB master tasks

    task apb_write;
        input [15:0] addr;
        input [31:0] data;
        begin
            @(posedge clk); #1;
            paddr = addr; pwdata = data; pwrite = 1'b1;
            psel = 1'b1; penable = 1'b0;
            @(posedge clk); #1;
            penable = 1'b1;
            @(posedge clk); #1;
            psel = 1'b0; penable = 1'b0; pwrite = 1'b0;
        end
    endtask

    task apb_read;
        input  [15:0] addr;
        output [31:0] data;
        begin
            @(posedge clk); #1;
            paddr = addr; pwrite = 1'b0;
            psel = 1'b1; penable = 1'b0;
            @(posedge clk); #1;
            penable = 1'b1;
            @(posedge clk); #1;
            data = prdata;
            psel = 1'b0; penable = 1'b0;
        end
    endtask

    // -----------------------------------------------------------------------
    // Reference data storage

    reg signed [ 7:0] ref_in  [0:511];   // input NWC
    reg signed [ 7:0] ref_wt  [0:511];   // weights: C_out × patch
    reg signed [31:0] ref_bs  [0: 15];   // bias per output channel
    reg        [ 4:0] ref_sh  [0: 15];   // shift per output channel
    reg signed [ 7:0] ref_out [0:511];   // expected output

    // -----------------------------------------------------------------------
    // Test dimension registers
    reg [ 7:0] t_c_in, t_c_out, t_k_w, t_stride;
    reg [15:0] t_w_in, t_w_out, t_patch_bytes;
    integer    tc, errors;

    // -----------------------------------------------------------------------
    // Reference conv1d: for each (c_out, w_out) compute
    //   acc = sum_n { in[w_out*stride*C_in + n] * wt[c_out*patch_bytes + n] }
    //   out = clip( (acc + bias) >>> shift, -128, 127 )
    task compute_reference;
        integer co, wo, n;
        integer acc, s;
        begin
            for (co = 0; co < t_c_out; co = co + 1) begin
                for (wo = 0; wo < t_w_out; wo = wo + 1) begin
                    acc = 0;
                    for (n = 0; n < t_patch_bytes; n = n + 1) begin
                        acc = acc +
                            $signed(ref_in[wo * t_stride * t_c_in + n]) *
                            $signed(ref_wt[co * t_patch_bytes        + n]);
                    end
                    acc = acc + ref_bs[co];
                    s = acc >>> ref_sh[co];
                    if      (s >  127)  ref_out[wo * t_c_out + co] =  8'sd127;
                    else if (s < -128)  ref_out[wo * t_c_out + co] = -8'sd128;
                    else                ref_out[wo * t_c_out + co] =  s[7:0];
                end
            end
        end
    endtask

    // -----------------------------------------------------------------------
    // Write reference arrays into behavioral memory
    task load_memory;
        integer idx;
        begin
            for (idx = 0; idx < t_w_in * t_c_in; idx = idx + 1)
                mem[SRC_BASE[15:0] + idx] = ref_in[idx];
            for (idx = 0; idx < t_c_out * t_patch_bytes; idx = idx + 1)
                mem[WT_BASE[15:0]  + idx] = ref_wt[idx];
            for (idx = 0; idx < t_c_out; idx = idx + 1) begin
                mem[BS_BASE[15:0] + idx*4 + 0] = ref_bs[idx][ 7: 0];
                mem[BS_BASE[15:0] + idx*4 + 1] = ref_bs[idx][15: 8];
                mem[BS_BASE[15:0] + idx*4 + 2] = ref_bs[idx][23:16];
                mem[BS_BASE[15:0] + idx*4 + 3] = ref_bs[idx][31:24];
            end
            for (idx = 0; idx < t_c_out; idx = idx + 1)
                mem[SH_BASE[15:0] + idx] = {3'b0, ref_sh[idx]};
            // Poison output region so stale data is visible on failure
            for (idx = 0; idx < t_w_out * t_c_out + 4; idx = idx + 1)
                mem[DST_BASE[15:0] + idx] = 8'hAA;
        end
    endtask

    // -----------------------------------------------------------------------
    // Compare DUT output (written to mem[DST_BASE..]) against reference
    task check_output;
        integer idx;
        reg [7:0] got;
        begin
            for (idx = 0; idx < t_w_out * t_c_out; idx = idx + 1) begin
                got = mem[DST_BASE[15:0] + idx];
                if (got !== ref_out[idx]) begin
                    $display("  [FAIL] TC%0d out[%0d]: exp=%0d (0x%02x)  got=%0d (0x%02x)",
                        tc, idx,
                        $signed(ref_out[idx]), ref_out[idx],
                        $signed(got),          got);
                    errors = errors + 1;
                end
            end
        end
    endtask

    // -----------------------------------------------------------------------
    // Block until r_done asserts (direct DUT register access)
    task wait_done;
        integer cnt;
        begin
            cnt = 0;
            while (!DUT.r_done && cnt < 100000) begin
                @(posedge clk);
                cnt = cnt + 1;
            end
            if (!DUT.r_done) begin
                $display("  [TIMEOUT] TC%0d: accelerator stalled (state=%0d w=%0d c=%0d)",
                    tc, DUT.state, DUT.w_pos, DUT.c_pos);
                errors = errors + 1;
            end
        end
    endtask

    // -----------------------------------------------------------------------
    // Program all config registers and kick the accelerator
    task run_accel;
        begin
            apb_write(16'h0004, SRC_BASE);
            apb_write(16'h0008, WT_BASE);
            apb_write(16'h000C, DST_BASE);
            apb_write(16'h0010, BS_BASE);
            apb_write(16'h0020, SH_BASE);
            apb_write(16'h0014, {t_stride, t_k_w, t_c_out, t_c_in});
            apb_write(16'h0018, {16'b0, t_w_in});
            apb_write(16'h0000, 32'h0000_0001);   // start
            `TB_WAIT_FOR_CLOCK_CYC(clk, 5)        // allow busy to assert
            wait_done;
            `TB_WAIT_FOR_CLOCK_CYC(clk, 3)
        end
    endtask

    // -----------------------------------------------------------------------
    // Stimulus
    integer i;
    reg [31:0] rd;

    initial begin
        errors   = 0;
        hready   = 1'b1;
        psel     = 1'b0; penable = 1'b0; pwrite = 1'b0;
        paddr    = 16'b0; pwdata = 32'b0;

        @(posedge rst_n);
        `TB_WAIT_FOR_CLOCK_CYC(clk, 5)

        // ----------------------------------------------------------------
        // TC0 — fixed parameters, all-ones weights: easy hand-verification
        //   W_in=4, K_w=2, C_in=2, C_out=2, stride=1  →  W_out=3
        //   input  : [0,1,2,3,4,5,6,7]  (NWC byte order)
        //   weights: all 1
        //   bias=0, shift=0
        //   expected: w_out=0→6,  w_out=1→14,  w_out=2→22  (both c_out identical)
        // ----------------------------------------------------------------
        begin
            tc = 0;
            t_c_in  = 8'd2; t_c_out  = 8'd2;
            t_k_w   = 8'd2; t_stride = 8'd1;
            t_w_in  = 16'd4;
            t_patch_bytes = t_k_w * t_c_in;               // 4
            t_w_out = (t_w_in - t_k_w) / t_stride + 1;   // 3

            for (i = 0; i < t_w_in * t_c_in; i = i + 1)
                ref_in[i] = i[7:0];
            for (i = 0; i < t_c_out * t_patch_bytes; i = i + 1)
                ref_wt[i] = 8'sd1;
            for (i = 0; i < t_c_out; i = i + 1) begin
                ref_bs[i] = 32'sd0;
                ref_sh[i] = 5'd0;
            end

            $display("[TB] TC%0d (fixed) c_in=%0d c_out=%0d k_w=%0d stride=%0d w_in=%0d w_out=%0d",
                     tc, t_c_in, t_c_out, t_k_w, t_stride, t_w_in, t_w_out);
            compute_reference;
            load_memory;
            run_accel;
            check_output;
        end

        // ----------------------------------------------------------------
        // TC1 — stride=2 sanity check
        //   W_in=6, K_w=2, C_in=1, C_out=1, stride=2  →  W_out=3
        // ----------------------------------------------------------------
        begin
            tc = 1;
            t_c_in  = 8'd1; t_c_out  = 8'd1;
            t_k_w   = 8'd2; t_stride = 8'd2;
            t_w_in  = 16'd6;
            t_patch_bytes = t_k_w * t_c_in;
            t_w_out = (t_w_in - t_k_w) / t_stride + 1;

            for (i = 0; i < t_w_in * t_c_in; i = i + 1)
                ref_in[i] = i[7:0];          // 0,1,2,3,4,5
            for (i = 0; i < t_c_out * t_patch_bytes; i = i + 1)
                ref_wt[i] = 8'sd1;
            ref_bs[0] = 32'sd0;
            ref_sh[0] = 5'd0;

            $display("[TB] TC%0d (stride2) c_in=%0d c_out=%0d k_w=%0d stride=%0d w_in=%0d w_out=%0d",
                     tc, t_c_in, t_c_out, t_k_w, t_stride, t_w_in, t_w_out);
            compute_reference;
            load_memory;
            run_accel;
            check_output;
        end

        // ----------------------------------------------------------------
        // TC2 — shift and bias, verify clip logic
        //   weights=127 to force large accumulator; shift+bias cause clip
        // ----------------------------------------------------------------
        begin
            tc = 2;
            t_c_in  = 8'd2; t_c_out  = 8'd1;
            t_k_w   = 8'd3; t_stride = 8'd1;
            t_w_in  = 16'd5;
            t_patch_bytes = t_k_w * t_c_in;
            t_w_out = (t_w_in - t_k_w) / t_stride + 1;

            for (i = 0; i < t_w_in * t_c_in; i = i + 1)
                ref_in[i] = 8'sd127;
            for (i = 0; i < t_c_out * t_patch_bytes; i = i + 1)
                ref_wt[i] = 8'sd127;
            ref_bs[0] = 32'sd1000;
            ref_sh[0] = 5'd4;            // /16 → still large → clips to 127

            $display("[TB] TC%0d (clip+)  c_in=%0d c_out=%0d k_w=%0d stride=%0d w_in=%0d w_out=%0d",
                     tc, t_c_in, t_c_out, t_k_w, t_stride, t_w_in, t_w_out);
            compute_reference;
            load_memory;
            run_accel;
            check_output;
        end

        // ----------------------------------------------------------------
        // TC3 — negative clip
        // ----------------------------------------------------------------
        begin
            tc = 3;
            t_c_in  = 8'd2; t_c_out  = 8'd1;
            t_k_w   = 8'd2; t_stride = 8'd1;
            t_w_in  = 16'd4;
            t_patch_bytes = t_k_w * t_c_in;
            t_w_out = (t_w_in - t_k_w) / t_stride + 1;

            for (i = 0; i < t_w_in * t_c_in; i = i + 1)
                ref_in[i] = 8'sd127;
            for (i = 0; i < t_c_out * t_patch_bytes; i = i + 1)
                ref_wt[i] = -8'sd128;
            ref_bs[0] = -32'sd5000;
            ref_sh[0] = 5'd2;            // still very negative → clips to -128

            $display("[TB] TC%0d (clip-)  c_in=%0d c_out=%0d k_w=%0d stride=%0d w_in=%0d w_out=%0d",
                     tc, t_c_in, t_c_out, t_k_w, t_stride, t_w_in, t_w_out);
            compute_reference;
            load_memory;
            run_accel;
            check_output;
        end

        // ----------------------------------------------------------------
        // TC4–TC9 — fully randomised
        // ----------------------------------------------------------------
        for (tc = 4; tc < 10; tc = tc + 1) begin
            t_c_in   = 1 + ($random & 3);           // 1..4
            t_c_out  = 1 + ($random & 3);           // 1..4
            t_k_w    = 1 + ($random & 3);           // 1..4
            t_stride = 1 + ($random & 1);           // 1..2
            // w_in = k_w + stride*(1..4 extra output steps)
            t_w_in   = t_k_w + t_stride * (1 + ($random & 3));

            t_patch_bytes = t_k_w * t_c_in;
            t_w_out  = (t_w_in - t_k_w) / t_stride + 1;

            for (i = 0; i < t_w_in * t_c_in; i = i + 1)
                ref_in[i] = $random;
            for (i = 0; i < t_c_out * t_patch_bytes; i = i + 1)
                ref_wt[i] = $random;
            for (i = 0; i < t_c_out; i = i + 1) begin
                ref_bs[i] = $random % 256;    // small bias: -255..255
                ref_sh[i] = $random & 3;      // shift 0..3
            end

            $display("[TB] TC%0d (rand)   c_in=%0d c_out=%0d k_w=%0d stride=%0d w_in=%0d w_out=%0d",
                     tc, t_c_in, t_c_out, t_k_w, t_stride, t_w_in, t_w_out);
            compute_reference;
            load_memory;
            run_accel;
            check_output;
        end

        // ----------------------------------------------------------------
        // Summary
        // ----------------------------------------------------------------
        $display("=====================================");
        $display("  Test cases  : %0d", tc);
        $display("  Errors      : %0d", errors);
        if (errors == 0)
            $display("  ALL TESTS PASSED");
        else
            $display("  TESTS FAILED");
        $display("=====================================");
        $finish;
    end

endmodule
`default_nettype wire
