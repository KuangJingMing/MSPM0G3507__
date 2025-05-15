#include "tests.h"
#include "common_include.h"
#include "log_config.h"
#include "log.h"
#include "FreeRTOS.h" // 确保包含了 FreeRTOS 的头文件，以便使用 xTaskGetTickCount 和 vTaskDelayUntil
#include "task.h"     // 确保包含了 FreeRTOS 的任务头文件

void imu_init(void) {
    while (ICM206xx_Init()){}
    imu_calibration_params_init();
		imu_temperature_ctrl_init();
}

void imu_task(void *arg) {
    imu_init();

    // vTaskDelayUntil 的参数
    // pxLastWakeTime 是一个指针，指向一个变量，该变量用于存储任务上一次被唤醒的时间点。
    // 这个变量在任务第一次运行时由 vTaskDelayUntil 初始化。
    TickType_t xLastWakeTime;

    // xFrequency 是任务的执行周期（以系统 Tick 为单位）。
    // sampling_frequent 是采样频率，这里是 200 Hz。
    // 所以周期是 1 / 200 = 0.005 秒 = 5 毫秒。
    // 将毫秒转换为 Tick，需要除以 configTICK_RATE_HZ 的倒数，或者乘以 configTICK_RATE_HZ。
    // 假设 configTICK_RATE_HZ 是 1000 (1ms/Tick)，那么 5ms 就是 5 Tick。
    // 如果 configTICK_RATE_HZ 是 200 (5ms/Tick)，那么 5ms 就是 1 Tick。
    // 为了通用性，我们将周期直接定义为 5 毫秒，并转换为 Tick。
    const TickType_t xFrequency = pdMS_TO_TICKS(5); // 使用 FreeRTOS 提供的宏进行转换，更安全

    // 初始化 xLastWakeTime 为当前时间，这样任务第一次执行时不会有延迟。
    xLastWakeTime = xTaskGetTickCount();

    uint32_t print_timer = 0; // 记录上一次打印的时间（单位：ms）

    for ( ; ; ) {
        // 等待，直到达到下一次唤醒的时间点。
        // 这会确保任务以大致固定的频率执行。
        vTaskDelayUntil( &xLastWakeTime, xFrequency );

        // IMU 数据处理和姿态更新
        imu_data_sampling();
        trackless_ahrs_update();
        simulation_pwm_output();
				imu_temperature_ctrl();
				alert_ticks();
        // 获取当前时间（使用 FreeRTOS 函数）
        uint32_t now = xTaskGetTickCount();

        // 每500ms输出一次Euler角
        if (now - print_timer >= pdMS_TO_TICKS(500)) { // 同样使用宏转换
            print_timer = now;
            log_i("Euler angles (deg): Roll=%.2f, Pitch=%.2f, Yaw=%.2f Temp = %.2f\r\n",
                   smartcar_imu.rpy_deg[_ROL],
                   smartcar_imu.rpy_deg[_PIT],
                   smartcar_imu.rpy_deg[_YAW], smartcar_imu.temperature_filter);
        }

        // 注意：使用 vTaskDelayUntil 后，就不需要额外的 vTaskDelay 了。
        // 任务的周期性由 vTaskDelayUntil 保证。
    }
}


void imu_task_create(void) {
    xTaskCreate(imu_task, "IMUTask", configMINIMAL_STACK_SIZE * 3, NULL, tskIDLE_PRIORITY + 1, NULL);
}
