#include "oled_menu.h"
#include "oled_driver.h"
#include "button_user.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "u8g2.h"
#define MAX_INDEX_COUNT 4

static void draw_centered_text(const char* text, uint8_t draw_border);
static inline void btn_single_click_callback(void* btn);
static TaskHandle_t xOLEDTaskHandle = NULL;

static void run_task01(void *arg) {
    u8g2_ClearBuffer(&u8g2);
    draw_centered_text("Running Task 01", 1);
    u8g2_SendBuffer(&u8g2);
}

static void run_task02(void *arg) {
    u8g2_ClearBuffer(&u8g2);
    draw_centered_text("Running Task 02", 1);
    u8g2_SendBuffer(&u8g2);
}

static void run_task03(void *arg) {
    u8g2_ClearBuffer(&u8g2);
    draw_centered_text("Running Task 03", 1);
    u8g2_SendBuffer(&u8g2);
}

static void run_task04(void *arg) {
    u8g2_ClearBuffer(&u8g2);
    draw_centered_text("Running Task 04", 1);
    u8g2_SendBuffer(&u8g2);
}

// 简化菜单节点初始化函数
static void init_menu_node(MenuNode *node, MenuNodeConfig config) {
    node->current_index = 0;
    node->window_start_index = 0;
    node->name = config.name;
    node->callback = config.callback;
    node->parent = config.parent;
    node->child_count = config.child_count;
    memset(node->children, 0, sizeof(node->children)); // 初始化子节点数组为 NULL
    if (config.children != NULL && config.child_count > 0) {
        for (uint8_t i = 0; i < config.child_count && i < MAX_CHILD; i++) {
            node->children[i] = config.children[i];
        }
    }
}

// 定义菜单节点（静态分配）
// 按照层级分组定义，便于查看层级关系
// 根节点 (Level 0)
static MenuNode menu_root;

// 第一级子菜单 (Level 1)
static MenuNode menu_run_tasks;
static MenuNode menu_show_status;
static MenuNode set_pid_speed;
static MenuNode set_pid_mileage;
static MenuNode play_games;

// 第二级子菜单 (Level 2, Run Tasks 的子菜单)
static MenuNode task01;
static MenuNode task02;
static MenuNode task03;
static MenuNode task04;

// 子节点数组，用于初始化父节点的 children
// Level 2 子节点数组 (Run Tasks 的子菜单)
static MenuNode *run_tasks_children[] = {
    &task01,
    &task02,
    &task03,
    &task04
};

// Level 1 子节点数组 (Main Menu 的子菜单)
static MenuNode *root_children[] = {
    &menu_run_tasks,
    &menu_show_status,
    &set_pid_speed,
    &set_pid_mileage,
    &play_games
};

static MenuNode* current_menu = &menu_root;

// 初始化所有节点
// 使用缩进和注释清晰表示层级关系
static void init_all_menu_nodes(void) {
    // Level 0: 根节点
    init_menu_node(&menu_root, (MenuNodeConfig){
        .name = "Main Menu",
        .callback = NULL,
        .parent = NULL,
        .child_count = 5,
        .children = root_children
    });

    // Level 1: 根节点的子菜单
    init_menu_node(&menu_run_tasks, (MenuNodeConfig){
        .name = "Run Tasks",
        .callback = NULL,
        .parent = &menu_root,
        .child_count = 4,
        .children = run_tasks_children
    });
    init_menu_node(&menu_show_status, (MenuNodeConfig){
        .name = "Show Status",
        .callback = NULL,
        .parent = &menu_root,
        .child_count = 0,
        .children = NULL
    });
    init_menu_node(&set_pid_speed, (MenuNodeConfig){
        .name = "Set Pid Speed",
        .callback = NULL,
        .parent = &menu_root,
        .child_count = 0,
        .children = NULL
    });
    init_menu_node(&set_pid_mileage, (MenuNodeConfig){
        .name = "Set Pid Mileage",
        .callback = NULL,
        .parent = &menu_root,
        .child_count = 0,
        .children = NULL
    });
    init_menu_node(&play_games, (MenuNodeConfig){
        .name = "Play Game",
        .callback = NULL,
        .parent = &menu_root,
        .child_count = 0,
        .children = NULL
    });

    // Level 2: Run Tasks 的子菜单
    init_menu_node(&task01, (MenuNodeConfig){
        .name = "run Task01",
        .callback = run_task01,
        .parent = &menu_run_tasks,
        .child_count = 0,
        .children = NULL
    });
    init_menu_node(&task02, (MenuNodeConfig){
        .name = "run Task02",
        .callback = run_task02,
        .parent = &menu_run_tasks,
        .child_count = 0,
        .children = NULL
    });
    init_menu_node(&task03, (MenuNodeConfig){
        .name = "run Task03",
        .callback = run_task03,
        .parent = &menu_run_tasks,
        .child_count = 0,
        .children = NULL
    });
    init_menu_node(&task04, (MenuNodeConfig){
        .name = "run Task04",
        .callback = run_task04,
        .parent = &menu_run_tasks,
        .child_count = 0,
        .children = NULL
    });
}

