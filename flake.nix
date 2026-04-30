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
          # Build the riscv-collab/riscv-gnu-toolchain meta-repo + GCC 14
          # branch from source (multilib soft-float, no Zcmp), matching the
          # recipe used to install /opt/riscv/gcc14-no-zcmp.  The previous
          # nixpkgs-backed toolchain was GCC 13.3 single-multilib (ilp32d
          # libgcc only) and produced inference cycle counts ~50M cycles
          # off the canonical multilib build.  See nix/riscv-toolchain.nix
          # for the full recipe + first-build hash flow.
          riscvToolchain = pkgs.callPackage ./nix/riscv-toolchain.nix {};

          # ── OpenOCD with RISC-V support ─────────────────────────────────────
          # Build the riscv/riscv-openocd fork from source so we get the
          # same `riscv-openocd` binary as the system install at
          # /usr/local/bin/riscv-openocd (configured with
          # --enable-remote-bitbang --enable-ftdi --program-prefix=riscv-).
          # See nix/riscv-openocd.nix for the recipe + first-build hash
          # flow.
          riscvOpenocd = pkgs.callPackage ./nix/riscv-openocd.nix {};

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
              # ── Optional fast-path toolchain override ────────────────────
              # The Nix-built toolchain (nix/riscv-toolchain.nix) is the
              # same canonical multilib GCC 14 + binutils + newlib build
              # as /opt/riscv/gcc14-no-zcmp — same recipe, same hashes.
              # If the local install is present, prefer it just to skip
              # the first-build wallclock (~30-60 min) on machines where
              # the binary cache is cold.  Functionally identical.
              # Set KWS_FORCE_NIX_TOOLCHAIN=1 to bypass the local fast
              # path and exercise the Nix-built toolchain — useful for
              # verifying byte-/cycle-equivalence between the two.
              if [ -z "$KWS_FORCE_NIX_TOOLCHAIN" ] \
                 && [ -x /opt/riscv/gcc14-no-zcmp/bin/riscv32-unknown-elf-gcc ]; then
                export PATH=/opt/riscv/gcc14-no-zcmp/bin:$PATH
                _kws_toolchain="local: /opt/riscv/gcc14-no-zcmp (cached)"
              else
                _kws_toolchain="nix: $(command -v riscv32-unknown-elf-gcc 2>/dev/null || echo 'unavailable')"
              fi

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
              echo "║  RISC-V toolchain: $_kws_toolchain"
              echo "║  NOTE: Quartus (FPGA synthesis) must be          ║"
              echo "║  installed separately and on PATH.               ║"
              echo "╚══════════════════════════════════════════════════╝"
              unset _kws_toolchain
            '';
          };
        }
      );
    };
}
