// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vour.h for the primary calling header

#ifndef VERILATED_VOUR___024ROOT_H_
#define VERILATED_VOUR___024ROOT_H_  // guard

#include "verilated.h"


class Vour__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vour___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ __VactContinue;
    IData/*31:0*/ our__DOT__readme;
    IData/*31:0*/ our__DOT__writeme;
    IData/*31:0*/ __VactIterCount;
    VlTriggerVec<0> __VactTriggered;
    VlTriggerVec<0> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vour__Syms* const vlSymsp;

    // PARAMETERS
    static constexpr IData/*31:0*/ our__DOT__WIDTH = 0x00000020U;

    // CONSTRUCTORS
    Vour___024root(Vour__Syms* symsp, const char* v__name);
    ~Vour___024root();
    VL_UNCOPYABLE(Vour___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
