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
    TaskHandle_t uart_handle;
    TaskHandle_t task_handle;
    app_init();
    xTaskCreate( uart_api_loop, "Uart loop", 4096, NULL, tskIDLE_PRIORITY, &uart_handle );
    xTaskCreate( task_run, "Uart loop", 4096, NULL, tskIDLE_PRIORITY, &task_handle );
}