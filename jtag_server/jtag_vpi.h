#ifndef JTAG_VPI_H
#define JTAG_VPI_H

#include "verilated.h"
#include "verilated_vpi.h"
#include "Vexample_soc.h"
#include "register.h"
#include <algorithm>
#include <cstdint>
#include <memory>

const std::vector<std::string> jtagInterfaceNames = {"tdi", "tdo", "tms", "tck", "trst_n"};

//JTAG VPI wrapper
class JtagVpi {
private:
    std::unique_ptr<Vexample_soc> top;
    vpiHandle tdiHandle;
    vpiHandle tdoHandle;
    vpiHandle tmsHandle;
    vpiHandle tckHandle;
    vpiHandle trst_nHandle;

    void initializeHandles();
    void initializeTop(VerilatedContext* contextp = nullptr);

public:
    JtagVpi(VerilatedContext* contextp = nullptr);
    ~JtagVpi();
    Register getTdi();
    Register getTdo();
    Register getTms();
    Register getTck();
    Register getTrst_n();
    void setTdi(int value);
    void setTms(int value);
    void setTck(int value);
    void setTrst_n(int value);
};

#endif // JTAG_VPI_H
