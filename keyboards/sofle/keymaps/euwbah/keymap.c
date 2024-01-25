
 /* Copyright 2023 euwbah
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  */
  // SOFLE RGB

// This include is required for using rpc transactions between master and slave
#include "./keymap_headers.h"

#include QMK_KEYBOARD_H

// in led update frames
#define OLED_INFO_DISPLAY_DURATION 50

typedef enum _oled_display_mode_enum {
    // Display RGB settings when modified
    OLED_DISPLAY_HUE = 0,
    OLED_DISPLAY_SAT,
    OLED_DISPLAY_VAL,
    OLED_DISPLAY_SPD,
    // Display lock key status when modified
    OLED_DISPLAY_CAPS_LOCK,
    OLED_DISPLAY_SCROLL_LOCK,
    OLED_DISPLAY_NUM_LOCK,
    OLED_DISPLAY_KANA,
    OLED_DISPLAY_SPONGEBOB,
    // Display oled settings when modified.
    OLED_DISPLAY_OLED_BRIGHTNESS,
    OLED_DISPLAY_EXT_MONITOR_BRIGHTNESS,
    OLED_DISPLAY_EXT_MONITOR_CONTRAST,
    // Display numrow layer when changed
    OLED_DISPLAY_NUMROW_LAYER,
} oled_display_mode_enum;

typedef enum _nav_state_enum {
    NAV_STATE_MO = 0, // Standard momentary nav layer
    NAV_STATE_HOLD = 1, // Perma nav layer (numpad on left)
} nav_state_enum;

// for storing mouse movement state.
typedef union {
    uint8_t _raw;
    struct {
        bool up : 1;
        bool down : 1;
        bool left : 1;
        bool right : 1;
    };
} mousekeys_status_t;

typedef union {
    uint32_t _raw;
    struct {
        uint8_t month : 4;
        uint8_t date: 5;
        uint8_t day_of_week : 3;
        uint8_t hour : 5;
        uint8_t minute : 6;
        uint8_t second : 6;
    };
} datetime_t; // 4 bytes

typedef union {
    uint16_t _raw;
    struct {
        uint8_t ext_monitor_brightness : 7;
        uint8_t ext_monitor_contrast : 7;
    };
} user_config_t;

// Sent using USER_SYNC_DATA transaction ID.
// Don't exceed RPC_M2S_BUFFER_SIZE bytes (see config.h)
typedef struct _m_to_s_data {
    // DO NOT CHANGE THE ORDER/ADD THINGS BEFORE THE FIRST 7 DEVICE UTIL STATS
    // There's memory hacks!!! Don't break it.
    uint8_t cpu_usage;                  // 1 byte
    uint8_t cpu_temp;                   // 2
    uint8_t ram_usage;                  // 3
    uint8_t gpu_usage;                  // 4
    uint8_t gpu_mem_usage;              // 5
    uint8_t gpu_temp;                   // 6
    uint8_t gpu1_usage;                 // 7

    // You can add other stuff here.
    bool sPoNgEbOb_case_active;         // 8
    datetime_t datetime;                // 12
    uint32_t last_keypress_timer;       // 16
    uint8_t oled_brightness;            // 17
} m_to_s_data_t;

// IMPORTANT: if you change the order of the layers here, you will need to modify
// the `names_of_layers` list!
enum sofle_layers {
    _DEFAULTS = 0,
    _QWERTY = 0,
    // _COLEMAK,
    _COLEMAKDH,
    _GENSHIN,
    _LOWER,
    _RAISE,
    _ADJUST,
    _NAV,
    _NAVSWAP,
};

enum custom_keycodes {
    KC_QWERTY = SAFE_RANGE,
    KC_GENSHIN,
    KC_COLEMAKDH,
    KC_LOWER,
    KC_RAISE,
    KC_ADJUST,
    KC_NAV,
    LAYCLR, // Clear layer state
    ENTCLR, // Enter + clear layer state
    sPgBoB, // toggle SpOnGeBoB case
    OLED_B_UP, // Increase oled brightness
    OLED_B_DN, // Decrease oled brightness
    MS_BTN1, MS_BTN2, MS_BTN3, MS_BTN4, MS_BTN5, // Custom implemented mouse buttons
    MS_U, MS_D, MS_L, MS_R, // Custom implemented mouse movements
    EXMON1, EXMON2, EXMON3, // Preset external monitor brightness settings
    DREFACC, // C deref access ->
    SCOPE, // Rust/C++ scope operator ::

    // Numrow special keys on the base layer. They can be mapped to different keys
    // separate from the layer.
    // IMPORTANT: These 12 custom keys must be placed last, continguously.
    NR0, NR1, NR2, NR3, NR4, NR5,
    NR6, NR7, NR8, NR9, NR10, NR11,
};

#define TONAV TO(_NAV) // Turn on NAV layer permanently
#define CLSWIN A(KC_F4)
#define TSKMGR C(S(KC_ESC))
#define CTALDEL C(A(KC_DEL))
#define VSCMDS C(S(KC_P))
#define LTSYNC C(A(KC_J)) // latex viewer pdf scroll sync
#define VSTERM A(KC_F12) // VSCode open terminal
#define IJNXT A(KC_J) // IntelliJ add next occurrence to selection
#define IJRENM S(KC_F6) // IntelliJ rename
#define IJEXTR C(A(KC_V)) // IntelliJ extract variable
#define DELBRC C(A(KC_BSPC)) // VS Remove brackets
#define SELBRC C(A(KC_RBRC)) // VS Select brackets
#define GOTOBRC C(S(KC_BSLS)) // VS Go to bracket
#define MODMEH C(A(KC_LSFT)) // Control + Alt + Shift
#define HEAD1 C(A(KC_1)) // Heading 1
#define HEAD2 C(A(KC_2)) // Heading 2
#define HEAD3 C(A(KC_3)) // Heading 3
#define HEAD4 C(A(KC_4)) // Heading 4
#define NORMTXT C(A(KC_0)) // Normal text
#define GDSTRK A(S(KC_5)) // Google doc/slide strikethrough
#define GDCMT C(A(KC_M)) // Google doc/slide comment
#define GDCMD A(KC_SLSH) // Google Docs open command palette.
#define WRDCNT C(S(KC_C)) // Google Docs word count
#define LALGN C(S(KC_L)) // Google Docs left align
#define CALGN C(S(KC_E)) // Google Docs center align
#define RALGN C(S(KC_R)) // Google Docs right align
#define INCFNT C(S(KC_DOT)) // Google Docs increase font size
#define DECFNT C(S(KC_COMM)) // Google Docs decrease font size

