#include "Vexample_soc.h"
#include "verilated.h"
#include "verilated_vpi.h"  // Required to get definitions

uint64_t main_time = 0;   // See comments in first example
double sc_time_stamp() { return main_time; }

void read_and_check() {
    vpiHandle vh1_tdi = vpi_handle_by_name((PLI_BYTE8*)"TOP.example_soc.tdi", NULL);
    if (!vh1_tdi) vl_fatal(__FILE__, __LINE__, "sim_main", "No handle found");
    const char* name_tdi = vpi_get_str(vpiName, vh1_tdi);
    const char* type_tdi = vpi_get_str(vpiType, vh1_tdi);
    const int size_tdi = vpi_get(vpiSize, vh1_tdi);
    printf("register name: %s, type: %s, size: %d\n", name_tdi, type_tdi, size_tdi);  // Prints "register name: tdi, type: vpiReg, size: 32"
    vpiHandle vh1_tdo = vpi_handle_by_name((PLI_BYTE8*)"TOP.example_soc.tdo", NULL);
    if (!vh1_tdo) vl_fatal(__FILE__, __LINE__, "example_soc", "No handle found");
    const char* name_tdo = vpi_get_str(vpiName, vh1_tdo);
    const char* type_tdo = vpi_get_str(vpiType, vh1_tdo);
    const int size_tdo = vpi_get(vpiSize, vh1_tdo);
    printf("register name: %s, type: %s, size: %d\n", name_tdo, type_tdo, size_tdo);  // Prints "register name: tdi, type: vpiReg, size: 32"

    s_vpi_value v;
    v.format = vpiIntVal;
    vpi_get_value(vh1_tdi, &v);
    printf("Value of %s: %d\n", name_tdi, v.value.integer);  // Prints "Value of tdi: 0"

    vpi_get_value(vh1_tdo, &v);
    printf("Value of %s: %d\n", name_tdo, v.value.integer);  // Prints "Value of tdi: 0"
}


int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};
    const std::unique_ptr<Vexample_soc> top{new Vexample_soc{contextp.get()}};

    contextp->internalsDump();  // See scopes to help debug
    while (!contextp->gotFinish()) {
        top->eval();
        VerilatedVpi::callValueCbs();  // For signal callbacks
        read_and_check();
        break;
    }
    return 0;
}
