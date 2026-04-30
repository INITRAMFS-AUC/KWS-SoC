/*  nix/riscv-toolchain.nix
 *
 *  Builds the same RISC-V bare-metal cross toolchain we have at
 *  /opt/riscv/gcc14-no-zcmp:
 *
 *    git clone https://github.com/riscv/riscv-gnu-toolchain
 *    cd riscv-gnu-toolchain
 *    git clone --depth=1 https://github.com/gcc-mirror/gcc gcc-14 \
 *              -b releases/gcc-14
 *    ./configure --with-gcc-src=$(pwd)/gcc-14 \
 *                --prefix=/opt/riscv/gcc14-no-zcmp \
 *                --with-arch=rv32ia_zicsr --with-abi=ilp32 \
 *                --with-multilib-generator="rv32i-ilp32--;...;rv32ima_zicsr_zifencei_zba_zbb_zbc_zbs_zbkb_zca_zcb-ilp32--"
 *    make -j$(nproc)
 *
 *  The result is a multilib soft-float `riscv32-unknown-elf-` toolchain
 *  that excludes the Zcmp extension (Hazard3 doesn't implement it).
 *
 *  ── First-build hash flow ────────────────────────────────────────────
 *  The two `lib.fakeHash` placeholders below are intentional.  When you
 *  run `nix build` against the dev shell for the first time:
 *
 *    1. Nix tries to fetch the source, computes the real hash, sees a
 *       mismatch with `lib.fakeHash`, and prints:
 *         "got: sha256-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx="
 *    2. Replace the matching `lib.fakeHash` in this file with that
 *       value, commit, rebuild.  The next attempt fetches successfully.
 *
 *  Repeat for both fetches (riscv-gnu-toolchain meta-repo and gcc-14).
 *
 *  ── Bumping the toolchain ─────────────────────────────────────────────
 *  - Pick a riscv-gnu-toolchain commit/tag with submodules pinned to
 *    binutils + newlib versions you want.
 *  - Pick a gcc-mirror/gcc commit on `releases/gcc-14` (or another
 *    branch if you intentionally want a different GCC line).
 *  - Update `riscv-gnu-toolchain-rev` / `gcc-rev`, restore both hashes
 *    to `lib.fakeHash`, run `nix build`, paste the new hashes back.
 */
{ lib, stdenv, fetchFromGitHub
, autoconf, automake, gawk, bison, flex, texinfo, gperf, libtool
, patchutils, bc, perl, python3, gnumake, git, makeWrapper
, gmp, mpfr, libmpc, isl, zlib, expat, file, gnused, gnutar, gzip, xz
}:

