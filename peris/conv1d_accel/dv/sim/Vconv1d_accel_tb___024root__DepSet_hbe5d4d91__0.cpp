// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vconv1d_accel_tb.h for the primary calling header

#include "Vconv1d_accel_tb__pch.h"
#include "Vconv1d_accel_tb___024root.h"

VL_ATTR_COLD void Vconv1d_accel_tb___024root___eval_initial__TOP(Vconv1d_accel_tb___024root* vlSelf);
VlCoroutine Vconv1d_accel_tb___024root___eval_initial__TOP__Vtiming__0(Vconv1d_accel_tb___024root* vlSelf);
VlCoroutine Vconv1d_accel_tb___024root___eval_initial__TOP__Vtiming__1(Vconv1d_accel_tb___024root* vlSelf);
VlCoroutine Vconv1d_accel_tb___024root___eval_initial__TOP__Vtiming__2(Vconv1d_accel_tb___024root* vlSelf);
VlCoroutine Vconv1d_accel_tb___024root___eval_initial__TOP__Vtiming__3(Vconv1d_accel_tb___024root* vlSelf);

void Vconv1d_accel_tb___024root___eval_initial(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___eval_initial\n"); );
    // Body
    Vconv1d_accel_tb___024root___eval_initial__TOP(vlSelf);
    vlSelf->__Vm_traceActivity[1U] = 1U;
    Vconv1d_accel_tb___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vconv1d_accel_tb___024root___eval_initial__TOP__Vtiming__1(vlSelf);
    Vconv1d_accel_tb___024root___eval_initial__TOP__Vtiming__2(vlSelf);
    Vconv1d_accel_tb___024root___eval_initial__TOP__Vtiming__3(vlSelf);
    vlSelf->__Vtrigprevexpr___TOP__conv1d_accel_tb__DOT__clk__0 
        = vlSelf->conv1d_accel_tb__DOT__clk;
    vlSelf->__Vtrigprevexpr___TOP__conv1d_accel_tb__DOT__rst_n__0 
        = vlSelf->conv1d_accel_tb__DOT__rst_n;
}

VL_INLINE_OPT VlCoroutine Vconv1d_accel_tb___024root___eval_initial__TOP__Vtiming__0(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___eval_initial__TOP__Vtiming__0\n"); );
    // Body
    co_await vlSelf->__VdlySched.delay(0x186a0ULL, 
                                       nullptr, "conv1d_accel_tb.v", 
                                       26);
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       26);
    vlSelf->conv1d_accel_tb__DOT__rst_n = 1U;
}

VL_INLINE_OPT VlCoroutine Vconv1d_accel_tb___024root___eval_initial__TOP__Vtiming__1(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___eval_initial__TOP__Vtiming__1\n"); );
    // Body
    co_await vlSelf->__VdlySched.delay(0x2540be400ULL, 
                                       nullptr, "conv1d_accel_tb.v", 
                                       28);
    VL_WRITEF("Verification Failed: Timeout\n");
    VL_FINISH_MT("conv1d_accel_tb.v", 28, "");
}

