// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vour.h for the primary calling header

#include "Vour__pch.h"
#include "Vour__Syms.h"
#include "Vour___024root.h"

// Parameter definitions for Vour___024root
constexpr IData/*31:0*/ Vour___024root::our__DOT__WIDTH;


void Vour___024root___ctor_var_reset(Vour___024root* vlSelf);

Vour___024root::Vour___024root(Vour__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vour___024root___ctor_var_reset(this);
}

void Vour___024root::__Vconfigure(bool first) {
    if (false && first) {}  // Prevent unused
}

Vour___024root::~Vour___024root() {
}
