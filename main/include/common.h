#pragma once
#include "esp_system.h"
#ifdef __cplusplus
extern "C" {
#endif
#define STORAGE_MOUNTPOINT    "/storage"
#define WEB_MOUNTPOINT        "/www"
#define STORAGE_LABEL         "storage"
#define WEB_LABEL             "www"
#define CONFIGFILE_DIR        "/storage/config.json"
#define MAX_CONFIG_LENGTH     2048
#ifdef __cplusplus
}
#endif