enum numrow_layers {
    NR_NUMROW = 0,
    NR_BRACKETS,
    NR_GDOCS,
    NR_GENSHIN,
    NR_FN,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[_QWERTY] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  NR0,      NR1,    NR2,     NR3,     NR4,     NR5,                       NR6,     NR7,    NR8,     NR9,     NR10,    NR11,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_TAB,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                      KC_Y,    KC_U,   KC_I,    KC_O,    KC_P,    KC_UNDS,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_BSPC,  KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                      KC_H,    KC_J,   KC_K,    KC_L,    KC_SCLN, KC_QUOT,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  KC_LCTL,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,  KC_MUTE,    MODMEH, KC_N,    KC_M,   KC_COMM, KC_DOT,  KC_SLSH, KC_ENT,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
                 KC_LALT, KC_LGUI, KC_LOWER, KC_LSFT, KC_SPC,     KC_NAV, KC_SPC, KC_RAISE,KC_LPRN, KC_RPRN
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),

// [_COLEMAK] = LAYOUT(
//   //,------------------------------------------------.                    ,---------------------------------------------------.
//   KC_TRNS,  KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                   KC_TRNS, KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
//   //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
//   KC_TRNS,  KC_Q,   KC_W,    KC_F,    KC_P,    KC_G,                      KC_J,    KC_L,   KC_U,    KC_Y,    KC_SCLN, KC_TRNS,
//   //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
//   KC_TRNS,  KC_A,   KC_R,    KC_S,    KC_T,    KC_D,                      KC_H,    KC_N,   KC_E,    KC_I,    KC_O,    KC_TRNS,
//   //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
//   KC_TRNS,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,  KC_TRNS,    KC_TRNS,KC_K,    KC_M,   KC_COMM, KC_DOT,  KC_SLSH, KC_TRNS,
//   //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
//                  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,    KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS
//   //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
// ),

[_COLEMAKDH] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  KC_TRNS,  KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                   KC_TRNS, KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_TRNS,  KC_Q,   KC_W,    KC_F,    KC_P,    KC_B,                      KC_J,    KC_L,   KC_U,    KC_Y,    KC_SCLN, KC_TRNS,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_TRNS,  KC_A,   KC_R,    KC_S,    KC_T,    KC_G,                      KC_M,    KC_N,   KC_E,    KC_I,    KC_O,    KC_TRNS,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  KC_TRNS,  KC_Z,   KC_X,    KC_C,    KC_D,    KC_V,  KC_TRNS,    KC_TRNS,KC_K,    KC_H,   KC_COMM, KC_DOT,  KC_SLSH, KC_TRNS,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,    KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),

[_GENSHIN] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  KC_ESC,   KC_5,   KC_1,    KC_2,    KC_3,    KC_4,                      NR6,     NR7,    NR8,     NR9,     NR10,    NR11,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_TAB,   KC_T,   KC_Q,    KC_W,    KC_E,    KC_R,                      KC_Y,    KC_U,   KC_I,    KC_O,    KC_P,    KC_UNDS,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_M,     KC_BSPC,KC_A,    KC_S,    KC_D,    KC_F,                      KC_H,    KC_J,   KC_K,    KC_L,    KC_SCLN, KC_QUOT,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  KC_LALT,  KC_Z,   KC_B,    KC_X,    KC_C,    KC_V,  KC_MUTE,    MODMEH, KC_N,    KC_M,   KC_COMM, KC_DOT,  KC_SLSH, KC_ENT,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
                 KC_LCTL, KC_LGUI, KC_LOWER, KC_LSFT, KC_SPC,     KC_NAV, KC_SPC, KC_RAISE,KC_U,    KC_J
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),

[_LOWER] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  KC_F13,   KC_F14, KC_F15,  KC_F16,   KC_F17,  KC_F18,                   KC_F19,  KC_F20,  KC_F21,  KC_F22,  KC_F23,  KC_F24,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_F11,   KC_F1,  KC_F2,   KC_F3,    KC_F4,   KC_F5,                    KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F12,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  CLSWIN,   TSKMGR, CTALDEL, KC_PSCR,  KC_NO,   KC_NO,                    KC_BRIU, KC_VOLU, MS_BTN1, MS_U,    MS_BTN2, MS_BTN4,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  VSCMDS,   LTSYNC, VSTERM,  IJNXT,    IJRENM,  IJEXTR, TONAV,   KC_TRNS, KC_BRID, KC_VOLD, MS_L,    MS_D,    MS_R,    MS_BTN5,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
            C(G(KC_LEFT)),C(G(KC_RGHT)),_______,_______,QK_BOOT, _______, _______, _______, _______, MS_BTN3
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),

// KC_LANG1 switches to hiragana when in Japanese IME mode. Also hangul/english toggle key.
// KC_LANG2 switches to half-width alphanumeric. Also hanja conversion key.
[_RAISE] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  KC_GRV,  KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,                     KC_F7,   KC_F8,   KC_F9,   KC_F10 , KC_F11 , KC_F12,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_TILD, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                      KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_TRNS,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  _______, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                   KC_CIRC, KC_LCBR, KC_RCBR, KC_LBRC, KC_RBRC, KC_BSLS,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |-------+-------+--------+--------+--------+---------|
  _______, KC_QUES, _______, KC_LPRN, KC_RPRN, KC_LT, _______,   _______, KC_GT,   KC_PLUS, KC_MINS, KC_ASTR, KC_SLSH, KC_PIPE,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
                 _______, _______, _______, _______, _______,    KC_LNG1, KC_LNG2, _______, KC_EQL,  KC_AMPR
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),

  [_ADJUST] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  EE_CLR,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  QK_BOOT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_QWERTY,                 XXXXXXX, KC_MPRV, KC_MPLY, KC_MNXT, XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, RGB_SPI, KC_GENSHIN,                XXXXXXX,OLED_B_DN,OLED_B_UP,XXXXXXX,XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD,RGB_SPD,KC_COLEMAKDH,XXXXXXX,XXXXXXX,XXXXXXX, EXMON1,  EXMON2,  EXMON3,  XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
                   _______, _______, _______, _______, _______,     _______, _______, _______, _______, _______
    //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),
