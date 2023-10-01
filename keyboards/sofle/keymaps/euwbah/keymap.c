
 /* Copyright 2021 Dane Evans
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
#include <stdio.h>

// This include is required for using rpc transactions between master and slave
#include "transactions.h"
#include "raw_hid.h"
#include "report.h"
#include "./config.h"

#include QMK_KEYBOARD_H

// in led update frames
#define OLED_INFO_DISPLAY_DURATION 50

enum sofle_layers {
    _DEFAULTS = 0,
    _QWERTY = 0,
    _COLEMAK,
    _COLEMAKDH,
    _LOWER,
    _RAISE,
    _ADJUST,
    _NAV
};

enum custom_keycodes {
    KC_QWERTY = SAFE_RANGE,
    KC_COLEMAK,
    KC_COLEMAKDH,
    KC_LOWER,
    KC_RAISE,
    KC_ADJUST,
    KC_NAV,
    KC_D_MUTE,
    sPgBoB, // toggle SpOnGeBoB case
    OLED_B_UP, // Increase oled brightness
    OLED_B_DN, // Decrease oled brightness
    MS_BTN1, MS_BTN2, MS_BTN3, MS_BTN4, MS_BTN5, // Custom implemented mouse buttons
    MS_U, MS_D, MS_L, MS_R, // Custom implemented mouse movements
};

enum oled_display_mode_enum {
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
};

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
} datetime_t;

typedef union {
    uint16_t _raw;
    struct {
        uint8_t ext_monitor_brightness : 7;
        uint8_t ext_monitor_contrast : 7;
    };
} user_config_t;

// Sent using USER_SYNC_DATA transaction ID.
// Don't exceed 32 bytes.
typedef struct _m_to_s_data {
    uint8_t cpu_usage;
    uint8_t cpu_temp;
    uint8_t ram_usage;
    uint8_t gpu_usage;
    uint8_t gpu_mem_usage;
    uint8_t gpu_temp;
    uint8_t gpu1_usage;
    bool sPoNgEbOb_case_active;
    datetime_t datetime;
    uint32_t last_keypress_timer;
    uint8_t oled_brightness;
} m_to_s_data_t;

#define CLSWIN A(KC_F4)
#define TSKMGR C(S(KC_ESC))
#define CTALDEL C(A(KC_DEL))
#define VSCMDS C(S(KC_P))
#define LTSYNC C(A(KC_J)) // latex viewer pdf scroll sync
#define VSTERM A(KC_F12) // VSCode open terminal
#define IJNXT A(KC_J) // IntelliJ add next occurrence to selection
#define IJRENM S(KC_F6) // IntelliJ rename
#define IJEXTR C(A(KC_V)) // IntelliJ extract variable

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  KC_ESC,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                      KC_6,    KC_7,   KC_8,    KC_9,    KC_0,    KC_GRV,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_TAB,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                      KC_Y,    KC_U,   KC_I,    KC_O,    KC_P,    KC_UNDS,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_BSPC,  KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                      KC_H,    KC_J,   KC_K,    KC_L,    KC_SCLN, KC_QUOT,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  KC_LCTL,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,  KC_MUTE, KC_D_MUTE, KC_N,    KC_M,   KC_COMM, KC_DOT,  KC_SLSH, KC_ENT,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
                 KC_LALT, KC_LGUI, KC_LOWER, KC_LSFT, KC_SPC,     KC_NAV, KC_SPC, KC_RAISE,KC_LPRN, KC_RPRN
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),

[_COLEMAK] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  KC_TRNS,  KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                   KC_TRNS, KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_TRNS,  KC_Q,   KC_W,    KC_F,    KC_P,    KC_G,                      KC_J,    KC_L,   KC_U,    KC_Y,    KC_SCLN, KC_TRNS,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_TRNS,  KC_A,   KC_R,    KC_S,    KC_T,    KC_D,                      KC_H,    KC_N,   KC_E,    KC_I,    KC_O,    KC_TRNS,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  KC_TRNS,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,  KC_TRNS,    KC_TRNS,KC_K,    KC_M,   KC_COMM, KC_DOT,  KC_SLSH, KC_TRNS,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,    KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),

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

[_LOWER] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  KC_F1,    KC_F2,  KC_F3,   KC_F4,    KC_F5,   KC_F6,                    KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_F13,   KC_F14, KC_F15,  KC_F16,   KC_F17,  KC_F18,                   KC_F19,  KC_F20,  KC_F21,  KC_F22,  KC_F23,  KC_F24,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  CLSWIN,   TSKMGR, CTALDEL, KC_PSCR,  KC_NO,   KC_NO,                    KC_BRIU, KC_VOLU, MS_BTN1, MS_U,    MS_BTN2, MS_BTN4,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  VSCMDS,   LTSYNC, VSTERM,  IJNXT,    IJRENM,  IJEXTR,TO(_NAV), KC_TRNS, KC_BRID, KC_VOLD, MS_L,    MS_D,    MS_R,    MS_BTN5,
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
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  _______, KC_QUES, _______, KC_LPRN, KC_RPRN, KC_LT, _______,   _______, KC_GT,   KC_PLUS, KC_MINS, KC_ASTR, KC_SLSH, KC_PIPE,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
                 _______, _______, _______, _______, _______,     KC_LNG1, KC_LNG2, _______, KC_EQL,  KC_AMPR
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),

  [_ADJUST] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  EE_CLR,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  QK_BOOT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_QWERTY,                 XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, RGB_SPI, KC_COLEMAK,                XXXXXXX,OLED_B_DN,OLED_B_UP,XXXXXXX,XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD,RGB_SPD,KC_COLEMAKDH,XXXXXXX,XXXXXXX,XXXXXXX, KC_MPRV, KC_MPLY, KC_MNXT, XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
                   _______, _______, _______, _______, _______,     _______, _______, _______, _______, _______
    //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),
/*
KC_INT1: japanese keyboard input 'ro'
KC_INT2: katakana/hiragana key
KC_INT3: Yen, full width hyphen
KC_INT4: Henkan
KC_INT5: Muhenkan
*/
[_NAV] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  TG(_NAV), KC_PIPE, KC_LPRN, KC_RPRN, KC_AMPR, KC_NUM,                   KC_BRK, KC_PSCR,  _______, _______, XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_EQL,   KC_CIRC, KC_P7,   KC_P8,   KC_P9,   KC_PERC,                  KC_PGUP, KC_HOME, KC_UP,   KC_END,  _______, C(A(KC_BSPC)),
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  _______,  KC_MINS, KC_P4,   KC_P5,   KC_P6,   KC_SLSH,                  KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, KC_APP,  C(A(KC_RBRC)),
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  _______,  KC_PLUS, KC_P1,   KC_P2,   KC_P3,   KC_ASTR,TO(_LOWER),sPgBoB,KC_DEL, KC_INS, KC_NO,  KC_SCRL, _______, C(S(KC_BSLS)),
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
            _______, KC_P0,   KC_PDOT, _______, KC_INT5,         _______, KC_INT1, KC_INT2, KC_INT3, KC_INT4
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),
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
static bool is_backspace_down = false;
static bool sPoNgEbOb_state = false; // automatically toggles when alphabets and shift is pressed.
static bool last_synced_sPoNgEbOb_active = true;

