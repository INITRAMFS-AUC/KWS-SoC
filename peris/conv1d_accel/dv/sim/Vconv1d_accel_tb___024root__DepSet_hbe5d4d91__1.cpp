// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vconv1d_accel_tb.h for the primary calling header

#include "Vconv1d_accel_tb__pch.h"
#include "Vconv1d_accel_tb___024root.h"

VL_INLINE_OPT void Vconv1d_accel_tb___024root___nba_comb__TOP__0(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___nba_comb__TOP__0\n"); );
    // Body
    vlSelf->conv1d_accel_tb__DOT__hrdata = 0xdeadbeefU;
    if (((IData)(vlSelf->conv1d_accel_tb__DOT__ap_valid) 
         & (~ (IData)(vlSelf->conv1d_accel_tb__DOT__ap_wr)))) {
        vlSelf->conv1d_accel_tb__DOT__hrdata = ((2U 
                                                 == (IData)(vlSelf->conv1d_accel_tb__DOT__ap_sz))
                                                 ? 
                                                ((vlSelf->conv1d_accel_tb__DOT__mem
                                                  [
                                                  (0xffffU 
                                                   & ((IData)(3U) 
                                                      + vlSelf->conv1d_accel_tb__DOT__ap_addr))] 
                                                  << 0x18U) 
                                                 | ((vlSelf->conv1d_accel_tb__DOT__mem
                                                     [
                                                     (0xffffU 
                                                      & ((IData)(2U) 
                                                         + vlSelf->conv1d_accel_tb__DOT__ap_addr))] 
                                                     << 0x10U) 
                                                    | ((vlSelf->conv1d_accel_tb__DOT__mem
                                                        [
                                                        (0xffffU 
                                                         & ((IData)(1U) 
                                                            + vlSelf->conv1d_accel_tb__DOT__ap_addr))] 
                                                        << 8U) 
                                                       | vlSelf->conv1d_accel_tb__DOT__mem
                                                       [
                                                       (0xffffU 
                                                        & vlSelf->conv1d_accel_tb__DOT__ap_addr)])))
                                                 : 
                                                ((vlSelf->conv1d_accel_tb__DOT__mem
                                                  [
                                                  (3U 
                                                   | (0xfffcU 
                                                      & vlSelf->conv1d_accel_tb__DOT__ap_addr))] 
                                                  << 0x18U) 
                                                 | ((vlSelf->conv1d_accel_tb__DOT__mem
                                                     [
                                                     (2U 
                                                      | (0xfffcU 
                                                         & vlSelf->conv1d_accel_tb__DOT__ap_addr))] 
                                                     << 0x10U) 
                                                    | ((vlSelf->conv1d_accel_tb__DOT__mem
                                                        [
                                                        (1U 
                                                         | (0xfffcU 
                                                            & vlSelf->conv1d_accel_tb__DOT__ap_addr))] 
                                                        << 8U) 
                                                       | vlSelf->conv1d_accel_tb__DOT__mem
                                                       [
                                                       (0xfffcU 
                                                        & vlSelf->conv1d_accel_tb__DOT__ap_addr)]))));
    }
}

void Vconv1d_accel_tb___024root___nba_sequent__TOP__0(Vconv1d_accel_tb___024root* vlSelf);
void Vconv1d_accel_tb___024root___nba_sequent__TOP__1(Vconv1d_accel_tb___024root* vlSelf);
void Vconv1d_accel_tb___024root___nba_sequent__TOP__2(Vconv1d_accel_tb___024root* vlSelf);