/*
Can be accessed in two modes - temporary (KC_NAV) & permenent (TONAV).
KC_INT1: japanese keyboard input 'ro'
KC_INT2: katakana/hiragana key
KC_INT3: Yen, full width hyphen
KC_INT4: Henkan
KC_INT5: Muhenkan
*/
[_NAV] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  LAYCLR,   KC_PIPE, KC_LPRN, KC_RPRN, KC_AMPR, KC_NUM,                   KC_BRK,  KC_PSCR, XXXXXXX, XXXXXXX, XXXXXXX, LAYCLR,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_EQL,   KC_CIRC, KC_P7,   KC_P8,   KC_P9,   KC_PERC,                  KC_PGUP, KC_HOME, KC_UP,   KC_END,  XXXXXXX, DELBRC,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  _______,  KC_MINS, KC_P4,   KC_P5,   KC_P6,   KC_SLSH,                  KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, KC_APP,  SELBRC,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  _______,  KC_PLUS, KC_P1,   KC_P2,   KC_P3,   KC_ASTR,TO(_LOWER),sPgBoB,KC_DEL,  KC_INS,  KC_NO,   KC_SCRL, KC_APP,  GOTOBRC,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
            _______, KC_P0,   KC_PDOT, _______, KC_INT5,         _______, KC_INT1, KC_INT2, KC_INT3, KC_INT4
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),
// Accessed by quick double tapping NAV
// Useful for right handed numpad
[_NAVSWAP] = LAYOUT(
  //,------------------------------------------------.                    ,-------------------------------------------+--------.
  LAYCLR,  KC_PSCR,  KC_BRK, _______, XXXXXXX, XXXXXXX,                   KC_NUM,  KC_PIPE, KC_LPRN, KC_RPRN, KC_PERC, LAYCLR,
  //|------+-------+--------+--------+--------+------|                   |-------+--------+--------+--------+---------+--------|
  _______, KC_HOME, KC_UP,   KC_END,  _______, DELBRC,                    KC_CIRC, KC_P7,   KC_P8,   KC_P9,   KC_LT,   KC_GT,
  //|------+-------+--------+--------+--------+------|                   |-------+--------+--------+--------+---------+--------|
  _______, KC_LEFT, KC_DOWN, KC_RGHT, KC_APP,  SELBRC,                    KC_MINS, KC_P4,   KC_P5,   KC_P6,   KC_SLSH, KC_BSPC,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |-------+--------+--------+--------+---------+--------|
  _______, KC_INS,  KC_NO,   KC_SCRL, _______, GOTOBRC,TO(_LOWER),ENTCLR, KC_PLUS, KC_P1,   KC_P2,   KC_P3,   KC_ASTR, KC_ENT,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |-------+--------+--------+--------+---------+--------|
                _______, _______, _______, _______, _______,      KC_EQL, _______, KC_COMM, KC_P0,   KC_PDOT
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------
),
};

static const char** numrow_layer_names = (const char*[]){"Num\n\n", "Brk\n\n", "GDocs\n", /*"Gensh\n",*/ "FN\n\n"};
static uint8_t current_numrow_layer = 0;
#define NUMROW_LAYER_COUNT 4
const uint16_t PROGMEM numrow_keymaps[][12] = {
    [NR_NUMROW] =   {  KC_ESC,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_GRV  },
    [NR_BRACKETS] = {  KC_ESC,   KC_LPRN,  KC_LBRC,  KC_LCBR,  KC_LT,    DREFACC,  SCOPE,    KC_GT,    KC_RCBR,  KC_RBRC,  KC_RPRN,  KC_GRV  },
    [NR_GDOCS] =    {  KC_ESC,   HEAD1,    HEAD2,    HEAD3,    HEAD4,    GDSTRK,   GDCMT,    DECFNT,   INCFNT,   WRDCNT,   NORMTXT,  GDCMD   },
    // [NR_GENSHIN] =  {  KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_ESC,   KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_GRV  },
    [NR_FN] =       {  KC_ESC,   KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12  },
};

const char* get_u6_str(uint8_t curr_num, char curr_pad) {
    static char    buf[3]   = {0};
    static uint8_t last_num = 0xFF;
    static char    last_pad = '\0';
    if (last_num == curr_num && last_pad == curr_pad) {
        return buf;
    }
    last_num = curr_num;
    last_pad = curr_pad;
    return get_numeric_str(buf, sizeof(buf), curr_num, curr_pad);
}

static m_to_s_data_t sync_data = {0};

// For storing in EEPROM.
static user_config_t user_config;

static uint8_t mod_state;
static uint8_t highest_layer_state;

// Bitmask for special key held down state
typedef union {
    uint8_t _raw;
    struct {
        bool backspace : 1;
        bool f : 1;
        uint8_t f_delay : 3; // keep track of cycles to wait before spamming f
    };
} key_held_state_t;
key_held_state_t key_held_state = {0};

static nav_state_enum nav_state = NAV_STATE_MO;

// NAV Double Tap state flow:
//
// 0: Starting state. Set to 255 if Nav pressed in this state.
//
// 255: Nav pressed, if non-NAV key is pressed after this, reset back to 0
//      If NAV is released cleanly, within DBL_TAP_DUR_FRAMES, this value stores the current info_display_timeout value.
//
// >= 1: Clean nav release, stores timer value of when nav was released. If pressed before <= DBL_TAP_DUR_FRAMES elapse,
//       Proceed to next state (254).
//
// 254: Second clean keypress of nav, upon release.
//
// After 254, if the release was within DBL_TAP_DUR_FRAMES frames, and no other keys were pressed while held,
// activate NAVSWAP permalayer
static uint8_t nav_dbl_tap_state = 0;

// Stores whether or not to capitalize letter. Toggles every letter/on shift.
static bool sPoNgEbOb_state = false;
static bool last_synced_sPoNgEbOb_active = true;

// NOTE: Remember u can't assign strings directly to char arrays, you need to strcpy.
// For info display, a short string describing the value being displayed
static oled_display_mode_enum oled_display_mode = OLED_DISPLAY_HUE;

