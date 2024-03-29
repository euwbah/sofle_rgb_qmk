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
  #pragma once


//#define USE_MATRIX_I2C

/* Select hand configuration */

///https://thomasbaart.nl/2018/12/01/reducing-firmware-size-in-qmk/


#define MASTER_LEFT
// #define MASTER_RIGHT
// #define EE_HANDS

#define CUSTOM_FONT

#define CUSTOM_LAYER_READ //if you remove this it causes issues - needs better guarding

// Reduce firmware size. Comment out these lines if u need to use these features.
#define LAYER_STATE_8BIT // comment this out if you need to use more than 8 layers.
#define NO_ACTION_TAPPING // For layer-tap/mod-tap.
#define NO_ACTION_ONESHOT // For one-shot keys & one-shot mods

#define FORCE_NKRO

#define NO_ACTION_MACRO
#define NO_ACTION_FUNCTION

// Denotes the maximum duration between taps/releases of the double tap action of the
// NAV key measured in (left) OLED frames. 1 frame is approximately 100ms ish?
#define DBL_TAP_DUR_FRAMES 3

// For syncing data across two halves of keyboard.
#define SPLIT_TRANSPORT_MIRROR
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_MODS_ENABLE
#define SPLIT_WPM_ENABLE
#define SPLIT_LED_STATE_ENABLE

// There's an issue with OLED timeouts on split keyboards - the secondary OLED dies after the set timeout.
// Disable the QMK's built-in OLED timeout feature, and impl a custom one.
#define OLED_TIMEOUT 0
#define CUSTOM_OLED_TIMEOUT 300000
#undef SPLIT_ACTIVITY_ENABLE

#define WPM_SAMPLE_SECONDS 2
#define WPM_SAMPLE_PERIODS 18

// These are default HID Usage page and IDs in QMK.
#define RAW_USAGE_PAGE 0xFF60
#define RAW_USAGE_ID 0x61
#define HID_SEND_DATA_DELAY 100 // How to long wait after data change before updating HID data (ms).

// transaction ID for custom RPC communication of host utilization stats from master to slave
#define SPLIT_TRANSACTION_IDS_USER USER_SYNC_DATA
#define FORCE_DATA_SYNC_TIME 1000 // User config: how often to force data sync in milliseconds.
#define RPC_M2S_BUFFER_SIZE 32 // RPC message size limit

// Implementing bespoke mousekey functions from report.h directly
#define MOUSE_ENABLE // for has_mouse_report_changed() function in report.h
#undef MOUSE_SHARED_EP // We're not using shared USB endpoint for mouse, changes contents of report_mouse_t struct.
#define MOUSE_UPDATE_INTERVAL 3 // How often to update mouse position in number of matrix scan iterations
#define MOUSE_SPEED_UPDATE_INTERVAL 10 // How often to update mouse speed in number of matrix scan iterations
#define MOUSE_SPEED_MAX 25

// #define MOUSEKEY_DELAY 10
// #define MOUSEKEY_INTERVAL 10
// #define MOUSEKEY_MOVE_DELTA 2
// #define MOUSEKEY_MAX_SPEED 10
// #define MOUSEKEY_TIME_TO_MAX 30
// #define MOUSEKEY_WHEEL_DELAY 80
// #define MOUSEKEY_WHEEL_INTERVAL 20
// #define MOUSEKEY_WHEEL_MAX_SPEED 1

#define ENCODER_DIRECTION_FLIP
#ifdef ENCODER_RESOLUTION
    #undef ENCODER_RESOLUTION
#endif
#define ENCODER_RESOLUTION 4
#define ENCODER_DEFAULT_POS 3

#define RGBLIGHT_SLEEP
//
// #define RGBLIGHT_LAYERS

/* ws2812 RGB LED */
#define WS2812_DI_PIN D3


#ifdef RGB_MATRIX_ENABLE
#define RGB_MATRIX_LED_COUNT 72
#endif

#undef RGBLED_NUM

