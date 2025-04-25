#include "common.h"
#include "log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "encoder.h" // 假设这是你的编码器头文件


int main(void)
{
    // 系统和硬件初始化
    SYSCFG_DL_init();
    Gray_Detection_Init();
    OLED_Init();
    // OLED_Test(); // 如果不需要测试，可以注释掉
    Motor_Init(); // 电机初始化
    Encoder_init(); // 编码器初始化（包括 FreeRTOS 软件定时器）
    // 设置初始电机速度
//    Motor_Control(1200, 1200);
    // 创建显示任务
    xTaskCreate(DisplayTask, "DisplayTask", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + 1, NULL);
    // 启动 FreeRTOS 调度器
    vTaskStartScheduler();

    while (1) {
    }

    return 0;
}