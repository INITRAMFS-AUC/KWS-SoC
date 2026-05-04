// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vconv1d_accel_tb.h for the primary calling header

#include "Vconv1d_accel_tb__pch.h"
#include "Vconv1d_accel_tb___024root.h"

VL_ATTR_COLD void Vconv1d_accel_tb___024root___eval_static__TOP(Vconv1d_accel_tb___024root* vlSelf);

VL_ATTR_COLD void Vconv1d_accel_tb___024root___eval_static(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___eval_static\n"); );
    // Body
    Vconv1d_accel_tb___024root___eval_static__TOP(vlSelf);
}

VL_ATTR_COLD void Vconv1d_accel_tb___024root___eval_static__TOP(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___eval_static__TOP\n"); );
    // Body
    vlSelf->conv1d_accel_tb__DOT__clk = 0U;
    vlSelf->conv1d_accel_tb__DOT__rst_n = 0U;
}

VL_ATTR_COLD void Vconv1d_accel_tb___024root___eval_final(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___eval_final\n"); );
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vconv1d_accel_tb___024root___dump_triggers__stl(Vconv1d_accel_tb___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vconv1d_accel_tb___024root___eval_phase__stl(Vconv1d_accel_tb___024root* vlSelf);

VL_ATTR_COLD void Vconv1d_accel_tb___024root___eval_settle(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___eval_settle\n"); );
    // Init
    IData/*31:0*/ __VstlIterCount;
    CData/*0:0*/ __VstlContinue;
    // Body
    __VstlIterCount = 0U;
    vlSelf->__VstlFirstIteration = 1U;
    __VstlContinue = 1U;
    while (__VstlContinue) {
        if (VL_UNLIKELY((0x64U < __VstlIterCount))) {
#ifdef VL_DEBUG
            Vconv1d_accel_tb___024root___dump_triggers__stl(vlSelf);
#endif
            VL_FATAL_MT("conv1d_accel_tb.v", 21, "", "Settle region did not converge.");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        __VstlContinue = 0U;
        if (Vconv1d_accel_tb___024root___eval_phase__stl(vlSelf)) {
            __VstlContinue = 1U;
        }
        vlSelf->__VstlFirstIteration = 0U;
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vconv1d_accel_tb___024root___dump_triggers__stl(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VstlTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VstlTriggered.word(0U))) {
        VL_DBG_MSGF("         'stl' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vconv1d_accel_tb___024root___stl_sequent__TOP__0(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___stl_sequent__TOP__0\n"); );
    // Body
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_wt_addr 
        = (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wt_addr 
           + ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos) 
              * (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_bytes)));
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
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__shifted 
        = VL_SHIFTRS_III(32,32,5, (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__acc 
                                   + vlSelf->conv1d_accel_tb__DOT__DUT__DOT__bias_val), (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__cur_shift));
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT____VdfgTmp_h2d093ac0__0 
        = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_buf
        [vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_idx];
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_in_word 
        = ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_sel)
            ? vlSelf->conv1d_accel_tb__DOT__DUT__DOT__in_buf1
           [vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_idx]
            : vlSelf->conv1d_accel_tb__DOT__DUT__DOT__in_buf0
           [vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_idx]);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__clipped 
        = (VL_LTS_III(32, 0x7fU, vlSelf->conv1d_accel_tb__DOT__DUT__DOT__shifted)
            ? 0x7fU : (VL_GTS_III(32, 0xffffff80U, vlSelf->conv1d_accel_tb__DOT__DUT__DOT__shifted)
                        ? 0x80U : (0xffU & vlSelf->conv1d_accel_tb__DOT__DUT__DOT__shifted)));
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p0 = (0xffffU 
                                                  & VL_MULS_III(16, 
                                                                (0xffffU 
                                                                 & VL_EXTENDS_II(16,8, 
                                                                                (0xffU 
                                                                                & vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_in_word))), 
                                                                (0xffffU 
                                                                 & VL_EXTENDS_II(16,8, 
                                                                                (0xffU 
                                                                                & vlSelf->conv1d_accel_tb__DOT__DUT__DOT____VdfgTmp_h2d093ac0__0)))));
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p1 = (0xffffU 
                                                  & VL_MULS_III(16, 
                                                                (0xffffU 
                                                                 & VL_EXTENDS_II(16,8, 
                                                                                (0xffU 
                                                                                & (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_in_word 
                                                                                >> 8U)))), 
                                                                (0xffffU 
                                                                 & VL_EXTENDS_II(16,8, 
                                                                                (0xffU 
                                                                                & (vlSelf->conv1d_accel_tb__DOT__DUT__DOT____VdfgTmp_h2d093ac0__0 
                                                                                >> 8U))))));
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p2 = (0xffffU 
                                                  & VL_MULS_III(16, 
                                                                (0xffffU 
                                                                 & VL_EXTENDS_II(16,8, 
                                                                                (0xffU 
                                                                                & (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_in_word 
                                                                                >> 0x10U)))), 
                                                                (0xffffU 
                                                                 & VL_EXTENDS_II(16,8, 
                                                                                (0xffU 
                                                                                & (vlSelf->conv1d_accel_tb__DOT__DUT__DOT____VdfgTmp_h2d093ac0__0 
                                                                                >> 0x10U))))));
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p3 = (0xffffU 
                                                  & VL_MULS_III(16, 
                                                                (0xffffU 
                                                                 & VL_EXTENDS_II(16,8, 
                                                                                (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_in_word 
                                                                                >> 0x18U))), 
                                                                (0xffffU 
                                                                 & VL_EXTENDS_II(16,8, 
                                                                                (vlSelf->conv1d_accel_tb__DOT__DUT__DOT____VdfgTmp_h2d093ac0__0 
                                                                                >> 0x18U)))));
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac4 = 
        ((((- (IData)((1U & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p0) 
                             >> 0xfU)))) << 0x10U) 
          | (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p0)) 
         + ((((- (IData)((1U & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p1) 
                                >> 0xfU)))) << 0x10U) 
             | (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p1)) 
            + ((((- (IData)((1U & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p2) 
                                   >> 0xfU)))) << 0x10U) 
                | (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p2)) 
               + (((- (IData)((1U & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p3) 
                                     >> 0xfU)))) << 0x10U) 
                  | (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p3)))));
}

