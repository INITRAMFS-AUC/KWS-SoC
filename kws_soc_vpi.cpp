// kws_soc_vpi.cpp — Verilator C++ testbench for kws_soc
//
// Drop-in CLI replacement for the Yosys CXXRTL-based kws_soc_tb.cpp.
// Implements the same OpenOCD remote_bitbang TCP server, UART-TX monitor,
// and I2S microphone stimulus model.
//
// Build (see Makefile target `sim-verilator`):
//   verilator -Wall -Wno-fatal --cc --trace-fst --x-initial 0 \
//             --top-module kws_soc \
//             --Mdir build_verilator \
//             --exe kws_soc_vpi.cpp sim/i2s_mic_sim.cpp \
//             -CFLAGS "-I$(ROOT_DIR) -march=native" \
//             $(FILE_LIST) -I$(HDL)
//   make -C build_verilator -j -f Vkws_soc.mk \
//        CXXFLAGS='$(UART_CFLAGS) -std=c++14 -O3 -march=native -I$(ROOT_DIR)' \
//        Vkws_soc
//
//   For VCD instead of FST: TRACE_FORMAT=VCD make sim_verilator
//
// OpenOCD remote_bitbang protocol (ASCII over TCP):
//   '0'-'7'  write {tck[2], tms[1], tdi[0]}
//   'R'      read TDO, send back '0' or '1'
//   'r','s'  deassert TRST
//   't','u'  assert TRST
//   'B','b'  blink LED (ignored)
//   'Q'      quit
//
// ============================================================================
// ARCHITECTURE — JTAG TIMING AND THE CDC CONSTRAINT
// ============================================================================
//
// Hazard3's JTAG-DTM lives in the TCK clock domain.  It transfers data into
// the system-clock (clk) domain through 2-stage CDC synchronisers.  For those
// synchronisers to correctly capture every TCK transition, the system clock
// must have had at least one (ideally two or more) edges between consecutive
// TCK half-edges.  In the original CXXRTL lockstep testbench this was
// guaranteed by structure:
//
//   for each clock cycle:                  ← one system clock edge
//       blocking_recv_one_jtag_byte();     ← one TCK half-edge at most
//
// THE SPEEDUP IS PRESERVED:
//   Between JTAG commands (firmware executing, OpenOCD idle), recv() returns
//   EAGAIN immediately and drain_jtag() returns in nanoseconds.  The system
//   clock runs at full Verilator speed — tens to hundreds of millions of
//   cycles per second — during those idle periods.
//   TCP_NODELAY and the 4 KiB buffer remain in place and still reduce the
//   per-command latency when OpenOCD IS active.
// ============================================================================

#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <cstdio>
#include <memory>
#include <cerrno>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>        // TCP_NODELAY

#include "sim/i2s_mic_sim.h"

#include "Vkws_soc.h"
#include "verilated.h"

// ---------------------------------------------------------------------------
// Trace format selection
//
// Default: FST (Fast Signal Trace) — ~10-50x smaller files than VCD, faster
// disk I/O.  Override by defining TRACE_VCD at compile time (e.g.
// -DTRACE_VCD via CXXFLAGS) to fall back to classic VCD.
//
// The Makefile controls this via the TRACE_FORMAT env variable:
//   TRACE_FORMAT=VCD  make sim_verilator   → uses VCD
//   make sim_verilator                     → uses FST (default)
// ---------------------------------------------------------------------------
#ifdef TRACE_VCD
#  include "verilated_vcd_c.h"
   typedef VerilatedVcdC TraceFile;
#  define TRACE_EXT "vcd"
#else
#  include "verilated_fst_c.h"
   typedef VerilatedFstC TraceFile;
#  define TRACE_EXT "fst"
#endif

// ---------------------------------------------------------------------------
// Build-time guards
// ---------------------------------------------------------------------------
#ifndef CLK_MHZ
#error "CLK_MHZ not defined — pass -DCLK_MHZ=<n> via CXXFLAGS"
#endif
#ifndef UART_BAUD_RATE
#error "UART_BAUD_RATE not defined — pass -DUART_BAUD_RATE=<n> via CXXFLAGS"
#endif

// ---------------------------------------------------------------------------
// UART state constants
// ---------------------------------------------------------------------------
#define UART_IDLE   0
#define UART_START  1
#define UART_STOP   9

#ifdef __x86_64__
#  define I64_FMT "%ld"
#else
#  define I64_FMT "%lld"
#endif

