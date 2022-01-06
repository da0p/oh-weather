#include "openweathermap.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "https_request.h"
#include "include/https_request.h"
#include "request_common.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

const char *TAG = "open_weather_map";

static response_t owm_response;

esp_err_t owm_set_response_coord(coord_t coord) {
  owm_response.coord.lat = coord.lat;
  owm_response.coord.lon = coord.lon;
  return ESP_OK;
}

void owm_get_response_coord(coord_t *coord) {
  coord->lat = owm_response.coord.lat;
  coord->lon = owm_response.coord.lon;
}

esp_err_t owm_set_response_weather(weather_t weather) {
  owm_response.weather = weather;
  return ESP_OK;
}

void owm_get_response_weather(weather_t *weather) {
  *weather = owm_response.weather;
}

esp_err_t owm_set_response_base(char *base) {
  strncpy(owm_response.base, base, sizeof(owm_response.base));
  return ESP_OK;
}

void owm_get_response_base(char *base, size_t size) {
  strncpy(base, owm_response.base, size);
}

esp_err_t owm_set_response_main(main_t main) {
  owm_response.main = main;
  return ESP_OK;
}

void owm_get_response_main(main_t *main) {
  *main = owm_response.main;
}

static void owm_clear_response(void) {
  memset(&owm_response, 0, sizeof(owm_response));
}

esp_err_t owm_set_response_visibility(int visibility) {
  owm_response.visibility = visibility;
  return ESP_OK;
}

void owm_get_response_visibility(int *visibility) {
  *visibility = owm_response.visibility;
}

esp_err_t owm_set_response_wind(wind_t wind) {
  owm_response.wind = wind;
  return ESP_OK;
}

void owm_get_response_wind(wind_t *wind) {
  *wind = owm_response.wind;
}

esp_err_t owm_set_response_clouds(clouds_t clouds) {
  owm_response.clouds = clouds;
  return ESP_OK;
}

void owm_get_response_clouds(clouds_t *clouds) {
  *clouds = owm_response.clouds;
}

esp_err_t owm_set_response_dt(int64_t dt) {
  owm_response.dt = dt;
  return ESP_OK;
}

void owm_get_response_dt(int64_t *dt) {
  *dt = owm_response.dt;
}

esp_err_t owm_set_response_sys(sys_t sys) {
  owm_response.sys = sys;
  return ESP_OK;
}

void owm_get_response_sys(sys_t *sys) {
  *sys = owm_response.sys;
}

esp_err_t owm_set_response_timezone(int64_t timezone) {
  owm_response.timezone = timezone;
  return ESP_OK;
}

void owm_get_response_timezone(int64_t *timezone) {
  *timezone = owm_response.timezone;
}

esp_err_t owm_set_response_id(int64_t id) {
  owm_response.id = id;
  return ESP_OK;
}

void owm_get_response_id(int64_t *id) {
  *id = owm_response.id;
}

esp_err_t owm_set_response_cod(int cod) {
  owm_response.cod = cod;
  return ESP_OK;
}

void owm_get_response_cod(int *cod) {
  *cod = owm_response.cod;
}

esp_err_t owm_set_response_name(char *name) {
  strncpy(owm_response.name, name, sizeof(owm_response.name));
  return ESP_OK;
}

void owm_get_response_name(char *name, size_t size) {
  strncpy(name, owm_response.name, size);
}

static void prepare_req_city_name(request_t request, char *owm_req, size_t size) {
  snprintf(owm_req, size, "%s?q=%s", request.api_endpoint, 
                                                  request.city_name);
  if (strlen(request.state_code) != 0) {
    strcat(owm_req, ",");
    strncat(owm_req, request.state_code, strlen(request.state_code));
  }

  if (strlen(request.country_code) != 0) {
    strcat(owm_req, ",");
    strncat(owm_req, request.country_code, strlen(request.country_code));
  }

  strcat(owm_req, "&appid=");
  strncat(owm_req, request.api_key, strlen(request.api_key));
} 

