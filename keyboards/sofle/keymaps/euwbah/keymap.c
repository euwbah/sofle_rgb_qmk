
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
    sPoNgEbOb, // toggle sPoNgEbOb case
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
};

// sent using the USER_SYNC_HOST_UTIL transaction ID.
// the RPC_M2S_BUFFER_SIZE defaults to 32 (can increase in config.h), so we can send up to 32 uint8_t values.
typedef struct _m_to_s_host_util_t {
    uint8_t cpu_usage;
    uint8_t cpu_temp;
    uint8_t ram_usage;
    uint8_t gpu_usage;
    uint8_t gpu_mem_usage;
    uint8_t gpu_temp;
    uint8_t gpu1_usage;
} m_to_s_host_util_t;

typedef struct _m_to_s_data {
    bool sPoNgEbOb_case_active;
} m_to_s_data_t;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  KC_ESC,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                      KC_6,    KC_7,   KC_8,    KC_9,    KC_0,    KC_GRV,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_TAB,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                      KC_Y,    KC_U,   KC_I,    KC_O,    KC_P,    KC_UNDS,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_BSPC,  KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                      KC_H,    KC_J,   KC_K,    KC_L,    KC_SCLN, KC_QUOT,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  KC_LCTL,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,  KC_MUTE,  KC_D_MUTE,KC_N,    KC_M,   KC_COMM, KC_DOT,  KC_SLSH, KC_ENT,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
                 KC_LALT, KC_LGUI, KC_LOWER, KC_LSFT,  KC_SPC,    KC_NAV,  KC_SPC,  KC_RAISE, KC_LPRN,  KC_RPRN
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),

[_COLEMAK] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  KC_TRNS,  KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                      KC_6,    KC_7,   KC_8,    KC_9,    KC_0,    KC_TRNS,
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
  KC_TRNS,  KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                      KC_6,    KC_7,   KC_8,    KC_9,    KC_0,    KC_TRNS,
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
  KC_F1,    KC_F2,  KC_F3,   A(KC_F4), KC_F5,   S(KC_F6),                 KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  A(KC_F12),
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_F13,   KC_F14, KC_F15,  KC_F16,   KC_F17,  KC_F18,                   KC_F19,  KC_F20,  KC_F21,  KC_F22,  KC_F23,  KC_F24,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  _______,C(S(KC_ESC)),C(A(KC_DEL)),KC_PSCR,KC_WH_U,KC_WH_L,                KC_BRIU, KC_VOLU, KC_BTN1, KC_MS_U, KC_BTN2, KC_BTN4,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  _______,  KC_NO,  KC_NO,   KC_NO,    KC_WH_D, KC_WH_R,_______, _______, KC_BRID, KC_VOLD, KC_MS_L, KC_MS_D, KC_MS_R, KC_BTN5,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
            C(G(KC_LEFT)),C(G(KC_RGHT)),_______,_______,QK_BOOT, _______, KC_BTN8, _______, KC_BTN7, KC_BTN3
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),

// KC_LANG1 switches to hiragana when in Japanese IME mode. Also hangul/english toggle key.
// KC_LANG2 switches to half-width alphanumeric. Also hanja conversion key.
[_RAISE] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  KC_GRV,    KC_F2,  KC_F3,   KC_F4,   KC_F5,   KC_F6,                      KC_F7,  KC_F8  , KC_F9,  KC_F10 , KC_F11 , KC_F12,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_TILD,  KC_1,   KC_2,    KC_3,    KC_4,     KC_5,                      KC_6,   KC_7,    KC_8,    KC_9,     KC_0,    KC_QUES,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  _______, KC_EXLM,  KC_AT,  KC_HASH,  KC_DLR,  KC_PERC,                 KC_CIRC,   KC_LCBR, KC_RCBR, KC_LBRC, KC_RBRC, KC_BSLS,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  _______, KC_QUES, _______, KC_LPRN, KC_RPRN,  KC_LT,_______,    _______,KC_GT,   KC_PLUS, KC_MINS, KC_ASTR, KC_SLSH, KC_PIPE,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
                 _______, _______, _______, _______, _______,     KC_LNG1, KC_LNG2, _______, KC_EQL,  KC_AMPR
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),

  [_ADJUST] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  EE_CLR,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  QK_BOOT, XXXXXXX,XXXXXXX, XXXXXXX, XXXXXXX, KC_COLEMAKDH,               XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  RGB_TOG, RGB_HUI,RGB_SAI, RGB_VAI, RGB_SPI, KC_COLEMAK,                 XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  RGB_MOD, RGB_HUD,RGB_SAD, RGB_VAD, RGB_SPD, KC_QWERTY,XXXXXXX,   XXXXXXX, XXXXXXX, KC_MPRV, KC_MPLY, KC_MNXT, XXXXXXX, XXXXXXX,
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
  _______, KC_PIPE, KC_LPRN, KC_RPRN, KC_AMPR, KC_NUM,                   KC_BRK, KC_PSCR,  _______, _______, XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  _______,  KC_CIRC, KC_P7,  KC_P8,   KC_P9,  KC_PERC,                   KC_PGUP, KC_HOME, KC_UP,   KC_END, _______, XXXXXXX,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  _______,  KC_MINS, KC_P4,  KC_P5,   KC_P6,  KC_SLSH,                   KC_PGDN, KC_LEFT,  KC_DOWN, KC_RGHT, _______, _______,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  _______,  KC_PLUS, KC_P1, KC_P2,    KC_P3,  KC_ASTR,_______, sPoNgEbOb,KC_DEL, KC_INS,  KC_PSCR,  KC_SCRL, _______, _______, // toggle sPoNgEbOb case
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
              KC_P0, KC_PDOT, KC_EQL, _______, KC_INT5,          _______, KC_INT1, KC_INT2, KC_INT3, KC_INT4
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),
};

