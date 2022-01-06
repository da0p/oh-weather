#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_REQ_SIZE                  1024
#define MAX_API_END_POINT_LENGTH      512
#define MAX_API_KEY_LENGTH            128
#define MAX_CITY_NAME_LENGTH          128
#define MAX_CITY_ID_LENGTH            64
#define MAX_ZIP_CODE_LENGTH           32
#define MAX_STATE_CODE_LENGTH         16
#define MAX_COUNTRY_CODE_LENGTH       16

#define OPEN_WEATHER_MAP              "openweathermap"

typedef enum request_platform_t {
  OPEN_WEATHER_MAP_REQ
} request_platform_t;

typedef enum request_type_t {
  UNKNOWN,
  CITY_NAME,
  CITY_ID,
  GEOGRAPHIC_COORDINATE,
  ZIP_CODE
}request_type_t;

typedef struct coordinate_t {
  double latitude;
  double longitude;
}coordinate_t;

typedef struct request_t {
  request_platform_t request_platform;
  request_type_t request_type;
  char api_endpoint[MAX_API_END_POINT_LENGTH];
  char api_key[MAX_API_KEY_LENGTH];
  char city_name[MAX_CITY_NAME_LENGTH]; 
  char city_id[MAX_CITY_ID_LENGTH];
  char state_code[MAX_STATE_CODE_LENGTH];
  char country_code[MAX_COUNTRY_CODE_LENGTH];
  coordinate_t coordinate;
  char zip_code[MAX_ZIP_CODE_LENGTH];
}request_t;

#ifdef __cplusplus
}
#endif