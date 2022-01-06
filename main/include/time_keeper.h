#pragma once
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Timer keeper init
 *
 */
void time_keeper_init(void);

/**
 * @brief Fetch the current time from SNTP and synchronize with esp32 system time
 *
 */
void time_keeper_fetch_time_and_sync(void*);

/**
 * @brief Set time zone
 *
 */
void time_keeper_set_time_zone(char *time_zone);

#ifdef __cplusplus
}
#endif
