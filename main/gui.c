#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lv_conf_internal.h"
#include "lv_core/lv_disp.h"
#include "lv_core/lv_obj.h"
#include "lv_core/lv_obj_style_dec.h"
#include "lv_draw/lv_img_buf.h"
#include "lv_font/lv_font.h"
#include "lv_misc/lv_area.h"
#include "lv_misc/lv_color.h"
#include "lv_widgets/lv_cont.h"
#include "lv_widgets/lv_img.h"
#include "lv_widgets/lv_label.h"
#include "lvgl.h"
#include "lvgl_helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "gui.h"
#include "response_common.h"
#include "openweathermap.h"
#include "gui_defines.h"

static const char *TAG = "GUI";

#define NUMBER_OF_ICONS   18

LV_IMG_DECLARE(i1d);
LV_IMG_DECLARE(i1n);
LV_IMG_DECLARE(i2d);
LV_IMG_DECLARE(i2n);
LV_IMG_DECLARE(i3d);
LV_IMG_DECLARE(i3n);
LV_IMG_DECLARE(i4d);
LV_IMG_DECLARE(i4n);
LV_IMG_DECLARE(i9d);
LV_IMG_DECLARE(i9n);
LV_IMG_DECLARE(i10d);
LV_IMG_DECLARE(i10n);
LV_IMG_DECLARE(i11d);
LV_IMG_DECLARE(i11n);
LV_IMG_DECLARE(i13d);
LV_IMG_DECLARE(i13n);
LV_IMG_DECLARE(i50d);
LV_IMG_DECLARE(i50n);
LV_IMG_DECLARE(itemp);
LV_IMG_DECLARE(ihum);

static lv_obj_t* clock_label;
static lv_obj_t* date_label;
static lv_obj_t* hum_label;
static lv_obj_t* temp_label;
static lv_obj_t *icon;
static lv_obj_t *itemp_icon;
static lv_obj_t *ihum_icon;
static lv_obj_t *info_label;
static lv_obj_t *bootup_label;

static char *img[] = {"01d", "01n", "02d", "02n", "03d", "03n", "04d", "04n", 
                      "09d", "09n", "10d", "10n", "11d", "11n",
                      "13d", "13n", "50d", "50n"};

lv_img_dsc_t icons[NUMBER_OF_ICONS];

static double to_c_deg(double temp_in_K) {
  return temp_in_K - 273.15;
}

int icon_index(char *str) {
  for (int i = 0; i < sizeof(img) / sizeof(img[0]); i++) {
    if (strncmp(str, img[i], 3) == 0) {
      return i;
    }
  }
  return -1;
}

void build_map(void) {
  icons[0] = i1d;
  icons[1] = i1n;
  icons[2] = i2d;
  icons[3] = i2n;
  icons[4] = i3d;
  icons[5] = i3n;
  icons[6] = i4d;
  icons[7] = i4n;
  icons[8] = i9d;
  icons[9] = i9n;
  icons[10] = i10d;
  icons[11] = i10n;
  icons[12] = i11d;
  icons[13] = i11n;
  icons[14] = i13d;
  icons[15] = i13n;
  icons[16] = i50d;
  icons[17] = i50n;
}

static void weather_display_callback(lv_task_t *task) {
  int index = -1;
  main_t _main;
  owm_get_response_main(&_main);
  lv_label_set_text_fmt(temp_label, "%0.2f C deg", to_c_deg(_main.temp));
  lv_label_set_text_fmt(hum_label, "%0.2f \%%", _main.humidity);
  weather_t _weather;
  owm_get_response_weather(&_weather);
  lv_label_set_text_fmt(info_label, "%s", _weather.main);
  if ((index = icon_index(_weather.icon)) != -1) {
    lv_img_set_src(icon, &icons[index]);
  }
}

void create_weather_display(lv_obj_t *parent) {
    icon = lv_img_create(parent, NULL);
    lv_obj_align(icon, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, WEATHER_ICON_X_OFFSET, WEATHER_ICON_Y_OFFSET);


    static lv_style_t info_style;
    info_label = lv_label_create(parent, NULL);
    lv_style_set_text_font(&info_style, LV_STATE_DEFAULT, &lv_font_montserrat_14);
    lv_style_set_text_color(&info_style, LV_STATE_DEFAULT, lv_color_hex(0x000000));
    lv_label_set_text(info_label, "");
    lv_obj_add_style(info_label, 0, &info_style);
    lv_obj_set_auto_realign(info_label, true);
    lv_obj_align(info_label, NULL, LV_ALIGN_IN_TOP_RIGHT, WEATHER_INFO_X_OFFSET, WEATHER_INFO_Y_OFFSET);
    lv_label_set_align(info_label, LV_LABEL_ALIGN_CENTER);
}

static void clock_callback(lv_task_t *task) {
  time_t now;
  char time_buf[64];
  char date_buf[64];
  struct tm timemeasurement;

  time(&now);
  localtime_r(&now, &timemeasurement);
  strftime(time_buf, sizeof(time_buf), "%I:%M:%S %p", &timemeasurement);
  strftime(date_buf, sizeof(date_buf), "%a %b %Y", &timemeasurement);
  lv_label_set_text(clock_label, time_buf);
  lv_label_set_text(date_label, date_buf);
}

