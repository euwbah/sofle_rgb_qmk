
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

#include QMK_KEYBOARD_H

// in led update frames
#define OLED_INFO_DISPLAY_DURATION 50

#define HSV_OVERRIDE_HELP(h, s, v, Override) h, s , Override
#define HSV_OVERRIDE(hsv, Override) HSV_OVERRIDE_HELP(hsv,Override)

// Light combinations
#define SET_INDICATORS(hsv) \
	{0, 1, HSV_OVERRIDE_HELP(hsv, INDICATOR_BRIGHTNESS)}, \
    {35+0, 1, hsv}
#define SET_UNDERGLOW(hsv) \
	{1, 6, hsv}, \
    {35+1, 6,hsv}
#define SET_NUMPAD(hsv)     \
	{35+15, 5, hsv},\
	  {35+22, 3, hsv},\
	  {35+27, 3, hsv}
#define SET_NUMROW(hsv) \
	{10, 2, hsv}, \
		{20, 2, hsv}, \
		{30, 2, hsv}, \
	  {35+ 10, 2, hsv}, \
	  {35+ 20, 2, hsv}, \
	  {35+ 30, 2, hsv}
#define SET_INNER_COL(hsv)	\
	{33, 4, hsv}, \
	  {35+ 33, 4, hsv}

#define SET_OUTER_COL(hsv) \
	{7, 4, hsv}, \
	  {35+ 7, 4, hsv}
#define SET_THUMB_CLUSTER(hsv) 	\
	{25, 2, hsv}, \
	  {35+ 25, 2, hsv}
#define SET_LAYER_ID(hsv) 	\
	{0, 1, HSV_OVERRIDE_HELP(hsv, INDICATOR_BRIGHTNESS)}, \
    {35+0, 1, HSV_OVERRIDE_HELP(hsv, INDICATOR_BRIGHTNESS)}, \
		{1, 6, hsv}, \
    {35+1, 6, hsv}, \
		{7, 4, hsv}, \
	  {35+ 7, 4, hsv}, \
		{25, 2, hsv}, \
	  {35+ 25, 2, hsv}


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
    KC_D_MUTE
};

enum oled_display_mode_enum {
    OLED_DISPLAY_HUE = 0,
    OLED_DISPLAY_SAT,
    OLED_DISPLAY_VAL,
    OLED_DISPLAY_SPD
};

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
                 KC_LALT, KC_LGUI, KC_LOWER, KC_LSFT,  KC_SPC   ,  KC_NAV, KC_SPC ,  KC_RAISE, KC_LPRN, KC_RPRN
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
  KC_F1,  KC_F2,  KC_F3,   A(KC_F4),   KC_F5,   S(KC_F6),                  KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_F13,   KC_F14,  KC_F15,  KC_F16,  KC_F17,  KC_F18,                   KC_F19,  KC_F20, KC_F21,   KC_F22,    KC_F23, KC_F24,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  _______, C(S(KC_ESC)),C(A(KC_DEL)), KC_NO,KC_WH_U,KC_PGUP,              KC_BRIU, KC_VOLU, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  _______,  KC_NO,  KC_NO,   KC_NO,   KC_WH_D, KC_PGDN,_______,    _______,KC_BRID,  KC_VOLD,  KC_NO,   KC_NO,   KC_NO,    _______,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
                 _______, _______, _______, _______, QK_BOOT,     _______, _______, _______, _______, _______
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),

