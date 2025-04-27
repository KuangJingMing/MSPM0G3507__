#include "button_user.h"
#include "multi_button.h"
#include "ti_msp_dl_config.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "log.h"
#include "oled.h"

struct Button btn1, btn2, btn3, btn4;
#define BUTTON_SCAN_PERIOD_MS 5
static TimerHandle_t xButtonTimer = NULL; // 软件定时器句柄

typedef enum {
	BUTTON_ID0 = 0,
	BUTTON_ID1,
	BUTTON_ID2,
	BUTTON_ID3,
} BUTTON_ID;

void btn_single_click_callback(void* btn)
{
    struct Button* button = (struct Button*) btn;
    char msg[2] = "1";  // 用于显示单字符
    if (button == &btn1) {
        msg[0] = '1';
    } else if (button == &btn2) {
        msg[0] = '2';
    } else if (button == &btn3) {
        msg[0] = '3';
    } else if (button == &btn4) {
        msg[0] = '4';
    } else {
        return;
    }

    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_ncenB24_tr);  // 选用中号字体
    u8g2_DrawStr(&u8g2, 40, 32, msg);           // 居中或适合你屏幕偏移 
    u8g2_SendBuffer(&u8g2);
}

uint8_t read_button_GPIO(uint8_t button_id) {
	switch(button_id)
	{
		case BUTTON_ID0:
			return DL_GPIO_readPins(GPIO_KEY_PIN_0_PORT, GPIO_KEY_PIN_0_PIN) == 0 ? 0 : 1;
			break;
		case BUTTON_ID1:
			return DL_GPIO_readPins(GPIO_KEY_PIN_1_PORT, GPIO_KEY_PIN_1_PIN) == 0 ? 0 : 1;
			break;
		case BUTTON_ID2:
			return DL_GPIO_readPins(GPIO_KEY_PIN_2_PORT, GPIO_KEY_PIN_2_PIN) == 0 ? 0 : 1;
			break;
		case BUTTON_ID3:
			return DL_GPIO_readPins(GPIO_KEY_PIN_3_PORT, GPIO_KEY_PIN_3_PIN) == 0 ? 0 : 1;
			break;
		default:
			return 0;
			break;
	}
}


void user_button_init(void)
{
    button_init(&btn1, read_button_GPIO, 0, BUTTON_ID0); 
    button_init(&btn2, read_button_GPIO, 0, BUTTON_ID1);
    button_init(&btn3, read_button_GPIO, 0, BUTTON_ID2);
    button_init(&btn4, read_button_GPIO, 0, BUTTON_ID3);

    button_attach(&btn1, SINGLE_CLICK, btn_single_click_callback);
    button_attach(&btn2, SINGLE_CLICK, btn_single_click_callback);
    button_attach(&btn3, SINGLE_CLICK, btn_single_click_callback);
    button_attach(&btn4, SINGLE_CLICK, btn_single_click_callback);

    button_start(&btn1);
    button_start(&btn2);
    button_start(&btn3);
    button_start(&btn4);
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