void create_clock(lv_obj_t *parent) {

  static lv_style_t clock_style;
  lv_style_init(&clock_style);
  clock_label = lv_label_create(parent, NULL);
  lv_style_set_text_font(&clock_style, LV_STATE_DEFAULT, &lv_font_montserrat_18);
  lv_style_set_text_color(&clock_style, LV_STATE_DEFAULT, lv_color_hex(0x000000));
  lv_obj_add_style(clock_label, 0, &clock_style);
  lv_obj_set_auto_realign(clock_label, true);
  lv_obj_align(clock_label, NULL, LV_ALIGN_IN_TOP_LEFT, CLOCK_INFO_X_OFFSET, CLOCK_INFO_Y_OFFSET);
  lv_label_set_text(clock_label, "");
  lv_label_set_align(clock_label, LV_LABEL_ALIGN_LEFT);

  static lv_style_t date_style;
  lv_style_init(&date_style);
  date_label = lv_label_create(parent, NULL);
  lv_style_set_text_font(&date_style, LV_STATE_DEFAULT, &lv_font_montserrat_10);
  lv_style_set_text_color(&date_style, LV_STATE_DEFAULT, lv_color_hex(0x000000));
  lv_obj_add_style(date_label, 0, &date_style);
  lv_obj_set_auto_realign(date_label, true);
  lv_obj_align(date_label, NULL, LV_ALIGN_IN_TOP_LEFT, DATE_INFO_X_OFFSET, DATE_INFO_Y_OFFSET);
  lv_label_set_text(date_label, "");
  lv_label_set_align(date_label, LV_LABEL_ALIGN_LEFT);

  lv_task_create(clock_callback, 1000, LV_TASK_PRIO_LOW, NULL);
}

static void create_measurement_info(lv_obj_t *parent) {
  static lv_style_t temp_style;
  temp_label = lv_label_create(parent, NULL);
  lv_style_set_text_font(&temp_style, LV_STATE_DEFAULT, &lv_font_montserrat_18);
  lv_style_set_text_color(&temp_style, LV_STATE_DEFAULT, lv_color_hex(0x000000));
  lv_obj_add_style(temp_label, 0, &temp_style);
  lv_obj_set_auto_realign(temp_label, true);
  lv_obj_align(temp_label, NULL, LV_ALIGN_IN_BOTTOM_LEFT, TEMP_INFO_X_OFFSET, TEMP_INFO_Y_OFFSET);
  lv_label_set_text(temp_label, "");
  lv_label_set_align(temp_label, LV_LABEL_ALIGN_CENTER);
  itemp_icon = lv_img_create(parent, NULL);
  lv_obj_align(itemp_icon, NULL, LV_ALIGN_IN_BOTTOM_LEFT, TEMP_ICON_X_OFFSET, TEMP_ICON_Y_OFFSET);
  lv_img_set_zoom(itemp_icon, TEMP_ICON_SCALE);
  lv_img_set_src(itemp_icon, &itemp);

  static lv_style_t hum_style;
  hum_label = lv_label_create(parent, NULL);
  lv_style_set_text_font(&hum_style, LV_STATE_DEFAULT, &lv_font_montserrat_14);
  lv_style_set_text_color(&hum_style, LV_STATE_DEFAULT, lv_color_hex(0x000000));
  lv_obj_add_style(hum_label, 0, &hum_style);
  lv_obj_set_auto_realign(hum_label, true);
  lv_obj_align(hum_label, NULL, LV_ALIGN_IN_BOTTOM_LEFT, HUM_INFO_X_OFFSET, HUM_INFO_Y_OFFSET);
  lv_label_set_text(hum_label, "");
  lv_label_set_align(hum_label, LV_LABEL_ALIGN_CENTER);
  ihum_icon = lv_img_create(parent, NULL);
  lv_obj_align(ihum_icon, NULL, LV_ALIGN_IN_BOTTOM_LEFT, HUM_ICON_X_OFFSET, HUM_ICON_Y_OFFSET);
  lv_img_set_zoom(ihum_icon, HUM_ICON_SCALE);
  lv_img_set_src(ihum_icon, &ihum);
  lv_task_create(weather_display_callback, 1000, LV_TASK_PRIO_LOW, NULL);
}

static void create_bootup_text(lv_obj_t *parent) {
  static lv_style_t bootup_style;
  bootup_label = lv_label_create(parent, NULL);
  lv_style_set_text_font(&bootup_style, LV_STATE_DEFAULT, &lv_font_montserrat_18);
  lv_style_set_text_color(&bootup_style, LV_STATE_DEFAULT, lv_color_hex(0x000000));
  lv_label_set_text(bootup_label, BOOTING_TEXT);
  lv_obj_add_style(bootup_label, 0, &bootup_style);
  lv_obj_set_auto_realign(bootup_label, true);
  lv_obj_align(bootup_label, NULL, LV_ALIGN_CENTER, BOOTUP_TEXT_X_OFFSET, BOOTUP_TEXT_Y_OFFSET);
  lv_label_set_align(bootup_label, LV_LABEL_ALIGN_CENTER);
}

void create_current_openweathermap_gui() {

  build_map();

  lv_obj_t *weather_scr = lv_obj_create(NULL, NULL);

  lv_obj_set_style_local_bg_color(weather_scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));

  create_clock(weather_scr);

  create_weather_display(weather_scr);

  create_measurement_info(weather_scr);

  lv_scr_load(weather_scr);
}

void create_bootup_gui() {
  lv_obj_t *bootup_scr = lv_obj_create(NULL, NULL);

  lv_obj_set_style_local_bg_color(bootup_scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));

  create_bootup_text(bootup_scr);

  lv_scr_load(bootup_scr);
}