#include "common_include.h"
#include "log_config.h"
#include "log.h"
#include "button_user.h"

void vOLEDTask(void *pvParameters)
{
	button_timer_init();
	button_timer_start();
	vOLEDOpeningAnimation();
	for ( ; ; ) {
		delay_ms(1);
	}
}

int main(void)
{
    // 系统和硬件初始化
    SYSCFG_DL_init();
    // 创建显示任务
		xTaskCreate(vOLEDTask, "OLED", 512, NULL, tskIDLE_PRIORITY+1, NULL);
    // 启动 FreeRTOS 调度器
    vTaskStartScheduler();
    while (1) {
    }

    return 0;
}