VL_INLINE_OPT VlCoroutine Vconv1d_accel_tb___024root___eval_initial__TOP__Vtiming__2(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___eval_initial__TOP__Vtiming__2\n"); );
    // Init
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__3__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__3__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__3__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__3__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__4__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__4__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__4__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__4__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__5__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__5__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__5__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__5__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__6__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__6__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__6__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__6__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__7__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__7__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__7__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__7__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__8__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__8__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__8__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__8__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__9__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__9__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__9__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__9__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__10__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__10__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__10__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__10__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__16__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__16__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__16__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__16__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__17__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__17__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__17__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__17__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__18__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__18__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__18__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__18__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__19__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__19__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__19__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__19__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__20__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__20__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__20__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__20__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__21__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__21__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__21__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__21__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__22__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__22__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__22__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__22__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__23__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__23__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__23__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__23__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__29__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__29__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__29__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__29__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__30__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__30__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__30__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__30__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__31__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__31__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__31__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__31__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__32__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__32__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__32__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__32__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__33__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__33__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__33__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__33__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__34__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__34__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__34__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__34__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__35__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__35__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__35__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__35__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__36__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__36__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__36__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__36__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__42__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__42__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__42__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__42__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__43__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__43__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__43__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__43__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__44__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__44__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__44__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__44__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__45__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__45__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__45__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__45__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__46__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__46__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__46__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__46__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__47__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__47__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__47__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__47__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__48__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__48__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__48__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__48__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__49__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__49__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__49__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__49__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__55__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__55__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__55__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__55__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__56__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__56__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__56__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__56__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__57__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__57__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__57__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__57__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__58__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__58__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__58__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__58__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__59__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__59__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__59__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__59__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__60__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__60__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__60__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__60__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__61__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__61__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__61__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__61__data = 0;
    SData/*15:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__62__addr;
    __Vtask_conv1d_accel_tb__DOT__apb_write__62__addr = 0;
    IData/*31:0*/ __Vtask_conv1d_accel_tb__DOT__apb_write__62__data;
    __Vtask_conv1d_accel_tb__DOT__apb_write__62__data = 0;
    // Body
    vlSelf->conv1d_accel_tb__DOT__errors = 0U;
    vlSelf->conv1d_accel_tb__DOT__hready = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    vlSelf->conv1d_accel_tb__DOT__paddr = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwdata = 0U;
    co_await vlSelf->__VtrigSched_h635928e4__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.rst_n)", 
                                                       "conv1d_accel_tb.v", 
                                                       301);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       302);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       302);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       302);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       302);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       302);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__tc = 0U;
    vlSelf->conv1d_accel_tb__DOT__t_c_in = 2U;
    vlSelf->conv1d_accel_tb__DOT__t_c_out = 2U;
    vlSelf->conv1d_accel_tb__DOT__t_k_w = 2U;
    vlSelf->conv1d_accel_tb__DOT__t_stride = 1U;
    vlSelf->conv1d_accel_tb__DOT__t_w_in = 4U;
    vlSelf->conv1d_accel_tb__DOT__t_patch_bytes = (0xffffU 
                                                   & ((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                                                      * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)));
    vlSelf->conv1d_accel_tb__DOT__t_w_out = (0xffffU 
                                             & ((IData)(1U) 
                                                + VL_DIV_III(32, 
                                                             ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                                              - (IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w)), (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride))));
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_in[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (0xffU & vlSelf->conv1d_accel_tb__DOT__i);
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_wt[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] = 1U;
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__ref_bs[(0xfU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] = 0U;
        vlSelf->conv1d_accel_tb__DOT__ref_sh[(0xfU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] = 0U;
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    VL_WRITEF("[TB] TC%0d (fixed) c_in=%0# c_out=%0# k_w=%0# stride=%0# w_in=%0# w_out=%0#\n",
              32,vlSelf->conv1d_accel_tb__DOT__tc,8,
              (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in),
              8,vlSelf->conv1d_accel_tb__DOT__t_c_out,
              8,(IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w),
              8,vlSelf->conv1d_accel_tb__DOT__t_stride,
              16,(IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in),
              16,vlSelf->conv1d_accel_tb__DOT__t_w_out);
    vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo = 0U;
        while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                < (IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out))) {
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc = 0U;
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n = 0U;
            while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    < (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes))) {
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                    = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                       + VL_MULS_III(32, VL_EXTENDS_II(32,8, 
                                                       vlSelf->conv1d_accel_tb__DOT__ref_in
                                                       [
                                                       (0x1ffU 
                                                        & (((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                             * (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride)) 
                                                            * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)) 
                                                           + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))]), 
                                     VL_EXTENDS_II(32,8, 
                                                   vlSelf->conv1d_accel_tb__DOT__ref_wt
                                                   [
                                                   (0x1ffU 
                                                    & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
                                                        * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)) 
                                                       + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))])));
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n);
            }
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                   + vlSelf->conv1d_accel_tb__DOT__ref_bs
                   [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s 
                = VL_SHIFTRS_III(32,32,5, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc, 
                                 vlSelf->conv1d_accel_tb__DOT__ref_sh
                                 [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__ref_out[(0x1ffU 
                                                   & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                       * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)) 
                                                      + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co))] 
                = (VL_LTS_III(32, 0x7fU, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                    ? 0x7fU : (VL_GTS_III(32, 0xffffff80U, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                                ? 0x80U : (0xffU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)));
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo);
        }
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x1000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_in[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x2000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_wt[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3000U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)]);
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3001U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 8U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3002U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 0x10U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3003U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
               >> 0x18U);
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x4000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_sh[
            (0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(4U) + ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
                              * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x5000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] = 0xaaU;
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    __Vtask_conv1d_accel_tb__DOT__apb_write__3__data = 0x1000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__3__addr = 4U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__3__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__3__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__4__data = 0x2000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__4__addr = 8U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__4__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__4__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__5__data = 0x5000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__5__addr = 0xcU;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__5__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__5__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__6__data = 0x3000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__6__addr = 0x10U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__6__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__6__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__7__data = 0x4000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__7__addr = 0x20U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__7__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__7__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__8__data 
        = (((IData)(vlSelf->conv1d_accel_tb__DOT__t_stride) 
            << 0x18U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                          << 0x10U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                        << 8U) | (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in))));
    __Vtask_conv1d_accel_tb__DOT__apb_write__8__addr = 0x14U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__8__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__8__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__9__data 
        = vlSelf->conv1d_accel_tb__DOT__t_w_in;
    __Vtask_conv1d_accel_tb__DOT__apb_write__9__addr = 0x18U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__9__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__9__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__10__data = 1U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__10__addr = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__10__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__10__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt = 0U;
    while (((~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done)) 
            & VL_GTS_III(32, 0x186a0U, vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt))) {
        co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                           nullptr, 
                                                           "@(posedge conv1d_accel_tb.clk)", 
                                                           "conv1d_accel_tb.v", 
                                                           261);
        vlSelf->__Vm_traceActivity[2U] = 1U;
        vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt);
    }
    if (VL_UNLIKELY((1U & (~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done))))) {
        VL_WRITEF("  [TIMEOUT] TC%0d: accelerator stalled (state=%0# w=%0# c=%0#)\n",
                  32,vlSelf->conv1d_accel_tb__DOT__tc,
                  4,(IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state),
                  16,vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos,
                  8,(IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos));
        vlSelf->conv1d_accel_tb__DOT__errors = ((IData)(1U) 
                                                + vlSelf->conv1d_accel_tb__DOT__errors);
    }
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)))) {
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got 
            = vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                                 & ((IData)(0x5000U) 
                                                    + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx))];
        if (VL_UNLIKELY(((IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got) 
                         != vlSelf->conv1d_accel_tb__DOT__ref_out
                         [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)]))) {
            VL_WRITEF("  [FAIL] TC%0d out[%0d]: exp=%0d (0x%02x)  got=%0d (0x%02x)\n",
                      32,vlSelf->conv1d_accel_tb__DOT__tc,
                      32,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx,
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got,
                      8,(IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got));
            vlSelf->conv1d_accel_tb__DOT__errors = 
                ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__errors);
        }
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__tc = 1U;
    vlSelf->conv1d_accel_tb__DOT__t_c_in = 1U;
    vlSelf->conv1d_accel_tb__DOT__t_c_out = 1U;
    vlSelf->conv1d_accel_tb__DOT__t_k_w = 2U;
    vlSelf->conv1d_accel_tb__DOT__t_stride = 2U;
    vlSelf->conv1d_accel_tb__DOT__t_w_in = 6U;
    vlSelf->conv1d_accel_tb__DOT__t_patch_bytes = (0xffffU 
                                                   & ((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                                                      * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)));
    vlSelf->conv1d_accel_tb__DOT__t_w_out = (0xffffU 
                                             & ((IData)(1U) 
                                                + VL_DIV_III(32, 
                                                             ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                                              - (IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w)), (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride))));
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_in[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (0xffU & vlSelf->conv1d_accel_tb__DOT__i);
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_wt[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] = 1U;
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__ref_bs[0U] = 0U;
    vlSelf->conv1d_accel_tb__DOT__ref_sh[0U] = 0U;
    VL_WRITEF("[TB] TC%0d (stride2) c_in=%0# c_out=%0# k_w=%0# stride=%0# w_in=%0# w_out=%0#\n",
              32,vlSelf->conv1d_accel_tb__DOT__tc,8,
              (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in),
              8,vlSelf->conv1d_accel_tb__DOT__t_c_out,
              8,(IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w),
              8,vlSelf->conv1d_accel_tb__DOT__t_stride,
              16,(IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in),
              16,vlSelf->conv1d_accel_tb__DOT__t_w_out);
    vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo = 0U;
        while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                < (IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out))) {
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc = 0U;
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n = 0U;
            while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    < (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes))) {
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                    = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                       + VL_MULS_III(32, VL_EXTENDS_II(32,8, 
                                                       vlSelf->conv1d_accel_tb__DOT__ref_in
                                                       [
                                                       (0x1ffU 
                                                        & (((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                             * (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride)) 
                                                            * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)) 
                                                           + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))]), 
                                     VL_EXTENDS_II(32,8, 
                                                   vlSelf->conv1d_accel_tb__DOT__ref_wt
                                                   [
                                                   (0x1ffU 
                                                    & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
                                                        * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)) 
                                                       + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))])));
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n);
            }
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                   + vlSelf->conv1d_accel_tb__DOT__ref_bs
                   [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s 
                = VL_SHIFTRS_III(32,32,5, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc, 
                                 vlSelf->conv1d_accel_tb__DOT__ref_sh
                                 [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__ref_out[(0x1ffU 
                                                   & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                       * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)) 
                                                      + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co))] 
                = (VL_LTS_III(32, 0x7fU, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                    ? 0x7fU : (VL_GTS_III(32, 0xffffff80U, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                                ? 0x80U : (0xffU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)));
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo);
        }
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x1000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_in[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x2000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_wt[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3000U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)]);
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3001U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 8U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3002U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 0x10U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3003U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
               >> 0x18U);
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x4000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_sh[
            (0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(4U) + ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
                              * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x5000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] = 0xaaU;
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    __Vtask_conv1d_accel_tb__DOT__apb_write__16__data = 0x1000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__16__addr = 4U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__16__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__16__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__17__data = 0x2000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__17__addr = 8U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__17__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__17__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__18__data = 0x5000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__18__addr = 0xcU;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__18__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__18__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__19__data = 0x3000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__19__addr = 0x10U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__19__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__19__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__20__data = 0x4000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__20__addr = 0x20U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__20__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__20__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__21__data 
        = (((IData)(vlSelf->conv1d_accel_tb__DOT__t_stride) 
            << 0x18U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                          << 0x10U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                        << 8U) | (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in))));
    __Vtask_conv1d_accel_tb__DOT__apb_write__21__addr = 0x14U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__21__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__21__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__22__data 
        = vlSelf->conv1d_accel_tb__DOT__t_w_in;
    __Vtask_conv1d_accel_tb__DOT__apb_write__22__addr = 0x18U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__22__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__22__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__23__data = 1U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__23__addr = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__23__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__23__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt = 0U;
    while (((~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done)) 
            & VL_GTS_III(32, 0x186a0U, vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt))) {
        co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                           nullptr, 
                                                           "@(posedge conv1d_accel_tb.clk)", 
                                                           "conv1d_accel_tb.v", 
                                                           261);
        vlSelf->__Vm_traceActivity[2U] = 1U;
        vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt);
    }
    if (VL_UNLIKELY((1U & (~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done))))) {
        VL_WRITEF("  [TIMEOUT] TC%0d: accelerator stalled (state=%0# w=%0# c=%0#)\n",
                  32,vlSelf->conv1d_accel_tb__DOT__tc,
                  4,(IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state),
                  16,vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos,
                  8,(IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos));
        vlSelf->conv1d_accel_tb__DOT__errors = ((IData)(1U) 
                                                + vlSelf->conv1d_accel_tb__DOT__errors);
    }
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)))) {
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got 
            = vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                                 & ((IData)(0x5000U) 
                                                    + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx))];
        if (VL_UNLIKELY(((IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got) 
                         != vlSelf->conv1d_accel_tb__DOT__ref_out
                         [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)]))) {
            VL_WRITEF("  [FAIL] TC%0d out[%0d]: exp=%0d (0x%02x)  got=%0d (0x%02x)\n",
                      32,vlSelf->conv1d_accel_tb__DOT__tc,
                      32,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx,
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got,
                      8,(IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got));
            vlSelf->conv1d_accel_tb__DOT__errors = 
                ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__errors);
        }
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__tc = 2U;
    vlSelf->conv1d_accel_tb__DOT__t_c_in = 2U;
    vlSelf->conv1d_accel_tb__DOT__t_c_out = 1U;
    vlSelf->conv1d_accel_tb__DOT__t_k_w = 3U;
    vlSelf->conv1d_accel_tb__DOT__t_stride = 1U;
    vlSelf->conv1d_accel_tb__DOT__t_w_in = 5U;
    vlSelf->conv1d_accel_tb__DOT__t_patch_bytes = (0xffffU 
                                                   & ((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                                                      * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)));
    vlSelf->conv1d_accel_tb__DOT__t_w_out = (0xffffU 
                                             & ((IData)(1U) 
                                                + VL_DIV_III(32, 
                                                             ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                                              - (IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w)), (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride))));
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_in[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] = 0x7fU;
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_wt[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] = 0x7fU;
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__ref_bs[0U] = 0x3e8U;
    vlSelf->conv1d_accel_tb__DOT__ref_sh[0U] = 4U;
    VL_WRITEF("[TB] TC%0d (clip+)  c_in=%0# c_out=%0# k_w=%0# stride=%0# w_in=%0# w_out=%0#\n",
              32,vlSelf->conv1d_accel_tb__DOT__tc,8,
              (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in),
              8,vlSelf->conv1d_accel_tb__DOT__t_c_out,
              8,(IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w),
              8,vlSelf->conv1d_accel_tb__DOT__t_stride,
              16,(IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in),
              16,vlSelf->conv1d_accel_tb__DOT__t_w_out);
    vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo = 0U;
        while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                < (IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out))) {
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc = 0U;
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n = 0U;
            while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    < (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes))) {
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                    = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                       + VL_MULS_III(32, VL_EXTENDS_II(32,8, 
                                                       vlSelf->conv1d_accel_tb__DOT__ref_in
                                                       [
                                                       (0x1ffU 
                                                        & (((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                             * (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride)) 
                                                            * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)) 
                                                           + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))]), 
                                     VL_EXTENDS_II(32,8, 
                                                   vlSelf->conv1d_accel_tb__DOT__ref_wt
                                                   [
                                                   (0x1ffU 
                                                    & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
                                                        * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)) 
                                                       + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))])));
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n);
            }
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                   + vlSelf->conv1d_accel_tb__DOT__ref_bs
                   [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s 
                = VL_SHIFTRS_III(32,32,5, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc, 
                                 vlSelf->conv1d_accel_tb__DOT__ref_sh
                                 [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__ref_out[(0x1ffU 
                                                   & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                       * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)) 
                                                      + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co))] 
                = (VL_LTS_III(32, 0x7fU, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                    ? 0x7fU : (VL_GTS_III(32, 0xffffff80U, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                                ? 0x80U : (0xffU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)));
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo);
        }
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x1000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_in[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x2000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_wt[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3000U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)]);
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3001U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 8U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3002U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 0x10U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3003U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
               >> 0x18U);
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x4000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_sh[
            (0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(4U) + ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
                              * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x5000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] = 0xaaU;
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    __Vtask_conv1d_accel_tb__DOT__apb_write__29__data = 0x1000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__29__addr = 4U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__29__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__29__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__30__data = 0x2000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__30__addr = 8U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__30__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__30__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__31__data = 0x5000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__31__addr = 0xcU;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__31__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__31__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__32__data = 0x3000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__32__addr = 0x10U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__32__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__32__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__33__data = 0x4000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__33__addr = 0x20U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__33__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__33__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__34__data 
        = (((IData)(vlSelf->conv1d_accel_tb__DOT__t_stride) 
            << 0x18U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                          << 0x10U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                        << 8U) | (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in))));
    __Vtask_conv1d_accel_tb__DOT__apb_write__34__addr = 0x14U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__34__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__34__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__35__data 
        = vlSelf->conv1d_accel_tb__DOT__t_w_in;
    __Vtask_conv1d_accel_tb__DOT__apb_write__35__addr = 0x18U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__35__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__35__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__36__data = 1U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__36__addr = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__36__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__36__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt = 0U;
    while (((~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done)) 
            & VL_GTS_III(32, 0x186a0U, vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt))) {
        co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                           nullptr, 
                                                           "@(posedge conv1d_accel_tb.clk)", 
                                                           "conv1d_accel_tb.v", 
                                                           261);
        vlSelf->__Vm_traceActivity[2U] = 1U;
        vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt);
    }
    if (VL_UNLIKELY((1U & (~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done))))) {
        VL_WRITEF("  [TIMEOUT] TC%0d: accelerator stalled (state=%0# w=%0# c=%0#)\n",
                  32,vlSelf->conv1d_accel_tb__DOT__tc,
                  4,(IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state),
                  16,vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos,
                  8,(IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos));
        vlSelf->conv1d_accel_tb__DOT__errors = ((IData)(1U) 
                                                + vlSelf->conv1d_accel_tb__DOT__errors);
    }
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)))) {
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got 
            = vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                                 & ((IData)(0x5000U) 
                                                    + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx))];
        if (VL_UNLIKELY(((IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got) 
                         != vlSelf->conv1d_accel_tb__DOT__ref_out
                         [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)]))) {
            VL_WRITEF("  [FAIL] TC%0d out[%0d]: exp=%0d (0x%02x)  got=%0d (0x%02x)\n",
                      32,vlSelf->conv1d_accel_tb__DOT__tc,
                      32,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx,
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got,
                      8,(IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got));
            vlSelf->conv1d_accel_tb__DOT__errors = 
                ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__errors);
        }
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__tc = 3U;
    vlSelf->conv1d_accel_tb__DOT__t_c_in = 2U;
    vlSelf->conv1d_accel_tb__DOT__t_c_out = 1U;
    vlSelf->conv1d_accel_tb__DOT__t_k_w = 2U;
    vlSelf->conv1d_accel_tb__DOT__t_stride = 1U;
    vlSelf->conv1d_accel_tb__DOT__t_w_in = 4U;
    vlSelf->conv1d_accel_tb__DOT__t_patch_bytes = (0xffffU 
                                                   & ((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                                                      * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)));
    vlSelf->conv1d_accel_tb__DOT__t_w_out = (0xffffU 
                                             & ((IData)(1U) 
                                                + VL_DIV_III(32, 
                                                             ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                                              - (IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w)), (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride))));
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_in[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] = 0x7fU;
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_wt[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] = 0x80U;
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__ref_bs[0U] = 0xffffec78U;
    vlSelf->conv1d_accel_tb__DOT__ref_sh[0U] = 2U;
    VL_WRITEF("[TB] TC%0d (clip-)  c_in=%0# c_out=%0# k_w=%0# stride=%0# w_in=%0# w_out=%0#\n",
              32,vlSelf->conv1d_accel_tb__DOT__tc,8,
              (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in),
              8,vlSelf->conv1d_accel_tb__DOT__t_c_out,
              8,(IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w),
              8,vlSelf->conv1d_accel_tb__DOT__t_stride,
              16,(IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in),
              16,vlSelf->conv1d_accel_tb__DOT__t_w_out);
    vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo = 0U;
        while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                < (IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out))) {
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc = 0U;
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n = 0U;
            while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    < (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes))) {
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                    = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                       + VL_MULS_III(32, VL_EXTENDS_II(32,8, 
                                                       vlSelf->conv1d_accel_tb__DOT__ref_in
                                                       [
                                                       (0x1ffU 
                                                        & (((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                             * (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride)) 
                                                            * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)) 
                                                           + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))]), 
                                     VL_EXTENDS_II(32,8, 
                                                   vlSelf->conv1d_accel_tb__DOT__ref_wt
                                                   [
                                                   (0x1ffU 
                                                    & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
                                                        * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)) 
                                                       + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))])));
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n);
            }
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                   + vlSelf->conv1d_accel_tb__DOT__ref_bs
                   [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s 
                = VL_SHIFTRS_III(32,32,5, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc, 
                                 vlSelf->conv1d_accel_tb__DOT__ref_sh
                                 [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__ref_out[(0x1ffU 
                                                   & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                       * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)) 
                                                      + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co))] 
                = (VL_LTS_III(32, 0x7fU, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                    ? 0x7fU : (VL_GTS_III(32, 0xffffff80U, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                                ? 0x80U : (0xffU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)));
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo);
        }
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x1000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_in[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x2000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_wt[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3000U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)]);
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3001U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 8U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3002U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 0x10U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3003U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
               >> 0x18U);
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x4000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_sh[
            (0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(4U) + ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
                              * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x5000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] = 0xaaU;
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    __Vtask_conv1d_accel_tb__DOT__apb_write__42__data = 0x1000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__42__addr = 4U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__42__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__42__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__43__data = 0x2000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__43__addr = 8U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__43__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__43__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__44__data = 0x5000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__44__addr = 0xcU;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__44__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__44__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__45__data = 0x3000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__45__addr = 0x10U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__45__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__45__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__46__data = 0x4000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__46__addr = 0x20U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__46__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__46__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__47__data 
        = (((IData)(vlSelf->conv1d_accel_tb__DOT__t_stride) 
            << 0x18U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                          << 0x10U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                        << 8U) | (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in))));
    __Vtask_conv1d_accel_tb__DOT__apb_write__47__addr = 0x14U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__47__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__47__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__48__data 
        = vlSelf->conv1d_accel_tb__DOT__t_w_in;
    __Vtask_conv1d_accel_tb__DOT__apb_write__48__addr = 0x18U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__48__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__48__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__49__data = 1U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__49__addr = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__49__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__49__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt = 0U;
    while (((~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done)) 
            & VL_GTS_III(32, 0x186a0U, vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt))) {
        co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                           nullptr, 
                                                           "@(posedge conv1d_accel_tb.clk)", 
                                                           "conv1d_accel_tb.v", 
                                                           261);
        vlSelf->__Vm_traceActivity[2U] = 1U;
        vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt);
    }
    if (VL_UNLIKELY((1U & (~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done))))) {
        VL_WRITEF("  [TIMEOUT] TC%0d: accelerator stalled (state=%0# w=%0# c=%0#)\n",
                  32,vlSelf->conv1d_accel_tb__DOT__tc,
                  4,(IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state),
                  16,vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos,
                  8,(IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos));
        vlSelf->conv1d_accel_tb__DOT__errors = ((IData)(1U) 
                                                + vlSelf->conv1d_accel_tb__DOT__errors);
    }
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)))) {
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got 
            = vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                                 & ((IData)(0x5000U) 
                                                    + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx))];
        if (VL_UNLIKELY(((IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got) 
                         != vlSelf->conv1d_accel_tb__DOT__ref_out
                         [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)]))) {
            VL_WRITEF("  [FAIL] TC%0d out[%0d]: exp=%0d (0x%02x)  got=%0d (0x%02x)\n",
                      32,vlSelf->conv1d_accel_tb__DOT__tc,
                      32,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx,
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got,
                      8,(IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got));
            vlSelf->conv1d_accel_tb__DOT__errors = 
                ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__errors);
        }
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__t_c_in = (0xffU & 
                                            ((IData)(1U) 
                                             + (IData)(
                                                       (3U 
                                                        & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_c_out = (0xffU 
                                             & ((IData)(1U) 
                                                + (IData)(
                                                          (3U 
                                                           & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_k_w = (0xffU & 
                                           ((IData)(1U) 
                                            + (IData)(
                                                      (3U 
                                                       & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_stride = (0xffU 
                                              & ((IData)(1U) 
                                                 + (IData)(
                                                           (1U 
                                                            & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_w_in = (0xffffU 
                                            & ((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                                               + ((IData)(vlSelf->conv1d_accel_tb__DOT__t_stride) 
                                                  * (IData)(
                                                            ((IData)(1U) 
                                                             + (IData)(
                                                                       (3U 
                                                                        & VL_RANDOM_I())))))));
    vlSelf->conv1d_accel_tb__DOT__t_patch_bytes = (0xffffU 
                                                   & ((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                                                      * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)));
    vlSelf->conv1d_accel_tb__DOT__t_w_out = (0xffffU 
                                             & ((IData)(1U) 
                                                + VL_DIV_III(32, 
                                                             ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                                              - (IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w)), (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride))));
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_in[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (0xffU & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_wt[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (0xffU & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__ref_bs[(0xfU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = VL_MODDIVS_III(32, (IData)(VL_RANDOM_I()), (IData)(0x100U));
        vlSelf->conv1d_accel_tb__DOT__ref_sh[(0xfU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (3U & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    VL_WRITEF("[TB] TC4 (rand)   c_in=%0# c_out=%0# k_w=%0# stride=%0# w_in=%0# w_out=%0#\n",
              8,vlSelf->conv1d_accel_tb__DOT__t_c_in,
              8,(IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out),
              8,vlSelf->conv1d_accel_tb__DOT__t_k_w,
              8,(IData)(vlSelf->conv1d_accel_tb__DOT__t_stride),
              16,vlSelf->conv1d_accel_tb__DOT__t_w_in,
              16,(IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out));
    vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo = 0U;
        while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                < (IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out))) {
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc = 0U;
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n = 0U;
            while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    < (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes))) {
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                    = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                       + VL_MULS_III(32, VL_EXTENDS_II(32,8, 
                                                       vlSelf->conv1d_accel_tb__DOT__ref_in
                                                       [
                                                       (0x1ffU 
                                                        & (((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                             * (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride)) 
                                                            * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)) 
                                                           + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))]), 
                                     VL_EXTENDS_II(32,8, 
                                                   vlSelf->conv1d_accel_tb__DOT__ref_wt
                                                   [
                                                   (0x1ffU 
                                                    & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
                                                        * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)) 
                                                       + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))])));
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n);
            }
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                   + vlSelf->conv1d_accel_tb__DOT__ref_bs
                   [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s 
                = VL_SHIFTRS_III(32,32,5, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc, 
                                 vlSelf->conv1d_accel_tb__DOT__ref_sh
                                 [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__ref_out[(0x1ffU 
                                                   & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                       * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)) 
                                                      + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co))] 
                = (VL_LTS_III(32, 0x7fU, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                    ? 0x7fU : (VL_GTS_III(32, 0xffffff80U, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                                ? 0x80U : (0xffU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)));
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo);
        }
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x1000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_in[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x2000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_wt[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3000U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)]);
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3001U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 8U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3002U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 0x10U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3003U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
               >> 0x18U);
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x4000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_sh[
            (0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(4U) + ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
                              * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x5000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] = 0xaaU;
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    __Vtask_conv1d_accel_tb__DOT__apb_write__55__data = 0x1000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__55__addr = 4U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__55__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__55__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__56__data = 0x2000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__56__addr = 8U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__56__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__56__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__57__data = 0x5000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__57__addr = 0xcU;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__57__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__57__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__58__data = 0x3000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__58__addr = 0x10U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__58__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__58__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__59__data = 0x4000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__59__addr = 0x20U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__59__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__59__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__60__data 
        = (((IData)(vlSelf->conv1d_accel_tb__DOT__t_stride) 
            << 0x18U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                          << 0x10U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                        << 8U) | (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in))));
    __Vtask_conv1d_accel_tb__DOT__apb_write__60__addr = 0x14U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__60__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__60__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__61__data 
        = vlSelf->conv1d_accel_tb__DOT__t_w_in;
    __Vtask_conv1d_accel_tb__DOT__apb_write__61__addr = 0x18U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__61__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__61__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__62__data = 1U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__62__addr = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__62__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__62__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt = 0U;
    while (((~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done)) 
            & VL_GTS_III(32, 0x186a0U, vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt))) {
        co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                           nullptr, 
                                                           "@(posedge conv1d_accel_tb.clk)", 
                                                           "conv1d_accel_tb.v", 
                                                           261);
        vlSelf->__Vm_traceActivity[2U] = 1U;
        vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt);
    }
    if (VL_UNLIKELY((1U & (~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done))))) {
        VL_WRITEF("  [TIMEOUT] TC4: accelerator stalled (state=%0# w=%0# c=%0#)\n",
                  4,vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state,
                  16,(IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos),
                  8,vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos);
        vlSelf->conv1d_accel_tb__DOT__errors = ((IData)(1U) 
                                                + vlSelf->conv1d_accel_tb__DOT__errors);
    }
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)))) {
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got 
            = vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                                 & ((IData)(0x5000U) 
                                                    + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx))];
        if (VL_UNLIKELY(((IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got) 
                         != vlSelf->conv1d_accel_tb__DOT__ref_out
                         [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)]))) {
            VL_WRITEF("  [FAIL] TC4 out[%0d]: exp=%0d (0x%02x)  got=%0d (0x%02x)\n",
                      32,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx,
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got,
                      8,(IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got));
            vlSelf->conv1d_accel_tb__DOT__errors = 
                ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__errors);
        }
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__tc = 5U;
    vlSelf->conv1d_accel_tb__DOT__t_c_in = (0xffU & 
                                            ((IData)(1U) 
                                             + (IData)(
                                                       (3U 
                                                        & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_c_out = (0xffU 
                                             & ((IData)(1U) 
                                                + (IData)(
                                                          (3U 
                                                           & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_k_w = (0xffU & 
                                           ((IData)(1U) 
                                            + (IData)(
                                                      (3U 
                                                       & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_stride = (0xffU 
                                              & ((IData)(1U) 
                                                 + (IData)(
                                                           (1U 
                                                            & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_w_in = (0xffffU 
                                            & ((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                                               + ((IData)(vlSelf->conv1d_accel_tb__DOT__t_stride) 
                                                  * (IData)(
                                                            ((IData)(1U) 
                                                             + (IData)(
                                                                       (3U 
                                                                        & VL_RANDOM_I())))))));
    vlSelf->conv1d_accel_tb__DOT__t_patch_bytes = (0xffffU 
                                                   & ((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                                                      * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)));
    vlSelf->conv1d_accel_tb__DOT__t_w_out = (0xffffU 
                                             & ((IData)(1U) 
                                                + VL_DIV_III(32, 
                                                             ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                                              - (IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w)), (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride))));
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_in[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (0xffU & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_wt[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (0xffU & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__ref_bs[(0xfU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = VL_MODDIVS_III(32, (IData)(VL_RANDOM_I()), (IData)(0x100U));
        vlSelf->conv1d_accel_tb__DOT__ref_sh[(0xfU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (3U & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    VL_WRITEF("[TB] TC5 (rand)   c_in=%0# c_out=%0# k_w=%0# stride=%0# w_in=%0# w_out=%0#\n",
              8,vlSelf->conv1d_accel_tb__DOT__t_c_in,
              8,(IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out),
              8,vlSelf->conv1d_accel_tb__DOT__t_k_w,
              8,(IData)(vlSelf->conv1d_accel_tb__DOT__t_stride),
              16,vlSelf->conv1d_accel_tb__DOT__t_w_in,
              16,(IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out));
    vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo = 0U;
        while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                < (IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out))) {
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc = 0U;
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n = 0U;
            while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    < (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes))) {
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                    = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                       + VL_MULS_III(32, VL_EXTENDS_II(32,8, 
                                                       vlSelf->conv1d_accel_tb__DOT__ref_in
                                                       [
                                                       (0x1ffU 
                                                        & (((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                             * (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride)) 
                                                            * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)) 
                                                           + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))]), 
                                     VL_EXTENDS_II(32,8, 
                                                   vlSelf->conv1d_accel_tb__DOT__ref_wt
                                                   [
                                                   (0x1ffU 
                                                    & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
                                                        * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)) 
                                                       + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))])));
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n);
            }
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                   + vlSelf->conv1d_accel_tb__DOT__ref_bs
                   [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s 
                = VL_SHIFTRS_III(32,32,5, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc, 
                                 vlSelf->conv1d_accel_tb__DOT__ref_sh
                                 [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__ref_out[(0x1ffU 
                                                   & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                       * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)) 
                                                      + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co))] 
                = (VL_LTS_III(32, 0x7fU, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                    ? 0x7fU : (VL_GTS_III(32, 0xffffff80U, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                                ? 0x80U : (0xffU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)));
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo);
        }
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x1000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_in[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x2000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_wt[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3000U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)]);
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3001U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 8U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3002U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 0x10U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3003U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
               >> 0x18U);
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x4000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_sh[
            (0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(4U) + ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
                              * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x5000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] = 0xaaU;
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    __Vtask_conv1d_accel_tb__DOT__apb_write__55__data = 0x1000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__55__addr = 4U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__55__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__55__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__56__data = 0x2000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__56__addr = 8U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__56__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__56__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__57__data = 0x5000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__57__addr = 0xcU;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__57__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__57__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__58__data = 0x3000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__58__addr = 0x10U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__58__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__58__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__59__data = 0x4000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__59__addr = 0x20U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__59__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__59__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__60__data 
        = (((IData)(vlSelf->conv1d_accel_tb__DOT__t_stride) 
            << 0x18U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                          << 0x10U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                        << 8U) | (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in))));
    __Vtask_conv1d_accel_tb__DOT__apb_write__60__addr = 0x14U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__60__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__60__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__61__data 
        = vlSelf->conv1d_accel_tb__DOT__t_w_in;
    __Vtask_conv1d_accel_tb__DOT__apb_write__61__addr = 0x18U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__61__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__61__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__62__data = 1U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__62__addr = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__62__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__62__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt = 0U;
    while (((~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done)) 
            & VL_GTS_III(32, 0x186a0U, vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt))) {
        co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                           nullptr, 
                                                           "@(posedge conv1d_accel_tb.clk)", 
                                                           "conv1d_accel_tb.v", 
                                                           261);
        vlSelf->__Vm_traceActivity[2U] = 1U;
        vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt);
    }
    if (VL_UNLIKELY((1U & (~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done))))) {
        VL_WRITEF("  [TIMEOUT] TC5: accelerator stalled (state=%0# w=%0# c=%0#)\n",
                  4,vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state,
                  16,(IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos),
                  8,vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos);
        vlSelf->conv1d_accel_tb__DOT__errors = ((IData)(1U) 
                                                + vlSelf->conv1d_accel_tb__DOT__errors);
    }
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)))) {
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got 
            = vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                                 & ((IData)(0x5000U) 
                                                    + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx))];
        if (VL_UNLIKELY(((IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got) 
                         != vlSelf->conv1d_accel_tb__DOT__ref_out
                         [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)]))) {
            VL_WRITEF("  [FAIL] TC5 out[%0d]: exp=%0d (0x%02x)  got=%0d (0x%02x)\n",
                      32,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx,
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got,
                      8,(IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got));
            vlSelf->conv1d_accel_tb__DOT__errors = 
                ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__errors);
        }
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__tc = 6U;
    vlSelf->conv1d_accel_tb__DOT__t_c_in = (0xffU & 
                                            ((IData)(1U) 
                                             + (IData)(
                                                       (3U 
                                                        & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_c_out = (0xffU 
                                             & ((IData)(1U) 
                                                + (IData)(
                                                          (3U 
                                                           & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_k_w = (0xffU & 
                                           ((IData)(1U) 
                                            + (IData)(
                                                      (3U 
                                                       & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_stride = (0xffU 
                                              & ((IData)(1U) 
                                                 + (IData)(
                                                           (1U 
                                                            & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_w_in = (0xffffU 
                                            & ((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                                               + ((IData)(vlSelf->conv1d_accel_tb__DOT__t_stride) 
                                                  * (IData)(
                                                            ((IData)(1U) 
                                                             + (IData)(
                                                                       (3U 
                                                                        & VL_RANDOM_I())))))));
    vlSelf->conv1d_accel_tb__DOT__t_patch_bytes = (0xffffU 
                                                   & ((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                                                      * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)));
    vlSelf->conv1d_accel_tb__DOT__t_w_out = (0xffffU 
                                             & ((IData)(1U) 
                                                + VL_DIV_III(32, 
                                                             ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                                              - (IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w)), (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride))));
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_in[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (0xffU & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_wt[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (0xffU & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__ref_bs[(0xfU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = VL_MODDIVS_III(32, (IData)(VL_RANDOM_I()), (IData)(0x100U));
        vlSelf->conv1d_accel_tb__DOT__ref_sh[(0xfU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (3U & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    VL_WRITEF("[TB] TC6 (rand)   c_in=%0# c_out=%0# k_w=%0# stride=%0# w_in=%0# w_out=%0#\n",
              8,vlSelf->conv1d_accel_tb__DOT__t_c_in,
              8,(IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out),
              8,vlSelf->conv1d_accel_tb__DOT__t_k_w,
              8,(IData)(vlSelf->conv1d_accel_tb__DOT__t_stride),
              16,vlSelf->conv1d_accel_tb__DOT__t_w_in,
              16,(IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out));
    vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo = 0U;
        while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                < (IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out))) {
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc = 0U;
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n = 0U;
            while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    < (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes))) {
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                    = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                       + VL_MULS_III(32, VL_EXTENDS_II(32,8, 
                                                       vlSelf->conv1d_accel_tb__DOT__ref_in
                                                       [
                                                       (0x1ffU 
                                                        & (((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                             * (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride)) 
                                                            * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)) 
                                                           + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))]), 
                                     VL_EXTENDS_II(32,8, 
                                                   vlSelf->conv1d_accel_tb__DOT__ref_wt
                                                   [
                                                   (0x1ffU 
                                                    & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
                                                        * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)) 
                                                       + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))])));
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n);
            }
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                   + vlSelf->conv1d_accel_tb__DOT__ref_bs
                   [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s 
                = VL_SHIFTRS_III(32,32,5, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc, 
                                 vlSelf->conv1d_accel_tb__DOT__ref_sh
                                 [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__ref_out[(0x1ffU 
                                                   & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                       * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)) 
                                                      + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co))] 
                = (VL_LTS_III(32, 0x7fU, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                    ? 0x7fU : (VL_GTS_III(32, 0xffffff80U, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                                ? 0x80U : (0xffU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)));
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo);
        }
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x1000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_in[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x2000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_wt[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3000U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)]);
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3001U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 8U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3002U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 0x10U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3003U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
               >> 0x18U);
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x4000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_sh[
            (0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(4U) + ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
                              * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x5000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] = 0xaaU;
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    __Vtask_conv1d_accel_tb__DOT__apb_write__55__data = 0x1000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__55__addr = 4U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__55__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__55__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__56__data = 0x2000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__56__addr = 8U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__56__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__56__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__57__data = 0x5000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__57__addr = 0xcU;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__57__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__57__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__58__data = 0x3000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__58__addr = 0x10U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__58__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__58__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__59__data = 0x4000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__59__addr = 0x20U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__59__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__59__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__60__data 
        = (((IData)(vlSelf->conv1d_accel_tb__DOT__t_stride) 
            << 0x18U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                          << 0x10U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                        << 8U) | (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in))));
    __Vtask_conv1d_accel_tb__DOT__apb_write__60__addr = 0x14U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__60__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__60__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__61__data 
        = vlSelf->conv1d_accel_tb__DOT__t_w_in;
    __Vtask_conv1d_accel_tb__DOT__apb_write__61__addr = 0x18U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__61__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__61__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__62__data = 1U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__62__addr = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__62__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__62__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt = 0U;
    while (((~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done)) 
            & VL_GTS_III(32, 0x186a0U, vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt))) {
        co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                           nullptr, 
                                                           "@(posedge conv1d_accel_tb.clk)", 
                                                           "conv1d_accel_tb.v", 
                                                           261);
        vlSelf->__Vm_traceActivity[2U] = 1U;
        vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt);
    }
    if (VL_UNLIKELY((1U & (~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done))))) {
        VL_WRITEF("  [TIMEOUT] TC6: accelerator stalled (state=%0# w=%0# c=%0#)\n",
                  4,vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state,
                  16,(IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos),
                  8,vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos);
        vlSelf->conv1d_accel_tb__DOT__errors = ((IData)(1U) 
                                                + vlSelf->conv1d_accel_tb__DOT__errors);
    }
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)))) {
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got 
            = vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                                 & ((IData)(0x5000U) 
                                                    + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx))];
        if (VL_UNLIKELY(((IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got) 
                         != vlSelf->conv1d_accel_tb__DOT__ref_out
                         [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)]))) {
            VL_WRITEF("  [FAIL] TC6 out[%0d]: exp=%0d (0x%02x)  got=%0d (0x%02x)\n",
                      32,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx,
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got,
                      8,(IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got));
            vlSelf->conv1d_accel_tb__DOT__errors = 
                ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__errors);
        }
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__tc = 7U;
    vlSelf->conv1d_accel_tb__DOT__t_c_in = (0xffU & 
                                            ((IData)(1U) 
                                             + (IData)(
                                                       (3U 
                                                        & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_c_out = (0xffU 
                                             & ((IData)(1U) 
                                                + (IData)(
                                                          (3U 
                                                           & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_k_w = (0xffU & 
                                           ((IData)(1U) 
                                            + (IData)(
                                                      (3U 
                                                       & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_stride = (0xffU 
                                              & ((IData)(1U) 
                                                 + (IData)(
                                                           (1U 
                                                            & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_w_in = (0xffffU 
                                            & ((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                                               + ((IData)(vlSelf->conv1d_accel_tb__DOT__t_stride) 
                                                  * (IData)(
                                                            ((IData)(1U) 
                                                             + (IData)(
                                                                       (3U 
                                                                        & VL_RANDOM_I())))))));
    vlSelf->conv1d_accel_tb__DOT__t_patch_bytes = (0xffffU 
                                                   & ((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                                                      * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)));
    vlSelf->conv1d_accel_tb__DOT__t_w_out = (0xffffU 
                                             & ((IData)(1U) 
                                                + VL_DIV_III(32, 
                                                             ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                                              - (IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w)), (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride))));
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_in[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (0xffU & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_wt[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (0xffU & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__ref_bs[(0xfU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = VL_MODDIVS_III(32, (IData)(VL_RANDOM_I()), (IData)(0x100U));
        vlSelf->conv1d_accel_tb__DOT__ref_sh[(0xfU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (3U & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    VL_WRITEF("[TB] TC7 (rand)   c_in=%0# c_out=%0# k_w=%0# stride=%0# w_in=%0# w_out=%0#\n",
              8,vlSelf->conv1d_accel_tb__DOT__t_c_in,
              8,(IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out),
              8,vlSelf->conv1d_accel_tb__DOT__t_k_w,
              8,(IData)(vlSelf->conv1d_accel_tb__DOT__t_stride),
              16,vlSelf->conv1d_accel_tb__DOT__t_w_in,
              16,(IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out));
    vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo = 0U;
        while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                < (IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out))) {
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc = 0U;
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n = 0U;
            while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    < (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes))) {
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                    = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                       + VL_MULS_III(32, VL_EXTENDS_II(32,8, 
                                                       vlSelf->conv1d_accel_tb__DOT__ref_in
                                                       [
                                                       (0x1ffU 
                                                        & (((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                             * (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride)) 
                                                            * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)) 
                                                           + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))]), 
                                     VL_EXTENDS_II(32,8, 
                                                   vlSelf->conv1d_accel_tb__DOT__ref_wt
                                                   [
                                                   (0x1ffU 
                                                    & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
                                                        * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)) 
                                                       + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))])));
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n);
            }
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                   + vlSelf->conv1d_accel_tb__DOT__ref_bs
                   [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s 
                = VL_SHIFTRS_III(32,32,5, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc, 
                                 vlSelf->conv1d_accel_tb__DOT__ref_sh
                                 [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__ref_out[(0x1ffU 
                                                   & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                       * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)) 
                                                      + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co))] 
                = (VL_LTS_III(32, 0x7fU, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                    ? 0x7fU : (VL_GTS_III(32, 0xffffff80U, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                                ? 0x80U : (0xffU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)));
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo);
        }
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x1000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_in[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x2000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_wt[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3000U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)]);
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3001U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 8U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3002U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 0x10U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3003U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
               >> 0x18U);
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x4000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_sh[
            (0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(4U) + ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
                              * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x5000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] = 0xaaU;
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    __Vtask_conv1d_accel_tb__DOT__apb_write__55__data = 0x1000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__55__addr = 4U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__55__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__55__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__56__data = 0x2000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__56__addr = 8U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__56__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__56__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__57__data = 0x5000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__57__addr = 0xcU;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__57__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__57__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__58__data = 0x3000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__58__addr = 0x10U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__58__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__58__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__59__data = 0x4000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__59__addr = 0x20U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__59__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__59__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__60__data 
        = (((IData)(vlSelf->conv1d_accel_tb__DOT__t_stride) 
            << 0x18U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                          << 0x10U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                        << 8U) | (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in))));
    __Vtask_conv1d_accel_tb__DOT__apb_write__60__addr = 0x14U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__60__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__60__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__61__data 
        = vlSelf->conv1d_accel_tb__DOT__t_w_in;
    __Vtask_conv1d_accel_tb__DOT__apb_write__61__addr = 0x18U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__61__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__61__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__62__data = 1U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__62__addr = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__62__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__62__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt = 0U;
    while (((~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done)) 
            & VL_GTS_III(32, 0x186a0U, vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt))) {
        co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                           nullptr, 
                                                           "@(posedge conv1d_accel_tb.clk)", 
                                                           "conv1d_accel_tb.v", 
                                                           261);
        vlSelf->__Vm_traceActivity[2U] = 1U;
        vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt);
    }
    if (VL_UNLIKELY((1U & (~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done))))) {
        VL_WRITEF("  [TIMEOUT] TC7: accelerator stalled (state=%0# w=%0# c=%0#)\n",
                  4,vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state,
                  16,(IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos),
                  8,vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos);
        vlSelf->conv1d_accel_tb__DOT__errors = ((IData)(1U) 
                                                + vlSelf->conv1d_accel_tb__DOT__errors);
    }
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)))) {
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got 
            = vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                                 & ((IData)(0x5000U) 
                                                    + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx))];
        if (VL_UNLIKELY(((IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got) 
                         != vlSelf->conv1d_accel_tb__DOT__ref_out
                         [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)]))) {
            VL_WRITEF("  [FAIL] TC7 out[%0d]: exp=%0d (0x%02x)  got=%0d (0x%02x)\n",
                      32,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx,
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got,
                      8,(IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got));
            vlSelf->conv1d_accel_tb__DOT__errors = 
                ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__errors);
        }
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__tc = 8U;
    vlSelf->conv1d_accel_tb__DOT__t_c_in = (0xffU & 
                                            ((IData)(1U) 
                                             + (IData)(
                                                       (3U 
                                                        & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_c_out = (0xffU 
                                             & ((IData)(1U) 
                                                + (IData)(
                                                          (3U 
                                                           & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_k_w = (0xffU & 
                                           ((IData)(1U) 
                                            + (IData)(
                                                      (3U 
                                                       & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_stride = (0xffU 
                                              & ((IData)(1U) 
                                                 + (IData)(
                                                           (1U 
                                                            & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_w_in = (0xffffU 
                                            & ((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                                               + ((IData)(vlSelf->conv1d_accel_tb__DOT__t_stride) 
                                                  * (IData)(
                                                            ((IData)(1U) 
                                                             + (IData)(
                                                                       (3U 
                                                                        & VL_RANDOM_I())))))));
    vlSelf->conv1d_accel_tb__DOT__t_patch_bytes = (0xffffU 
                                                   & ((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                                                      * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)));
    vlSelf->conv1d_accel_tb__DOT__t_w_out = (0xffffU 
                                             & ((IData)(1U) 
                                                + VL_DIV_III(32, 
                                                             ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                                              - (IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w)), (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride))));
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_in[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (0xffU & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_wt[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (0xffU & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__ref_bs[(0xfU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = VL_MODDIVS_III(32, (IData)(VL_RANDOM_I()), (IData)(0x100U));
        vlSelf->conv1d_accel_tb__DOT__ref_sh[(0xfU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (3U & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    VL_WRITEF("[TB] TC8 (rand)   c_in=%0# c_out=%0# k_w=%0# stride=%0# w_in=%0# w_out=%0#\n",
              8,vlSelf->conv1d_accel_tb__DOT__t_c_in,
              8,(IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out),
              8,vlSelf->conv1d_accel_tb__DOT__t_k_w,
              8,(IData)(vlSelf->conv1d_accel_tb__DOT__t_stride),
              16,vlSelf->conv1d_accel_tb__DOT__t_w_in,
              16,(IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out));
    vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo = 0U;
        while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                < (IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out))) {
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc = 0U;
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n = 0U;
            while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    < (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes))) {
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                    = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                       + VL_MULS_III(32, VL_EXTENDS_II(32,8, 
                                                       vlSelf->conv1d_accel_tb__DOT__ref_in
                                                       [
                                                       (0x1ffU 
                                                        & (((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                             * (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride)) 
                                                            * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)) 
                                                           + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))]), 
                                     VL_EXTENDS_II(32,8, 
                                                   vlSelf->conv1d_accel_tb__DOT__ref_wt
                                                   [
                                                   (0x1ffU 
                                                    & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
                                                        * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)) 
                                                       + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))])));
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n);
            }
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                   + vlSelf->conv1d_accel_tb__DOT__ref_bs
                   [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s 
                = VL_SHIFTRS_III(32,32,5, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc, 
                                 vlSelf->conv1d_accel_tb__DOT__ref_sh
                                 [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__ref_out[(0x1ffU 
                                                   & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                       * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)) 
                                                      + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co))] 
                = (VL_LTS_III(32, 0x7fU, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                    ? 0x7fU : (VL_GTS_III(32, 0xffffff80U, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                                ? 0x80U : (0xffU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)));
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo);
        }
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x1000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_in[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x2000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_wt[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3000U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)]);
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3001U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 8U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3002U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 0x10U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3003U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
               >> 0x18U);
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x4000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_sh[
            (0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(4U) + ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
                              * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x5000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] = 0xaaU;
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    __Vtask_conv1d_accel_tb__DOT__apb_write__55__data = 0x1000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__55__addr = 4U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__55__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__55__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__56__data = 0x2000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__56__addr = 8U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__56__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__56__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__57__data = 0x5000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__57__addr = 0xcU;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__57__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__57__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__58__data = 0x3000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__58__addr = 0x10U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__58__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__58__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__59__data = 0x4000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__59__addr = 0x20U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__59__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__59__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__60__data 
        = (((IData)(vlSelf->conv1d_accel_tb__DOT__t_stride) 
            << 0x18U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                          << 0x10U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                        << 8U) | (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in))));
    __Vtask_conv1d_accel_tb__DOT__apb_write__60__addr = 0x14U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__60__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__60__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__61__data 
        = vlSelf->conv1d_accel_tb__DOT__t_w_in;
    __Vtask_conv1d_accel_tb__DOT__apb_write__61__addr = 0x18U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__61__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__61__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__62__data = 1U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__62__addr = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__62__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__62__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt = 0U;
    while (((~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done)) 
            & VL_GTS_III(32, 0x186a0U, vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt))) {
        co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                           nullptr, 
                                                           "@(posedge conv1d_accel_tb.clk)", 
                                                           "conv1d_accel_tb.v", 
                                                           261);
        vlSelf->__Vm_traceActivity[2U] = 1U;
        vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt);
    }
    if (VL_UNLIKELY((1U & (~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done))))) {
        VL_WRITEF("  [TIMEOUT] TC8: accelerator stalled (state=%0# w=%0# c=%0#)\n",
                  4,vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state,
                  16,(IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos),
                  8,vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos);
        vlSelf->conv1d_accel_tb__DOT__errors = ((IData)(1U) 
                                                + vlSelf->conv1d_accel_tb__DOT__errors);
    }
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)))) {
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got 
            = vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                                 & ((IData)(0x5000U) 
                                                    + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx))];
        if (VL_UNLIKELY(((IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got) 
                         != vlSelf->conv1d_accel_tb__DOT__ref_out
                         [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)]))) {
            VL_WRITEF("  [FAIL] TC8 out[%0d]: exp=%0d (0x%02x)  got=%0d (0x%02x)\n",
                      32,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx,
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got,
                      8,(IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got));
            vlSelf->conv1d_accel_tb__DOT__errors = 
                ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__errors);
        }
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__tc = 9U;
    vlSelf->conv1d_accel_tb__DOT__t_c_in = (0xffU & 
                                            ((IData)(1U) 
                                             + (IData)(
                                                       (3U 
                                                        & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_c_out = (0xffU 
                                             & ((IData)(1U) 
                                                + (IData)(
                                                          (3U 
                                                           & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_k_w = (0xffU & 
                                           ((IData)(1U) 
                                            + (IData)(
                                                      (3U 
                                                       & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_stride = (0xffU 
                                              & ((IData)(1U) 
                                                 + (IData)(
                                                           (1U 
                                                            & VL_RANDOM_I()))));
    vlSelf->conv1d_accel_tb__DOT__t_w_in = (0xffffU 
                                            & ((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                                               + ((IData)(vlSelf->conv1d_accel_tb__DOT__t_stride) 
                                                  * (IData)(
                                                            ((IData)(1U) 
                                                             + (IData)(
                                                                       (3U 
                                                                        & VL_RANDOM_I())))))));
    vlSelf->conv1d_accel_tb__DOT__t_patch_bytes = (0xffffU 
                                                   & ((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                                                      * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)));
    vlSelf->conv1d_accel_tb__DOT__t_w_out = (0xffffU 
                                             & ((IData)(1U) 
                                                + VL_DIV_III(32, 
                                                             ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                                              - (IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w)), (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride))));
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_in[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (0xffU & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__ref_wt[(0x1ffU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (0xffU & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    vlSelf->conv1d_accel_tb__DOT__i = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__i < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__ref_bs[(0xfU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = VL_MODDIVS_III(32, (IData)(VL_RANDOM_I()), (IData)(0x100U));
        vlSelf->conv1d_accel_tb__DOT__ref_sh[(0xfU 
                                              & vlSelf->conv1d_accel_tb__DOT__i)] 
            = (3U & VL_RANDOM_I());
        vlSelf->conv1d_accel_tb__DOT__i = ((IData)(1U) 
                                           + vlSelf->conv1d_accel_tb__DOT__i);
    }
    VL_WRITEF("[TB] TC9 (rand)   c_in=%0# c_out=%0# k_w=%0# stride=%0# w_in=%0# w_out=%0#\n",
              8,vlSelf->conv1d_accel_tb__DOT__t_c_in,
              8,(IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out),
              8,vlSelf->conv1d_accel_tb__DOT__t_k_w,
              8,(IData)(vlSelf->conv1d_accel_tb__DOT__t_stride),
              16,vlSelf->conv1d_accel_tb__DOT__t_w_in,
              16,(IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out));
    vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo = 0U;
        while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                < (IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out))) {
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc = 0U;
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n = 0U;
            while ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    < (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes))) {
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                    = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                       + VL_MULS_III(32, VL_EXTENDS_II(32,8, 
                                                       vlSelf->conv1d_accel_tb__DOT__ref_in
                                                       [
                                                       (0x1ffU 
                                                        & (((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                             * (IData)(vlSelf->conv1d_accel_tb__DOT__t_stride)) 
                                                            * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)) 
                                                           + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))]), 
                                     VL_EXTENDS_II(32,8, 
                                                   vlSelf->conv1d_accel_tb__DOT__ref_wt
                                                   [
                                                   (0x1ffU 
                                                    & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
                                                        * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)) 
                                                       + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n))])));
                vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n 
                    = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n);
            }
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                = (vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc 
                   + vlSelf->conv1d_accel_tb__DOT__ref_bs
                   [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s 
                = VL_SHIFTRS_III(32,32,5, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc, 
                                 vlSelf->conv1d_accel_tb__DOT__ref_sh
                                 [(0xfU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co)]);
            vlSelf->conv1d_accel_tb__DOT__ref_out[(0x1ffU 
                                                   & ((vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                                                       * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)) 
                                                      + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co))] 
                = (VL_LTS_III(32, 0x7fU, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                    ? 0x7fU : (VL_GTS_III(32, 0xffffff80U, vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)
                                ? 0x80U : (0xffU & vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s)));
            vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo 
                = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo);
        }
        vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_in) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x1000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_in[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes)))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x2000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_wt[
            (0x1ffU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3000U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)]);
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3001U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 8U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3002U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (0xffU & (vlSelf->conv1d_accel_tb__DOT__ref_bs
                        [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
                        >> 0x10U));
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x3003U) 
                                              + VL_SHIFTL_III(16,32,32, vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx, 2U)))] 
            = (vlSelf->conv1d_accel_tb__DOT__ref_bs
               [(0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)] 
               >> 0x18U);
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x4000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] 
            = vlSelf->conv1d_accel_tb__DOT__ref_sh[
            (0xfU & vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx)];
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            < ((IData)(4U) + ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
                              * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out))))) {
        vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                           & ((IData)(0x5000U) 
                                              + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx))] = 0xaaU;
        vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx);
    }
    __Vtask_conv1d_accel_tb__DOT__apb_write__55__data = 0x1000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__55__addr = 4U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__55__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__55__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__56__data = 0x2000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__56__addr = 8U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__56__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__56__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__57__data = 0x5000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__57__addr = 0xcU;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__57__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__57__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__58__data = 0x3000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__58__addr = 0x10U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__58__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__58__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__59__data = 0x4000U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__59__addr = 0x20U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__59__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__59__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__60__data 
        = (((IData)(vlSelf->conv1d_accel_tb__DOT__t_stride) 
            << 0x18U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_k_w) 
                          << 0x10U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out) 
                                        << 8U) | (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_in))));
    __Vtask_conv1d_accel_tb__DOT__apb_write__60__addr = 0x14U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__60__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__60__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__61__data 
        = vlSelf->conv1d_accel_tb__DOT__t_w_in;
    __Vtask_conv1d_accel_tb__DOT__apb_write__61__addr = 0x18U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__61__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__61__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__62__data = 1U;
    __Vtask_conv1d_accel_tb__DOT__apb_write__62__addr = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       146);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__paddr = __Vtask_conv1d_accel_tb__DOT__apb_write__62__addr;
    vlSelf->conv1d_accel_tb__DOT__pwdata = __Vtask_conv1d_accel_tb__DOT__apb_write__62__data;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       149);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__penable = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VdlySched.delay(0x3e8ULL, nullptr, 
                                       "conv1d_accel_tb.v", 
                                       151);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__psel = 0U;
    vlSelf->conv1d_accel_tb__DOT__penable = 0U;
    vlSelf->conv1d_accel_tb__DOT__pwrite = 0U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       284);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt = 0U;
    while (((~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done)) 
            & VL_GTS_III(32, 0x186a0U, vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt))) {
        co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                           nullptr, 
                                                           "@(posedge conv1d_accel_tb.clk)", 
                                                           "conv1d_accel_tb.v", 
                                                           261);
        vlSelf->__Vm_traceActivity[2U] = 1U;
        vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt);
    }
    if (VL_UNLIKELY((1U & (~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done))))) {
        VL_WRITEF("  [TIMEOUT] TC9: accelerator stalled (state=%0# w=%0# c=%0#)\n",
                  4,vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state,
                  16,(IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos),
                  8,vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos);
        vlSelf->conv1d_accel_tb__DOT__errors = ((IData)(1U) 
                                                + vlSelf->conv1d_accel_tb__DOT__errors);
    }
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    co_await vlSelf->__VtrigSched_h25d0ce37__0.trigger(0U, 
                                                       nullptr, 
                                                       "@(posedge conv1d_accel_tb.clk)", 
                                                       "conv1d_accel_tb.v", 
                                                       286);
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx = 0U;
    while ((vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            < ((IData)(vlSelf->conv1d_accel_tb__DOT__t_w_out) 
               * (IData)(vlSelf->conv1d_accel_tb__DOT__t_c_out)))) {
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got 
            = vlSelf->conv1d_accel_tb__DOT__mem[(0xffffU 
                                                 & ((IData)(0x5000U) 
                                                    + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx))];
        if (VL_UNLIKELY(((IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got) 
                         != vlSelf->conv1d_accel_tb__DOT__ref_out
                         [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)]))) {
            VL_WRITEF("  [FAIL] TC9 out[%0d]: exp=%0d (0x%02x)  got=%0d (0x%02x)\n",
                      32,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx,
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__ref_out
                      [(0x1ffU & vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx)],
                      8,vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got,
                      8,(IData)(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got));
            vlSelf->conv1d_accel_tb__DOT__errors = 
                ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__errors);
        }
        vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx 
            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx);
    }
    vlSelf->conv1d_accel_tb__DOT__tc = 0xaU;
    VL_WRITEF("=====================================\n  Test cases  : %0d\n  Errors      : %0d\n",
              32,vlSelf->conv1d_accel_tb__DOT__tc,32,
              vlSelf->conv1d_accel_tb__DOT__errors);
    if ((0U == vlSelf->conv1d_accel_tb__DOT__errors)) {
        VL_WRITEF("  ALL TESTS PASSED\n");
    } else {
        VL_WRITEF("  TESTS FAILED\n");
    }
    VL_WRITEF("=====================================\n");
    VL_FINISH_MT("conv1d_accel_tb.v", 459, "");
    vlSelf->__Vm_traceActivity[2U] = 1U;
}