static uint8_t cpu_usage = 0;
static uint8_t cpu_temp = 0;
static uint8_t ram_usage = 0;
static uint8_t gpu_usage = 0;
static uint8_t gpu_mem_usage = 0;
static uint8_t gpu_temp = 0;
static uint8_t gpu1_usage = 0;

static uint8_t mod_state;
static bool is_backspace_down = false;
static bool sPoNgEbOb_case_active = false;
static bool sPoNgEbOb_state = false; // automatically toggles when alphabets and shift is pressed.
static bool last_synced_sPoNgEbOb_active = true;

// NOTE: Remember u can't assign strings directly to char arrays, you need to strcpy.
// For info display, a short string describing the value being displayed
static uint8_t oled_display_mode = OLED_DISPLAY_HUE;

static uint8_t info_display_timeout = 0; // in led update frames

void user_sync_host_util_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    const m_to_s_host_util_t* host_util = (const m_to_s_host_util_t*)in_data;
    cpu_usage = host_util->cpu_usage;
    cpu_temp = host_util->cpu_temp;
    ram_usage = host_util->ram_usage;
    gpu_usage = host_util->gpu_usage;
    gpu_mem_usage = host_util->gpu_mem_usage;
    gpu_temp = host_util->gpu_temp;
    gpu1_usage = host_util->gpu1_usage;
}

void user_sync_data_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    const m_to_s_data_t* data = (const m_to_s_data_t*)in_data;
    sPoNgEbOb_case_active = data->sPoNgEbOb_case_active;
}

void keyboard_post_init_user() {
    transaction_register_rpc(USER_SYNC_HOST_UTIL, user_sync_host_util_slave_handler);
    transaction_register_rpc(USER_SYNC_DATA, user_sync_data_slave_handler);
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (is_keyboard_master()) {
        return OLED_ROTATION_270;
    }
    return OLED_ROTATION_270;
    // return rotation;
}

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
            oled_write_ln_P(PSTR("CmkDH"), false);
            break;

        default:
            oled_write_ln_P(PSTR("Undef"), false);
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
            oled_write_P(PSTR("Undef"), false);
    }

    oled_write_P(PSTR("\n"), false);

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
                if (sPoNgEbOb_case_active) {
                    oled_write_ln_P(PSTR("ON"), true);
                } else {
                    oled_write_ln_P(PSTR("OFF"), false);
                }
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

#define ANIM_FRAME 250

static uint32_t anim_timer = 0;
// static uint32_t anim_sleep_timer = 0;

static void oled_render_anim(void) {
    // const uint8_t speed = get_current_wpm();

    // if (speed != 0) {
    //     oled_on();
    //     anim_sleep_timer = timer_read32();
    // } else {
    //     if (timer_elapsed32(anim_sleep_timer) > OLED_TIMEOUT) {
    //         oled_off();
    //         return; // Return cuz any write to the oled will turn it on again.
    //     }
    // }

    if (timer_elapsed32(anim_timer) > ANIM_FRAME) {
        anim_timer = timer_read32();
        uint8_t cursor_y = 0;

        oled_set_cursor(0, cursor_y++);
        oled_write_P(PSTR("CPU"), false);
        oled_set_cursor(0, cursor_y++);
        draw_utilization_bar(cpu_usage);

        oled_set_cursor(0, cursor_y++);
        oled_write_P(PSTR("CTEMP"), false);
        oled_set_cursor(0, cursor_y++);
        draw_utilization_bar(cpu_temp);

        oled_set_cursor(0, cursor_y++);
        oled_write_P(PSTR("RAM"), false);
        oled_set_cursor(0, cursor_y++);
        draw_utilization_bar(ram_usage);

        oled_set_cursor(0, cursor_y++);
        oled_write_P(PSTR("GPU"), false);
        oled_set_cursor(0, cursor_y++);
        draw_utilization_bar(gpu_usage);

        oled_set_cursor(0, cursor_y++);
        oled_write_P(PSTR("VRAM"), false);
        oled_set_cursor(0, cursor_y++);
        draw_utilization_bar(gpu_mem_usage);

        oled_set_cursor(0, cursor_y++);
        oled_write_P(PSTR("GTEMP"), false);
        oled_set_cursor(0, cursor_y++);
        draw_utilization_bar(gpu_temp);

        oled_set_cursor(0, cursor_y++);
        oled_write_P(PSTR("GPU1"), false);
        oled_set_cursor(0, cursor_y++);
        draw_utilization_bar(gpu1_usage);
    }
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        print_status_narrow();
    } else {
        oled_render_anim();
    }
    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
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
        if (sPoNgEbOb_case_active) {
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
        case sPoNgEbOb:
            sPoNgEbOb_case_active = !sPoNgEbOb_case_active;
            oled_display_mode = OLED_DISPLAY_SPONGEBOB;
            info_display_timeout = OLED_INFO_DISPLAY_DURATION;
            return false;
        }
    }
    return true;
}

