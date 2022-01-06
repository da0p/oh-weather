#pragma once
#include "esp_system.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parse config file
 *
 */
esp_err_t start_rest_server(const char *base_path);
#ifdef __cplusplus
}
#endif