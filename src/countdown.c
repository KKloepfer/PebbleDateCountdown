/**
 * This code is provided under the MIT License.
 * 
 * Copyright (c) 2013 Matthew Congrove (http://github.com/mcongrove)
 * 
 * Utilizes portions of code (PDUtils) by Peter Hardy (http://github.com/phardy)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "PDutils.h"

#define APP_UUID { 0x27, 0x9D, 0xBE, 0xC4, 0x87, 0xBB, 0x42, 0xCB, 0x9B, 0x7E, 0x11, 0x9A, 0xAD, 0xA2, 0xD1, 0xEE }
#define APP_TYPE APP_INFO_WATCH_FACE

PBL_APP_INFO(APP_UUID, "Countdown Face", "Matthew Congrove", 1, 0, RESOURCE_ID_IMAGE_MENU_ICON, APP_TYPE);

Window window_root;
TextLayer label_time;
TextLayer label_countdown;
TextLayer label_text_one;
TextLayer label_text_two;

char timeText[] = "00:00";
char countText[4];

static int EVENT_MONTH = 10;
static int EVENT_DAY = 12;
static int EVENT_YEAR = 2013;
static int EVENT_HOUR = 16;
static int EVENT_MINUTE = 0;
static int EVENT_SECOND = 0;

bool calculate_countdown() {
	PblTm now;
	PblTm event;
	time_t seconds_now;
	time_t seconds_event;
	int difference;
	char *time_format;
	static char countText[] = "";
	
	get_time(&now);
	
	seconds_now = pmktime(&now);
	
	event.tm_year = EVENT_YEAR - 1900;
	event.tm_mon = EVENT_MONTH - 1;
	event.tm_mday = EVENT_DAY;
	event.tm_hour = EVENT_HOUR;
	event.tm_min = EVENT_MINUTE;
	event.tm_sec = EVENT_SECOND;
	
	seconds_event = pmktime(&event);
	
	difference = ((((seconds_event - seconds_now) / 60) / 60) / 24);
	
	if(difference < 0) {
		difference = 0;
	}
	
	snprintf(countText, 100, "%d", difference);
	
	text_layer_set_text(&label_countdown, countText);
	
	if(clock_is_24h_style()) {
		time_format = "%R";
	} else {
		time_format = "%I:%M";
	}
	
	string_format_time(timeText, sizeof(timeText), time_format, &now);
	text_layer_set_text(&label_time, timeText);
	
	return true;
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
	(void)ctx;
	(void)t;
	
	calculate_countdown();
}

void window_load(Window *window) {
	text_layer_init(&label_time, window_root.layer.frame);
	text_layer_set_text_color(&label_time, GColorWhite);
	text_layer_set_background_color(&label_time, GColorClear);
	text_layer_set_text_alignment(&label_time, GTextAlignmentCenter);
	layer_set_frame(&label_time.layer, GRect(0, 13, 144, 30));
	text_layer_set_font(&label_time, fonts_get_system_font(FONT_KEY_GOTHIC_28));
	layer_add_child(&window_root.layer, &label_time.layer);
	
	text_layer_init(&label_countdown, window_root.layer.frame);
	text_layer_set_text_color(&label_countdown, GColorWhite);
	text_layer_set_background_color(&label_countdown, GColorClear);
	text_layer_set_text_alignment(&label_countdown, GTextAlignmentCenter);
	layer_set_frame(&label_countdown.layer, GRect(0, 56, 144, 55));
	text_layer_set_font(&label_countdown, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	layer_add_child(&window_root.layer, &label_countdown.layer);
	
	text_layer_init(&label_text_one, window_root.layer.frame);
	text_layer_set_text_color(&label_text_one, GColorWhite);
	text_layer_set_background_color(&label_text_one, GColorClear);
	text_layer_set_text_alignment(&label_text_one, GTextAlignmentCenter);
	layer_set_frame(&label_text_one.layer, GRect(0, 110, 144, 23));
	text_layer_set_text(&label_text_one, "Days Until");
	text_layer_set_font(&label_text_one, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	layer_add_child(&window_root.layer, &label_text_one.layer);
	
	text_layer_init(&label_text_two, window_root.layer.frame);
	text_layer_set_text_color(&label_text_two, GColorWhite);
	text_layer_set_background_color(&label_text_two, GColorClear);
	text_layer_set_text_alignment(&label_text_two, GTextAlignmentCenter);
	layer_set_frame(&label_text_two.layer, GRect(0, 130, 144, 23));
	text_layer_set_text(&label_text_two, "the Wedding");
	text_layer_set_font(&label_text_two, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	layer_add_child(&window_root.layer, &label_text_two.layer);
}

void handle_init(AppContextRef ctx) {
	resource_init_current_app(&APP_RESOURCES);
	
	handle_minute_tick(ctx, NULL);
	
	window_init(&window_root, "Countdown");
	window_set_fullscreen(&window_root, true);
	window_set_background_color(&window_root, GColorBlack);
	window_stack_push(&window_root, true);
	window_set_window_handlers(&window_root, (WindowHandlers){
		.load = window_load
	});
}

void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,
		.tick_info = {
			.tick_handler = &handle_minute_tick,
			.tick_units = MINUTE_UNIT
		}
	};
	
	app_event_loop(params, &handlers);
}