static void prepare_req_city_id(request_t request, char *owm_req, size_t size) {
  snprintf(owm_req, size, "%s?id=%s&appid=%s", request.api_endpoint, 
                                                          request.city_id,
                                                          request.api_key);
}

static void prepare_req_geo(request_t request, char *owm_req, size_t size) {
  snprintf(owm_req, size, "%s?lat=%0.2f&lon=%0.2f&appid=%s", request.api_endpoint, 
                                                          request.coordinate.latitude,
                                                          request.coordinate.longitude,
                                                          request.api_key);
}

static void prepare_req_zip_code(request_t request, char *owm_req, size_t size) {
  snprintf(owm_req, size, "%s?zip=%s,%s&appid=%s", request.api_endpoint, 
                                                          request.zip_code,
                                                          request.country_code,
                                                          request.api_key);
}

static void owm_prepare_current_weather_request(request_t request, char *owm_req) {
  switch(request.request_type) {
    case CITY_NAME:
      prepare_req_city_name(request, owm_req, MAX_REQ_SIZE);
      break;
    case CITY_ID:
      prepare_req_city_id(request, owm_req, MAX_REQ_SIZE);
      break;
    case GEOGRAPHIC_COORDINATE:
      prepare_req_geo(request, owm_req, MAX_REQ_SIZE);
      break;
    case ZIP_CODE:
      prepare_req_zip_code(request, owm_req, MAX_REQ_SIZE);
      break;
    default:
      ESP_LOGE(TAG, "Error in preparing request");
      break;
  }
}

