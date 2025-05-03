#include "common_include.h"
#include "log.h"
#include "timer_task_notification.h"
// --- Implement your sub-task handler functions here or in a separate file ---
// They should be designed to execute quickly and not block.
void task1(void)
{

}

void task2(void)
{

}

void task3(void)
{
	button_ticks();
}

void task4(void)
{
	NotifyMenuFromISR();
}

// Define periodic tasks array
period_task_t period_tasks[] = {
    { EVENT_IMU_UPDATE,       RUN,  task1, 5    }, // {ID, RUNNING_FLAG, task_handler, period_ms}
    { EVENT_TEMP_CONTROL,     RUN,  task2, 5    },
    { EVENT_KEY_STATE_UPDATE, RUN,  task3, 20   },
    { EVENT_MENU_VAR_UPDATE,  IDLE, task4, 1000 }, // 1000ms (1s)
    // Add more tasks here
};


// Software timer handle definition
TimerHandle_t xTimer5ms = NULL;

// Target task handle definition


// Software timer callback function
static void vTimer5msCallback( TimerHandle_t xTimer )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // ** VERY IMPORTANT: Ensure xTargetTaskHandle is not NULL before notifying **
    if (PeriodicEventTaskHandle != NULL)
    {
        // Notify the target task. We don't need to send a specific value
        // with GiveFromISR, as the target task will get the current tick count
        // and decide which sub-tasks to run.
        vTaskNotifyGiveFromISR( PeriodicEventTaskHandle, &xHigherPriorityTaskWoken );

        // In Timer Service Task, FreeRTOS handles the yield automatically.
        // portYIELD_FROM_ISR(&xHigherPriorityTaskWoken); // Usually not needed here
    }
}

void create_5ms_sw_timer(void)
{
    xTimer5ms = xTimerCreate(
        "5ms Timer",
        TIMER_5MS_PERIOD_TICKS,
        pdTRUE, // Auto-reload timer
        (void *)0,
        vTimer5msCallback
    );

    if (xTimer5ms == NULL)
    {
        // Timer creation failed
        // Handle the error, e.g., print message, assert
        log_i("Failed to create 5ms software timer.\n");
        // assert(pdFALSE);
    }
		start_5ms_sw_timer();
}

void start_5ms_sw_timer(void)
{
    if (xTimer5ms != NULL)
    {
        // Start the timer. The 0 means don't block.
        if (xTimerStart(xTimer5ms, 0) == pdPASS)
        {
           log_i("5ms software timer started.\n");
        } else {
           log_i("Failed to start 5ms software timer.\n");
        }
    }
}

void stop_5ms_sw_timer(void)
{
    if (xTimer5ms != NULL)
    {
        xTimerStop(xTimer5ms, 0);
        log_i("5ms software timer stopped.\n");
    }
}

// Function to enable/disable periodic tasks dynamically
void enable_periodic_task(EVENT_IDS event_id)
{
    for (int i = 0; i < NUM_PERIOD_TASKS; i++)
    {
        if (period_tasks[i].id == event_id)
        {
            period_tasks[i].is_running = RUN;
            break;
        }
    }
}

void disable_periodic_task(EVENT_IDS event_id)
{
    for (int i = 0; i < NUM_PERIOD_TASKS; i++)
    {
        if (period_tasks[i].id == event_id)
        {
            period_tasks[i].is_running = IDLE;
            break;
        }
    }
}

