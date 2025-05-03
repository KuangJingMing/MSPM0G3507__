#ifndef TIMER_TASK_NOTIFICATION_H
#define TIMER_TASK_NOTIFICATION_H

#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "stdbool.h"

// 定义软件定时器的周期（以 tick 为单位）

#define TIMER_5MS_PERIOD_MS     5
#define TIMER_5MS_PERIOD_TICKS  pdMS_TO_TICKS(TIMER_5MS_PERIOD_MS)
#define NUM_PERIOD_TASKS 4

// 定义事件 ID 枚举
typedef enum {
    EVENT_NONE = 0, // Add a default/invalid event ID
    EVENT_IMU_UPDATE,
    EVENT_TEMP_CONTROL,
    EVENT_KEY_STATE_UPDATE,
    EVENT_MENU_VAR_UPDATE,
    // Add more events as needed
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
} period_task_t;

// 声明周期任务配置数组
extern period_task_t period_tasks[];


// 声明软件定时器句柄
extern TimerHandle_t xTimer5ms;

// 声明目标任务的句柄
extern TaskHandle_t PeriodicEventTaskHandle;;

// 声明创建 5ms 软件定时器的函数
void create_5ms_sw_timer(void);

// 声明启动/停止定时器的函数
void start_5ms_sw_timer(void);
void stop_5ms_sw_timer(void);

// 声明目标任务函数（通常在另一个文件中定义）
void PeriodicEventTask(void *pvParameters);

// Declare functions to enable/disable periodic tasks
void enable_periodic_task(EVENT_IDS event_id);
void disable_periodic_task(EVENT_IDS event_id);

// Declare your sub-task handler functions (should be implemented elsewhere)
// These are declared here so they can be assigned to the function pointers
void task1(void); // Corresponds to EVENT_IMU_UPDATE
void task2(void); // Corresponds to EVENT_TEMP_CONTROL
void task3(void); // Corresponds to EVENT_KEY_STATE_UPDATE
void task4(void); // Corresponds to EVENT_MENU_VAR_UPDATE


#endif // TIMER_TASK_NOTIFICATION_H
