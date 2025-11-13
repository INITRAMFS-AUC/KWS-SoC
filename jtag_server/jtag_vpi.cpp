#include "jtag_vpi.h"
#include <cstring>
#include "logger.h"

void JtagVpi::initializeTop(VerilatedContext* contextp) {
    if (contextp == nullptr) {
        Logger::error("VerilatedContext is null");
        vl_fatal(__FILE__, __LINE__, "jtag_vpi", "");
    }
    this->top = new unique_ptr<Vexample_soc>{new Vexample_soc{contextp->get()}};
}

void JtagVpi::initializeHandles() {
    for (int i = 0; i < jtagInterfaceNames.length(); i++) {
        std::string str = jtagInterfaceNames[i];
        std::string line_name = str  +"TOP.example_soc." ;
        vpiHandle handle = vpi_handle_by_name((PLI_BYTE8*)line_name.c_str(), NULL);
        if (handle) {
            Logger::info("Found handle for jtag_vpi.cpp");
        } else {
            Logger::error("Failed to find handle for jtag_vpi.cpp");
            vl_fatal(__FILE__, __LINE__, "jtag_vpi", "");
        }

        if (str == "tdi") {
            tdiHandle = handle;
        } else if (str == "tdo") {
            tdoHandle = handle;
        } else if (str == "tms") {
            tmsHandle = handle;
        } else if (str == "tck") {
            tckHandle = handle;
        } else if (str == "trst_n") {
            trst_nHandle = handle;
        } else {
            Logger::error("Handle Signal Mismatch, check JtagVpi class config, aborting");
            exit(EXIT_FAILURE);
        }
    }
}

JtagVpi::JtagVpi(VerilatedContext* contextp) {
    initializeTop(contextp);
    initializeHandles();
}

void JtagVpi::setTdi(int value) {
}