static uint8_t info_display_timeout = 0; // in led update frames
static uint16_t send_hid_user_data_timer16 = 0; // Wait for a bit before sending HID user data to host.
static bool hid_user_data_sent = false;

// for custom mouse keys impl

static report_mouse_t custom_mouse_report = {0, 0, 0, 0, 0};
static report_mouse_t old_mouse_report = {0, 0, 0, 0, 0};

// stores number of matrix scan iterations since last mouse update
// we don't use a timer to save on ROM and CPU.
static uint8_t mouse_frames_counter = 0;

// stores number of iterations since last mouse speed update
static uint8_t mouse_speed_frames_counter = 0;

static int8_t mouse_vel_x = 0;
static int8_t mouse_vel_y = 0;
// mousekeys bitmask
static mousekeys_status_t mousekeys_status = {0};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        sync_data.last_keypress_timer = sync_timer_read32();
        if (keycode != KC_NAV) {
            // Other keys were pressed before NAV was released, reset state.
            nav_dbl_tap_state = 0;
        }
    }
    if (keycode == KC_BSPC) {
        if (record->event.pressed && (mod_state & MOD_MASK_ALT)) {
            tap_code(KC_CAPS);
            oled_display_mode = OLED_DISPLAY_CAPS_LOCK;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            return false;
        }
        key_held_state.backspace = record->event.pressed;
    }
    if (keycode == KC_F) {
        key_held_state.f = record->event.pressed;
        if (get_highest_layer(default_layer_state) == _GENSHIN) {
            // tap_code(KC_SPC); // debug
            key_held_state.f_delay = 0;
            // return false; // let the key through
        }
    }

    if (record->event.pressed) {
        // if (sync_data.sPoNgEbOb_case_active) {
        //     if (keycode >= KC_A && keycode <= KC_Z) {
        //         sPoNgEbOb_state = !sPoNgEbOb_state;
        //         if (sPoNgEbOb_state) {
        //             tap_code16(S(keycode));
        //             return false;
        //         }
        //     } else if (keycode == KC_LSFT || keycode == KC_RSFT) {
        //         sPoNgEbOb_state = !sPoNgEbOb_state;
        //     }
        // }
        // Numrow layering support
        // If unicode enabled, we need to also add the upper bound, otherwise this will
        // catch unicode keys and segfault.
        // Remember to also add the upper bound for the key release event below.
        if (keycode >= NR0 /* && keycode <= NR11 */) {
            uint16_t cust_kc = pgm_read_word(&numrow_keymaps[current_numrow_layer][keycode - NR0]);
            switch(cust_kc) {
            case DREFACC:
                tap_code(KC_MINS);
                tap_code16(KC_GT);
                break;
            case SCOPE:
                tap_code16(KC_COLON);
                tap_code16(KC_COLON);
                break;
            default:
                register_code16(cust_kc);
            }
            return false;
        }
        switch (keycode) {
        case KC_A ... KC_Z:
            if (sync_data.sPoNgEbOb_case_active) {
                sPoNgEbOb_state = !sPoNgEbOb_state;
                if (sPoNgEbOb_state) {
                    tap_code16(S(keycode));
                    return false;
                }
            }
            break;
        case KC_LSFT:
            sPoNgEbOb_state = !sPoNgEbOb_state;
            break;
        case KC_LOWER:
            layer_on(_LOWER);
            update_tri_layer(_LOWER, _RAISE, _ADJUST);
            break;
        case KC_RAISE:
            layer_on(_RAISE);
            update_tri_layer(_LOWER, _RAISE, _ADJUST);
            break;
        case KC_ADJUST:
            layer_on(_ADJUST);
            break;
        case KC_NAV:
            // reuse the info_display_timeout for timing double nav tap, before resetting it
            if (nav_dbl_tap_state == 0) {
                nav_dbl_tap_state = 255;
            } else if (nav_dbl_tap_state != 255 && (nav_dbl_tap_state - info_display_timeout) <= DBL_TAP_DUR_FRAMES) {
                nav_dbl_tap_state = 254;
            }
            layer_on(_NAV);
            nav_state = NAV_STATE_MO;

            oled_display_mode = OLED_DISPLAY_NUM_LOCK;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            break;
        case TONAV:
            nav_state = NAV_STATE_HOLD;
            break;
        case ENTCLR:
            tap_code(KC_ENT); // enter + fallthrough layer clear
        case LAYCLR:
            layer_clear();
            return false;
        case KC_QWERTY:
            set_single_persistent_default_layer(_QWERTY);
            break;
        case KC_GENSHIN:
            set_single_persistent_default_layer(_GENSHIN);
            break;
        case KC_COLEMAKDH:
            set_single_persistent_default_layer(_COLEMAKDH);
            break;
        case RGB_HUI:
        case RGB_HUD:
            oled_display_mode = OLED_DISPLAY_HUE;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            break;
        case RGB_SAI:
        case RGB_SAD:
            oled_display_mode = OLED_DISPLAY_SAT;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            break;
        case RGB_VAI:
        case RGB_VAD:
            oled_display_mode = OLED_DISPLAY_VAL;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            break;
        case RGB_SPI:
        case RGB_SPD:
            oled_display_mode = OLED_DISPLAY_SPD;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            break;
        case KC_LCTL:
            if (key_held_state.backspace) {
                register_code(KC_LCTL);
                tap_code(KC_BSPC);
                unregister_code(KC_LCTL);
                return false;
            }
            break;
        case KC_SCRL:
            oled_display_mode = OLED_DISPLAY_SCROLL_LOCK;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            break;
        case KC_NUM:
            oled_display_mode = OLED_DISPLAY_NUM_LOCK;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            break;
        case KC_INT2:
            oled_display_mode = OLED_DISPLAY_KANA;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            break;
        case sPgBoB:
            sync_data.sPoNgEbOb_case_active = !sync_data.sPoNgEbOb_case_active;
            oled_display_mode = OLED_DISPLAY_SPONGEBOB;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            break;
        case OLED_B_UP: ;
            if (sync_data.oled_brightness >= 240) {
                oled_set_brightness(255);
            } else {
                oled_set_brightness(sync_data.oled_brightness + 16);
            }
            oled_display_mode = OLED_DISPLAY_OLED_BRIGHTNESS;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            perform_data_sync();
            break;
        case OLED_B_DN: ;
            if (sync_data.oled_brightness <= 16) {
                oled_set_brightness(0);
            } else {
                oled_set_brightness(sync_data.oled_brightness - 16);
            }
            oled_display_mode = OLED_DISPLAY_OLED_BRIGHTNESS;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            perform_data_sync();
            break;
        case MS_BTN1:
            custom_mouse_report.buttons |= MOUSE_BTN1;
            break;
        case MS_BTN2:
            custom_mouse_report.buttons |= MOUSE_BTN2;
            break;
        case MS_BTN3:
            custom_mouse_report.buttons |= MOUSE_BTN3;
            break;
        case MS_BTN4:
            custom_mouse_report.buttons |= MOUSE_BTN4;
            break;
        case MS_BTN5:
            custom_mouse_report.buttons |= MOUSE_BTN5;
            break;
        case MS_U:
            mousekeys_status.up = true;
            mousekeys_status.down = false;
            if (mouse_vel_y > 0)
                mouse_vel_y = -mouse_vel_y;
            break;
        case MS_D:
            mousekeys_status.down = true;
            mousekeys_status.up = false;
            if (mouse_vel_y < 0)
                mouse_vel_y = -mouse_vel_y;
            break;
        case MS_L:
            mousekeys_status.left = true;
            mousekeys_status.right = false;
            if (mouse_vel_x > 0)
                mouse_vel_x = -mouse_vel_x;
            break;
        case MS_R:
            mousekeys_status.right = true;
            mousekeys_status.left = false;
            if (mouse_vel_x < 0)
                mouse_vel_x = -mouse_vel_x;
            break;
        case EXMON1:
            // bright mode
            user_config.ext_monitor_brightness = 60;
            user_config.ext_monitor_contrast = 50;
            update_ext_mon_setting();
            break;
        case EXMON2:
            // light mode
            user_config.ext_monitor_brightness = 5;
            user_config.ext_monitor_contrast = 50;
            update_ext_mon_setting();
            break;
        case EXMON3:
            // dark mode
            user_config.ext_monitor_brightness = 0;
            user_config.ext_monitor_contrast = 25;
            update_ext_mon_setting();
            break;
        }
    } else {
        // key up event

        // handle numrow layers
        if (keycode >= NR0 /* && keycode <= NR11 */) {
            unregister_code16(pgm_read_word(&numrow_keymaps[current_numrow_layer][keycode - NR0]));
        }
        switch (keycode) {
        case KC_LOWER:
            layer_off(_LOWER);
            update_tri_layer(_LOWER, _RAISE, _ADJUST);
            break;
        case KC_RAISE:
            layer_off(_RAISE);
            update_tri_layer(_LOWER, _RAISE, _ADJUST);
            break;
        case KC_ADJUST:
            layer_off(_ADJUST);
            break;
        case KC_NAV:
            if (info_display_timeout >= OLED_INFO_DISPLAY_DURATION - DBL_TAP_DUR_FRAMES) {
                // ^ Hack to reuse info_display_timeout to time the duration from last NAV keypress to current release.

                if (nav_dbl_tap_state == 255) {
                    // Short, clean tap. Store info_display_timeout value.
                    nav_dbl_tap_state = info_display_timeout;
                } else if (nav_dbl_tap_state == 254) {
                    // The release
                    layer_move(_NAVSWAP);
                    return false;
                }
            }
            else {
                // Reset state, release did not occur within 4 frames.
                nav_dbl_tap_state = 0;
            }
            layer_off(_NAV);
            break;
        case MS_BTN1:
            custom_mouse_report.buttons &= ~MOUSE_BTN1;
            break;
        case MS_BTN2:
            custom_mouse_report.buttons &= ~MOUSE_BTN2;
            break;
        case MS_BTN3:
            custom_mouse_report.buttons &= ~MOUSE_BTN3;
            break;
        case MS_BTN4:
            custom_mouse_report.buttons &= ~MOUSE_BTN4;
            break;
        case MS_BTN5:
            custom_mouse_report.buttons &= ~MOUSE_BTN5;
            break;
        case MS_U:
            mousekeys_status.up = false;
            break;
        case MS_D:
            mousekeys_status.down = false;
            break;
        case MS_L:
            mousekeys_status.left = false;
            break;
        case MS_R:
            mousekeys_status.right = false;
            break;
        }
    }
    send_mouse_report();

    return true;
}

