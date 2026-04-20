`timescale 1ns / 1ps

module ahbl_flash_ctrl_eb_cache_tb;

    reg         HCLK = 0;
    reg         HRESETn = 0;
    reg [31:0]  HADDR = 0;
    reg [1:0]   HTRANS = 0;
    reg         HWRITE = 0;
    reg         HREADY_IN = 1;
    wire        HREADYOUT;
    wire [31:0] HRDATA;

    wire csn, sck;
    wire [3:0] doe, spi_do, di;

    // Clock Generation
    always #5 HCLK = ~HCLK;

    // DUT
    ahbl_flash_ctrl_eb_cache #(.LW(256)) duv (
        .HCLK(HCLK),
        .HRESETn(HRESETn),
        .HSEL(1'b1),
        .HADDR(HADDR),
        .HTRANS(HTRANS),
        .HWRITE(HWRITE),
        .HREADY(HREADY_IN & HREADYOUT), // Global Bus HREADY
        .HREADYOUT(HREADYOUT),
        .HRDATA(HRDATA),
        .csn(csn), .sck(sck), .doe(doe), .spi_do(spi_do), .di(di)
    );

    // Correctly handle tri-state bidirectional logic per pin based on doe mask
    wire [3:0] SIO;
    assign SIO[0] = doe[0] ? spi_do[0] : 1'bz;
    assign SIO[1] = doe[1] ? spi_do[1] : 1'bz;
    assign SIO[2] = doe[2] ? spi_do[2] : 1'bz;
    assign SIO[3] = doe[3] ? spi_do[3] : 1'bz;

    assign di = SIO;
    sst26wf080b FLASH (.SCK(sck), .SIO(SIO), .CEb(csn));

    initial begin
        $dumpfile("ahbl_cache_tb.vcd");
        $dumpvars(0, ahbl_flash_ctrl_eb_cache_tb);
    end

    // Test Procedure
    // Test Procedure
    initial begin
        // Explicitly define boundaries to silence 1364-2005 warning
        // Assuming your flash memory array is defined as [0:MAX_ADDR]
        #1 $readmemh("init.hex", FLASH.I0.memory, 0, 1048575); // Adjust limit to your model size

        #100 HRESETn = 1;
        repeat(5) @(posedge HCLK);

        $display("\n[%0t] Starting Cached Pipeline Reads...", $time);

        // 1. Cold Miss
        $display("[%0t] Issuing Read to 0x80000000 (Expect Stall)", $time);
        ahb_read(32'h80000000);
        $display("[%0t] Completed 0x00: %h", $time, HRDATA);

        // 2. Cache Hit
        $display("[%0t] Issuing Read to 0x80000004 (Expect Instant Hit)", $time);
        ahb_read(32'h80000004);
        $display("[%0t] Completed 0x04: %h", $time, HRDATA);

        #1000;
        $finish; // End cleanly here. Ensure there are no other $finish calls!
    end

    // AHB-Lite Read Task with Cycle Counting
    task ahb_read(input [31:0] addr);
        integer stall_cycles;
        begin
            stall_cycles = 0;

            // Address Phase
            while(!HREADYOUT) @(posedge HCLK);
            HADDR  <= addr;
            HTRANS <= 2'b10; // NONSEQ
            HWRITE <= 1'b0;

            // Move to Data Phase
            @(posedge HCLK);
            #1;
            HADDR  <= 32'hBAADF00D; // Prove lockout
            HTRANS <= 2'b00;        // IDLE

            // Wait Phase (Count stalls)
            while (!HREADYOUT) begin
                stall_cycles = stall_cycles + 1;
                @(posedge HCLK);
                #1;
            end

            if (stall_cycles > 0)
                $display("      -> Cache MISS. Bus stalled for %0d cycles.", stall_cycles);
            else
                $display("      -> Cache HIT. 0 wait states.");
        end
    endtask
endmodule
