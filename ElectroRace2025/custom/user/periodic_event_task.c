#include "periodic_event_task.h"
#include "timer_task_notification.h" // Include this to access period_tasks and FreeRTOS tick count
#include "log_config.h"
#include "log.h"

TaskHandle_t PeriodicEventTaskHandle;

// The main task that is notified by the 5ms timer
void PeriodicEventTask(void *pvParameters)
{
    // Variables to keep track of the last execution time for each sub-task
    // Store the tick count (scaled by 5ms) when the task last executed.
    // Use a 32-bit array as tick count is 32-bit.
    // Initialize to a value that ensures they run on the first tick if is_running is RUN.
    static uint32_t last_execution_tick[NUM_PERIOD_TASKS] = {0};


    // Initialize last_execution_tick based on current time to ensure correct timing on start
    // Get the current tick count after the scheduler is running
    uint32_t ulCurrentTick = xTaskGetTickCount();

    for (int i = 0; i < NUM_PERIOD_TASKS; i++)
    {
        // Calculate the period in 5ms ticks
        uint32_t period_ticks = period_tasks[i].period_ms / TIMER_5MS_PERIOD_MS;
        // Handle cases where period is less than 5ms or not a multiple
        if (period_ticks == 0 && period_tasks[i].period_ms > 0) {
             period_ticks = 1; // Minimum period is 5ms
        } else if (period_tasks[i].period_ms == 0) {
            period_ticks = 1; // Assume 0ms period means run every tick
        }

        // Initialize last_execution_tick so the task runs after its period
        // Subtracting the period_ticks will make the condition ((int32_t)(current_tick - last_execution_tick[i]) >= (int32_t)period_ticks)
        // true on the first relevant tick.
        last_execution_tick[i] = ulCurrentTick - period_ticks;

        // A more robust initialisation to ensure tasks with a period of 5ms run on the first tick:
        if (period_tasks[i].period_ms <= TIMER_5MS_PERIOD_MS && period_tasks[i].period_ms > 0) {
            last_execution_tick[i] = ulCurrentTick; // Or ulCurrentTick - 1 to guarantee execution on the very next tick
        }
    }


    while (1)
    {
        // Wait for a notification from the 5ms timer.
        // ulTaskNotifyTake clears the notification count.
        // We don't need the returned value, just the fact that we were notified.
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // pdTRUE clears the notification value (count of notifications)
                                                 // portMAX_DELAY waits indefinitely

        // Received a notification (a 5ms tick has passed)
        uint32_t current_tick = xTaskGetTickCount(); // Get the current tick count (in FreeRTOS ticks, which are 5ms)

        // Check each periodic task to see if it's time to run
        for (int i = 0; i < NUM_PERIOD_TASKS; i++)
        {
            // Check if the task is currently enabled to run and if it has a valid handler
            if (period_tasks[i].is_running == RUN && period_tasks[i].task_handler != NULL)
            {
                // Calculate the period in 5ms ticks
                uint32_t period_ticks = period_tasks[i].period_ms / TIMER_5MS_PERIOD_MS;
                // Ensure minimum period of 1 tick if period_ms > 0 but < TIMER_5MS_PERIOD_MS
                if (period_ticks == 0 && period_tasks[i].period_ms > 0) {
                    period_ticks = 1;
                } else if (period_tasks[i].period_ms == 0) {
                    period_ticks = 1; // Treat 0ms period as 5ms
                }

                // Check if it's time to run the task
                // (current_tick - last_execution_tick[i]) gives the number of 5ms ticks since last execution.
                // If this is greater than or equal to period_ticks, it's time to run.
                // Using signed comparison for 32-bit tick counter wrap-around safety.
                if ((int32_t)(current_tick - last_execution_tick[i]) >= (int32_t)period_ticks)
                {
                    // It's time to execute this sub-task
                    period_tasks[i].task_handler();

                    // Update the last execution tick count
                    // Update by adding period_ticks to the previous last_execution_tick to maintain phase
                    // This handles wrap-around correctly if period_ticks is not excessively large.
                    last_execution_tick[i] += period_ticks;

                     // If task took longer than its period, update last_execution_tick to current_tick
                     // to prevent immediately running it again or getting stuck.
                     // This approach prioritizes meeting future deadlines over catching up on missed ones.
                    if ((int32_t)(current_tick - last_execution_tick[i]) > 0) {
                         last_execution_tick[i] = current_tick;
                    }
                }
            }
        }
    }
}

void create_periodic_event_task(void) {
    BaseType_t xReturn = xTaskCreate(PeriodicEventTask, // 任务函数
                                     "PeriodicEventTask", // 任务名称
                                     configMINIMAL_STACK_SIZE * 2, // 堆栈大小
                                     NULL, // 传递给任务的参数
                                     tskIDLE_PRIORITY + 1, // 任务优先级
                                     &PeriodicEventTaskHandle); // 任务句柄

    if (xReturn == pdPASS) {
        // 任务创建成功
        // 你可以在这里打印一条日志或者设置一个标志
        log_i("PeriodicEventTask created successfully!\n");
    } else {
        // 任务创建失败
        // 这通常是由于内存不足
        // 你需要在这里处理错误，例如打印错误信息，或者进入一个错误状态
        log_e("Failed to create PeriodicEventTask! Error code: %d\n", xReturn);
        // 可以在这里实现一些错误恢复或报告机制
    }
}