/**
 * @brief 在 OLED 上居中显示一串文本，并可选地绘制边框
 * @param text 要显示的文本
 * @param draw_border 是否绘制边框，1 为绘制，0 为不绘制
 */
static void draw_centered_text(const char* text, uint8_t draw_border) {
    // 计算文字居中的位置
    uint8_t text_width = u8g2_GetStrWidth(&u8g2, text);
    uint8_t x = (u8g2_GetDisplayWidth(&u8g2) - text_width) / 2;
    uint8_t y = u8g2_GetDisplayHeight(&u8g2) / 2;

    // 绘制文本
    u8g2_DrawStr(&u8g2, x, y, text);

    // 如果需要，绘制边框
    if (draw_border) {
        u8g2_DrawFrame(&u8g2, x - 10, y - 12, text_width + 20, 20);
    }
}

static void draw_menu(void) {	
    u8g2_ClearBuffer(&u8g2);
    // 设置字体为英文小字体
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tr);

    // 检查当前界面类型
    if (current_menu->child_count == 0 && current_menu->callback == NULL) {
        // 无子菜单且无回调，显示静态提示界面
        draw_centered_text("Nothing Here!", 1); // 1 表示绘制边框
    } else {
        // 绘制当前菜单标题（位于屏幕顶部，居中显示）
        uint8_t title_width = u8g2_GetStrWidth(&u8g2, current_menu->name);
        uint8_t title_x = (u8g2_GetDisplayWidth(&u8g2) - title_width) / 2; // 计算居中位置
        u8g2_DrawStr(&u8g2, title_x, 8, current_menu->name); // 显示当前菜单名称作为标题

        // 如果有子菜单，绘制菜单项
        if (current_menu->child_count > 0) {
            for (int i = 0; i < MAX_INDEX_COUNT && (i + current_menu->window_start_index) < current_menu->child_count; ++i) {
                int y = 20 + i * MENU_LINE_HEIGHT; // 起始位置下移，为标题留空间
                
                if ((i + current_menu->window_start_index) == current_menu->current_index) {
                    // 绘制选中项的边框
                    int y1 = y - 9;
                    int width = 128;
                    int height = MENU_LINE_HEIGHT - 2;
                    
                    // 绘制实心边框
                    u8g2_DrawFrame(&u8g2, 0, y1, width, height); // 使用 DrawFrame 代替四条线
                    
                    // 绘制指示箭头
                    u8g2_DrawStr(&u8g2, 2, y, ">");
                    u8g2_DrawStr(&u8g2, 10, y, current_menu->children[i + current_menu->window_start_index]->name);
                } else {
                    // 绘制非选中项
                    u8g2_DrawStr(&u8g2, 10, y, current_menu->children[i + current_menu->window_start_index]->name);
                }
            }
            
            // 绘制滚动指示器（如果需要）
            if (current_menu->window_start_index > 0) {
                // 上滚动指示器（放大且位置稍低）
                u8g2_DrawTriangle(&u8g2, 124, 14, 120, 20, 128, 20); // 调整位置，避免与标题冲突
            }
            
            if (current_menu->window_start_index + MAX_INDEX_COUNT < current_menu->child_count) {
                // 下滚动指示器（放大且确保在屏幕内）
                u8g2_DrawTriangle(&u8g2, 124, 60, 120, 54, 128, 54);
            }
        }
    }
    
    u8g2_SendBuffer(&u8g2);
}

static void excute_callback(void) {
	if (current_menu->callback != NULL) current_menu->callback(NULL);
}

static void menu_init(void) {
	u8g2_Init();
	init_all_menu_nodes();
  show_oled_opening_animation();
  user_button_init(&btn_single_click_callback);    
  draw_menu();
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

static void enter_current(void) {
	if (current_menu->child_count == 0) return;
	current_menu = current_menu->children[current_menu->current_index];
}

static void return_previous(void) {
	if (current_menu->parent == NULL) return;
	current_menu = current_menu->parent;
}

static void vOLEDTask(void *pvParameters)
{
		menu_init();
    for ( ; ; ) {
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // 等待通知、收到自动清0
      draw_menu();
			excute_callback();
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
