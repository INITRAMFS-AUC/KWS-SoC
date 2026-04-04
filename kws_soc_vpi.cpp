// kws_soc_vpi.cpp — Verilator C++ testbench for kws_soc
//
// Drop-in replacement for the Yosys CXXRTL-based kws_soc_tb.cpp.
// Implements the same CLI, the same OpenOCD remote_bitbang TCP server,
// the same UART-TX monitor, and the same I2S microphone stimulus model.
//
// Build (see Makefile target `sim-verilator`):
//   verilator -Wall -Wno-fatal --cc --trace \
//             --top-module kws_soc \
//             --Mdir build_verilator \
//             --exe kws_soc_vpi.cpp sim/i2s_mic_sim.cpp \
//             -CFLAGS "-I$(ROOT_DIR)" \        ← needed so "sim/i2s_mic_sim.h" resolves
//             $(FILE_LIST) -I$(HDL)
//   make -C build_verilator -f Vkws_soc.mk \
//        CXXFLAGS='-DCLK_MHZ=36 -DUART_BAUD_RATE=115200 -std=c++14 -O3 -I$(ROOT_DIR)' \
//        Vkws_soc
//
// NOTE: The existing Makefile sim-verilator rule needs two small tweaks:
//   1. Add  -CFLAGS "-I$(ROOT_DIR)"  to the verilator invocation so that
//      the compiler can find "sim/i2s_mic_sim.h" when building from build_verilator/.
//   2. Append  -I$(ROOT_DIR)  to the CXXFLAGS passed to the inner make.
//
// OpenOCD remote_bitbang protocol (ASCII over TCP):
//   '0'-'7'  write {tck[2], tms[1], tdi[0]}
//   'R'      read TDO, reply '0' or '1'
//   'r','s'  deassert TRST (step)
//   't','u'  assert TRST
//   'B','b'  blink LED (ignored)
//   'Q'      quit

#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <cstdio>
#include <memory>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

// I2S microphone behavioural model (shared with CXXRTL tb)
#include "sim/i2s_mic_sim.h"

// Verilator-generated model — produced by running verilator on kws_soc
#include "Vkws_soc.h"
#include "verilated.h"
#include "verilated_vcd_c.h"   // requires --trace in verilator flags

// ---------------------------------------------------------------------------
// Build-time configuration — injected via CXXFLAGS (same as CXXRTL build)
// ---------------------------------------------------------------------------
#ifndef CLK_MHZ
#error "CLK_MHZ is not defined — pass -DCLK_MHZ=<n> via CXXFLAGS (see Makefile UART_CFLAGS)"
#endif
#ifndef UART_BAUD_RATE
#error "UART_BAUD_RATE is not defined — pass -DUART_BAUD_RATE=<n> via CXXFLAGS"
#endif

// ---------------------------------------------------------------------------
// UART state-machine constants  (identical to kws_soc_tb.cpp)
// ---------------------------------------------------------------------------
#define UART_IDLE   0
#define UART_START  1
#define UART_STOP   9

// printf format for int64_t (CXXRTL tb compat)
#ifdef __x86_64__
#  define I64_FMT "%ld"
#else
#  define I64_FMT "%lld"
#endif

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------
static constexpr int TCP_BUF_SIZE = 256;

// ---------------------------------------------------------------------------
// Help text — mirrors kws_soc_tb.cpp option set
// ---------------------------------------------------------------------------
static const char *help_str =
    "Usage: kws_soc_vpi [--port n] [--vcd x.vcd] [--cycles n]\n"
    "                   [--mic x.hex] [--jtagdump x] [--jtagreplay x]\n"
    "\n"
    "    --vcd x.vcd      : Dump VCD waveforms to file (requires --trace at build)\n"
    "    --cycles n       : Maximum cycles before exit (0 = unlimited, default)\n"
    "    --port n         : TCP port for OpenOCD remote_bitbang server\n"
    "    --mic x.hex      : Hex audio file fed to the I2S microphone model\n"
    "    --jtagdump x     : Record raw remote_bitbang byte stream to file\n"
    "    --jtagreplay x   : Replay a previously recorded bitbang byte stream\n"
    "\n"
    "Exactly one of --port or --jtagreplay must be supplied.\n";

static void exit_help(const std::string &err = "") {
    std::cerr << err << help_str;
    exit(-1);
}

