#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include "sys_conf.h"
#include "esp_vfs.h"
#include "nvs_flash.h"
#include "net.h"
#include "vfs.h"
#include "config_helper.h"
#include "time_keeper.h"
#include "rest_server.h"
#include "common.h"

#define OH_WEATHER_RESET_GPIO      CONFIG_OH_WEATHER_RESET_GPIO
#define OH_WEATHER_RESET_PIN_SEL (1ULL<<OH_WEATHER_RESET_GPIO)

static const char *TAG = "sys_conf";
struct sys_conf_t {
  uint32_t polling_interval;
  bool development_mode;
  char time_zone[256];
  char wifi_ssid[32];
  char wifi_passphrase[32];
  char ap_ssid[32];
  char ap_passphrase[32];
};

static sys_conf_t sys_conf_dev;

esp_err_t flash_mem_init(void) {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  return ret;
}

bool is_sta_mode(void) {

  gpio_config_t io_conf;

  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = OH_WEATHER_RESET_PIN_SEL;
  io_conf.pull_up_en = 1;
  gpio_config(&io_conf);

  if (gpio_get_level(OH_WEATHER_RESET_GPIO)) return true;

  return false;
}

bool sys_conf_init() {
    sys_conf_dev.development_mode = false;
    sys_conf_dev.polling_interval = 0;

    ESP_ERROR_CHECK(vfs_init(STORAGE_MOUNTPOINT, STORAGE_LABEL, WEB_MOUNTPOINT, WEB_LABEL));

    config_helper_parse(CONFIGFILE_DIR);

    ESP_ERROR_CHECK(flash_mem_init());

    if (is_sta_mode()) {
      ESP_LOGI(TAG, "WiFi Station Mode");
      ESP_ERROR_CHECK(net_start(sys_conf_dev.wifi_ssid, sys_conf_dev.wifi_passphrase, WIFI_MODE_STA));
      ESP_ERROR_CHECK(start_rest_server(WEB_MOUNTPOINT));
      time_keeper_init();
      time_keeper_set_time_zone(sys_conf_dev.time_zone);

      return true;
    }
    else {
      ESP_LOGI(TAG, "WiFi AP Mode");
      ESP_ERROR_CHECK(net_start(sys_conf_dev.ap_ssid, sys_conf_dev.ap_passphrase, WIFI_MODE_AP));
      
      return false;
    }
}

esp_err_t sys_conf_set_polling_interval(uint32_t polling_interval) {
    sys_conf_dev.polling_interval = polling_interval;
    return ESP_OK;
}

uint32_t sys_conf_get_polling_interval(void) {
    return sys_conf_dev.polling_interval;
}

esp_err_t sys_conf_set_time_zone(char *time_zone) {
    bzero(sys_conf_dev.time_zone, 256);
    strncpy(sys_conf_dev.time_zone, time_zone, 256);
    return ESP_OK;
}

esp_err_t sys_conf_get_time_zone(char *time_zone, size_t size) {
  bzero(time_zone, size);
  strncpy(time_zone, sys_conf_dev.time_zone, size);
  return ESP_OK;
}

esp_err_t sys_conf_set_wifi_ssid(char *wifi_ssid) {
    bzero(sys_conf_dev.wifi_ssid, 32);
    strncpy(sys_conf_dev.wifi_ssid, wifi_ssid, 32);
    return ESP_OK;
}

esp_err_t sys_conf_get_wifi_ssid(char *wifi_ssid, size_t size) {
  bzero(wifi_ssid, size);
  strncpy(wifi_ssid, sys_conf_dev.wifi_ssid, size);
  return ESP_OK;
}

esp_err_t sys_conf_set_wifi_passphrase(char *wifi_passphrase) {
    bzero(sys_conf_dev.wifi_passphrase, 32);
    strncpy(sys_conf_dev.wifi_passphrase, wifi_passphrase, 32);
    return ESP_OK;
}

esp_err_t sys_conf_get_wifi_passphrase(char *wifi_passphrase, size_t size) {
  bzero(wifi_passphrase, size);
  strncpy(wifi_passphrase, sys_conf_dev.wifi_passphrase, size);
  return ESP_OK;
}

esp_err_t sys_conf_set_ap_ssid(char *ap_ssid) {
    bzero(sys_conf_dev.ap_ssid, 32);
    strncpy(sys_conf_dev.ap_ssid, ap_ssid, 32);
    return ESP_OK;
}

esp_err_t sys_conf_get_ap_ssid(char *ap_ssid, size_t size) {
  bzero(ap_ssid, size);
  strncpy(ap_ssid, sys_conf_dev.ap_ssid, size);
  return ESP_OK;
}

esp_err_t sys_conf_set_development_mode(bool development_mode) {
    sys_conf_dev.development_mode = development_mode;
    return ESP_OK;
}

esp_err_t sys_conf_set_ap_passphrase(char *ap_passphrase) {
    bzero(sys_conf_dev.ap_passphrase, 32);
    strncpy(sys_conf_dev.ap_passphrase, ap_passphrase, 32);
    return ESP_OK;
}

esp_err_t sys_conf_get_ap_passphrase(char *ap_passphrase, size_t size) {
  bzero(ap_passphrase, size);
  strncpy(ap_passphrase, sys_conf_dev.ap_passphrase, size);
  return ESP_OK;
}

bool sys_conf_get_development_mode(void) {
    return sys_conf_dev.development_mode;
}