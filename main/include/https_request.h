#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include "esp_system.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Send http get request to an url
 *
 */
esp_err_t https_with_url(char *web_url);

/**
 * @brief Check if the http request is finished
 *
 */
bool is_request_done(void);

/**
 * @brief Reset the condition in order to send a
 * a request
 */
void reset_request_condition(void);

/**
 * @brief Read the content's response
 */
void read_content(char *content, size_t size);

#ifdef __cplusplus
}
#endif