VL_ATTR_COLD void Vconv1d_accel_tb___024root___eval_stl(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___eval_stl\n"); );
    // Body
    if ((1ULL & vlSelf->__VstlTriggered.word(0U))) {
        Vconv1d_accel_tb___024root___stl_sequent__TOP__0(vlSelf);
        vlSelf->__Vm_traceActivity[3U] = 1U;
        vlSelf->__Vm_traceActivity[2U] = 1U;
        vlSelf->__Vm_traceActivity[1U] = 1U;
        vlSelf->__Vm_traceActivity[0U] = 1U;
    }
}

VL_ATTR_COLD void Vconv1d_accel_tb___024root___eval_triggers__stl(Vconv1d_accel_tb___024root* vlSelf);

VL_ATTR_COLD bool Vconv1d_accel_tb___024root___eval_phase__stl(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___eval_phase__stl\n"); );
    // Init
    CData/*0:0*/ __VstlExecute;
    // Body
    Vconv1d_accel_tb___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = vlSelf->__VstlTriggered.any();
    if (__VstlExecute) {
        Vconv1d_accel_tb___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vconv1d_accel_tb___024root___dump_triggers__act(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VactTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 0 is active: @(posedge conv1d_accel_tb.clk or negedge conv1d_accel_tb.rst_n)\n");
    }
    if ((2ULL & vlSelf->__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 1 is active: @(posedge conv1d_accel_tb.clk)\n");
    }
    if ((4ULL & vlSelf->__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
    if ((8ULL & vlSelf->__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 3 is active: @(posedge conv1d_accel_tb.rst_n)\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vconv1d_accel_tb___024root___dump_triggers__nba(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___dump_triggers__nba\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VnbaTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 0 is active: @(posedge conv1d_accel_tb.clk or negedge conv1d_accel_tb.rst_n)\n");
    }
    if ((2ULL & vlSelf->__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 1 is active: @(posedge conv1d_accel_tb.clk)\n");
    }
    if ((4ULL & vlSelf->__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
    if ((8ULL & vlSelf->__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 3 is active: @(posedge conv1d_accel_tb.rst_n)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vconv1d_accel_tb___024root___ctor_var_reset(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___ctor_var_reset\n"); );
    // Body
    vlSelf->conv1d_accel_tb__DOT__clk = VL_RAND_RESET_I(1);
    vlSelf->conv1d_accel_tb__DOT__rst_n = VL_RAND_RESET_I(1);
    vlSelf->conv1d_accel_tb__DOT__paddr = VL_RAND_RESET_I(16);
    vlSelf->conv1d_accel_tb__DOT__psel = VL_RAND_RESET_I(1);
    vlSelf->conv1d_accel_tb__DOT__penable = VL_RAND_RESET_I(1);
    vlSelf->conv1d_accel_tb__DOT__pwrite = VL_RAND_RESET_I(1);
    vlSelf->conv1d_accel_tb__DOT__pwdata = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__haddr = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__hburst = VL_RAND_RESET_I(3);
    vlSelf->conv1d_accel_tb__DOT__hsize = VL_RAND_RESET_I(3);
    vlSelf->conv1d_accel_tb__DOT__htrans = VL_RAND_RESET_I(2);
    vlSelf->conv1d_accel_tb__DOT__hwrite = VL_RAND_RESET_I(1);
    vlSelf->conv1d_accel_tb__DOT__hwdata = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__hrdata = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__hready = VL_RAND_RESET_I(1);
    for (int __Vi0 = 0; __Vi0 < 65536; ++__Vi0) {
        vlSelf->conv1d_accel_tb__DOT__mem[__Vi0] = VL_RAND_RESET_I(8);
    }
    vlSelf->conv1d_accel_tb__DOT__ap_addr = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__ap_wr = VL_RAND_RESET_I(1);
    vlSelf->conv1d_accel_tb__DOT__ap_valid = VL_RAND_RESET_I(1);
    vlSelf->conv1d_accel_tb__DOT__ap_sz = VL_RAND_RESET_I(3);
    for (int __Vi0 = 0; __Vi0 < 512; ++__Vi0) {
        vlSelf->conv1d_accel_tb__DOT__ref_in[__Vi0] = VL_RAND_RESET_I(8);
    }
    for (int __Vi0 = 0; __Vi0 < 512; ++__Vi0) {
        vlSelf->conv1d_accel_tb__DOT__ref_wt[__Vi0] = VL_RAND_RESET_I(8);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->conv1d_accel_tb__DOT__ref_bs[__Vi0] = VL_RAND_RESET_I(32);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->conv1d_accel_tb__DOT__ref_sh[__Vi0] = VL_RAND_RESET_I(5);
    }
    for (int __Vi0 = 0; __Vi0 < 512; ++__Vi0) {
        vlSelf->conv1d_accel_tb__DOT__ref_out[__Vi0] = VL_RAND_RESET_I(8);
    }
    vlSelf->conv1d_accel_tb__DOT__t_c_in = VL_RAND_RESET_I(8);
    vlSelf->conv1d_accel_tb__DOT__t_c_out = VL_RAND_RESET_I(8);
    vlSelf->conv1d_accel_tb__DOT__t_k_w = VL_RAND_RESET_I(8);
    vlSelf->conv1d_accel_tb__DOT__t_stride = VL_RAND_RESET_I(8);
    vlSelf->conv1d_accel_tb__DOT__t_w_in = VL_RAND_RESET_I(16);
    vlSelf->conv1d_accel_tb__DOT__t_w_out = VL_RAND_RESET_I(16);
    vlSelf->conv1d_accel_tb__DOT__t_patch_bytes = VL_RAND_RESET_I(16);
    vlSelf->conv1d_accel_tb__DOT__tc = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__errors = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got = VL_RAND_RESET_I(8);
    vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__i = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__rd = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_src_addr = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wt_addr = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_dst_addr = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_bs_addr = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift_addr = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_in = VL_RAND_RESET_I(8);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_out = VL_RAND_RESET_I(8);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_k_w = VL_RAND_RESET_I(8);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_stride = VL_RAND_RESET_I(8);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_w_in = VL_RAND_RESET_I(16);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift = VL_RAND_RESET_I(5);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_busy = VL_RAND_RESET_I(1);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done = VL_RAND_RESET_I(1);
    for (int __Vi0 = 0; __Vi0 < 64; ++__Vi0) {
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__in_buf0[__Vi0] = VL_RAND_RESET_I(32);
    }
    for (int __Vi0 = 0; __Vi0 < 64; ++__Vi0) {
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__in_buf1[__Vi0] = VL_RAND_RESET_I(32);
    }
    for (int __Vi0 = 0; __Vi0 < 64; ++__Vi0) {
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_buf[__Vi0] = VL_RAND_RESET_I(32);
    }
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_bytes = VL_RAND_RESET_I(16);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_words = VL_RAND_RESET_I(6);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_out_cnt = VL_RAND_RESET_I(16);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__tail_bytes = VL_RAND_RESET_I(2);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wt_lane0 = VL_RAND_RESET_I(2);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_wt_addr = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state = VL_RAND_RESET_I(4);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos = VL_RAND_RESET_I(16);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos = VL_RAND_RESET_I(8);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx = VL_RAND_RESET_I(7);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_idx = VL_RAND_RESET_I(6);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_word_idx = VL_RAND_RESET_I(6);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__acc = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__bias_val = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__result_byte = VL_RAND_RESET_I(8);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__cur_shift = VL_RAND_RESET_I(5);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift_lane = VL_RAND_RESET_I(2);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wait = VL_RAND_RESET_I(1);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_sel = VL_RAND_RESET_I(1);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_done = VL_RAND_RESET_I(1);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__load_done = VL_RAND_RESET_I(1);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_in_word = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p0 = VL_RAND_RESET_I(16);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p1 = VL_RAND_RESET_I(16);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p2 = VL_RAND_RESET_I(16);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p3 = VL_RAND_RESET_I(16);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac4 = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__shifted = VL_RAND_RESET_I(32);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__clipped = VL_RAND_RESET_I(8);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_pack__DOT__cur_lane = VL_RAND_RESET_I(2);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_pack__DOT__rdbyte = VL_RAND_RESET_I(8);
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT____VdfgTmp_h2d093ac0__0 = 0;
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__acc = VL_RAND_RESET_I(32);
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_done = VL_RAND_RESET_I(1);
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_idx = VL_RAND_RESET_I(6);
    vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr = VL_RAND_RESET_I(32);
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__r_wait = VL_RAND_RESET_I(1);
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v0 = 0;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v0 = 0;
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__load_done = VL_RAND_RESET_I(1);
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_idx = VL_RAND_RESET_I(7);
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = VL_RAND_RESET_I(4);
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__w_pos = VL_RAND_RESET_I(16);
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_sel = VL_RAND_RESET_I(1);
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__c_pos = VL_RAND_RESET_I(8);
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v0 = 0;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1 = 0;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2 = 0;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3 = 0;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4 = 0;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v5 = 0;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v1 = 0;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v1 = 0;
    vlSelf->__Vtrigprevexpr___TOP__conv1d_accel_tb__DOT__clk__0 = VL_RAND_RESET_I(1);
    vlSelf->__Vtrigprevexpr___TOP__conv1d_accel_tb__DOT__rst_n__0 = VL_RAND_RESET_I(1);
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->__Vm_traceActivity[__Vi0] = 0;
    }
}
