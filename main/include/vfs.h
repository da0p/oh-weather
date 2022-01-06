#pragma once
#include "esp_system.h"
#ifdef __cplusplus
extern "C" {
#endif

esp_err_t vfs_init(char *storage_mp, char *storage_label, char *web_mp, char *web_label);

#ifdef __cplusplus
}
#endif