// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "Vconv1d_accel_tb__Syms.h"


VL_ATTR_COLD void Vconv1d_accel_tb___024root__trace_init_sub__TOP__0(Vconv1d_accel_tb___024root* vlSelf, VerilatedVcd* tracep) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root__trace_init_sub__TOP__0\n"); );
    // Init
    const int c = vlSymsp->__Vm_baseCode;
    // Body
    tracep->pushPrefix("conv1d_accel_tb", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+119,0,"clk",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+120,0,"rst_n",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+1,0,"paddr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBit(c+2,0,"psel",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+3,0,"penable",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+4,0,"pwrite",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+5,0,"pwdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+121,0,"prdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+59,0,"haddr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+60,0,"hburst",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+61,0,"hsize",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+62,0,"htrans",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+63,0,"hwrite",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+64,0,"hwdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+122,0,"hrdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+6,0,"hready",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+125,0,"hresp",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+126,0,"SRC_BASE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+127,0,"WT_BASE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+128,0,"BS_BASE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+129,0,"SH_BASE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+130,0,"DST_BASE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+65,0,"ap_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+66,0,"ap_wr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+67,0,"ap_valid",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+68,0,"ap_sz",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->pushPrefix("ref_bs", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 16; ++i) {
        tracep->declBus(c+7+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 31,0);
    }
    tracep->popPrefix();
    tracep->pushPrefix("ref_sh", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 16; ++i) {
        tracep->declBus(c+23+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 4,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+39,0,"t_c_in",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+40,0,"t_c_out",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+41,0,"t_k_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+42,0,"t_stride",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+43,0,"t_w_in",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+44,0,"t_w_out",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+45,0,"t_patch_bytes",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+46,0,"tc",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INTEGER, false,-1, 31,0);
    tracep->declBus(c+47,0,"errors",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INTEGER, false,-1, 31,0);
    tracep->declBus(c+48,0,"compute_reference__Vstatic__co",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INTEGER, false,-1, 31,0);
    tracep->declBus(c+49,0,"compute_reference__Vstatic__wo",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INTEGER, false,-1, 31,0);
    tracep->declBus(c+50,0,"compute_reference__Vstatic__n",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INTEGER, false,-1, 31,0);
    tracep->declBus(c+51,0,"compute_reference__Vstatic__acc",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INTEGER, false,-1, 31,0);
    tracep->declBus(c+52,0,"compute_reference__Vstatic__s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INTEGER, false,-1, 31,0);
    tracep->declBus(c+53,0,"load_memory__Vstatic__idx",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INTEGER, false,-1, 31,0);
    tracep->declBus(c+54,0,"check_output__Vstatic__idx",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INTEGER, false,-1, 31,0);
    tracep->declBus(c+55,0,"check_output__Vstatic__got",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+56,0,"wait_done__Vstatic__cnt",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INTEGER, false,-1, 31,0);
    tracep->declBus(c+57,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INTEGER, false,-1, 31,0);
    tracep->declBus(c+131,0,"rd",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->pushPrefix("DUT", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+119,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+120,0,"rst_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+1,0,"paddr",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBit(c+2,0,"psel",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+3,0,"penable",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+4,0,"pwrite",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+5,0,"pwdata",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+121,0,"prdata",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+132,0,"pready",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+125,0,"pslverr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+59,0,"haddr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+60,0,"hburst",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+61,0,"hsize",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+62,0,"htrans",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+63,0,"hwrite",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+64,0,"hwdata",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+133,0,"hprot",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBit(c+125,0,"hmastlock",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+122,0,"hrdata",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+6,0,"hready",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+125,0,"hresp",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+134,0,"HTRANS_IDLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+135,0,"HTRANS_NONSEQ",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+136,0,"HTRANS_SEQ",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+137,0,"HBURST_SINGLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+138,0,"HBURST_INCR",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+137,0,"HSIZE_BYTE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+139,0,"HSIZE_WORD",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+69,0,"r_src_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+123,0,"r_wt_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+70,0,"r_dst_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+71,0,"r_bs_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+72,0,"r_shift_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+73,0,"r_c_in",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+74,0,"r_c_out",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+75,0,"r_k_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+76,0,"r_stride",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+77,0,"r_w_in",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+124,0,"r_shift",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+78,0,"r_busy",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+79,0,"r_done",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+80,0,"patch_bytes",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+81,0,"patch_words",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 5,0);
    tracep->declBus(c+82,0,"w_out_cnt",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+83,0,"tail_bytes",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+84,0,"r_wt_lane0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+85,0,"w_wt_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+140,0,"S_IDLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+141,0,"S_INIT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+142,0,"S_IN_ADDR",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+133,0,"S_IN_DATA",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+143,0,"S_WT_ADDR",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+144,0,"S_WT_DATA",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+145,0,"S_MAC",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+146,0,"S_BIAS_ADDR",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+147,0,"S_BIAS_DATA",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+148,0,"S_WT_DATA_W",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+149,0,"S_SHIFT_DATA",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+150,0,"S_WRITE_ADDR",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+151,0,"S_WRITE_DATA",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+152,0,"S_ADVANCE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+153,0,"S_DONE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+154,0,"S_MAC_OVLP",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+86,0,"state",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+87,0,"w_pos",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+88,0,"c_pos",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+89,0,"buf_idx",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 6,0);
    tracep->declBus(c+90,0,"mac_idx",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 5,0);
    tracep->declBus(c+91,0,"wt_word_idx",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 5,0);
    tracep->declBus(c+92,0,"acc",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+93,0,"bias_val",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+94,0,"result_byte",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+95,0,"cur_shift",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+96,0,"r_shift_lane",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+97,0,"r_wait",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+98,0,"buf_sel",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+99,0,"mac_done",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+100,0,"load_done",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+58,0,"start_pulse",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+101,0,"mac_in_word",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+102,0,"i0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+103,0,"i1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+104,0,"i2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+105,0,"i3",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+106,0,"w0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+107,0,"w1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+108,0,"w2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+109,0,"w3",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+110,0,"p0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+111,0,"p1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+112,0,"p2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+113,0,"p3",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+114,0,"mac4",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+115,0,"shifted",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+116,0,"clipped",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->pushPrefix("wt_pack", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+117,0,"cur_lane",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+118,0,"rdbyte",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
}

VL_ATTR_COLD void Vconv1d_accel_tb___024root__trace_init_top(Vconv1d_accel_tb___024root* vlSelf, VerilatedVcd* tracep) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root__trace_init_top\n"); );
    // Body
    Vconv1d_accel_tb___024root__trace_init_sub__TOP__0(vlSelf, tracep);
}

VL_ATTR_COLD void Vconv1d_accel_tb___024root__trace_const_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
VL_ATTR_COLD void Vconv1d_accel_tb___024root__trace_full_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
void Vconv1d_accel_tb___024root__trace_chg_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
void Vconv1d_accel_tb___024root__trace_cleanup(void* voidSelf, VerilatedVcd* /*unused*/);

VL_ATTR_COLD void Vconv1d_accel_tb___024root__trace_register(Vconv1d_accel_tb___024root* vlSelf, VerilatedVcd* tracep) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root__trace_register\n"); );
    // Body
    tracep->addConstCb(&Vconv1d_accel_tb___024root__trace_const_0, 0U, vlSelf);
    tracep->addFullCb(&Vconv1d_accel_tb___024root__trace_full_0, 0U, vlSelf);
    tracep->addChgCb(&Vconv1d_accel_tb___024root__trace_chg_0, 0U, vlSelf);
    tracep->addCleanupCb(&Vconv1d_accel_tb___024root__trace_cleanup, vlSelf);
}

VL_ATTR_COLD void Vconv1d_accel_tb___024root__trace_const_0_sub_0(Vconv1d_accel_tb___024root* vlSelf, VerilatedVcd::Buffer* bufp);

VL_ATTR_COLD void Vconv1d_accel_tb___024root__trace_const_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root__trace_const_0\n"); );
    // Init
    Vconv1d_accel_tb___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vconv1d_accel_tb___024root*>(voidSelf);
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    Vconv1d_accel_tb___024root__trace_const_0_sub_0((&vlSymsp->TOP), bufp);
}

VL_ATTR_COLD void Vconv1d_accel_tb___024root__trace_const_0_sub_0(Vconv1d_accel_tb___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root__trace_const_0_sub_0\n"); );
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode);
    // Body
    bufp->fullBit(oldp+125,(0U));
    bufp->fullIData(oldp+126,(0x1000U),32);
    bufp->fullIData(oldp+127,(0x2000U),32);
    bufp->fullIData(oldp+128,(0x3000U),32);
    bufp->fullIData(oldp+129,(0x4000U),32);
    bufp->fullIData(oldp+130,(0x5000U),32);
    bufp->fullIData(oldp+131,(vlSelf->conv1d_accel_tb__DOT__rd),32);
    bufp->fullBit(oldp+132,(1U));
    bufp->fullCData(oldp+133,(3U),4);
    bufp->fullCData(oldp+134,(0U),2);
    bufp->fullCData(oldp+135,(2U),2);
    bufp->fullCData(oldp+136,(3U),2);
    bufp->fullCData(oldp+137,(0U),3);
    bufp->fullCData(oldp+138,(1U),3);
    bufp->fullCData(oldp+139,(2U),3);
    bufp->fullCData(oldp+140,(0U),4);
    bufp->fullCData(oldp+141,(1U),4);
    bufp->fullCData(oldp+142,(2U),4);
    bufp->fullCData(oldp+143,(4U),4);
    bufp->fullCData(oldp+144,(5U),4);
    bufp->fullCData(oldp+145,(6U),4);
    bufp->fullCData(oldp+146,(7U),4);
    bufp->fullCData(oldp+147,(8U),4);
    bufp->fullCData(oldp+148,(9U),4);
    bufp->fullCData(oldp+149,(0xaU),4);
    bufp->fullCData(oldp+150,(0xbU),4);
    bufp->fullCData(oldp+151,(0xcU),4);
    bufp->fullCData(oldp+152,(0xdU),4);
    bufp->fullCData(oldp+153,(0xeU),4);
    bufp->fullCData(oldp+154,(0xfU),4);
}