// The rotary encoders I bought send 2 pulses per detent, so we need to halve the signals.
// A value of 0 represents no pulse received, requires 2 pulses in the same direction to send an action.
// A value of 1 represents 1 pulse received clockwize, -1 represents 1 pulse anti-clockwise.
// static int8_t encoder_1_state = 0;
// static int8_t encoder_2_state = 0;

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        // if ((clockwise && encoder_1_state == 1) || (!clockwise && encoder_1_state == -1)) {
        //     encoder_1_state = 0;
        // } else {
        //     encoder_1_state = clockwise ? 1 : -1;
        //     return false;
        // }

        if (mod_state & (MOD_MASK_ALT | MOD_MASK_CTRL)) {
            if (clockwise) {
                tap_code(KC_TAB);
            } else {
                tap_code16(S(KC_TAB));
            }
        } else {
            switch (highest_layer_state) {
            case _LOWER:
                if (clockwise) {
                    tap_code(KC_BRIU);
                } else {
                    tap_code(KC_BRID);
                }
                break;
            case _RAISE:
                if (clockwise) {
                    user_config.ext_monitor_brightness += 5;
                    if (user_config.ext_monitor_brightness > 100) {
                        user_config.ext_monitor_brightness = 100;
                    }
                } else {
                    user_config.ext_monitor_brightness -= 5;
                    if (user_config.ext_monitor_brightness >= 100) {
                        user_config.ext_monitor_brightness = 0;
                    }
                }
                update_ext_mon_setting();
                break;
            case _NAV:
                if (nav_state == NAV_STATE_HOLD) {
                    tap_code(clockwise ? KC_RGHT : KC_LEFT);
                    break;
                }
                if (clockwise) {
                    user_config.ext_monitor_contrast += 5;
                    if (user_config.ext_monitor_contrast > 100) {
                        user_config.ext_monitor_contrast = 100;
                    }
                } else {
                    user_config.ext_monitor_contrast -= 5;
                    if (user_config.ext_monitor_contrast >= 100) {
                        user_config.ext_monitor_contrast = 0;
                    }
                }
                update_ext_mon_setting();
                oled_display_mode = OLED_DISPLAY_EXT_MONITOR_CONTRAST;
                break;
            case _NAVSWAP:
                tap_code(clockwise ? KC_DOWN : KC_UP);
                break;
            case _ADJUST:
                // cycle numrow layer
                current_numrow_layer += clockwise * 2 - 1;
                if (current_numrow_layer == 255) {
                    current_numrow_layer = NUMROW_LAYER_COUNT - 1;
                }
                if (current_numrow_layer >= NUMROW_LAYER_COUNT) {
                    current_numrow_layer = 0;
                }
                oled_display_mode = OLED_DISPLAY_NUMROW_LAYER;
                info_display_timeout = OLED_INFO_DISPLAY_DURATION;
                break;
            default:
                if (clockwise) {
                    tap_code(KC_VOLU);
                } else {
                    tap_code(KC_VOLD);
                }
            }
        }
    } else if (index == 1) {
        // if ((clockwise && encoder_2_state == 1) || (!clockwise && encoder_2_state == -1)) {
        //     encoder_2_state = 0;
        // } else {
        //     encoder_2_state = clockwise ? 1 : -1;
        //     return false;
        // }
        int8_t scroll_direction_flag = clockwise * 2 - 1;
        // drastic attempt to reduce code size, doing this only saves 2 bytes
        uint16_t left_or_right = clockwise ? KC_RGHT : KC_LEFT;
        uint8_t up_or_down = clockwise ? KC_DOWN : KC_UP;

        switch (highest_layer_state) {
        case _ADJUST:
            custom_mouse_report.h = scroll_direction_flag;
            break;
        case _RAISE:
            custom_mouse_report.v = -scroll_direction_flag;
            break;
        case _LOWER:
            tap_code(up_or_down);
            break;
        case _NAV:
            if (nav_state == NAV_STATE_HOLD) {
                tap_code(up_or_down);
            } else {
                tap_code16(C(S(left_or_right)));
            }
            break;
        // case _NAVSWAP: // same as default
        //     tap_code(clockwise ? KC_RGHT : KC_LEFT);
        //     break;
        default:
            tap_code(left_or_right);
            break;
		}
        send_mouse_report();
    }
    return false;
}

