// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vconv1d_accel_tb.h for the primary calling header

#ifndef VERILATED_VCONV1D_ACCEL_TB___024ROOT_H_
#define VERILATED_VCONV1D_ACCEL_TB___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vconv1d_accel_tb__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vconv1d_accel_tb___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        CData/*0:0*/ conv1d_accel_tb__DOT__clk;
        CData/*0:0*/ conv1d_accel_tb__DOT__rst_n;
        CData/*0:0*/ conv1d_accel_tb__DOT__psel;
        CData/*0:0*/ conv1d_accel_tb__DOT__penable;
        CData/*0:0*/ conv1d_accel_tb__DOT__pwrite;
        CData/*2:0*/ conv1d_accel_tb__DOT__hburst;
        CData/*2:0*/ conv1d_accel_tb__DOT__hsize;
        CData/*1:0*/ conv1d_accel_tb__DOT__htrans;
        CData/*0:0*/ conv1d_accel_tb__DOT__hwrite;
        CData/*0:0*/ conv1d_accel_tb__DOT__hready;
        CData/*0:0*/ conv1d_accel_tb__DOT__ap_wr;
        CData/*0:0*/ conv1d_accel_tb__DOT__ap_valid;
        CData/*2:0*/ conv1d_accel_tb__DOT__ap_sz;
        CData/*7:0*/ conv1d_accel_tb__DOT__t_c_in;
        CData/*7:0*/ conv1d_accel_tb__DOT__t_c_out;
        CData/*7:0*/ conv1d_accel_tb__DOT__t_k_w;
        CData/*7:0*/ conv1d_accel_tb__DOT__t_stride;
        CData/*7:0*/ conv1d_accel_tb__DOT__check_output__Vstatic__got;
        CData/*7:0*/ conv1d_accel_tb__DOT__DUT__DOT__r_c_in;
        CData/*7:0*/ conv1d_accel_tb__DOT__DUT__DOT__r_c_out;
        CData/*7:0*/ conv1d_accel_tb__DOT__DUT__DOT__r_k_w;
        CData/*7:0*/ conv1d_accel_tb__DOT__DUT__DOT__r_stride;
        CData/*4:0*/ conv1d_accel_tb__DOT__DUT__DOT__r_shift;
        CData/*0:0*/ conv1d_accel_tb__DOT__DUT__DOT__r_busy;
        CData/*0:0*/ conv1d_accel_tb__DOT__DUT__DOT__r_done;
        CData/*5:0*/ conv1d_accel_tb__DOT__DUT__DOT__patch_words;
        CData/*1:0*/ conv1d_accel_tb__DOT__DUT__DOT__tail_bytes;
        CData/*1:0*/ conv1d_accel_tb__DOT__DUT__DOT__r_wt_lane0;
        CData/*3:0*/ conv1d_accel_tb__DOT__DUT__DOT__state;
        CData/*7:0*/ conv1d_accel_tb__DOT__DUT__DOT__c_pos;
        CData/*6:0*/ conv1d_accel_tb__DOT__DUT__DOT__buf_idx;
        CData/*5:0*/ conv1d_accel_tb__DOT__DUT__DOT__mac_idx;
        CData/*5:0*/ conv1d_accel_tb__DOT__DUT__DOT__wt_word_idx;
        CData/*7:0*/ conv1d_accel_tb__DOT__DUT__DOT__result_byte;
        CData/*4:0*/ conv1d_accel_tb__DOT__DUT__DOT__cur_shift;
        CData/*1:0*/ conv1d_accel_tb__DOT__DUT__DOT__r_shift_lane;
        CData/*0:0*/ conv1d_accel_tb__DOT__DUT__DOT__r_wait;
        CData/*0:0*/ conv1d_accel_tb__DOT__DUT__DOT__buf_sel;
        CData/*0:0*/ conv1d_accel_tb__DOT__DUT__DOT__mac_done;
        CData/*0:0*/ conv1d_accel_tb__DOT__DUT__DOT__load_done;
        CData/*7:0*/ conv1d_accel_tb__DOT__DUT__DOT__clipped;
        CData/*1:0*/ conv1d_accel_tb__DOT__DUT__DOT__wt_pack__DOT__cur_lane;
        CData/*7:0*/ conv1d_accel_tb__DOT__DUT__DOT__wt_pack__DOT__rdbyte;
        CData/*0:0*/ __Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_done;
        CData/*5:0*/ __Vdly__conv1d_accel_tb__DOT__DUT__DOT__mac_idx;
        CData/*0:0*/ __Vdly__conv1d_accel_tb__DOT__DUT__DOT__r_wait;
        CData/*0:0*/ __Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v0;
        CData/*0:0*/ __Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v0;
        CData/*0:0*/ __Vdly__conv1d_accel_tb__DOT__DUT__DOT__load_done;
        CData/*6:0*/ __Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_idx;
        CData/*3:0*/ __Vdly__conv1d_accel_tb__DOT__DUT__DOT__state;
        CData/*0:0*/ __Vdly__conv1d_accel_tb__DOT__DUT__DOT__buf_sel;
        CData/*7:0*/ __Vdly__conv1d_accel_tb__DOT__DUT__DOT__c_pos;
        CData/*0:0*/ __Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v0;
        CData/*0:0*/ __Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v1;
        CData/*0:0*/ __Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v2;
        CData/*0:0*/ __Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v3;
        CData/*0:0*/ __Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v4;
        CData/*0:0*/ __Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__wt_buf__v5;
        CData/*0:0*/ __Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf1__v1;
        CData/*0:0*/ __Vdlyvset__conv1d_accel_tb__DOT__DUT__DOT__in_buf0__v1;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __Vtrigprevexpr___TOP__conv1d_accel_tb__DOT__clk__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__conv1d_accel_tb__DOT__rst_n__0;
    };
    struct {
        CData/*0:0*/ __VactContinue;
        SData/*15:0*/ conv1d_accel_tb__DOT__paddr;
        SData/*15:0*/ conv1d_accel_tb__DOT__t_w_in;
        SData/*15:0*/ conv1d_accel_tb__DOT__t_w_out;
        SData/*15:0*/ conv1d_accel_tb__DOT__t_patch_bytes;
        SData/*15:0*/ conv1d_accel_tb__DOT__DUT__DOT__r_w_in;
        SData/*15:0*/ conv1d_accel_tb__DOT__DUT__DOT__patch_bytes;
        SData/*15:0*/ conv1d_accel_tb__DOT__DUT__DOT__w_out_cnt;
        SData/*15:0*/ conv1d_accel_tb__DOT__DUT__DOT__w_pos;
        SData/*15:0*/ conv1d_accel_tb__DOT__DUT__DOT__p0;
        SData/*15:0*/ conv1d_accel_tb__DOT__DUT__DOT__p1;
        SData/*15:0*/ conv1d_accel_tb__DOT__DUT__DOT__p2;
        SData/*15:0*/ conv1d_accel_tb__DOT__DUT__DOT__p3;
        SData/*15:0*/ __Vdly__conv1d_accel_tb__DOT__DUT__DOT__w_pos;
        IData/*31:0*/ conv1d_accel_tb__DOT__pwdata;
        IData/*31:0*/ conv1d_accel_tb__DOT__haddr;
        IData/*31:0*/ conv1d_accel_tb__DOT__hwdata;
        IData/*31:0*/ conv1d_accel_tb__DOT__hrdata;
        IData/*31:0*/ conv1d_accel_tb__DOT__ap_addr;
        IData/*31:0*/ conv1d_accel_tb__DOT__tc;
        IData/*31:0*/ conv1d_accel_tb__DOT__errors;
        IData/*31:0*/ conv1d_accel_tb__DOT__compute_reference__Vstatic__co;
        IData/*31:0*/ conv1d_accel_tb__DOT__compute_reference__Vstatic__wo;
        IData/*31:0*/ conv1d_accel_tb__DOT__compute_reference__Vstatic__n;
        IData/*31:0*/ conv1d_accel_tb__DOT__compute_reference__Vstatic__acc;
        IData/*31:0*/ conv1d_accel_tb__DOT__compute_reference__Vstatic__s;
        IData/*31:0*/ conv1d_accel_tb__DOT__load_memory__Vstatic__idx;
        IData/*31:0*/ conv1d_accel_tb__DOT__check_output__Vstatic__idx;
        IData/*31:0*/ conv1d_accel_tb__DOT__wait_done__Vstatic__cnt;
        IData/*31:0*/ conv1d_accel_tb__DOT__i;
        IData/*31:0*/ conv1d_accel_tb__DOT__rd;
        IData/*31:0*/ conv1d_accel_tb__DOT__DUT__DOT__r_src_addr;
        IData/*31:0*/ conv1d_accel_tb__DOT__DUT__DOT__r_wt_addr;
        IData/*31:0*/ conv1d_accel_tb__DOT__DUT__DOT__r_dst_addr;
        IData/*31:0*/ conv1d_accel_tb__DOT__DUT__DOT__r_bs_addr;
        IData/*31:0*/ conv1d_accel_tb__DOT__DUT__DOT__r_shift_addr;
        IData/*31:0*/ conv1d_accel_tb__DOT__DUT__DOT__w_wt_addr;
        IData/*31:0*/ conv1d_accel_tb__DOT__DUT__DOT__acc;
        IData/*31:0*/ conv1d_accel_tb__DOT__DUT__DOT__bias_val;
        IData/*31:0*/ conv1d_accel_tb__DOT__DUT__DOT__mac_in_word;
        IData/*31:0*/ conv1d_accel_tb__DOT__DUT__DOT__mac4;
        IData/*31:0*/ conv1d_accel_tb__DOT__DUT__DOT__shifted;
        IData/*31:0*/ conv1d_accel_tb__DOT__DUT__DOT____VdfgTmp_h2d093ac0__0;
        IData/*31:0*/ __Vdly__conv1d_accel_tb__DOT__DUT__DOT__acc;
        IData/*31:0*/ __Vdly__conv1d_accel_tb__DOT__haddr;
        IData/*31:0*/ __VactIterCount;
        VlUnpacked<CData/*7:0*/, 65536> conv1d_accel_tb__DOT__mem;
        VlUnpacked<CData/*7:0*/, 512> conv1d_accel_tb__DOT__ref_in;
        VlUnpacked<CData/*7:0*/, 512> conv1d_accel_tb__DOT__ref_wt;
        VlUnpacked<IData/*31:0*/, 16> conv1d_accel_tb__DOT__ref_bs;
        VlUnpacked<CData/*4:0*/, 16> conv1d_accel_tb__DOT__ref_sh;
        VlUnpacked<CData/*7:0*/, 512> conv1d_accel_tb__DOT__ref_out;
        VlUnpacked<IData/*31:0*/, 64> conv1d_accel_tb__DOT__DUT__DOT__in_buf0;
        VlUnpacked<IData/*31:0*/, 64> conv1d_accel_tb__DOT__DUT__DOT__in_buf1;
        VlUnpacked<IData/*31:0*/, 64> conv1d_accel_tb__DOT__DUT__DOT__wt_buf;
        VlUnpacked<CData/*0:0*/, 4> __Vm_traceActivity;
    };
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h25d0ce37__0;
    VlTriggerScheduler __VtrigSched_h635928e4__0;
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<4> __VactTriggered;
    VlTriggerVec<4> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vconv1d_accel_tb__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vconv1d_accel_tb___024root(Vconv1d_accel_tb__Syms* symsp, const char* v__name);
    ~Vconv1d_accel_tb___024root();
    VL_UNCOPYABLE(Vconv1d_accel_tb___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
