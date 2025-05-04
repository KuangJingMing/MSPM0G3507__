#include "common_include.h"

#include "log_config.h" // 日志配置
#include "log.h"

void system_init(void) {
  SYSCFG_DL_init();
}

void user_init(void) {
	menu_init_and_create();
	AT24CXX_Init();
	imu_init_blocking();
	imu_temperature_ctrl_init();
	car_init();
	create_periodic_event_task();
}

void test_task_init_and_create(void) {
	at24cxx_single_test();	
}

int main(void) {
    system_init();
	
#if UNIT_TEST_MODE	//是否使能单元测试功能
    test_task_init_and_create(); // 这里也建议加任务创建
#else
    user_init();
#endif

    vTaskStartScheduler(); // 启动调度器

    // 如果能来到这里，说明调度器启动失败
    while (1) {
			log_e("Scheduler Start Faild");
			play_alert_blocking(3, COLOR_YELLOW);
			break;
    }

    return 0;
}

// 堆栈溢出钩子。置顶函数名，不可修改。
// rgb 紫色
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    log_e("!!! STACK OVERFLOW: %s !!!\n", pcTaskName); // 错误日志输出
	
		play_alert_blocking(3, COLOR_PURPLE);
	
		taskDISABLE_INTERRUPTS();
    while (1)
    {

    }
}

//发生严重错误时进入
// rgb 红色
void HardFault_Handler(void) 
{
    log_e("!!! Unhandled Interrupt HardFault_Handler !!!\n");
	
		play_alert_blocking(3, COLOR_RED);
	
		taskDISABLE_INTERRUPTS();
    while (1)
    {

    }

}

//内存分配失败钩子
void vApplicationMallocFailedHook( void )
{

    log_e("!!! MALLOC FAILED !!! FreeRTOS heap allocation failed.\n");

    play_alert_blocking(3, COLOR_RED);

    taskDISABLE_INTERRUPTS();
    for( ;; )
    {

    }
}