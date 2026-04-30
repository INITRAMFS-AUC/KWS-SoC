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
{ lib, stdenv, fetchFromGitHub, fetchgit
, autoconf, automake, gawk, bison, flex, texinfo, gperf, libtool
, patchutils, bc, perl, python3, gnumake, git, makeWrapper
, gmp, mpfr, libmpc, isl, zlib, expat, file, gnused, gnutar, gzip, xz
}:

let
  # Pin every component the local recipe pulls.  When the upstream
  # `make` runs `git submodule update --init` against the meta-repo,
  # it checks out exactly the commits the meta-repo's gitlinks point
  # at.  We replicate that here component-by-component so the build is
  # reproducible AND identical-by-content to the recipe in
  # /opt/riscv/gcc14-no-zcmp.

  # 1. The meta-repo itself, fetched WITHOUT submodules.  Its tracked
  #    files are just the configure script + Makefile.in + helper
  #    scripts.  Submodule directories show up as empty stubs, which
  #    we then populate from the per-component fetches below.
  riscv-gnu-toolchain-rev  = "2024.12.16";  # tag, == 43536ac
  riscv-gnu-toolchain-hash = "sha256-FZE7DIW+aP5mAmmWdgMXohOhMLngQrG2zoyF+zV97+A=";

  # 2. gcc-mirror/gcc on releases/gcc-14, pinned to a SHA so we don't
  #    drift with branch tip movement.  This is the equivalent of
  #    `git clone --depth=1 ... -b releases/gcc-14 gcc-14` in the
  #    local recipe.  Re-prefetch with:
  #      nix-prefetch-git --url https://github.com/gcc-mirror/gcc \
  #                       --rev releases/gcc-14
  gcc-rev  = "ed10445fe222d3973ae13eda9bf211f315c5e3f9";   # 2024-05-07
  gcc-hash = "sha256-lA/i5ceWaZhw+RzRqDZ0ch+qxomYjjEhiEovxOEJfpM=";

  # 3. binutils-gdb at the SHA the meta-repo's `binutils` submodule
  #    pins (read with `git ls-tree HEAD binutils` against the meta).
  #    Pulled directly from sourceware — the same upstream URL the
  #    meta-repo's .gitmodules declares.
  binutils-rev  = "beb2cdbcda911764b2bed5e57921fe90493260bd";  # 2024-08-17
  binutils-hash = "sha256-p/zvtCd3uzZgn4DTAkG/SxSrk3U7TLA3Vy4lI0a/5OY=";

  # 4. newlib-cygwin at the meta-repo's `newlib` submodule pin.
  newlib-rev  = "26f7004bf73c421c3fd5e5a6ccf470d05337b435";  # 2023-12-31
  newlib-hash = "sha256-6jaggRYn2WH/aWCQsxzC15y5aYyBPBDkqN7C16u63ac=";

  # We deliberately skip dejagnu (testsuite only — we don't run tests),
  # gdb (we use riscv-openocd), glibc / musl / uclibc-ng (we build
  # `riscv32-unknown-elf` with newlib, no full libc needed), llvm
  # (we use GCC), pk (no proxy kernel), qemu (sim done in Verilator),
  # spike (sim done in Verilator), uclibc-ng.  The Makefile picks
  # which submodules to actually use based on configure flags; with
  # --with-newlib + no --enable-* for the rest, it never looks at
  # those directories.

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
    owner = "riscv-collab";  # canonical, github.com/riscv/... 301-redirects here
    repo  = "riscv-gnu-toolchain";
    rev   = riscv-gnu-toolchain-rev;
    hash  = riscv-gnu-toolchain-hash;
    fetchSubmodules = false;   # we populate submodule dirs manually below
  };

  gcc-14-src = fetchFromGitHub {
    owner = "gcc-mirror";
    repo  = "gcc";
    rev   = gcc-rev;
    hash  = gcc-hash;
  };

  binutils-src = fetchgit {
    url    = "https://sourceware.org/git/binutils-gdb.git";
    rev    = binutils-rev;
    hash   = binutils-hash;
    fetchSubmodules = false;
  };

  newlib-src = fetchgit {
    url    = "https://sourceware.org/git/newlib-cygwin.git";
    rev    = newlib-rev;
    hash   = newlib-hash;
    fetchSubmodules = false;
  };

in
stdenv.mkDerivation {
  pname    = "riscv32-unknown-elf-gcc14-no-zcmp";
  version  = "14-${riscv-gnu-toolchain-rev}";

  # Four sources, assembled in unpackPhase to mirror the local recipe:
  #   riscv-gnu-toolchain/        ← meta-repo (configure + Makefile.in)
  #   riscv-gnu-toolchain/binutils ← populate the empty submodule dir
  #   riscv-gnu-toolchain/newlib   ← populate the empty submodule dir
  #   gcc-14/                     ← sibling, referenced via --with-gcc-src
  srcs = [
    riscv-gnu-toolchain-src
    gcc-14-src
    binutils-src
    newlib-src
  ];
  sourceRoot = ".";

  nativeBuildInputs = [
    autoconf automake gawk bison flex texinfo gperf libtool patchutils
    bc perl python3 gnumake git makeWrapper gnused gnutar gzip xz file
  ];
  buildInputs = [ gmp mpfr libmpc isl zlib expat ];

  # `cp -r` preserves the +x bit on configure / install-sh / etc.;
  # `chmod -R u+w` makes the tree writable since Nix-store files are
  # mode 444 by default.  We overlay binutils + newlib INSIDE the
  # meta-repo's submodule paths so the meta-repo's Makefile finds them
  # at exactly the locations its build steps expect — same shape as
  # `git submodule update --init` would produce in the local recipe.
  unpackPhase = ''
    runHook preUnpack
    cp -r "${riscv-gnu-toolchain-src}" riscv-gnu-toolchain
    cp -r "${gcc-14-src}"              gcc-14
    chmod -R u+w riscv-gnu-toolchain gcc-14

    # Replace the empty `binutils` / `newlib` submodule stubs with the
    # real (pre-fetched, hash-pinned) sources.  rmdir first so the
    # cp doesn't end up nesting `binutils/binutils-gdb/`.
    rmdir riscv-gnu-toolchain/binutils
    cp -r "${binutils-src}" riscv-gnu-toolchain/binutils
    chmod -R u+w riscv-gnu-toolchain/binutils

    rmdir riscv-gnu-toolchain/newlib
    cp -r "${newlib-src}" riscv-gnu-toolchain/newlib
    chmod -R u+w riscv-gnu-toolchain/newlib
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
