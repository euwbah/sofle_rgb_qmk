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
  - Lower layer: laptop brightness control, permanent NAV/Numpad layer (press Esc to exit).
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
  - Macros:
    - Backspace = Alt+F4: close window
    - A = Ctrl+Shift+Esc: task manager
    - S = Ctrl+Alt+Del
    - D = Print Screen
    - Ctrl = Ctrl+Shift+P: VSCode/Pulsar/Atom open command palette
    - Z = Ctrl+Alt+J: VSCode SyncTex scroll pdf view to cursor position.
    - X = Alt+F12: Open terminal vscode
    - C = Alt+J: IntelliJ select word/next occurrences of selection.
    - V = Shift+F6: IntelliJ refactor rename identifiers
    - B = Ctrl+Alt+V: IntelliJ "extract variable" keybinding
    - Ctrl+Win+Left/Right: Switch desktop
  - volume & brightness keys
  - Other non-macro function keys 1 to 24
  - one-handed QK_BOOT for quick bootloader access on one half
  - Mouse keys
- Left OLED displays adjusted settings (caps lock, num lock, RGB HSV, external monitor brightness, etc...)
- WPM displayed on left OLED.
- Hardware utilization stats displayed on right OLED (Requires setting up & running [HID Info Sender](https://github.com/euwbah/hid-info-sender))
- Date & Time ([HID Info Sender](https://github.com/euwbah/hid-info-sender))

Setup:

- `qmk compile -kb sofle -km euwbah`
- Setup the [HID Info Sender](https://github.com/euwbah/hid-info-sender) to get the secondary OLED to display hardware utilization stats.
