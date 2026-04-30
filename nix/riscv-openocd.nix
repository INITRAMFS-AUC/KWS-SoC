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
, makeWrapper, perl, which
}:

let
  # Pinned to a concrete SHA on the riscv-collab "riscv" branch so the
  # build is reproducible.  Re-prefetch with:
  #   nix-prefetch-git --url https://github.com/riscv/riscv-openocd \
  #                    --rev refs/heads/riscv --fetch-submodules
  # and paste rev + hash here.  fetchFromGitHub interprets a bare
  # branch name as `refs/tags/<name>` (which doesn't exist) — must be
  # a SHA or refs/heads/X.
  riscv-openocd-rev  = "eb01c632a4bb1c07d2bddb008d6987c809f1c496";  # 2025-10-09
  riscv-openocd-hash = "sha256-4a6Mt7nT6Lwvj5hf3vC9CFyZ+wSPrdXn/Ng670ZyRLI=";

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
    which   # ./bootstrap uses `which libtool` to locate libtool
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
