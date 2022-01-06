#include "config_helper.h"
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
#include "cJSON.h"
#include "request_common.h"
#include "sys_conf.h"
#include "weather_request.h"
#include "common.h"

#define MAX_PLATFORM_NAME_LENGTH 256

#define MAX_TIME_ZONE_LENGTH 256

static const char *TAG = "config_helper";

static esp_err_t parse_sys_config(char *json_string) {

  esp_err_t status = ESP_OK;

  cJSON *config_json = cJSON_Parse(json_string);
  if (config_json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      ESP_LOGE(TAG, "Error before: %s\n", error_ptr);
    }
    status = ESP_FAIL;
    goto end;
  }

  cJSON *sys_config = cJSON_GetObjectItemCaseSensitive(config_json, "sysConfig");
  if (sys_config == NULL) {
    status = ESP_FAIL;
    goto end;
  }
  if (cJSON_HasObjectItem(sys_config, "developmentMode")) {
    const cJSON *development_mode = cJSON_GetObjectItemCaseSensitive(sys_config, "developmentMode");
    if (cJSON_IsNumber(development_mode)) {
      if (development_mode->valueint == 1) {
        sys_conf_set_development_mode(true);
        ESP_LOGI(TAG, "Development mode enabled");
      } else {
        sys_conf_set_development_mode(false);
        ESP_LOGI(TAG, "Development mode disabled");
      }
    }
    else {
      status = ESP_FAIL;
      goto end;
    }
  } else {
    status = ESP_FAIL;
    goto end;
  }

  if (cJSON_HasObjectItem(sys_config, "pollingInterval")) {
    const cJSON *polling_interval = cJSON_GetObjectItemCaseSensitive(sys_config, "pollingInterval");
    if (cJSON_IsString(polling_interval) && (polling_interval->valuestring != NULL) && (strlen(polling_interval->valuestring) != 0)) {
      ESP_LOGI(TAG, "Update Interval: %s", polling_interval->valuestring);
      char *end = NULL;
      sys_conf_set_polling_interval((uint32_t) strtoul(polling_interval->valuestring, &end, 10));
    }
    else {
      status = ESP_FAIL;
      goto end;
    }
  } else {
    status = ESP_FAIL;
    goto end;
  }

  if (cJSON_HasObjectItem(sys_config, "timeZone")) {
    const cJSON *time_zone = cJSON_GetObjectItemCaseSensitive(sys_config, "timeZone");
    if (cJSON_IsString(time_zone) && (time_zone->valuestring != NULL) && (strlen(time_zone->valuestring) != 0)) {
      ESP_LOGI(TAG, "timeZone: %s", time_zone->valuestring);
      sys_conf_set_time_zone(time_zone->valuestring);
    }
    else {
      status = ESP_FAIL;
      goto end;
    }
  } else {
    status = ESP_FAIL;
    goto end;
  }

  if (cJSON_HasObjectItem(sys_config, "wifiSSID")) {
    const cJSON *wifiSSID = cJSON_GetObjectItemCaseSensitive(sys_config, "wifiSSID");
    if (cJSON_IsString(wifiSSID) && (wifiSSID->valuestring != NULL) && (strlen(wifiSSID->valuestring) != 0)) {
      ESP_LOGI(TAG, "wifiSSID: %s", wifiSSID->valuestring);
      sys_conf_set_wifi_ssid(wifiSSID->valuestring);
    }
    else {
      status = ESP_FAIL;
      goto end;
    }
  } else {
    status = ESP_FAIL;
    goto end;
  }

  if (cJSON_HasObjectItem(sys_config, "wifiPassphrase")) {
    const cJSON *wifiPassphrase = cJSON_GetObjectItemCaseSensitive(sys_config, "wifiPassphrase");
    if (cJSON_IsString(wifiPassphrase) && (wifiPassphrase->valuestring != NULL) && (strlen(wifiPassphrase->valuestring) != 0)) {
      ESP_LOGI(TAG, "wifiPassphrase: %s", wifiPassphrase->valuestring);
      sys_conf_set_wifi_passphrase(wifiPassphrase->valuestring);
    }
    else {
      status = ESP_FAIL;
      goto end;
    }
  } else {
    status = ESP_FAIL;
    goto end;
  }

  if (cJSON_HasObjectItem(sys_config, "apSSID")) {
    const cJSON *apSSID = cJSON_GetObjectItemCaseSensitive(sys_config, "apSSID");
    if (cJSON_IsString(apSSID) && (apSSID->valuestring != NULL) && (strlen(apSSID->valuestring) != 0)) {
      ESP_LOGI(TAG, "apSSID: %s", apSSID->valuestring);
      sys_conf_set_ap_ssid(apSSID->valuestring);
    }
    else {
      status = ESP_FAIL;
      goto end;
    }
  } else {
    status = ESP_FAIL;
    goto end;
  }

  if (cJSON_HasObjectItem(sys_config, "apPassphrase")) {
    const cJSON *apPassphrase = cJSON_GetObjectItemCaseSensitive(sys_config, "apPassphrase");
    if (cJSON_IsString(apPassphrase) && (apPassphrase->valuestring != NULL) && (strlen(apPassphrase->valuestring) != 0)) {
      ESP_LOGI(TAG, "apPassphrase: %s", apPassphrase->valuestring);
      sys_conf_set_ap_passphrase(apPassphrase->valuestring);
    }
    else {
      status = ESP_FAIL;
      goto end;
    }
  } else {
    status = ESP_FAIL;
    goto end;
  }

  end:
    cJSON_Delete(config_json);
    return status;
} 

