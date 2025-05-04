#include "periodic_event_task.h"
#include "common_include.h" // 确保 TIMER_5MS_PERIOD_MS 在这里定义为 5
#include "log_config.h"
#include "log.h"

static TaskHandle_t PeriodicEventTaskHandle;

void debug_task(void) {
	log_i("Euler angles (deg): Roll=%.2f, Pitch=%.2f, Yaw=%.2f Temp = %.2f\r\n",
				 smartcar_imu.rpy_deg[_ROL],
				 smartcar_imu.rpy_deg[_PIT],
				 smartcar_imu.rpy_deg[_YAW], smartcar_imu.temperature_filter);
}

// Define periodic tasks array
// {ID, RUNNING_FLAG, task_handler, period_ms}
period_task_t period_tasks[] = {
    { EVENT_IMU_UPDATE,       RUN,  imu_update_task, 					 5    }, // 5ms
    { EVENT_TEMP_CONTROL,     RUN,  imu_temperature_ctrl_task, 5    }, // 5ms
    { EVENT_KEY_STATE_UPDATE, RUN,  button_ticks, 		 				 20   }, // 20ms
    { EVENT_MENU_VAR_UPDATE,  IDLE, NotifyMenuFromISR, 				 2000 }, // 2000ms (2s)
		{ EVENT_PERIOD_PRINT,  		RUN, 	debug_task, 							 500  }, // 500ms
		{ EVENT_ALERT, 						RUN,  alert_ticks, 							 10,  }, // 10ms
};


// The main task that is notified by the 5ms timer
void PeriodicEventTask(void *pvParameters)
{
    // Variables to keep track of the last execution tick for each sub-task.
    // Stores the tick count (in FreeRTOS ticks, which are 5ms) when the task last executed.
    // Use a 32-bit array as tick count is 32-bit.
    // Initialize to a value that ensures they run on the first relevant tick.
    static uint32_t last_execution_tick[NUM_PERIOD_TASKS];

    // Use xLastWakeTime for vTaskDelayUntil to maintain strict periodicity.
    TickType_t xLastWakeTime;

    // Get the current tick count after the scheduler is running.
    uint32_t ulCurrentTick = xTaskGetTickCount();

    // Calculate the delay frequency for the main task.
    const TickType_t xFrequency = pdMS_TO_TICKS(TIMER_5MS_PERIOD_MS); // Use macro for safety

    // Initialize last_execution_tick for each task.
    // Initialize them so they are due to run after their period from the start.
    // This ensures tasks with period >= 5ms are correctly phased from the start.
    // Tasks with period < 5ms will be treated as 5ms period (run every tick).
    for (int i = 0; i < NUM_PERIOD_TASKS; i++)
    {
        // Calculate the period in FreeRTOS ticks (5ms ticks)
        uint32_t period_ticks = period_tasks[i].period_ms / TIMER_5MS_PERIOD_MS;

        // Handle periods less than the scheduler tick period (5ms).
        // Treat any positive period less than or equal to 5ms as 1 tick period.
        if (period_tasks[i].period_ms > 0 && period_ticks == 0) {
             period_ticks = 1;
        } else if (period_tasks[i].period_ms == 0) {
             period_ticks = 1; // Treat 0ms period as 5ms (1 tick)
        }
         // Store the calculated period in ticks back into the struct if you want to avoid recalculation
         // period_tasks[i].period_ticks = period_ticks; // Requires adding a field to period_task_t

        // Initialize last_execution_tick to ensure the task is scheduled correctly.
        // Subtracting period_ticks ensures the condition is met after period_ticks from start.
        // Using ulCurrentTick - period_ticks is generally the most reliable way
        // to phase tasks relative to the task start time.
        last_execution_tick[i] = ulCurrentTick - period_ticks;

        // Alternative for tasks with period <= 5ms: Initialize so they run on the very next tick.
        // This might be preferred if you want these high-frequency tasks to start immediately.
        // If period_ticks is 1 (period_ms <= 5ms and > 0), setting last_execution_tick to ulCurrentTick - 1
        // ensures it runs when current_tick becomes ulCurrentTick + 1 (the next tick).
        if (period_tasks[i].period_ms > 0 && period_tasks[i].period_ms <= TIMER_5MS_PERIOD_MS) {
             last_execution_tick[i] = ulCurrentTick - 1; // Ensure execution on the next tick
        }
        // For tasks with period_ms = 0, also treat as 1 tick period and schedule for next tick
        else if (period_tasks[i].period_ms == 0) {
             last_execution_tick[i] = ulCurrentTick - 1;
        }
        // For periods > 5ms, the initialisation ulCurrentTick - period_ticks is fine.
    }

    // Initialize xLastWakeTime with the current time.
    // This ensures the first call to vTaskDelayUntil waits for a full xFrequency,
    // and the task starts processing after that first delay.
    xLastWakeTime = xTaskGetTickCount();


    while (1)
    {
        // Block until the next multiple of xFrequency from xLastWakeTime.
        // This maintains a fixed execution rate of the main loop (every 5ms).
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // Get the current tick count after being unblocked.
        // This tick count represents the start of the current 5ms time slice.
        uint32_t current_tick = xTaskGetTickCount();

        // Check each periodic task to see if it's time to run
        for (int i = 0; i < NUM_PERIOD_TASKS; i++)
        {
            // Check if the task is currently enabled to run and if it has a valid handler
            if (period_tasks[i].is_running == RUN && period_tasks[i].task_handler != NULL)
            {
                // Calculate the period in FreeRTOS ticks (5ms ticks).
                // Recalculate or use a stored value if you added it to the struct.
                uint32_t period_ticks = period_tasks[i].period_ms / TIMER_5MS_PERIOD_MS;
                if (period_tasks[i].period_ms > 0 && period_ticks == 0) {
                     period_ticks = 1;
                } else if (period_tasks[i].period_ms == 0) {
                     period_ticks = 1;
                }

                // Check if it's time to run the task.
                // (current_tick - last_execution_tick[i]) is the number of 5ms ticks
                // that have passed since the last execution.
                // Using signed comparison for 32-bit tick counter wrap-around safety.
                if ((int32_t)(current_tick - last_execution_tick[i]) >= (int32_t)period_ticks)
                {
                    // It's time to execute this sub-task.
                    period_tasks[i].task_handler();

                    // Update the last execution tick count.
                    // To maintain the phase and ensure tasks run exactly on their period
                    // relative to their last scheduled time, add period_ticks.
                    last_execution_tick[i] += period_ticks;

                    // **Important Consideration:** If the task executed took longer than 5ms,
                    // the scheduling might be affected. However, since you've confirmed
                    // task execution time is < 1ms, we don't need the complex check
                    // `if ((int32_t)(current_tick - last_execution_tick[i]) > 0)`.
                    // Adding period_ticks is sufficient and correct for maintaining phase
                    // when tasks complete well within the tick period.
                }
            }
        }
    }
}


