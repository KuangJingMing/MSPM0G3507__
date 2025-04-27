#include "common_include.h"
//#include "log_config.h"
#include "log.h"

void init_task(void *pvParameters) {
	create_oled_menu();
	vTaskDelete(NULL);
}

int main(void)
{
    SYSCFG_DL_init(); // 系统和硬件初始化
		xTaskCreate(init_task, "init_task", 512, NULL, tskIDLE_PRIORITY + 2, NULL);
		vTaskStartScheduler();
    while (1) {
    
		}
    return 0;
}
