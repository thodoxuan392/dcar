#include "task.h"
#include "stdio.h"
#include "stdint.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "bluetooth_evt_handler.h"
#include "button.h"
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


/**
 * @brief Dcar component state
 * 
 */
enum {
    CURTAIN_STATE_UP,
    CURTAIN_STATE_DOWN
};

enum {
    LED_STATE_ON,
    LED_STATE_OFF
};

enum {
    LEFT_SEAT_STATE_UP,
    LEFT_SEAT_STATE_DOWN,
    LEFT_SEAT_STATE_STOP
};

enum {
    RIGHT_SEAT_STATE_UP,
    RIGHT_SEAT_STATE_DOWN,
    RIGHT_SEAT_STATE_STOP
}; 

enum {
    LIGHT_TOP_STATE_ON,
    LIGHT_TOP_STATE_OFF,
};

enum {
    LIGHT_CEILING_STATE_ON,
    LIGHT_CEILING_STATE_OFF,
};

enum {
    LIGHT_DRAWER_STATE_ON,
    LIGHT_DRAWER_STATE_OFF,
};

enum {
    LIGHT_SIDE_STATE_ON,
    LIGHT_SIDE_STATE_OFF,
};

enum {
    TABLE_LEFT_STATE_ON,
    TABLE_LEFT_STATE_OFF,
    TABLE_LEFT_STATE_STOP,
};

enum {
    TABLE_RIGHT_STATE_ON,
    TABLE_RIGHT_STATE_OFF,
    TABLE_RIGHT_STATE_STOP,
};

enum {
    TIVI_STATE_ON,
    TIVI_STATE_OFF,
};

enum {
    CURTAIN_FRONT_LEFT_STATE_UP,
    CURTAIN_FRONT_LEFT_STATE_DOWN,
    CURTAIN_FRONT_LEFT_STATE_STOP,
};

enum {
    CURTAIN_FRONT_RIGHT_STATE_UP,
    CURTAIN_FRONT_RIGHT_STATE_DOWN,
    CURTAIN_FRONT_RIGHT_STATE_STOP,
};


enum {
    CURTAIN_REAR_LEFT_STATE_UP,
    CURTAIN_REAR_LEFT_STATE_DOWN,
    CURTAIN_REAR_LEFT_STATE_STOP,
};

enum {
    CURTAIN_REAR_RIGHT_STATE_UP,
    CURTAIN_REAR_RIGHT_STATE_DOWN,
    CURTAIN_REAR_RIGHT_STATE_STOP,
};

enum {
    CURTAIN_REAR_CENTER_STATE_UP,
    CURTAIN_REAR_CENTER_STATE_DOWN,
    CURTAIN_REAR_CENTER_STATE_STOP
};
typedef struct {
    uint8_t curtain;
    uint8_t left_seat;
    uint8_t right_seat;
    uint8_t light_top;
    uint8_t light_ceiling;
    uint8_t light_drawer;
    uint8_t light_side;
    uint8_t table_left;
    uint8_t table_right;
    uint8_t tv;
    uint8_t curtain_front_left;
    uint8_t curtain_front_right;
    uint8_t curtain_rear_left;
    uint8_t curtain_rear_right;
    uint8_t curtain_rear_center;
}dcar_handle_t;

static dcar_handle_t dcar_state;
static void task_handle_dcar_state_by_cmd(cmd_t *cmd);
static void task_build_command_from_button(uint8_t button_index, cmd_t *cmd);
void task_init(){
    // Init timer for countdown to retry
    esp_timer_create(&retry_timer_args, &retry_timer);
}


