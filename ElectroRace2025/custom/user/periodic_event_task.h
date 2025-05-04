#ifndef PERIODIC_EVENT_TASK_H
#define PERIODIC_EVENT_TASK_H

#include "FreeRTOS.h"
#include "task.h"

// 定义软件定时器的周期（以 tick 为单位）

#define TIMER_5MS_PERIOD_MS     5
#define TIMER_5MS_PERIOD_TICKS  pdMS_TO_TICKS(TIMER_5MS_PERIOD_MS)
#define NUM_PERIOD_TASKS 7

// 定义事件 ID 枚举
typedef enum {
    EVENT_NONE = 0, // Add a default/invalid event ID
    EVENT_IMU_UPDATE,
    EVENT_TEMP_CONTROL,
    EVENT_KEY_STATE_UPDATE,
    EVENT_MENU_VAR_UPDATE,
    EVENT_PERIOD_PRINT,
		EVENT_ALERT,
} EVENT_IDS;

// 定义运行状态枚举
typedef enum {
    RUN,
    IDLE,
} RUNNING_FLAG;

// 定义周期性任务结构体
typedef struct {
    EVENT_IDS id;
    RUNNING_FLAG is_running;
    void (*task_handler)(void); // Function pointer to the sub-task handler
    uint16_t period_ms;         // Task's period in milliseconds
		uint32_t last_execution_tick;
} period_task_t;


void create_periodic_event_task(void);

void enable_periodic_task(EVENT_IDS event_id);
void disable_periodic_task(EVENT_IDS event_id);

#endif // PERIODIC_EVENT_TASK_H
