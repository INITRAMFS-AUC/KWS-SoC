`timescale 1ns/1ps
`include "tb.vh"

module ro_dmc_tb;
    localparam LW = 32*16, NL=64;
    reg             cpu_rd;
    reg [31:0]      cpu_aaddr;
    reg [31:0]      cpu_daddr;
    wire            cpu_hit;
    wire [31:0]     cpu_data;

    reg [LW-1:0]    m_data;
    wire [31:0]     m_addr;
    wire            m_start;
    reg             m_done;

    `TB(ro_dmc_tb, clk, rst_n, 1'b0, 10_000)

    ro_dmc #(.LW(LW), .NL(NL)) duv (
        .clk(clk),
        .rst_n(rst_n),
        .cpu_rd(cpu_rd),
        .cpu_aaddr(cpu_aaddr),
        .cpu_daddr(cpu_daddr),
        .cpu_hit(cpu_hit),
        .cpu_data(cpu_data),
        .m_data(m_data),
        .m_addr(m_addr),
        .m_start(m_start),
        .m_done(m_done)
    );

    // --- Mock Flash Memory Behavior ---
    reg [3:0] flash_delay;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            m_done <= 0;
            m_data <= 0;
            flash_delay <= 0;
        end else begin
            if (m_start && flash_delay == 0) begin
                flash_delay <= 5; // Emulate 5 cycle latency
                m_done <= 0;
            end else if (flash_delay > 1) begin
                flash_delay <= flash_delay - 1;
                m_done <= 0;
            end else if (flash_delay == 1) begin
                flash_delay <= 0;
                m_done <= 1;
                // Generate dummy data: [Word 1=0x11111111, Word 0=0x00000000] etc.
                m_data <= { {224{1'b0}}, 32'h33333333, 32'h22222222, 32'h11111111, 32'h00000000 };
            end else begin
                m_done <= 0;
            end
        end
    end

    // --- AHB Pipeline Emulation ---
    initial begin
        cpu_rd = 0;
        cpu_aaddr = 0;
        cpu_daddr = 0;

        @(posedge rst_n);
        @(posedge clk);

        $display("\n--- Test 1: Cold Miss on 0x8000_0000 ---");
        // Address Phase
        cpu_rd = 1;
        cpu_aaddr = 32'h8000_0000;
        @(posedge clk);

        // Data Phase begins (but cache misses, so bus stalls)
        cpu_rd = 0;
        cpu_daddr = 32'h8000_0000;

        // Wait for cache hit (which happens when m_done fires)
        wait(cpu_hit);
        @(posedge clk);
        $display("Data received: %h (Expected: 00000000)", cpu_data);


        $display("\n--- Test 2: Cache Hit on Next Word (0x8000_0004) ---");
        cpu_rd = 1;
        cpu_aaddr = 32'h8000_0004;
        @(posedge clk);

        cpu_rd = 0;
        cpu_daddr = 32'h8000_0004;
        @(posedge clk);
        $display("Data received: %h (Expected: 11111111) | Hit: %b", cpu_data, cpu_hit);


        $display("\n--- Test 3: Cache Hit on Another Word (0x8000_0008) ---");
        cpu_rd = 1;
        cpu_aaddr = 32'h8000_0008;
        @(posedge clk);

        cpu_rd = 0;
        cpu_daddr = 32'h8000_0008;
        @(posedge clk);
        $display("Data received: %h (Expected: 22222222) | Hit: %b", cpu_data, cpu_hit);

        #100;
        $finish;
    end

endmodule
