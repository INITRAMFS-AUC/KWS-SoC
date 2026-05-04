// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vconv1d_accel_tb.h for the primary calling header

#include "Vconv1d_accel_tb__pch.h"
#include "Vconv1d_accel_tb__Syms.h"
#include "Vconv1d_accel_tb___024root.h"

void Vconv1d_accel_tb___024root___ctor_var_reset(Vconv1d_accel_tb___024root* vlSelf);

Vconv1d_accel_tb___024root::Vconv1d_accel_tb___024root(Vconv1d_accel_tb__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , __VdlySched{*symsp->_vm_contextp__}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vconv1d_accel_tb___024root___ctor_var_reset(this);
}

void Vconv1d_accel_tb___024root::__Vconfigure(bool first) {
    if (false && first) {}  // Prevent unused
}

Vconv1d_accel_tb___024root::~Vconv1d_accel_tb___024root() {
}
