#ifndef __BLUETOOTH_UART_CONVERTER_H__
#define __BLUETOOTH_UART_CONVERTER_H__

#include "uart_api.h"

typedef struct {
    uint8_t bluetooth_device_id;
    device_id_t uart_device_id;
}device_mapping_t;

typedef struct {
    uint8_t bluetooth_value;
    value_t uart_value;
}value_mapping_t;


int bluetooth_to_uart_device_id(uint8_t bluetooth_device_id , device_id_t* uart_device_id);
int uart_to_bluetooth_device_id(device_id_t uart_device_id, uint8_t* bluetooth_device_id );

int bluetooth_to_uart_value(uint8_t bluetooth_value , value_t* uart_value);
int uart_to_bluetooth_value(value_t uart_value, uint8_t* bluetooth_value );

#endif