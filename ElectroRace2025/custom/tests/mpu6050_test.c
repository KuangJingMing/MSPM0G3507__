#include "tests.h"
#include "common_include.h"
#include "log_config.h"
#include "log.h"
#include "inv_mpu.h"

extern sw_i2c_t mpu_i2c_cfg;

static void imu_init(void) {
	SOFT_IIC_Init(&mpu_i2c_cfg);
	while (mpu_dmp_init());
}

static void imu_task(void *arg) {
    imu_init();

    TickType_t xLastWakeTime;

    const TickType_t xFrequency = pdMS_TO_TICKS(20); // 使用 FreeRTOS 提供的宏进行转换，更安全

    // 初始化 xLastWakeTime 为当前时间，这样任务第一次执行时不会有延迟。
    xLastWakeTime = xTaskGetTickCount();

    uint32_t print_timer = 0; // 记录上一次打印的时间（单位：ms）

    for ( ; ; ) {
        // 等待，直到达到下一次唤醒的时间点。
        // 这会确保任务以大致固定的频率执行。
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
				float yaw = 0, roll = 0, pitch = 0;
				mpu_dmp_get_data(&yaw, &roll, &pitch);
			
        // 获取当前时间（使用 FreeRTOS 函数）
        uint32_t now = xTaskGetTickCount();

        // 每500ms输出一次Euler角
        if (now - print_timer >= pdMS_TO_TICKS(500)) { // 同样使用宏转换
            print_timer = now;
            log_i("Euler angles (deg): Roll=%.2f, Pitch=%.2f, Yaw=%.2f",
                   roll,
                   pitch,
                   yaw);
        }
    }
}


void mpu6050_task_create(void) {
    xTaskCreate(imu_task, "IMUTask", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, NULL);
}
