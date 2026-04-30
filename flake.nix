{
  description = "KWS-SoC — RISC-V SoC simulation and firmware development shell";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";

  outputs = { self, nixpkgs }:
    let
      supportedSystems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
    in {
      devShells = forAllSystems (system:
        let
          pkgs = import nixpkgs { inherit system; };

          # ── yosys with working yosys-config --datdir ───────────────────────
          # nixpkgs yosys-config passes --datdir through literally instead of
          # expanding it to the share path, which breaks the CXXRTL include:
          #   -I$(shell yosys-config --datdir)/include/backends/cxxrtl/runtime
          # We prepend a wrapper binary that intercepts --datdir and returns the
          # correct path; all other flags fall through to the original.
          yosysFixed = pkgs.symlinkJoin {
            name = "yosys-with-datdir";
            paths = [
              (pkgs.writeShellScriptBin "yosys-config" ''
                for arg in "$@"; do
                  case "$arg" in
                    --datdir) printf '%s\n' "${pkgs.yosys}/share/yosys"; exit 0 ;;
                  esac
                done
                exec "${pkgs.yosys}/bin/yosys-config" "$@"
              '')
              pkgs.yosys
            ];
          };

          # ── RISC-V 32-bit bare-metal cross toolchain ────────────────────────
          # nixpkgs ships the toolchain under the riscv32-none-elf- prefix, but
          # test/Makefile hardcodes CROSS_COMPILE = riscv32-unknown-elf-
          # so we add aliased symlinks for every riscv32-none-elf-* binary.
          riscvPkgs = pkgs.pkgsCross.riscv32-embedded;

          riscvToolchain = pkgs.symlinkJoin {
            name = "riscv32-unknown-elf-toolchain";
            paths = with riscvPkgs.buildPackages; [ gcc binutils gdb ];
            postBuild = ''
              for exe in "$out"/bin/riscv32-none-elf-*; do
                base=$(basename "$exe")
                alias="riscv32-unknown-elf-''${base#riscv32-none-elf-}"
                [ -e "$out/bin/$alias" ] || ln -s "$exe" "$out/bin/$alias"
              done
            '';
          };

          # ── OpenOCD with RISC-V support ─────────────────────────────────────
          # The installed riscv-openocd is 0.12.0+dev-04404 (riscv/riscv-openocd fork,
          # built with --enable-remote-bitbang --enable-ftdi --program-prefix=riscv-).
          # nixpkgs openocd 0.12.0 is built with the same flags (verified: configureFlags
          # includes --enable-remote-bitbang and --enable-ftdi) so it is a drop-in.
          # We alias the binary as `riscv-openocd` to match Makefile invocations.
          #
          # TODO: Run `make openocd-sim` with this binary to confirm the remote-bitbang
          # JTAG session works identically to the fork before removing this comment.
          # If there are issues, replace with a derivation that builds the fork from:
          #   github:riscv/riscv-openocd  (pin to commit eb01c632a or HEAD of riscv-0.12 branch)
          riscvOpenocd = pkgs.symlinkJoin {
            name = "riscv-openocd";
            paths = [ pkgs.openocd ];
            postBuild = ''
              ln -s "$out/bin/openocd" "$out/bin/riscv-openocd"
            '';
          };

        in {
          default = pkgs.mkShell {
            name = "kws-soc";

            packages = [
              (pkgs.python3.withPackages (ps: with ps; [ pyserial ]))
              # ── RTL simulation ───────────────────────────────────────────────
              # yosys: CXXRTL synthesis backend (make sim_yosys)
              yosysFixed
              # verilator: fast C++-based simulation + lint (make sim_verilator, make lint)
              pkgs.verilator
              # iverilog: Icarus Verilog (iverilog + vvp) for XIP testbenches (make test-xip)
              pkgs.iverilog

              # ── Host C++ compiler ────────────────────────────────────────────
              # Makefile uses clang++ explicitly (CLANGXX := clang++) to compile
              # the CXXRTL and Verilator testbench harnesses.
              pkgs.clang

              # ── RISC-V firmware toolchain ────────────────────────────────────
              # Provides riscv32-unknown-elf-{gcc,as,ld,objcopy,objdump,gdb}
              riscvToolchain

              # ── On-chip debug ────────────────────────────────────────────────
              # riscv-openocd for JTAG over remote-bitbang (make openocd-sim / openocd-hw)
              riscvOpenocd

              # ── Scripting & build ────────────────────────────────────────────
              # python3: Hazard3/scripts/listfiles and scripts/gen_inst_params.py
              pkgs.gnumake
              pkgs.git

              # ── Waveform viewer & debug utilities ────────────────────────────
              # gtkwave: view *.vcd / *.fst from sim-vcd / sim-verilator-vcd
              pkgs.gtkwave
              # telnet: used by `make telnet` to connect to OpenOCD telnet port
              pkgs.inetutils
            ];

            shellHook = ''
              echo "╔══════════════════════════════════════════════════╗"
              echo "║  KWS-SoC dev shell                               ║"
              echo "╠══════════════════════════════════════════════════╣"
              echo "║  Simulation                                       ║"
              echo "║    make sim_yosys       # CXXRTL build            ║"
              echo "║    make sim_verilator   # Verilator build         ║"
              echo "║    make sim-verilator   # run Verilator sim       ║"
              echo "║  Firmware                                         ║"
              echo "║    make test            # compile all C/ASM tests ║"
              echo "║  Debug                                            ║"
              echo "║    make openocd-sim     # start OpenOCD for sim   ║"
              echo "║    make gdb             # start GDB session       ║"
              echo "╠══════════════════════════════════════════════════╣"
              echo "║  NOTE: Quartus (FPGA synthesis) must be          ║"
              echo "║  installed separately and on PATH.               ║"
              echo "╚══════════════════════════════════════════════════╝"
            '';
          };
        }
      );
    };
}