VL_ATTR_COLD void Vconv1d_accel_tb___024root__trace_full_0_sub_0(Vconv1d_accel_tb___024root* vlSelf, VerilatedVcd::Buffer* bufp);

VL_ATTR_COLD void Vconv1d_accel_tb___024root__trace_full_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root__trace_full_0\n"); );
    // Init
    Vconv1d_accel_tb___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vconv1d_accel_tb___024root*>(voidSelf);
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    Vconv1d_accel_tb___024root__trace_full_0_sub_0((&vlSymsp->TOP), bufp);
}

VL_ATTR_COLD void Vconv1d_accel_tb___024root__trace_full_0_sub_0(Vconv1d_accel_tb___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    if (false && vlSelf) {}  // Prevent unused
    Vconv1d_accel_tb__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vconv1d_accel_tb___024root__trace_full_0_sub_0\n"); );
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode);
    // Body
    bufp->fullSData(oldp+1,(vlSelf->conv1d_accel_tb__DOT__paddr),16);
    bufp->fullBit(oldp+2,(vlSelf->conv1d_accel_tb__DOT__psel));
    bufp->fullBit(oldp+3,(vlSelf->conv1d_accel_tb__DOT__penable));
    bufp->fullBit(oldp+4,(vlSelf->conv1d_accel_tb__DOT__pwrite));
    bufp->fullIData(oldp+5,(vlSelf->conv1d_accel_tb__DOT__pwdata),32);
    bufp->fullBit(oldp+6,(vlSelf->conv1d_accel_tb__DOT__hready));
    bufp->fullIData(oldp+7,(vlSelf->conv1d_accel_tb__DOT__ref_bs[0]),32);
    bufp->fullIData(oldp+8,(vlSelf->conv1d_accel_tb__DOT__ref_bs[1]),32);
    bufp->fullIData(oldp+9,(vlSelf->conv1d_accel_tb__DOT__ref_bs[2]),32);
    bufp->fullIData(oldp+10,(vlSelf->conv1d_accel_tb__DOT__ref_bs[3]),32);
    bufp->fullIData(oldp+11,(vlSelf->conv1d_accel_tb__DOT__ref_bs[4]),32);
    bufp->fullIData(oldp+12,(vlSelf->conv1d_accel_tb__DOT__ref_bs[5]),32);
    bufp->fullIData(oldp+13,(vlSelf->conv1d_accel_tb__DOT__ref_bs[6]),32);
    bufp->fullIData(oldp+14,(vlSelf->conv1d_accel_tb__DOT__ref_bs[7]),32);
    bufp->fullIData(oldp+15,(vlSelf->conv1d_accel_tb__DOT__ref_bs[8]),32);
    bufp->fullIData(oldp+16,(vlSelf->conv1d_accel_tb__DOT__ref_bs[9]),32);
    bufp->fullIData(oldp+17,(vlSelf->conv1d_accel_tb__DOT__ref_bs[10]),32);
    bufp->fullIData(oldp+18,(vlSelf->conv1d_accel_tb__DOT__ref_bs[11]),32);
    bufp->fullIData(oldp+19,(vlSelf->conv1d_accel_tb__DOT__ref_bs[12]),32);
    bufp->fullIData(oldp+20,(vlSelf->conv1d_accel_tb__DOT__ref_bs[13]),32);
    bufp->fullIData(oldp+21,(vlSelf->conv1d_accel_tb__DOT__ref_bs[14]),32);
    bufp->fullIData(oldp+22,(vlSelf->conv1d_accel_tb__DOT__ref_bs[15]),32);
    bufp->fullCData(oldp+23,(vlSelf->conv1d_accel_tb__DOT__ref_sh[0]),5);
    bufp->fullCData(oldp+24,(vlSelf->conv1d_accel_tb__DOT__ref_sh[1]),5);
    bufp->fullCData(oldp+25,(vlSelf->conv1d_accel_tb__DOT__ref_sh[2]),5);
    bufp->fullCData(oldp+26,(vlSelf->conv1d_accel_tb__DOT__ref_sh[3]),5);
    bufp->fullCData(oldp+27,(vlSelf->conv1d_accel_tb__DOT__ref_sh[4]),5);
    bufp->fullCData(oldp+28,(vlSelf->conv1d_accel_tb__DOT__ref_sh[5]),5);
    bufp->fullCData(oldp+29,(vlSelf->conv1d_accel_tb__DOT__ref_sh[6]),5);
    bufp->fullCData(oldp+30,(vlSelf->conv1d_accel_tb__DOT__ref_sh[7]),5);
    bufp->fullCData(oldp+31,(vlSelf->conv1d_accel_tb__DOT__ref_sh[8]),5);
    bufp->fullCData(oldp+32,(vlSelf->conv1d_accel_tb__DOT__ref_sh[9]),5);
    bufp->fullCData(oldp+33,(vlSelf->conv1d_accel_tb__DOT__ref_sh[10]),5);
    bufp->fullCData(oldp+34,(vlSelf->conv1d_accel_tb__DOT__ref_sh[11]),5);
    bufp->fullCData(oldp+35,(vlSelf->conv1d_accel_tb__DOT__ref_sh[12]),5);
    bufp->fullCData(oldp+36,(vlSelf->conv1d_accel_tb__DOT__ref_sh[13]),5);
    bufp->fullCData(oldp+37,(vlSelf->conv1d_accel_tb__DOT__ref_sh[14]),5);
    bufp->fullCData(oldp+38,(vlSelf->conv1d_accel_tb__DOT__ref_sh[15]),5);
    bufp->fullCData(oldp+39,(vlSelf->conv1d_accel_tb__DOT__t_c_in),8);
    bufp->fullCData(oldp+40,(vlSelf->conv1d_accel_tb__DOT__t_c_out),8);
    bufp->fullCData(oldp+41,(vlSelf->conv1d_accel_tb__DOT__t_k_w),8);
    bufp->fullCData(oldp+42,(vlSelf->conv1d_accel_tb__DOT__t_stride),8);
    bufp->fullSData(oldp+43,(vlSelf->conv1d_accel_tb__DOT__t_w_in),16);
    bufp->fullSData(oldp+44,(vlSelf->conv1d_accel_tb__DOT__t_w_out),16);
    bufp->fullSData(oldp+45,(vlSelf->conv1d_accel_tb__DOT__t_patch_bytes),16);
    bufp->fullIData(oldp+46,(vlSelf->conv1d_accel_tb__DOT__tc),32);
    bufp->fullIData(oldp+47,(vlSelf->conv1d_accel_tb__DOT__errors),32);
    bufp->fullIData(oldp+48,(vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__co),32);
    bufp->fullIData(oldp+49,(vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__wo),32);
    bufp->fullIData(oldp+50,(vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__n),32);
    bufp->fullIData(oldp+51,(vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__acc),32);
    bufp->fullIData(oldp+52,(vlSelf->conv1d_accel_tb__DOT__compute_reference__Vstatic__s),32);
    bufp->fullIData(oldp+53,(vlSelf->conv1d_accel_tb__DOT__load_memory__Vstatic__idx),32);
    bufp->fullIData(oldp+54,(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__idx),32);
    bufp->fullCData(oldp+55,(vlSelf->conv1d_accel_tb__DOT__check_output__Vstatic__got),8);
    bufp->fullIData(oldp+56,(vlSelf->conv1d_accel_tb__DOT__wait_done__Vstatic__cnt),32);
    bufp->fullIData(oldp+57,(vlSelf->conv1d_accel_tb__DOT__i),32);
    bufp->fullBit(oldp+58,(((IData)(vlSelf->conv1d_accel_tb__DOT__psel) 
                            & ((IData)(vlSelf->conv1d_accel_tb__DOT__penable) 
                               & ((IData)(vlSelf->conv1d_accel_tb__DOT__pwrite) 
                                  & (IData)(((0U == 
                                              (0x3cU 
                                               & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))) 
                                             & vlSelf->conv1d_accel_tb__DOT__pwdata)))))));
    bufp->fullIData(oldp+59,(vlSelf->conv1d_accel_tb__DOT__haddr),32);
    bufp->fullCData(oldp+60,(vlSelf->conv1d_accel_tb__DOT__hburst),3);
    bufp->fullCData(oldp+61,(vlSelf->conv1d_accel_tb__DOT__hsize),3);
    bufp->fullCData(oldp+62,(vlSelf->conv1d_accel_tb__DOT__htrans),2);
    bufp->fullBit(oldp+63,(vlSelf->conv1d_accel_tb__DOT__hwrite));
    bufp->fullIData(oldp+64,(vlSelf->conv1d_accel_tb__DOT__hwdata),32);
    bufp->fullIData(oldp+65,(vlSelf->conv1d_accel_tb__DOT__ap_addr),32);
    bufp->fullBit(oldp+66,(vlSelf->conv1d_accel_tb__DOT__ap_wr));
    bufp->fullBit(oldp+67,(vlSelf->conv1d_accel_tb__DOT__ap_valid));
    bufp->fullCData(oldp+68,(vlSelf->conv1d_accel_tb__DOT__ap_sz),3);
    bufp->fullIData(oldp+69,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_src_addr),32);
    bufp->fullIData(oldp+70,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_dst_addr),32);
    bufp->fullIData(oldp+71,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_bs_addr),32);
    bufp->fullIData(oldp+72,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift_addr),32);
    bufp->fullCData(oldp+73,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_in),8);
    bufp->fullCData(oldp+74,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_c_out),8);
    bufp->fullCData(oldp+75,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_k_w),8);
    bufp->fullCData(oldp+76,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_stride),8);
    bufp->fullSData(oldp+77,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_w_in),16);
    bufp->fullBit(oldp+78,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_busy));
    bufp->fullBit(oldp+79,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_done));
    bufp->fullSData(oldp+80,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_bytes),16);
    bufp->fullCData(oldp+81,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__patch_words),6);
    bufp->fullSData(oldp+82,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_out_cnt),16);
    bufp->fullCData(oldp+83,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__tail_bytes),2);
    bufp->fullCData(oldp+84,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wt_lane0),2);
    bufp->fullIData(oldp+85,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_wt_addr),32);
    bufp->fullCData(oldp+86,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__state),4);
    bufp->fullSData(oldp+87,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__w_pos),16);
    bufp->fullCData(oldp+88,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__c_pos),8);
    bufp->fullCData(oldp+89,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_idx),7);
    bufp->fullCData(oldp+90,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_idx),6);
    bufp->fullCData(oldp+91,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_word_idx),6);
    bufp->fullIData(oldp+92,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__acc),32);
    bufp->fullIData(oldp+93,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__bias_val),32);
    bufp->fullCData(oldp+94,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__result_byte),8);
    bufp->fullCData(oldp+95,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__cur_shift),5);
    bufp->fullCData(oldp+96,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift_lane),2);
    bufp->fullBit(oldp+97,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wait));
    bufp->fullBit(oldp+98,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__buf_sel));
    bufp->fullBit(oldp+99,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_done));
    bufp->fullBit(oldp+100,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__load_done));
    bufp->fullIData(oldp+101,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_in_word),32);
    bufp->fullCData(oldp+102,((0xffU & vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_in_word)),8);
    bufp->fullCData(oldp+103,((0xffU & (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_in_word 
                                        >> 8U))),8);
    bufp->fullCData(oldp+104,((0xffU & (vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_in_word 
                                        >> 0x10U))),8);
    bufp->fullCData(oldp+105,((vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac_in_word 
                               >> 0x18U)),8);
    bufp->fullCData(oldp+106,((0xffU & vlSelf->conv1d_accel_tb__DOT__DUT__DOT____VdfgTmp_h2d093ac0__0)),8);
    bufp->fullCData(oldp+107,((0xffU & (vlSelf->conv1d_accel_tb__DOT__DUT__DOT____VdfgTmp_h2d093ac0__0 
                                        >> 8U))),8);
    bufp->fullCData(oldp+108,((0xffU & (vlSelf->conv1d_accel_tb__DOT__DUT__DOT____VdfgTmp_h2d093ac0__0 
                                        >> 0x10U))),8);
    bufp->fullCData(oldp+109,((vlSelf->conv1d_accel_tb__DOT__DUT__DOT____VdfgTmp_h2d093ac0__0 
                               >> 0x18U)),8);
    bufp->fullSData(oldp+110,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p0),16);
    bufp->fullSData(oldp+111,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p1),16);
    bufp->fullSData(oldp+112,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p2),16);
    bufp->fullSData(oldp+113,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__p3),16);
    bufp->fullIData(oldp+114,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__mac4),32);
    bufp->fullIData(oldp+115,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__shifted),32);
    bufp->fullCData(oldp+116,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__clipped),8);
    bufp->fullCData(oldp+117,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_pack__DOT__cur_lane),2);
    bufp->fullCData(oldp+118,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__wt_pack__DOT__rdbyte),8);
    bufp->fullBit(oldp+119,(vlSelf->conv1d_accel_tb__DOT__clk));
    bufp->fullBit(oldp+120,(vlSelf->conv1d_accel_tb__DOT__rst_n));
    bufp->fullIData(oldp+121,(((0x20U & (IData)(vlSelf->conv1d_accel_tb__DOT__paddr))
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
    bufp->fullIData(oldp+122,(vlSelf->conv1d_accel_tb__DOT__hrdata),32);
    bufp->fullIData(oldp+123,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_wt_addr),32);
    bufp->fullCData(oldp+124,(vlSelf->conv1d_accel_tb__DOT__DUT__DOT__r_shift),5);
}
