
#ifndef REGISTER_H
#define REGISTER_H

#include "verilated_vpi.h"
#include "verilated.h"
#include <cstdint>

struct Register {
    char* name;
    char* type;
    uint32_t size_tdi = 0;
    s_vpi_value v;
};

#endif
