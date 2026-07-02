{
  description = "kibord";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    qmk_firmware = {
      url = "github:qmk/qmk_firmware/0.33.8";
      flake = false;
    };
    ChibiOS = {
      url = "github:qmk/ChibiOS";
      flake = false;
    };
    ChibiOS-Contrib = {
      url = "github:qmk/ChibiOS-Contrib";
      flake = false;
    };
    googletest = {
      url = "github:qmk/googletest";
      flake = false;
    };
    lufa = {
      url = "github:qmk/lufa";
      flake = false;
    };
    v-usb = {
      url = "github:qmk/v-usb";
      flake = false;
    };
    printf = {
      url = "github:qmk/printf";
      flake = false;
    };
    kp_boot_32u4 = {
      url = "github:ahtn/kp_boot_32u4";
      flake = false;
    };
    hexdumpSrc = {
      url = "github:sarnold/hexdump";
      flake = false;
    };
  };

  outputs =
    localFlakes@{
      self,
      nixpkgs,
      qmk_firmware,
      kp_boot_32u4,
      hexdumpSrc,
      ...
    }:
    let
      supportedSystems = [
        "x86_64-linux"
        "aarch64-linux"
      ];

      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;

      subWorld = {
        mods = [ "pkgs" ];

        lambda =
          with builtins;
          {
            src,
            lib,
            pkgsCross,
            stdenv,
            python3,
            python3Packages,
            dfu-programmer,
            dfu-util,
            diffutils,
            git,
            avrdude,
            gcc-arm-embedded,
            teensy-loader-cli,
            hidapi,
            fetchhg,
            qmk,
          }:

          let
            inherit (lib) optionals mapAttrsToList optional;
            inherit (stdenv) mkDerivation;

            inherit (python3Packages)
              buildPythonApplication
              buildPythonPackage
              fetchPypi
              intelhex
              cffi
              ;

            submodulesIndeks = {
              ChibiOS = "lib/chibios";
              ChibiOS-Contrib = "lib/chibios-contrib";
              googletest = "lib/googletest";
              lufa = "lib/lufa";
              v-usb = "lib/vusb";
              printf = "lib/printf";
            };

            linkSubmodule =
              n: v:
              let
                mod = localFlakes.${n};
              in
              "ln -s ${mod}/* ${v}";

            linkSubmodules = concatStringsSep "\n" (mapAttrsToList linkSubmodule submodulesIndeks);

            pythonEnv = python3.withPackages (
              p: with p; [
                # requirements.txt
                appdirs
                argcomplete
                colorama
                dotty-dict
                hid
                hjson
                jsonschema
                milc
                pillow
                pygments
                pyserial
                pyusb
                platformdirs
                # requirements-dev.txt
                nose2
                flake8
                pep8-naming
                yapf
              ]
            );

            avrlibc = pkgsCross.avr.libc;
            avr_incflags = [
              "-isystem ${avrlibc}/avr/include"
              "-B${avrlibc}/avr/lib/avr5"
              "-L${avrlibc}/avr/lib/avr5"
              "-B${avrlibc}/avr/lib/avr35"
              "-L${avrlibc}/avr/lib/avr35"
              "-B${avrlibc}/avr/lib/avr51"
              "-L${avrlibc}/avr/lib/avr51"
            ];

            bypassGccBug = true;
            disableArrayBoundsFlag = "-Wno-array-bounds";

            avrPackages = [
              avrdude
              pkgsCross.avr.buildPackages.gcc
              pkgsCross.avr.buildPackages.binutils
              avrlibc
            ];

            mkQmkOS =
              {
                kibord,
                avr ? false,
                arm ? false,
                teensy ? false,
              }:
              let
                inherit (kibord) iuniksDir keyboardModel;
                keymapKeyboardModel = kibord.keymapKeyboardModel or keyboardModel;
                keymap = builtins.baseNameOf iuniksDir;

              in
              mkDerivation {
                pname = "qmk-os";
                version = qmk_firmware.shortRev;
                src = qmk_firmware;

                buildInputs = [
                  dfu-programmer
                  dfu-util
                  diffutils
                  git
                  qmk
                  pythonEnv
                ]
                ++ optionals avr avrPackages
                ++ optionals arm [ gcc-arm-embedded ]
                ++ optionals teensy [ teensy-loader-cli ];

                postPatch = ''
                  ${linkSubmodules}
                  mkdir -p ./keyboards/${keymapKeyboardModel}/keymaps
                  ln -s ${iuniksDir} ./keyboards/${keymapKeyboardModel}/keymaps/${keymap}
                '';

                CFLAGS = optionals avr avr_incflags ++ (optional bypassGccBug disableArrayBoundsFlag);
                ASFLAGS = optionals avr avr_incflags;

                buildPhase = ''
                  make ${keyboardModel}:${keymap}
                '';

                installPhase = ''
                  mkdir -p $out
                  cp ./.build/*hex $out/
                '';
              };

            hexdump = buildPythonPackage {
              pname = "hexdump";
              version = hexdumpSrc.shortRev;
              src = hexdumpSrc;
              doCheck = false;
              pyproject = true;
              build-system = [ python3Packages.setuptools ];
            };

            easyhid = buildPythonPackage rec {
              pname = "easyhid";
              version = "0.0.9";
              src = fetchPypi {
                inherit pname version;
                sha256 = "68bfed780bb0c2cebebfc1e977093985b0ccb512ad20b8bb030d73b525f3eaf2";
              };
              postPatch = ''
                substituteInPlace easyhid/easyhid.py \
                  --replace "ctypes.util.find_library('hidapi-libusb')" \
                  "'${hidapi}/lib/libhidapi-libusb.so.0'"
              '';
              buildInputs = [
                hidapi
                cffi
              ];
              pyproject = true;
              build-system = [ python3Packages.setuptools ];
            };

            kpBootPackages = [
              intelhex
              cffi
              hexdump
              easyhid
              hidapi
            ];

            kpBootCli = buildPythonApplication {
              name = "kp-boot-cli";
              version = kp_boot_32u4.shortRev;
              src = kp_boot_32u4;
              propagatedBuildInputs = kpBootPackages;
              pyproject = true;
              build-system = [ python3Packages.setuptools ];
            };

            kpBootloader = mkDerivation {
              name = "kp-bootloader";
              version = kp_boot_32u4.shortRev;
              src = kp_boot_32u4;
              buildInputs =
                kpBootPackages # May not be needed
                ++ avrPackages;
            };

          in
          {
            inherit kpBootCli kpBootloader;

            LiGoldragon = {
              minidox = mkQmkOS {
                avr = true;
                kibord = {
                  iuniksDir = ./maple_computing/minidox/LiGoldragon;
                  keyboardModel = "maple_computing/minidox/rev1";
                  keymapKeyboardModel = "maple_computing/minidox";
                };
              };
              ergodone = mkQmkOS {
                avr = true;
                kibord = {
                  iuniksDir = ./ergodone/coleremak;
                  keyboardModel = "ergodone";
                };
              };
            };

          };

      };

      buildFor =
        system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        subWorld.lambda {
          src = self;
          inherit (pkgs)
            lib
            stdenv
            python3
            python3Packages
            dfu-programmer
            dfu-util
            diffutils
            git
            avrdude
            gcc-arm-embedded
            teensy-loader-cli
            hidapi
            fetchhg
            qmk
            ;
          pkgsCross = pkgs.pkgsCross;
        };
    in
    {
      SubWorld = subWorld;

      packages = forAllSystems (
        system:
        let
          build = buildFor system;
        in
        {
          default = build.LiGoldragon.minidox;
          minidox = build.LiGoldragon.minidox;
          ergodone = build.LiGoldragon.ergodone;
          inherit (build) kpBootCli kpBootloader;
        }
      );

      checks = forAllSystems (system: {
        minidox = self.packages.${system}.minidox;
      });
    };
}
