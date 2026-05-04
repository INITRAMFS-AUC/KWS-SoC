// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "Vconv1d_accel_tb__Syms.h"


void Vconv1d_accel_tb___024root__trace_chg_0_sub_0(Vconv1d_accel_tb___024root* vlSelf, VerilatedVcd::Buffer* bufp);

void Vconv1d_accel_tb___024root__trace_chg_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root__trace_chg_0\n"); );
    // Init
    Vconv1d_accel_tb___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vconv1d_accel_tb___024root*>(voidSelf);
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (VL_UNLIKELY(!vlSymsp->__Vm_activity)) return;
    // Body
    Vconv1d_accel_tb___024root__trace_chg_0_sub_0((&vlSymsp->TOP), bufp);
}

void Vconv1d_accel_tb___024root__trace_chg_0_sub_0(Vconv1d_accel_tb___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root__trace_chg_0_sub_0\n"); );
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode + 1);
    // Body
    if (VL_UNLIKELY((vlSelf->__Vm_traceActivity[1U] 
                     | vlSelf->__Vm_traceActivity[2U]))) {
        bufp->chgSData(oldp+0,(vlSelf->conv1d_accel_tb__DOT__paddr),16);
        bufp->chgBit(oldp+1,(vlSelf->conv1d_accel_tb__DOT__psel));
        bufp->chgBit(oldp+2,(vlSelf->conv1d_accel_tb__DOT__penable));
        bufp->chgBit(oldp+3,(vlSelf->conv1d_accel_tb__DOT__pwrite));
        bufp->chgIData(oldp+4,(vlSelf->conv1d_accel_tb__DOT__pwdata),32);
        bufp->chgBit(oldp+5,(vlSelf->conv1d_accel_tb__DOT__hready));
        bufp->chgIData(oldp+6,(vlSelf->conv1d_accel_tb__DOT__ref_bs[0]),32);
        bufp->chgIData(oldp+7,(vlSelf->conv1d_accel_tb__DOT__ref_bs[1]),32);
        bufp->chgIData(oldp+8,(vlSelf->conv1d_accel_tb__DOT__ref_bs[2]),32);
        bufp->chgIData(oldp+9,(vlSelf->conv1d_accel_tb__DOT__ref_bs[3]),32);
        bufp->chgIData(oldp+10,(vlSelf->conv1d_accel_tb__DOT__ref_bs[4]),32);
        bufp->chgIData(oldp+11,(vlSelf->conv1d_accel_tb__DOT__ref_bs[5]),32);
        bufp->chgIData(oldp+12,(vlSelf->conv1d_accel_tb__DOT__ref_bs[6]),32);
        bufp->chgIData(oldp+13,(vlSelf->conv1d_accel_tb__DOT__ref_bs[7]),32);
        bufp->chgIData(oldp+14,(vlSelf->conv1d_accel_tb__DOT__ref_bs[8]),32);
        bufp->chgIData(oldp+15,(vlSelf->conv1d_accel_tb__DOT__ref_bs[9]),32);
        bufp->chgIData(oldp+16,(vlSelf->conv1d_accel_tb__DOT__ref_bs[10]),32);
        bufp->chgIData(oldp+17,(vlSelf->conv1d_accel_tb__DOT__ref_bs[11]),32);
        bufp->chgIData(oldp+18,(vlSelf->conv1d_accel_tb__DOT__ref_bs[12]),32);
        bufp->chgIData(oldp+19,(vlSelf->conv1d_accel_tb__DOT__ref_bs[13]),32);
        bufp->chgIData(oldp+20,(vlSelf->conv1d_accel_tb__DOT__ref_bs[14]),32);
        bufp->chgIData(oldp+21,(vlSelf->conv1d_accel_tb__DOT__ref_bs[15]),32);
        bufp->chgCData(oldp+22,(vlSelf->conv1d_accel_tb__DOT__ref_sh[0]),5);
        bufp->chgCData(oldp+23,(vlSelf->conv1d_accel_tb__DOT__ref_sh[1]),5);
        bufp->chgCData(oldp+24,(vlSelf->conv1d_accel_tb__DOT__ref_sh[2]),5);
        bufp->chgCData(oldp+25,(vlSelf->conv1d_accel_tb__DOT__ref_sh[3]),5);
        bufp->chgCData(oldp+26,(vlSelf->conv1d_accel_tb__DOT__ref_sh[4]),5);
        bufp->chgCData(oldp+27,(vlSelf->conv1d_accel_tb__DOT__ref_sh[5]),5);
        bufp->chgCData(oldp+28,(vlSelf->conv1d_accel_tb__DOT__ref_sh[6]),5);
        bufp->chgCData(oldp+29,(vlSelf->conv1d_accel_tb__DOT__ref_sh[7]),5);
        bufp->chgCData(oldp+30,(vlSelf->conv1d_accel_tb__DOT__ref_sh[8]),5);
        bufp->chgCData(oldp+31,(vlSelf->conv1d_accel_tb__DOT__ref_sh[9]),5);
        bufp->chgCData(oldp+32,(vlSelf->conv1d_accel_tb__DOT__ref_sh[10]),5);
        bufp->chgCData(oldp+33,(vlSelf->conv1d_accel_tb__DOT__ref_sh[11]),5);
        bufp->chgCData(oldp+34,(vlSelf->conv1d_accel_tb__DOT__ref_sh[12]),5);
        bufp->chgCData(oldp+35,(vlSelf->conv1d_accel_tb__DOT__ref_sh[13]),5);
        bufp->chgCData(oldp+36,(vlSelf->conv1d_accel_tb__DOT__ref_sh[14]),5);
        bufp->chgCData(oldp+37,(vlSelf->conv1d_accel_tb__DOT__ref_sh[15]),5);
        bufp->chgCData(oldp+38,(vlSelf->conv1d_accel_tb__DOT__t_c_in),8);
        bufp->chgCData(oldp+39,(vlSelf->conv1d_accel_tb__DOT__t_c_out),8);
        bufp->chgCData(oldp+40,(vlSelf->conv1d_accel_tb__DOT__t_k_w),8);
        bufp->chgCData(oldp+41,(vlSelf->conv1d_accel_tb__DOT__t_stride),8);
        bufp->chgSData(oldp+42,(vlSelf->conv1d_accel_tb__DOT__t_w_in),16);
        bufp->chgSData(oldp+43,(vlSelf->conv1d_accel_tb__DOT__t_w_out),16);
        bufp->chgSData(oldp+44,(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes),16);
        bufp->chgIData(oldp+45,(vlSelf->conv1d_accel_tb__DOT__tc),32);
        bufp->chgIData(oldp+46,(vlSelf->conv1d_accel_tb__DOT__errors),32);
        bufp->chgIData(oldp+47,(vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co),32);
        bufp->chgIData(oldp+48,(vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo),32);
        bufp->chgIData(oldp+49,(vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n),32);
        bufp->chgIData(oldp+50,(vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc),32);
        bufp->chgIData(oldp+51,(vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s),32);
        bufp->chgIData(oldp+52,(vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx),32);
        bufp->chgIData(oldp+53,(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx),32);
        bufp->chgCData(oldp+54,(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got),8);
        bufp->chgIData(oldp+55,(vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt),32);
        bufp->chgIData(oldp+56,(vlSelf->conv1d_accel_tb__DOT__i),32);
        bufp->chgBit(oldp+57,(((IData)(vlSelf->conv1d_accel_tb__DOT__psel) 
                               & ((IData)(vlSelf->conv1d_accel_tb__DOT__penable) 
                                  & ((IData)(vlSelf->conv1d_accel_tb__DOT__pwrite) 
                                     & (IData)(((0U 
                                                 == 
                                                 (0x3cU 
                                                  & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))) 
                                                & vlSelf->conv1d_accel_tb__DOT__pwdata)))))));
    }
    if (VL_UNLIKELY(vlSelf->__Vm_traceActivity[3U])) {
        bufp->chgIData(oldp+58,(vlSelf->conv1d_accel_tb__DOT__haddr),32);
        bufp->chgCData(oldp+59,(vlSelf->conv1d_accel_tb__DOT__hburst),3);
        bufp->chgCData(oldp+60,(vlSelf->conv1d_accel_tb__DOT__hsize),3);
        bufp->chgCData(oldp+61,(vlSelf->conv1d_accel_tb__DOT__htrans),2);
        bufp->chgBit(oldp+62,(vlSelf->conv1d_accel_tb__DOT__hwrite));
        bufp->chgIData(oldp+63,(vlSelf->conv1d_accel_tb__DOT__hwdata),32);
        bufp->chgIData(oldp+64,(vlSelf->conv1d_accel_tb__DOT__ap_addr),32);
        bufp->chgBit(oldp+65,(vlSelf->conv1d_accel_tb__DOT__ap_wr));
        bufp->chgBit(oldp+66,(vlSelf->conv1d_accel_tb__DOT__ap_valid));
        bufp->chgCData(oldp+67,(vlSelf->conv1d_accel_tb__DOT__ap_sz),3);
        bufp->chgIData(oldp+68,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_src_addr),32);
        bufp->chgIData(oldp+69,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_dst_addr),32);
        bufp->chgIData(oldp+70,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_bs_addr),32);
        bufp->chgIData(oldp+71,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift_addr),32);
        bufp->chgCData(oldp+72,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_in),8);
        bufp->chgCData(oldp+73,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_out),8);
        bufp->chgCData(oldp+74,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_k_w),8);
        bufp->chgCData(oldp+75,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_stride),8);
        bufp->chgSData(oldp+76,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_w_in),16);
        bufp->chgBit(oldp+77,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_busy));
        bufp->chgBit(oldp+78,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done));
        bufp->chgSData(oldp+79,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_bytes),16);
        bufp->chgCData(oldp+80,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_words),6);
        bufp->chgSData(oldp+81,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_out_cnt),16);
        bufp->chgCData(oldp+82,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__tail_bytes),2);
        bufp->chgCData(oldp+83,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wt_lane0),2);
        bufp->chgIData(oldp+84,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_wt_addr),32);
        bufp->chgCData(oldp+85,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state),4);
        bufp->chgSData(oldp+86,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos),16);
        bufp->chgCData(oldp+87,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos),8);
        bufp->chgCData(oldp+88,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx),7);
        bufp->chgCData(oldp+89,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_idx),6);
        bufp->chgCData(oldp+90,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_word_idx),6);
        bufp->chgIData(oldp+91,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__acc),32);
        bufp->chgIData(oldp+92,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__bias_val),32);
        bufp->chgCData(oldp+93,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__result_byte),8);
        bufp->chgCData(oldp+94,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__cur_shift),5);
        bufp->chgCData(oldp+95,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift_lane),2);
        bufp->chgBit(oldp+96,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wait));
        bufp->chgBit(oldp+97,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_sel));
        bufp->chgBit(oldp+98,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_done));
        bufp->chgBit(oldp+99,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__load_done));
        bufp->chgIData(oldp+100,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_in_word),32);
        bufp->chgCData(oldp+101,((0xffU & vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_in_word)),8);
        bufp->chgCData(oldp+102,((0xffU & (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_in_word 
                                           >> 8U))),8);
        bufp->chgCData(oldp+103,((0xffU & (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_in_word 
                                           >> 0x10U))),8);
        bufp->chgCData(oldp+104,((vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_in_word 
                                  >> 0x18U)),8);
        bufp->chgCData(oldp+105,((0xffU & vlSelf->conv1d_accel_tb__DOT__DUT__DOT____VdfgTmp_h2d093ac0__0)),8);
        bufp->chgCData(oldp+106,((0xffU & (vlSelf->conv1d_accel_tb__DOT__DUT__DOT____VdfgTmp_h2d093ac0__0 
                                           >> 8U))),8);
        bufp->chgCData(oldp+107,((0xffU & (vlSelf->conv1d_accel_tb__DOT__DUT__DOT____VdfgTmp_h2d093ac0__0 
                                           >> 0x10U))),8);
        bufp->chgCData(oldp+108,((vlSelf->conv1d_accel_tb__DOT__DUT__DOT____VdfgTmp_h2d093ac0__0 
                                  >> 0x18U)),8);
        bufp->chgSData(oldp+109,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p0),16);
        bufp->chgSData(oldp+110,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p1),16);
        bufp->chgSData(oldp+111,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p2),16);
        bufp->chgSData(oldp+112,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p3),16);
        bufp->chgIData(oldp+113,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac4),32);
        bufp->chgIData(oldp+114,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__shifted),32);
        bufp->chgCData(oldp+115,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__clipped),8);
        bufp->chgCData(oldp+116,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_pack__DOT__cur_lane),2);
        bufp->chgCData(oldp+117,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_pack__DOT__rdbyte),8);
    }
    bufp->chgBit(oldp+118,(vlSelf->conv1d_accel_tb__DOT__clk));
    bufp->chgBit(oldp+119,(vlSelf->conv1d_accel_tb__DOT__rst_n));
    bufp->chgIData(oldp+120,(((0x20U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))
                               ? ((0x10U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))
                                   ? 0U : ((8U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))
                                            ? 0U : 
                                           ((4U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))
                                             ? 0U : vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift_addr)))
                               : ((0x10U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))
                                   ? ((8U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))
                                       ? ((4U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))
                                           ? (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift)
                                           : (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_w_in))
                                       : ((4U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))
                                           ? (((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_stride) 
                                               << 0x18U) 
                                              | (((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_k_w) 
                                                  << 0x10U) 
                                                 | (((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_out) 
                                                     << 8U) 
                                                    | (IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_in))))
                                           : vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_bs_addr))
                                   : ((8U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))
                                       ? ((4U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))
                                           ? vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_dst_addr
                                           : vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wt_addr)
                                       : ((4U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))
                                           ? vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_src_addr
                                           : (((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done) 
                                               << 9U) 
                                              | ((IData)(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_busy) 
                                                 << 8U))))))),32);
    bufp->chgIData(oldp+121,(vlSelf->conv1d_accel_tb__DOT__hrdata),32);
    bufp->chgIData(oldp+122,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wt_addr),32);
    bufp->chgCData(oldp+123,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift),5);
}

void Vconv1d_accel_tb___024root__trace_cleanup(void* voidSelf, VerilatedVcd* /*unused*/) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root__trace_cleanup\n"); );
    // Init
    Vconv1d_accel_tb___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vconv1d_accel_tb___024root*>(voidSelf);
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    vlSymsp->__Vm_activity = false;
    vlSymsp->TOP.__Vm_traceActivity[0U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[1U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[2U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[3U] = 0U;
}
