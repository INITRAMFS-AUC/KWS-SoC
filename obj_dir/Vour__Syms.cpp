// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vour__pch.h"
#include "Vour.h"
#include "Vour___024root.h"

// FUNCTIONS
Vour__Syms::~Vour__Syms()
{

    // Tear down scope hierarchy
    __Vhier.remove(0, &__Vscope_our);

}

Vour__Syms::Vour__Syms(VerilatedContext* contextp, const char* namep, Vour* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup module instances
    , TOP{this, namep}
{
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-12);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
    // Setup scopes
    __Vscope_our.configure(this, name(), "our", "our", -12, VerilatedScope::SCOPE_MODULE);

    // Set up scope hierarchy
    __Vhier.add(0, &__Vscope_our);

    // Setup export functions
    for (int __Vfinal = 0; __Vfinal < 2; ++__Vfinal) {
        __Vscope_our.varInsert(__Vfinal,"WIDTH", const_cast<void*>(static_cast<const void*>(&(TOP.our__DOT__WIDTH))), true, VLVT_UINT32,VLVD_NODIR|VLVF_PUB_RD,1 ,31,0);
        __Vscope_our.varInsert(__Vfinal,"readme", &(TOP.our__DOT__readme), false, VLVT_UINT32,VLVD_NODIR|VLVF_PUB_RD,1 ,31,0);
        __Vscope_our.varInsert(__Vfinal,"writeme", &(TOP.our__DOT__writeme), false, VLVT_UINT32,VLVD_NODIR|VLVF_PUB_RW,1 ,31,0);
    }
}
