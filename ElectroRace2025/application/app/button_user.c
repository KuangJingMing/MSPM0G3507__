#include "button_user.h"
#include "multi_button.h"
#include "ti_msp_dl_config.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "log.h"

#define BUTTON_SCAN_PERIOD_MS 30

struct Button btn1, btn2, btn3, btn4, btn5;

static TimerHandle_t xButtonTimer = NULL; // 软件定时器句柄

uint8_t read_button_GPIO(uint8_t button_id) {
	switch(button_id)
	{
		case BUTTON_UP:
			return DL_GPIO_readPins(GPIO_KEY_PORT, GPIO_KEY_PIN_0_PIN) == 0 ? 0 : 1;
			break;
		case BUTTON_DOWN:
			return DL_GPIO_readPins(GPIO_KEY_PORT, GPIO_KEY_PIN_1_PIN) == 0 ? 0 : 1;
			break;
		case BUTTON_LEFT:
			return DL_GPIO_readPins(GPIO_KEY_PORT, GPIO_KEY_PIN_2_PIN) == 0 ? 0 : 1;
			break;
		case BUTTON_RIGHT:
			return DL_GPIO_readPins(GPIO_KEY_PORT, GPIO_KEY_PIN_3_PIN) == 0 ? 0 : 1;
			break;
		case BUTTON_MIDDLE:
			return DL_GPIO_readPins(GPIO_KEY_PORT, GPIO_KEY_PIN_4_PIN) == 0 ? 0 : 1;
			break;
		default:
			return 0;
			break;
	}
}



// 软件定时器回调
static void vButtonTimerCallback(TimerHandle_t xTimer)
{
    button_ticks();
}

// 定时器启动
void button_timer_start(void)
{
    if (xButtonTimer != NULL) {
        xTimerStart(xButtonTimer, 0);
    }
}

// 定时器停止
void button_timer_stop(void)
{
    if (xButtonTimer != NULL) {
        xTimerStop(xButtonTimer, 0);
    }
}

// 定时器初始化（一般只需调用一次即可）
void button_timer_init(void)
{
    if (xButtonTimer == NULL) {
        xButtonTimer = xTimerCreate(
            "ButtonTimer",
            pdMS_TO_TICKS(BUTTON_SCAN_PERIOD_MS),
            pdTRUE,
            NULL,
            vButtonTimerCallback
        );
    }
}


void user_button_init(BtnCallback cb)
{
    button_init(&btn1, read_button_GPIO, 0, BUTTON_UP); 
    button_init(&btn2, read_button_GPIO, 0, BUTTON_DOWN);
    button_init(&btn3, read_button_GPIO, 0, BUTTON_LEFT);
    button_init(&btn4, read_button_GPIO, 0, BUTTON_RIGHT);
		button_init(&btn5, read_button_GPIO, 0, BUTTON_MIDDLE);
		
    button_attach(&btn1, SINGLE_CLICK, cb);
    button_attach(&btn2, SINGLE_CLICK, cb);
    button_attach(&btn3, SINGLE_CLICK, cb);
    button_attach(&btn4, SINGLE_CLICK, cb);
		button_attach(&btn5, SINGLE_CLICK, cb);
		
    button_start(&btn1);
    button_start(&btn2);
    button_start(&btn3);
    button_start(&btn4);
		button_start(&btn5);
		
		button_timer_stop();
		button_timer_init();
		button_timer_start();
}
