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
#include "sys_conf.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_netif.h"
#include "esp_event.h"

static const char *TAG = "vfs";

static void SPIFFS_Directory(char *path) {
  DIR *dir = opendir(path);
  assert(dir != NULL);
  while (true) {
    struct dirent *pe = readdir(dir);
    if (!pe)
      break;
    ESP_LOGI(__FUNCTION__, "d_name=%s d_ino=%d d_type=%x", pe->d_name,
             pe->d_ino, pe->d_type);
  }
  closedir(dir);
}

esp_err_t vfs_init(char *storage_mp, char *storage_label, char *web_mp, char *web_label) {

  ESP_LOGI(TAG, "Initializing SPIFFS");

  esp_vfs_spiffs_conf_t conf = {.base_path = storage_mp,
                                .partition_label = storage_label,
                                .max_files = 10,
                                .format_if_mount_failed = true};
  
  esp_err_t ret = esp_vfs_spiffs_register(&conf);

  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      ESP_LOGE(TAG, "Failed to mount or format filesystem");
    } else if (ret == ESP_ERR_NOT_FOUND) {
      ESP_LOGE(TAG, "Failed to find SPIFFS partition");
    } else {
      ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
    }
    return ESP_FAIL;
  }

  size_t total = 0, used = 0;
  ret = esp_spiffs_info(storage_label, &total, &used);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)",
             esp_err_to_name(ret));
    return ESP_FAIL;
  } else {
    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
  }
  SPIFFS_Directory(storage_mp);

  esp_vfs_spiffs_conf_t www_conf = {
        .base_path = web_mp,
        .partition_label = web_label,
        .max_files = 5,
        .format_if_mount_failed = false
  };

  ret = esp_vfs_spiffs_register(&www_conf);

  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      ESP_LOGE(TAG, "Failed to mount or format filesystem");
    } else if (ret == ESP_ERR_NOT_FOUND) {
      ESP_LOGE(TAG, "Failed to find SPIFFS partition");
    } else {
      ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
    }
    return ESP_FAIL;
  }

  total = 0, used = 0;
  ret = esp_spiffs_info(web_label, &total, &used);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)",
             esp_err_to_name(ret));
    return ESP_FAIL;
  } else {
    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
  }
  SPIFFS_Directory(web_mp);

  return ESP_OK;
}