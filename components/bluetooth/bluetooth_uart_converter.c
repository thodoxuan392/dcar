#include "bluetooth_uart_converter.h"
#include "stdio.h"
#include "stdint.h"


static device_mapping_t device_mappping_entry[] = {
    { 0 , LEFT_TABLE },
    { 1 , RIGHT_TABLE },
    { 2 , CONTROL_PANEL },
    
    { 3 , CURTAIN_REAR_LEFT },
    { 4 , CURTAIN_REAR_CENTER },
    { 5 , CURTAIN_REAR_RIGHT },
    
    { 6 , CURTAIN_FRONT_LEFT },
    { 7 , CURTAIN_FRONT_RIGHT },
    { 8 , TV },

    { 9 , LEFT_SEAT_A },
    { 10 , LEFT_SEAT_B },
    { 11 , LIGHT_4X },

    { 12 , RIGHT_SEAT_A },
    { 13 , RIGHT_SEAT_B },
    { 14 , LIGHT_SIDE },

    { 15 , LIGHT_CEILING },
    { 16 , LIGHT_DRAWERS },
    { 17 , NOT_USE },
};

static value_mapping_t  value_mapping_entry[] = {
    { 0 , OFF},
    { 1 , MAX_SPEED},
    { 2 , INVERSE_MAX_SPEED},
};

int bluetooth_to_uart_device_id(uint8_t bluetooth_device_id , device_id_t* uart_device_id){
    uint8_t nb_device_id_entry = sizeof(device_mappping_entry) / sizeof(device_mapping_t);
    for (size_t i = 0; i < nb_device_id_entry; i++)
    {
        if(device_mappping_entry[i].bluetooth_device_id == bluetooth_device_id){
           * uart_device_id = device_mappping_entry[i].uart_device_id;
           return 0;
        }
    }
    return -1;
}

int uart_to_bluetooth_device_id(device_id_t uart_device_id, uint8_t* bluetooth_device_id ){
    uint8_t nb_device_id_entry = sizeof(device_mappping_entry) / sizeof(device_mapping_t);
    for (size_t i = 0; i < nb_device_id_entry; i++)
    {
        if(device_mappping_entry[i].uart_device_id == uart_device_id){
           * bluetooth_device_id = device_mappping_entry[i].bluetooth_device_id;
           return 0;
        }
    }
    return -1;
}

int bluetooth_to_uart_value(uint8_t bluetooth_value , value_t* uart_value){
    uint8_t nb_value_entry = sizeof(value_mapping_entry) / sizeof(value_mapping_t);
    for (size_t i = 0; i < nb_value_entry; i++)
    {
        if(value_mapping_entry[i].bluetooth_value == bluetooth_value){
           * uart_value = value_mapping_entry[i].uart_value;
           return 0;
        }
    }
    return -1;
}

int uart_to_bluetooth_value(value_t uart_value, uint8_t* bluetooth_value ){
    uint8_t nb_value_entry = sizeof(value_mapping_entry) / sizeof(value_mapping_t);
    for (size_t i = 0; i < nb_value_entry; i++)
    {
        if(value_mapping_entry[i].uart_value == uart_value){
           * bluetooth_value = value_mapping_entry[i].bluetooth_value;
           return 0;
        }
    }
    return -1;
}