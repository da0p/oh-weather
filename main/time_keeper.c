#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_sntp.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "time_keeper.h"

/* Timer interval once every day (24 Hours) */
#define TIME_PERIOD (86400000000ULL)

static const char *TAG = "time_sync";

void time_keeper_init(void) {

    time_keeper_fetch_time_and_sync(NULL);

    const esp_timer_create_args_t update_timer_args = {
    .callback = &time_keeper_fetch_time_and_sync
    };
    esp_timer_handle_t update_timer;
    ESP_ERROR_CHECK(esp_timer_create(&update_timer_args, &update_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(update_timer, TIME_PERIOD));
}
void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
    sntp_init();
}

static void obtain_time(void)
{
    /**
     * NTP server address could be aquired via DHCP,
     * see LWIP_DHCP_GET_NTP_SRV menuconfig option
     */
#ifdef LWIP_DHCP_GET_NTP_SRV
    sntp_servermode_dhcp(1);
#endif

    // wait for time to be set
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) {
        ESP_LOGI(TAG, "Waiting for system time to be set...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void time_keeper_fetch_time_and_sync(void *args)
{
    initialize_sntp();
    obtain_time();
    sntp_stop();
}

void time_keeper_set_time_zone(char *time_zone) {
    setenv("TZ", time_zone, 1);
    tzset();
}