bool rgb_matrix_indicators_user(void) {
    if (host_keyboard_led_state().caps_lock) {
        uint8_t rgb_val = rgb_matrix_get_val();
        rgb_matrix_set_color(0, 0, rgb_val / 4, rgb_val / 4);
        rgb_matrix_set_color(36, 0, rgb_val / 4, rgb_val / 4);
        rgb_matrix_set_color(9, 0, rgb_val, rgb_val);
    } else if (sync_data.sPoNgEbOb_case_active) {
        uint8_t rgb_val = rgb_matrix_get_val();
        rgb_matrix_set_color(36, rgb_val / 2, rgb_val / 8, 0);
        rgb_matrix_set_color(0, rgb_val / 2, rgb_val / 8, 0);
    } else {
        rgb_matrix_set_color(36, 0, 0, 0);
        rgb_matrix_set_color(0, 0, 0, 0);
    }

    switch (highest_layer_state) {
    case _NAV:
    case _NAVSWAP: ;
        // display num_lock state
        uint8_t rgb_val = rgb_matrix_get_val();
        uint8_t num_lock_idx = 32;
        uint8_t scroll_lock_idx = 36+18;
        if (highest_layer_state == _NAVSWAP) {
            num_lock_idx = 36+32;
            scroll_lock_idx = 25;
        }
        if (host_keyboard_led_state().num_lock) {
            rgb_matrix_set_color(num_lock_idx, 0, rgb_val / 2, rgb_val / 4);
        } else {
            rgb_matrix_set_color(num_lock_idx, rgb_val / 2, rgb_val / 8, 0);
        }
        // display scroll_lock state
        if (host_keyboard_led_state().scroll_lock) {
            rgb_matrix_set_color(scroll_lock_idx, 0, rgb_val / 2, rgb_val / 4);
        } else {
            rgb_matrix_set_color(scroll_lock_idx, rgb_val / 2, rgb_val / 8, 0);
        }
        break;
    }
    return true;
}

/**
 * @brief Sends mouse report (only if data changed since last send)
 *
 */
void send_mouse_report(void) {
    bool should_send_report = has_mouse_report_changed(&custom_mouse_report, &old_mouse_report);

    if (should_send_report) {
        host_mouse_send(&custom_mouse_report);
    }
    // Buttons remain, but movements/scroll are reset.
    // The address of custom_mouse_report + 1 is the first movement related struct member
    // (assuming MOUSE_SHARED_EP not defined)
    memset(((uint8_t*) &custom_mouse_report) + 1, 0, sizeof(custom_mouse_report) - 1);
    memcpy(&old_mouse_report, &custom_mouse_report, sizeof(custom_mouse_report));
}

/**
 * @brief Callback handler for slave side to receive sync data from master side.
 *
 * @param in_buflen length of input data
 * @param in_data input data
 * @param out_buflen length of output data
 * @param out_data output data
 */
void user_sync_data_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    sync_data = *(const m_to_s_data_t*)in_data;
    oled_set_brightness(sync_data.oled_brightness);
}

static uint32_t last_synced_time = 0;
// Set to true when sync failed to immediately retry on the next housekeeping_task iteration.
static bool force_sync_retry = false;

/**
 * @brief Send sync data from master side to slave side.
 * @return returns `true` if sync successful, otherwise `false`. If called on slave side, always returns `true`.
 */
bool perform_data_sync(void) {
    if (is_keyboard_master()) {
        // no need for bidirectional communication, so use transaction_rpc_send instead of
        // transaction_rpc_exec
        sync_data.oled_brightness = oled_get_brightness();
        last_synced_sPoNgEbOb_active = sync_data.sPoNgEbOb_case_active;
        last_synced_time = timer_read32();
        bool sync_success = transaction_rpc_send(
            USER_SYNC_DATA,
            sizeof(sync_data),
            &sync_data
        );
        force_sync_retry = !sync_success;
        return sync_success;
    }
    return true; // if slave keyboard calls this function, no need to do anything.
}

