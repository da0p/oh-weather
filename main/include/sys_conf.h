#pragma once
#include "esp_system.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct sys_conf_t sys_conf_t;
/**
 * @brief Initiailize system config
 *
 */
bool sys_conf_init(void);

/**
 * @brief Polling interval setter
 *
 */
esp_err_t sys_conf_set_polling_interval(uint32_t polling_interval);

/**
 * @brief Polling interval getter
 *
 */
uint32_t sys_conf_get_polling_interval(void);

/**
 * @brief Development mode setter
 *
 */
esp_err_t sys_conf_set_development_mode(bool development_mode);

/**
 * @brief Development mode getter
 *
 */
bool sys_conf_get_development_mode(void);

/**
 * @brief Timezone setter
 *
 */
esp_err_t sys_conf_set_time_zone(char *time_zone);

/**
 * @brief Development mode setter
 *
 */
esp_err_t sys_conf_get_time_zone(char *time_zone, size_t size);

/**
 * @brief Wifi ssid setter
 *
 */
esp_err_t sys_conf_set_wifi_ssid(char *wifi_ssid);

/**
 * @brief Wifi ssid getter
 *
 */
esp_err_t sys_conf_get_wifi_ssid(char *wifi_ssid, size_t size);

/**
 * @brief wifi passphrase setter
 *
 */
esp_err_t sys_conf_set_wifi_passphrase(char *wifi_passphrase);

/**
 * @brief Wifi passphrase getter
 *
 */
esp_err_t sys_conf_get_wifi_passphrase(char *wifi_passphrase, size_t size);

/**
 * @brief Wifi ssid setter
 *
 */
esp_err_t sys_conf_set_ap_ssid(char *ap_ssid);

/**
 * @brief Wifi ssid getter
 *
 */
esp_err_t sys_conf_get_ap_ssid(char *ap_ssid, size_t size);

/**
 * @brief ap passphrase setter
 *
 */
esp_err_t sys_conf_set_ap_passphrase(char *ap_passphrase);

/**
 * @brief Wifi passphrase getter
 *
 */
esp_err_t sys_conf_get_ap_passphrase(char *ap_passphrase, size_t size);
#ifdef __cplusplus
}
#endif