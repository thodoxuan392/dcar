idf_component_register(SRCS "bluetooth_core.c" "bluetooth_evt_handler.c" "bluetooth_uart_converter.c"
                    INCLUDE_DIRS "include" 
                    REQUIRES nvs_flash bt freertos uart) 