//#define RGBLIGHT_EFFECT_BREATHING
// #define RGBLIGHT_EFFECT_RAINBOW_MOOD
//#define RGBLIGHT_EFFECT_RAINBOW_SWIRL
//#define RGBLIGHT_EFFECT_SNAKE
// #define RGBLIGHT_EFFECT_KNIGHT
//#define RGBLIGHT_EFFECT_CHRISTMAS
//#define RGBLIGHT_EFFECT_STATIC_GRADIENT
//#define RGBLIGHT_EFFECT_RGB_TEST
//#define RGBLIGHT_EFFECT_ALTERNATING
//#define RGBLIGHT_EFFECT_TWINKLE

#define RGBLED_NUM 72
//#define RGBLED_SPLIT
#define RGB_MATRIX_SPLIT { 36, 36 }
#define RGBLED_SPLIT { 36, 36 } // haven't figured out how to use this yet

//#define RGBLED_NUM 30
#define RGBLIGHT_LIMIT_VAL 120
#define RGBLIGHT_HUE_STEP 10
#define RGBLIGHT_SAT_STEP 17
#define RGBLIGHT_VAL_STEP 17

#   define RGB_MATRIX_KEYPRESSES // reacts to keypresses
// #   define RGB_MATRIX_KEYRELEASES // reacts to keyreleases (instead of keypresses)
#   define RGB_DISABLE_WHEN_USB_SUSPENDED // turn off effects when suspended
#   define RGB_MATRIX_FRAMEBUFFER_EFFECTS
// #   define RGB_MATRIX_LED_PROCESS_LIMIT (RGB_MATRIX_LED_COUNT + 4) / 5 // limits the number of LEDs to process in an animation per task run (increases keyboard responsiveness)
// #   define RGB_MATRIX_LED_FLUSH_LIMIT 16 // limits in milliseconds how frequently an animation will update the LEDs. 16 (16ms) is equivalent to limiting to 60fps (increases keyboard responsiveness)
#    define RGB_MATRIX_MAXIMUM_BRIGHTNESS 170 // limits maximum brightness of LEDs to 150 out of 255. Higher may cause the controller to crash.

// #define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_MULTISPLASH

#    define RGB_MATRIX_HUE_STEP 8
#    define RGB_MATRIX_SAT_STEP 8
#    define RGB_MATRIX_VAL_STEP 8
#    define RGB_MATRIX_SPD_STEP 10

/* Disable the animations you don't want/need.  You will need to disable a good number of these    *
 * because they take up a lot of space.  Disable until you can successfully compile your firmware. */
//  #   define ENABLE_RGB_MATRIX_ALPHAS_MODS
//  #   define ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
//  #   define ENABLE_RGB_MATRIX_BREATHING
 #   define ENABLE_RGB_MATRIX_CYCLE_ALL
//  #   define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
//  #   define ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
//  #   define ENABLE_RGB_MATRIX_CYCLE_OUT_IN
//  #   define ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
//  #   define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
//  #   define ENABLE_RGB_MATRIX_DUAL_BEACON
//  #   define ENABLE_RGB_MATRIX_RAINBOW_BEACON
//  #   define ENABLE_RGB_MATRIX_RAINBOW_PINWHEELS
//  #   define ENABLE_RGB_MATRIX_RAINDROPS
//  #   define ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
//  #   define ENABLE_RGB_MATRIX_TYPING_HEATMAP
//  #   define ENABLE_RGB_MATRIX_DIGITAL_RAIN
//  #   define ENABLE_RGB_MATRIX_SOLID_REACTIVE
//  #   define ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE
//  #   define ENABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE
//  #   define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
//  #   define ENABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS
 #   define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTICROSS
//  #   define ENABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS
//  #   define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS
//  #   define ENABLE_RGB_MATRIX_SPLASH
 #   define ENABLE_RGB_MATRIX_MULTISPLASH
//  #   define ENABLE_RGB_MATRIX_SOLID_SPLASH
 #   define ENABLE_RGB_MATRIX_SOLID_MULTISPLASH
#define RGB_MATRIX_SOLID_REACTIVE_GRADIENT_MODE
