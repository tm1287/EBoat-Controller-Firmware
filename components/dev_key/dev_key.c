#include <stdio.h>
#include "dev_key.h"
#include "esp_attr.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "app_vehicleStateManager.h"
#include "esp_log.h"

//#define GPIO_KEY_PIN GPIO_NUM_21
#define GPIO_KEY_PIN GPIO_NUM_23

static const char* TAG = "dev_key";

static bool isKeyPresent;

static void IRAM_ATTR dev_key_gpio_event_handler(void){
    int gpioValue = gpio_get_level(GPIO_KEY_PIN);
    isKeyPresent = (gpioValue == 0);

    ESP_DRAM_LOGI(TAG, "Key Present?: %d", isKeyPresent);
}

void dev_key_init(){
    gpio_config_t channelCfg = {
        .intr_type = GPIO_INTR_ANYEDGE, //Rising and Falling Edge
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << GPIO_KEY_PIN),
        .pull_down_en = 0,
        //.pull_up_en = 0, //Already has hardware pullup
        .pull_up_en = 1,
    };

    gpio_config(&channelCfg);

    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3);
    gpio_isr_handler_add(GPIO_KEY_PIN, (gpio_isr_t) dev_key_gpio_event_handler, NULL);

    isKeyPresent = false;
}

bool dev_key_isKeyPresent(){
    return isKeyPresent;
}