void Vconv1d_accel_tb___024root___eval_nba(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___eval_nba\n"); );
    // Body
    if ((1ULL & vlSelf->__VnbaTriggered.word(0U))) {
        Vconv1d_accel_tb___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((2ULL & vlSelf->__VnbaTriggered.word(0U))) {
        Vconv1d_accel_tb___024root___nba_sequent__TOP__1(vlSelf);
    }
    if ((1ULL & vlSelf->__VnbaTriggered.word(0U))) {
        Vconv1d_accel_tb___024root___nba_sequent__TOP__2(vlSelf);
        vlSelf->__Vm_traceActivity[3U] = 1U;
    }
    if ((3ULL & vlSelf->__VnbaTriggered.word(0U))) {
        Vconv1d_accel_tb___024root___nba_comb__TOP__0(vlSelf);
    }
}

void Vconv1d_accel_tb___024root___timing_resume(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___timing_resume\n"); );
    // Body
    if ((2ULL & vlSelf->__VactTriggered.word(0U))) {
        vlSelf->__VtrigSched_h25d0ce37__0.resume("@(posedge conv1d_accel_tb.clk)");
    }
    if ((8ULL & vlSelf->__VactTriggered.word(0U))) {
        vlSelf->__VtrigSched_h635928e4__0.resume("@(posedge conv1d_accel_tb.rst_n)");
    }
    if ((4ULL & vlSelf->__VactTriggered.word(0U))) {
        vlSelf->__VdlySched.resume();
    }
}

void Vconv1d_accel_tb___024root___timing_commit(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___timing_commit\n"); );
    // Body
    if ((! (2ULL & vlSelf->__VactTriggered.word(0U)))) {
        vlSelf->__VtrigSched_h25d0ce37__0.commit("@(posedge conv1d_accel_tb.clk)");
    }
    if ((! (8ULL & vlSelf->__VactTriggered.word(0U)))) {
        vlSelf->__VtrigSched_h635928e4__0.commit("@(posedge conv1d_accel_tb.rst_n)");
    }
}

void Vconv1d_accel_tb___024root___eval_triggers__act(Vconv1d_accel_tb___024root* vlSelf);
void Vconv1d_accel_tb___024root___eval_act(Vconv1d_accel_tb___024root* vlSelf);

bool Vconv1d_accel_tb___024root___eval_phase__act(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___eval_phase__act\n"); );
    // Init
    VlTriggerVec<4> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vconv1d_accel_tb___024root___eval_triggers__act(vlSelf);
    Vconv1d_accel_tb___024root___timing_commit(vlSelf);
    __VactExecute = vlSelf->__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelf->__VactTriggered, vlSelf->__VnbaTriggered);
        vlSelf->__VnbaTriggered.thisOr(vlSelf->__VactTriggered);
        Vconv1d_accel_tb___024root___timing_resume(vlSelf);
        Vconv1d_accel_tb___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vconv1d_accel_tb___024root___eval_phase__nba(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___eval_phase__nba\n"); );
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelf->__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vconv1d_accel_tb___024root___eval_nba(vlSelf);
        vlSelf->__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vconv1d_accel_tb___024root___dump_triggers__nba(Vconv1d_accel_tb___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vconv1d_accel_tb___024root___dump_triggers__act(Vconv1d_accel_tb___024root* vlSelf);
#endif  // VL_DEBUG

void Vconv1d_accel_tb___024root___eval(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___eval\n"); );
    // Init
    IData/*31:0*/ __VnbaIterCount;
    CData/*0:0*/ __VnbaContinue;
    // Body
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY((0x64U < __VnbaIterCount))) {
#ifdef VL_DEBUG
            Vconv1d_accel_tb___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("conv1d_accel_tb.v", 21, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelf->__VactIterCount = 0U;
        vlSelf->__VactContinue = 1U;
        while (vlSelf->__VactContinue) {
            if (VL_UNLIKELY((0x64U < vlSelf->__VactIterCount))) {
#ifdef VL_DEBUG
                Vconv1d_accel_tb___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("conv1d_accel_tb.v", 21, "", "Active region did not converge.");
            }
            vlSelf->__VactIterCount = ((IData)(1U) 
                                       + vlSelf->__VactIterCount);
            vlSelf->__VactContinue = 0U;
            if (Vconv1d_accel_tb___024root___eval_phase__act(vlSelf)) {
                vlSelf->__VactContinue = 1U;
            }
        }
        if (Vconv1d_accel_tb___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vconv1d_accel_tb___024root___eval_debug_assertions(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___eval_debug_assertions\n"); );
}
#endif  // VL_DEBUG
