#include "common_include.h"
#include "log_config.h"
#include "log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "encoder.h" // 假设这是你的编码器头文件

void TestTask(void *arg) {
	for ( ; ; ) {
		
		delay_ms(100);
	}
}



int main(void)
{
    // 系统和硬件初始化
    SYSCFG_DL_init();
		embedfire_protocol_receive_init();
    gray_detection_init();
    OLED_Init();
    // OLED_Test(); // 如果不需要测试，可以注释掉
    Encoder_init(); // 编码器初始化（包括 FreeRTOS 软件定时器）
		motor_init();
		motor_set_pwm(MOTOR_FRONT_LEFT, -1500);
		motor_set_pwm(MOTOR_FRONT_RIGHT, -1500);
    // 创建显示任务
    xTaskCreate(TestTask, "TestTask", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + 1, NULL);
    // 启动 FreeRTOS 调度器
    vTaskStartScheduler();

    while (1) {
    }

    return 0;
}