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
    char msg[200] = {0};
    esp_ble_gatts_cb_param_t *param = data;
    char * ble_msg =  strndup((char*)param->write.value ,param->write.len);
    size_t msg_len = param->write.len;
    ESP_LOGI(TAG, "%s\n" , ble_msg);
    cmd_t cmd;
    if(ble_msg[0] != '!' || ble_msg[msg_len-1] != '#'){
        ESP_LOGE(TAG , "Bluetooth Command Wrong Format, It should be !{CMD}#");
        return;
    }
    memcpy(msg , ble_msg + 1, msg_len - 2);
    if(!strcmp(msg , "DCAR_CURTAIN_UP")){
        // Max Speed
        cmd.value = MAX_SPEED;

        // CURTAIN_FRONT_LEFT
        cmd.device = CURTAIN_FRONT_LEFT;
        bluetooth_cmd_add(cmd);

        // CURTAIN_FRONT_RIGHT
        cmd.device = CURTAIN_FRONT_RIGHT;
        bluetooth_cmd_add(cmd);

        // CURTAIN_REAR_CENTER
        cmd.device = CURTAIN_REAR_CENTER;
        bluetooth_cmd_add(cmd);

        // CURTAIN_REAR_LEFT
        cmd.device = CURTAIN_REAR_LEFT;
        bluetooth_cmd_add(cmd);

        // CURTAIN_REAR_LEFT
        cmd.device = CURTAIN_REAR_RIGHT;
        bluetooth_cmd_add(cmd);
        
    }
    else if(!strcmp(msg , "DCAR_CURTAIN_DOWN")){
        cmd.value = OFF;

        // CURTAIN_FRONT_LEFT
        cmd.device = CURTAIN_FRONT_LEFT;
        bluetooth_cmd_add(cmd);

        // CURTAIN_FRONT_RIGHT
        cmd.device = CURTAIN_FRONT_RIGHT;
        bluetooth_cmd_add(cmd);

        // CURTAIN_REAR_CENTER
        cmd.device = CURTAIN_REAR_CENTER;
        bluetooth_cmd_add(cmd);

        // CURTAIN_REAR_LEFT
        cmd.device = CURTAIN_REAR_LEFT;
        bluetooth_cmd_add(cmd);

        // CURTAIN_REAR_LEFT
        cmd.device = CURTAIN_REAR_RIGHT;
        bluetooth_cmd_add(cmd);
    }
    else if(!strcmp(msg , "DCAR_LED_ON")){
        cmd.value = MAX_SPEED;

        cmd.device = LIGHT_4X;
        bluetooth_cmd_add(cmd);

        cmd.device = LIGHT_CEILING;
        bluetooth_cmd_add(cmd);

        cmd.device = LIGHT_DRAWERS;
        bluetooth_cmd_add(cmd);

        cmd.device = LIGHT_SIDE;
        bluetooth_cmd_add(cmd);

    }
    else if(!strcmp(msg , "DCAR_LED_OFF")){
        cmd.value = OFF;

        cmd.device = LIGHT_4X;
        bluetooth_cmd_add(cmd);

        cmd.device = LIGHT_CEILING;
        bluetooth_cmd_add(cmd);

        cmd.device = LIGHT_DRAWERS;
        bluetooth_cmd_add(cmd);

        cmd.device = LIGHT_SIDE;
        bluetooth_cmd_add(cmd);

    }
    else if(!strcmp(msg , "LEFT_SEAT_UP") || !strcmp(msg , "DCAR_LEFT_SEAT_ON")){
        cmd.value = MAX_SPEED;

        cmd.device = LEFT_SEAT_A;
        bluetooth_cmd_add(cmd);
    }
    else if(!strcmp(msg , "LEFT_SEAT_DOWN") || !strcmp(msg , "DCAR_LEFT_SEAT_OFF")){
        cmd.value = MAX_SPEED;

        cmd.device = LEFT_SEAT_B;
        bluetooth_cmd_add(cmd);
    }
    else if(!strcmp(msg , "RIGHT_SEAT_UP") || !strcmp(msg , "DCAR_RIGHT_SEAT_ON")){
        cmd.value = MAX_SPEED;

        cmd.device = RIGHT_SEAT_A;
        bluetooth_cmd_add(cmd);
    }
    else if(!strcmp(msg , "LEFT_SEAT_DOWN") || !strcmp(msg , "DCAR_LEFT_SEAT_OFF")){
        cmd.value = MAX_SPEED;

        cmd.device = RIGHT_SEAT_B;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "LEFT_SEAT_STOP")){
        cmd.value = INVERSE_MAX_SPEED;

        cmd.device = LEFT_SEAT_A;
        bluetooth_cmd_add(cmd);

        cmd.device = LEFT_SEAT_B;
        bluetooth_cmd_add(cmd);
    }
    else if(!strcmp(msg , "RIGHT_SEAT_STOP")){
        cmd.value = INVERSE_MAX_SPEED;

        cmd.device = RIGHT_SEAT_A;
        bluetooth_cmd_add(cmd);

        cmd.device = RIGHT_SEAT_B;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "LIGHT_TOP_ON")){
        cmd.value = MAX_SPEED;

        cmd.device = LIGHT_4X;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "LIGHT_TOP_OFF")){
        cmd.value = OFF;

        cmd.device = LIGHT_4X;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "LIGHT_CEILLING_ON")){
        cmd.value = MAX_SPEED;

        cmd.device = LIGHT_CEILING;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "LIGHT_CEILLING_OFF")){
        cmd.value = OFF;

        cmd.device = LIGHT_CEILING;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "LIGHT_DRAWERS_ON")){
        cmd.value = MAX_SPEED;

        cmd.device = LIGHT_DRAWERS;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "LIGHT_DRAWERS_OFF")){
        cmd.value = OFF;

        cmd.device = LIGHT_DRAWERS;
        bluetooth_cmd_add(cmd);
    }

        else if(!strcmp(msg , "LIGHT_SIDE_ON")){
        cmd.value = MAX_SPEED;

        cmd.device = LIGHT_SIDE;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "LIGHT_SIDE_OFF")){
        cmd.value = OFF;

        cmd.device = LIGHT_SIDE;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "TABLE_LEFT_ON")){
        cmd.value = MAX_SPEED;

        cmd.device = LEFT_TABLE;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "TABLE_LEFT_OFF")){
        cmd.value = OFF;

        cmd.device = LEFT_TABLE;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "TABLE_LEFT_STOP")){
        cmd.value = INVERSE_MAX_SPEED;

        cmd.device = LEFT_TABLE;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "TABLE_RIGHT_ON")){
        cmd.value = MAX_SPEED;

        cmd.device = RIGHT_TABLE;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "TABLE_RIGHT_OFF")){
        cmd.value = OFF;

        cmd.device = RIGHT_TABLE;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "TABLE_RIGHT_STOP")){
        cmd.value = INVERSE_MAX_SPEED;

        cmd.device = RIGHT_TABLE;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "TIVI_ON")){
        cmd.value = MAX_SPEED;

        cmd.device = TV;
        bluetooth_cmd_add(cmd);
    }
    else if(!strcmp(msg , "TIVI_OFF")){
        cmd.value = OFF;

        cmd.device = TV;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "CURTAIN_FRONT_LEFT_UP")){
        cmd.value = MAX_SPEED;

        cmd.device = CURTAIN_FRONT_LEFT;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "CURTAIN_FRONT_LEFT_DOWN")){
        cmd.value = OFF;

        cmd.device = CURTAIN_FRONT_LEFT;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "CURTAIN_FRONT_LEFT_STOP")){
        cmd.value = INVERSE_MAX_SPEED;

        cmd.device = CURTAIN_FRONT_LEFT;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "CURTAIN_FRONT_RIGHT_UP")){
        cmd.value = MAX_SPEED;

        cmd.device = CURTAIN_FRONT_RIGHT;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "CURTAIN_FRONT_RIGHT_DOWN")){
        cmd.value = OFF;

        cmd.device = CURTAIN_FRONT_RIGHT;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "CURTAIN_FRONT_RIGHT_STOP")){
        cmd.value = INVERSE_MAX_SPEED;

        cmd.device = CURTAIN_FRONT_RIGHT;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "CURTAIN_REAR_LEFT_UP")){
        cmd.value = MAX_SPEED;

        cmd.device = CURTAIN_REAR_LEFT;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "CURTAIN_REAR_LEFT_DOWN")){
        cmd.value = OFF;

        cmd.device = CURTAIN_REAR_LEFT;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "CURTAIN_REAR_LEFT_STOP")){
        cmd.value = INVERSE_MAX_SPEED;

        cmd.device = CURTAIN_REAR_LEFT;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "CURTAIN_REAR_RIGHT_UP")){
        cmd.value = MAX_SPEED;

        cmd.device = CURTAIN_REAR_RIGHT;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "CURTAIN_REAR_RIGHT_DOWN")){
        cmd.value = OFF;

        cmd.device = CURTAIN_REAR_RIGHT;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "CURTAIN_REAR_RIGHT_STOP")){
        cmd.value = INVERSE_MAX_SPEED;

        cmd.device = CURTAIN_REAR_RIGHT;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "CURTAIN_REAR_CENTER_UP")){
        cmd.value = MAX_SPEED;

        cmd.device = CURTAIN_REAR_CENTER;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "CURTAIN_REAR_CENTER_DOWN")){
        cmd.value = OFF;

        cmd.device = CURTAIN_REAR_CENTER;
        bluetooth_cmd_add(cmd);
    }

    else if(!strcmp(msg , "CURTAIN_REAR_CENTER_STOP")){
        cmd.value = INVERSE_MAX_SPEED;

        cmd.device = CURTAIN_REAR_CENTER;
        bluetooth_cmd_add(cmd);
    }


    else{
        ESP_LOGE(TAG, "Bluetooth Command Not Found\n");
        return;
    }
}


/* Public Function */

void bluetooth_evt_handler_init(){
    // Init Queue Bluetooth Message
    bt_cmd_queue = xQueueCreate(bt_cmd_queue_size , sizeof(cmd_t));

}


int bluetooth_cmd_get(cmd_t *cmd_t){
    return xQueueReceive(bt_cmd_queue , cmd_t , 10);
}

void bluetooth_cmd_add(cmd_t cmd){
    cmd_t temp_cmd = cmd;
    xQueueSend(bt_cmd_queue, &temp_cmd ,200);
    ESP_LOGI(TAG,"bluetooth_cmd_add OK");
}


char * bluetooth_message_create(uint16_t device , uint8_t value){
    sprintf(cmd_str, "Device: 0x%04x, Value: 0x%02x" , device, value);
    return cmd_str;
}