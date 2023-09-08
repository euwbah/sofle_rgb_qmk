# euwbah's keymap for the Sofle RGB v2.1

Features:

- Host LED indicators:
  - Caps Lock
  - Num Lock (when Nav layer/numpad is active)
- aUtOmAtEd SpOnGeBoB cAsE (Nav + press right encoder to toggle)
- Rotary encoder double signal per detent correction
- Backspace is where CapsLock is (as per official Colemak layout)
- Ctrl+Backspace (backspace word shortcut) can be accessed with pinky pressing both keys at once, order doesn't matter.
- CapsLock is accessed with Alt+Backspace
- Left rotary encoder:
  - volume control, mute on click
  - When alt/ctrl is held down, does alt+tab/alt+shift+tab/ctrl+tab/ctrl+shift+tab.
  - Lower layer: brightness control.
  - Raise layer: external monitor brightness control (requires [HID Info Sender](https://github.com/euwbah/hid-info-sender))
  - Nav layer: external monitor contrast control (requires [HID Info Sender](https://github.com/euwbah/hid-info-sender))
- Right rotary encoder:
  - left/right arrow keys
  - Lower layer: up/down arrow keys
  - Raise layer: mouse scroll
  - Nav layer: Ctrl+Shift+left/right
- Shift is on the left thumb.
- Parentheses, grave/backtick, underscore are accessible from the base layer.
- Navigation layer, with arrow keys, home, end, Print Screen on the right, and num pad on the left.
- Raise layer: numbers, symbols, function, and language keys
- Lower layer:
  - special function macros function keys (Alt+F4, Shift+F6, Alt+F12)
  - Ctrl+Shift+Esc (task manager), Ctrl+Alt+Del, Print Screen
  - volume & brightness keys
  - Ctrl+Alt+V (IntelliJ "extract variable" keybinding)
  - Other non-macro function keys 1 to 24
  - Switch desktop
  - one-handed QK_BOOT for quick bootloader access on one half
  - Mouse keys
- Modified RGB controls will display on left OLED.
- WPM displayed on left OLED.
- Hardware utilization stats displayed on right OLED (Requires setting up & running [HID Info Sender](https://github.com/euwbah/hid-info-sender))

Setup:

- `qmk compile -kb sofle -km euwbah`
- Setup the [HID Info Sender](https://github.com/euwbah/hid-info-sender) to get the secondary OLED to display hardware utilization stats.
