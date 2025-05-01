#include "common_include.h"
#include "log_config.h" // 确保包含这个，以启用日志
#include "log.h"
#include "ICM20608.h"
#include "Fusion/Fusion.h"
#include "imu_app.h"
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "eeprom.h"

// IMU 数据获取、处理和打印任务
void imu_process_and_print_task(void *pvParameters) {

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(5); // 任务运行频率 5ms
    uint32_t print_count = 0; // 打印计数器
    const uint32_t print_interval = 2; // 每 100 个任务周期打印一次 (即 100 * 5ms = 500ms 打印一次)

    // 任务开始时获取当前时间
    xLastWakeTime = xTaskGetTickCount();
	smartcar_imu.imu_cal_flag = 0;
    for (;;) {
        // 等待下一个周期的唤醒，以保持固定频率
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // 执行 IMU 数据处理和姿态解算
        imu_data_sampling();
        trackless_ahrs_update();
        temperature_state_check();
				
        // 打印欧拉角 (控制打印频率)
        print_count++;
        if (print_count >= print_interval) {
            print_count = 0; // 重置计数器

            // 获取欧拉角
            float roll = smartcar_imu.rpy_deg[_ROL];
            float pitch = smartcar_imu.rpy_deg[_PIT];
            float yaw = smartcar_imu.rpy_deg[_YAW];

            // 使用日志宏打印欧拉角
           serialplot_send_multi_data(1, yaw);
        }
    }

    // 如果任务循环退出，通常是错误情况，删除任务自身
    vTaskDelete(NULL);
}


void system_init(void) {
    SYSCFG_DL_init(); // 系统硬件初始化
    // 其他低层硬件初始化，如时钟、GPIO 等
}



void user_init(void) {
    //menu_init(); // 其他用户模块初始化
    // 在创建任务之前初始化外设

		while(ICM206xx_Init()) {}
    w25qxx_gpio_init();
    // 创建 IMU 处理和打印任务
    // 适当增加堆栈大小
    BaseType_t result = xTaskCreate(
        imu_process_and_print_task,         // 任务函数
        "imu_print_task",       // 任务名称
        configMINIMAL_STACK_SIZE * 2,  	// 堆栈大小（示例：增加到2倍）
        NULL,                       // 任务参数
        tskIDLE_PRIORITY + 2,       // 任务优先级
        NULL                        // 任务句柄
    );

    if (result == pdPASS) {
        log_i("imu_print_task created successfully.");
    } else {
        log_e("Failed to create imu_print_task. Error code: %ld", result);
        // 错误处理逻辑
        while (1);
    }

    // 您可能在这里创建其他任务
    // ... 创建其他任务 ...
}

int main(void) {
    system_init();
    user_init();
    vTaskStartScheduler();

    // 如果调度器启动失败，会到达这里
    while (1) {
        log_e("Scheduler failed to start!");
    }
    return 0;
}
