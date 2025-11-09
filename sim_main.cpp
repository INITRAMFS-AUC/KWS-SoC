  #include "Vour.h"
  #include "verilated.h"
  #include "verilated_vpi.h"  // Required to get definitions

  uint64_t main_time = 0;   // See comments in first example
  double sc_time_stamp() { return main_time; }

  void read_and_check() {
      vpiHandle vh1 = vpi_handle_by_name((PLI_BYTE8*)"TOP.our.readme", NULL);
      if (!vh1) vl_fatal(__FILE__, __LINE__, "sim_main", "No handle found");
      const char* name = vpi_get_str(vpiName, vh1);
      const char* type = vpi_get_str(vpiType, vh1);
      const int size = vpi_get(vpiSize, vh1);
      printf("register name: %s, type: %s, size: %d\n", name, type, size);  // Prints "register name: readme, type: vpiReg, size: 32"

      s_vpi_value v;
      v.format = vpiIntVal;
      vpi_get_value(vh1, &v);
      printf("Value of %s: %d\n", name, v.value.integer);  // Prints "Value of readme: 0"
  }

  int main(int argc, char** argv) {
      Verilated::commandArgs(argc, argv);
      const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};
      const std::unique_ptr<Vour> top{new Vour{contextp.get()}};

      contextp->internalsDump();  // See scopes to help debug
      while (!contextp->gotFinish()) {
          top->eval();
          VerilatedVpi::callValueCbs();  // For signal callbacks
          read_and_check();
      }
      return 0;
  }
