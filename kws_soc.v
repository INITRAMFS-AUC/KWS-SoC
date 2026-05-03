/*****************************************************************************\
|                      Copyright (C) 2021-2022 Luke Wren                      |
|                     SPDX-License-Identifier: Apache-2.0                     |
\*****************************************************************************/

// Example file integrating a Hazard3 processor, processor JTAG + debug
// components, some memory and a UART.
`include "kws_soc_config.vh"

module kws_soc #(
    parameter DTM_TYPE   = `DTM_TYPE,
    parameter SRAM_DEPTH = `SRAM_DEPTH,
    parameter CLK_MHZ    = `CLK_MHZ, // For timer timebase

    `include "hazard3_config.vh"
) (
    // System clock + reset
    input wire clk  /*verilator public_flat_rw*/,
    input wire rst_n  /*verilator public_flat_rw*/,

    // JTAG port to RISC-V JTAG-DTM
    input  wire tck  /*verilator public_flat_rw*/,
    input  wire trst_n  /*verilator public_flat_rw*/,
    input  wire tms  /*verilator public_flat_rw*/,
    input  wire tdi  /*verilator public_flat_rw*/,
    output wire tdo  /*verilator public_flat_rd*/,

    // UART
    output wire              uart_tx  /*verilator public_flat_rd*/,
    input  wire              uart_rx  /*verilator public_flat_rw*/,

    // I2S
    input  wire              i2s_sd      /*verilator public_flat_rw*/,
    output wire              i2s_sck_out /*verilator public_flat_rd*/,
    output wire              i2s_ws_out  /*verilator public_flat_rd*/,

    // XIP QSPI Flash
    output wire              xip_csn,
    output wire              xip_sck,
    output wire [3:0]        xip_doe,
    output wire [3:0]        xip_do,
    input  wire [3:0]        flash_di
);
  // ----------------------------------------------------------------------------
  // Processor debug

  wire        dmi_psel;
  wire        dmi_penable;
  wire        dmi_pwrite;
  wire [ 8:0] dmi_paddr;
  wire [31:0] dmi_pwdata;
  wire [31:0] dmi_prdata;
  wire        dmi_pready;
  wire        dmi_pslverr;


  // TCK-domain DTM logic can force a hard reset
  wire        dmihardreset_req;
  wire        assert_dmi_reset = !rst_n || dmihardreset_req;
  wire        rst_n_dmi;

  reset_sync dmi_reset_sync_u (
      .clk      (clk),
      .rst_n_in (!assert_dmi_reset),
      .rst_n_out(rst_n_dmi)
  );

  generate
    if (DTM_TYPE == "JTAG") begin

      // Standard RISC-V JTAG-DTM connected to external IOs.
      // JTAG-DTM IDCODE should be a JEP106-compliant ID:
      localparam IDCODE = 32'hdeadbeef;

      hazard3_jtag_dtm #(
          .IDCODE(IDCODE)
      ) dtm_u (
          .tck   (tck),
          .trst_n(trst_n),
          .tms   (tms),
          .tdi   (tdi),
          .tdo   (tdo),

          .dmihardreset_req(dmihardreset_req),

          .clk_dmi  (clk),
          .rst_n_dmi(rst_n_dmi),

          .dmi_psel   (dmi_psel),
          .dmi_penable(dmi_penable),
          .dmi_pwrite (dmi_pwrite),
          .dmi_paddr  (dmi_paddr),
          .dmi_pwdata (dmi_pwdata),
          .dmi_prdata (dmi_prdata),
          .dmi_pready (dmi_pready),
          .dmi_pslverr(dmi_pslverr)
      );

    end else if (DTM_TYPE == "ECP5") begin

      // Attach RISC-V DTM's DTMCS/DMI registers to ECP5 ER1/ER2 registers. This
      // allows the processor to be debugged through the ECP5 chip TAP, using
      // regular upstream OpenOCD.

      // Connects to ECP5 TAP internally by instantiating a JTAGG primitive.
      assign tdo = 1'b0;

      hazard3_ecp5_jtag_dtm dtm_u (
          .dmihardreset_req(dmihardreset_req),

          .clk_dmi  (clk),
          .rst_n_dmi(rst_n_dmi),

          .dmi_psel   (dmi_psel),
          .dmi_penable(dmi_penable),
          .dmi_pwrite (dmi_pwrite),
          .dmi_paddr  (dmi_paddr),
          .dmi_pwdata (dmi_pwdata),
          .dmi_prdata (dmi_prdata),
          .dmi_pready (dmi_pready),
          .dmi_pslverr(dmi_pslverr)
      );

    end
  endgenerate


  localparam N_HARTS = 1;
  localparam XLEN = 32;

  wire                    sys_reset_req;
  wire                    sys_reset_done;
  wire [     N_HARTS-1:0] hart_reset_req;
  wire [     N_HARTS-1:0] hart_reset_done;

  wire [     N_HARTS-1:0] hart_req_halt;
  wire [     N_HARTS-1:0] hart_req_halt_on_reset;
  wire [     N_HARTS-1:0] hart_req_resume;
  wire [     N_HARTS-1:0] hart_halted;
  wire [     N_HARTS-1:0] hart_running;

  wire [N_HARTS*XLEN-1:0] hart_data0_rdata;
  wire [N_HARTS*XLEN-1:0] hart_data0_wdata;
  wire [     N_HARTS-1:0] hart_data0_wen;

  wire [N_HARTS*XLEN-1:0] hart_instr_data;
  wire [     N_HARTS-1:0] hart_instr_data_vld;
  wire [     N_HARTS-1:0] hart_instr_data_rdy;
  wire [     N_HARTS-1:0] hart_instr_caught_exception;
  wire [     N_HARTS-1:0] hart_instr_caught_ebreak;

  wire [            31:0] sbus_addr;
  wire                    sbus_write;
  wire [             1:0] sbus_size;
  wire                    sbus_vld;
  wire                    sbus_rdy;
  wire                    sbus_err;
  wire [            31:0] sbus_wdata;
  wire [            31:0] sbus_rdata;

  hazard3_dm #(
      .N_HARTS     (N_HARTS),
      .HAVE_SBA    (1),        // System bus access, enable it for debugging
      .NEXT_DM_ADDR(0)
  ) dm (
      .clk  (clk),
      .rst_n(rst_n),

      .dmi_psel                   (dmi_psel),
      .dmi_penable                (dmi_penable),
      .dmi_pwrite                 (dmi_pwrite),
      .dmi_paddr                  (dmi_paddr),
      .dmi_pwdata                 (dmi_pwdata),
      .dmi_prdata                 (dmi_prdata),
      .dmi_pready                 (dmi_pready),
      .dmi_pslverr                (dmi_pslverr),

      .sys_reset_req              (sys_reset_req),
      .sys_reset_done             (sys_reset_done),
      .hart_reset_req             (hart_reset_req),
      .hart_reset_done            (hart_reset_done),

      .hart_req_halt              (hart_req_halt),
      .hart_req_halt_on_reset     (hart_req_halt_on_reset),
      .hart_req_resume            (hart_req_resume),
      .hart_halted                (hart_halted),
      .hart_running               (hart_running),

      .hart_data0_rdata           (hart_data0_rdata),
      .hart_data0_wdata           (hart_data0_wdata),
      .hart_data0_wen             (hart_data0_wen),

      .hart_instr_data            (hart_instr_data),
      .hart_instr_data_vld        (hart_instr_data_vld),
      .hart_instr_data_rdy        (hart_instr_data_rdy),
      .hart_instr_caught_exception(hart_instr_caught_exception),
      .hart_instr_caught_ebreak   (hart_instr_caught_ebreak),

      .sbus_addr                  (sbus_addr),
      .sbus_write                 (sbus_write),
      .sbus_size                  (sbus_size),
      .sbus_vld                   (sbus_vld),
      .sbus_rdy                   (sbus_rdy),
      .sbus_err                   (sbus_err),
      .sbus_wdata                 (sbus_wdata),
      .sbus_rdata                 (sbus_rdata)
  );


  // Generate resynchronised reset for CPU based on upstream system reset and on
  // system/hart reset requests from DM.

  wire assert_cpu_reset = !rst_n || sys_reset_req || hart_reset_req[0];
  wire rst_n_cpu;

  reset_sync cpu_reset_sync (
      .clk      (clk),
      .rst_n_in (!assert_cpu_reset),
      .rst_n_out(rst_n_cpu)
  );

  // Still some work to be done on the reset handshake -- this ought to be
  // resynchronised to DM's reset domain here, and the DM should wait for a
  // rising edge after it has asserted the reset pulse, to make sure the tail
  // of the previous "done" is not passed on.
  assign sys_reset_done  = rst_n_cpu;
  assign hart_reset_done = rst_n_cpu;

  // ----------------------------------------------------------------------------
  // Processor

  // Instruction-fetch port (i_*)
  wire [W_ADDR-1:0] i_haddr;
  wire              i_hwrite;
  wire [       1:0] i_htrans;
  wire [       2:0] i_hsize;
  wire [       2:0] i_hburst;
  wire [       3:0] i_hprot;
  wire              i_hmastlock;
  wire [       7:0] i_hmaster;
  wire              i_hready;
  wire              i_hresp;
  wire              i_hexcl = 1'b0;    // i-port never does exclusive
  wire [W_DATA-1:0] i_hwdata;
  wire [W_DATA-1:0] i_hrdata;

  // Load/store port (d_*)
  wire [W_ADDR-1:0] d_haddr;
  wire              d_hwrite;
  wire [       1:0] d_htrans;
  wire [       2:0] d_hsize;
  wire [       2:0] d_hburst;
  wire [       3:0] d_hprot;
  wire              d_hmastlock;
  wire [       7:0] d_hmaster;
  wire              d_hexcl;     // CPU output, fed into crossbar src_hexcl
  wire              d_hready;
  wire              d_hresp;
  wire              d_hexokay;   // driven by crossbar src_hexokay[1] (d-port slot)
  wire [W_DATA-1:0] d_hwdata;
  wire [W_DATA-1:0] d_hrdata;

  // Crossbar AHB5 master-side outputs (we don't have a real exclusive monitor;
  // the splitter/arbiter return hexokay=1 by default when no slave drives it).
`ifdef XIP_PLAYBACK
  wire [4:0]        src_hexokay_xbar;  // {playback, accel, dmac, d-port, i-port}