let
  # Latest stable tag of riscv-collab/riscv-gnu-toolchain near Nov 2025.
  # Bump to a newer rev when you want the upstream binutils/newlib bumps.
  riscv-gnu-toolchain-rev  = "2024.12.16";
  riscv-gnu-toolchain-hash = lib.fakeHash;

  # Branch tip of `releases/gcc-14` near Nov 2025.  The recipe uses a
  # depth=1 clone of that branch — we pick a specific commit instead so
  # builds are reproducible.
  gcc-rev  = "releases/gcc-14";    # replace with concrete SHA after first build
  gcc-hash = lib.fakeHash;

  # Multilib generator string: every soft-float ilp32 extension combo our
  # SoC could plausibly target, deliberately excluding Zcmp (Hazard3 has
  # no Zcmp).  Verbatim copy of the recipe in /opt/riscv/gcc14-no-zcmp.
  multilibGenerator =
    "rv32i-ilp32--;rv32im-ilp32--;rv32ia-ilp32--;rv32ima-ilp32--;"
  + "rv32ic-ilp32--;rv32imc-ilp32--;rv32iac-ilp32--;rv32imac-ilp32--;"
  + "rv32i_zicsr-ilp32--;rv32im_zicsr-ilp32--;rv32ia_zicsr-ilp32--;"
  + "rv32ima_zicsr-ilp32--;rv32ic_zicsr-ilp32--;rv32imc_zicsr-ilp32--;"
  + "rv32iac_zicsr-ilp32--;rv32imac_zicsr-ilp32--;"
  + "rv32i_zicsr_zifencei-ilp32--;rv32im_zicsr_zifencei-ilp32--;"
  + "rv32ia_zicsr_zifencei-ilp32--;rv32ima_zicsr_zifencei-ilp32--;"
  + "rv32ic_zicsr_zifencei-ilp32--;rv32imc_zicsr_zifencei-ilp32--;"
  + "rv32iac_zicsr_zifencei-ilp32--;rv32imac_zicsr_zifencei-ilp32--;"
  + "rv32im_zba_zbb_zbs-ilp32--;rv32ima_zba_zbb_zbs-ilp32--;"
  + "rv32imc_zba_zbb_zbs-ilp32--;rv32imac_zba_zbb_zbs-ilp32--;"
  + "rv32im_zicsr_zba_zbb_zbs-ilp32--;rv32ima_zicsr_zba_zbb_zbs-ilp32--;"
  + "rv32imc_zicsr_zba_zbb_zbs-ilp32--;rv32imac_zicsr_zba_zbb_zbs-ilp32--;"
  + "rv32im_zicsr_zifencei_zba_zbb_zbs-ilp32--;"
  + "rv32ima_zicsr_zifencei_zba_zbb_zbs-ilp32--;"
  + "rv32imc_zicsr_zifencei_zba_zbb_zbs-ilp32--;"
  + "rv32imac_zicsr_zifencei_zba_zbb_zbs-ilp32--;"
  + "rv32im_zba_zbb_zbs_zbkb-ilp32--;rv32ima_zba_zbb_zbs_zbkb-ilp32--;"
  + "rv32imc_zba_zbb_zbs_zbkb-ilp32--;rv32imac_zba_zbb_zbs_zbkb-ilp32--;"
  + "rv32im_zicsr_zba_zbb_zbs_zbkb-ilp32--;"
  + "rv32ima_zicsr_zba_zbb_zbs_zbkb-ilp32--;"
  + "rv32imc_zicsr_zba_zbb_zbs_zbkb-ilp32--;"
  + "rv32imac_zicsr_zba_zbb_zbs_zbkb-ilp32--;"
  + "rv32im_zicsr_zifencei_zba_zbb_zbs_zbkb-ilp32--;"
  + "rv32ima_zicsr_zifencei_zba_zbb_zbs_zbkb-ilp32--;"
  + "rv32imc_zicsr_zifencei_zba_zbb_zbs_zbkb-ilp32--;"
  + "rv32imac_zicsr_zifencei_zba_zbb_zbs_zbkb-ilp32--;"
  + "rv32im_zba_zbb_zbc_zbs_zbkb-ilp32--;"
  + "rv32ima_zba_zbb_zbc_zbs_zbkb-ilp32--;"
  + "rv32imc_zba_zbb_zbc_zbs_zbkb-ilp32--;"
  + "rv32imac_zba_zbb_zbc_zbs_zbkb-ilp32--;"
  + "rv32im_zicsr_zba_zbb_zbc_zbs_zbkb-ilp32--;"
  + "rv32ima_zicsr_zba_zbb_zbc_zbs_zbkb-ilp32--;"
  + "rv32imc_zicsr_zba_zbb_zbc_zbs_zbkb-ilp32--;"
  + "rv32imac_zicsr_zba_zbb_zbc_zbs_zbkb-ilp32--;"
  + "rv32im_zicsr_zifencei_zba_zbb_zbc_zbs_zbkb-ilp32--;"
  + "rv32ima_zicsr_zifencei_zba_zbb_zbc_zbs_zbkb-ilp32--;"
  + "rv32imc_zicsr_zifencei_zba_zbb_zbc_zbs_zbkb-ilp32--;"
  + "rv32imac_zicsr_zifencei_zba_zbb_zbc_zbs_zbkb-ilp32--;"
  + "rv32i_zca-ilp32--;rv32im_zca-ilp32--;rv32ia_zca-ilp32--;"
  + "rv32ima_zca-ilp32--;rv32i_zicsr_zca-ilp32--;"
  + "rv32im_zicsr_zca-ilp32--;rv32ia_zicsr_zca-ilp32--;"
  + "rv32ima_zicsr_zca-ilp32--;rv32i_zicsr_zifencei_zca-ilp32--;"
  + "rv32im_zicsr_zifencei_zca-ilp32--;rv32ia_zicsr_zifencei_zca-ilp32--;"
  + "rv32ima_zicsr_zifencei_zca-ilp32--;"
  + "rv32im_zba_zbb_zbs_zca-ilp32--;rv32ima_zba_zbb_zbs_zca-ilp32--;"
  + "rv32im_zicsr_zba_zbb_zbs_zca-ilp32--;"
  + "rv32ima_zicsr_zba_zbb_zbs_zca-ilp32--;"
  + "rv32im_zicsr_zifencei_zba_zbb_zbs_zca-ilp32--;"
  + "rv32ima_zicsr_zifencei_zba_zbb_zbs_zca-ilp32--;"
  + "rv32im_zba_zbb_zbs_zbkb_zca-ilp32--;"
  + "rv32ima_zba_zbb_zbs_zbkb_zca-ilp32--;"
  + "rv32im_zicsr_zba_zbb_zbs_zbkb_zca-ilp32--;"
  + "rv32ima_zicsr_zba_zbb_zbs_zbkb_zca-ilp32--;"
  + "rv32im_zicsr_zifencei_zba_zbb_zbs_zbkb_zca-ilp32--;"
  + "rv32ima_zicsr_zifencei_zba_zbb_zbs_zbkb_zca-ilp32--;"
  + "rv32im_zba_zbb_zbc_zbs_zbkb_zca-ilp32--;"
  + "rv32ima_zba_zbb_zbc_zbs_zbkb_zca-ilp32--;"
  + "rv32im_zicsr_zba_zbb_zbc_zbs_zbkb_zca-ilp32--;"
  + "rv32ima_zicsr_zba_zbb_zbc_zbs_zbkb_zca-ilp32--;"
  + "rv32im_zicsr_zifencei_zba_zbb_zbc_zbs_zbkb_zca-ilp32--;"
  + "rv32ima_zicsr_zifencei_zba_zbb_zbc_zbs_zbkb_zca-ilp32--;"
  + "rv32i_zca_zcb-ilp32--;rv32im_zca_zcb-ilp32--;"
  + "rv32ia_zca_zcb-ilp32--;rv32ima_zca_zcb-ilp32--;"
  + "rv32i_zicsr_zca_zcb-ilp32--;rv32im_zicsr_zca_zcb-ilp32--;"
  + "rv32ia_zicsr_zca_zcb-ilp32--;rv32ima_zicsr_zca_zcb-ilp32--;"
  + "rv32i_zicsr_zifencei_zca_zcb-ilp32--;"
  + "rv32im_zicsr_zifencei_zca_zcb-ilp32--;"
  + "rv32ia_zicsr_zifencei_zca_zcb-ilp32--;"
  + "rv32ima_zicsr_zifencei_zca_zcb-ilp32--;"
  + "rv32im_zba_zbb_zbs_zca_zcb-ilp32--;"
  + "rv32ima_zba_zbb_zbs_zca_zcb-ilp32--;"
  + "rv32im_zicsr_zba_zbb_zbs_zca_zcb-ilp32--;"
  + "rv32ima_zicsr_zba_zbb_zbs_zca_zcb-ilp32--;"
  + "rv32im_zicsr_zifencei_zba_zbb_zbs_zca_zcb-ilp32--;"
  + "rv32ima_zicsr_zifencei_zba_zbb_zbs_zca_zcb-ilp32--;"
  + "rv32im_zba_zbb_zbs_zbkb_zca_zcb-ilp32--;"
  + "rv32ima_zba_zbb_zbs_zbkb_zca_zcb-ilp32--;"
  + "rv32im_zicsr_zba_zbb_zbs_zbkb_zca_zcb-ilp32--;"
  + "rv32ima_zicsr_zba_zbb_zbs_zbkb_zca_zcb-ilp32--;"
  + "rv32im_zicsr_zifencei_zba_zbb_zbs_zbkb_zca_zcb-ilp32--;"
  + "rv32ima_zicsr_zifencei_zba_zbb_zbs_zbkb_zca_zcb-ilp32--;"
  + "rv32im_zba_zbb_zbc_zbs_zbkb_zca_zcb-ilp32--;"
  + "rv32ima_zba_zbb_zbc_zbs_zbkb_zca_zcb-ilp32--;"
  + "rv32im_zicsr_zba_zbb_zbc_zbs_zbkb_zca_zcb-ilp32--;"
  + "rv32ima_zicsr_zba_zbb_zbc_zbs_zbkb_zca_zcb-ilp32--;"
  + "rv32im_zicsr_zifencei_zba_zbb_zbc_zbs_zbkb_zca_zcb-ilp32--;"
  + "rv32ima_zicsr_zifencei_zba_zbb_zbc_zbs_zbkb_zca_zcb-ilp32--";

  riscv-gnu-toolchain-src = fetchFromGitHub {
    owner = "riscv-collab";
    repo  = "riscv-gnu-toolchain";
    rev   = riscv-gnu-toolchain-rev;
    hash  = riscv-gnu-toolchain-hash;
    fetchSubmodules = true;     # binutils, newlib, glibc, mpfr, etc.
  };

  gcc-14-src = fetchFromGitHub {
    owner = "gcc-mirror";
    repo  = "gcc";
    rev   = gcc-rev;
    hash  = gcc-hash;
  };

