#include "menu_logic.h"
#include "menu_ui.h"

#define VARIABLE_UPDATE_PERIOD_MS 1000  // 变量更新周期，1秒

static TaskHandle_t xOLEDTaskHandle;
static TimerHandle_t xVariableUpdateTimer = NULL;
static MenuNode *current_menu;
static menu_variables_t menu_variables[MAX_INDEX_COUNT];

void create_oled_menu(MenuNode *root) {
    current_menu = root;
    xTaskCreate(vOLEDTask, "OLED_MENU", 512, NULL, tskIDLE_PRIORITY + 1, &xOLEDTaskHandle);
}
void select_next(void)
{
  if (current_menu->current_index < current_menu->child_count - 1) current_menu->current_index++;
  if (current_menu->current_index >= current_menu->window_start_index + MAX_INDEX_COUNT)
    current_menu->window_start_index = current_menu->current_index - MAX_INDEX_COUNT + 1;
}

void select_previous(void)
{
  if (current_menu->current_index > 0) current_menu->current_index--;
  if (current_menu->current_index < current_menu->window_start_index) current_menu->window_start_index = current_menu->current_index;
}

void enter_current(void) {
	if (current_menu->child_count == 0) return;
	current_menu = current_menu->children[current_menu->current_index];
}

void return_previous(void) {
	if (current_menu->parent == NULL) return;
	current_menu = current_menu->parent;
}

void add_variable(const char *name, float *val_ptr) {
    static uint8_t data_index = 0;
    if (data_index < MAX_INDEX_COUNT) {
        menu_variables[data_index].name = name;
        menu_variables[data_index].val_ptr = val_ptr;
        data_index++;
    }
}

void vOLEDTask(void *pvParameters)
{
		u8g2_Init();
		#if SHOW_OPENING_ANIMATION
			show_oled_opening_animation();
		#endif
		draw_menu(current_menu);
    for ( ; ; ) {
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // 等待通知、收到自动清0
      draw_menu(current_menu);
			execute_callback();
			if (current_menu->type == MENU_TYPE_VARIABLE_VIEW) {
        draw_variables_menu(menu_variables);
			} else {
				stop_listening_variable_timer();
			}
    }
}

void execute_callback(void) {
	if (current_menu->callback != NULL) current_menu->callback(NULL);
}



// 定时器回调函数，用于更新变量显示
void vVariableUpdateTimerCallback(TimerHandle_t xTimer) {
    NotifyMenuFromISR();
}

// 创建定时器
void create_listening_variable_timer(void) {
    if (xVariableUpdateTimer == NULL) {
        xVariableUpdateTimer = xTimerCreate(
            "VariableTimer",
            pdMS_TO_TICKS(VARIABLE_UPDATE_PERIOD_MS),
            pdTRUE,  // 自动重载，周期性触发
            NULL,
            vVariableUpdateTimerCallback
        );
    }
}
 
// 启动定时器
void start_listening_variable_timer(void) {
    if (xVariableUpdateTimer != NULL) {
        xTimerStart(xVariableUpdateTimer, 0);
    }
}

// 停止定时器
void stop_listening_variable_timer(void) {
    if (xVariableUpdateTimer != NULL) {
        xTimerStop(xVariableUpdateTimer, 0);
    }
}

/**
 * @brief 在中断服务程序中通知指定任务
 * @param taskHandle 要通知的任务句柄
 * @note 此函数只能在 ISR 中调用
 */
void NotifyMenuFromISR(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    // 向指定任务发送通知
    vTaskNotifyGiveFromISR(xOLEDTaskHandle, &xHigherPriorityTaskWoken);
    
    // 根据优先级情况决定是否需要任务切换
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// 实现初始化函数
void init_menu_node(MenuNode *node, const char *name, MenuCallback callback, MenuType type, MenuNode *parent, uint8_t child_count, MenuNode **children) {
    node->current_index = 0;
    node->window_start_index = 0;
    node->name = name;
    node->callback = callback;
    node->type = type;
    node->parent = parent;
    node->child_count = child_count;
    memset(node->children, 0, sizeof(node->children));
    if (children != NULL && child_count > 0) {
        for (uint8_t i = 0; i < child_count && i < MAX_CHILD; i++) {
            node->children[i] = children[i];
        }
    }
}
