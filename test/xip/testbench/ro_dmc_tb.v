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
        .cpu_ahit(cpu_hit),
        .cpu_data(cpu_data),
        .m_data(m_data),
        .m_addr(m_addr),
        .m_start(m_start),
        .m_done(m_done)
    );

    // --- Smart Mock Flash Memory ---
    reg [3:0] flash_delay;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            m_done <= 0;
            m_data <= 0;
            flash_delay <= 0;
        end else begin
            if (m_start && flash_delay == 0) begin
                flash_delay <= 5;
                m_done <= 0;
            end else if (flash_delay > 1) begin
                flash_delay <= flash_delay - 1;
                m_done <= 0;
            end else if (flash_delay == 1) begin
                flash_delay <= 0;
                m_done <= 1;
                // Generate dynamic data based on the requested address
                // Word 0 = base addr, Word 1 = base+4, Word 2 = base+8...
                m_data <= { {256{1'b0}},
                            m_addr + 32'h1c, m_addr + 32'h18,
                            m_addr + 32'h14, m_addr + 32'h10,
                            m_addr + 32'h0c, m_addr + 32'h08,
                            m_addr + 32'h04, m_addr };
            end else begin
                m_done <= 0;
            end
        end
    end

    // --- AHB Pipeline Emulation Task ---
    task ahb_read(input [31:0] addr);
    begin
        // 1. Address Phase Setup
        cpu_rd    <= 1;
        cpu_aaddr <= addr;

        // Wait for the clock edge where the cache samples the request
        @(posedge clk);

        // 2. Emulate AHB Stall (HREADY)
        // We use #1 to avoid delta-cycle race conditions with combinational logic
        #1;
        while (!cpu_hit) begin
            @(posedge clk);
            #1;
        end

        // 3. Move to Data Phase
        cpu_rd    <= 0;
        cpu_daddr <= addr;
        @(posedge clk);
    end
    endtask

    initial begin
        cpu_rd = 0;
        cpu_aaddr = 0;
        cpu_daddr = 0;

        @(posedge rst_n);
        @(posedge clk);

        $display("\n--- Test 1: Cold Miss on 0x8000_0000 (Index 0) ---");
        ahb_read(32'h8000_0000);
        $display("Data received: %h (Expected: 80000000) | Hit: %b", cpu_data, cpu_hit);

        $display("\n--- Test 2: Cache Hit on Next Word 0x8000_0004 ---");
        ahb_read(32'h8000_0004);
        $display("Data received: %h (Expected: 80000004) | Hit: %b", cpu_data, cpu_hit);

        $display("\n--- Test 3: Eviction/Collision Miss on 0x8000_1000 ---");
        ahb_read(32'h8000_1000);
        $display("Data received: %h (Expected: 80001000) | Hit: %b", cpu_data, cpu_hit);

        $display("\n--- Test 4: Thrashing Miss back to 0x8000_0000 ---");
        ahb_read(32'h8000_0000);
        $display("Data received: %h (Expected: 80000000) | Hit: %b", cpu_data, cpu_hit);

        #50;
        $finish;
    end
endmodule