VL_INLINE_OPT VlCoroutine Vconv1d_accel_tb___024root___eval_initial__TOP__Vtiming__3(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___eval_initial__TOP__Vtiming__3\n"); );
    // Body
    while (1U) {
        co_await vlSelf->__VdlySched.delay(0x2710ULL, 
                                           nullptr, 
                                           "conv1d_accel_tb.v", 
                                           25);
        vlSelf->conv1d_accel_tb__DOT__clk = (1U & (~ (IData)(vlSelf->conv1d_accel_tb__DOT__clk)));
    }
}

void Vconv1d_accel_tb___024root___eval_act(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___eval_act\n"); );
}

VL_INLINE_OPT void Vconv1d_accel_tb___024root___nba_sequent__TOP__0(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___nba_sequent__TOP__0\n"); );
    // Body
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__w_pos 
        = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos;
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state 
        = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state;
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_idx 
        = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx;
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__load_done 
        = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__load_done;
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__r_wait 
        = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wait;
    vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr = vlSelf->conv1d_accel_tb__DOT__haddr;
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_done 
        = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_done;
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__c_pos 
        = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos;
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__acc 
        = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__acc;
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_sel 
        = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_sel;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v0 = 0U;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v1 = 0U;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v0 = 0U;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v1 = 0U;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v0 = 0U;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1 = 0U;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2 = 0U;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3 = 0U;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4 = 0U;
    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v5 = 0U;
    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_idx 
        = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_idx;
    if (vlSelf->conv1d_accel_tb__DOT__rst_n) {
        if ((((IData)(vlSelf->conv1d_accel_tb__DOT__psel) 
              & (IData)(vlSelf->conv1d_accel_tb__DOT__penable)) 
             & (IData)(vlSelf->conv1d_accel_tb__DOT__pwrite))) {
            if ((1U & (~ ((IData)(vlSelf->conv1d_accel_tb__DOT__paddr) 
                          >> 5U)))) {
                if ((0x10U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))) {
                    if ((8U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))) {
                        if ((4U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))) {
                            vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift 
                                = (0x1fU & vlSelf->conv1d_accel_tb__DOT__pwdata);
                        }
                    }
                }
                if ((1U & (~ ((IData)(vlSelf->conv1d_accel_tb__DOT__paddr) 
                              >> 4U)))) {
                    if ((8U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))) {
                        if ((1U & (~ ((IData)(vlSelf->conv1d_accel_tb__DOT__paddr) 
                                      >> 2U)))) {
                            vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wt_addr 
                                = vlSelf->conv1d_accel_tb__DOT__pwdata;
                        }
                    }
                }
            }
        }
    } else {
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift = 0U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wt_addr = 0U;
    }
}

