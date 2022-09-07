#ifndef __TASK_H__
#define __TASK_H__


typedef enum {
    TASK_STATE_WAIT_BT_CMD,
    TASK_STATE_WAIT_FOR_UART_RSP,
    TASK_STATE_RETRY,
    TASK_STATE_SUCCESS,
    TASK_STATE_FAILED
} task_state_t;

void task_init();
void task_run();


#endif
