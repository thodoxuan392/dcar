#include "stdint.h"
#include "stdbool.h"

/* Device ID define */

typedef enum{
    LEFT_TABLE=0x01F0,
    RIGHT_TABLE=0x01F1,
    CONTROL_PANEL=0x01F2,

    //Board 2
    CURTAIN_REAR_LEFT=0x02F0,
    CURTAIN_REAR_CENTER=0x02F1,
    CURTAIN_REAR_RIGHT=0x02F2,

    //Board 3
    CURTAIN_FRONT_LEFT=0x03F0,
    CURTAIN_FRONT_RIGHT=0x03F1,
    TV=0x03F2,

    //Board 4
    LEFT_SEAT_A=0x04F0,
    LEFT_SEAT_B=0x04F1,
    LIGHT_4X=0x04F2,

    //Board 5
    RIGHT_SEAT_A=0x05F0,
    RIGHT_SEAT_B=0x05F1,
    LIGHT_SIDE=0x05F2,

    //Board 6
    LIGHT_CEILING=0x06F0,
    LIGHT_DRAWERS=0x06F1,
    NOT_USE=0x06F2
}device_id_t;




/* Value Define */

typedef enum{
    OFF=0x00,
    MAX_SPEED=0xFF,
    INVERSE_MAX_SPEED=0x7F
}value_t;

/*
* Data structure
*/

// Command Structure 
typedef struct {
    uint16_t device_id;
    uint8_t value;
}uart_cmd_t;


void uart_api_init();
void uart_api_loop();                      // Run in supper loop for parse rsp
void uart_api_send_cmd(uart_cmd_t);         
uint8_t * uart_api_get_buffer();
void uart_api_reset_buffer();