request_platform_t platform_to_send_request(char *platform) {
  if (strncmp(OPEN_WEATHER_MAP, platform, strlen(OPEN_WEATHER_MAP)) == 0) return OPEN_WEATHER_MAP_REQ;

  return OPEN_WEATHER_MAP_REQ;
}

static esp_err_t parse_weather_request(char *json_string) {
  const cJSON *request_platform = NULL;
  const cJSON *api_endpoint = NULL;
  const cJSON *request_api_key = NULL;
  const cJSON *city_name = NULL;
  const cJSON *city_id = NULL;
  const cJSON *coordinates = NULL;
  const cJSON *zip_code = NULL;
  esp_err_t status = ESP_OK; 

  cJSON *config_json = cJSON_Parse(json_string);
  if (config_json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      ESP_LOGE(TAG, "Error before: %s\n", error_ptr);
    }
    status = ESP_FAIL;
    goto end;
  }

  cJSON *weather_request = cJSON_GetObjectItemCaseSensitive(config_json, "weatherRequest");
  if (weather_request == NULL) {
    status = ESP_FAIL;
    goto end;
  }

  request_platform = cJSON_GetObjectItemCaseSensitive(weather_request, "requestPlatform");
  if (cJSON_IsString(request_platform) && (request_platform->valuestring != NULL) && (strlen(request_platform->valuestring) != 0)) {
    ESP_LOGI(TAG, "requestPlatform: %s", request_platform->valuestring);
    weather_req_set_req_platform(platform_to_send_request(request_platform->valuestring));
  }
  else {
    ESP_LOGE(TAG, "requestPlatform must be set in config file");
    status = ESP_FAIL;
    goto end;
  }

  api_endpoint = cJSON_GetObjectItemCaseSensitive(weather_request, "apiEndpoint");
  if (cJSON_IsString(api_endpoint) && (api_endpoint->valuestring != NULL) && (strlen(api_endpoint->valuestring) != 0)) {
    ESP_LOGI(TAG, "apiEndPoint: %s", api_endpoint->valuestring);
    weather_req_set_api_endpoint(api_endpoint->valuestring);
  }
  else {
    ESP_LOGE(TAG, "apiEndPoint must be set in config file");
    status = ESP_FAIL;
    goto end;
  }

  request_api_key = cJSON_GetObjectItemCaseSensitive(weather_request, "requestApiKey");
  if (cJSON_IsString(request_api_key) && (request_api_key->valuestring != NULL) && (strlen(request_api_key->valuestring) != 0)) {
    ESP_LOGI(TAG, "apiKey: %s", request_api_key->valuestring);
    weather_req_set_api_key(request_api_key->valuestring);
  }
  else {
    ESP_LOGE(TAG, "requestApiKey must be set in config file");
    status = ESP_FAIL;
    goto end;
  }

  if (cJSON_HasObjectItem(weather_request, "cityName")) {
    city_name = cJSON_GetObjectItemCaseSensitive(weather_request, "cityName");
    if (cJSON_IsString(city_name) && (city_name->valuestring != NULL) && (strlen(city_name->valuestring) != 0)) {
      ESP_LOGI(TAG, "cityName: %s", city_name->valuestring);
      weather_req_set_req_city_name(city_name->valuestring);
      cJSON *state_code = cJSON_GetObjectItemCaseSensitive(weather_request, "stateCode");
      if (cJSON_IsString(state_code) && (state_code->valuestring != NULL) && (strlen(state_code->valuestring) != 0)) {
        ESP_LOGI(TAG, "cityName: %s", state_code->valuestring);
        weather_req_set_req_state_code(state_code->valuestring);
      }
      cJSON *country_code = cJSON_GetObjectItemCaseSensitive(weather_request, "countryCode");
      if (cJSON_IsString(country_code) && (country_code->valuestring != NULL) && (strlen(country_code->valuestring) != 0)) {
        ESP_LOGI(TAG, "cityName: %s", country_code->valuestring);
        weather_req_set_req_country_code(country_code->valuestring);
      }
      status = ESP_OK;
      goto end;
    }
  }
  
  if (cJSON_HasObjectItem(weather_request, "cityId")) {
    city_id = cJSON_GetObjectItemCaseSensitive(weather_request, "cityId");
    if (cJSON_IsString(city_id) && (city_id->valuestring != NULL) && (strlen(city_id->valuestring) != 0)) {
      ESP_LOGI(TAG, "cityId: %s", city_id->valuestring);
      weather_req_set_req_city_id(city_id->valuestring);
      status = ESP_OK;
      goto end;
    }
  }

  if (cJSON_HasObjectItem(weather_request, "zipCode")) {
    zip_code = cJSON_GetObjectItemCaseSensitive(weather_request, "zipCode");
    if (cJSON_IsString(zip_code) && (zip_code->valuestring != NULL) && (strlen(zip_code->valuestring) != 0)) {
      ESP_LOGI(TAG, "zipCode: %s", zip_code->valuestring);
      cJSON *country_code = cJSON_GetObjectItemCaseSensitive(weather_request, "countryCode");
      if (cJSON_IsString(country_code) && (country_code->valuestring != NULL) && (strlen(country_code->valuestring) != 0)) {
        ESP_LOGI(TAG, "countryCode: %s", country_code->valuestring);
      } else {
        ESP_LOGE(TAG, "If zipCode is used, countryCode must be set");
        status = ESP_FAIL;
        goto end;
      }
      weather_req_set_req_zip_code(zip_code->valuestring);
      weather_req_set_req_country_code(country_code->valuestring);
      status = ESP_OK;
      goto end;
    }
  }

  if (cJSON_HasObjectItem(weather_request, "coordinates")) {
    coordinates = cJSON_GetObjectItemCaseSensitive(weather_request, "coordinates");
    cJSON *latitude = cJSON_GetObjectItemCaseSensitive(coordinates, "latitude");
    cJSON *longitude = cJSON_GetObjectItemCaseSensitive(coordinates, "longitude");
    double lat, lon;
    if (cJSON_IsString(latitude) && (latitude->valuestring != NULL) && (strlen(latitude->valuestring) != 0)) {
      ESP_LOGI(TAG, "latitude: %s", latitude->valuestring);
      char *end;
      lat = strtod(latitude->valuestring, &end);
    } else {
      status = ESP_FAIL;
      goto end;
    }
    if (cJSON_IsString(longitude) && (longitude->valuestring != NULL) && (strlen(longitude->valuestring) != 0) ) {
      ESP_LOGI(TAG, "longitude: %s", longitude->valuestring);
      char *end;
      lon = strtod(longitude->valuestring, &end);
    } else {
      status = ESP_FAIL;
      goto end;
    }
    weather_req_set_req_coordinate(lat, lon);
    status = ESP_OK;
    goto end;
  }

  end:
    cJSON_Delete(config_json);
    return status;
}

