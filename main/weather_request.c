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
#include "weather_request.h"
#include "request_common.h"

static request_t request;

esp_err_t weather_request_clear(void) {
    request.request_platform = OPEN_WEATHER_MAP_REQ;
    request.request_type = UNKNOWN;
    bzero(request.api_endpoint, MAX_API_END_POINT_LENGTH);
    bzero(request.api_key, MAX_API_KEY_LENGTH);
    bzero(request.city_name, MAX_CITY_NAME_LENGTH);
    bzero(request.city_id, MAX_CITY_ID_LENGTH);
    bzero(request.zip_code, MAX_ZIP_CODE_LENGTH);
    bzero(request.state_code, MAX_STATE_CODE_LENGTH);
    bzero(request.country_code, MAX_COUNTRY_CODE_LENGTH);
    request.coordinate.latitude = 0.0;
    request.coordinate.longitude = 0.0;
    return ESP_OK;
}

esp_err_t weather_req_set_req_platform(request_platform_t req_plf) {
    request.request_platform = req_plf;
    return ESP_OK;
}

request_platform_t weather_req_get_req_platform(void) {
    return request.request_platform;
}

esp_err_t weather_req_set_req_type(request_type_t request_type) {
    request.request_type = request_type;
    return ESP_OK;
}


request_type_t weather_req_get_req_type(void) {
    return request.request_type;
}

esp_err_t weather_req_set_api_endpoint(char *api_endpoint) {
    strncpy(request.api_endpoint, api_endpoint, MAX_API_END_POINT_LENGTH);
    return ESP_OK;
}

esp_err_t weather_req_get_api_endpoint(char *api_endpoint, size_t size) {
    if (size >= MAX_API_END_POINT_LENGTH) {
        strncpy(api_endpoint, request.api_endpoint, MAX_API_END_POINT_LENGTH);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t weather_req_set_api_key(char *api_key) {
    strncpy(request.api_key, api_key, MAX_API_KEY_LENGTH);
    return ESP_OK;
}

esp_err_t weather_req_get_api_key(char *api_key, size_t size) {
    if (size >= MAX_API_KEY_LENGTH) {
        strncpy(api_key, request.api_key, MAX_API_KEY_LENGTH);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t weather_req_set_req_city_name(char *city_name) {
    strncpy(request.city_name, city_name, MAX_CITY_NAME_LENGTH);
    weather_req_set_req_type(CITY_NAME);
    return ESP_OK;
}

esp_err_t weather_req_get_req_city_name(char *city_name, size_t size) {
    if (size >= MAX_CITY_NAME_LENGTH) {
        strncpy(city_name, request.city_name, MAX_CITY_NAME_LENGTH);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t weather_req_set_req_city_id(char *city_id) {
    strncpy(request.city_id, city_id, MAX_CITY_ID_LENGTH);
    weather_req_set_req_type(CITY_ID);
    return ESP_OK;
}

esp_err_t weather_req_get_req_city_id(char *city_id, size_t size) {
    if (size >= MAX_CITY_ID_LENGTH) {
        strncpy(city_id, request.city_id, MAX_CITY_ID_LENGTH);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t weather_req_set_req_zip_code(char *zip_code) {
    strncpy(request.zip_code, zip_code, MAX_ZIP_CODE_LENGTH);
    weather_req_set_req_type(ZIP_CODE);
    return ESP_OK;
}

esp_err_t weather_req_get_req_zip_code(char *zip_code, size_t size) {
    if (size >= MAX_ZIP_CODE_LENGTH) {
        strncpy(zip_code, request.zip_code, MAX_ZIP_CODE_LENGTH);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t weather_req_set_req_state_code(char *state_code) {
    strncpy(request.state_code, state_code, MAX_STATE_CODE_LENGTH);
    return ESP_OK;
}

esp_err_t weather_req_get_req_state_code(char *state_code, size_t size) {
    if (size >= MAX_STATE_CODE_LENGTH) {
        strncpy(state_code, request.state_code, MAX_STATE_CODE_LENGTH);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t weather_req_set_req_country_code(char *country_code) {
    strncpy(request.country_code, country_code, MAX_COUNTRY_CODE_LENGTH);
    return ESP_OK;
}

esp_err_t weather_req_get_req_country_code(char *country_code, size_t size) {
    if (size >= MAX_COUNTRY_CODE_LENGTH) {
        strncpy(country_code, request.country_code, MAX_COUNTRY_CODE_LENGTH);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t weather_req_set_req_latitude(double latitude) {
    request.coordinate.latitude = latitude;
    return ESP_OK;
}

esp_err_t weather_req_set_req_longitude(double longitude) {
    request.coordinate.latitude = longitude;
    return ESP_OK;
}

esp_err_t weather_req_set_req_coordinate(double latitude, double longitude) {
    weather_req_set_req_latitude(latitude);
    weather_req_set_req_longitude(longitude);
    weather_req_set_req_type(GEOGRAPHIC_COORDINATE);
    return ESP_OK;
}

coordinate_t weather_req_get_req_coordinate(void) {
    return request.coordinate;
}

void request_current_weather(void) {
    switch (request.request_platform) {
    case OPEN_WEATHER_MAP_REQ:
        owm_request_current_weather(request);
        break;
    default:
        owm_request_current_weather(request);
        break;
    }
}