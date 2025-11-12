#include "jtag_vpi.h"
#include <cstring>
#include <string>
#include <iostream>

// initialize vpi handles
void JtagVpi::JtagVpi() {
    for (int i =0; i < jtagInterfaceLines; i++) {
        char buf[8] = jtagInterfaceNames[i];
        std::string line_name = strcat(buf, "TOP.example_soc.");
        vpiHandle handle = vpi_handle_by_name((PLI_BYTE8*)line_name.c_str(), NULL);
        if (handle) {
            std::cout << ("Found handle for jtag_vpi.cpp\n");
        } else {
            vpi_printf("Failed to find handle for jtag_vpi.cpp\n");
        }
    }

}

void JtagVpi::setTdi(int value) {
    tdi.v = value;
}


for (int i =0; i < jtagInterfaceLines; i++) {
