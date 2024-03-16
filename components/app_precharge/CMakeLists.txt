idf_component_register(SRCS "app_precharge.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver dev_contactor app_vehicleStateManager
)
