# zmk-keyboard-demetra_v2

[ZMK](https://zmk.dev) hardware module for **Demetra v2** — a 34-key (3×5+2)
wireless split ergonomic keyboard built around the nRF52840 (u-blox/Rigado
BMD-340 module), with USB and Bluetooth LE.

## Board targets

| Half  | Board target               | Notes                          |
|-------|----------------------------|--------------------------------|
| Left  | `demetra_v2_left`          | Central: USB + BLE, ZMK Studio |
| Right | `demetra_v2_right`         | Peripheral                     |

Requires ZMK with hardware model v2 (ZMK `main` / v0.4+, Zephyr 4.x).

Features: USB HID, BLE (up to 7 profiles), split over BLE,
[ZMK Studio](https://zmk.dev/docs/features/studio) support (physical layout
included), UF2 drag-and-drop flashing via the pre-installed Adafruit nRF52
bootloader (double-tap reset → `DEMETRA_V2` USB drive).

## Building your own firmware

The easiest path is the user config template — no toolchain needed, firmware
is built by GitHub Actions:

**→ [fractalysid/zmk-config-demetra_v2](https://github.com/fractalysid/zmk-config-demetra_v2)**

To reference this module from any ZMK user config, add it to
`config/west.yml`:

```yaml
manifest:
  remotes:
    - name: zmkfirmware
      url-base: https://github.com/zmkfirmware
    - name: fractalysid
      url-base: https://github.com/fractalysid
  projects:
    - name: zmk
      remote: zmkfirmware
      revision: main
      import: app/west.yml
    - name: zmk-keyboard-demetra_v2
      remote: fractalysid
      revision: main
  self:
    path: config
```

## Flashing

1. Double-tap the reset button: the `DEMETRA_V2` USB drive appears.
2. Drag the `.uf2` file onto the drive. The keyboard reboots into the new
   firmware. Flash each half with its own image (left/right).

## Repository layout

- `boards/fractalysid/demetra_v2/` — the board definition (ZMK HWMv2):
  devicetree, physical layout, Kconfig, metadata.
- `hwm1/` — legacy hardware-model-v1 board for ZMK v0.3, used internally for
  development; not needed by users.

## License

[MIT](LICENSE)
