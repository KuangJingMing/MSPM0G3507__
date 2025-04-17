#include "motor_controller.h"
#include "log.h"
//#include "log_config.h"

// 任务函数：用于读取速度并更新 OLED 显示
 void DisplayTask(void *pvParameters)
{
    float right_speed, left_speed;
    const TickType_t xDelay = pdMS_TO_TICKS(20); // 每 20ms 更新一次显示
    static int log_counter = 0; // 用于降低日志输出频率的计数器
    const int log_frequency = 50; // 每 50 次循环输出一次日志（即 50 * 20ms = 1000ms，即每秒输出一次）

    for (;;) {
        // 获取左右电机速度
        right_speed = get_right_motor_speed();
        left_speed = get_left_motor_speed();

        // 更新 OLED 显示
        OLED_Showdecimal(0, 0, right_speed, 4, 2, 16, 1);
        OLED_Showdecimal(1, 3, left_speed, 4, 2, 16, 1);

        // 每隔一定次数输出一次日志，避免过于频繁
        log_counter++;
        if (log_counter >= log_frequency) {
            LOG_I("DisplayTask - Left Motor Speed: %.3f m/s, Right Motor Speed: %.3f m/s", left_speed, right_speed);
            log_counter = 0; // 重置计数器
        }

        // 任务延迟 20ms
        vTaskDelay(xDelay);
    }
}
