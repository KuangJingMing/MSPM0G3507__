#include "common_include.h"

#include "log_config.h" // 日志配置
#include "log.h"


void system_init(void) {
    SYSCFG_DL_init();
}

void user_init(void) {
    menu_init_and_create();
}

void test_task_init_and_create(void) {

}

int main(void) {
    system_init();

#if UNIT_TEST_MODE
    test_task_init_and_create(); // 这里也建议加任务创建
#else
    user_init();
#endif

    vTaskStartScheduler(); // 启动调度器

    // 如果能来到这里，说明调度器启动失败
    while (1) {
			log_e("Scheduler Start Faild");
			break;
    }

    return 0;
}


// 堆栈溢出钩子。置顶函数名，不可修改。
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    log_e("!!! STACK OVERFLOW: %s !!!\n", pcTaskName); // 错误日志输出
    while (1)
    {

    }
}
// 默认句柄发生严重错误时进入
void Default_Handler(void)
{
    log_e("!!! Unhandled Interrupt !!!\n");
    while (1)
    {

    }
}