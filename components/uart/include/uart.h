#include "stdint.h"


void app_uart_init();
uint8_t app_uart_is_available();
void app_uart_read_bytes(uint8_t * data, uint16_t max_data_size,  uint16_t * data_len);
void app_uart_write_bytes(uint8_t * data, uint16_t data_len );
