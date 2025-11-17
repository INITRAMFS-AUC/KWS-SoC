// soc_jtag_wrapper.cpp
// Verilator testbench for Hazard3 example_soc with JTAG server integration

#include <verilated.h>
#include "Vexample_soc.h"
#include <chrono>
#include <thread>
#include "jtagServer.h"
#include <iostream>
#include <memory>

// Simulation parameters
static const uint64_t CLK_PERIOD_NS = 83;    // 12 MHz system clock (83.33ns)
static const uint64_t TCK_PERIOD_NS = 20000;   // 10 MHz JTAG clock (100ns)
static const int JTAG_PORT = 5555;           // Default jtag_vpi port
static const uint64_t RESET_CYCLES = 20;     // Number of clock cycles for reset
static const uint64_t WALL_TIME_PER_CYCLE_NS = 10000;

using namespace std::chrono;
auto start_time = steady_clock::now();
auto next_wall_time = start_time;

// Global simulation time
static uint64_t main_time = 0;

// Verilator callback for getting simulation time
double sc_time_stamp() {
    return main_time;
}

int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    // Instantiate DUT
    std::unique_ptr<Vexample_soc> dut(new Vexample_soc);

    // Create JTAG server
    VerilatorJtagServer jtag(TCK_PERIOD_NS);

    // Print banner
    std::cout << "========================================" << std::endl;
    std::cout << "Hazard3 Verilator JTAG Testbench" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "System Clock: " << (1000.0 / CLK_PERIOD_NS) << " MHz" << std::endl;
    std::cout << "JTAG Clock:   " << (1000.0 / TCK_PERIOD_NS) << " MHz" << std::endl;
    std::cout << "Initializing JTAG server on port " << JTAG_PORT << "..." << std::endl;

    // Initialize JTAG server
    if (jtag.init_jtag_server(JTAG_PORT, false) != 0) {
        std::cerr << "ERROR: Failed to initialize JTAG server" << std::endl;
        return 1;
    }

    std::cout << "========================================" << std::endl;
    std::cout << "JTAG server ready. Waiting for connection..." << std::endl;
    std::cout << "Connect with:" << std::endl;
    std::cout << "  openocd -f openocd_hazard3.cfg" << std::endl;
    std::cout << "Or use telnet:" << std::endl;
    std::cout << "  telnet localhost 4444" << std::endl;
    std::cout << "========================================" << std::endl;

    // Initialize all signals
    dut->clk = 0;
    dut->rst_n = 0;
    dut->trst_n = 0;
    dut->tck = 0;
    dut->tms = 0;
    dut->tdi = 0;
    dut->uart_rx = 1;  // UART idle state

    // Reset sequence
    std::cout << "Applying reset..." << std::endl;
    for (uint64_t i = 0; i < RESET_CYCLES * 2; i++) {
        dut->clk = !dut->clk;
        dut->eval();
        main_time += CLK_PERIOD_NS / 2;
    }

    // Release reset
    dut->rst_n = 1;
    dut->trst_n = 1;
    std::cout << "Reset released. System running." << std::endl;

    // Track previous UART state for basic monitoring
    uint8_t uart_prev = 1;
    uint64_t cycle_count = 0;

    // Main simulation loop
    while (!Verilated::gotFinish() && !jtag.stop_simu) {

        // --- Realtime Control Logic ---
        // Calculate the wall clock time corresponding to the next simulation cycle
        next_wall_time += nanoseconds(WALL_TIME_PER_CYCLE_NS / 2); // Half cycle

        // Pause until the target wall clock time is reached
        std::this_thread::sleep_until(next_wall_time);

        // Toggle system clock
        dut->clk = !dut->clk;

        // On positive clock edge
        if (dut->clk) {
            cycle_count++;

            // Handle JTAG operations
            int result = jtag.doJTAG(
                main_time,
                &dut->tms,
                &dut->tdi,
                &dut->tck,
                dut->tdo
            );

            // Check for JTAG errors
            if (result == 1) {
                std::cout << "\nJTAG client disconnected. Simulation ending." << std::endl;
                break;
            } else if (result == 1) {
                std::cerr << "\nERROR: JTAG communication error" << std::endl;
                break;
            }

            // Simple UART monitoring (detect start bits)
            if (uart_prev == 1 && dut->uart_tx == 0) {
                // UART start bit detected - you could add full UART decoder here
                // For now, just note that there's activity
                std::cout << "[" << cycle_count << "] UART activity detected" << std::endl;
            }
            uart_prev = dut->uart_tx;

            // Periodic status output (every 1 million cycles = ~83ms)
            if (cycle_count % 1000000 == 0) {
                std::cout << "Simulation running... "
                          << (main_time / 1000) << " us elapsed" << std::endl;
            }
        }

        // Evaluate DUT
        dut->eval();

        // Advance time
        main_time += CLK_PERIOD_NS / 2;
    }

    // Simulation complete
    std::cout << "\n========================================" << std::endl;
    std::cout << "Simulation ended" << std::endl;
    std::cout << "Total cycles: " << cycle_count << std::endl;
    std::cout << "Simulation time: " << (main_time / 1000) << " us" << std::endl;
    std::cout << "========================================" << std::endl;

    // Cleanup
    dut->final();

    return 0;
}
