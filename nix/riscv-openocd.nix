/*  nix/riscv-openocd.nix
 *
 *  Builds the same `riscv-openocd` we have system-installed at
 *  /usr/local/bin/riscv-openocd:
 *
 *    git clone https://github.com/riscv/riscv-openocd.git
 *    cd riscv-openocd
 *    ./bootstrap
 *    ./configure --enable-remote-bitbang --enable-ftdi --program-prefix=riscv-
 *    make -j $(nproc)
 *    sudo make install
 *
 *  This is the riscv-collab fork of OpenOCD with RISC-V Debug Module
 *  support, used for our Verilator + remote-bitbang JTAG flow
 *  (`make openocd-sim`) and for FPGA bring-up (`make openocd-hw`).
 *
 *  ── First-build hash flow ────────────────────────────────────────────
 *  Same lib.fakeHash pattern as nix/riscv-toolchain.nix: nix build
 *  prints the real hash on first attempt; paste it back into
 *  `riscv-openocd-hash` and rebuild.
 */
{ lib, stdenv, fetchFromGitHub
, autoconf, automake, libtool, pkg-config, gnumake, gnused
, libusb1, libftdi1, hidapi, jimtcl, libjaylink, libyaml
, makeWrapper, perl
}:

let
  riscv-openocd-rev  = "riscv";   # tracks main "riscv" branch — pin to a SHA for reproducibility
  riscv-openocd-hash = lib.fakeHash;

  src = fetchFromGitHub {
    owner = "riscv";
    repo  = "riscv-openocd";
    rev   = riscv-openocd-rev;
    hash  = riscv-openocd-hash;
    fetchSubmodules = true;       # JimTcl / libjaylink come along
  };
in
stdenv.mkDerivation {
  pname   = "riscv-openocd";
  version = "0.12.0+riscv-${if builtins.stringLength riscv-openocd-rev > 7
                            then builtins.substring 0 7 riscv-openocd-rev
                            else riscv-openocd-rev}";

  inherit src;

  nativeBuildInputs = [
    autoconf automake libtool pkg-config gnumake gnused makeWrapper perl
  ];
  buildInputs = [
    libusb1 libftdi1 hidapi jimtcl libjaylink libyaml
  ];

  preConfigure = ''
    ./bootstrap
  '';

  configureFlags = [
    "--enable-remote-bitbang"
    "--enable-ftdi"
    "--program-prefix=riscv-"
  ];

  enableParallelBuilding = true;

  meta = {
    description =
      "OpenOCD with RISC-V Debug Module support (riscv/riscv-openocd "
      + "fork).  Matches the /usr/local/bin/riscv-openocd recipe.";
    platforms = lib.platforms.linux;
    license   = lib.licenses.gpl2Plus;
  };
}
