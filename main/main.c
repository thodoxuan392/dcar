#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bluetooth_core.h"
#include "bluetooth_evt_handler.h"
#include "uart_api.h"
#include "task.h"

void app_init(){
    bluetooth_init();
    bluetooth_evt_handler_init();
    uart_api_init();
    task_init();
}


void app_main(void)
{
    app_init();
    // xTaskCreatePinnedToCore( task_run, "Task loop", 8096, NULL, tskIDLE_PRIORITY, NULL , 0);
    // xTaskCreatePinnedToCore( uart_api_loop, "Uart loop", 8096, NULL, tskIDLE_PRIORITY, NULL , 0 );
    while(1){
        uart_api_loop();
        task_run();
    }
}