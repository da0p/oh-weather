#pragma once
#include "esp_system.h"
#include "openweathermap.h"
#include "request_common.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Initiailize weather request
 *
 */
esp_err_t weather_request_clear(void);

/**
 * @brief Request Platform setter
 *
 */
esp_err_t weather_req_set_req_platform(request_platform_t req_plf);

/**
 * @brief Request Platform getter
 *
 */
request_platform_t weather_req_get_req_platform(void);

/**
 * @brief Request type setter
 *
 */
esp_err_t weather_req_set_req_type(request_type_t request_type);

/**
 * @brief Request type getter
 *
 */
request_type_t weather_req_get_req_type(void);

/**
 * @brief Request api end point setter
 *
 */
esp_err_t weather_req_set_api_endpoint(char *api_endpoint);

/**
 * @brief Request api end point getter
 *
 */
esp_err_t weather_req_get_api_endpoint(char *api_endpoint, size_t size);

/**
 * @brief Request set api key setter
 *
 */
esp_err_t weather_req_set_api_key(char *api_key);

/**
 * @brief Request api key getter
 *
 */
esp_err_t weather_req_get_api_key(char *api_key, size_t size);

/**
 * @brief Request city name setter
 *
 */
esp_err_t weather_req_set_req_city_name(char *city_name);

/**
 * @brief Request city name getter
 *
 */
esp_err_t weather_req_get_req_city_name(char *city_name, size_t size);

/**
 * @brief Request city id setter
 *
 */
esp_err_t weather_req_set_req_city_id(char *city_id);

/**
 * @brief Request city id getter
 *
 */
esp_err_t weather_req_get_req_city_id(char *city_id, size_t size);

/**
 * @brief Request zip code setter
 *
 */
esp_err_t weather_req_set_req_zip_code(char *zip_code);

/**
 * @brief Request zip code getter
 *
 */
esp_err_t weather_req_get_req_zip_code(char *zip_code, size_t size);

/**
 * @brief Request state code setter
 *
 */
esp_err_t weather_req_set_req_state_code(char *state_code);

/**
 * @brief Request state code getter
 *
 */
esp_err_t weather_req_get_req_state_code(char *state_code, size_t size);

/**
 * @brief Request country code setter
 *
 */
esp_err_t weather_req_set_req_country_code(char *country_code);

/**
 * @brief Request country code getter
 *
 */
esp_err_t weather_req_get_req_country_code(char *country_code, size_t size);

/**
 * @brief Request coordinate setter
 *
 */
esp_err_t weather_req_set_req_coordinate(double latitude, double longitude);

/**
 * @brief Request coordinate getter
 *
 */
coordinate_t weather_req_get_req_coordinate(void);

/**
 * @brief Request current weather data
 *
 */
void request_current_weather(void);

/**
 * @brief Request forcast data
 *
 */
esp_err_t request_forcast_weather(void);

#ifdef __cplusplus
}
#endif