void task_run(){
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
            uint8_t button_index = button_get_pressed();
            if(button_index != BUTTON_UNKNOWN){
                task_build_command_from_button(button_index, &current_cmd);
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
            task_handle_dcar_state_by_cmd(&current_cmd);
            retry_current = 0;
            previous_cmd_time = current_time;
            task_state = TASK_STATE_WAIT_BT_CMD;
            break;
        case TASK_STATE_FAILED:
            ESP_LOGE(TAG , "Bluetooth Command failed: %s \n" , 
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

static void task_build_command_from_button(uint8_t button_index, cmd_t *cmd){
    switch (button_index)
    {
    case BUTTON_LEFT_TABLE:
        if(dcar_state.table_left == TABLE_LEFT_STATE_OFF){
            // Turn on
            cmd->value = MAX_SPEED;
        }else if(dcar_state.table_left == TABLE_LEFT_STATE_ON){
            // Stop
            cmd->value = INVERSE_MAX_SPEED;
        } else {
            // Off
            cmd->value = OFF;
        }
        cmd->device = LEFT_TABLE;
        break;
    case BUTTON_RIGHT_TABLE:
        if(dcar_state.table_right == TABLE_LEFT_STATE_OFF){
            // Turn on
            cmd->value = MAX_SPEED;
        }else if(dcar_state.table_right == TABLE_LEFT_STATE_ON){
            // Stop
            cmd->value = INVERSE_MAX_SPEED;
        } else {
            // Off
            cmd->value = OFF;
        }
        cmd->device = RIGHT_TABLE;
        break;
    default:
        break;
    }
}

static void task_handle_dcar_state_by_cmd(cmd_t *cmd){
    switch (cmd->device)
    {
    case LEFT_TABLE:
        if(cmd->value == MAX_SPEED){
            dcar_state.table_left = TABLE_LEFT_STATE_ON;
        } else if(cmd->value == INVERSE_MAX_SPEED){
            dcar_state.table_left = TABLE_LEFT_STATE_STOP;
        } else if(cmd->value == OFF){
            dcar_state.table_left = TABLE_LEFT_STATE_OFF;
        }
        break;
    case RIGHT_TABLE:
        if(cmd->value == MAX_SPEED){
            dcar_state.table_left = TABLE_RIGHT_STATE_ON;
        } else if(cmd->value == INVERSE_MAX_SPEED){
            dcar_state.table_left = TABLE_RIGHT_STATE_STOP;
        } else if(cmd->value == OFF){
            dcar_state.table_left = TABLE_RIGHT_STATE_OFF;
        }
        break;

    case CURTAIN_REAR_LEFT:
        if(cmd->value == MAX_SPEED){
            dcar_state.curtain_rear_left = CURTAIN_REAR_LEFT_STATE_UP;
        } else if(cmd->value == INVERSE_MAX_SPEED){
            dcar_state.curtain_rear_left = CURTAIN_REAR_LEFT_STATE_STOP;
        } else if(cmd->value == OFF){
            dcar_state.curtain_rear_left = CURTAIN_REAR_LEFT_STATE_DOWN;
        }
        break;

    case CURTAIN_REAR_RIGHT:
        if(cmd->value == MAX_SPEED){
            dcar_state.curtain_rear_right = CURTAIN_REAR_RIGHT_STATE_UP;
        } else if(cmd->value == INVERSE_MAX_SPEED){
            dcar_state.curtain_rear_right = CURTAIN_REAR_RIGHT_STATE_STOP;
        } else if(cmd->value == OFF){
            dcar_state.curtain_rear_right = CURTAIN_REAR_RIGHT_STATE_DOWN;
        }
        break;

    case CURTAIN_REAR_CENTER:
        if(cmd->value == MAX_SPEED){
            dcar_state.curtain_rear_center = CURTAIN_REAR_CENTER_STATE_UP;
        } else if(cmd->value == INVERSE_MAX_SPEED){
            dcar_state.curtain_rear_center = CURTAIN_REAR_CENTER_STATE_STOP;
        } else if(cmd->value == OFF){
            dcar_state.curtain_rear_center = CURTAIN_REAR_CENTER_STATE_DOWN;
        }
        break;

    case CURTAIN_FRONT_LEFT:
        if(cmd->value == MAX_SPEED){
            dcar_state.curtain_front_left = CURTAIN_FRONT_LEFT_STATE_UP;
        } else if(cmd->value == INVERSE_MAX_SPEED){
            dcar_state.curtain_front_left = CURTAIN_FRONT_LEFT_STATE_STOP;
        } else if(cmd->value == OFF){
            dcar_state.curtain_rear_center = CURTAIN_FRONT_LEFT_STATE_DOWN;
        }
        break;
    case CURTAIN_FRONT_RIGHT:
        if(cmd->value == MAX_SPEED){
            dcar_state.curtain_front_right = CURTAIN_FRONT_RIGHT_STATE_UP;
        } else if(cmd->value == INVERSE_MAX_SPEED){
            dcar_state.curtain_front_right = CURTAIN_FRONT_RIGHT_STATE_STOP;
        } else if(cmd->value == OFF){
            dcar_state.curtain_front_right = CURTAIN_FRONT_RIGHT_STATE_DOWN;
        }
        break;
    case TV:
        if(cmd->value == MAX_SPEED){
            dcar_state.tv = TIVI_STATE_ON;
        } else if(cmd->value == OFF){
            dcar_state.tv = TIVI_STATE_OFF;
        }
        break;
    case LEFT_SEAT_A:
        if(cmd->value == MAX_SPEED){
            dcar_state.left_seat = LEFT_SEAT_STATE_UP;
        } else if(cmd->value == INVERSE_MAX_SPEED){
            dcar_state.left_seat = LEFT_SEAT_STATE_STOP;
        }
        break;
    case LEFT_SEAT_B:
        if(cmd->value == MAX_SPEED){
            dcar_state.left_seat = LEFT_SEAT_STATE_DOWN;
        } else if(cmd->value == INVERSE_MAX_SPEED){
            dcar_state.left_seat = LEFT_SEAT_STATE_STOP;
        }
        break;
    case LIGHT_4X:
        if(cmd->value == MAX_SPEED){
            dcar_state.light_top = LIGHT_TOP_STATE_ON;
        } else if(cmd->value == OFF){
            dcar_state.light_top = LIGHT_TOP_STATE_OFF;
        }
        break;
    case RIGHT_SEAT_A:
        if(cmd->value == MAX_SPEED){
            dcar_state.right_seat = RIGHT_SEAT_STATE_UP;
        } else if(cmd->value == INVERSE_MAX_SPEED){
            dcar_state.right_seat = RIGHT_SEAT_STATE_STOP;
        }
        break;
    case RIGHT_SEAT_B:
        if(cmd->value == MAX_SPEED){
            dcar_state.right_seat = RIGHT_SEAT_STATE_DOWN;
        } else if(cmd->value == INVERSE_MAX_SPEED){
            dcar_state.right_seat = RIGHT_SEAT_STATE_STOP;
        }
        break;
    case LIGHT_SIDE:
        if(cmd->value == MAX_SPEED){
            dcar_state.light_side = LIGHT_SIDE_STATE_ON;
        } else if(cmd->value == OFF){
            dcar_state.light_side = LIGHT_SIDE_STATE_OFF;
        }
        break;
    case LIGHT_CEILING:
        if(cmd->value == MAX_SPEED){
            dcar_state.light_ceiling = LIGHT_CEILING_STATE_ON;
        } else if(cmd->value == OFF){
            dcar_state.light_ceiling = LIGHT_CEILING_STATE_OFF;
        }
        break;
    case LIGHT_DRAWERS:
        if(cmd->value == MAX_SPEED){
            dcar_state.light_drawer = LIGHT_DRAWER_STATE_ON;
        } else if(cmd->value == OFF){
            dcar_state.light_drawer = LIGHT_DRAWER_STATE_OFF;
        }
        break;
    default:
        break;
    }
}

static void retry_timer_cb(void * arg){
    bool* tem_retry_flag = (bool*)arg;
    *tem_retry_flag = true;
}