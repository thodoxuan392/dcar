#include "stdint.h"

// 
#define START_BYTE          '!'
#define END_BYTE            '#'

// Header Define
#define uart_msg_board_info_id                      (0x01)<<0
#define uart_msg_open_music_id                      (0x01)<<1

/*
* Data structure
*/


// Msg Board Info
typedef struct {
    
}uart_msg_board_info_t;

// Msg Open Music
typedef struct {
    
}uart_msg_open_music_t;


// Message Structure 
typedef struct {
    uint8_t header;
    typedef union
    {
        uart_msg_board_info_t uart_msg_board_info;
        uart_msg_open_music_t uart_msg_open_music;
    }data;
}uart_msg_t;

