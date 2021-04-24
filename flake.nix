{
  description = "kibord";

  inputs = {
    qmk_firmware = {
      url = file:///git/github.com/qmk/qmk_firmware;
      ref = "master";
      rev = "bc15c4f4ab81c1e2950dfc1c38cf86dc626573c9";
      type = "git";
      flake = false;
      submodules = false;
    };
    ChibiOS = {
      url = file:///git/github.com/qmk/ChibiOS;
      ref = "master";
      type = "git";
      flake = false;
      submodules = false;
    };
    ChibiOS-Contrib = {
      url = file:///git/github.com/qmk/ChibiOS-Contrib;
      ref = "master";
      type = "git";
      flake = false;
      submodules = false;
    };
    uGFX = {
      url = file:///git/github.com/qmk/uGFX;
      ref = "master";
      type = "git";
      flake = false;
      submodules = false;
    };
    googletest = {
      url = file:///git/github.com/qmk/googletest;
      ref = "master";
      type = "git";
      flake = false;
      submodules = false;
    };
    lufa = {
      url = file:///git/github.com/qmk/lufa;
      ref = "master";
      type = "git";
      flake = false;
      submodules = false;
    };
    v-usb = {
      url = file:///git/github.com/qmk/v-usb;
      ref = "master";
      type = "git";
      flake = false;
      submodules = false;
    };
    printf = {
      url = file:///git/github.com/qmk/printf;
      ref = "master";
      type = "git";
      flake = false;
      submodules = false;
    };
    kp_boot_32u4 = {
      url = file:///git/github.com/ahtn/kp_boot_32u4;
      ref = "master";
      type = "git";
      flake = false;
    };
    hexdumpSrc = {
      url = file:///git/github.com/sarnold/hexdump;
      ref = "main";
      type = "git";
      flake = false;
    };
  };

  outputs = localFlakes@{ self, qmk_firmware, kp_boot_32u4, hexdumpSrc, ... }: {
    SobUyrld = {
      modz = [ "pkgs" ];


      lamdy =
        { self
        , kor
        , pkgsCross
        , stdenv
        , python3
        , python3Packages
        , dfu-programmer
        , dfu-util
        , diffutils
        , git
        , avrdude
        , gcc-arm-embedded
        , teensy-loader-cli
        , hidapi
        , fetchhg
        }:

        let
          inherit (builtins) concatStringsSep;
          inherit (kor) optionals mapAttrsToList;
          inherit (stdenv) mkDerivation;

          submodulesIndeks = {
            ChibiOS = "lib/chibios";
            ChibiOS-Contrib = "lib/chibios-contrib";
            uGFX = "lib/ugfx";
            googletest = "lib/googletest";
            lufa = "lib/lufa";
            v-usb = "lib/vusb";
            printf = "lib/printf";
          };

          linkSubmodule = n: v:
            let mod = localFlakes.${n};
            in "ln -s ${mod}/* ${v}";

          linkSubmodules = concatStringsSep "\n"
            (mapAttrsToList linkSubmodule submodulesIndeks);

          hjson = with python3Packages; buildPythonPackage rec {
            pname = "hjson";
            version = "3.0.1";
            src = fetchPypi {
              inherit pname version;
              sha256 = "1yaimcgz8w0ps1wk28wk9g9zdidp79d14xqqj9rjkvxalvx2f5qx";
            };
            doCheck = false;
          };

          milc = with python3Packages; buildPythonPackage rec {
            pname = "milc";
            version = "1.0.10";
            src = fetchPypi {
              inherit pname version;
              sha256 = "1q1p7qrqk78mw67nhv04zgxaq8himmdxmy2vp4fmi7chwgcbpi32";
            };
            propagatedBuildInputs = [
              appdirs
              argcomplete
              colorama
            ];
            doCheck = false;
          };

          pythonEnv = python3.withPackages (p: with p; [
            # requirements.txt
            appdirs
            argcomplete
            colorama
            hjson
            jsonschema
            milc
            pygments
            # requirements-dev.txt
            nose2
            flake8
            pep8-naming
            yapf
          ]);

          avrlibc = pkgsCross.avr.libcCross;
          avr_incflags = [
            "-isystem ${avrlibc}/avr/include"
            "-B${avrlibc}/avr/lib/avr5"
            "-L${avrlibc}/avr/lib/avr5"
            "-B${avrlibc}/avr/lib/avr35"
            "-L${avrlibc}/avr/lib/avr35"
            "-B${avrlibc}/avr/lib/avr51"
            "-L${avrlibc}/avr/lib/avr51"
          ];

          avrPackages = [
            avrdude
            pkgsCross.avr.buildPackages.gcc
            pkgsCross.avr.buildPackages.binutils
            avrlibc
          ];

          mkQmkOS = { kibord, avr ? false, arm ? false, teensy ? false }:
            let
              inherit (kibord) iuniksDir keyboardModel;
              keymap = builtins.baseNameOf iuniksDir;

            in
            mkDerivation {
              pname = "qmk-os";
              version = qmk_firmware.shortRev;
              src = qmk_firmware;

              buildInputs = [ dfu-programmer dfu-util diffutils git pythonEnv ]
                ++ optionals avr avrPackages
                ++ optionals arm [ gcc-arm-embedded ]
                ++ optionals teensy [ teensy-loader-cli ];

              postPatch = ''
                ${linkSubmodules}
                ln -s ${iuniksDir} ./keyboards/${keyboardModel}/keymaps/${keymap}
              '';

              CFLAGS = optionals avr avr_incflags;
              ASFLAGS = optionals avr avr_incflags;

              buildPhase = ''
                make ${keyboardModel}:${keymap}
              '';

              installPhase = ''
                mkdir -p $out
                cp ./.build/*hex $out/
              '';
            };

          inherit (python3Packages) buildPythonApplication
            buildPythonPackage fetchPypi intelhex cffi;

          hexdump = buildPythonPackage {
            pname = "hexdump";
            version = hexdumpSrc.shortRev;
            src = hexdumpSrc;
            doCheck = false;
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
            buildInputs = [ hidapi cffi ];
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
          };

          kpBootloader = mkDerivation {
            name = "kp-bootloader";
            version = kp_boot_32u4.shortRev;
            src = kp_boot_32u4;
            buildInputs = kpBootPackages # May not be needed
              ++ avrPackages;
          };

        in
        {
          inherit kpBootCli kpBootloader;

          lisErgodone =
            let
            in
            mkQmkOS {
              avr = true;
              kibord = {
                iuniksDir = ./ergodone/coleremak;
                keyboardModel = "ergodone";
              };
            };

        };

    };

  };
}