// ---------------------------------------------------------------------------
// Buffer sizes
// ---------------------------------------------------------------------------
// 4 KiB: keeps recv() syscall count low across OpenOCD command batches.
// Only ONE write command per drain_jtag() call is consumed (see above).
static constexpr int JTAG_RX_BUF = 4096;
static constexpr int JTAG_TX_BUF = 4096;

// ---------------------------------------------------------------------------
// Help
// ---------------------------------------------------------------------------
static const char *help_str =
    "Usage: kws_soc_vpi [--port n] [--waves x] [--vcd x.vcd] [--cycles n]\n"
    "                   [--mic x.hex] [--jtagdump x] [--jtagreplay x]\n"
    "                   [--vcd-sample-rate n]\n"
    "\n"
    "    --port n             : TCP port for OpenOCD remote_bitbang server\n"
    "    --waves x            : Dump waveforms to file (format depends on build:\n"
    "                           FST by default, VCD if compiled with -DTRACE_VCD)\n"
    "    --vcd x.vcd          : Alias for --waves (kept for backward compat)\n"
    "    --vcd-sample-rate n  : Dump waveform every n cycles (default 1).\n"
    "                           n=10 or 100 dramatically cuts waveform I/O.\n"
    "                           Down-sampled waveform dump — records one snapshot every 10 clock cycles.\n "
    "                           Reduces file size and disk I/O by ~10x with negligible loss for\n "
    "                           functional debugging.  Increase --vcd-sample-rate further if even less\n "
    "                           resolution is acceptable.\n "
    "    --cycles n           : Exit after n simulated clock cycles (0 = unlimited)\n"
    "    --mic x.hex          : Audio hex file for the I2S microphone model\n"
    "    --jtagdump x         : Record raw remote_bitbang byte stream to file\n"
    "    --jtagreplay x       : Replay a recorded bitbang byte stream\n"
    "\n"
    "Trace format: " TRACE_EXT " (compile with -DTRACE_VCD to use VCD instead)\n"
    "\n"

    "Exactly one of --port or --jtagreplay must be supplied.\n"
    " ________________________________________________________\n\n "
    "Example\n "
    " kws_soc_vpi --port 9824 --waves waves." TRACE_EXT "\n ";

static void exit_help(const std::string &err = "") {
    std::cerr << err << help_str;
    exit(-1);
}

// ---------------------------------------------------------------------------
// Socket helpers
// ---------------------------------------------------------------------------
static void set_nonblocking(int fd) {
    int fl = fcntl(fd, F_GETFL, 0);
    if (fl < 0 || fcntl(fd, F_SETFL, fl | O_NONBLOCK) < 0) {
        perror("fcntl O_NONBLOCK");
        exit(-1);
    }
}

static void set_tcp_nodelay(int fd) {
    int one = 1;
    // Disable Nagle: TDO reply bytes must reach OpenOCD immediately so it can
    // issue the next JTAG command without waiting for a coalesce timeout.
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
}

static int wait_for_connection(int server_fd, uint16_t port,
                               struct sockaddr_in *addr, socklen_t *addrlen) {
    printf("Waiting for connection on port %u\n", port);
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(-1);
    }
    int fd = accept(server_fd, reinterpret_cast<struct sockaddr *>(addr), addrlen);
    if (fd < 0) {
        perror("accept");
        exit(-1);
    }
    set_nonblocking(fd);    // never sleep in recv() — the key speed change
    set_tcp_nodelay(fd);    // send TDO replies immediately
    printf("Connected\n");
    return fd;
}

// ---------------------------------------------------------------------------
// JTAG socket state bundle
// ---------------------------------------------------------------------------
struct JtagCtx {
    int  server_fd    = -1;
    int  sock_fd      = -1;
    uint16_t port     = 0;

    char rxbuf[JTAG_RX_BUF];
    char txbuf[JTAG_TX_BUF];
    int  rx_ptr       = 0;
    int  rx_remaining = 0;
    int  tx_ptr       = 0;

    struct sockaddr_in addr = {};
    socklen_t          addrlen = sizeof(addr);

    std::ofstream dump_fd;
    std::ifstream replay_fd;
    bool dump_jtag    = false;
    bool replay_jtag  = false;
};

static void jtag_flush_tx(JtagCtx &jc) {
    if (jc.tx_ptr > 0) {
        send(jc.sock_fd, jc.txbuf, jc.tx_ptr, 0);
        jc.tx_ptr = 0;
    }
}

