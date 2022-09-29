#include <stdio.h>
#include "uart_core.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_err.h"

static const char * TAG = "uart";

static const uint16_t buffer_size  = 1024;
static const uart_port_t uart_num = UART_NUM_2;
static const uint8_t rx_pin = GPIO_NUM_16;
static const uint8_t tx_pin = GPIO_NUM_17;

static const uart_config_t uart_config = {
    .baud_rate = 9600,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 122,
};



void uart_core_init(){
    // Config UART
    ESP_ERROR_CHECK(uart_driver_install(uart_num, buffer_size * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    return;
}
esp_err_t uart_core_buffered_data_len(uint16_t * buffered_len){
    esp_err_t ret;
    ret = uart_get_buffered_data_len(uart_num ,buffered_len);
    if(ret != ESP_OK){
        ESP_LOGE(TAG, "uart_get_buffered_data_len failed 0x%03x" , ret);
        return ret;
    }
    return ESP_OK;
}

esp_err_t uart_core_read_bytes(uint8_t * data, uint16_t max_data_size,  uint16_t * data_len){
    // Get Data Length
    esp_err_t ret;
    uint16_t uart_len;
    ret = uart_get_buffered_data_len(uart_num ,&uart_len);
    if(ret != ESP_OK){
        ESP_LOGE(TAG, "uart_get_buffered_data_len failed 0x%03x" , ret);
        return ret;
    }

    if(uart_len > max_data_size){
        ESP_LOGE(TAG, "Uart size over max buffer\n");
        return ret;
    }
    * data_len = uart_read_bytes(uart_num, data, max_data_size , 0);
    if(* data_len == -1){
        ESP_LOGE(TAG, "uart_read_bytes failed");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t uart_core_write_bytes(uint8_t * data, uint16_t data_len ){

    int rc = uart_write_bytes(uart_num, (uint8_t*)data, data_len);
    if(rc == -1){
        ESP_LOGE(TAG, "Uart write failed\n");
        return ESP_FAIL;
    }
    return ESP_OK;
}