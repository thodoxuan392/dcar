#include "stdint.h"
#include "esp_err.h"

void uart_core_init();
esp_err_t uart_core_buffered_data_len(uint16_t *);
esp_err_t uart_core_read_bytes(uint8_t * data, uint16_t max_data_size,  uint16_t * data_len);
esp_err_t uart_core_write_bytes(uint8_t * data, uint16_t data_len );
