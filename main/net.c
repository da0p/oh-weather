#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include "protocol_examples_common.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "wifi_manager.h"

static const char *TAG = "net";

esp_err_t net_start(char *wifi_ssid, char *wifi_passphrase, wifi_mode_t mode) {

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_manager_start(wifi_ssid, wifi_passphrase, mode);

  return ESP_OK;
}