// KC_LANG1 switches to hiragana when in Japanese IME mode,
// KC_LANG2 switches to half-width alphanumeric.
[_RAISE] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  KC_GRV,    KC_F2,  KC_F3,   KC_F4,   KC_F5,   KC_F6,                      KC_F7,  KC_F8  , KC_F9,  KC_F10 , KC_F11 , KC_F12,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  KC_TILD,  KC_1,   KC_2,    KC_3,    KC_4,     KC_5,                      KC_6,   KC_7,    KC_8,    KC_9,     KC_0,    KC_BSLS,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  _______, KC_EXLM,  KC_AT,  KC_HASH,  KC_DLR,  KC_PERC,                 KC_CIRC,   KC_LCBR, KC_RCBR, KC_LBRC, KC_RBRC, KC_PIPE,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  _______, KC_QUES, _______, KC_LPRN, KC_RPRN,  KC_LT,_______,    _______,KC_GT,   KC_PLUS, KC_MINS, KC_ASTR, KC_SLSH, KC_QUES,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
                 _______, _______, _______, _______, _______,     KC_LNG1, KC_LNG2, _______, KC_EQL,  KC_AMPR
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),

  [_ADJUST] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  EE_CLR,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  QK_BOOT, XXXXXXX,XXXXXXX, XXXXXXX, XXXXXXX, KC_COLEMAKDH,                    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  RGB_TOG, RGB_HUI,RGB_SAI, RGB_VAI, RGB_SPI, KC_COLEMAK,             C(G(KC_LEFT)),KC_NO,KC_NO,C(G(KC_RGHT)),XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  RGB_MOD, RGB_HUD,RGB_SAD, RGB_VAD, RGB_SPD, KC_QWERTY,XXXXXXX,   XXXXXXX, XXXXXXX, KC_MPRV, KC_MPLY, KC_MNXT, XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
                   _______, _______, _______, _______, _______,     _______, _______, _______, _______, _______
    //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),

[_NAV] = LAYOUT(
  //,------------------------------------------------.                    ,---------------------------------------------------.
  _______, KC_PIPE, KC_LPRN, KC_RPRN, KC_AMPR, KC_NUM,                   KC_BRK, KC_PSCR,  _______, _______, XXXXXXX, XXXXXXX,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  _______,  KC_CIRC, KC_P7,  KC_P8,   KC_P9,  KC_PERC,                   KC_PGUP, KC_HOME, KC_UP,   KC_END, _______, XXXXXXX,
  //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
  _______,  KC_MINS, KC_P4,  KC_P5,   KC_P6,  KC_SLSH,                   KC_PGDN, KC_LEFT,  KC_DOWN, KC_RGHT, _______, _______,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
  _______,  KC_PLUS, KC_P1, KC_P2,    KC_P3,  KC_ASTR,_______,    _______,KC_DEL, KC_INS,  KC_PSCR,  KC_SCRL, _______, _______,
  //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
              KC_P0, KC_PDOT, KC_EQL, _______, _______,          _______, _______, _______ , _______, _______
  //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
),
};

static uint8_t mod_state;
static bool is_backspace_down = false;

// NOTE: Remember u can't assign strings directly to char arrays, you need to strcpy.
// For info display, a short string describing the value being displayed
static uint8_t oled_display_mode = OLED_DISPLAY_HUE;

static uint8_t info_display_timeout = 0; // in led update frames

// Now define the array of layers. Later layers take precedence

// // QWERTY,
// // Light on inner column and underglow
// const rgblight_segment_t PROGMEM layer_qwerty_lights[] = RGBLIGHT_LAYER_SEGMENTS(
//   SET_LAYER_ID(HSV_RED)

// );
// const rgblight_segment_t PROGMEM layer_colemakdh_lights[] = RGBLIGHT_LAYER_SEGMENTS(
//   SET_LAYER_ID(HSV_PINK)
// );

// // _NUM,
// // Light on outer column and underglow
// const rgblight_segment_t PROGMEM layer_num_lights[] = RGBLIGHT_LAYER_SEGMENTS(
// 	SET_LAYER_ID(HSV_TEAL)

// );
// // _SYMBOL,
// // Light on inner column and underglow
// const rgblight_segment_t PROGMEM layer_symbol_lights[] = RGBLIGHT_LAYER_SEGMENTS(
// 	SET_LAYER_ID(HSV_BLUE)

//     );
// // _COMMAND,
// // Light on inner column and underglow
// const rgblight_segment_t PROGMEM layer_command_lights[] = RGBLIGHT_LAYER_SEGMENTS(
//   SET_LAYER_ID(HSV_PURPLE)
// );

