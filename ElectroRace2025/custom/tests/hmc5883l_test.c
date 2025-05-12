#include "FreeRTOS.h"
#include "task.h"
#include "hmc5883l.h"
#include "tests.h"
#include "log_config.h"
#include "log.h"

static void hmc5883l_test_task(void *pvParameters)
{
    int16_t x, y, z;
    hmc5883l_init();

    while (1)
    {
        hmc5883l_read_xyz(&x, &y, &z);

        log_i("HMC5883L: X=%d, Y=%d, Z=%d yaw = %.2lf\r\n", x, y, z, hmc5883l_get_heading());
		
        vTaskDelay(pdMS_TO_TICKS(200)); // 200ms采样周期
    }
}

// 任务创建函数，供外部调用，比如 main 或 setup 里调用一次即可
void hmc5883l_test_task_start(void)
{
    xTaskCreate(
        hmc5883l_test_task,    // 任务函数
        "hmc5883lTest",        // 任务名
        256,                   // 堆栈大小（字，视平台决定，够用即可）
        NULL,                  // 参数
        tskIDLE_PRIORITY + 1,  // 优先级，略高于空闲任务即可
        NULL                   // 不需要保存任务句柄
    );
}
