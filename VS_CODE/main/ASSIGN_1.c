#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_task_wdt.h"  

#define DEBUG_FACTOR 1

#define PHBTN_1 GPIO_NUM_5 
#define PHBTN_2 GPIO_NUM_16
#define WAVEFORM GPIO_NUM_33
#define ALT_WAVEFORM GPIO_NUM_15

bool dataEnabled = false;
bool altMode = false;

void IRAM_ATTR toggleData(void* arg) {
    dataEnabled = !dataEnabled;
}

void IRAM_ATTR toggleMode(void* arg) {
    altMode = !altMode;
}

void delayMicroseconds(int us) {
    int64_t start_time = esp_timer_get_time();
    while ((esp_timer_get_time() - start_time) < us) {
        vTaskDelay(1);
    }
}

void generateWaveform(void* arg) {
    esp_task_wdt_add(NULL);

    int p = 1100;
    int e = 500;
    int n = 17;
    int u = 3000;
    int sync_on = 50;

    int debug_multiplier = DEBUG_FACTOR;
    p *= debug_multiplier;
    e *= debug_multiplier;
    u *= debug_multiplier;
    sync_on *= debug_multiplier;

    while (1) {  
        esp_task_wdt_reset();

        if (dataEnabled) {  
            for (int i = 0; i < n; i++) {
                gpio_set_level(WAVEFORM, 1);
                delayMicroseconds(p + (i * 50) * debug_multiplier);
                gpio_set_level(WAVEFORM, 0);
                delayMicroseconds(e);  
            }

            delayMicroseconds(u);
            gpio_set_level(ALT_WAVEFORM, 1);
            delayMicroseconds(sync_on);
            gpio_set_level(ALT_WAVEFORM, 0);
        }

        vTaskDelay(1);
    }
}

void app_main() {
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pin_bit_mask = (1ULL << PHBTN_1) | (1ULL << PHBTN_2);
    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(PHBTN_1, toggleData, NULL);
    gpio_isr_handler_add(PHBTN_2, toggleMode, NULL);

    gpio_set_direction(WAVEFORM, GPIO_MODE_OUTPUT);
    gpio_set_direction(ALT_WAVEFORM, GPIO_MODE_OUTPUT);

    xTaskCreatePinnedToCore(generateWaveform, "waveform_task", 4096, NULL, 1, NULL, 1);
}