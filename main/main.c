#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "dev_key.h"
#include "app_vehicleStateManager.h"
#include "app_precharge.h"

static vehicle_state_manager_data_S vehicle_state_manager_data; 
static vehicle_state_manager_data_S* data = &vehicle_state_manager_data;

void initDevs() {
    dev_key_init();
}

void initApps() {
    app_vehicleStateManager_init(data);
    app_precharge_init();
}
    

void app_main(void)
{
    initDevs();
    initApps();

    // Start Tasks
    //xTaskCreate( app_vehicleStateManager_run, "app_vehicle_state_manager", configMINIMAL_STACK_SIZE, (void*) data, 15, NULL);
    xTaskCreate( app_precharge_run, "app_precharge", configMINIMAL_STACK_SIZE * 2, (void*) data, 10, NULL);
}
