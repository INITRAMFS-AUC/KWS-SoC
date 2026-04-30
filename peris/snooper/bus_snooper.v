// SPDX-License-Identifier: Apache-2.0
//
// Bus snooper: APB slave + 16-entry ring buffer that captures bridge-side
// transactions alongside CPU M-stage forwarding signals. Built to confirm the
// 2-port d-port → APB-bridge HWDATA corruption documented in
// docs/2port_dport_bridge_bug.md.
//
// Memory map (paddr offsets within the slave's 16 KB region at 0x4000_C000):
//   0x000 .. 0x1FC  Ring buffer (16 entries x 32 bytes; one word per offset)
//                     entry layout (32 bytes per entry):
//                       +0x00 cycle counter at commit
//                       +0x04 addr_ctrl word — bridge / CPU-d-port / M-stage aphase state:
//                               [15:0]  bridge_haddr[15:0]
//                               [16]    bridge_hwrite
//                               [18:17] bridge_htrans[1:0]
//                               [19]    dport_hwrite          (CPU d-port aphase)
//                               [21:20] dport_htrans[1:0]     (CPU d-port aphase)
//                               [22]    bus_aph_req_d         (CPU asks for an aphase)
//                               [27:23] xm_memop[4:0]         (5'h10=NONE, 5'h05=SW, 5'h04=LBU, ...)
//                               [28]    m_bus_stall           (M-stage held by bus dphase)
//                               [31:29] reserved (zero)
//                       +0x08 bridge_hwdata sampled at dphase end
//                       +0x0C dbg_m_wdata   sampled at dphase end
//                       +0x10 {19'h0, dbg_mw_rd[4:0], 3'h0, dbg_xm_rs2[4:0]}
//                       +0x14 dbg_xm_result
//                       +0x18 dbg_mw_result
//                       +0x1C dport_haddr[31:0] — CPU d-port haddr at the same aphase the
//                                                bridge captured. If this differs from the
//                                                bridge_haddr at +0x04, the crossbar is
//                                                misrouting (fabric bug). If they match,
//                                                the CPU itself emitted the wrong haddr.
//   0x200 CTRL  (RW): bit0 enable
//   0x204 STAT  (RO): {19'h0, head[3:0], 4'h0, count[4:0]}
//   0x208 CYCLE (RO): free-running cycle counter (low 32 bits)
//   0x20C RESET (W1C): write 1 to clear ring + head + count + pending
//
// Capture sequencing: AHB-Lite address phase precedes data phase by one cycle.
// We latch addr/control in aphase, and commit one entry on the cycle when
// bridge_hready=1 ends the data phase, alongside bridge_hwdata and the CPU taps
// (which drive bus_wdata_d in that same cycle).

module bus_snooper #(
    parameter W_DATA    = 32,
    parameter N_ENTRIES = 16
) (
    input  wire              clk,
    input  wire              rst_n,

    // APB slave (control + ring readout)
    input  wire              apbs_psel,
    input  wire              apbs_penable,
    input  wire              apbs_pwrite,
    input  wire [15:0]       apbs_paddr,
    input  wire [W_DATA-1:0] apbs_pwdata,
    output reg  [W_DATA-1:0] apbs_prdata,
    output wire              apbs_pready,
    output wire              apbs_pslverr,

    // AHB bridge interface taps (snooped, never driven)
    input  wire [15:0]       bridge_haddr,
    input  wire              bridge_hwrite,
    input  wire [1:0]        bridge_htrans,
    input  wire              bridge_hready,
    input  wire [W_DATA-1:0] bridge_hwdata,

    // CPU d-port taps (between hazard3_cpu_2port output and the crossbar input).
    // Captured in the same aphase as the bridge taps so we can tell whether a
    // bridge_haddr differs from what the CPU actually emitted (fabric misroute)
    // vs. the CPU emitting the wrong address itself (CPU bug).
    input  wire [W_DATA-1:0] dport_haddr,
    input  wire              dport_hwrite,
    input  wire [1:0]        dport_htrans,

    // CPU-internal taps (M-stage forwarding observability)
    input  wire [4:0]        dbg_xm_rs2,
    input  wire [4:0]        dbg_mw_rd,
    input  wire [W_DATA-1:0] dbg_xm_result,
    input  wire [W_DATA-1:0] dbg_mw_result,
    input  wire [W_DATA-1:0] dbg_m_wdata,

    // Pipelining-state taps. Captured at aphase along with dport_*.
    input  wire              dbg_bus_aph_req_d,
    input  wire [4:0]        dbg_xm_memop,
    input  wire              dbg_m_bus_stall
);

assign apbs_pready  = 1'b1;
assign apbs_pslverr = 1'b0;

localparam W_IDX = 4;  // log2(N_ENTRIES); fixed at 16

reg [W_DATA-1:0] r_cycle    [0:N_ENTRIES-1];
reg [W_DATA-1:0] r_addrctrl [0:N_ENTRIES-1];
reg [W_DATA-1:0] r_hwdata   [0:N_ENTRIES-1];
reg [W_DATA-1:0] r_mwdata   [0:N_ENTRIES-1];
reg [W_DATA-1:0] r_idx      [0:N_ENTRIES-1];
reg [W_DATA-1:0] r_xmres    [0:N_ENTRIES-1];
reg [W_DATA-1:0] r_mwres    [0:N_ENTRIES-1];
reg [W_DATA-1:0] r_dport_a  [0:N_ENTRIES-1];

