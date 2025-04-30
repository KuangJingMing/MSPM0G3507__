#include "common_include.h"
#include "log_config.h"
#include "log.h"


// 新的 FreeRTOS 任务，用于周期性打印编码器数据
void encoder_print_task(void *pvParameters) {
    const TickType_t xDelay = pdMS_TO_TICKS(200); // 500ms 延时
    log_i("Encoder Print Task Started.");
		encoder_application_init();
    for (;;) {
        // 读取右轮编码器位置 (索引 0)
        int32_t right_position = encoder_manager_read(&robot_encoder_manager, 0);
        // 读取左轮编码器位置 (索引 1)
        // 注意：这里只读不清零，如果您需要周期性获取增量，可以使用 read_and_reset
        int32_t left_position = encoder_manager_read(&robot_encoder_manager, 1);
        // 使用 log_i 宏打印编码器数据
        log_i("Right Encoder: %ld, Left Encoder: %ld", right_position, left_position);
        // 任务延时
        vTaskDelay(xDelay);
    }
}

void init_task(void *pvParameters) {
	xTaskCreate(encoder_print_task, "encoder_print_task", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
	vTaskDelete(NULL);
}

int main(void)
{
    SYSCFG_DL_init(); // 系统硬件初始化
	menu_init();
//	xTaskCreate(encoder_print_task, "encoder_print_task", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
		vTaskStartScheduler();
    while (1) {
    
		}
    return 0;
}
