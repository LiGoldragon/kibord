# Kibord

Personal QMK keymaps built with Nix. The MiniDox target is intentionally a build-only source update; do not flash from this repository until the physical keyboard's bootloader, half selection, and reset procedure are identified.

## MiniDox Build

The MiniDox source lives at `maple_computing/minidox/LiGoldragon` and builds QMK target `maple_computing/minidox/rev1` from upstream QMK `0.33.8`.

Build the firmware artifact:

```sh
nix build .#minidox
```

The build output is a hex artifact under `result/`, normally named `maple_computing_minidox_rev1_LiGoldragon.hex`.

The base layer intentionally emits Colemak letter keycodes from the firmware. A raw QWERTY host should receive `qwfpgjluy;` from the physical top row. Do not additionally Colemak-remap the MiniDox in the operating system; any OS-side Colemak treatment should be applied separately and per-device for the laptop keyboard.

The number row is on `RAISE` and keeps the existing number-key physical positions. Its outputs are ordered in the conventional left-to-right direction: `1` starts on the left pinky side and `0` ends on the right pinky side.

## MiniDox Bootloader Entry

This keymap enables QMK Leader support for a deliberate bootloader-entry sequence before any future reflashing:

- Hold `LOWER` and `RAISE` together to enter `ADJUST`, tap the left home-row pinky `QK_LEAD` key, release the layer keys, then type `r e s e t` on the base layer. The sequence calls QMK's bootloader reset path.
- As a tucked-away fallback matching the upstream MiniDox default's reset placement, `ADJUST` also has a direct `QK_BOOT` key on the left bottom-row pinky position.

## Pre-Flash Safety

Flashing is out of scope until the hardware facts are known. Before any flash attempt:

- Identify each half's controller and bootloader without intentionally entering bootloader mode during ordinary typing work.
- Confirm which half is connected over USB and how handedness is selected.
- Flash one half at a time only after the matching bootloader and artifact are known.
- Disconnect USB power before plugging or unplugging the TRRS cable. Do not hot-plug TRRS while either half is powered.
- Keep the adjacent `qmkBinaries` MiniDox hex files as reference artifacts only; they are not a documented recovery procedure.
- If the controller is a Pro Micro or compatible Caterina board, QMK documents `QK_BOOT`, a PCB reset button, or briefly shorting `RST` to `GND` as bootloader-entry methods; some variants may require two resets within 750 ms. This is an inferred fallback for this MiniDox and has not been verified on the actual hardware.