void send_hid_user_data(void) {
    // Send 0x01, <external monitor brightness>, <contrast>
    uint8_t bytes[32];
    memset(bytes, 0, sizeof(bytes));
    bytes[0] = 0x01;
    bytes[1] = user_config.ext_monitor_brightness;
    bytes[2] = user_config.ext_monitor_contrast;
    raw_hid_send(bytes, 32);
}

// Store external monitor settings in EEPROM & schedule send_hid_user_data
static void update_ext_mon_setting(void) {
    eeconfig_update_user(user_config._raw);
    hid_user_data_sent = false;
    send_hid_user_data_timer16 = timer_read();
    oled_display_mode = OLED_DISPLAY_EXT_MONITOR_BRIGHTNESS;
    info_display_timeout = OLED_INFO_DISPLAY_DURATION;
}

void keyboard_post_init_user() {
    user_config._raw = eeconfig_read_user();
    transaction_register_rpc(USER_SYNC_DATA, user_sync_data_slave_handler);
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (is_keyboard_master()) {
        return OLED_ROTATION_270;
    }
    return OLED_ROTATION_270;
    // return rotation;
}

static char string_buffer[30] = "\0";
static char* str_buf_ptr = string_buffer;

#define RESET_BUFFER() str_buf_ptr = string_buffer; string_buffer[0] = '\0';

static void cat_buffer(const char* str) {
    while (*str != '\0') {
        *str_buf_ptr = *str;
        str_buf_ptr++;
        str++;
    }
    *str_buf_ptr = '\0';
}

