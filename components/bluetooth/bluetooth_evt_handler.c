#include "bluetooth_evt_handler.h"
#include "string.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define CMD             "CMD"
#define START_BYPE      '!'
#define STOP_BYTE       '#'
#define SEPERATE_BYTE   '_'


/* Private Variable */
static char cmd_str[200];

static const char * TAG = "bluetooth_evt_handler";
// Bluetooth Command Queue
static QueueHandle_t bt_cmd_queue;
static const uint16_t bt_cmd_queue_size = 100;


/* Private Function*/

void bluetooth_event_write_cb(void * data){
    esp_ble_gatts_cb_param_t *param = data;
    char * ble_msg =  strndup((char*)param->write.value ,param->write.len);
    ESP_LOGI(TAG , "Write cb data: %s\n" , ble_msg);
    uint8_t device_id;
    uint8_t value;
    int rc = bluetooth_messag_parse(ble_msg, &device_id, &value);
    if(rc != 0){
        ESP_LOGE(TAG, "Bluetooth Parse Error\n");
        return;
    }
    bt_cmd_t bt_cmd = {
        .device_id = device_id,
        .value = value
    };
    bluetooth_cmd_add(bt_cmd);
    ESP_LOGI(TAG , "device_id %d , value: %d\n" , device_id, value);
}


/* Public Function */

void bluetooth_evt_handler_init(){
    // Init Queue Bluetooth Message
    bt_cmd_queue = xQueueCreate(bt_cmd_queue_size , sizeof(bt_cmd_t));

}


int bluetooth_cmd_get(bt_cmd_t *bt_cmd){
    return xQueueReceive(bt_cmd_queue , bt_cmd , 10);
}

void bluetooth_cmd_add(bt_cmd_t bt_cmd){
    bt_cmd_t temp = bt_cmd;
    xQueueSend(bt_cmd_queue, &temp ,200);
    ESP_LOGI(TAG,"bluetooth_cmd_add OK");
}


char * bluetooth_message_create(uint8_t bt_device_id , uint8_t bt_value){
    sprintf(cmd_str, "!CMD_%d_%d#" , bt_device_id, bt_value);
    return cmd_str;
}

// Return 0 is Succes, return -1 is false
int bluetooth_messag_parse(char *message , uint8_t* bt_device_id, uint8_t* bt_value){
    size_t message_len = strlen(message);
    /*
        state:
            0: find START BYTE
            1: get bt_device_id
            2: get bt_device_id
            3: get bt_value
    */
    uint8_t state = 0;
    char *cmd = CMD;
    uint8_t device_id = 0;
    uint8_t value = 0;
    for (size_t i = 0; i < message_len; i++)
    {
        /* code */
        switch (state)
        {
        case 0:
            /* code */
            if(message[i] == START_BYPE){
                state = 1;
            }
            break;
        case 1:
            /* code */
            if(message[i] == SEPERATE_BYTE){
                state = 2;
            }else{
                // Check that valid "CMD"
                if(i > strlen(cmd)){
                    ESP_LOGE(TAG, "CMD prefix oversize\n");
                    return -1;
                }
                // Compare cmd from message with "CMD" with less than 1 index
                if(message[i] != cmd[i-1]){
                    // Failed because diff from "CMD"
                    ESP_LOGE(TAG, "CMD note matched\n");
                    return -1;
                }
            }
            break;
        case 2:
            /* code */
            if(message[i] == SEPERATE_BYTE){
                state = 3;
            }else{
                device_id+= message[i] - '0';
            }
            break;
        case 3:
            /* code */
            if(message[i] == STOP_BYTE){
                *bt_device_id = device_id;
                *bt_value = value;
                return 0;
            }else{
                value+= message[i] - '0';
            }
            break;
        default:
            break;
        }
    }
    ESP_LOGE(TAG, "Command %s not found STOP_BYTE '%c'\n" ,message, STOP_BYTE);
    return -1;
}