esp_err_t owm_parse_current_weather_response(char *response) {

  owm_clear_response();

  esp_err_t status = ESP_OK;
  cJSON *response_json = cJSON_Parse(response);
  if (response_json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      ESP_LOGE(TAG, "Error before: %s\n", error_ptr);
    }
    status = ESP_FAIL;
    goto end;
  }

  cJSON *coord = cJSON_GetObjectItemCaseSensitive(response_json, "coord");
  if (coord != NULL) {
    cJSON *lat = cJSON_GetObjectItemCaseSensitive(coord, "lat");
    coord_t _coord;
    if (cJSON_IsNumber(lat)) {
      _coord.lat = lat->valuedouble;
      ESP_LOGI(TAG, "Lat: %0.2f", _coord.lat);
    }

    cJSON *lon = cJSON_GetObjectItemCaseSensitive(coord, "lon");
    if (cJSON_IsNumber(lon)) {
      _coord.lon = lon->valuedouble;
      ESP_LOGI(TAG, "Lon: %0.2f", _coord.lon);
    }
    owm_set_response_coord(_coord);
  }

  cJSON *weathers = cJSON_GetObjectItemCaseSensitive(response_json, "weather");
  if (weathers != NULL) {
    cJSON *weather;
    cJSON_ArrayForEach(weather, weathers) {
      cJSON *id = cJSON_GetObjectItemCaseSensitive(weather, "id");
      weather_t _weather;
      if (cJSON_IsNumber(id)) {
        _weather.id = id->valueint;
        ESP_LOGI(TAG, "Weather Id: %d", _weather.id);
      }
      cJSON *wmain = cJSON_GetObjectItemCaseSensitive(weather, "main");
      if (cJSON_IsString(wmain) && (wmain->valuestring != NULL) && (strlen(wmain->valuestring) != 0)) {
        strncpy(_weather.main, wmain->valuestring, sizeof(_weather.main)); 
        ESP_LOGI(TAG, "Weather main: %s", _weather.main);
      }
      cJSON *description = cJSON_GetObjectItemCaseSensitive(weather, "description");
      if (cJSON_IsString(description) && (description->valuestring != NULL) && (strlen(description->valuestring) != 0)) {
        strncpy(_weather.description, description->valuestring, sizeof(_weather.description)); 
        ESP_LOGI(TAG, "Weather description: %s", _weather.description);
      }
      cJSON *icon = cJSON_GetObjectItemCaseSensitive(weather, "icon");
      if (cJSON_IsString(icon) && (icon->valuestring != NULL) && (strlen(icon->valuestring) != 0)) {
        strncpy(_weather.icon, icon->valuestring, sizeof(_weather.icon)); 
        ESP_LOGI(TAG, "Weather icon: %s", _weather.icon);
      }
      owm_set_response_weather(_weather);
    }
  }

  cJSON *base = cJSON_GetObjectItemCaseSensitive(response_json, "base");
  if (cJSON_IsString(base) && (base->valuestring != NULL) && (strlen(base->valuestring) != 0)) {
    owm_set_response_base(base->valuestring);
    ESP_LOGI(TAG, "base: %s", base->valuestring);
  }

  cJSON *main_json = cJSON_GetObjectItemCaseSensitive(response_json, "main");
  if (main_json != NULL) {
    main_t _main;
    cJSON *temp = cJSON_GetObjectItemCaseSensitive(main_json, "temp");
    if (cJSON_IsNumber(temp)) {
      _main.temp = temp->valuedouble;
      ESP_LOGI(TAG, "main temp: %0.2f", _main.temp);
    }
    cJSON *feels_like = cJSON_GetObjectItemCaseSensitive(main_json, "feels_like");
    if (cJSON_IsNumber(feels_like)) {
      _main.feels_like = feels_like->valuedouble;
      ESP_LOGI(TAG, "main feels_like: %0.2f", _main.feels_like);
    }
    cJSON *temp_min = cJSON_GetObjectItemCaseSensitive(main_json, "temp_min");
    if (cJSON_IsNumber(temp_min)) {
      _main.temp_min = temp_min->valuedouble;
      ESP_LOGI(TAG, "main temp_min: %0.2f", _main.temp_min);
    }
    cJSON *temp_max = cJSON_GetObjectItemCaseSensitive(main_json, "temp_max");
    if (cJSON_IsNumber(temp_max)) {
      _main.temp_max = temp_max->valuedouble;
      ESP_LOGI(TAG, "main temp_max: %0.2f", _main.temp_max);
    }
    cJSON *pressure = cJSON_GetObjectItemCaseSensitive(main_json, "pressure");
    if (cJSON_IsNumber(pressure)) {
      _main.pressure = pressure->valuedouble;
      ESP_LOGI(TAG, "main pressure: %0.2f", _main.pressure);
    }
    cJSON *humidity = cJSON_GetObjectItemCaseSensitive(main_json, "humidity");
    if (cJSON_IsNumber(humidity)) {
      _main.humidity = humidity->valuedouble;
      ESP_LOGI(TAG, "main humidity: %0.2f", _main.humidity);
    }
    owm_set_response_main(_main);
  }

  cJSON *visibility = cJSON_GetObjectItemCaseSensitive(response_json, "visibility"); 
  if (cJSON_IsNumber(visibility)) {
    owm_set_response_visibility(visibility->valueint);
    ESP_LOGI(TAG, "visibility: %d", visibility->valueint);
  }

  cJSON *wind = cJSON_GetObjectItemCaseSensitive(response_json, "wind");
  if (wind != NULL) {
    wind_t _wind;
    cJSON *speed = cJSON_GetObjectItemCaseSensitive(wind, "speed");
    if (cJSON_IsNumber(speed)) {
      _wind.speed = speed->valuedouble;
      ESP_LOGI(TAG, "wind speed: %0.2f", _wind.speed);
    }
    cJSON *deg = cJSON_GetObjectItemCaseSensitive(wind, "deg");
    if (cJSON_IsNumber(deg)) {
      _wind.deg = deg->valuedouble;
      ESP_LOGI(TAG, "wind deg: %0.2f", _wind.deg);
    }
    cJSON *gust = cJSON_GetObjectItemCaseSensitive(wind, "gust");
    if (cJSON_IsNumber(gust)) {
      _wind.gust = gust->valuedouble;
      ESP_LOGI(TAG, "wind gust: %0.2f", _wind.gust);
    }
    owm_set_response_wind(_wind);
  }
  
  cJSON *clouds = cJSON_GetObjectItemCaseSensitive(response_json, "clouds");
  if (clouds != NULL) {
    clouds_t _clouds;
    cJSON *all = cJSON_GetObjectItemCaseSensitive(clouds, "all");
    if (cJSON_IsNumber(all)) {
      _clouds.all = all->valueint;
      ESP_LOGI(TAG, "clouds all: %d", _clouds.all);
    }
    owm_set_response_clouds(_clouds);
  }

  cJSON *dt = cJSON_GetObjectItemCaseSensitive(response_json, "dt");
  if (cJSON_IsNumber(dt)) {
    owm_set_response_dt((int64_t)dt->valuedouble);
    ESP_LOGI(TAG, "dt: %lld", (int64_t)dt->valuedouble);
  }

  cJSON *sys = cJSON_GetObjectItemCaseSensitive(response_json, "sys");
  if (sys != NULL) {
    sys_t _sys;
    cJSON *type = cJSON_GetObjectItemCaseSensitive(sys, "type");
    if (cJSON_IsNumber(type)) {
      _sys.type = type->valueint;
      ESP_LOGI(TAG, "sys type: %d", _sys.type);
    }
    cJSON *sid = cJSON_GetObjectItemCaseSensitive(sys, "id");
    if (cJSON_IsNumber(sid)) {
      _sys.id = sid->valueint;
      ESP_LOGI(TAG, "sys id: %d", _sys.id);
    }
    cJSON *country = cJSON_GetObjectItemCaseSensitive(sys, "country");
    if (cJSON_IsString(country) && (country->valuestring != NULL) && (strlen(country->valuestring) != 0)) {
      strncpy(_sys.country, country->valuestring, sizeof(_sys.country));
      ESP_LOGI(TAG, "sys country: %s", _sys.country);
    }
    cJSON *sunrise = cJSON_GetObjectItemCaseSensitive(sys, "sunrise");
    if (cJSON_IsNumber(sunrise)) {
      _sys.sunrise = (int64_t) sunrise->valuedouble;
      ESP_LOGI(TAG, "sys sunrise: %lld", _sys.sunrise);
    } 
    cJSON *sunset = cJSON_GetObjectItemCaseSensitive(sys, "sunset");
    if (cJSON_IsNumber(sunset)) {
      _sys.sunset = (int64_t) sunset->valuedouble;
      ESP_LOGI(TAG, "sys sunset: %lld", _sys.sunset);
    }
    owm_set_response_sys(_sys);
  }

  cJSON *timezone = cJSON_GetObjectItemCaseSensitive(response_json, "timezone");
  if (cJSON_IsNumber(timezone)) {
    owm_set_response_timezone((int64_t)timezone->valueint);
    ESP_LOGI(TAG, "timezone: %lld", (int64_t)timezone->valueint);
  }

  cJSON *id = cJSON_GetObjectItemCaseSensitive(response_json, "id");
  if (cJSON_IsNumber(id)) {
    owm_set_response_id((uint32_t) id->valuedouble);
    ESP_LOGI(TAG, "id: %u", (uint32_t)id->valuedouble);
  }

  cJSON *name = cJSON_GetObjectItemCaseSensitive(response_json, "name");
  if (cJSON_IsString(name) && (name->valuestring != NULL) && (strlen(name->valuestring) != 0)) {
    owm_set_response_name(name->valuestring);
    ESP_LOGI(TAG, "name: %s", name->valuestring);
  }

  cJSON *cod = cJSON_GetObjectItemCaseSensitive(response_json, "cod");
  if (cJSON_IsNumber(cod)) {
    owm_set_response_cod(cod->valueint);
    ESP_LOGI(TAG, "code: %d", cod->valueint);
  }

  end:
    cJSON_Delete(response_json);
    return status;
}

void owm_request_current_weather(request_t request) {

  char req[MAX_REQ_SIZE];
  char content[MAX_RESPONSE_SIZE];

  bzero(req, MAX_REQ_SIZE);
  bzero(content, MAX_RESPONSE_SIZE);

  reset_request_condition();

  owm_prepare_current_weather_request(request, req);
  https_with_url(req);

  while (!is_request_done());

  if (is_request_done()) {
    read_content(content, MAX_RESPONSE_SIZE);
    //printf("%s\n", content);
    owm_parse_current_weather_response(content);
  }
}
