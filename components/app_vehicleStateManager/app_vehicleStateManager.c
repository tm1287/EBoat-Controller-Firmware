#include <stdio.h>
#include <string.h>
#include "esp_log.h"

#include "dev_key.h"

#include "app_vehicleStateManager.h"
#include "app_precharge.h"

#define VEHICLE_POWER_STATE_TASK_PRIORITY 2

static const char* TAG = "app_vsm";

static QueueHandle_t stateReqQueueHandler = NULL;
static TaskHandle_t currVehicleStateTask = NULL;

void app_vehicleStateManager_init(vehicle_state_manager_data_S* data) {
    memset(data, 0, sizeof(vehicle_state_manager_data_S));
    
    stateReqQueueHandler = xQueueCreate(10, sizeof(uint32_t));

    data->vehicleState = VEHICLE_STATE_IDLE;
    data->prevVehicleState = VEHICLE_STATE_IDLE;
    data->stateRequestQueue = stateReqQueueHandler;
}


void app_vehicleStateManager_run(void* pvParameters) {
    vehicle_state_manager_data_S* data = (vehicle_state_manager_data_S*) pvParameters;
    for( ;; )
    {
        uint32_t rxBuffer;
        if(xQueueReceive(stateReqQueueHandler, &rxBuffer, pdMS_TO_TICKS(100))){
            vehicle_state_E nextState = (vehicle_state_E) rxBuffer;
            
            switch(nextState){
                case VEHICLE_STATE_PRECHARGE:
                    //xTaskCreate( app_precharge_run, "app_precharge", 400, (void*) data, VEHICLE_POWER_STATE_TASK_PRIORITY, &currVehicleStateTask);
                    break;
                default:
                    data->vehicleState = VEHICLE_STATE_FAULT;
            }
            // Remember to update prevVehicleState
            // Also remember to update vehicleState from nextState
        } else {
            ESP_LOGI(TAG, "Aww, no state req in queue :(, check back in a bit.");
        }
    }
}