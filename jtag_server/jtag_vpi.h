#ifndef JTAG_VPI_H
#define JTAG_VPI_H

#include "verilated.h"
#include "verilated_vpi.h"
#include "Vexample_soc.h"
#include "register.h"
#include <cstdint>

const uint8_t jtagInterfaceLines = 5;
const char* jtagInterfaceNames[] = {"tdi", "tdo", "tms", "tck", "trst_n"};

//JTAG VPI wrapper
class JtagVpi {
private:
    Vexample_soc* top;
    vpiHandle vpiHandle;
    Register tdi;
    Register tdo;
    Register tms;
    Register tck;
    Register trst_n;

public:
    JtagVpi();
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