esp_err_t config_helper_read(const char *file_name, char *json_string, size_t size) {

  ESP_LOGI(TAG, "Open config file");

  FILE *fp = fopen(file_name, "r");

  if (fp == NULL) {
    ESP_LOGE(TAG, "Failed to open config file %s", file_name);
    return ESP_FAIL;
  }

  fread(json_string, sizeof(char), size, fp);

  if (ferror(fp)) {
    ESP_LOGE(TAG, "Error on reading config file %s", file_name);
    return ESP_FAIL;
  }

  fclose(fp);
  return ESP_OK;
}

esp_err_t config_helper_write(const char *file_name, char *json_string, size_t size) {

  ESP_LOGI(TAG, "Open config file");

  FILE *fp = fopen(file_name, "w");

  if (fp == NULL) {
    ESP_LOGE(TAG, "Failed to open config file %s", file_name);
    return ESP_FAIL;
  }

  fwrite(json_string, sizeof(char), size, fp);

  if (ferror(fp)) {
    ESP_LOGE(TAG, "Error on writing config file %s", file_name);
    return ESP_FAIL;
  }

  fclose(fp);

  return ESP_OK;
}

esp_err_t config_helper_parse(const char *file_name) {
  char json_string[MAX_CONFIG_LENGTH];

  config_helper_read(file_name, json_string, MAX_CONFIG_LENGTH); 

  if (parse_sys_config(json_string) != ESP_OK) {
    ESP_LOGE(TAG, "Parsing system config failed");
    return ESP_FAIL;
  }
  weather_request_clear();
  if (parse_weather_request(json_string) != ESP_OK) {
    ESP_LOGE(TAG, "Parsing weather request config failed");
    return ESP_FAIL;
  }

  return ESP_OK;
}