// Function to enable/disable periodic tasks dynamically
void enable_periodic_task(EVENT_IDS event_id)
{
    for (int i = 0; i < NUM_PERIOD_TASKS; i++)
    {
        if (period_tasks[i].id == event_id)
        {
            // When enabling, reset the last_execution_tick to schedule it correctly
            // for the next possible execution time based on its period.
            // We can set it to the current tick minus its period to make it due soon.
            uint32_t ulCurrentTick = xTaskGetTickCount();
            uint32_t period_ticks = period_tasks[i].period_ms / TIMER_5MS_PERIOD_MS;
            if (period_tasks[i].period_ms > 0 && period_ticks == 0) {
                 period_ticks = 1;
            } else if (period_tasks[i].period_ms == 0) {
                 period_ticks = 1;
            }
            // Set it to be just due.
            period_tasks[i].last_execution_tick = ulCurrentTick - period_ticks;

            // For tasks with period <= 5ms, schedule for the very next tick after enabling.
            if (period_tasks[i].period_ms > 0 && period_tasks[i].period_ms <= TIMER_5MS_PERIOD_MS) {
                 period_tasks[i].last_execution_tick = ulCurrentTick - 1;
            } else if (period_tasks[i].period_ms == 0) {
                 period_tasks[i].last_execution_tick = ulCurrentTick - 1;
            }


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
            // Optionally, you could reset last_execution_tick here if you want
            // the task to restart its period from zero when re-enabled.
            // last_execution_tick[i] = 0; // Example: reset to tick 0
            break;
        }
    }
}


void create_periodic_event_task(void) {
    // Define task parameters (adjust stack size as needed)
    const char *const pcName = "PeriodicEventTask";
    const configSTACK_DEPTH_TYPE usStackDepth = configMINIMAL_STACK_SIZE * 2;
    void *const pvParameters = NULL;
    UBaseType_t uxPriority = tskIDLE_PRIORITY + 1;
    TaskHandle_t *const pxCreatedTask = &PeriodicEventTaskHandle;

    BaseType_t xReturn = xTaskCreate(PeriodicEventTask,
                                     pcName,
                                     usStackDepth,
                                     pvParameters,
                                     uxPriority,
                                     pxCreatedTask);

    if (xReturn == pdPASS) {
        log_i("PeriodicEventTask created successfully!");
    } else {
        log_e("Failed to create PeriodicEventTask! Error code: %d", xReturn);
        // Consider adding a loop or system reset here for critical errors
    }
}
