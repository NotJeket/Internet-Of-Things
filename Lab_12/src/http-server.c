#include <string.h>
#include "esp_log.h"
#include "esp_http_server.h"
#include "http-server.h"

static const char *TAG = "HTTP_SERVER";

esp_err_t hello_get_handler(httpd_req_t *req)
{
    httpd_resp_send(req, "Hello, ESP32!", strlen("Hello, ESP32!"));
    return ESP_OK;
}

httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    ESP_LOGI(TAG, "Starting HTTP server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        httpd_uri_t uri = {
            .uri       = "/hello",
            .method    = HTTP_GET,
            .handler   = hello_get_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &uri);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}
