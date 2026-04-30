`timescale 1ns / 1ps

module flash_ctrl_eb_tb;
    reg          clk = 0;
    reg          rst_n = 0;
    reg          start;
    wire         done;

    reg [23:0]   A;
    wire [255:0] D;
    wire         csn;
    wire         sck;
    wire [3:0]   doe;
    wire [3:0]   spi_do;
    wire [3:0]   di;

    flash_ctrl_eb #(.LW(256)) duv (
        .clk(clk),
        .rst_n(rst_n),
        .start(start),
        .done(done),
        .A(A),
        .D(D),
        .csn(csn),
        .sck(sck),
        .doe(doe),
        .spi_do(spi_do),
        .di(di)
    );

    wire [3:0] SIO;
    // Correctly handle tri-state bidirectional logic based on doe mask
    assign SIO[0] = doe[0] ? spi_do[0] : 1'bz;
    assign SIO[1] = doe[1] ? spi_do[1] : 1'bz;
    assign SIO[2] = doe[2] ? spi_do[2] : 1'bz;
    assign SIO[3] = doe[3] ? spi_do[3] : 1'bz;

    assign di = SIO;

    sst26wf080b FLASH (.SCK(sck), .SIO(SIO), .CEb(csn));

    // 50 MHz Clock
    always #10 clk = !clk;

    initial begin
        $dumpfile("flash_ctrl_eb_tb.vcd");
        $dumpvars(0, flash_ctrl_eb_tb);

        // Explicity define bounds to stop the 1364-2005 ambiguity warning
        #1 $readmemh("init.hex", FLASH.I0.memory, 0, 1048575);
    end

    initial begin
        start = 0;
        A = 24'b0;

        $display("\n==========================================");
        $display("[%0t] Starting SPI Flash Controller Test", $time);
        $display("==========================================");

        // Power-on Reset
        #100;
        @(posedge clk);
        rst_n = 1;
        $display("[%0t] System Reset Released.", $time);

        // Test 1: Cold Fetch (will issue 66, 99, and EB)
        flash_fetch(24'h000000);

        // Test 2: CRM Fetch (Should skip 66 and 99, and jump straight to Address)
        flash_fetch(24'h000020);

        $display("\n==========================================");
        $display("[%0t] All Tests Completed.", $time);
        $display("==========================================\n");
        #100;
        $finish;
    end

    // --- Verification Task ---
    task flash_fetch(input [23:0] addr);
        integer wait_cycles;
    begin
        @(posedge clk);
        A = addr;
        start = 1;

        @(posedge clk);
        start = 0;

        $display("\n[%0t] ---> Requesting Fetch at Address: 0x%06x", $time, addr);

        wait_cycles = 0;
        // Standard Verilog-2005 compatible timeout loop
        while (!done && wait_cycles < 5000) begin
            @(posedge clk);
            wait_cycles = wait_cycles + 1;
        end

        if (wait_cycles >= 5000) begin
            $display("[%0t] [FAILED] Timeout! Controller never asserted 'done'.", $time);
            $finish;
        end else begin
            $display("[%0t] <--- Fetch Complete! Data Received: %h", $time, D);

            if (D === 256'bx) begin
                $display("    [FAILED] Bus received undefined (X) data.");
            end else if (D == 256'b0) begin
                $display("    [WARNING] Data is all zeroes. Check init.hex contents.");
            end else begin
                $display("    [PASSED] Valid data block extracted.");
            end
        end
    end
    endtask

endmodule