// ---------------------------------------------------------------------------
// drain_jtag() — called once per simulated clock cycle
//
// Non-blocking receive: returns immediately if no bytes are ready.
//
// CDC CONSTRAINT ENFORCEMENT:
//   Processes at most ONE pin-write command per call ('0'-'7', 'r'/'s', 't'/'u').
//   'R' (TDO read), 'B'/'b' (blink), and 'Q' (quit) are consumed without
//   limit in the same pass because they carry no pin-state and do not advance
//   the JTAG state machine.
//
//   After the single pin-write the function returns, allowing the main loop
//   to advance the system clock once before the next TCK half-edge.  This
//   replicates the original 1:1 TCK:clk relationship that the CDC
//   synchronisers inside Hazard3's JTAG-DTM depend on.
//
// Returns true when OpenOCD sends 'Q' or the replay stream is exhausted.
// ---------------------------------------------------------------------------
static bool drain_jtag(JtagCtx &jc, Vkws_soc *top) {

    // -- Refill rx buffer if empty ------------------------------------------
    if (jc.rx_remaining == 0) {
        jc.rx_ptr = 0;
        int n;

        if (jc.replay_jtag) {
            n = static_cast<int>(jc.replay_fd.readsome(jc.rxbuf, JTAG_RX_BUF));
            if (n == 0) return true;    // stream exhausted
        } else {
            n = static_cast<int>(
                    recv(jc.sock_fd, jc.rxbuf, JTAG_RX_BUF, MSG_DONTWAIT));

            if (n == 0) {
                // TCP peer closed — wait for OpenOCD to reconnect
                close(jc.sock_fd);
                jc.sock_fd = wait_for_connection(
                                 jc.server_fd, jc.port, &jc.addr, &jc.addrlen);
                return false;
            } else if (n < 0) {
                // EAGAIN / EWOULDBLOCK — nothing ready this cycle
                return false;
            }
        }

        jc.rx_remaining = n;
        if (jc.dump_jtag)
            jc.dump_fd.write(jc.rxbuf, n);
    }

    // -- Process bytes, honouring the CDC constraint ------------------------
    //
    // We consume 'R', 'B', 'b' freely.  The moment we hit a pin-write command
    // ('0'-'7', 'r', 's', 't', 'u') we execute it and immediately return so
    // that the main loop can tick the system clock before we come back to
    // consume the next pin-write.

    while (jc.rx_remaining > 0) {
        const char c = jc.rxbuf[jc.rx_ptr++];
        --jc.rx_remaining;

        if (c >= '0' && c <= '7') {
            // Pin-write: update TCK/TMS/TDI, eval combinational logic so TDO
            // is valid for any 'R' that arrives on the next drain_jtag() call.
            const int mask = c - '0';
            top->tck = static_cast<CData>((mask >> 2) & 1);
            top->tms = static_cast<CData>((mask >> 1) & 1);
            top->tdi = static_cast<CData>((mask >> 0) & 1);
            top->eval();

            // CDC CONSTRAINT: one pin-write consumed — return now.
            // The main loop will clock the system once before we are called again.
            jtag_flush_tx(jc);
            return false;
        }
        else if (c == 'r' || c == 's') {
            top->trst_n = 1;
            jtag_flush_tx(jc);
            return false;   // pin-write — honour the CDC constraint
        }
        else if (c == 't' || c == 'u') {
            top->trst_n = 0;
            jtag_flush_tx(jc);
            return false;   // pin-write — honour the CDC constraint
        }
        else if (c == 'R') {
            // TDO sample: no pin state changed, no JTAG clock — free to batch
            jc.txbuf[jc.tx_ptr++] = top->tdo ? '1' : '0';
            if (jc.tx_ptr >= JTAG_TX_BUF)
                jtag_flush_tx(jc);
        }
        else if (c == 'B' || c == 'b') {
            // Blink — no-op, no pin state changed
        }
        else if (c == 'Q') {
            printf("OpenOCD sent quit command\n");
            jtag_flush_tx(jc);
            return true;    // signal main loop to exit
        }
        // Unknown bytes silently ignored for forward compatibility
    }

    jtag_flush_tx(jc);
    return false;
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char **argv) {

    // -----------------------------------------------------------------------
    // Argument parsing
    // -----------------------------------------------------------------------
    std::string mic_hex_path;
    bool        dump_waves      = false;
    std::string waves_path;
    int64_t     max_cycles      = 0;
    int         vcd_sample_rate = 1;

    JtagCtx jc;

    for (int i = 1; i < argc; ++i) {
        const std::string s(argv[i]);
        if (s.rfind("--", 0) != 0) {
            std::cerr << "Unexpected positional argument: " << s << "\n";
            exit_help();
        }
        else if (s == "--vcd" || s == "--waves") {
            if (argc - i < 2) exit_help(s + " requires an argument\n");
            dump_waves = true;
            waves_path = argv[++i];
        }
        else if (s == "--vcd-sample-rate") {
            if (argc - i < 2) exit_help("--vcd-sample-rate requires an argument\n");
            vcd_sample_rate = std::stoi(argv[++i]);
            if (vcd_sample_rate < 1) exit_help("--vcd-sample-rate must be >= 1\n");
        }
        else if (s == "--cycles") {
            if (argc - i < 2) exit_help("--cycles requires an argument\n");
            max_cycles = std::stol(argv[++i], nullptr, 0);
        }
        else if (s == "--port") {
            if (argc - i < 2) exit_help("--port requires an argument\n");
            jc.port = static_cast<uint16_t>(std::stol(argv[++i], nullptr, 0));
        }
        else if (s == "--mic") {
            if (argc - i < 2) exit_help("--mic requires an argument\n");
            mic_hex_path = argv[++i];
        }
        else if (s == "--jtagdump") {
            if (argc - i < 2) exit_help("--jtagdump requires an argument\n");
            jc.dump_jtag = true;
            jc.dump_fd.open(argv[++i]);
            if (!jc.dump_fd.is_open()) {
                std::cerr << "Failed to open JTAG dump file\n";
                return -1;
            }
        }
        else if (s == "--jtagreplay") {
            if (argc - i < 2) exit_help("--jtagreplay requires an argument\n");
            jc.replay_jtag = true;
            jc.replay_fd.open(argv[++i]);
            if (!jc.replay_fd.is_open()) {
                std::cerr << "Failed to open JTAG replay file\n";
                return -1;
            }
        }
        else {
            std::cerr << "Unrecognised argument: " << s << "\n";
            exit_help();
        }
    }

    if (jc.port == 0 && !jc.replay_jtag)
        exit_help("At least one of --port or --jtagreplay must be specified.\n");
    if (jc.dump_jtag && jc.port == 0)
        exit_help("--jtagdump requires --port.\n");
    if (jc.replay_jtag && jc.port != 0)
        exit_help("Cannot specify both --port and --jtagreplay.\n");

    // -----------------------------------------------------------------------
    // TCP server
    // -----------------------------------------------------------------------
    if (jc.port != 0) {
        int opt = 1;
        jc.server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (jc.server_fd < 0) {
            perror("socket");
            exit(-1);
        }
        setsockopt(jc.server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt));

        jc.addr.sin_family      = AF_INET;
        jc.addr.sin_addr.s_addr = INADDR_ANY;
        jc.addr.sin_port        = htons(jc.port);

        if (bind(jc.server_fd,
                 reinterpret_cast<struct sockaddr *>(&jc.addr),
                 sizeof(jc.addr)) < 0) {
            perror("bind");
            exit(-1);
        }
        jc.sock_fd = wait_for_connection(
                         jc.server_fd, jc.port, &jc.addr, &jc.addrlen);
    }

    // -----------------------------------------------------------------------
    // I2S microphone model
    // -----------------------------------------------------------------------
    I2SMicSim i2s_mic(0x00000000);
    if (!mic_hex_path.empty()) {
        printf("Loading mic audio from %s\n", mic_hex_path.c_str());
        i2s_mic.load_hex_file(mic_hex_path);
    } else {
        printf("Warning: No --mic file provided. Mic will output 0x00.\n");
    }

    // -----------------------------------------------------------------------
    // Verilator context + model
    // traceEverOn() must precede the first eval().
    // -----------------------------------------------------------------------
    std::unique_ptr<VerilatedContext> contextp(new VerilatedContext);
    contextp->commandArgs(1, argv);
    contextp->traceEverOn(dump_waves);

    std::unique_ptr<Vkws_soc> top(new Vkws_soc(contextp.get()));

    // -----------------------------------------------------------------------
    // Waveform trace (FST default, VCD if compiled with -DTRACE_VCD)
    // -----------------------------------------------------------------------
    std::unique_ptr<TraceFile> tfp;
    if (dump_waves) {
        tfp.reset(new TraceFile);
        top->trace(tfp.get(), /*depth=*/99);
        tfp->open(waves_path.c_str());
        printf("Trace [" TRACE_EXT "]: %s  (sample every %d cycle(s))\n",
               waves_path.c_str(), vcd_sample_rate);
    }

    // -----------------------------------------------------------------------
    // Reset sequence  (mirrors CXXRTL tb)
    // -----------------------------------------------------------------------
    top->clk     = 0;
    top->rst_n   = 0;
    top->tck     = 0;
    top->trst_n  = 0;
    top->tms     = 0;
    top->tdi     = 0;
    top->uart_rx = 1;   // UART idle-high
    top->sd      = 0;
    top->eval();

    top->clk = 1;
    top->tck = 1;
    top->eval();
    top->clk = 0;
    top->tck = 0;
    top->trst_n = 1;
    top->rst_n  = 1;
    top->eval();

    if (tfp) tfp->dump(0);

    // -----------------------------------------------------------------------
    // UART decode state
    // -----------------------------------------------------------------------
    int uart_state         = UART_IDLE;
    int uart_bit_timer     = 0;
    int uart_shifter       = 0;
    const int uart_cycles_per_bit =
        static_cast<int>((CLK_MHZ * 1000000.0) / UART_BAUD_RATE);

    // -----------------------------------------------------------------------
    // Main simulation loop
    //
    // The system clock runs freely at full Verilator speed.  drain_jtag() is
    // non-blocking and returns in nanoseconds when OpenOCD is idle, but
    // enforces the 1 TCK half-edge : 1 clk cycle CDC constraint by consuming
    // at most one pin-write per call when OpenOCD IS sending commands.
    // -----------------------------------------------------------------------
    bool quit = false;

    for (int64_t cycle = 0;
            !quit && (max_cycles == 0 || cycle < max_cycles);
            ++cycle) {

        // ---- Falling edge --------------------------------------------------
        top->clk = 0;
        top->eval();
        if (tfp && (cycle % vcd_sample_rate == 0))
            tfp->dump(static_cast<uint64_t>(cycle * 2));

        // ---- Rising edge ---------------------------------------------------
        top->clk = 1;
        top->eval();
        if (tfp && (cycle % vcd_sample_rate == 0))
            tfp->dump(static_cast<uint64_t>(cycle * 2 + 1));

        // ---- UART TX decode ------------------------------------------------
        {
            const bool uart_tx = static_cast<bool>(top->uart_tx);
            if (uart_state == UART_IDLE) {
                if (!uart_tx) {
                    uart_state     = UART_START;
                    uart_bit_timer = uart_cycles_per_bit
                                     + uart_cycles_per_bit / 2;
                }
            } else if (--uart_bit_timer == 0) {
                if (uart_state >= UART_START && uart_state < UART_STOP) {
                    uart_shifter  |= (uart_tx ? 1 : 0) << (uart_state - 1);
                    uart_bit_timer = uart_cycles_per_bit;
                    ++uart_state;
                } else if (uart_state == UART_STOP) {
                    const char ch = static_cast<char>(uart_shifter);
                    putchar(ch);
                    // Flush on newline — avoids a syscall per character
                    if (ch == '\n') fflush(stdout);
                    uart_shifter = 0;
                    uart_state   = UART_IDLE;
                }
            }
        }

        // ---- I2S microphone ------------------------------------------------
        top->sd = static_cast<CData>(
                      i2s_mic.step(static_cast<bool>(top->sck_out),
                                   static_cast<bool>(top->ws_out)));

        // ---- JTAG (non-blocking, CDC-safe) ---------------------------------
        if (jc.port != 0 || jc.replay_jtag)
            quit = drain_jtag(jc, top.get());

        if (!quit && max_cycles != 0 && cycle + 1 == max_cycles) {
            printf("Max cycles (" I64_FMT ") reached\n", max_cycles);
            quit = true;
        }
    }

    // -----------------------------------------------------------------------
    // Teardown
    // -----------------------------------------------------------------------
    fflush(stdout);
    top->final();

    if (tfp) {
        tfp->close();
        tfp.reset();
    }
    top.reset();
    contextp.reset();

    if (jc.sock_fd   >= 0) close(jc.sock_fd);
    if (jc.server_fd >= 0) close(jc.server_fd);
    if (jc.dump_jtag)   jc.dump_fd.close();
    if (jc.replay_jtag) jc.replay_fd.close();

    return 0;
}
