#ifndef PERIODIC_EVENT_TASK_H
#define PERIODIC_EVENT_TASK_H

#include "FreeRTOS.h"
#include "task.h"

// 定义软件定时器的周期（以 tick 为单位）

#define TIMER_5MS_PERIOD_MS     5

// 定义事件 ID 枚举
typedef enum {
    EVENT_NONE = 0, // Add a default/invalid event ID
    EVENT_IMU_UPDATE,
    EVENT_TEMP_CONTROL,
    EVENT_KEY_STATE_UPDATE,
    EVENT_MENU_VAR_UPDATE,
    EVENT_PERIOD_PRINT,
		EVENT_ALERT,
		EVENT_CAR,
	  NUM_PERIOD_TASKS
} EVENT_IDS;

// 定义运行状态枚举
typedef enum {
    RUN,
    IDLE,
} TASK_STATE;


// 增加period_ticks字段，避免重复计算
typedef struct {
    EVENT_IDS id;
    TASK_STATE is_running;
    void (*task_handler)(void);
    uint32_t period_ms;
    uint32_t period_ticks;  // 存储计算后的ticks值
} period_task_t;


void create_periodic_event_task(void);

void enable_periodic_task(EVENT_IDS event_id);
void disable_periodic_task(EVENT_IDS event_id);

#endif // PERIODIC_EVENT_TASK_H
