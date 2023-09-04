# euwbah's keymap for the Sofle RGB v2.1

Features:

- Left half is the primary half.
- Rotary encoder double signal per detent correction
- Navigation layer, with arrow keys, home, end, Print Screen on the right, and num pad on the left.
- Raise layer accesses all numbers, symbols, and function keys.
- Lower layer accesses special function macros (Alt+F4, Ctrl+Shift+Esc, Ctrl+Alt+Del, Shift+F6, volume, brightness, Function keys 1 to 24, mouse scroll, one-handed QK_BOOT for quick bootloader access on one half)
- Adjust layer accesses RGB controls and desktop switch shortcut.
- Modified RGB controls will display on left OLED.
- WPM displayed on left OLED.
- Hardware utilization stats displayed on right OLED (Requires setting up & running [HID Info Sender](https://github.com/euwbah/hid-info-sender))

Setup:

- `qmk compile -kb sofle -km euwbah`
- Setup the [HID Info Sender](https://github.com/euwbah/hid-info-sender) to get the secondary OLED to display hardware utilization stats.
