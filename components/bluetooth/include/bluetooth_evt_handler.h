#ifndef __BLUETOOTH_HANDLER_H__
#define __BLUETOOTH_HANDLER_H__

#include "stdint.h"
#include "stdbool.h"
#include "cmd.h"

void bluetooth_evt_handler_init();

int bluetooth_cmd_get(cmd_t*);
void  bluetooth_cmd_add(cmd_t bt_cmd);

char * bluetooth_message_create(uint16_t bt_device_id , uint8_t bt_value);
int bluetooth_messag_parse(char *message , uint8_t* bt_device_id, uint8_t* bt_value);

#endif