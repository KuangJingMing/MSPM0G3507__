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
		{ EVENT_CAR, 				   		RUN,  car_task, 							   20,  }, // 10ms
};


// 任务调度函数
void PeriodicEventTask(void *pvParameters)
{
    // 跟踪每个子任务的上次执行时间
    static uint32_t last_execution_tick[NUM_PERIOD_TASKS];
    
    TickType_t xLastWakeTime;
    uint32_t ulCurrentTick = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(TIMER_5MS_PERIOD_MS);
    // 预计算所有任务的period_ticks并初始化last_execution_tick
    for (int i = 0; i < NUM_PERIOD_TASKS; i++)
    {
        // 计算ticks并存储，避免后续重复计算
        period_tasks[i].period_ticks = (period_tasks[i].period_ms > 0) 
            ? MAX(1, period_tasks[i].period_ms / TIMER_5MS_PERIOD_MS) 
            : 1;  // 处理0ms和小于5ms的特殊情况
        
        // 对于短周期任务(<=5ms)，设置为下一个tick运行
        if (period_tasks[i].period_ms <= TIMER_5MS_PERIOD_MS) {
            last_execution_tick[i] = ulCurrentTick - 1;
        } else {
            // 对于长周期任务，设置为一个周期后运行
            last_execution_tick[i] = ulCurrentTick - period_tasks[i].period_ticks;
        }
    }
    xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        // 等待下一个5ms周期
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        uint32_t current_tick = xTaskGetTickCount();
        // 检查并执行到期的任务
        for (int i = 0; i < NUM_PERIOD_TASKS; i++)
        {
            if (period_tasks[i].is_running == RUN && period_tasks[i].task_handler != NULL)
            {
                // 使用预计算的period_ticks，避免重复计算
                if ((int32_t)(current_tick - last_execution_tick[i]) >= (int32_t)period_tasks[i].period_ticks)
                {
                    // 执行任务
                    period_tasks[i].task_handler();
                    
                    // 更新下次执行时间
                    last_execution_tick[i] += period_tasks[i].period_ticks;
                }
            }
        }
    }
}
// 动态启用任务
void enable_periodic_task(EVENT_IDS event_id)
{
    uint32_t ulCurrentTick = xTaskGetTickCount();
    
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
void create_periodic_event_task(void) {
    BaseType_t xReturn = xTaskCreate(
        PeriodicEventTask,
        "PeriodicEventTask",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        tskIDLE_PRIORITY + 1,
        &PeriodicEventTaskHandle);
    if (xReturn == pdPASS) {
        log_i("PeriodicEventTask created successfully!");
    } else {
        log_e("Failed to create PeriodicEventTask! Error code: %d", xReturn);
    }
}
