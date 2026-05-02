`timescale 1ns / 1ps

module ahbl_flash_ctrl_eb_cache #(parameter LW=256, NL=32) (
    input   wire                HCLK,
    input   wire                HRESETn,
    input   wire                HSEL,
    input   wire [31:0]         HADDR,
    input   wire [1:0]          HTRANS,
    input   wire                HWRITE,
    input   wire                HREADY,
    output  wire                HREADYOUT,
    output  wire [31:0]         HRDATA,
    output  wire                HRESP,

    // External Interface to Quad I/O
    output wire                 csn,
    output wire                 sck,
    output wire [3:0]           doe,
    output wire [3:0]           spi_do,
    input  wire [3:0]           di
);

    assign HRESP = 1'b0; // Always OKAY

    // --- AHB Pipeline Tracking ---
    // A valid address phase request
    wire address_phase_req = HSEL && HTRANS[1] && !HWRITE && HREADY;

    // We must track what is happening in the DATA phase to drive HREADYOUT correctly
    reg        dphase_active;
    reg [31:0] dphase_addr;

    always @(posedge HCLK or negedge HRESETn) begin
        if (!HRESETn) begin
            dphase_active <= 1'b0;
            dphase_addr   <= 32'b0;
        end else if (HREADY) begin
            // When the bus advances, latch the address phase into the data phase
            dphase_active <= HSEL && HTRANS[1] && !HWRITE;
            dphase_addr   <= HADDR;
        end
    end

    // --- Cache Instantiation ---
    wire        cpu_ahit;
    wire        cpu_dhit;
    wire        m_start;
    wire [31:0] m_addr;
    wire [31:0] cpu_data;

    reg         m_done_reg;
    wire [LW-1:0]    flash_data_bus;
    wire [LW/32-1:0] flash_word_done;

    ro_dmc #(.LW(LW), .NL(NL)) cache_inst (
        .clk(HCLK),
        .rst_n(HRESETn),
        .cpu_rd(address_phase_req), // Address phase triggers a cache check
        .cpu_aaddr(HADDR),          // Address phase address
        .cpu_daddr(dphase_addr),    // Data phase address
        .cpu_dvalid(dphase_active), // Used by CWF deferred-miss rescue
        .cpu_ahit(cpu_ahit),
        .cpu_dhit(cpu_dhit),
        .cpu_data(cpu_data),

        // Memory Interface to FSM
        .m_data(flash_data_bus),
        .m_word_done(flash_word_done),
        .m_addr(m_addr),
        .m_start(m_start),
        .m_done(m_done_reg)
    );

    // --- AHB Output Logic ---
    assign HRDATA = cpu_data;
    // Stall the bus only if we are actively in a data phase and the cache missed
    assign HREADYOUT = dphase_active ? cpu_dhit : 1'b1;

    // --- Flash Fetch FSM ---
    localparam ST_IDLE      = 2'd0;
    localparam ST_START_CMD = 2'd1;
    localparam ST_WAIT_LOW  = 2'd2;
    localparam ST_WAIT_HIGH = 2'd3;

    reg [1:0] state;
    reg       flash_start_reg;
    wire      flash_done;

    always @(posedge HCLK or negedge HRESETn) begin
        if (!HRESETn) begin
            state           <= ST_IDLE;
            flash_start_reg <= 1'b0;
            m_done_reg      <= 1'b0;
        end else begin
            m_done_reg <= 1'b0; // Default to 0 (pulse for 1 cycle)

            case (state)
                ST_IDLE: begin
                    // Cache missed during an address phase, start fetch!
                    if (m_start) begin
                        flash_start_reg <= 1'b1;
                        state           <= ST_START_CMD;
                    end
                end

                ST_START_CMD: begin
                    flash_start_reg <= 1'b0;
                    state           <= ST_WAIT_LOW;
                end

                ST_WAIT_LOW: begin
                    if (!flash_done) state <= ST_WAIT_HIGH;
                end

                ST_WAIT_HIGH: begin
                    if (flash_done) begin
                        m_done_reg <= 1'b1; // Tell the cache to latch the data!
                        state      <= ST_IDLE;
                    end
                end
            endcase
        end
    end

    // --- Flash Controller ---
    flash_ctrl_eb #(.LW(LW)) flash_ctrl (
        .clk(HCLK),
        .rst_n(HRESETn),
        .start(flash_start_reg),
        .done(flash_done),
        // Send the latched miss address to the flash controller, aligned to 32 bytes
        .A({m_addr[23:5], 5'b00000}),
        .D(flash_data_bus),
        .word_done(flash_word_done),
        .csn(csn), .sck(sck), .doe(doe), .spi_do(spi_do), .di(di)
    );

    always @(posedge HCLK) begin
        if (address_phase_req) begin
`ifdef XIP_DEBUG
            $display("[AHB PROBE] XIP Wrapper received read request for Address: %h", HADDR);
`endif
        end
    end

endmodule
