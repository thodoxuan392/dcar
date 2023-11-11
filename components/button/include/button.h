#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include <stdbool.h>

#define BUTTON_DEBOUNCING_TIME    20 // milliseconds

enum {
    BUTTON_LEFT_TABLE = 0,
    BUTTON_RIGHT_TABLE,
    BUTTON_UNKNOWN
};


void button_init();
void button_run();
bool button_is_pressed(uint8_t button_index);
uint8_t button_get_pressed();
bool button_clear(uint8_t button_index);


#endif // BUTTON_H