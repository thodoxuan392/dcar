#include <stdio.h>
#include "uart.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_err.h"

static const char * TAG = "uart";

static const uart_port_t uart_num = UART_NUM_1;
static const uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 122,
};


void app_uart_init(){
    // Config UART
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    return;
}
uint8_t app_uart_is_available(){
    uint16_t uart_len;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num ,&uart_len));
    if(uart_len > 0){
        return 1;
    }
    return 0;
}

void app_uart_read_bytes(uint8_t * data, uint16_t max_data_size,  uint16_t * data_len){
    // Get Data Length
    uint16_t uart_len;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num ,&uart_len));
    if(uart_len > max_data_size){
        ESP_LOGE(TAG, "Uart size over max buffer\n");
        return;
    }
    ESP_ERROR_CHECK(uart_read_bytes(uart_num, data, data_len , 100));
}
void app_uart_write_bytes(uint8_t * data, uint16_t data_len ){
    ESP_ERROR_CHECK(uart_write_bytes(uart_num, data, data_len));
}