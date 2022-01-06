#include <string.h>
#include "esp_event_base.h"
#include "esp_wifi_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "wifi_manager.h"

/* Default setting for WiFi */
#define DEFAULT_AP_WIFI_CHANNEL              1
#define DEFAULT_AP_MAX_STA_CONS              4

/* FreeRTOS event group to signal when we are connected */
static EventGroupHandle_t s_wifi_event_group;

#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1

static const char *TAG = "wifi station";

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT) {
        switch(event_id) {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                esp_wifi_connect();
                ESP_LOGI(TAG, "retry to connect to the AP");
                break;
            case WIFI_EVENT_AP_STACONNECTED:
            {
                wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t*) event_data;
                ESP_LOGI(TAG, "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
                break;
            }
            case WIFI_EVENT_AP_STADISCONNECTED:
            {
                wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t*) event_data;
                ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d", MAC2STR(event->mac), event->aid);
            }
            default:
                break;
        }
    } else if (event_base == IP_EVENT) {
       switch(event_id) {
           case IP_EVENT_STA_GOT_IP:
           {
                ip_event_got_ip_t *event = (ip_event_got_ip_t*) event_data;
                ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
                xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
                break;
           }
            default:
                break;
       } 
    }
}

static void wifi_sta_start(char *wifi_ssid, char *passphrase) {

    s_wifi_event_group = xEventGroupCreate();

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config;

    strncpy((char*) wifi_config.sta.ssid, wifi_ssid, sizeof(wifi_config.sta.ssid) - 1);
    ESP_LOGI(TAG, "wifi_ssid: %s", wifi_config.sta.ssid);
    strncpy((char*) wifi_config.sta.password, passphrase, sizeof(wifi_config.sta.password) - 1);
    ESP_LOGI(TAG, "wifi_password: %s", wifi_config.sta.password);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "wifi sta finished");

    /* Wait until either the connection is established (WIFI_CONNECTED_BIT) or connection failed */ 
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s", wifi_config.sta.ssid);
    } else {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s", wifi_config.sta.ssid);
    }

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
}

static void wifi_softap_start(char *wifi_ssid, char *passphrase) {

    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));
    
    wifi_config_t wifi_config;
    strncpy((char*) wifi_config.ap.ssid, wifi_ssid, sizeof(wifi_config.ap.ssid) - 1);
    strncpy((char*) wifi_config.ap.password, passphrase, sizeof(wifi_config.ap.password) - 1);
    wifi_config.ap.ssid_len = strlen(wifi_ssid);
    wifi_config.ap.channel = DEFAULT_AP_WIFI_CHANNEL;
    wifi_config.ap.max_connection = DEFAULT_AP_MAX_STA_CONS;
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;

    if (strlen(wifi_ssid) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
                wifi_config.ap.ssid, wifi_config.ap.password, wifi_config.ap.channel);
}

void wifi_manager_start(char *wifi_ssid, char *passphrase, wifi_mode_t mode) {
    
    switch(mode) {
        case WIFI_MODE_STA:
            wifi_sta_start(wifi_ssid, passphrase);
            break;
        case WIFI_MODE_AP:
            wifi_softap_start(wifi_ssid, passphrase);
            break;
        default:
            break;
    }
}