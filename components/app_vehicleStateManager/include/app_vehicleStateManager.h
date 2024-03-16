#include "freertos/FreeRTOS.h"

#ifndef APP_VEHICLE_STATE_MANAGER_H
#define APP_VEHICLE_STATE_MANAGER_H

typedef enum {
    VEHICLE_STATE_IDLE,
    VEHICLE_STATE_PRECHARGE,
    VEHICLE_STATE_READY,
    VEHICLE_STATE_RUN,
    VEHICLE_STATE_FAULT
} vehicle_state_E;

typedef struct {
    vehicle_state_E vehicleState;
    vehicle_state_E prevVehicleState;
    QueueHandle_t stateRequestQueue;
} vehicle_state_manager_data_S;

void app_vehicleStateManager_init(vehicle_state_manager_data_S* data);
void app_vehicleStateManager_run(void* pvParameters);

#endif