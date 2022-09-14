#include "stdint.h"
#include "stdbool.h"
#include "cmd.h"


void uart_api_init();
void uart_api_loop();                      // Run in supper loop for parse rsp
void uart_api_send_cmd(cmd_t);   
int uart_api_cmp_with_ptr(cmd_t , uint8_t* ptr);      
uint8_t * uart_api_get_buffer();
void uart_api_reset_buffer();







