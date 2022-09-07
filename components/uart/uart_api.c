#include "uart_api.h"
#include "uart_core.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"


static uint8_t uart_core_buffer[255] = {0};
static uint16_t uart_buffer_index = 0;


void uart_api_init(){
    uart_core_init();
}

void uart_api_loop(){
    // Get data from uart core -> push to uart_core_buffer
    esp_err_t ret;
    uint16_t buffered_len;
    uart_core_buffered_data_len(&buffered_len);
    if(buffered_len > 0){
        uint16_t data_len;
        ret = uart_core_read_bytes(&uart_core_buffer[uart_buffer_index], 255 - uart_buffer_index, &data_len);
        if(ret > 0){
            uart_buffer_index += data_len;
        }
    }
}

void uart_api_send_cmd(uart_cmd_t uart_cmd){
    uint8_t device_id_high = uart_cmd.device_id >> 8;
    uint8_t device_id_low = uart_cmd.device_id & 0xFF;
    uint8_t value  = uart_cmd.value;
    uart_core_write_bytes(&device_id_high, 1);
    uart_core_write_bytes(&device_id_low, 1);
    uart_core_write_bytes(&value, 1);
} 

uint8_t * uart_api_get_buffer(){
    return uart_core_buffer;
}

void uart_api_reset_buffer(){
    uart_buffer_index = 0;
    memset(uart_core_buffer , 0, 255);
}