#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
 
#include "lwip/err.h"
#include "lwip/sys.h"
#include "freertos/event_groups.h"
 
#include "esp_http_server.h"
 
#define DEFAULT_SCAN_LIST_SIZE 30
 
uint16_t number = DEFAULT_SCAN_LIST_SIZE;
wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
/* Our URI handler function to be called during GET /uri request */
// esp_err_t get_handler(httpd_req_t *req)
// {
 
//     /* Send a simple response */
//     const char resp[] = "<html> <body> <form action=/results.html target=_blank method=post> <label for=fname>Networks found:</label> <br> <select name=ssid> <option value=ssid-exemplu-1>ssid-exemplu-1</option> <option value=ssid-exemplu-2>ssid-exemplu-2</option> <option value=ssid-exemplu-3>ssid-exemplu-3</option> <option value=ssid-exemplu-4>ssid-exemplu-4</option> </select> <br> <label for=ipass>Security key:</label><br> <input type=password name=ipass><br> <input type=submit value=Submit> </form> </body> </html>";
 
//     const char header[]="<html> <body> <form action=/results.html target=_blank method=post> <label for=fname>Networks found:</label> <br> <select name=ssid>";
//     const char footer[]="</select> <br> <label for=ipass>Security key:</label><br> <input type=password name=ipass><br> <input type=submit value=Submit> </form> </body> </html>";
 
//     const char template[]="<option 'value=%s>%s</option>";
 
//     strcat(resp,header);
 
//     //ap_info.length()
 
//     // for (int i=0;i<)
 
//     strcat(resp,footer);
 
//     httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);  // TODO: Trimitere sir de caractere ce contine pagina web prezentata in laborator (lista populata cu rezultatele scanarii)
//     return ESP_OK;
// }
 
esp_err_t get_handler(httpd_req_t *req)
{
    /* Send a simple response */
    char resp[1024]; 
    sprintf(resp, "<html> <body> <form action=/results.html target=_blank method=post> <label for=fname>Networks found:</label> <br> <select name=ssid>");
 
    printf("ap_num: %d\n", number);
 
    for (int i = 0; i < number; i++) {
        printf("SSID: %s\n", ap_info[i].ssid);
        sprintf(resp + strlen(resp), "<option value=%s>%s</option>", ap_info[i].ssid, ap_info[i].ssid);
    }
 
    strcat(resp, "</select> <br> <label for=ipass>Security key:</label><br> <input type=password name=ipass><br> <input type=submit value=Submit> </form> </body> </html>");
 
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
 
    return ESP_OK;
}
 
void scanare_ap(void){
 
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);
 
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
 
    memset(ap_info, 0, sizeof(ap_info));
 
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_scan_start(NULL, true);
 
    esp_wifi_scan_get_ap_records(&number, ap_info);
 
 
    esp_wifi_stop();
    esp_wifi_deinit();
    esp_wifi_clear_default_wifi_driver_and_handlers(sta_netif);
    esp_netif_destroy(sta_netif);
}
 
 
/* Our URI handler function to be called during POST /uri request */
esp_err_t post_handler(httpd_req_t *req)
{
    /* Destination buffer for content of HTTP POST request.
     * httpd_req_recv() accepts char* only, but content could
     * as well be any binary data (needs type casting).
     * In case of string data, null termination will be absent, and
     * content length would give length of string */
    char content[100];
 
    /* Truncate if content length larger than the buffer */
    size_t recv_size = MIN(req->content_len, sizeof(content));
 
    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) {  /* 0 return value indicates connection closed */
        /* Check if timeout occurred */
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            /* In case of timeout one can choose to retry calling
             * httpd_req_recv(), but to keep it simple, here we
             * respond with an HTTP 408 (Request Timeout) error */
            httpd_resp_send_408(req);
        }
        /* In case of error, returning ESP_FAIL will
         * ensure that the underlying socket is closed */
        return ESP_FAIL;
    }
 
    content[ret] = '\0';
 
    char ssid[50];
    char password[50];
 
    sscanf(content, "ssid=%49[^&]&ipass=%49s", ssid, password);
 
    char resp[300];
    snprintf(resp, sizeof(resp), "SSID: %s, Password: %s", ssid, password);
 
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
 
/* URI handler structure for GET /uri */
httpd_uri_t uri_get = {
    .uri      = "/index.html",
    .method   = HTTP_GET,
    .handler  = get_handler,
    .user_ctx = NULL
};
 
/* URI handler structure for POST /uri */
httpd_uri_t uri_post = {
    .uri      = "/results.html",
    .method   = HTTP_POST,
    .handler  = post_handler,
    .user_ctx = NULL
};
 
/* Function for starting the webserver */
httpd_handle_t start_webserver(void)
{
    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
 
    /* Empty handle to esp_http_server */
    httpd_handle_t server = NULL;
 
    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK) {
        /* Register URI handlers */
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post);
    }
    /* If server failed to start, handle will be NULL */
    return server;
}
 
/* Function for stopping the webserver */
void stop_webserver(httpd_handle_t server)
{
    if (server) {
        /* Stop the httpd server */
        httpd_stop(server);
    }
}