static const char** names_of_days = (const char*[]) {
    "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

static const char** names_of_months = (const char*[]) {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

// The text must match the order of the sofle_layers enum,
// the first index, lower, is the first layer after the base layers.
static const char** names_of_layers = (const char*[]) {
    "Lower\n", "Raise\n", "Adj\n\n", "Nav\n\n", "NavSw\n"
};

static void render_left_oled(void) {
    // Print current mode
    RESET_BUFFER();
    oled_set_cursor(0, 0);
    cat_buffer("euw\nbah\n\n");

    switch (get_highest_layer(default_layer_state)) {
        case _QWERTY:
            cat_buffer("Qwrt\n\n");
            break;
        case _GENSHIN:
            cat_buffer("Gensh\n");
            break;
        case _COLEMAKDH:
            cat_buffer("CmkDH\n");
            break;
    }
    // Print current layer
    if (highest_layer_state < _LOWER) {
        cat_buffer(numrow_layer_names[current_numrow_layer]);
    } else {
        cat_buffer(names_of_layers[highest_layer_state - _LOWER]);
    }

    // 0-255: show number verbatim
    // 256: OFF
    // 257: ON
    int16_t info_display = -1;

    if (info_display_timeout > 0) {
        switch (oled_display_mode) {
            case OLED_DISPLAY_HUE:
                cat_buffer("HUE\n");
                info_display = rgb_matrix_get_hue();
                break;
            case OLED_DISPLAY_SAT:
                cat_buffer("SAT\n");
                info_display = rgb_matrix_get_sat();
                break;
            case OLED_DISPLAY_VAL:
                cat_buffer("VAL\n");
                info_display = rgb_matrix_get_val();
                break;
            case OLED_DISPLAY_SPD:
                cat_buffer("SPD\n");
                info_display = rgb_matrix_get_speed();
                break;
            case OLED_DISPLAY_CAPS_LOCK:
                cat_buffer("CAPS\n");
                info_display = 256 + host_keyboard_led_state().caps_lock;
                break;
            case OLED_DISPLAY_SCROLL_LOCK:
                cat_buffer("SCRLK");
                info_display = 256 + host_keyboard_led_state().scroll_lock;
                break;
            case OLED_DISPLAY_NUM_LOCK:
                cat_buffer("NUMLK");
                info_display = 256 + host_keyboard_led_state().num_lock;
                break;
            case OLED_DISPLAY_KANA:
                cat_buffer("KANA\n");
                info_display = 256 + host_keyboard_led_state().kana;
                break;
            case OLED_DISPLAY_SPONGEBOB:
                cat_buffer("sPbOb");
                info_display = 256 + sync_data.sPoNgEbOb_case_active;
                break;
            case OLED_DISPLAY_OLED_BRIGHTNESS:
                cat_buffer("OLED\n");
                info_display = oled_get_brightness();
                break;
            case OLED_DISPLAY_EXT_MONITOR_BRIGHTNESS:
                cat_buffer("BRGHT");
                info_display = user_config.ext_monitor_brightness;
                break;
            case OLED_DISPLAY_EXT_MONITOR_CONTRAST:
                cat_buffer("CONT\n");
                info_display = user_config.ext_monitor_contrast;
                break;
            case OLED_DISPLAY_NUMROW_LAYER:
                cat_buffer(numrow_layer_names[current_numrow_layer]);
                break;
        }
        info_display_timeout--;
        oled_write(string_buffer, false);
        RESET_BUFFER();
        if (info_display >= 256) {
            bool is_on = info_display & 1;
            oled_write_ln(is_on ? "ON" : "OFF", is_on);
        } else if (info_display != -1) {
            oled_write_ln(get_u8_str(info_display, ' '), false);
        }
    } else {
        oled_write(string_buffer, false);
        RESET_BUFFER();
    }
    oled_write_ln_P(PSTR("\n\n\n"), false);

    oled_set_cursor(0, 11);

    cat_buffer("WPM\n");
    cat_buffer(get_u8_str(get_current_wpm(), ' '));

    if (sync_data.datetime.month != 0) {
        cat_buffer("\n\n");
        cat_buffer(get_u6_str(sync_data.datetime.date, ' '));
        cat_buffer(names_of_months[sync_data.datetime.month - 1]);
        cat_buffer(names_of_days[sync_data.datetime.day_of_week]);
    }

    oled_write(string_buffer, false);
}

// lines is the length of the bar in pixels
// bar_byte is 8 bits, each bit represents a pixel in the bar.
// e.g. 0xFF will be a full bar of 8 pixels.
static void draw_bar(uint8_t length, uint8_t bar_byte) {
    char full_width_bar[6] = {bar_byte, bar_byte, bar_byte, bar_byte, bar_byte, bar_byte};
    for (uint8_t i = 0; i < length / 6; i++) {
        oled_write_raw(full_width_bar, 6);
        oled_advance_char();
    }
    char partial_bar[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    for (uint8_t i = 0; i < length % 6; i++) {
        partial_bar[i] = bar_byte;
    }
    oled_write_raw(partial_bar, 6);
    oled_advance_char();
}

static void clear_bar(uint8_t length) {
    char full_width_bar[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    for (uint8_t i = 0; i < length / 6; i++) {
        oled_write_raw(full_width_bar, 6);
        oled_advance_char();
    }
    oled_write_raw(full_width_bar, 6);
    oled_advance_char();
}

static void draw_utilization_bar(uint8_t utilization) {
    uint8_t bar_length = utilization / 3;
    if (bar_length > 32) {
        bar_length = 32;
    }
    draw_bar(bar_length, 0x7E);
    clear_bar(32-bar_length);
}

#define ANIM_FRAME 200

static uint32_t anim_timer = 0;
const static char** device_utilization_names = (const char*[]) {
    "CPU", "CTEMP", "RAM", "GPU", "VRAM", "GTEMP", "GPU1"
};

static void render_right_oled(void) {
    if (timer_elapsed32(anim_timer) > ANIM_FRAME) {
        anim_timer = timer_read32();
        uint8_t cursor_y = 0;

        // using memory saving hacks
        uint8_t* utilization_data_ptr = (uint8_t*) &sync_data;
        char** utilization_names_ptr = (char**) device_utilization_names;

        for (uint8_t util_idx = 0; util_idx < 7; util_idx++) {
            oled_set_cursor(0, cursor_y++);
            oled_write(*utilization_names_ptr++, false);
            oled_set_cursor(0, cursor_y++);
            draw_utilization_bar(*utilization_data_ptr++);
        }

        if (sync_data.datetime.month != 0) {
            oled_set_cursor(0, cursor_y++);
            RESET_BUFFER();
            cat_buffer(get_u6_str(sync_data.datetime.hour, '0'));
            cat_buffer(":");
            cat_buffer(get_u6_str(sync_data.datetime.minute, '0'));
            cat_buffer(get_u6_str(sync_data.datetime.second, '0'));
            oled_write(string_buffer, false);
        }
    }
}

bool oled_task_user(void) {
    if (sync_timer_elapsed32(sync_data.last_keypress_timer) > CUSTOM_OLED_TIMEOUT) {
        if (is_oled_on()) {
            oled_off();
        }
        return false;
    } else if (!is_oled_on()) {
        oled_on();
    }
    if (is_keyboard_master()) {
        render_left_oled();
    } else {
        render_right_oled();
    }
    return false;
}

// This runs at the end of update loop.
void housekeeping_task_user(void) {
    // update mods & layer state
    mod_state = get_mods();
    highest_layer_state = get_highest_layer(layer_state);

    if (timer_elapsed32(last_synced_time) > FORCE_DATA_SYNC_TIME
        || sync_data.sPoNgEbOb_case_active != last_synced_sPoNgEbOb_active
        || force_sync_retry) {
        perform_data_sync();
    }

    if (timer_elapsed(send_hid_user_data_timer16) > HID_SEND_DATA_DELAY && !hid_user_data_sent) {
        send_hid_user_data();
        hid_user_data_sent = true;
    }

    // update mouse tings
    if (mouse_speed_frames_counter++ > MOUSE_SPEED_UPDATE_INTERVAL) {


        // Implement spam F
        // F will be spammed at the same rate as MOUSE_SPEED_UPDATE_INTERVAL
        if (key_held_state.f && get_highest_layer(default_layer_state) == _GENSHIN) {
            if (key_held_state.f_delay != 6)
                key_held_state.f_delay++;
            else
                tap_code(KC_F);
        }

        mouse_speed_frames_counter = 0;

        if (mousekeys_status.right) {
            if (mouse_vel_x < MOUSE_SPEED_MAX)
                mouse_vel_x += 2;
        } else if (mousekeys_status.left) {
            if (mouse_vel_x > -MOUSE_SPEED_MAX)
                mouse_vel_x -= 2;
        } else if (mouse_vel_x < 0) {
            mouse_vel_x += -mouse_vel_x / 4 + 1;
        } else if (mouse_vel_x > 0) {
            mouse_vel_x += -mouse_vel_x / 4 - 1;
        }
        if (mousekeys_status.up) {
            if (mouse_vel_y > -MOUSE_SPEED_MAX)
                mouse_vel_y -= 2;
        } else if (mousekeys_status.down) {
            if (mouse_vel_y < MOUSE_SPEED_MAX)
                mouse_vel_y += 2;
        } else if (mouse_vel_y < 0) {
            mouse_vel_y += -mouse_vel_y / 4 + 1;
        } else if (mouse_vel_y > 0) {
            mouse_vel_y += -mouse_vel_y / 4 - 1;
        }
    }

    if (mouse_frames_counter++ > MOUSE_UPDATE_INTERVAL) {
        mouse_frames_counter = 0;
        custom_mouse_report.x = mouse_vel_x;
        custom_mouse_report.y = mouse_vel_y;
        send_mouse_report();
    }
}

void raw_hid_receive(uint8_t *data, uint8_t length) {
    if(data[0] == 1) {
        // First data byte 1 means we are receiving host utilization data.
        uint8_t* data_ptr = data + 1;
        uint8_t* sync_data_ptr = ((uint8_t*) &sync_data);
        for (uint8_t i = 1; i <= 7; i++) {
            *sync_data_ptr++ = *data_ptr++;
        }
        // Saving ROM for:
        // sync_data.cpu_usage = data[1];
        // sync_data.cpu_temp = data[2];
        // sync_data.ram_usage = data[3];
        // sync_data.gpu_usage = data[4];
        // sync_data.gpu_mem_usage = data[5];
        // sync_data.gpu_temp = data[6];
        // sync_data.gpu1_usage = data[7];

        sync_data.datetime.month = data[10];
        sync_data.datetime.date = data[11];
        sync_data.datetime.day_of_week = data[12];
        sync_data.datetime.hour = data[13];
        sync_data.datetime.minute = data[14];
        sync_data.datetime.second = data[15];

        perform_data_sync();
    }
}
