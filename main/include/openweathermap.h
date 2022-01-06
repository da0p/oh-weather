#pragma once

#include <stdlib.h>
#include "esp_system.h"
#include "request_common.h"
#include "response_common.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get response coordinates
 *
 */
void owm_get_response_coord(coord_t *coord);

/**
 * @brief Get response weather info
 *
 */
void owm_get_response_weather(weather_t *weather);

/**
 * @brief Get response base
 *
 */
void owm_get_response_base(char *base, size_t size);

/**
 * @brief Get response main info
 *
 */
void owm_get_response_main(main_t *main); 

/**
 * @brief Get response visibility
 *
 */
void owm_get_response_visibility(int *visibility);

/**
 * @brief Get wind info
 *
 */
void owm_get_response_wind(wind_t *wind);

/**
 * @brief Get response clouds
 *
 */
void owm_get_response_clouds(clouds_t *clouds);

/**
 * @brief Get response utc time
 *
 */
void owm_get_response_dt(int64_t *dt);

/**
 * @brief Get response system info
 *
 */
void owm_get_response_sys(sys_t *sys);

/**
 * @brief Get location timezone 
 *
 */
void owm_get_response_timezone(int64_t *timezone);

/**
 * @brief Get id
 *
 */
void owm_get_response_id(int64_t *id); 

/**
 * @brief Get response code
 *
 */
void owm_get_response_cod(int *cod); 

/**
 * @brief Get city name
 *
 */
void owm_get_response_name(char *name, size_t size);

/**
 * @brief Send http get request to an url
 *
 */
void owm_request_current_weather(request_t request);

#ifdef __cplusplus
}
#endif