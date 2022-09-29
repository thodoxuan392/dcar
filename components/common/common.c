#include <stdio.h>
#include "common.h"
#include "esp_log.h"
#include "string.h"

#define BUFFER_SIZE 255

static char buffer[BUFFER_SIZE];
static const char * TAG = __FILE__;


char* str_replace(char* str_input , char * pattern , char * replace){
    
    //Validation data
    if(!pattern || !str_input || !replace){
        ESP_LOGE(TAG , "String is NULL with str_input %p, pattern %p, replace %p" , str_input , pattern, replace);
        return NULL;
    }
    // Check len of str_input
    uint16_t input_len = strlen(str_input);
    uint16_t pattern_len = strlen(pattern);
    uint16_t replace_len = strlen(replace);
    if(input_len + replace_len - pattern_len > BUFFER_SIZE){
        ESP_LOGE(TAG , "Common buffer is overflow with size %d and BUFFER_SIZE %d" , input_len + replace_len - pattern_len  , BUFFER_SIZE);
        return NULL;
    }

    memset(buffer , 0 , BUFFER_SIZE);
    memcpy(buffer , str_input, input_len);
    // Get pattern location
    char * location = strstr(buffer, pattern);
    if(location == NULL){
        // Not found pattern in str_input
        return strdup(buffer);
    }
    ESP_LOGD(TAG ,"Found pattern %s in %s" , pattern, buffer);
    uint8_t len_cp = input_len - (location-buffer) - pattern_len;
    ESP_LOGD(TAG ,"Len %d\n" , len_cp);
    if(pattern_len >= replace_len){
        for (uint8_t i = 0; i < len_cp; i++)
        {
            location[replace_len + i] = location[pattern_len + i];

        }
    }
    else{
        for (uint8_t i = len_cp; i > 0 ; i--)
        {
            location[replace_len + i - 1] = location[pattern_len + i - 1 ];

        }
    }
    memcpy(location , replace , replace_len);
    return strdup(buffer);
}