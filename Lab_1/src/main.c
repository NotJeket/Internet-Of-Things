#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define GPIO_OUTPUT_IO          4
#define GPIO_INPUT_IO           2
#define GPIO_OUTPUT_PIN_SEL     (1ULL<<GPIO_OUTPUT_IO)
#define GPIO_INPUT_PIN_SEL      (1ULL<<GPIO_INPUT_IO)


volatile int button_press_count = 0;

void button_task(void *pvParameter) {
    
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_NEGEDGE; 
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1; 
    gpio_config(&io_conf);
    
    while(1) {

        if(gpio_get_level(GPIO_INPUT_IO) == 0) {
            button_press_count++;
            printf("Button press count: %d\n", button_press_count);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            while(gpio_get_level(GPIO_INPUT_IO) == 0) {
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void app_main() {
    // Zero-initialize the config structure.
    gpio_config_t io_conf = {};
    // Disable interrupt for output GPIO
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // Set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    // Bit mask of the pins that you want to set
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    // Disable pull-down mode
    io_conf.pull_down_en = 0;
    // Disable pull-up mode
    io_conf.pull_up_en = 0;
    // Configure GPIO with the given settings
    gpio_config(&io_conf);
    //button press
    xTaskCreate(&button_task, "button_task", 2048, NULL, 10, NULL);
    
    int cnt = 0;
    while(1) {
        // printf("cnt: %d\n", cnt++);
        //1 sec
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_IO, cnt % 2);
        
        //0.5 sec
        vTaskDelay(500 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_IO, !(cnt % 2));
        
        //0.25 sec
        vTaskDelay(250 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_IO, cnt % 2);
        
        //0.75 sec
        vTaskDelay(750 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_IO, !(cnt % 2));
    }
}

    //normal code
//     int cnt = 0;
//     while(1) {
//         printf("cnt: %d\n", cnt++);
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//         gpio_set_level(GPIO_OUTPUT_IO, cnt % 2);
//     };
// }

// modified code for task 2

