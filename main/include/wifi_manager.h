#pragma once
#include "esp_system.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "esp_wifi_types.h"
/**
 * @brief Initialize wifi sta
 */
 void wifi_manager_start(char *ssid, char *passphrase, wifi_mode_t mode);

#ifdef __cplusplus
}
#endif