// ---------------------------------------------------------------------------
// TCP helpers
// ---------------------------------------------------------------------------
static int wait_for_connection(int server_fd, uint16_t port,
                                struct sockaddr *addr, socklen_t *addrlen) {
    printf("Waiting for connection on port %u\n", port);
    if (listen(server_fd, 3) < 0) { perror("listen"); exit(-1); }
    int fd = accept(server_fd, addr, addrlen);
    if (fd < 0) { perror("accept"); exit(-1); }
    printf("Connected\n");
    return fd;
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char **argv) {

    // -----------------------------------------------------------------------
    // Argument parsing
    // -----------------------------------------------------------------------
    std::string mic_hex_path;
    bool        dump_waves       = false;
    std::string waves_path;
    int64_t     max_cycles       = 0;       // 0 = unlimited
    uint16_t    port             = 0;
    bool        dump_jtag        = false;
    std::string jtag_dump_path;
    bool        replay_jtag      = false;
    std::string jtag_replay_path;

    for (int i = 1; i < argc; ++i) {
        std::string s(argv[i]);
        if (s.rfind("--", 0) != 0) {
            std::cerr << "Unexpected positional argument: " << s << "\n";
            exit_help();
        }
        else if (s == "--vcd") {
            if (argc - i < 2) exit_help("--vcd requires an argument\n");
            dump_waves = true;  waves_path = argv[++i];
        }
        else if (s == "--cycles") {
            if (argc - i < 2) exit_help("--cycles requires an argument\n");
            max_cycles = std::stol(argv[++i], nullptr, 0);
        }
        else if (s == "--port") {
            if (argc - i < 2) exit_help("--port requires an argument\n");
            port = static_cast<uint16_t>(std::stol(argv[++i], nullptr, 0));
        }
        else if (s == "--mic") {
            if (argc - i < 2) exit_help("--mic requires an argument\n");
            mic_hex_path = argv[++i];
        }
        else if (s == "--jtagdump") {
            if (argc - i < 2) exit_help("--jtagdump requires an argument\n");
            dump_jtag = true;  jtag_dump_path = argv[++i];
        }
        else if (s == "--jtagreplay") {
            if (argc - i < 2) exit_help("--jtagreplay requires an argument\n");
            replay_jtag = true;  jtag_replay_path = argv[++i];
        }
        else {
            std::cerr << "Unrecognised argument: " << s << "\n";
            exit_help();
        }
    }

    // Validate option combinations (same rules as CXXRTL tb)
    if (port == 0 && !replay_jtag)
        exit_help("At least one of --port or --jtagreplay must be specified.\n");
    if (dump_jtag && port == 0)
        exit_help("--jtagdump requires --port to also be specified.\n");
    if (replay_jtag && port != 0)
        exit_help("Cannot specify both --port and --jtagreplay.\n");

    // -----------------------------------------------------------------------
    // TCP socket — OpenOCD remote_bitbang server
    // -----------------------------------------------------------------------
    int server_fd = -1, sock_fd = -1;
    struct sockaddr_in sock_addr = {};
    int       sock_opt     = 1;
    socklen_t sock_addr_len = sizeof(sock_addr);

    char txbuf[TCP_BUF_SIZE];
    char rxbuf[TCP_BUF_SIZE];
    int  rx_ptr = 0, rx_remaining = 0, tx_ptr = 0;

    if (port != 0) {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) { perror("socket"); exit(-1); }

        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &sock_opt, sizeof(sock_opt));

        sock_addr.sin_family      = AF_INET;
        sock_addr.sin_addr.s_addr = INADDR_ANY;
        sock_addr.sin_port        = htons(port);

        if (bind(server_fd, reinterpret_cast<struct sockaddr *>(&sock_addr),
                 sizeof(sock_addr)) < 0) {
            perror("bind"); exit(-1);
        }
        sock_fd = wait_for_connection(server_fd, port,
                                       reinterpret_cast<struct sockaddr *>(&sock_addr),
                                       &sock_addr_len);
    }

    // -----------------------------------------------------------------------
    // JTAG dump / replay file handles
    // -----------------------------------------------------------------------
    std::ofstream jtag_dump_fd;
    if (dump_jtag) {
        jtag_dump_fd.open(jtag_dump_path);
        if (!jtag_dump_fd.is_open()) {
            std::cerr << "Failed to open JTAG dump file: " << jtag_dump_path << "\n";
            return -1;
        }
    }

    std::ifstream jtag_replay_fd;
    if (replay_jtag) {
        jtag_replay_fd.open(jtag_replay_path);
        if (!jtag_replay_fd.is_open()) {
            std::cerr << "Failed to open JTAG replay file: " << jtag_replay_path << "\n";
            return -1;
        }
    }

    // -----------------------------------------------------------------------
    // I2S microphone behavioural model
    // -----------------------------------------------------------------------
    I2SMicSim i2s_mic(0x00000000);
    if (!mic_hex_path.empty()) {
        printf("Loading mic audio from %s\n", mic_hex_path.c_str());
        i2s_mic.load_hex_file(mic_hex_path);
    } else {
        printf("Warning: No --mic file provided. Mic will output 0x00.\n");
    }

    // -----------------------------------------------------------------------
    // Verilator context + model instantiation
    //
    // VerilatedContext is the modern (v4.210+) per-simulation context object.
    // traceEverOn MUST be called before the first model eval() if tracing.
    // -----------------------------------------------------------------------
    std::unique_ptr<VerilatedContext> contextp(new VerilatedContext);

    // Pass only argv[0] so Verilator doesn't choke on our custom flags
    contextp->commandArgs(1, argv);

    // Enable tracing infrastructure globally before the model is constructed
    contextp->traceEverOn(dump_waves);

    std::unique_ptr<Vkws_soc> top(new Vkws_soc(contextp.get()));

    // -----------------------------------------------------------------------
    // VCD waveform writer
    // -----------------------------------------------------------------------
    std::unique_ptr<VerilatedVcdC> tfp;
    if (dump_waves) {
        tfp.reset(new VerilatedVcdC);
        top->trace(tfp.get(), /*levels=*/99);   // trace full hierarchy
        tfp->open(waves_path.c_str());
        printf("VCD dump to: %s\n", waves_path.c_str());
    }

    // -----------------------------------------------------------------------
    // Reset sequence
    //
    // Mirrors the CXXRTL tb:
    //   • All inputs driven to defined values before first eval()
    //   • UART RX held high (idle)
    //   • One clock pulse while resets asserted, then deassert
    // -----------------------------------------------------------------------
    top->clk    = 0;
    top->rst_n  = 0;
    top->tck    = 0;
    top->trst_n = 0;
    top->tms    = 0;
    top->tdi    = 0;
    top->uart_rx = 1;   // UART line is idle-high
    top->sd     = 0;
    top->eval();

    // One full clock cycle while held in reset (equivalent to CXXRTL initial pulse)
    top->clk = 1;  top->tck = 1;  top->eval();
    top->clk = 0;  top->tck = 0;

    // Release resets
    top->trst_n = 1;
    top->rst_n  = 1;
    top->eval();

    // Dump time-0 state
    if (tfp) tfp->dump(0);

    // -----------------------------------------------------------------------
    // UART RX decode state  (same algorithm as kws_soc_tb.cpp)
    // -----------------------------------------------------------------------
    int uart_state         = UART_IDLE;
    int uart_bit_timer     = 0;
    int uart_shifter       = 0;
    // Cycles per UART bit — computed from CLK_MHZ / UART_BAUD_RATE macros
    const int uart_cycles_per_bit =
        static_cast<int>((CLK_MHZ * 1000000.0) / UART_BAUD_RATE);

    // -----------------------------------------------------------------------
    // Main simulation loop
    // -----------------------------------------------------------------------
    bool timed_out = false;

    for (int64_t cycle = 0; max_cycles == 0 || cycle < max_cycles; ++cycle) {

        // -- Falling edge (negedge) -----------------------------------------
        top->clk = 0;
        top->eval();
        if (tfp) tfp->dump(static_cast<uint64_t>(cycle * 2));

        // -- Rising edge (posedge) ------------------------------------------
        top->clk = 1;
        top->eval();
        if (tfp) tfp->dump(static_cast<uint64_t>(cycle * 2 + 1));

        // ---- UART TX monitor (prints DUT console output) ------------------
        // Samples uart_tx after the rising edge, exactly like the CXXRTL tb.
        {
            bool uart_tx = static_cast<bool>(top->uart_tx);

            if (uart_state == UART_IDLE) {
                if (!uart_tx) {
                    uart_state     = UART_START;
                    // Wait 1.5 bit-periods to hit the centre of the first data bit
                    uart_bit_timer = uart_cycles_per_bit + uart_cycles_per_bit / 2;
                }
            } else {
                if (--uart_bit_timer == 0) {
                    if (uart_state >= UART_START && uart_state < UART_STOP) {
                        uart_shifter  |= (uart_tx ? 1 : 0) << (uart_state - 1);
                        uart_bit_timer = uart_cycles_per_bit;
                        uart_state++;
                    } else if (uart_state == UART_STOP) {
                        putchar(static_cast<char>(uart_shifter));
                        fflush(stdout);
                        uart_shifter = 0;
                        uart_state   = UART_IDLE;
                    }
                }
            }
        }

        // ---- I2S microphone model -----------------------------------------
        // Drive sd (serial data in) from the behavioural mic model.
        {
            bool i2s_sck = static_cast<bool>(top->sck_out);
            bool i2s_ws  = static_cast<bool>(top->ws_out);
            top->sd = static_cast<CData>(i2s_mic.step(i2s_sck, i2s_ws));
        }

        // ---- OpenOCD remote_bitbang JTAG handling -------------------------
        //
        // Protocol (ASCII bytes over TCP, same as CXXRTL tb):
        //
        //  '0'–'7'  write JTAG pins: bit2=TCK, bit1=TMS, bit0=TDI  → step simulation
        //  'R'      sample TDO, enqueue '0'/'1' reply
        //  'r','s'  deassert TRST (JTAG step)
        //  't','u'  assert TRST (no step — waits for next write)
        //  'B','b'  blink LED — ignored in simulation
        //  'Q'      OpenOCD quit — exit main loop
        //
        bool got_exit_cmd = false;
        bool step_done    = false;

        if (port != 0 || replay_jtag) {
            while (!step_done) {
                if (rx_remaining > 0) {
                    char c = rxbuf[rx_ptr++];
                    --rx_remaining;

                    if (c == 'r' || c == 's') {
                        // Deassert TRST, count as one simulation step
                        top->trst_n = 1;
                        step_done   = true;
                    }
                    else if (c == 't' || c == 'u') {
                        // Assert TRST — does not count as a step; keeps blocking
                        top->trst_n = 0;
                    }
                    else if (c >= '0' && c <= '7') {
                        // Write {TCK, TMS, TDI} and propagate
                        int mask    = c - '0';
                        top->tck    = static_cast<CData>((mask >> 2) & 1);
                        top->tms    = static_cast<CData>((mask >> 1) & 1);
                        top->tdi    = static_cast<CData>((mask >> 0) & 1);
                        // Immediately propagate new JTAG inputs through
                        // combinational logic so TDO is valid for 'R' reads.
                        top->eval();
                        step_done = true;
                    }
                    else if (c == 'R') {
                        // Sample TDO and buffer the ASCII reply
                        txbuf[tx_ptr++] = top->tdo ? '1' : '0';
                        // Flush when buffer full or at end of current receive batch
                        if (tx_ptr >= TCP_BUF_SIZE || rx_remaining == 0) {
                            send(sock_fd, txbuf, tx_ptr, 0);
                            tx_ptr = 0;
                        }
                    }
                    else if (c == 'B' || c == 'b') {
                        // Blink — no-op in simulation
                    }
                    else if (c == 'Q') {
                        printf("OpenOCD sent quit command\n");
                        got_exit_cmd = true;
                        step_done    = true;
                    }
                    // Unknown bytes are silently ignored (future-proofing)
                }
                else {
                    // Receive buffer exhausted — flush pending TDO replies first
                    if (tx_ptr > 0) {
                        send(sock_fd, txbuf, tx_ptr, 0);
                        tx_ptr = 0;
                    }
                    rx_ptr = 0;

                    if (replay_jtag) {
                        rx_remaining = static_cast<int>(
                            jtag_replay_fd.readsome(rxbuf, TCP_BUF_SIZE));
                    } else {
                        rx_remaining = static_cast<int>(
                            read(sock_fd, rxbuf, TCP_BUF_SIZE));
                    }

                    if (dump_jtag && rx_remaining > 0)
                        jtag_dump_fd.write(rxbuf, rx_remaining);

                    if (rx_remaining == 0) {
                        if (port == 0) {
                            // Replay stream exhausted — we're done
                            got_exit_cmd = true;
                            step_done    = true;
                        } else {
                            // TCP peer disconnected — wait for OpenOCD to reconnect
                            sock_fd = wait_for_connection(
                                server_fd, port,
                                reinterpret_cast<struct sockaddr *>(&sock_addr),
                                &sock_addr_len);
                        }
                    }
                }
            } // while (!step_done)
        } // if JTAG enabled

        // ---- Cycle limit check --------------------------------------------
        if (max_cycles != 0 && cycle + 1 == max_cycles) {
            printf("Max cycles (" I64_FMT ") reached\n", max_cycles);
            timed_out = true;
        }

        if (got_exit_cmd || timed_out)
            break;

    } // main simulation loop

    // -----------------------------------------------------------------------
    // Teardown
    // -----------------------------------------------------------------------
    top->final();   // flush SystemVerilog final blocks and assertions

    if (tfp) {
        tfp->close();
        tfp.reset();
    }

    top.reset();
    contextp.reset();

    if (sock_fd  >= 0) close(sock_fd);
    if (server_fd >= 0) close(server_fd);
    if (dump_jtag)   jtag_dump_fd.close();
    if (replay_jtag) jtag_replay_fd.close();

    return 0;
}