// RTL module equivalents for sky130_fd_sc_hd UDP primitives.
//
// UDP tables (primitive...table...endtable) are not supported by Verilator.
// This file re-declares every UDP used by sky130_fd_sc_hd.v (FUNCTIONAL path)
// as a plain module with always/assign so the gate-level netlist can elaborate.
//
// Semantics preserved from the UDP tables:
//   DFFs:    clock edge captures D; async SET/RESET override (SET dominates)
//   Latches: transparent when GATE=1; async RESET overrides
//   Muxes:   combinational selects
//   Pwrgood: transparent pass-through (VPWR=1/VGND=0 always assumed good)
//
// Power/notifier ports (VPWR, VGND, NOTIFIER, SLEEP) are declared inputs
// and unused — no functional effect in RTL simulation.

`default_nettype none

// ---------------------------------------------------------------------------
// D flip-flops
// ---------------------------------------------------------------------------

// Plain positive-edge DFF
module sky130_fd_sc_hd__udp_dff$P (Q, D, CLK);
    output reg Q;
    input      D, CLK;
    always @(posedge CLK) Q <= D;
endmodule

// Positive-edge DFF with power ports + notifier
module sky130_fd_sc_hd__udp_dff$P_pp$PG$N (Q, D, CLK, NOTIFIER, VPWR, VGND);
    output reg Q;
    input      D, CLK;
    input      NOTIFIER, VPWR, VGND;
    always @(posedge CLK) Q <= D;
endmodule

// Positive-edge DFF with async reset
module sky130_fd_sc_hd__udp_dff$PR (Q, D, CLK, RESET);
    output reg Q;
    input      D, CLK, RESET;
    always @(posedge CLK or posedge RESET)
        if (RESET) Q <= 1'b0;
        else       Q <= D;
endmodule

// Positive-edge DFF with async reset, power ports + notifier
module sky130_fd_sc_hd__udp_dff$PR_pp$PG$N (Q, D, CLK, RESET, NOTIFIER, VPWR, VGND);
    output reg Q;
    input      D, CLK, RESET;
    input      NOTIFIER, VPWR, VGND;
    always @(posedge CLK or posedge RESET)
        if (RESET) Q <= 1'b0;
        else       Q <= D;
endmodule

// Positive-edge DFF with async set
module sky130_fd_sc_hd__udp_dff$PS (Q, D, CLK, SET);
    output reg Q;
    input      D, CLK, SET;
    always @(posedge CLK or posedge SET)
        if (SET) Q <= 1'b1;
        else     Q <= D;
endmodule

// Positive-edge DFF with async set, power ports + notifier
module sky130_fd_sc_hd__udp_dff$PS_pp$PG$N (Q, D, CLK, SET, NOTIFIER, VPWR, VGND);
    output reg Q;
    input      D, CLK, SET;
    input      NOTIFIER, VPWR, VGND;
    always @(posedge CLK or posedge SET)
        if (SET) Q <= 1'b1;
        else     Q <= D;
endmodule

// Negative-edge DFF with async SET (dominates) and RESET.
// CLK_N is the inverted system clock: posedge CLK_N = negedge true clock.
module sky130_fd_sc_hd__udp_dff$NSR (Q, SET, RESET, CLK_N, D);
    output reg Q;
    input      SET, RESET, CLK_N, D;
    always @(posedge CLK_N or posedge SET or posedge RESET)
        if      (SET)   Q <= 1'b1;
        else if (RESET) Q <= 1'b0;
        else            Q <= D;
endmodule

// Negative-edge DFF with SET/RESET, power ports + notifier
module sky130_fd_sc_hd__udp_dff$NSR_pp$PG$N (Q, SET, RESET, CLK_N, D, NOTIFIER, VPWR, VGND);
    output reg Q;
    input      SET, RESET, CLK_N, D;
    input      NOTIFIER, VPWR, VGND;
    always @(posedge CLK_N or posedge SET or posedge RESET)
        if      (SET)   Q <= 1'b1;
        else if (RESET) Q <= 1'b0;
        else            Q <= D;
endmodule

// ---------------------------------------------------------------------------
// Latches
// ---------------------------------------------------------------------------

// Level-sensitive latch, transparent when GATE=1
module sky130_fd_sc_hd__udp_dlatch$P (Q, D, GATE);
    output reg Q;
    input      D, GATE;
    always @(*) if (GATE) Q <= D;
endmodule

// lP variant — identical behaviour, different drive annotation in the PDK
module sky130_fd_sc_hd__udp_dlatch$lP (Q, D, GATE);
    output reg Q;
    input      D, GATE;
    always @(*) if (GATE) Q <= D;
endmodule

// Latch with power ports + notifier
module sky130_fd_sc_hd__udp_dlatch$P_pp$PG$N (Q, D, GATE, NOTIFIER, VPWR, VGND);
    output reg Q;
    input      D, GATE;
    input      NOTIFIER, VPWR, VGND;
    always @(*) if (GATE) Q <= D;
endmodule

// lP with power ports + notifier
module sky130_fd_sc_hd__udp_dlatch$lP_pp$PG$N (Q, D, GATE, NOTIFIER, VPWR, VGND);
    output reg Q;
    input      D, GATE;
    input      NOTIFIER, VPWR, VGND;
    always @(*) if (GATE) Q <= D;
endmodule

// Level-sensitive latch with async reset
module sky130_fd_sc_hd__udp_dlatch$PR (Q, D, GATE, RESET);
    output reg Q;
    input      D, GATE, RESET;
    always @(*)
        if      (RESET) Q <= 1'b0;
        else if (GATE)  Q <= D;
endmodule

// Latch with async reset, power ports + notifier
module sky130_fd_sc_hd__udp_dlatch$PR_pp$PG$N (Q, D, GATE, RESET, NOTIFIER, VPWR, VGND);
    output reg Q;
    input      D, GATE, RESET;
    input      NOTIFIER, VPWR, VGND;
    always @(*)
        if      (RESET) Q <= 1'b0;
        else if (GATE)  Q <= D;
endmodule

// ---------------------------------------------------------------------------
// Muxes
// ---------------------------------------------------------------------------

module sky130_fd_sc_hd__udp_mux_2to1 (X, A0, A1, S);
    output wire X;
    input       A0, A1, S;
    assign X = S ? A1 : A0;
endmodule

module sky130_fd_sc_hd__udp_mux_2to1_N (Y, A0, A1, S);
    output wire Y;
    input       A0, A1, S;
    assign Y = ~(S ? A1 : A0);
endmodule

module sky130_fd_sc_hd__udp_mux_4to2 (X, A0, A1, A2, A3, S0, S1);
    output wire X;
    input       A0, A1, A2, A3, S0, S1;
    assign X = S1 ? (S0 ? A3 : A2) : (S0 ? A1 : A0);
endmodule

// ---------------------------------------------------------------------------
// Power-good primitives — transparent pass-through.
// In real silicon these force the output to X when supply rails are absent.
// For functional GLS simulation VPWR=1 / VGND=0 is guaranteed, so the
// output simply follows the input.
// ---------------------------------------------------------------------------

module sky130_fd_sc_hd__udp_pwrgood_pp$PG (UDP_OUT, UDP_IN, VPWR, VGND);
    output wire UDP_OUT;
    input       UDP_IN, VPWR, VGND;
    assign UDP_OUT = UDP_IN;
endmodule

module sky130_fd_sc_hd__udp_pwrgood_pp$P (UDP_OUT, UDP_IN, VPWR);
    output wire UDP_OUT;
    input       UDP_IN, VPWR;
    assign UDP_OUT = UDP_IN;
endmodule

module sky130_fd_sc_hd__udp_pwrgood_pp$G (UDP_OUT, UDP_IN, VGND);
    output wire UDP_OUT;
    input       UDP_IN, VGND;
    assign UDP_OUT = UDP_IN;
endmodule

module sky130_fd_sc_hd__udp_pwrgood$l_pp$PG (UDP_OUT, UDP_IN, VPWR, VGND);
    output wire UDP_OUT;
    input       UDP_IN, VPWR, VGND;
    assign UDP_OUT = UDP_IN;
endmodule

module sky130_fd_sc_hd__udp_pwrgood$l_pp$G (UDP_OUT, UDP_IN, VGND);
    output wire UDP_OUT;
    input       UDP_IN, VGND;
    assign UDP_OUT = UDP_IN;
endmodule

module sky130_fd_sc_hd__udp_pwrgood$l_pp$PG$S (UDP_OUT, UDP_IN, VPWR, VGND, SLEEP);
    output wire UDP_OUT;
    input       UDP_IN, VPWR, VGND, SLEEP;
    assign UDP_OUT = UDP_IN;
endmodule

`default_nettype wire
