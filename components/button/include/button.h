#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include <stdbool.h>

#include <cmd.h>

#define BUTTON_DEBOUNCING_TIME    10 // milliseconds


void button_init();
void button_run();
bool button_is_pressed(uint8_t button_index);
uint8_t button_get_pressed();
bool button_cmd_get(cmd_t *cmd);
void button_clear(uint8_t button_index);


#endif // BUTTON_H