VL_INLINE_OPT void Vconv1d_accel_tb___024root___nba_sequent__TOP__1(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___nba_sequent__TOP__1\n"); );
    // Init
    SData/*15:0*/ __Vdlyvdim0__conv1d_accel_tb__DOT__mem__v0;
    __Vdlyvdim0__conv1d_accel_tb__DOT__mem__v0 = 0;
    CData/*7:0*/ __Vdlyvval__conv1d_accel_tb__DOT__mem__v0;
    __Vdlyvval__conv1d_accel_tb__DOT__mem__v0 = 0;
    CData/*0:0*/ __Vdlyvset__conv1d_accel_tb__DOT__mem__v0;
    __Vdlyvset__conv1d_accel_tb__DOT__mem__v0 = 0;
    SData/*15:0*/ __Vdlyvdim0__conv1d_accel_tb__DOT__mem__v1;
    __Vdlyvdim0__conv1d_accel_tb__DOT__mem__v1 = 0;
    CData/*7:0*/ __Vdlyvval__conv1d_accel_tb__DOT__mem__v1;
    __Vdlyvval__conv1d_accel_tb__DOT__mem__v1 = 0;
    CData/*0:0*/ __Vdlyvset__conv1d_accel_tb__DOT__mem__v1;
    __Vdlyvset__conv1d_accel_tb__DOT__mem__v1 = 0;
    SData/*15:0*/ __Vdlyvdim0__conv1d_accel_tb__DOT__mem__v2;
    __Vdlyvdim0__conv1d_accel_tb__DOT__mem__v2 = 0;
    CData/*7:0*/ __Vdlyvval__conv1d_accel_tb__DOT__mem__v2;
    __Vdlyvval__conv1d_accel_tb__DOT__mem__v2 = 0;
    SData/*15:0*/ __Vdlyvdim0__conv1d_accel_tb__DOT__mem__v3;
    __Vdlyvdim0__conv1d_accel_tb__DOT__mem__v3 = 0;
    CData/*7:0*/ __Vdlyvval__conv1d_accel_tb__DOT__mem__v3;
    __Vdlyvval__conv1d_accel_tb__DOT__mem__v3 = 0;
    SData/*15:0*/ __Vdlyvdim0__conv1d_accel_tb__DOT__mem__v4;
    __Vdlyvdim0__conv1d_accel_tb__DOT__mem__v4 = 0;
    CData/*7:0*/ __Vdlyvval__conv1d_accel_tb__DOT__mem__v4;
    __Vdlyvval__conv1d_accel_tb__DOT__mem__v4 = 0;
    // Body
    __Vdlyvset__conv1d_accel_tb__DOT__mem__v0 = 0U;
    __Vdlyvset__conv1d_accel_tb__DOT__mem__v1 = 0U;
    if ((((IData)(vlSelf->conv1d_accel_tb__DOT__ap_valid) 
          & (IData)(vlSelf->conv1d_accel_tb__DOT__ap_wr)) 
         & (IData)(vlSelf->conv1d_accel_tb__DOT__hready))) {
        if ((0U == (IData)(vlSelf->conv1d_accel_tb__DOT__ap_sz))) {
            __Vdlyvval__conv1d_accel_tb__DOT__mem__v0 
                = (0xffU & ((2U & vlSelf->conv1d_accel_tb__DOT__ap_addr)
                             ? ((1U & vlSelf->conv1d_accel_tb__DOT__ap_addr)
                                 ? (vlSelf->conv1d_accel_tb__DOT__hwdata 
                                    >> 0x18U) : (vlSelf->conv1d_accel_tb__DOT__hwdata 
                                                 >> 0x10U))
                             : ((1U & vlSelf->conv1d_accel_tb__DOT__ap_addr)
                                 ? (vlSelf->conv1d_accel_tb__DOT__hwdata 
                                    >> 8U) : vlSelf->conv1d_accel_tb__DOT__hwdata)));
            __Vdlyvset__conv1d_accel_tb__DOT__mem__v0 = 1U;
            __Vdlyvdim0__conv1d_accel_tb__DOT__mem__v0 
                = (0xffffU & vlSelf->conv1d_accel_tb__DOT__ap_addr);
        } else if ((2U == (IData)(vlSelf->conv1d_accel_tb__DOT__ap_sz))) {
            __Vdlyvval__conv1d_accel_tb__DOT__mem__v1 
                = (0xffU & vlSelf->conv1d_accel_tb__DOT__hwdata);
            __Vdlyvset__conv1d_accel_tb__DOT__mem__v1 = 1U;
            __Vdlyvdim0__conv1d_accel_tb__DOT__mem__v1 
                = (0xffffU & vlSelf->conv1d_accel_tb__DOT__ap_addr);
            __Vdlyvval__conv1d_accel_tb__DOT__mem__v2 
                = (0xffU & (vlSelf->conv1d_accel_tb__DOT__hwdata 
                            >> 8U));
            __Vdlyvdim0__conv1d_accel_tb__DOT__mem__v2 
                = (0xffffU & ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__ap_addr));
            __Vdlyvval__conv1d_accel_tb__DOT__mem__v3 
                = (0xffU & (vlSelf->conv1d_accel_tb__DOT__hwdata 
                            >> 0x10U));
            __Vdlyvdim0__conv1d_accel_tb__DOT__mem__v3 
                = (0xffffU & ((IData)(2U) + vlSelf->conv1d_accel_tb__DOT__ap_addr));
            __Vdlyvval__conv1d_accel_tb__DOT__mem__v4 
                = (vlSelf->conv1d_accel_tb__DOT__hwdata 
                   >> 0x18U);
            __Vdlyvdim0__conv1d_accel_tb__DOT__mem__v4 
                = (0xffffU & ((IData)(3U) + vlSelf->conv1d_accel_tb__DOT__ap_addr));
        }
    }
    if (__Vdlyvset__conv1d_accel_tb__DOT__mem__v0) {
        vlSelf->conv1d_accel_tb__DOT__mem[__Vdlyvdim0__conv1d_accel_tb__DOT__mem__v0] 
            = __Vdlyvval__conv1d_accel_tb__DOT__mem__v0;
    }
    if (__Vdlyvset__conv1d_accel_tb__DOT__mem__v1) {
        vlSelf->conv1d_accel_tb__DOT__mem[__Vdlyvdim0__conv1d_accel_tb__DOT__mem__v1] 
            = __Vdlyvval__conv1d_accel_tb__DOT__mem__v1;
        vlSelf->conv1d_accel_tb__DOT__mem[__Vdlyvdim0__conv1d_accel_tb__DOT__mem__v2] 
            = __Vdlyvval__conv1d_accel_tb__DOT__mem__v2;
        vlSelf->conv1d_accel_tb__DOT__mem[__Vdlyvdim0__conv1d_accel_tb__DOT__mem__v3] 
            = __Vdlyvval__conv1d_accel_tb__DOT__mem__v3;
        vlSelf->conv1d_accel_tb__DOT__mem[__Vdlyvdim0__conv1d_accel_tb__DOT__mem__v4] 
            = __Vdlyvval__conv1d_accel_tb__DOT__mem__v4;
    }
}

