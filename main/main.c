#include <stdio.h>
#include "bluetooth.h"

void app_init(){
    bluetooth_init();
}


void app_main(void)
{
    app_init();
    while(1){
    }
}