void housekeeping_task_user(void) {
    mod_state = get_mods();

    if (is_keyboard_master()) {
        if (sPoNgEbOb_case_active != last_synced_sPoNgEbOb_active) {
            m_to_s_data_t data = {sPoNgEbOb_case_active};
            transaction_rpc_send(USER_SYNC_DATA, sizeof(data), &data);
            last_synced_sPoNgEbOb_active = sPoNgEbOb_case_active;
        }
    }
}

// The rotary encoders I bought send 2 pulses per detent, so we need to halve the signals.
// A value of 0 represents no pulse received, requires 2 pulses in the same direction to send an action.
// A value of 1 represents 1 pulse received clockwize, -1 represents 1 pulse anti-clockwise.
static int8_t encoder_1_state = 0;
static int8_t encoder_2_state = 0;

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        if ((clockwise && encoder_1_state == 1) || (!clockwise && encoder_1_state == -1)) {
            encoder_1_state = 0;
        } else {
            encoder_1_state = clockwise ? 1 : -1;
            return false;
        }

        if (mod_state & (MOD_MASK_ALT | MOD_MASK_CTRL)) {
            if (clockwise) {
                tap_code(KC_TAB);
            } else {
                tap_code16(S(KC_TAB));
            }
            return false;
        } else {
            if (clockwise) {
                tap_code(KC_VOLU);
            } else {
                tap_code(KC_VOLD);
            }
        }
    } else if (index == 1) {
        if ((clockwise && encoder_2_state == 1) || (!clockwise && encoder_2_state == -1)) {
            encoder_2_state = 0;
        } else {
            encoder_2_state = clockwise ? 1 : -1;
            return false;
        }

        switch (get_highest_layer(layer_state)) {
        // case _COLEMAK:
        // case _QWERTY:
        // case _COLEMAKDH:
        //     if (clockwise) {
        //         tap_code(KC_PGDN);
        //     } else {
        //         tap_code(KC_PGUP);
        //     }
        //     return false;
        case _RAISE:
            if (clockwise) {
                tap_code(KC_RIGHT);
            } else {
                tap_code(KC_LEFT);
            }
            return false;
        case _LOWER:
            if (clockwise) {
                tap_code(KC_DOWN);
            } else {
                tap_code(KC_UP);
            }
            return false;
        case _NAV:
            if (clockwise) {
                register_mods(MOD_MASK_CS);
                tap_code(KC_RGHT);
                unregister_mods(MOD_MASK_CS);
            } else {
                register_mods(MOD_MASK_CS);
                tap_code(KC_LEFT);
                unregister_mods(MOD_MASK_CS);
            }
            return false;
        default:
                if (clockwise) {
                    tap_code(KC_WH_D);
                } else {
                    tap_code(KC_WH_U);
                }
            return false;
		}
    }
    return true;
}

bool rgb_matrix_indicators_user(void) {
    if (host_keyboard_led_state().caps_lock) {
        uint8_t rgb_val = rgb_matrix_get_val();
        rgb_matrix_set_color(0, 0, rgb_val / 3, rgb_val / 3);
        rgb_matrix_set_color(9, 0, rgb_val, rgb_val);
    } else {
        rgb_matrix_set_color(0, 0, 0, 0);
    }

    if (sPoNgEbOb_case_active) {
        uint8_t rgb_val = rgb_matrix_get_val();
        rgb_matrix_set_color(36, rgb_val, rgb_val / 2, 0);
        rgb_matrix_set_color(0, rgb_val, rgb_val / 2, 0);
    } else {
        rgb_matrix_set_color(36, 0, 0, 0);
        rgb_matrix_set_color(0, 0, 0, 0);
    }
    return true;
}

void raw_hid_receive(uint8_t *data, uint8_t length) {
    if(data[0] == 1) {
        // First data byte 1 means we are receiving host utilization data.
        cpu_usage = data[1];
        cpu_temp = data[2];
        ram_usage = data[3];
        gpu_usage = data[4];
        gpu_mem_usage = data[5];
        gpu_temp = data[6];
        gpu1_usage = data[7];

        m_to_s_host_util_t host_util_data = {cpu_usage, cpu_temp, ram_usage, gpu_usage, gpu_mem_usage, gpu_temp, gpu1_usage};
        // no need for bidirectional communication, so use transaction_rpc_send instead of
        // transaction_rpc_exec
        transaction_rpc_send(USER_SYNC_HOST_UTIL, sizeof(host_util_data), &host_util_data);
    }
}