VL_INLINE_OPT void Vconv1d_accel_tb___024root___nba_sequent__TOP__2(Vconv1d_accel_tb___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root___nba_sequent__TOP__2\n"); );
    // Init
    CData/*5:0*/ __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v0;
    __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v0 = 0;
    IData/*31:0*/ __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v0;
    __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v0 = 0;
    CData/*5:0*/ __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v0;
    __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v0 = 0;
    IData/*31:0*/ __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v0;
    __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v0 = 0;
    CData/*5:0*/ __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v0;
    __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v0 = 0;
    IData/*31:0*/ __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v0;
    __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v0 = 0;
    CData/*5:0*/ __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1;
    __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1 = 0;
    CData/*4:0*/ __Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1;
    __Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1 = 0;
    CData/*7:0*/ __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1;
    __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1 = 0;
    CData/*5:0*/ __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2;
    __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2 = 0;
    CData/*4:0*/ __Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2;
    __Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2 = 0;
    CData/*7:0*/ __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2;
    __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2 = 0;
    CData/*5:0*/ __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3;
    __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3 = 0;
    CData/*4:0*/ __Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3;
    __Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3 = 0;
    CData/*7:0*/ __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3;
    __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3 = 0;
    CData/*5:0*/ __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4;
    __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4 = 0;
    CData/*4:0*/ __Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4;
    __Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4 = 0;
    CData/*7:0*/ __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4;
    __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4 = 0;
    CData/*5:0*/ __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v5;
    __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v5 = 0;
    CData/*5:0*/ __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v1;
    __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v1 = 0;
    IData/*31:0*/ __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v1;
    __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v1 = 0;
    CData/*5:0*/ __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v1;
    __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v1 = 0;
    IData/*31:0*/ __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v1;
    __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v1 = 0;
    // Body
    if (vlSelf->conv1d_accel_tb__DOT__rst_n) {
        if (vlSelf->conv1d_accel_tb__DOT__hready) {
            vlSelf->conv1d_accel_tb__DOT__ap_addr = vlSelf->conv1d_accel_tb__DOT__haddr;
            vlSelf->conv1d_accel_tb__DOT__ap_valid 
                = (1U & ((IData)(vlSelf->conv1d_accel_tb__DOT__htrans) 
                         >> 1U));
            vlSelf->conv1d_accel_tb__DOT__ap_wr = vlSelf->conv1d_accel_tb__DOT__hwrite;
            vlSelf->conv1d_accel_tb__DOT__ap_sz = vlSelf->conv1d_accel_tb__DOT__hsize;
        }
        if ((8U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state))) {
            if ((4U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state))) {
                if ((2U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state))) {
                    if ((1U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state))) {
                        if ((1U & (~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_done)))) {
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__acc 
                                = (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__acc 
                                   + vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac4);
                            if (((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_idx) 
                                 == (0x3fU & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_words) 
                                              - (IData)(1U))))) {
                                vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_done = 1U;
                            } else {
                                vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_idx 
                                    = (0x3fU & ((IData)(1U) 
                                                + (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_idx)));
                            }
                        }
                        if (((~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__load_done)) 
                             & (IData)(vlSelf->conv1d_accel_tb__DOT__hready))) {
                            if (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wait) {
                                vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr 
                                    = ((IData)(4U) 
                                       + vlSelf->conv1d_accel_tb__DOT__haddr);
                                vlSelf->conv1d_accel_tb__DOT__htrans = 3U;
                                vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__r_wait = 0U;
                            } else {
                                if (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_sel) {
                                    __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v0 
                                        = vlSelf->conv1d_accel_tb__DOT__hrdata;
                                    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v0 = 1U;
                                    __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v0 
                                        = (0x3fU & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx));
                                } else {
                                    __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v0 
                                        = vlSelf->conv1d_accel_tb__DOT__hrdata;
                                    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v0 = 1U;
                                    __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v0 
                                        = (0x3fU & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx));
                                }
                                if (((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx) 
                                     == (0x7fU & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_words) 
                                                  - (IData)(1U))))) {
                                    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__load_done = 1U;
                                    vlSelf->conv1d_accel_tb__DOT__htrans = 0U;
                                } else {
                                    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_idx 
                                        = (0x7fU & 
                                           ((IData)(1U) 
                                            + (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx)));
                                    vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr 
                                        = ((IData)(4U) 
                                           + vlSelf->conv1d_accel_tb__DOT__haddr);
                                    vlSelf->conv1d_accel_tb__DOT__htrans = 3U;
                                }
                            }
                        }
                        if (((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_done) 
                             & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__load_done))) {
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 0xbU;
                        }
                    } else {
                        vlSelf->conv1d_accel_tb__DOT__htrans = 0U;
                        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_busy = 0U;
                        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done = 1U;
                        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 0U;
                    }
                } else if ((1U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state))) {
                    if (((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos) 
                         < (0xffffU & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_out_cnt) 
                                       - (IData)(1U))))) {
                        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__w_pos 
                            = (0xffffU & ((IData)(1U) 
                                          + (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos)));
                        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__acc = 0U;
                        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_sel 
                            = (1U & (~ (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_sel)));
                        if (((0xffffU & ((IData)(1U) 
                                         + (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos))) 
                             < (0xffffU & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_out_cnt) 
                                           - (IData)(1U))))) {
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr 
                                = (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_src_addr 
                                   + ((((IData)(2U) 
                                        + (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos)) 
                                       * (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_stride)) 
                                      * (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_in)));
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_idx = 0U;
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_idx = 0U;
                            vlSelf->conv1d_accel_tb__DOT__htrans = 2U;
                            vlSelf->conv1d_accel_tb__DOT__hburst = 1U;
                            vlSelf->conv1d_accel_tb__DOT__hsize = 2U;
                            vlSelf->conv1d_accel_tb__DOT__hwrite = 0U;
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__r_wait = 1U;
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_done = 0U;
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__load_done = 0U;
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 0xfU;
                        } else {
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_idx = 0U;
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 6U;
                        }
                    } else {
                        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__w_pos = 0U;
                        if (((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos) 
                             < (0xffU & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_out) 
                                         - (IData)(1U))))) {
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__c_pos 
                                = (0xffU & ((IData)(1U) 
                                            + (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos)));
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__acc = 0U;
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_sel = 0U;
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 4U;
                        } else {
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 0xeU;
                        }
                    }
                } else if (vlSelf->conv1d_accel_tb__DOT__hready) {
                    vlSelf->conv1d_accel_tb__DOT__hwrite = 0U;
                    vlSelf->conv1d_accel_tb__DOT__htrans = 0U;
                    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 0xdU;
                }
            } else if ((2U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state))) {
                if ((1U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state))) {
                    vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr 
                        = ((vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_dst_addr 
                            + ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos) 
                               * (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_out))) 
                           + (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos));
                    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__result_byte 
                        = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__clipped;
                    vlSelf->conv1d_accel_tb__DOT__hwdata 
                        = (((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__clipped) 
                            << 0x18U) | (((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__clipped) 
                                          << 0x10U) 
                                         | (((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__clipped) 
                                             << 8U) 
                                            | (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__clipped))));
                    vlSelf->conv1d_accel_tb__DOT__htrans = 2U;
                    vlSelf->conv1d_accel_tb__DOT__hburst = 0U;
                    vlSelf->conv1d_accel_tb__DOT__hsize = 0U;
                    vlSelf->conv1d_accel_tb__DOT__hwrite = 1U;
                    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 0xcU;
                } else if (vlSelf->conv1d_accel_tb__DOT__hready) {
                    if (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wait) {
                        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__r_wait = 0U;
                    } else {
                        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__acc = 0U;
                        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__cur_shift 
                            = (0x1fU & ((2U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift_lane))
                                         ? ((1U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift_lane))
                                             ? (vlSelf->conv1d_accel_tb__DOT__hrdata 
                                                >> 0x18U)
                                             : (vlSelf->conv1d_accel_tb__DOT__hrdata 
                                                >> 0x10U))
                                         : ((1U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift_lane))
                                             ? (vlSelf->conv1d_accel_tb__DOT__hrdata 
                                                >> 8U)
                                             : vlSelf->conv1d_accel_tb__DOT__hrdata)));
                        vlSelf->conv1d_accel_tb__DOT__htrans = 0U;
                        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 2U;
                    }
                }
            } else if ((1U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state))) {
                if (vlSelf->conv1d_accel_tb__DOT__hready) {
                    if (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wait) {
                        vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr 
                            = ((IData)(4U) + vlSelf->conv1d_accel_tb__DOT__haddr);
                        vlSelf->conv1d_accel_tb__DOT__htrans = 3U;
                        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__r_wait = 0U;
                    } else {
                        __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v0 
                            = vlSelf->conv1d_accel_tb__DOT__hrdata;
                        vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v0 = 1U;
                        __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v0 
                            = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_word_idx;
                        if (((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_word_idx) 
                             == (0x3fU & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_words) 
                                          - (IData)(1U))))) {
                            vlSelf->conv1d_accel_tb__DOT__htrans = 0U;
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 7U;
                        } else {
                            vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_word_idx 
                                = (0x3fU & ((IData)(1U) 
                                            + (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_word_idx)));
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr 
                                = ((IData)(4U) + vlSelf->conv1d_accel_tb__DOT__haddr);
                            vlSelf->conv1d_accel_tb__DOT__htrans = 3U;
                        }
                    }
                }
            } else if (vlSelf->conv1d_accel_tb__DOT__hready) {
                if (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wait) {
                    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__r_wait = 0U;
                } else {
                    vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr 
                        = (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift_addr 
                           + (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos));
                    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__bias_val 
                        = vlSelf->conv1d_accel_tb__DOT__hrdata;
                    vlSelf->conv1d_accel_tb__DOT__htrans = 2U;
                    vlSelf->conv1d_accel_tb__DOT__hburst = 0U;
                    vlSelf->conv1d_accel_tb__DOT__hsize = 0U;
                    vlSelf->conv1d_accel_tb__DOT__hwrite = 0U;
                    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift_lane 
                        = (3U & (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift_addr 
                                 + (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos)));
                    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__r_wait = 1U;
                    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 0xaU;
                }
            }
        } else if ((4U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state))) {
            if ((2U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state))) {
                if ((1U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state))) {
                    vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr 
                        = (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_bs_addr 
                           + VL_SHIFTL_III(32,32,32, (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos), 2U));
                    vlSelf->conv1d_accel_tb__DOT__htrans = 2U;
                    vlSelf->conv1d_accel_tb__DOT__hburst = 0U;
                    vlSelf->conv1d_accel_tb__DOT__hsize = 2U;
                    vlSelf->conv1d_accel_tb__DOT__hwrite = 0U;
                    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__r_wait = 1U;
                    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 8U;
                } else {
                    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__acc 
                        = (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__acc 
                           + vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac4);
                    vlSelf->conv1d_accel_tb__DOT__htrans = 0U;
                    if (((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_idx) 
                         == (0x3fU & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_words) 
                                      - (IData)(1U))))) {
                        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 0xbU;
                    } else {
                        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_idx 
                            = (0x3fU & ((IData)(1U) 
                                        + (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_idx)));
                    }
                }
            } else if ((1U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state))) {
                if (vlSelf->conv1d_accel_tb__DOT__hready) {
                    if (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wait) {
                        vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr 
                            = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__haddr);
                        vlSelf->conv1d_accel_tb__DOT__htrans = 3U;
                        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__r_wait = 0U;
                    } else {
                        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_pack__DOT__cur_lane 
                            = (3U & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wt_lane0) 
                                     + (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx)));
                        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_pack__DOT__rdbyte 
                            = (0xffU & ((2U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_pack__DOT__cur_lane))
                                         ? ((1U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_pack__DOT__cur_lane))
                                             ? (vlSelf->conv1d_accel_tb__DOT__hrdata 
                                                >> 0x18U)
                                             : (vlSelf->conv1d_accel_tb__DOT__hrdata 
                                                >> 0x10U))
                                         : ((1U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_pack__DOT__cur_lane))
                                             ? (vlSelf->conv1d_accel_tb__DOT__hrdata 
                                                >> 8U)
                                             : vlSelf->conv1d_accel_tb__DOT__hrdata)));
                        if ((2U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx))) {
                            if ((1U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx))) {
                                __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1 
                                    = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_pack__DOT__rdbyte;
                                vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1 = 1U;
                                __Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1 = 0x18U;
                                __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1 
                                    = (0x1fU & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx) 
                                                >> 2U));
                            } else {
                                __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2 
                                    = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_pack__DOT__rdbyte;
                                vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2 = 1U;
                                __Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2 = 0x10U;
                                __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2 
                                    = (0x1fU & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx) 
                                                >> 2U));
                            }
                        } else if ((1U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx))) {
                            __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3 
                                = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_pack__DOT__rdbyte;
                            vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3 = 1U;
                            __Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3 = 8U;
                            __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3 
                                = (0x1fU & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx) 
                                            >> 2U));
                        } else {
                            __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4 
                                = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_pack__DOT__rdbyte;
                            vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4 = 1U;
                            __Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4 = 0U;
                            __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4 
                                = (0x1fU & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx) 
                                            >> 2U));
                        }
                        if (((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx) 
                             == (0xffffU & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_bytes) 
                                            - (IData)(1U))))) {
                            vlSelf->conv1d_accel_tb__DOT__htrans = 0U;
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 7U;
                        } else {
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_idx 
                                = (0x7fU & ((IData)(1U) 
                                            + (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx)));
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr 
                                = ((IData)(1U) + vlSelf->conv1d_accel_tb__DOT__haddr);
                            vlSelf->conv1d_accel_tb__DOT__htrans = 3U;
                        }
                    }
                }
            } else {
                vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr 
                    = vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_wt_addr;
                vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wt_lane0 
                    = (3U & vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_wt_addr);
                vlSelf->conv1d_accel_tb__DOT__htrans = 2U;
                vlSelf->conv1d_accel_tb__DOT__hburst = 1U;
                vlSelf->conv1d_accel_tb__DOT__hwrite = 0U;
                vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__r_wait = 1U;
                if (((0U == (3U & vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_wt_addr)) 
                     & (0U == (3U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_bytes))))) {
                    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_word_idx = 0U;
                    vlSelf->conv1d_accel_tb__DOT__hsize = 2U;
                    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 9U;
                } else {
                    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_idx = 0U;
                    vlSelf->conv1d_accel_tb__DOT__hsize = 0U;
                    vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v5 = 1U;
                    __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v5 
                        = (0x3fU & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_words) 
                                    - (IData)(1U)));
                    vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 5U;
                }
            }
        } else if ((2U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state))) {
            if ((1U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state))) {
                if (vlSelf->conv1d_accel_tb__DOT__hready) {
                    if (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wait) {
                        vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr 
                            = ((IData)(4U) + vlSelf->conv1d_accel_tb__DOT__haddr);
                        vlSelf->conv1d_accel_tb__DOT__htrans = 3U;
                        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__r_wait = 0U;
                    } else {
                        if (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_sel) {
                            __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v1 
                                = vlSelf->conv1d_accel_tb__DOT__hrdata;
                            vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v1 = 1U;
                            __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v1 
                                = (0x3fU & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx));
                        } else {
                            __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v1 
                                = vlSelf->conv1d_accel_tb__DOT__hrdata;
                            vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v1 = 1U;
                            __Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v1 
                                = (0x3fU & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx));
                        }
                        if (((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx) 
                             == (0x7fU & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_words) 
                                          - (IData)(1U))))) {
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_idx = 0U;
                            if ((1U < (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_out_cnt))) {
                                vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr 
                                    = (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_src_addr 
                                       + ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_stride) 
                                          * (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_in)));
                                vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_idx = 0U;
                                vlSelf->conv1d_accel_tb__DOT__htrans = 2U;
                                vlSelf->conv1d_accel_tb__DOT__hburst = 1U;
                                vlSelf->conv1d_accel_tb__DOT__hsize = 2U;
                                vlSelf->conv1d_accel_tb__DOT__hwrite = 0U;
                                vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__r_wait = 1U;
                                vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_done = 0U;
                                vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__load_done = 0U;
                                vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 0xfU;
                            } else {
                                vlSelf->conv1d_accel_tb__DOT__htrans = 0U;
                                vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 6U;
                            }
                        } else {
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_idx 
                                = (0x7fU & ((IData)(1U) 
                                            + (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx)));
                            vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr 
                                = ((IData)(4U) + vlSelf->conv1d_accel_tb__DOT__haddr);
                            vlSelf->conv1d_accel_tb__DOT__htrans = 3U;
                        }
                    }
                }
            } else {
                vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_idx = 0U;
                vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr 
                    = (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_src_addr 
                       + (((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos) 
                           * (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_stride)) 
                          * (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_in)));
                vlSelf->conv1d_accel_tb__DOT__htrans = 2U;
                vlSelf->conv1d_accel_tb__DOT__hburst = 1U;
                vlSelf->conv1d_accel_tb__DOT__hsize = 2U;
                vlSelf->conv1d_accel_tb__DOT__hwrite = 0U;
                vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__r_wait = 1U;
                vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 3U;
            }
        } else if ((1U & (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state))) {
            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__c_pos = 0U;
            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_sel = 0U;
            vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_bytes 
                = (0xffffU & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_k_w) 
                              * (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_in)));
            vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_words 
                = (0x3fU & (((IData)(3U) + ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_k_w) 
                                            * (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_in))) 
                            >> 2U));
            vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_out_cnt 
                = (0xffffU & ((IData)(1U) + VL_DIV_III(16, 
                                                       (0xffffU 
                                                        & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_w_in) 
                                                           - (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_k_w))), (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_stride))));
            vlSelf->conv1d_accel_tb__DOT__DUT__DOT__tail_bytes 
                = (3U & ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_k_w) 
                         * (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_in)));
            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__w_pos = 0U;
            vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 4U;
        } else {
            vlSelf->conv1d_accel_tb__DOT__htrans = 0U;
            if (((IData)(vlSelf->conv1d_accel_tb__DOT__psel) 
                 & ((IData)(vlSelf->conv1d_accel_tb__DOT__penable) 
                    & ((IData)(vlSelf->conv1d_accel_tb__DOT__pwrite) 
                       & (IData)(((0U == (0x3cU & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))) 
                                  & vlSelf->conv1d_accel_tb__DOT__pwdata)))))) {
                vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done = 0U;
                vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_busy = 1U;
                vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 1U;
            }
        }
        if ((((IData)(vlSelf->conv1d_accel_tb__DOT__psel) 
              & (IData)(vlSelf->conv1d_accel_tb__DOT__penable)) 
             & (IData)(vlSelf->conv1d_accel_tb__DOT__pwrite))) {
            if ((1U & (~ ((IData)(vlSelf->conv1d_accel_tb__DOT__paddr) 
                          >> 5U)))) {
                if ((1U & (~ ((IData)(vlSelf->conv1d_accel_tb__DOT__paddr) 
                              >> 4U)))) {
                    if ((1U & (~ ((IData)(vlSelf->conv1d_accel_tb__DOT__paddr) 
                                  >> 3U)))) {
                        if ((4U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))) {
                            vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_src_addr 
                                = vlSelf->conv1d_accel_tb__DOT__pwdata;
                        }
                    }
                    if ((8U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))) {
                        if ((4U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))) {
                            vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_dst_addr 
                                = vlSelf->conv1d_accel_tb__DOT__pwdata;
                        }
                    }
                }
                if ((0x10U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))) {
                    if ((1U & (~ ((IData)(vlSelf->conv1d_accel_tb__DOT__paddr) 
                                  >> 3U)))) {
                        if ((4U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))) {
                            vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_stride 
                                = (vlSelf->conv1d_accel_tb__DOT__pwdata 
                                   >> 0x18U);
                            vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_in 
                                = (0xffU & vlSelf->conv1d_accel_tb__DOT__pwdata);
                            vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_out 
                                = (0xffU & (vlSelf->conv1d_accel_tb__DOT__pwdata 
                                            >> 8U));
                            vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_k_w 
                                = (0xffU & (vlSelf->conv1d_accel_tb__DOT__pwdata 
                                            >> 0x10U));
                        }
                        if ((1U & (~ ((IData)(vlSelf->conv1d_accel_tb__DOT__paddr) 
                                      >> 2U)))) {
                            vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_bs_addr 
                                = vlSelf->conv1d_accel_tb__DOT__pwdata;
                        }
                    }
                    if ((8U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))) {
                        if ((1U & (~ ((IData)(vlSelf->conv1d_accel_tb__DOT__paddr) 
                                      >> 2U)))) {
                            vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_w_in 
                                = (0xffffU & vlSelf->conv1d_accel_tb__DOT__pwdata);
                        }
                    }
                }
            }
            if ((0x20U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))) {
                if ((1U & (~ ((IData)(vlSelf->conv1d_accel_tb__DOT__paddr) 
                              >> 4U)))) {
                    if ((1U & (~ ((IData)(vlSelf->conv1d_accel_tb__DOT__paddr) 
                                  >> 3U)))) {
                        if ((1U & (~ ((IData)(vlSelf->conv1d_accel_tb__DOT__paddr) 
                                      >> 2U)))) {
                            vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift_addr 
                                = vlSelf->conv1d_accel_tb__DOT__pwdata;
                        }
                    }
                }
            }
        }
    } else {
        vlSelf->conv1d_accel_tb__DOT__ap_valid = 0U;
        vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr = 0U;
        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__c_pos = 0U;
        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_idx = 0U;
        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_idx = 0U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_word_idx = 0U;
        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__acc = 0U;
        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_sel = 0U;
        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state = 0U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_busy = 0U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done = 0U;
        vlSelf->conv1d_accel_tb__DOT__htrans = 0U;
        vlSelf->conv1d_accel_tb__DOT__hwrite = 0U;
        vlSelf->conv1d_accel_tb__DOT__hburst = 0U;
        vlSelf->conv1d_accel_tb__DOT__hsize = 2U;
        vlSelf->conv1d_accel_tb__DOT__hwdata = 0U;
        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__w_pos = 0U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wt_lane0 = 0U;
        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__r_wait = 0U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__bias_val = 0U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__cur_shift = 0U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift_lane = 0U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_bytes = 0U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_words = 0U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_out_cnt = 0U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__tail_bytes = 0U;
        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_done = 0U;
        vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__load_done = 0U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_src_addr = 0U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_stride = 1U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_in = 1U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_out = 1U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_dst_addr = 0U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift_addr = 0U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_bs_addr = 0U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_k_w = 1U;
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_w_in = 1U;
    }
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_done 
        = vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_done;
    vlSelf->conv1d_accel_tb__DOT__haddr = vlSelf->__Vdly__conv1d_accel_tb__DOT__haddr;
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wait 
        = vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__r_wait;
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__load_done 
        = vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__load_done;
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx 
        = vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_idx;
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state = vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__state;
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos = vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__w_pos;
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos = vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__c_pos;
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__acc = vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__acc;
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_sel 
        = vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_sel;
    if (vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v0) {
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__in_buf1[__Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v0] 
            = __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v0;
    }
    if (vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v1) {
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__in_buf1[__Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v1] 
            = __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v1;
    }
    if (vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v0) {
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__in_buf0[__Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v0] 
            = __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v0;
    }
    if (vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v1) {
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__in_buf0[__Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v1] 
            = __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v1;
    }
    if (vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v0) {
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_buf[__Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v0] 
            = __Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v0;
    }
    if (vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1) {
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_buf[__Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1] 
            = (((~ ((IData)(0xffU) << (IData)(__Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1))) 
                & vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_buf
                [__Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1]) 
               | (0xffffffffULL & ((IData)(__Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1) 
                                   << (IData)(__Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1))));
    }
    if (vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2) {
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_buf[__Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2] 
            = (((~ ((IData)(0xffU) << (IData)(__Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2))) 
                & vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_buf
                [__Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2]) 
               | (0xffffffffULL & ((IData)(__Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2) 
                                   << (IData)(__Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2))));
    }
    if (vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3) {
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_buf[__Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3] 
            = (((~ ((IData)(0xffU) << (IData)(__Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3))) 
                & vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_buf
                [__Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3]) 
               | (0xffffffffULL & ((IData)(__Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3) 
                                   << (IData)(__Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3))));
    }
    if (vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4) {
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_buf[__Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4] 
            = (((~ ((IData)(0xffU) << (IData)(__Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4))) 
                & vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_buf
                [__Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4]) 
               | (0xffffffffULL & ((IData)(__Vdlyvval__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4) 
                                   << (IData)(__Vdlyvlsb__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4))));
    }
    if (vlSelf->__Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v5) {
        vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_buf[__Vdlyvdim0__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v5] = 0U;
    }
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_idx 
        = vlSelf->__Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_idx;
    vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_wt_addr 
        = (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wt_addr 
           + ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos) 
              * (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_bytes)));
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
