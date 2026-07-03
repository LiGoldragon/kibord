# Kibord

Personal QMK keymaps built with Nix. The MiniDox target is intentionally in a build, review, then explicit-approval cycle; do not flash from this repository until the reconstructed layout, physical keyboard bootloader, half selection, and reset procedure are reviewed and approved.

## MiniDox Recovery Status

The MiniDox source lives at `maple_computing/minidox/LiGoldragon` and builds QMK target `maple_computing/minidox/rev1` from upstream QMK `0.33.8`.

This keymap was reconstructed from the old MiniDox binary evidence, not from the previously current source. Evidence used:

- `/home/li/primary/agent-outputs/MiniDoxBinaryRecoveryAvr/GeneralCodeImplementer-RecoveryReport.md`
- `/home/li/primary/agent-outputs/MiniDoxBinaryRecoveryAvr/right.bin`
- `/home/li/primary/agent-outputs/MiniDoxBinaryRecoveryAvr/left.bin`
- the `qmkBinaries` MiniDox artifacts at commit `f6e2093debe57ee15acedc88b3dd772cf4071fb5`

The reconstructed source keeps a firmware-Colemak `BASE` layer. A raw QWERTY host should receive `qwfpgjluy;` from the physical top row. Do not additionally Colemak-remap the MiniDox in the operating system; any OS-side Colemak treatment should be applied separately and per-device for the laptop keyboard.

The first clean MiniDox-native iteration keeps the recovered behavior as the starting point, but presents each layer as a physical MiniDox diagram in `keymap.c`, following the pinned upstream MiniDox default keymap's visual style. The diagrams are the review surface: each layer is shown as left hand, right hand, and two thumb clusters before its `LAYOUT_split_3x5_3(...)` block.

The `NUMBERS` layer keeps the same physical number positions recovered from the old layout, but the number taps now read left-to-right as `1 2 3 4 5 6 7 8 9 0`. Positions that already carried modifier-taps still carry their modifier-tap wrappers; only the tapped number output was reordered.

Confidence:

- High: top-row base output `Q W F P G / J L U Y ;`, the left artifact's layer 3 resembling the right artifact's layer 1 `NUMBERS` layer, `QK_LEAD`, `QK_BOOT`, and leader sequence `r e s e t` entering QMK's reset path.
- Medium: `SYMBOLS`, `FUNCTIONS`, and the retained non-default `QWERTY_RECOVERED` layer from the longer left artifact.
- Known unknown: exact physical placement for some home/bottom row positions because the current QMK `LAYOUT_split_3x5_3` macro may not match the old build source's matrix-to-physical assumptions.
- Known unknown: recovered `QK_USER_0`, `QK_USER_1`, and `QK_USER_3` positions are preserved as inert historical placeholders because their stripped `process_record_user` behavior was not proven.

## MiniDox Build

Build the firmware artifact:

```sh
nix build .#minidox
```

The build output is a hex artifact under `result/`, normally named `maple_computing_minidox_rev1_LiGoldragon.hex`.

First-cycle workflow:

1. Build with `nix build .#minidox`.
2. Review `maple_computing/minidox/LiGoldragon/keymap.c` diagrams against the physical MiniDox.
3. Compare the `result/` hex artifact only after the source layout is accepted.
4. Flash only after explicit approval for the target half, bootloader, handedness, and recovery procedure.

## MiniDox Bootloader Entry

This keymap enables QMK Leader support for deliberate bootloader and one-shot modifier sequences:

- Tap `QK_LEAD` on the base thumb row, then type `r e s e t` on the base layer. The sequence calls QMK's bootloader reset path.
- Tap `QK_LEAD`, then `s` for one-shot Shift.
- Tap `QK_LEAD`, then `c` for one-shot Ctrl.
- Tap `QK_LEAD`, then `a` for one-shot Alt.
- Tap `QK_LEAD`, then `g` for one-shot Gui.
- The `FUNCTIONS` layer also keeps tucked-away direct `QK_BOOT` fallback positions recovered from the left artifact.
- Entering `SYMBOLS` and `NUMBERS` together activates `FUNCTIONS` through QMK tri-layer handling.

## No-Flash Safety

Flashing is out of scope for this repository state. Before any flash attempt:

- Review the reconstructed keymap against the binary evidence and the actual physical MiniDox.
- Identify each half's controller and bootloader without intentionally entering bootloader mode during ordinary typing work.
- Confirm which half is connected over USB and how handedness is selected.
- Flash one half at a time only after the matching bootloader and artifact are known.
- Disconnect USB power before plugging or unplugging the TRRS cable. Do not hot-plug TRRS while either half is powered.
- Keep the adjacent `qmkBinaries` MiniDox hex files as reference artifacts only; they are not a documented recovery procedure.
- If the controller is a Pro Micro or compatible Caterina board, QMK documents `QK_BOOT`, a PCB reset button, or briefly shorting `RST` to `GND` as bootloader-entry methods; some variants may require two resets within 750 ms. This is an inferred fallback for this MiniDox and has not been verified on the actual hardware.
