#include <stdio.h>
#include "button.h"

#include <driver/gpio.h>
#include <esp_timer.h>

typedef struct {
    uint64_t pin_num;
    uint8_t state;
    uint8_t pressed;
} ButtonHandle_t;

static gpio_config_t button_config = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
};

static ButtonHandle_t button_table[] = {
    [BUTTON_LEFT_TABLE] = { GPIO_NUM_18, false, false },
    [BUTTON_RIGHT_TABLE] = { GPIO_NUM_19, false, false }
};
uint8_t button_nb = sizeof(button_table)/sizeof(button_table[0]);

static uint32_t button_timer_cnt = 0;
static bool button_timer_flag = true;

static void button_timer_callback();


static esp_timer_handle_t button_timer;
static esp_timer_create_args_t button_timer_args = {
            .callback = &button_timer_callback,
};


void button_init(){
    uint64_t button_mask = 0;
    for (size_t i = 0; i < button_nb; i++)
    {
        button_mask |= 1ULL << button_table[i].pin_num;
    }
    button_config.pin_bit_mask = button_mask;
    gpio_config(&button_config);
    esp_timer_create(&button_timer_args, &button_timer);
    esp_timer_start_periodic(button_timer, 1000);
}

void button_run(){
    if(button_timer_flag){
        button_timer_flag = false;
        button_timer_cnt = BUTTON_DEBOUNCING_TIME;
        uint32_t mask_value = gpio_input_get();
        for (size_t i = 0; i < button_nb; i++)
        {
            uint8_t current_state = 1ULL & ( mask_value >> button_table[i].pin_num);
            if(button_table[i].state == current_state){
                button_table[i].pressed = !button_table[i].state;   // Low level is pressed
            }
            button_table[i].state = current_state;
        }
    }
}

bool button_is_pressed(uint8_t button_index){
    return button_table[button_index].pressed;
}

uint8_t button_get_pressed(){
    for (size_t i = 0; i < button_nb; i++)
    {
        if(button_table[i].pressed){
            return i;
        }
    }
    return BUTTON_UNKNOWN;
}

bool button_clear(uint8_t button_index){
    return true;
}

static void button_timer_callback(){
    if(button_timer_cnt > 0){
        button_timer_cnt--;
        if(button_timer_cnt == 0){
            button_timer_flag = true;
        }
    }
}