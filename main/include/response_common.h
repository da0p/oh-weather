#pragma once
#include <stdlib.h>
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_RESPONSE_SIZE 1024

typedef struct coord_t {
    double lat;
    double lon;
} coord_t;

typedef struct weather_t {
    uint32_t id;
    char main[64];
    char description[64];
    char icon[32];
}weather_t;

typedef struct main_t {
    double temp;
    double feels_like;
    double temp_min;
    double temp_max;
    double pressure;
    double humidity;
} main_t;

typedef struct wind_t {
    double speed;
    double deg;
    double gust;
} wind_t;

typedef struct clouds_t {
    int all;
} clouds_t;

typedef struct sys_t {
    int type;
    uint32_t id;
    float message;
    char country[8];
    int64_t sunrise;
    int64_t sunset;
} sys_t;

typedef struct response_t {
    coord_t coord;
    weather_t weather;
    char base[64];
    main_t main;
    int visibility;
    wind_t wind;
    clouds_t clouds;
    int64_t dt;
    sys_t sys;
    int64_t timezone;
    uint32_t id;
    char name[256];
    int cod;
}response_t;

#ifdef __cplusplus
}
#endif
