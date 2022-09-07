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
    while(1){
        uart_api_loop();
        task_run();
    }
}