// NOTE: Remember u can't assign strings directly to char arrays, you need to strcpy.
// For info display, a short string describing the value being displayed
static uint8_t oled_display_mode = OLED_DISPLAY_HUE;

static uint8_t info_display_timeout = 0; // in led update frames
static uint32_t send_hid_user_data_timer = 0; // Wait for a bit before sending HID user data to host.
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

// static char string_buffer[100] = "";

static void print_status_narrow(void) {
    // Print current mode
    oled_write_ln_P(PSTR("euw\nbah\n"), false);

    switch (get_highest_layer(default_layer_state)) {
        case _QWERTY:
            oled_write_ln_P(PSTR("Qwrt"), false);
            break;
        case _COLEMAK:
            oled_write_ln_P(PSTR("Clmk"), false);
            break;
        case _COLEMAKDH:
            oled_write_P(PSTR("CmkDH"), false);
            break;

        default:
            oled_write_ln_P(PSTR(""), false);
    }
    oled_write_P(PSTR("\n"), false);
    // Print current layer
    switch (get_highest_layer(layer_state)) {
        case _COLEMAK:
        case _QWERTY:
        case _COLEMAKDH:
            oled_write_ln_P(PSTR("Base"), false);
            break;
        case _RAISE:
            // No need new line, after 5 characters, a new line is automatically inserted.
            oled_write_P(PSTR("Raise"), false);
            break;
        case _LOWER:
            // No need new line, after 5 characters, a new line is automatically inserted.
            oled_write_P(PSTR("Lower"), false);
            break;
        case _ADJUST:
            oled_write_ln_P(PSTR("Adj"), false);
            break;
        case _NAV:
            oled_write_ln_P(PSTR("Nav"), false);
            break;
        default:
            oled_write_ln_P(PSTR(""), false);
    }

    oled_write_ln_P(PSTR(""), false);

    if (info_display_timeout > 0) {
        switch (oled_display_mode) {
            case OLED_DISPLAY_HUE:
                oled_write_ln_P(PSTR("HUE"), false);
                oled_write_ln(get_u8_str(rgb_matrix_get_hue(), ' '), false);
                break;
            case OLED_DISPLAY_SAT:
                oled_write_ln_P(PSTR("SAT"), false);
                oled_write_ln(get_u8_str(rgb_matrix_get_sat(), ' '), false);
                break;
            case OLED_DISPLAY_VAL:
                oled_write_ln_P(PSTR("VAL"), false);
                oled_write_ln(get_u8_str(rgb_matrix_get_val(), ' '), false);
                break;
            case OLED_DISPLAY_SPD:
                oled_write_ln_P(PSTR("SPD"), false);
                oled_write_ln(get_u8_str(rgb_matrix_get_speed(), ' '), false);
                break;
            case OLED_DISPLAY_CAPS_LOCK:
                oled_write_ln_P(PSTR("CAPS"), false);
                if (host_keyboard_led_state().caps_lock) {
                    oled_write_ln_P(PSTR("ON"), true);
                } else {
                    oled_write_ln_P(PSTR("OFF"), false);
                }
                break;
            case OLED_DISPLAY_SCROLL_LOCK:
                oled_write_P(PSTR("SCRLK"), false);
                if (host_keyboard_led_state().scroll_lock) {
                    oled_write_ln_P(PSTR("ON"), true);
                } else {
                    oled_write_ln_P(PSTR("OFF"), false);
                }
                break;
            case OLED_DISPLAY_NUM_LOCK:
                oled_write_P(PSTR("NUMLK"), false);
                if (host_keyboard_led_state().num_lock) {
                    oled_write_ln_P(PSTR("ON"), true);
                } else {
                    oled_write_ln_P(PSTR("OFF"), false);
                }
                break;
            case OLED_DISPLAY_KANA:
                oled_write_ln_P(PSTR("KANA"), false);
                if (host_keyboard_led_state().kana) {
                    oled_write_ln_P(PSTR("ON"), true);
                } else {
                    oled_write_ln_P(PSTR("OFF"), false);
                }
                break;
            case OLED_DISPLAY_SPONGEBOB:
                oled_write_P(PSTR("sPbOb"), false);
                if (sync_data.sPoNgEbOb_case_active) {
                    oled_write_ln_P(PSTR("ON"), true);
                } else {
                    oled_write_ln_P(PSTR("OFF"), false);
                }
                break;
            case OLED_DISPLAY_OLED_BRIGHTNESS:
                oled_write_ln_P(PSTR("OLED"), false);
                oled_write_ln(get_u8_str(oled_get_brightness(), ' '), false);
                break;
            case OLED_DISPLAY_EXT_MONITOR_BRIGHTNESS:
                oled_write_P(PSTR("BRGHT"), false);
                oled_write_ln(get_u8_str(user_config.ext_monitor_brightness, ' '), false);
                break;
            case OLED_DISPLAY_EXT_MONITOR_CONTRAST:
                oled_write_ln_P(PSTR("CONT"), false);
                oled_write_ln(get_u8_str(user_config.ext_monitor_contrast, ' '), false);
                break;
        }
        info_display_timeout--;
    } else {
        // The text is persistent per line, so we need to clear it with this.
        oled_write_ln_P(PSTR("\n\n"), false);
    }

    oled_set_cursor(0, 11);
    oled_write_P(PSTR("WPM"), false);
    oled_set_cursor(0, 12);
    oled_write(get_u8_str(get_current_wpm(), ' '), false);

    if (sync_data.datetime.month != 0) {
        oled_set_cursor(0, 14);
        oled_write(get_u6_str(sync_data.datetime.month, ' '), false);
        oled_write_P(PSTR("/"), false);
        oled_write(get_u6_str(sync_data.datetime.date, '0'), false);
        switch (sync_data.datetime.day_of_week) {
        case 0:
            oled_write_P(PSTR("Mon"), false);
            break;
        case 1:
            oled_write_P(PSTR("Tue"), false);
            break;
        case 2:
            oled_write_P(PSTR("Wed"), false);
            break;
        case 3:
            oled_write_P(PSTR("Thu"), false);
            break;
        case 4:
            oled_write_P(PSTR("Fri"), false);
            break;
        case 5:
            oled_write_P(PSTR("Sat"), false);
            break;
        case 6:
            oled_write_P(PSTR("Sun"), false);
            break;
        }
    }
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

static void oled_render_anim(void) {
    if (timer_elapsed32(anim_timer) > ANIM_FRAME) {
        anim_timer = timer_read32();
        uint8_t cursor_y = 0;

        oled_set_cursor(0, cursor_y++);
        oled_write_P(PSTR("CPU"), false);
        oled_set_cursor(0, cursor_y++);
        draw_utilization_bar(sync_data.cpu_usage);

        oled_set_cursor(0, cursor_y++);
        oled_write_P(PSTR("CTEMP"), false);
        oled_set_cursor(0, cursor_y++);
        draw_utilization_bar(sync_data.cpu_temp);

        oled_set_cursor(0, cursor_y++);
        oled_write_P(PSTR("RAM"), false);
        oled_set_cursor(0, cursor_y++);
        draw_utilization_bar(sync_data.ram_usage);

        oled_set_cursor(0, cursor_y++);
        oled_write_P(PSTR("GPU"), false);
        oled_set_cursor(0, cursor_y++);
        draw_utilization_bar(sync_data.gpu_usage);

        oled_set_cursor(0, cursor_y++);
        oled_write_P(PSTR("VRAM"), false);
        oled_set_cursor(0, cursor_y++);
        draw_utilization_bar(sync_data.gpu_mem_usage);

        oled_set_cursor(0, cursor_y++);
        oled_write_P(PSTR("GTEMP"), false);
        oled_set_cursor(0, cursor_y++);
        draw_utilization_bar(sync_data.gpu_temp);

        oled_set_cursor(0, cursor_y++);
        oled_write_P(PSTR("GPU1"), false);
        oled_set_cursor(0, cursor_y++);
        draw_utilization_bar(sync_data.gpu1_usage);

        if (sync_data.datetime.month != 0) {
            oled_set_cursor(0, cursor_y++);
            oled_write(get_u6_str(sync_data.datetime.hour, '0'), false);
            oled_write_P(PSTR(":"), false);
            oled_write(get_u6_str(sync_data.datetime.minute, '0'), false);
            oled_write(get_u6_str(sync_data.datetime.second, '0'), false);
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
        print_status_narrow();
    } else {
        oled_render_anim();
    }
    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        sync_data.last_keypress_timer = sync_timer_read32();
    }
    switch (keycode) {
        case KC_QWERTY:
            if (record->event.pressed) {
                set_single_persistent_default_layer(_QWERTY);
            }
            return false;
        case KC_COLEMAK:
            if (record->event.pressed) {
                set_single_persistent_default_layer(_COLEMAK);
            }
            return false;
        case KC_COLEMAKDH:
            if (record->event.pressed) {
                set_single_persistent_default_layer(_COLEMAKDH);
            }
            return false;
        case KC_LOWER:
            if (record->event.pressed) {
                layer_on(_LOWER);
                update_tri_layer(_LOWER, _RAISE, _ADJUST);
            } else {
                layer_off(_LOWER);
                update_tri_layer(_LOWER, _RAISE, _ADJUST);
            }
            return false;
        case KC_RAISE:
            if (record->event.pressed) {
                layer_on(_RAISE);
                update_tri_layer(_LOWER, _RAISE, _ADJUST);
            } else {
                layer_off(_RAISE);
                update_tri_layer(_LOWER, _RAISE, _ADJUST);
            }
            return false;
        case KC_ADJUST:
            if (record->event.pressed) {
                layer_on(_ADJUST);
            } else {
                layer_off(_ADJUST);
            }
            return false;
        case KC_NAV:
            if (record->event.pressed) {
                layer_on(_NAV);
                oled_display_mode = OLED_DISPLAY_NUM_LOCK;
                info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            } else {
                layer_off(_NAV);
            }
            return false;
        case KC_D_MUTE:
            if (record->event.pressed) {
                register_mods(mod_config(MOD_MEH));
                register_code(KC_UP);
            } else {
                unregister_mods(mod_config(MOD_MEH));
                unregister_code(KC_UP);
            }
            return false;
        case KC_BSPC:
            if (record->event.pressed && (mod_state & MOD_MASK_ALT)) {
                tap_code(KC_CAPS);
                oled_display_mode = OLED_DISPLAY_CAPS_LOCK;
                info_display_timeout = OLED_INFO_DISPLAY_DURATION;
                return false;
            }
            is_backspace_down = record->event.pressed;
            return true;
    }

    if (record->event.pressed) {
        if (sync_data.sPoNgEbOb_case_active) {
            if (keycode >= KC_A && keycode <= KC_Z) {
                sPoNgEbOb_state = !sPoNgEbOb_state;
                if (sPoNgEbOb_state) {
                    tap_code16(S(keycode));
                    return false;
                }
            } else if (keycode == KC_LSFT || keycode == KC_RSFT) {
                sPoNgEbOb_state = !sPoNgEbOb_state;
                return true;
            }
        }
        switch (keycode) {
        case RGB_HUI:
        case RGB_HUD:
            oled_display_mode = OLED_DISPLAY_HUE;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            return true;
        case RGB_SAI:
        case RGB_SAD:
            oled_display_mode = OLED_DISPLAY_SAT;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            return true;
        case RGB_VAI:
        case RGB_VAD:
            oled_display_mode = OLED_DISPLAY_VAL;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            return true;
        case RGB_SPI:
        case RGB_SPD:
            oled_display_mode = OLED_DISPLAY_SPD;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            return true;
        case KC_LCTL:
            if (is_backspace_down) {
                register_code(KC_LCTL);
                tap_code(KC_BSPC);
                unregister_code(KC_LCTL);
                return false;
            }
            return true;
        case KC_SCRL:
            oled_display_mode = OLED_DISPLAY_SCROLL_LOCK;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            return true;
        case KC_NUM:
            oled_display_mode = OLED_DISPLAY_NUM_LOCK;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            return true;
        case KC_INT2:
            oled_display_mode = OLED_DISPLAY_KANA;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            return true;
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
        }
    } else {
        // key up event
        switch (keycode) {
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

// This runs at the end of update loop.
void housekeeping_task_user(void) {
    mod_state = get_mods();

    if (timer_elapsed32(last_synced_time) > FORCE_DATA_SYNC_TIME
        || sync_data.sPoNgEbOb_case_active != last_synced_sPoNgEbOb_active
        || force_sync_retry) {
        perform_data_sync();
    }

    if (timer_elapsed32(send_hid_user_data_timer) > HID_SEND_DATA_DELAY && !hid_user_data_sent) {
        send_hid_user_data();
        hid_user_data_sent = true;
    }

    // update mouse tings
    if (mouse_speed_frames_counter++ > MOUSE_SPEED_UPDATE_INTERVAL) {
        mouse_speed_frames_counter = 0;

        if (mousekeys_status.right) {
            if (mouse_vel_x < MOUSE_SPEED_MAX)
                mouse_vel_x += 2;
        } else if (mousekeys_status.left) {
            if (mouse_vel_x > -MOUSE_SPEED_MAX)
                mouse_vel_x -= 2;
        } else {
            if (mouse_vel_x < 0) {
                mouse_vel_x += -mouse_vel_x / 4 + 1;
            } else if (mouse_vel_x > 0) {
                mouse_vel_x += -mouse_vel_x / 4 - 1;
            }
        }
        if (mousekeys_status.up) {
            if (mouse_vel_y > -MOUSE_SPEED_MAX)
                mouse_vel_y -= 2;
        } else if (mousekeys_status.down) {
            if (mouse_vel_y < MOUSE_SPEED_MAX)
                mouse_vel_y += 2;
        } else {
            if (mouse_vel_y < 0) {
                mouse_vel_y += -mouse_vel_y / 4 + 1;
            } else if (mouse_vel_y > 0) {
                mouse_vel_y += -mouse_vel_y / 4 - 1;
            }
        }
    }

    if (mouse_frames_counter++ > MOUSE_UPDATE_INTERVAL) {
        mouse_frames_counter = 0;
        custom_mouse_report.x = mouse_vel_x;
        custom_mouse_report.y = mouse_vel_y;
        send_mouse_report();
    }
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
            switch (get_highest_layer(layer_state)) {
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
                    oled_display_mode = OLED_DISPLAY_EXT_MONITOR_BRIGHTNESS;
                    info_display_timeout = OLED_INFO_DISPLAY_DURATION;
                } else {
                    user_config.ext_monitor_brightness -= 5;
                    if (user_config.ext_monitor_brightness >= 100) {
                        user_config.ext_monitor_brightness = 0;
                    }
                    oled_display_mode = OLED_DISPLAY_EXT_MONITOR_BRIGHTNESS;
                    info_display_timeout = OLED_INFO_DISPLAY_DURATION;
                }
                eeconfig_update_user(user_config._raw);
                hid_user_data_sent = false;
                send_hid_user_data_timer = timer_read32();
                break;
            case _NAV:
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
                oled_display_mode = OLED_DISPLAY_EXT_MONITOR_CONTRAST;
                info_display_timeout = OLED_INFO_DISPLAY_DURATION;
                eeconfig_update_user(user_config._raw);
                hid_user_data_sent = false;
                send_hid_user_data_timer = timer_read32();
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

        switch (get_highest_layer(layer_state)) {
        case _ADJUST:
            custom_mouse_report.h = clockwise ? 1 : -1;
            break;
        case _RAISE:
            custom_mouse_report.v = clockwise ? -1 : 1;
            break;
        case _LOWER:
            if (clockwise) {
                tap_code(KC_DOWN);
            } else {
                tap_code(KC_UP);
            }
            break;
        case _NAV:
            if (clockwise) {
                tap_code16(C(S(KC_RGHT)));
            } else {
                tap_code16(C(S(KC_LEFT)));
            }
            break;
        default:
                if (clockwise) {
                    tap_code(KC_RGHT);
                } else {
                    tap_code(KC_LEFT);
                }
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

    if (get_highest_layer(layer_state) == _NAV) {
        // display num_lock state
        uint8_t rgb_val = rgb_matrix_get_val();
        if (host_keyboard_led_state().num_lock) {
            rgb_matrix_set_color(32, 0, rgb_val / 2, rgb_val / 4);
        } else {
            rgb_matrix_set_color(32, rgb_val / 2, rgb_val / 8, 0);
        }
        // display scroll_lock state
        if (host_keyboard_led_state().scroll_lock) {
            rgb_matrix_set_color(36+18, 0, rgb_val / 2, rgb_val / 4);
        } else {
            rgb_matrix_set_color(36+18, rgb_val / 2, rgb_val / 8, 0);
        }
    }
    return true;
}

void raw_hid_receive(uint8_t *data, uint8_t length) {
    if(data[0] == 1) {
        // First data byte 1 means we are receiving host utilization data.
        sync_data.cpu_usage = data[1];
        sync_data.cpu_temp = data[2];
        sync_data.ram_usage = data[3];
        sync_data.gpu_usage = data[4];
        sync_data.gpu_mem_usage = data[5];
        sync_data.gpu_temp = data[6];
        sync_data.gpu1_usage = data[7];

        sync_data.datetime.month = data[10];
        sync_data.datetime.date = data[11];
        sync_data.datetime.day_of_week = data[12];
        sync_data.datetime.hour = data[13];
        sync_data.datetime.minute = data[14];
        sync_data.datetime.second = data[15];

        perform_data_sync();
    }
}
