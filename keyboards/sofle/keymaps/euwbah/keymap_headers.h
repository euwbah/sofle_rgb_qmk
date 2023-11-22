#include <stdio.h>
// This include is required for using rpc transactions between master and slave
#include "transactions.h"
// This include is required for sending & receiving raw HID
#include "raw_hid.h"
// This include is required for custom mouse keys
#include "report.h"
#include "./config.h"

void send_mouse_report(void);
void user_sync_data_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data);
bool perform_data_sync(void);
void send_hid_user_data(void);
oled_rotation_t oled_init_user(oled_rotation_t rotation);
static void cat_buffer(const char* str);
static void render_left_oled(void);
static void draw_bar(uint8_t length, uint8_t bar_byte);
static void clear_bar(uint8_t length);
static void draw_utilization_bar(uint8_t utilization);
static void render_right_oled(void);
static void update_ext_mon_setting(void);