`else
  wire [3:0]        src_hexokay_xbar;  // {accel, dmac, d-port, i-port}
`endif
  assign d_hexokay = src_hexokay_xbar[1];

  wire              pwrup_req;
  wire              unblock_out;

  wire              uart_irq;
  wire              timer_irq;
  wire              i2s_irq;
  wire              dmac_irq;

  hazard3_cpu_2port #(
      // These must have the values given here for you to end up with a useful SoC:
      .RESET_VECTOR       (RESET_VECTOR),
      .MTVEC_INIT         (MTVEC_INIT),
      .CSR_M_MANDATORY    (1),
      .CSR_M_TRAP         (1),
      .DEBUG_SUPPORT      (1),
      // interrupts
      // uart
      // i2s
      // timer is not an external interrupt, gets handled differently
      .NUM_IRQS           (3),
      .RESET_REGFILE      (0),
      // Can be overridden from the defaults in hazard3_config.vh during
      // instantiation of kws_soc():
      .EXTENSION_A        (EXTENSION_A),
      .EXTENSION_C        (EXTENSION_C),
      .EXTENSION_M        (EXTENSION_M),
      .EXTENSION_ZBA      (EXTENSION_ZBA),
      .EXTENSION_ZBB      (EXTENSION_ZBB),
      .EXTENSION_ZBC      (EXTENSION_ZBC),
      .EXTENSION_ZBS      (EXTENSION_ZBS),
      .EXTENSION_ZBKB     (EXTENSION_ZBKB),
      .EXTENSION_ZIFENCEI (EXTENSION_ZIFENCEI),
      .EXTENSION_XH3BEXTM (EXTENSION_XH3BEXTM),
      .EXTENSION_XH3IRQ   (1),
      .EXTENSION_XH3PMPM  (EXTENSION_XH3PMPM),
      .EXTENSION_XH3POWER (EXTENSION_XH3POWER),
      .CSR_COUNTER        (CSR_COUNTER),
      .U_MODE             (U_MODE),
      .PMP_REGIONS        (PMP_REGIONS),
      .PMP_GRAIN          (PMP_GRAIN),
      .PMP_HARDWIRED      (PMP_HARDWIRED),
      .PMP_HARDWIRED_ADDR (PMP_HARDWIRED_ADDR),
      .PMP_HARDWIRED_CFG  (PMP_HARDWIRED_CFG),
      .MVENDORID_VAL      (MVENDORID_VAL),
      .BREAKPOINT_TRIGGERS(BREAKPOINT_TRIGGERS),
      .IRQ_PRIORITY_BITS  (IRQ_PRIORITY_BITS),
      // MIMPID_VAL / MHARTID_VAL are no longer parameters in upstream HEAD
      // (8af9929); they are wired as input ports below (mhartid_val, eco_version).
      .REDUCED_BYPASS     (REDUCED_BYPASS),
      .MULDIV_UNROLL      (MULDIV_UNROLL),
      .MUL_FAST           (MUL_FAST),
      .MUL_FASTER         (MUL_FASTER),
      .MULH_FAST          (MULH_FAST),
      .FAST_BRANCHCMP     (FAST_BRANCHCMP),
      .BRANCH_PREDICTOR   (BRANCH_PREDICTOR),
      .MTVEC_WMASK        (MTVEC_WMASK)
  ) cpu (
      .clk          (clk),
      .clk_always_on(clk),
      .rst_n        (rst_n_cpu),

      .pwrup_req  (pwrup_req),
      .pwrup_ack  (pwrup_req),   // Tied back
      .clk_en     (  /* unused */),
      .unblock_out(unblock_out),
      .unblock_in (unblock_out), // Tied back

      // Instruction-fetch port
      .i_haddr    (i_haddr),
      .i_hwrite   (i_hwrite),
      .i_htrans   (i_htrans),
      .i_hsize    (i_hsize),
      .i_hburst   (i_hburst),
      .i_hprot    (i_hprot),
      .i_hmastlock(i_hmastlock),
      .i_hmaster  (i_hmaster),
      .i_hready   (i_hready),
      .i_hresp    (i_hresp),
      .i_hwdata   (i_hwdata),
      .i_hrdata   (i_hrdata),

      // Load/store port
      .d_haddr    (d_haddr),
      .d_hwrite   (d_hwrite),
      .d_htrans   (d_htrans),
      .d_hsize    (d_hsize),
      .d_hburst   (d_hburst),
      .d_hprot    (d_hprot),
      .d_hmastlock(d_hmastlock),
      .d_hmaster  (d_hmaster),
      .d_hexcl    (d_hexcl),
      .d_hready   (d_hready),
      .d_hresp    (d_hresp),
      .d_hexokay  (d_hexokay),
      .d_hwdata   (d_hwdata),
      .d_hrdata   (d_hrdata),

      .dbg_req_halt              (hart_req_halt),
      .dbg_req_halt_on_reset     (hart_req_halt_on_reset),
      .dbg_req_resume            (hart_req_resume),
      .dbg_halted                (hart_halted),
      .dbg_running               (hart_running),

      .dbg_data0_rdata           (hart_data0_rdata),
      .dbg_data0_wdata           (hart_data0_wdata),
      .dbg_data0_wen             (hart_data0_wen),

      .dbg_instr_data            (hart_instr_data),
      .dbg_instr_data_vld        (hart_instr_data_vld),
      .dbg_instr_data_rdy        (hart_instr_data_rdy),
      .dbg_instr_caught_exception(hart_instr_caught_exception),
      .dbg_instr_caught_ebreak   (hart_instr_caught_ebreak),

      .dbg_sbus_addr             (sbus_addr),
      .dbg_sbus_write            (sbus_write),
      .dbg_sbus_size             (sbus_size),
      .dbg_sbus_vld              (sbus_vld),
      .dbg_sbus_rdy              (sbus_rdy),
      .dbg_sbus_err              (sbus_err),
      .dbg_sbus_wdata            (sbus_wdata),
      .dbg_sbus_rdata            (sbus_rdata),

      .irq      ({uart_irq, dmac_irq}),

      .soft_irq (1'b0),
      .timer_irq(timer_irq),

      // Memory-ordering: we don't implement a fence interface, so tie fence_rdy
      // high (always ready). The fence_i_vld / fence_d_vld outputs are unused.
      .fence_i_vld (/* unused */),
      .fence_d_vld (/* unused */),
      .fence_rdy   (1'b1),

      // Identification CSR values (formerly parameters MHARTID_VAL / MIMPID_VAL).
      .mhartid_val (32'h0),
      .eco_version (4'h0)
  );


  // ----------------------------------------------------------------------------
  // Bus fabric
  //
  // Memory map:
  // - 128 kB SRAM at... 0x0000_0000
  // - System timer at.. 0x4000_0000
  // - UART at.......... 0x4000_4000
  // - I2S at........... 0x4000_8000
  // - DMAC at.......... 0x6000_0000
  // - XIP Flash at..... 0x8000_0000
  //
  // Masters (N_MASTERS=3):
  //   Master 0 — i-port (instruction fetch)  — LSB slice
  //   Master 1 — d-port (load/store + SBA)
  //   Master 2 — DMAC master port            — MSB slice
  //
  // Slaves (N_SLAVES=4):
  //   Slave 0 — SRAM        (0x0000_0000, mask 0xe000_0000)
  //   Slave 1 — APB bridge  (0x4000_0000, mask 0xe000_0000)
  //   Slave 2 — DMAC regs   (0x6000_0000, mask 0xe000_0000)
  //   Slave 3 — XIP flash   (0x8000_0000, mask 0xe000_0000)

  // CONN_MATRIX[master*N_SLAVES + slave] — which masters can reach which slaves
  // Master 0 (i-port): SRAM[0]=1, Bridge[1]=0, DMACregs[2]=0, XIP[3]=1 → 4'b1001 → bits [ 3:0]
  // Master 1 (d-port): SRAM[0]=1, Bridge[1]=1, DMACregs[2]=1, XIP[3]=1 → 4'b1111 → bits [ 7:4]
  // Master 2 (dmac) : SRAM[0]=1, Bridge[1]=1, DMACregs[2]=0, XIP[3]=0 → 4'b0011 → bits [11:8]
  // CONN_MATRIX_TRANSPOSE[slave*N_MASTERS + master]:
  // SRAM←{dmac,d,i}=3'b111 [2:0], Bridge←{dmac,d,i}=3'b110 [5:3],
  // DMACregs←{dmac,d,i}=3'b010 [8:6], XIP←{dmac,d,i}=3'b011 [11:9]

  // --- Slave-side wires ---

  wire              sram0_hready_resp;
  wire              sram0_hready;
  wire              sram0_hresp;
  wire [W_ADDR-1:0] sram0_haddr;
  wire              sram0_hwrite;
  wire [       1:0] sram0_htrans;
  wire [       2:0] sram0_hsize;
  wire [       2:0] sram0_hburst;
  wire [       3:0] sram0_hprot;
  wire              sram0_hmastlock;
  wire [W_DATA-1:0] sram0_hwdata;
  wire [W_DATA-1:0] sram0_hrdata;

  wire              bridge_hready_resp;
  wire              bridge_hready;
  wire              bridge_hresp;
  wire [W_ADDR-1:0] bridge_haddr;
  wire              bridge_hwrite;
  wire [       1:0] bridge_htrans;
  wire [       2:0] bridge_hsize;
  wire [       2:0] bridge_hburst;
  wire [       3:0] bridge_hprot;
  wire              bridge_hmastlock;
  wire [W_DATA-1:0] bridge_hwdata;
  wire [W_DATA-1:0] bridge_hrdata;

  wire              xip_hready_resp;
  wire              xip_hready;
  wire              xip_hresp;
  wire [W_ADDR-1:0] xip_haddr;
  wire              xip_hwrite;
  wire [       1:0] xip_htrans;
  wire [       2:0] xip_hsize;
  wire [       2:0] xip_hburst;
  wire [       3:0] xip_hprot;
  wire              xip_hmastlock;
  wire [W_DATA-1:0] xip_hwdata;
  wire [W_DATA-1:0] xip_hrdata;

  // DMAC AHB slave signals (crossbar slave port 2)
  wire              dmac_hready_resp;
  wire              dmac_hready;
  wire [W_ADDR-1:0] dmac_haddr;
  wire              dmac_hwrite;
  wire [       1:0] dmac_htrans;
  wire [       2:0] dmac_hsize;
  wire [       2:0] dmac_hburst;
  wire [       3:0] dmac_hprot;
  wire              dmac_hmastlock;
  wire [W_DATA-1:0] dmac_hwdata;
  wire [W_DATA-1:0] dmac_hrdata;

  // DMAC AHB master signals (crossbar master port 2)
  wire [W_ADDR-1:0] dmac_m_haddr;
  wire              dmac_m_hwrite;
  wire [       1:0] dmac_m_htrans;
  wire [       2:0] dmac_m_hsize;
  wire [W_DATA-1:0] dmac_m_hwdata;
  wire              dmac_m_hready;
  wire [W_DATA-1:0] dmac_m_hrdata;
  wire              dmac_m_hresp; // crossbar drives this; DMAC master ignores HRESP

  // Conv1D accelerator AHB master signals (crossbar master port 3)
  wire [W_ADDR-1:0] accel_haddr;
  wire              accel_hwrite;
  wire [       1:0] accel_htrans;
  wire [       2:0] accel_hsize;
  wire [       2:0] accel_hburst;
  wire [       3:0] accel_hprot;
  wire              accel_hmastlock;
  wire [W_DATA-1:0] accel_hwdata;
  wire              accel_hready;
  wire [W_DATA-1:0] accel_hrdata;
  wire              accel_hresp;

`ifdef XIP_PLAYBACK
  // XIP sample playback AHB master signals (crossbar master port 4)
  wire [W_ADDR-1:0] playback_haddr;
  wire              playback_hwrite;
  wire [       1:0] playback_htrans;
  wire [       2:0] playback_hsize;
  wire [W_DATA-1:0] playback_hrdata;
  wire              playback_hready;
  wire              playback_hresp;
`endif

`ifdef XIP_PLAYBACK
  // Bus fabric:
  // Masters (N_MASTERS=5):
  //   Master 0 — i-port     SRAM+XIP              CONN[3:0]  = 4'b1001
  //   Master 1 — d-port     SRAM+Bridge+DMAC+XIP  CONN[7:4]  = 4'b1111
  //   Master 2 — DMAC       SRAM+Bridge           CONN[11:8] = 4'b0011
  //   Master 3 — accel      SRAM+XIP              CONN[15:12]= 4'b1001
  //   Master 4 — playback   XIP only              CONN[19:16]= 4'b1000
  // CONN_MATRIX_TRANSPOSE[slave*N_MASTERS+master]:
  //   SRAM[0]    ← accel+dmac+d+i     5'b01111  [4:0]
  //   Bridge[1]  ← dmac+d-port        5'b00110  [9:5]
  //   DMACregs[2]← d-port only        5'b00010  [14:10]
  //   XIP[3]     ← playback+accel+d+i 5'b10011  [19:15]

  localparam N_MASTERS_XBAR = 5;
  localparam CONN_MATRIX_PB       = 20'b1000_1001_0011_1111_1001;
  localparam CONN_MATRIX_TRANS_PB = 20'b1001_1000_1000_1100_1111;
`else
  // Bus fabric:
  // Masters (N_MASTERS=4):
  //   Master 0 — i-port   SRAM+XIP              CONN[3:0]  = 4'b1001
  //   Master 1 — d-port   SRAM+Bridge+DMAC+XIP  CONN[7:4]  = 4'b1111
  //   Master 2 — DMAC     SRAM+Bridge            CONN[11:8] = 4'b0011
  //   Master 3 — accel    SRAM+XIP               CONN[15:12]= 4'b1001
  // CONN_MATRIX_TRANSPOSE[slave*N_MASTERS+master]:
  //   SRAM[0]    ← all 4          4'b1111  [3:0]
  //   Bridge[1]  ← d-port+dmac   4'b0110  [7:4]
  //   DMACregs[2]← d-port only   4'b0010  [11:8]
  //   XIP[3]     ← i+d+accel     4'b1011  [15:12]

  localparam N_MASTERS_XBAR = 4;
  localparam CONN_MATRIX_PB       = 16'b1001_0011_1111_1001;
  localparam CONN_MATRIX_TRANS_PB = 16'b1011_0010_0110_1111;
`endif

  ahbl_crossbar #(
      .N_MASTERS(N_MASTERS_XBAR),
      .N_SLAVES (4),
      .W_ADDR   (W_ADDR),
      .W_DATA   (W_DATA),
      .ADDR_MAP (128'h80000000_60000000_40000000_00000000),
      .ADDR_MASK(128'he0000000_e0000000_e0000000_e0000000),
      .CONN_MATRIX           (CONN_MATRIX_PB),
      .CONN_MATRIX_TRANSPOSE (CONN_MATRIX_TRANS_PB)
  ) xbar_u (
      .clk  (clk),
      .rst_n(rst_n),

`ifdef XIP_PLAYBACK
      // Masters (N_MASTERS=5): {playback [MSB], accel, dmac, d-port, i-port [LSB]}.
      .src_hready_resp({playback_hready,  accel_hready,    dmac_m_hready,  d_hready,    i_hready   }),
      .src_hresp      ({playback_hresp,   accel_hresp,     dmac_m_hresp,   d_hresp,     i_hresp    }),
      .src_hexokay    (src_hexokay_xbar),
      .src_haddr      ({playback_haddr,   accel_haddr,     dmac_m_haddr,   d_haddr,     i_haddr    }),
      .src_hwrite     ({playback_hwrite,  accel_hwrite,    dmac_m_hwrite,  d_hwrite,    i_hwrite   }),
      .src_htrans     ({playback_htrans,  accel_htrans,    dmac_m_htrans,  d_htrans,    i_htrans   }),
      .src_hsize      ({playback_hsize,   accel_hsize,     dmac_m_hsize,   d_hsize,     i_hsize    }),
      .src_hburst     ({3'b000,           accel_hburst,    3'b000,         d_hburst,    i_hburst   }),
      .src_hprot      ({4'b0011,          accel_hprot,     4'b0011,        d_hprot,     i_hprot    }),
      .src_hmaster    ({8'd0,             8'd0,            8'd0,           d_hmaster,   i_hmaster  }),
      .src_hmastlock  ({1'b0,             accel_hmastlock, 1'b0,           d_hmastlock, i_hmastlock}),
      .src_hexcl      ({1'b0,             1'b0,            1'b0,           d_hexcl,     i_hexcl    }),
      .src_hwdata     ({32'h0,            accel_hwdata,    dmac_m_hwdata,  d_hwdata,    i_hwdata   }),
      .src_hrdata     ({playback_hrdata,  accel_hrdata,    dmac_m_hrdata,  d_hrdata,    i_hrdata   }),
`else
      // Masters (N_MASTERS=4): {accel [MSB], dmac, d-port, i-port [LSB]}.
      .src_hready_resp({accel_hready,    dmac_m_hready,  d_hready,    i_hready   }),
      .src_hresp      ({accel_hresp,     dmac_m_hresp,   d_hresp,     i_hresp    }),
      .src_hexokay    (src_hexokay_xbar),
      .src_haddr      ({accel_haddr,     dmac_m_haddr,   d_haddr,     i_haddr    }),
      .src_hwrite     ({accel_hwrite,    dmac_m_hwrite,  d_hwrite,    i_hwrite   }),
      .src_htrans     ({accel_htrans,    dmac_m_htrans,  d_htrans,    i_htrans   }),
      .src_hsize      ({accel_hsize,     dmac_m_hsize,   d_hsize,     i_hsize    }),
      .src_hburst     ({accel_hburst,    3'b000,         d_hburst,    i_hburst   }),
      .src_hprot      ({accel_hprot,     4'b0011,        d_hprot,     i_hprot    }),
      .src_hmaster    ({8'd0,            8'd0,           d_hmaster,   i_hmaster  }),
      .src_hmastlock  ({accel_hmastlock, 1'b0,           d_hmastlock, i_hmastlock}),
      .src_hexcl      ({1'b0,            1'b0,           d_hexcl,     i_hexcl    }),
      .src_hwdata     ({accel_hwdata,    dmac_m_hwdata,  d_hwdata,    i_hwdata   }),
      .src_hrdata     ({accel_hrdata,    dmac_m_hrdata,  d_hrdata,    i_hrdata   }),
`endif

      // Slaves: {xip [MSB], dmac regs, bridge, sram [LSB]}.
      // No slave drives hexokay; tie off.  dst_hmaster / dst_hexcl unused.
      .dst_hready_resp({xip_hready_resp,  dmac_hready_resp, bridge_hready_resp, sram0_hready_resp}),
      .dst_hready     ({xip_hready,       dmac_hready,      bridge_hready,      sram0_hready     }),
      .dst_hresp      ({xip_hresp,        1'b0,             bridge_hresp,       sram0_hresp      }),
      .dst_hexokay    (4'b0000),
      .dst_haddr      ({xip_haddr,        dmac_haddr,       bridge_haddr,       sram0_haddr      }),
      .dst_hwrite     ({xip_hwrite,       dmac_hwrite,      bridge_hwrite,      sram0_hwrite     }),
      .dst_htrans     ({xip_htrans,       dmac_htrans,      bridge_htrans,      sram0_htrans     }),
      .dst_hsize      ({xip_hsize,        dmac_hsize,       bridge_hsize,       sram0_hsize      }),
      .dst_hburst     ({xip_hburst,       dmac_hburst,      bridge_hburst,      sram0_hburst     }),
      .dst_hprot      ({xip_hprot,        dmac_hprot,       bridge_hprot,       sram0_hprot      }),
      .dst_hmaster    (/* unused */),
      .dst_hmastlock  ({xip_hmastlock,    dmac_hmastlock,   bridge_hmastlock,   sram0_hmastlock  }),
      .dst_hexcl      (/* unused */),
      .dst_hwdata     ({xip_hwdata,       dmac_hwdata,      bridge_hwdata,      sram0_hwdata     }),
      .dst_hrdata     ({xip_hrdata,       dmac_hrdata,      bridge_hrdata,      sram0_hrdata     })
  );

  // APB layer

  wire        bridge_psel;
  wire        bridge_penable;
  wire        bridge_pwrite;
  wire [15:0] bridge_paddr;
  wire [31:0] bridge_pwdata;
  wire [31:0] bridge_prdata;
  wire        bridge_pready;
  wire        bridge_pslverr;

  wire        uart_psel;
  wire        uart_penable;
  wire        uart_pwrite;
  wire [15:0] uart_paddr;
  wire [31:0] uart_pwdata;
  wire [31:0] uart_prdata;
  wire        uart_pready;
  wire        uart_pslverr;

  wire        timer_psel;
  wire        timer_penable;
  wire        timer_pwrite;
  wire [15:0] timer_paddr;
  wire [31:0] timer_pwdata;
  wire [31:0] timer_prdata;
  wire        timer_pready;
  wire        timer_pslverr;

  wire        i2s_psel;
  wire        i2s_penable;
  wire        i2s_pwrite;
  wire [15:0] i2s_paddr;
  wire [31:0] i2s_pwdata;
  wire [31:0] i2s_prdata;
  wire        i2s_pready;
  wire        i2s_pslverr;

  wire        accel_psel;
  wire        accel_penable;
  wire        accel_pwrite;
  wire [15:0] accel_paddr;
  wire [31:0] accel_pwdata;
  wire [31:0] accel_prdata;
  wire        accel_pready;
  wire        accel_pslverr;

  ahbl_to_apb apb_bridge_u (
      .clk  (clk),
      .rst_n(rst_n),

      .ahbls_hready     (bridge_hready),
      .ahbls_hready_resp(bridge_hready_resp),
      .ahbls_hresp      (bridge_hresp),
      .ahbls_haddr      (bridge_haddr),
      .ahbls_hwrite     (bridge_hwrite),
      .ahbls_htrans     (bridge_htrans),
      .ahbls_hsize      (bridge_hsize),
      .ahbls_hburst     (bridge_hburst),
      .ahbls_hprot      (bridge_hprot),
      .ahbls_hmastlock  (bridge_hmastlock),
      .ahbls_hwdata     (bridge_hwdata),
      .ahbls_hrdata     (bridge_hrdata),

      .apbm_paddr  (bridge_paddr),
      .apbm_psel   (bridge_psel),
      .apbm_penable(bridge_penable),
      .apbm_pwrite (bridge_pwrite),
      .apbm_pwdata (bridge_pwdata),
      .apbm_pready (bridge_pready),
      .apbm_prdata (bridge_prdata),
      .apbm_pslverr(bridge_pslverr)
  );

  // APB slaves:
  //   slave 0 = timer  paddr[15:14]=2'b00 → 0x0000
  //   slave 1 = i2s    paddr[15:14]=2'b10 → 0x8000
  //   slave 2 = uart   paddr[15:14]=2'b01 → 0x4000
  //   slave 3 = accel  paddr[15:14]=2'b11 → 0xC000
  apb_splitter #(
      .N_SLAVES (4),
      .W_ADDR   (16),
      .ADDR_MAP (64'hC000_4000_8000_0000),
      .ADDR_MASK(64'hC000_C000_C000_C000)
  ) inst_apb_splitter (
      .apbs_paddr   (bridge_paddr),
      .apbs_psel    (bridge_psel),
      .apbs_penable (bridge_penable),
      .apbs_pwrite  (bridge_pwrite),
      .apbs_pwdata  (bridge_pwdata),
      .apbs_pready  (bridge_pready),
      .apbs_prdata  (bridge_prdata),
      .apbs_pslverr (bridge_pslverr),

      .apbm_paddr   ({accel_paddr,   uart_paddr,   i2s_paddr,   timer_paddr  }),
      .apbm_psel    ({accel_psel,    uart_psel,    i2s_psel,    timer_psel   }),
      .apbm_penable ({accel_penable, uart_penable, i2s_penable, timer_penable}),
      .apbm_pwrite  ({accel_pwrite,  uart_pwrite,  i2s_pwrite,  timer_pwrite }),
      .apbm_pwdata  ({accel_pwdata,  uart_pwdata,  i2s_pwdata,  timer_pwdata }),
      .apbm_pready  ({accel_pready,  uart_pready,  i2s_pready,  timer_pready }),
      .apbm_prdata  ({accel_prdata,  uart_prdata,  i2s_prdata,  timer_prdata }),
      .apbm_pslverr ({accel_pslverr, uart_pslverr, i2s_pslverr, timer_pslverr})
  );

  // ----------------------------------------------------------------------------
  // Memory and peripherals

  // No preloaded bootloader -- just use the debugger! (the processor will
  // actually enter an infinite crash loop after reset if memory is
  // zero-initialised so don't leave the little guy hanging too long)

  ahb_sync_sram #(
      .DEPTH(SRAM_DEPTH)
  ) sram0 (
      .clk  (clk),
      .rst_n(rst_n),

      .ahbls_hready_resp(sram0_hready_resp),
      .ahbls_hready     (sram0_hready),
      .ahbls_hresp      (sram0_hresp),
      .ahbls_haddr      (sram0_haddr),
      .ahbls_hwrite     (sram0_hwrite),
      .ahbls_htrans     (sram0_htrans),
      .ahbls_hsize      (sram0_hsize),
      .ahbls_hburst     (sram0_hburst),
      .ahbls_hprot      (sram0_hprot),
      .ahbls_hmastlock  (sram0_hmastlock),
      .ahbls_hwdata     (sram0_hwdata),
      .ahbls_hrdata     (sram0_hrdata)
  );

  wire xip_hsel_internal = (xip_haddr[31:28] == 4'h8);
  // TODO: Make the following parameters config dependent
  ahbl_flash_ctrl_eb_cache #(
      .LW(32*8),
      .NL(256)
  ) xip (
      .HCLK                (clk),
      .HRESETn             (rst_n_cpu),

      .HSEL                (xip_hsel_internal), // TODO: change splitter to avoid need for this
      .HADDR               (xip_haddr),
      .HTRANS              (xip_htrans),
      .HWRITE              (xip_hwrite),
      .HREADY              (xip_hready),
      .HREADYOUT           (xip_hready_resp),
      .HRDATA              (xip_hrdata),
      .HRESP               (xip_hresp),

      // External Interface to Quad I/O
      .csn                  (xip_csn),
      .sck                  (xip_sck),
      .doe                  (xip_doe),
      .spi_do               (xip_do),
      .di                   (flash_di)
  );

  wire dmac_hsel_internal = (dmac_haddr[31:29] == 3'b011);

  MS_DMAC_AHBL dmac_u (
      .HCLK    (clk),
      .HRESETn (rst_n),

      .IRQ     (dmac_irq),

      // Slave port — configuration registers (0x6000_0000)
      .HSEL     (dmac_hsel_internal),
      .HADDR    (dmac_haddr),
      .HTRANS   (dmac_htrans),
      .HWRITE   (dmac_hwrite),
      .HREADY   (dmac_hready),
      .HWDATA   (dmac_hwdata),
      .HSIZE    (dmac_hsize),
      .HREADYOUT(dmac_hready_resp),
      .HRDATA   (dmac_hrdata),

      // Master port — DMA transfers (crossbar master 2)
      .M_HADDR  (dmac_m_haddr),
      .M_HTRANS (dmac_m_htrans),
      .M_HSIZE  (dmac_m_hsize),
      .M_HWRITE (dmac_m_hwrite),
      .M_HWDATA (dmac_m_hwdata),
      .M_HREADY (dmac_m_hready),
      .M_HRDATA (dmac_m_hrdata),

      // Peripheral trigger: I2S FIFO-ready fires DMA when CTRL.TRIGGER[0]=1
      .PIRQ    (i2s_irq)
  );

  uart_mini uart_u (
      .clk  (clk),
      .rst_n(rst_n),

      .apbs_psel   (uart_psel),
      .apbs_penable(uart_penable),
      .apbs_pwrite (uart_pwrite),
      .apbs_paddr  (uart_paddr),
      .apbs_pwdata (uart_pwdata),
      .apbs_prdata (uart_prdata),
      .apbs_pready (uart_pready),
      .apbs_pslverr(uart_pslverr),

      .rx  (uart_rx),
      .tx  (uart_tx),
      .cts (1'b0),
      .rts (  /* unused */),
      .irq (uart_irq),
      .dreq(  /* unused */)
  );

  // I2S SD input mux: real mic pin vs XIP sample playback
  wire i2s_sd_internal;

`ifdef XIP_PLAYBACK
  // XIP sample playback: read samples from XIP flash via AHB bus and
  // feed them serially to the I2S receiver's sd input.
  wire xip_player_sd;

  xip_sample_player #(
      .SAMPLE_XIP_ADDR(32'h8001_0000),  // matches .playback_samples in link.ld
      .N_SAMPLES      (8000)            // 1 second at 8 kHz
  ) xip_player_u (
      .clk       (clk),
      .rst_n     (rst_n),
      .sck_ref   (i2s_sck_out),
      .ws_ref    (i2s_ws_out),
      .sd_out    (xip_player_sd),
      // AHB master port (crossbar master slot 4)
      .m_haddr   (playback_haddr),
      .m_hwrite  (playback_hwrite),
      .m_htrans  (playback_htrans),
      .m_hsize   (playback_hsize),
      .m_hrdata  (playback_hrdata),
      .m_hready  (playback_hready),
      .m_hresp   (playback_hresp)
  );

  assign i2s_sd_internal = xip_player_sd;
`else
  // Normal operation: use external I2S mic input
  assign i2s_sd_internal = i2s_sd;
`endif

  // FIFO_DEPTH comes from the root Makefile (-DI2S_FIFO_DEPTH=N).  The
  // firmware DMA burst (test/common/kws_bare_main.c) is derived as N/2
  // from the same source so the two stay in lock-step — see task #13.
`ifndef I2S_FIFO_DEPTH
`define I2S_FIFO_DEPTH 16
`endif
  apb_i2s_receiver #(
      .FIFO_DEPTH(`I2S_FIFO_DEPTH)
  ) apb_i2s_receiver_inst (
      .clk          (clk),
      .rst_n        (rst_n),
      .sd           (i2s_sd_internal),
      .apbs_psel    (i2s_psel),
      .apbs_penable (i2s_penable),
      .apbs_pwrite  (i2s_pwrite),
      .apbs_paddr   (i2s_paddr),
      .apbs_pwdata  (i2s_pwdata),
      .apbs_prdata  (i2s_prdata),
      .apbs_pready  (i2s_pready),
      .apbs_pslverr (i2s_pslverr),
      .sck_out      (i2s_sck_out),
      .ws_out       (i2s_ws_out),
      .i2s_irq      (i2s_irq)
  );


  // Microsecond timebase for timer

  reg [$clog2(CLK_MHZ)-1:0] timer_tick_ctr;
  reg                       timer_tick;

  always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      timer_tick_ctr <= {$clog2(CLK_MHZ) {1'b0}};
      timer_tick <= 1'b0;
    end else begin
      if (|timer_tick_ctr) begin
        timer_tick_ctr <= timer_tick_ctr - 1'b1;
      end else begin
        timer_tick_ctr <= CLK_MHZ - 1;
      end
      timer_tick <= ~|timer_tick_ctr;
    end
  end

  hazard3_riscv_timer timer_u (
      .clk  (clk),
      .rst_n(rst_n),

      .psel   (timer_psel),
      .penable(timer_penable),
      .pwrite (timer_pwrite),
      .paddr  (timer_paddr),
      .pwdata (timer_pwdata),
      .prdata (timer_prdata),
      .pready (timer_pready),
      .pslverr(timer_pslverr),

      .dbg_halt(&hart_halted),

      .tick(timer_tick),

      .timer_irq(timer_irq)
  );

  // Conv1D hardware accelerator — APB slave at 0x4000_C000, AHB master port 3.
  conv1d_accel accel_u (
      .clk  (clk),
      .rst_n(rst_n),

      // APB slave
      .paddr  (accel_paddr),
      .psel   (accel_psel),
      .penable(accel_penable),
      .pwrite (accel_pwrite),
      .pwdata (accel_pwdata),
      .prdata (accel_prdata),
      .pready (accel_pready),
      .pslverr(accel_pslverr),

      // AHB-Lite master (crossbar port 3)
      .haddr    (accel_haddr),
      .hburst   (accel_hburst),
      .hsize    (accel_hsize),
      .htrans   (accel_htrans),
      .hwrite   (accel_hwrite),
      .hwdata   (accel_hwdata),
      .hprot    (accel_hprot),
      .hmastlock(accel_hmastlock),
      .hrdata   (accel_hrdata),
      .hready   (accel_hready),
      .hresp    (accel_hresp)
  );

endmodule
