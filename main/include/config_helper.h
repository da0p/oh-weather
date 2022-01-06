#pragma once
#include "esp_system.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Write config file
 *
 */
esp_err_t config_helper_write(const char *file_name, char *json_string, size_t size);

/**
 * @brief Read config file
 *
 */
esp_err_t config_helper_read(const char *file_name, char *json_string, size_t size);

/**
 * @brief Parse config file
 *
 */
esp_err_t config_helper_parse(const char *file_name);

#ifdef __cplusplus
}
#endif