in
stdenv.mkDerivation {
  pname    = "riscv32-unknown-elf-gcc14-no-zcmp";
  version  = "14-${riscv-gnu-toolchain-rev}";

  # Two sources merged in unpackPhase.  The toolchain meta-repo provides
  # the configure script + binutils/newlib submodules; gcc-14-src goes
  # into a sibling directory and is referenced via --with-gcc-src.
  srcs = [ riscv-gnu-toolchain-src gcc-14-src ];
  sourceRoot = ".";

  nativeBuildInputs = [
    autoconf automake gawk bison flex texinfo gperf libtool patchutils
    bc perl python3 gnumake git makeWrapper gnused gnutar gzip xz file
  ];
  buildInputs = [ gmp mpfr libmpc isl zlib expat ];

  # Paths relative to $sourceRoot after unpack.
  unpackPhase = ''
    runHook preUnpack
    cp -r --no-preserve=mode "${riscv-gnu-toolchain-src}" riscv-gnu-toolchain
    cp -r --no-preserve=mode "${gcc-14-src}"               gcc-14
    runHook postUnpack
  '';

  configurePhase = ''
    runHook preConfigure
    cd riscv-gnu-toolchain
    ./configure \
      --prefix="$out" \
      --with-gcc-src="$NIX_BUILD_TOP/gcc-14" \
      --with-arch=rv32ia_zicsr \
      --with-abi=ilp32 \
      --with-multilib-generator='${multilibGenerator}'
    runHook postConfigure
  '';

  buildPhase = ''
    runHook preBuild
    make -j"$NIX_BUILD_CORES"
    runHook postBuild
  '';

  # The Makefile installs into $prefix.  Add `riscv32-none-elf-*` aliases
  # so the rest of the flake (which uses the `none-elf` triple in some
  # paths, mirroring nixpkgs convention) also works.
  installPhase = ''
    runHook preInstall
    # `make` already installed under $out via --prefix during build.
    for exe in "$out"/bin/riscv32-unknown-elf-*; do
      [ -e "$exe" ] || continue
      base=$(basename "$exe")
      alias=riscv32-none-elf-''${base#riscv32-unknown-elf-}
      [ -e "$out/bin/$alias" ] || ln -s "$exe" "$out/bin/$alias"
    done
    runHook postInstall
  '';

  enableParallelBuilding = true;

  # Sandbox the build: the meta-Makefile shells out to `git` to update
  # submodule state in some paths; with all submodules already present
  # via fetchSubmodules=true this should be a no-op, but disable network
  # explicitly so a missing submodule fails loudly instead of silently
  # going online.
  __noChroot = false;
  preferLocalBuild = false;

  meta = {
    description =
      "Multilib soft-float riscv32-unknown-elf cross toolchain "
      + "(GCC 14 + binutils + newlib, no Zcmp).  Matches the "
      + "/opt/riscv/gcc14-no-zcmp recipe.";
    platforms = lib.platforms.linux;
    license   = lib.licenses.gpl3Plus;
  };
}