// //_NUMPAD
// const rgblight_segment_t PROGMEM layer_numpad_lights[] = RGBLIGHT_LAYER_SEGMENTS(
// 	SET_INDICATORS(HSV_ORANGE),
//     SET_UNDERGLOW(HSV_ORANGE),
// 	SET_NUMPAD(HSV_BLUE),
//     {7, 4, HSV_ORANGE},
//     {25, 2, HSV_ORANGE},
//     {35+6, 4, HSV_ORANGE},
//     {35+25, 2, HSV_ORANGE}
//     );
// // _SWITCHER   // light up top row
// const rgblight_segment_t PROGMEM layer_switcher_lights[] = RGBLIGHT_LAYER_SEGMENTS(
// 	SET_LAYER_ID(HSV_GREEN),
// 	SET_NUMROW(HSV_GREEN)
// );

// const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
//     layer_qwerty_lights,
// 	layer_num_lights,// overrides layer 1
// 	layer_symbol_lights,
//     layer_command_lights,
// 	layer_numpad_lights,
// 	layer_switcher_lights,  // Overrides other layers
// 	layer_colemakdh_lights
// );

// layer_state_t layer_state_set_user(layer_state_t state) {
// 	rgblight_set_layer_state(0, layer_state_cmp(state, _DEFAULTS) && layer_state_cmp(default_layer_state,_QWERTY));
// 	rgblight_set_layer_state(7, layer_state_cmp(state, _DEFAULTS) && layer_state_cmp(default_layer_state,_COLEMAKDH));


// 	rgblight_set_layer_state(1, layer_state_cmp(state, _LOWER));
// 	rgblight_set_layer_state(2, layer_state_cmp(state, _RAISE));
// 	rgblight_set_layer_state(3, layer_state_cmp(state, _ADJUST));
// 	rgblight_set_layer_state(4, layer_state_cmp(state, _NAV));
// 	rgblight_set_layer_state(5, layer_state_cmp(state, _SWITCH));
//     return state;
// }



// This runs right atfer initializing the individual components of the keyboard
// void keyboard_post_init_user(void) {
//     Enable the LED layers
//     rgblight_layers = my_rgb_layers;
// 	rgblight_mode(10);// haven't found a way to set this in a more useful way
// }

// Instead of bongo cat, which uses a lot of memory, uncomment & use this function to display a static logo instead.
// static void render_logo(void) {
//     static const char PROGMEM qmk_logo[] = {
//         0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94,
//         0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4,
//         0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0x00
//     };

//     oled_write_P(qmk_logo, false);
// }

static void print_status_narrow(void) {
    // Print current mode
    oled_write_P(PSTR("\n"), false);
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
        }
        info_display_timeout--;
    } else {
        // The text is persistent per line, so we need to clear it with this.
        oled_write_ln_P(PSTR("\n\n"), false);
    }
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (is_keyboard_master()) {
        return OLED_ROTATION_270;
    }
    return rotation;
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

#define ANIM_FRAME 300

static uint32_t anim_timer = 0;
static uint32_t anim_sleep_timer = 0;

static void oled_render_anim(void) {

    const uint8_t speed = get_current_wpm();

    void oled_render_speed(void) {
        oled_set_cursor(0, 0);
        oled_write_P(PSTR("WPM"), false);
        oled_set_cursor(0, 1);
        oled_write(get_u8_str(speed, ' '), false);
    }

    if (speed != 0) {
        oled_on();
        anim_sleep_timer = timer_read32();
    } else {
        if (timer_elapsed32(anim_sleep_timer) > OLED_TIMEOUT) {
            oled_off();
            return; // Return cuz any write to the oled will turn it on again.
        }
    }

    if (timer_elapsed32(anim_timer) > ANIM_FRAME) {
        anim_timer = timer_read32();
        oled_set_cursor(4, 2);
        uint8_t bar_length = speed / 5;
        draw_bar(bar_length, 0xFC);
        clear_bar(40-bar_length);
        oled_render_speed();
    }
}

bool oled_task_user(void) {
    mod_state = get_mods();
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
                return false;
            }
            is_backspace_down = record->event.pressed;
            return true;
        case KC_LCTL:
            if (record->event.pressed) {
                if (is_backspace_down) {
                    register_code(KC_LCTL);
                    tap_code(KC_BSPC);
                    unregister_code(KC_LCTL);
                    return false;
                }
            }
            return true;
    }

    if (record->event.pressed) {
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
        }
    }
    return true;
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

        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
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
    return true;
}
