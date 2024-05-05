#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/event_groups.h"
#include "esp_http_server.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "soft-ap.h"
#include "http-server.h"

// #include "../mdns/include/mdns.h"

// Define your Wi-Fi event handler function
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_STA_START) {
        // Start Wi-Fi scanning when STA mode starts
        esp_wifi_scan_start(NULL, true);
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // Handle Wi-Fi disconnection events
        // Reconnect here if needed
    }
}

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // TODO: 3. Pornire mod STA + scanare SSID-uri disponibile

    // Initialize Wi-Fi in STA mode
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_t *wifi_netif = esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);

    // Register Wi-Fi event handler
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));

    // Start Wi-Fi
    esp_wifi_start();

    // TODO: 4. Initializare mDNS (daca mai ramana timp)    

    // TODO: 1. Pornire softAP

    wifi_init_softap();

    // TODO: 2. Pornire server web (si config specifice in http-server.c) 
    start_webserver();
}