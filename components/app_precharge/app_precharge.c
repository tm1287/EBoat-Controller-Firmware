#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"

#include "app_vehicleStateManager.h"
#include "app_precharge.h"
#include "dev_contactor.h"
#include "driver/gptimer.h"

#define GPIO_PRECHARGE_RELAY_PIN GPIO_NUM_4

#define PRECHARGE_RELAY_CLOSED 1
#define PRECHARGE_RELAY_OPEN !PRECHARGE_RELAY_CLOSED

#define TIMER_RESOLUTION_TICKS_PER_S (1000000)

#define PRECHARGE_TIMER_DURATION_MS 3000

#define MS_TO_US(ms) (ms*1000)

static const char* TAG = "app_precharge";
vehicle_state_manager_data_S* data = NULL;

static gptimer_handle_t prechargeTimer = NULL;

static bool IRAM_ATTR prechargeTimerCallback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
    // stop timer immediately
    gptimer_stop(timer);
    // Retrieve count value and send to queue
    ESP_DRAM_LOGI(TAG, "Key Present?: %d", edata->count_value);

    //example_queue_element_t ele = {
    //    .event_count = edata->count_value
    //};
    //xQueueSendFromISR(queue, &ele, &high_task_awoken);
    
    // return whether we need to yield at the end of ISR
    return (high_task_awoken == pdTRUE);
}

void app_precharge_init(){
    gpio_config_t channelCfg = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << GPIO_PRECHARGE_RELAY_PIN),
        .pull_down_en = 0,
        .pull_up_en = 1,
    };
    gpio_config(&channelCfg);

    dev_contactor_open();

    ESP_LOGI(TAG, "Create timer handle");
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TIMER_RESOLUTION_TICKS_PER_S, // 1MHz, 1 tick=1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &prechargeTimer));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = prechargeTimerCallback,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(prechargeTimer, &cbs, NULL));

    ESP_LOGI(TAG, "Enable timer");
    ESP_ERROR_CHECK(gptimer_enable(prechargeTimer));

    ESP_LOGI(TAG, "Start timer, stop it at alarm event");
    gptimer_alarm_config_t prechargeAlarmConfig = {
        .alarm_count = 3 * TIMER_RESOLUTION_TICKS_PER_S, // period = 3s
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(prechargeTimer, &prechargeAlarmConfig));
}

void app_precharge_run(void* pvParameters){
    // Setup a 3 second timer
    for ( ;; ){
        uint64_t count;
        ESP_ERROR_CHECK(gptimer_get_raw_count(prechargeTimer, &count));
        double elapsedTime = count * (1.0f / TIMER_RESOLUTION_TICKS_PER_S) * 1000;

        if(!(count > 0)){
            ESP_ERROR_CHECK(gptimer_start(prechargeTimer));
        }

        ESP_LOGI(TAG, "Timer count value=%.2f ms", elapsedTime);

        gpio_set_level(GPIO_PRECHARGE_RELAY_PIN, PRECHARGE_RELAY_CLOSED);
        // Monitor Vbatt
        // Monitor Vesc
        // Check to see if within 5% of each other
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
