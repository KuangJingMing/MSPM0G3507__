#include "oled_menu.h"
#include "oled_driver.h"
#include "button_user.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "u8g2.h"

#define MAX_INDEX_COUNT 4

static inline void btn_single_click_callback(void* btn);
static TaskHandle_t xOLEDTaskHandle = NULL;

static MenuNode menu_show_status = {
    .name = "Show Status",
    .callback = NULL,
    .parent = NULL,
    .children = {NULL},
    .child_count = 0,
};

static MenuNode menu_run_tasks = {
    .name = "Run Tasks",
    .callback = NULL,
    .parent = NULL,
    .children = {NULL},
    .child_count = 0,
};

static MenuNode set_pid = {
    .name = "Set Pid",
    .callback = NULL,
    .parent = NULL,
    .children = {NULL},
    .child_count = 0,
};

static MenuNode play_games = {
    .name = "Play Game",
    .callback = NULL,
    .parent = NULL,
    .children = {NULL},
    .child_count = 0,
};

static MenuNode test_01 = {
    .name = "Test 01",
    .callback = NULL,
    .parent = NULL,
    .children = {NULL},
    .child_count = 0,
};

static MenuNode test_02 = {
    .name = "Test 02",
    .callback = NULL,
    .parent = NULL,
    .children = {NULL},
    .child_count = 0,
};

static MenuNode menu_root = {
    .name = "Main Menu",
    .callback = NULL,
    .parent = NULL,
    .children = {&menu_run_tasks, &menu_show_status, &set_pid, &play_games, &test_01, &test_02},
    .child_count = 6,
};


static MenuNode* current_menu = &menu_root;
static int current_index = 0;
static int window_start_index = 0;

static void draw_menu(void)
{
    u8g2_ClearBuffer(&u8g2);
    // 设置字体为英文小字体
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tr);
    
    for(int i = 0; i < MAX_INDEX_COUNT && (i + window_start_index) < current_menu->child_count; ++i)
    {
        int y = 10 + i * MENU_LINE_HEIGHT; // 起始位置上移
        
        if((i + window_start_index) == current_index)
        {
            // 绘制选中项的边框
            int y1 = y - 9;
            int width = 128;
            int height = MENU_LINE_HEIGHT - 2;
            
            // 绘制实心边框
            u8g2_DrawFrame(&u8g2, 0, y1, width, height); // 使用DrawFrame代替四条线
            
            // 绘制指示箭头
            u8g2_DrawStr(&u8g2, 2, y, ">");
            u8g2_DrawStr(&u8g2, 10, y, current_menu->children[i + window_start_index]->name);
        }
        else
        {
            // 绘制非选中项
            u8g2_DrawStr(&u8g2, 10, y, current_menu->children[i + window_start_index]->name);
        }
    }
    
    // 绘制滚动指示器（如果需要）
    if(window_start_index > 0) {
        // 上滚动指示器（放大且位置稍低）
        u8g2_DrawTriangle(&u8g2, 124, 4, 120, 10, 128, 10);
    }
    
    if(window_start_index + MAX_INDEX_COUNT < current_menu->child_count) {
        // 下滚动指示器（放大且确保在屏幕内）
        u8g2_DrawTriangle(&u8g2, 124, 60, 120, 54, 128, 54);
    }
    
    u8g2_SendBuffer(&u8g2);
}


static void menu_init(void) {
	u8g2_Init();
  show_oled_opening_animation();
  user_button_init(&btn_single_click_callback);    
  draw_menu();
}


void select_next(void)
{
  if (current_index < menu_root.child_count - 1) current_index++;
  if (current_index >= window_start_index + MAX_INDEX_COUNT)
    window_start_index = current_index - MAX_INDEX_COUNT + 1;
  if(window_start_index > menu_root.child_count - MAX_INDEX_COUNT)
    window_start_index = menu_root.child_count - MAX_INDEX_COUNT;
  if(window_start_index < 0) window_start_index = 0;
}

void select_previous(void)
{
  if (current_index > 0) current_index--;
  if (current_index < window_start_index)
    window_start_index = current_index;
  if(window_start_index < 0) window_start_index = 0;
}

static void enter_current(void) {

}

static void return_previous(void) {

}

static void vOLEDTask(void *pvParameters)
{
		menu_init();
    for ( ; ; ) {
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // 等待通知、收到自动清0
      draw_menu();
    }
}

void create_oled_menu(void) {
    xTaskCreate(vOLEDTask, "OLED_MENU", 128, NULL, tskIDLE_PRIORITY + 1, &xOLEDTaskHandle);
}

static inline void btn_single_click_callback(void* btn)
{
    struct Button* button = (struct Button*) btn;
    if (button == &btn1) {  			 //BUTTON_UP
			select_previous();
    } else if (button == &btn2) {  //BUTTON_DOWN
			select_next();
    } else if (button == &btn3) {  //BUTTON_LEFT
			enter_current(); 
    } else if (button == &btn4) {  //BUTTON_RIGHT
			return_previous();
    } else if (button == &btn5) {  //BUTTON_MIDDLE

    }
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(xOLEDTaskHandle, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