reg [W_IDX-1:0]  head;
reg [W_IDX:0]    count;
reg [W_DATA-1:0] cycle_ctr;
reg              enable;

reg              pend_vld;
reg [15:0]       pend_haddr;
reg              pend_hwrite;
reg [1:0]        pend_htrans;
reg [W_DATA-1:0] pend_dport_haddr;
reg              pend_dport_hwrite;
reg [1:0]        pend_dport_htrans;
reg              pend_aph_req;
reg [4:0]        pend_xm_memop;
reg              pend_m_bus_stall;

wire wen = apbs_psel && apbs_penable && apbs_pwrite;

localparam [9:0] ADDR_CTRL  = 10'h200;
localparam [9:0] ADDR_STAT  = 10'h204;
localparam [9:0] ADDR_CYCLE = 10'h208;
localparam [9:0] ADDR_RESET = 10'h20c;

wire [9:0] saddr = apbs_paddr[9:0];

wire ctrl_wen  = wen && (saddr == ADDR_CTRL);
wire reset_wen = wen && (saddr == ADDR_RESET) && apbs_pwdata[0];

wire commit       = enable && pend_vld    && bridge_hready;
wire latch_aphase = enable && bridge_htrans[1] && (!pend_vld || bridge_hready);

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        head              <= {W_IDX{1'b0}};
        count             <= {(W_IDX+1){1'b0}};
        cycle_ctr         <= {W_DATA{1'b0}};
        enable            <= 1'b0;
        pend_vld          <= 1'b0;
        pend_haddr        <= 16'h0;
        pend_hwrite       <= 1'b0;
        pend_htrans       <= 2'b00;
        pend_dport_haddr  <= {W_DATA{1'b0}};
        pend_dport_hwrite <= 1'b0;
        pend_dport_htrans <= 2'b00;
        pend_aph_req      <= 1'b0;
        pend_xm_memop     <= 5'h10;     // MEMOP_NONE
        pend_m_bus_stall  <= 1'b0;
    end else begin
        cycle_ctr <= cycle_ctr + 1'b1;

        if (ctrl_wen) begin
            enable <= apbs_pwdata[0];
        end

        if (reset_wen) begin
            head     <= {W_IDX{1'b0}};
            count    <= {(W_IDX+1){1'b0}};
            pend_vld <= 1'b0;
        end else begin
            if (commit) begin
                r_cycle   [head] <= cycle_ctr;
                r_addrctrl[head] <= {3'h0,
                                     pend_m_bus_stall,
                                     pend_xm_memop,
                                     pend_aph_req,
                                     pend_dport_htrans, pend_dport_hwrite,
                                     pend_htrans,       pend_hwrite,
                                     pend_haddr};
                r_hwdata  [head] <= bridge_hwdata;
                r_mwdata  [head] <= dbg_m_wdata;
                r_idx     [head] <= {19'h0, dbg_mw_rd, 3'h0, dbg_xm_rs2};
                r_xmres   [head] <= dbg_xm_result;
                r_mwres   [head] <= dbg_mw_result;
                r_dport_a [head] <= pend_dport_haddr;
                head <= head + 1'b1;
                if (count != N_ENTRIES[W_IDX:0])
                    count <= count + 1'b1;
            end

            if (latch_aphase) begin
                pend_vld          <= 1'b1;
                pend_haddr        <= bridge_haddr;
                pend_hwrite       <= bridge_hwrite;
                pend_htrans       <= bridge_htrans;
                pend_dport_haddr  <= dport_haddr;
                pend_dport_hwrite <= dport_hwrite;
                pend_dport_htrans <= dport_htrans;
                pend_aph_req      <= dbg_bus_aph_req_d;
                pend_xm_memop     <= dbg_xm_memop;
                pend_m_bus_stall  <= dbg_m_bus_stall;
            end else if (commit) begin
                pend_vld <= 1'b0;
            end
        end
    end
end

// APB read demux

reg  [W_DATA-1:0]  csr_rdata;
reg  [W_DATA-1:0]  ring_rdata;
wire [W_IDX-1:0]   r_ent_idx  = apbs_paddr[8:5];
wire [2:0]         r_word_idx = apbs_paddr[4:2];

always @(*) begin
    case (r_word_idx)
        3'd0:    ring_rdata = r_cycle   [r_ent_idx];
        3'd1:    ring_rdata = r_addrctrl[r_ent_idx];
        3'd2:    ring_rdata = r_hwdata  [r_ent_idx];
        3'd3:    ring_rdata = r_mwdata  [r_ent_idx];
        3'd4:    ring_rdata = r_idx     [r_ent_idx];
        3'd5:    ring_rdata = r_xmres   [r_ent_idx];
        3'd6:    ring_rdata = r_mwres   [r_ent_idx];
        3'd7:    ring_rdata = r_dport_a [r_ent_idx];
        default: ring_rdata = 32'h0;
    endcase
end

always @(*) begin
    case (saddr)
        ADDR_CTRL:  csr_rdata = {31'h0, enable};
        ADDR_STAT:  csr_rdata = {{(32-(W_IDX+1)-4-W_IDX){1'b0}}, head, 4'h0, count};
        ADDR_CYCLE: csr_rdata = cycle_ctr;
        default:    csr_rdata = 32'h0;
    endcase
end

always @(*) begin
    apbs_prdata = apbs_paddr[9] ? csr_rdata : ring_rdata;
end

endmodule
