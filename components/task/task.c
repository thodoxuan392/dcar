#include "task.h"
#include "stdio.h"
#include "stdint.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "bluetooth_evt_handler.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "uart_api.h"
#include "string.h"

static const char * TAG = "Task";

static const uint8_t max_retry_num = 3;
static const uint16_t retry_interval = 200;          // 200ms
static uint8_t retry_current = 0;


static task_state_t task_state = TASK_STATE_WAIT_BT_CMD;

//Current Command and Respond
static cmd_t current_cmd;
static uint8_t * buffer_ptr;

// Retry Timer handler
static bool retry_flag = false;
static void retry_timer_cb(void * arg);
esp_timer_handle_t retry_timer;

// Previous CMD
static const uint16_t cmd_time_threshold = 200; //200 ms
TickType_t previous_cmd_time = 0;
TickType_t current_time;

const esp_timer_create_args_t retry_timer_args = {
            .callback = &retry_timer_cb,
            /* argument specified here will be passed to timer callback function */
            .arg = (void*) &retry_flag,
};

void task_init(){
    // Init timer for countdown to retry
    esp_timer_create(&retry_timer_args, &retry_timer);
}


void task_run(){
    // while(1){
        
    // }
    current_time = xTaskGetTickCount();
    switch (task_state)
        {
        case TASK_STATE_WAIT_BT_CMD:
        if(current_time - previous_cmd_time > pdMS_TO_TICKS(cmd_time_threshold) ){
            // Check bluetooth cmd is available
            if(bluetooth_cmd_get(&current_cmd)){
                // Send uart cmd
                uart_api_reset_buffer();
                esp_timer_start_once(retry_timer, retry_interval * 200);
                uart_api_send_cmd(current_cmd);
                ESP_LOGI(TAG, "uart_cmd_t size: %d" ,  sizeof(cmd_t));
                // Switch to task_state TASK_STATE_WAIT_FOR_UART_RSPs
                task_state = TASK_STATE_WAIT_FOR_UART_RSP;
                
            }   
        }

            break;
        case TASK_STATE_WAIT_FOR_UART_RSP:
            // Check bluetooth cmd is available
            buffer_ptr = uart_api_get_buffer();
            if(uart_api_cmp_with_ptr(current_cmd , buffer_ptr)){
                task_state = TASK_STATE_SUCCESS;
                uart_api_reset_buffer();
            }else if(retry_flag){
                if(retry_current >= max_retry_num - 1){
                    retry_current = 0;
                    task_state = TASK_STATE_FAILED;
                }else{
                    retry_current ++;
                    task_state = TASK_STATE_RETRY;
                }
                retry_flag = false;
            }
            break;
        case TASK_STATE_RETRY:
            // Send uart cmd
            ESP_LOGI(TAG, "TASK_STATE_RETRY");
            esp_timer_start_once(retry_timer, retry_interval * 1000);
            uart_api_send_cmd(current_cmd);
            // Switch to task_state TASK_STATE_WAIT_FOR_UART_RSPs
            task_state = TASK_STATE_WAIT_FOR_UART_RSP;
            break;
        case TASK_STATE_SUCCESS:
            ESP_LOGI(TAG , "Bluetooth Command Success: %s \n" , 
                        bluetooth_message_create(current_cmd.device , 
                                                    current_cmd .value));
            retry_current = 0;
            previous_cmd_time = current_time;
            task_state = TASK_STATE_WAIT_BT_CMD;
            break;
        case TASK_STATE_FAILED:
            ESP_LOGI(TAG , "Bluetooth Command failed: %s \n" , 
                            bluetooth_message_create(current_cmd.device , 
                                                        current_cmd .value));
            retry_current = 0;
            previous_cmd_time = current_time;
            task_state = TASK_STATE_WAIT_BT_CMD;
            break;
        default:
            break;
        }
}


static void retry_timer_cb(void * arg){
    bool* tem_retry_flag = (bool*)arg;
    *tem_retry_flag = true;
}