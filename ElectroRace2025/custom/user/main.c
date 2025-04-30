#include "common_include.h"
#include "log_config.h"
#include "log.h"

// 新的 FreeRTOS 任务，用于周期性打印编码器数据
void encoder_print_task(void *pvParameters) {
    const TickType_t xDelay = pdMS_TO_TICKS(200); // 200ms 延时
    log_i("Encoder Print Task Started.");
    encoder_application_init();
    for (;;) {
        int32_t right_position = encoder_manager_read(&robot_encoder_manager, 0);
        int32_t left_position = encoder_manager_read(&robot_encoder_manager, 1);
        log_i("Right Encoder: %ld, Left Encoder: %ld", right_position, left_position);
        vTaskDelay(xDelay);
    }
}

void system_init(void) {
    SYSCFG_DL_init(); // 系统硬件初始化
}

void user_init(void) {
    menu_init();
    // 创建编码器打印任务，并检查是否成功
    BaseType_t result = xTaskCreate(
        encoder_print_task,         // 任务函数
        "encoder_print_task",       // 任务名称
        configMINIMAL_STACK_SIZE,  	// 堆栈大小（单位：字）
        NULL,                       // 任务参数
        tskIDLE_PRIORITY + 2,       // 任务优先级
        NULL                        // 任务句柄
    );

    if (result == pdPASS) {
        log_i("Encoder Print Task created successfully.");
    } else {
        log_e("Failed to create Encoder Print Task. Error code: %ld", result);
        // 可以在这里添加错误处理逻辑，例如死循环或重试
        while (1);
    }
}

int main(void) {
    system_init();
    user_init();
    vTaskStartScheduler();
    while (1) {
        // 如果调度器启动失败，会到达这里
        log_e("Scheduler failed to start!");
    }
    return 0;
}