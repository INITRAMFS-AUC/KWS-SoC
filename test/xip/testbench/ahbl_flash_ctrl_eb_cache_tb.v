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
    wire [3:0] doe, do, di;

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
        .csn(csn), .sck(sck), .doe(doe), .do(do), .di(di)
    );

    // Flash Model Connection
    wire [3:0] SIO = (doe == 4'b1111) ? do : 4'bzzzz;
    assign di = SIO;
    sst26wf080b FLASH (.SCK(sck), .SIO(SIO), .CEb(csn));

    // Monitor
    initial begin
        $dumpfile("ahbl_cacheless_tb.vcd");
        $dumpvars(0, ahbl_flash_ctrl_eb_cache_tb);
    end

    // Test Procedure
    initial begin
        // 1. Load the Flash memory!
        #1 $readmemh("init.hex", FLASH.I0.memory);

        // 2. Reset
        #100 HRESETn = 1;
        repeat(5) @(posedge HCLK);

        // 3. Sequential Read Test (0x00, 0x04, 0x08)
        // Since this is cacheless, each one will trigger a full SPI fetch
        $display("[%0t] Starting Cacheless Reads...", $time);

        ahb_read(32'h80000000);
        $display("[%0t] Read 0x00: %h (Expected: 0100006f)", $time, HRDATA);

        ahb_read(32'h80000004);
        $display("[%0t] Read 0x04: %h", $time, HRDATA);

        ahb_read(32'h80000018);
        $display("[%0t] Read 0x18: %h (Expected: 30529073)", $time, HRDATA);

        #1000;
        $finish;
    end

    // Simple AHB-Lite Read Task
    task ahb_read(input [31:0] addr);
        begin
            // 1. Wait for Slave to be ready for Address Phase
            while(!HREADYOUT) @(posedge HCLK);

            // 2. Drive Address Phase
            HADDR  <= addr;
            HTRANS <= 2'b10;
            HWRITE <= 1'b0;

            // 3. Move to Data Phase (Slave will drop HREADYOUT on this edge)
            @(posedge HCLK);
            #1; // Delay to allow Slave state machine to transition
            HADDR  <= 32'hBAADF00D; // Change address to prove lockout
            HTRANS <= 2'b00;

            // 4. Wait for Slave to finish Fetching
            // Data is valid ONLY when HREADYOUT returns to 1
            while(!HREADYOUT) @(posedge HCLK);

            // The rising edge has occurred where HREADYOUT is 1.
            // In a real CPU, the data is sampled EXACTLY at this posedge.
            $display("[%0t] TB: Sampled HRDATA for %h = %h", $time, addr, HRDATA);
        end
    endtask
endmodule
