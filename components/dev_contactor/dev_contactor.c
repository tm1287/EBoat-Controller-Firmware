#include <stdio.h>
#include "dev_contactor.h"
#include "driver/gpio.h"

#define GPIO_CONTACTOR_PIN GPIO_NUM_2 

#define CONTACTORS_CLOSED 1
#define CONTACTORS_OPEN !CONTACTORS_CLOSED

void dev_contactor_init(){
    gpio_config_t channelCfg = {
        .intr_type = GPIO_INTR_DISABLE, //Rising and Falling Edge
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << GPIO_CONTACTOR_PIN),
        .pull_down_en = 0,
        .pull_up_en = 1,
    };
    gpio_config(&channelCfg);
}

void dev_contactor_open() {
    gpio_set_level(GPIO_CONTACTOR_PIN, (uint32_t) CONTACTORS_OPEN);
}

void dev_contactor_close() {
    gpio_set_level(GPIO_CONTACTOR_PIN, (uint32_t) CONTACTORS_CLOSED);
}
