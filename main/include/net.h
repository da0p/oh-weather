#pragma once
#include "esp_system.h"
#include "esp_wifi_types.h"
#ifdef __cplusplus
extern "C" {
#endif

esp_err_t net_start(char *wifi_ssid, char *wifi_passphrase, wifi_mode_t mode);

#ifdef __cplusplus
}
#endif