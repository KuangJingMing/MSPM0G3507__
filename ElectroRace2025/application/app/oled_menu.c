#include "oled_menu.h"
#include "oled_driver.h"
#include "button_user.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "u8g2.h"
#include "stdio.h"
#include "stdlib.h"


typedef struct menu_variables_t {
	const char *name;
	float value;
} menu_variables_t;

#define SHOW_OPENING_ANIMATION 1 	// 是否显示开场动画 1 / 0
#define MAX_INDEX_COUNT 4     		// 一次可以显示的最大窗口数量
#define LONG_PRESS_COUNTER 5  		// 按键长按的频率 越少越快

static void draw_centered_text(const char* text, uint8_t draw_border);
static inline void btn_single_click_callback(void* btn);
static inline void btn_long_press_cb(void *btn);
static TaskHandle_t xOLEDTaskHandle = NULL;
static void add_variable(const char *name, float val);
static void create_listening_variable_timer(void);
static void start_listening_variable_timer(void);
static void stop_listening_variable_timer(void);
static menu_variables_t menu_variables[MAX_INDEX_COUNT];
static void draw_variables_menu(void);

void show_oled_opening_animation(void)
{
    float angle = 0.0f;
    uint32_t startTime = xTaskGetTickCount();
    do {
        u8g2_ClearBuffer(&u8g2);

        // 计算已过ms
        uint32_t elapsedTime = (xTaskGetTickCount() - startTime) * portTICK_PERIOD_MS;

        // 1. --- MSPM0 波浪主标题 + “高亮字母动画” ---
        u8g2_SetFont(&u8g2, u8g2_font_inb16_mr);
        const char* text = "MSPM0";
        int textWidth = u8g2_GetStrWidth(&u8g2, text);

        // 每一帧随机一个字符用白底黑字高亮
        int highlight_i = ((int)(angle * 1.5f)) % 5;
        for (int i = 0, x = 64 - textWidth/2; text[i] != '\0'; i++)
        {
            char c[2] = {text[i], '\0'};
            int charWidth = u8g2_GetStrWidth(&u8g2, c);
            int y = 30 + 8 * sinf(angle + i * 0.7f);
            if(i == highlight_i) {
                // 椭圆高亮底框
                u8g2_SetDrawColor(&u8g2, 1);
                u8g2_DrawBox(&u8g2, x-2, y-15, charWidth+4, 19);
                u8g2_SetDrawColor(&u8g2, 0);
                u8g2_DrawStr(&u8g2, x, y, c);
                u8g2_SetDrawColor(&u8g2, 1);
            } else {
                u8g2_DrawStr(&u8g2, x, y, c);
            }
            x += charWidth;
        }

        // 2. --- 副标题 WELCOME，淡入 ---
        uint32_t fadein = elapsedTime < 1500 ? elapsedTime : 1500;
        float alpha = (float)fadein / 1500.f;
        u8g2_SetFont(&u8g2, u8g2_font_profont12_tr);
        const char* subtext = "Welcome";
        int subw = u8g2_GetStrWidth(&u8g2, subtext);
        // 点阵淡入，随机点分布
        for(int i=0,x=64-subw/2;subtext[i]!='\0';i++) {
            char c[2]={subtext[i],'\0'};
            int cw = u8g2_GetStrWidth(&u8g2,c);
            int y = 50 + 2*sinf(angle + i*0.5f);  //副标题也微微波动
            // 字符显示点阵按alpha填补，制造淡入
            if((i*13+y+(int)angle)%10 < (int)(10*alpha)) // 随机采样点数呈现淡入
                u8g2_DrawStr(&u8g2,x,y,c);
            x+=cw;
        }

        // 3. --- 顶部&底部波浪，交错+加星点 ---
        for (int x = 0; x < 128; x++)
        {
            int y1 = 60 + 3 * sinf(angle + x * 0.09f);
            u8g2_DrawPixel(&u8g2, x, y1);

            // 底部点星星
            if (((x*15 + (int)angle*8) % 113) == 0 && (elapsedTime%600)<400)
                u8g2_DrawPixel(&u8g2, x, y1-2);

            int y2 = 7 + 3 * sinf(angle + x * 0.1f + 3.14159f);
            u8g2_DrawPixel(&u8g2, x, y2);

            // 顶部点星星
            if (((x*9+(int)angle*11)%127)==0 && (elapsedTime%700)<300)
                u8g2_DrawPixel(&u8g2, x, y2+2);
        }

        // 4. --- 进度条波浪填充 ---
        int bar_len = (elapsedTime * 118) / 3000;
        if (bar_len > 118) bar_len = 118;
        u8g2_DrawFrame(&u8g2, 5, 62, 118, 2);
        // 用小波浪做进度条填充
        for(int i=0;i<bar_len;i++) {
            int y = 62 + (int)(sinf(angle+i*0.25f)*1.1f);
            u8g2_DrawPixel(&u8g2, 5+i, y);
            u8g2_DrawPixel(&u8g2, 5+i, 63); //下沿也补齐
        }
        u8g2_SendBuffer(&u8g2);
        angle += 0.2f;
        vTaskDelay(pdMS_TO_TICKS(50)); // 20FPS
    } while((xTaskGetTickCount() - startTime) * portTICK_PERIOD_MS < 2000);
    u8g2_ClearBuffer(&u8g2);
    u8g2_SendBuffer(&u8g2);
}

static void run_task01_cb(void *arg) {
    u8g2_ClearBuffer(&u8g2);
    draw_centered_text("Running Task 01", 1);
    u8g2_SendBuffer(&u8g2);
}

static void run_task02_cb(void *arg) {
    u8g2_ClearBuffer(&u8g2);
    draw_centered_text("Running Task 02", 1);
    u8g2_SendBuffer(&u8g2);
}

static void run_task03_cb(void *arg) {
    u8g2_ClearBuffer(&u8g2);
    draw_centered_text("Running Task 03", 1);
    u8g2_SendBuffer(&u8g2);
}

static void run_task04_cb(void *arg) {
    u8g2_ClearBuffer(&u8g2);
    draw_centered_text("Running Task 04", 1);
    u8g2_SendBuffer(&u8g2);
}

static void view_variables_cb(void *arg) {
		start_listening_variable_timer();
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
static MenuNode menu_view_variables;
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
    &menu_view_variables,
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
    init_menu_node(&menu_view_variables, (MenuNodeConfig){
        .name = "View Variables",
        .callback = view_variables_cb,
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
        .callback = run_task01_cb,
        .parent = &menu_run_tasks,
        .child_count = 0,
        .children = NULL
    });
    init_menu_node(&task02, (MenuNodeConfig){
        .name = "run Task02",
        .callback = run_task02_cb,
        .parent = &menu_run_tasks,
        .child_count = 0,
        .children = NULL
    });
    init_menu_node(&task03, (MenuNodeConfig){
        .name = "run Task03",
        .callback = run_task03_cb,
        .parent = &menu_run_tasks,
        .child_count = 0,
        .children = NULL
    });
    init_menu_node(&task04, (MenuNodeConfig){
        .name = "run Task04",
        .callback = run_task04_cb,
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


static void draw_variables_menu(void) {
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tr);

    // 绘制标题
    const char* title = "Variables";
    uint8_t title_width = u8g2_GetStrWidth(&u8g2, title);
    uint8_t title_x = (u8g2_GetDisplayWidth(&u8g2) - title_width) / 2;
    u8g2_DrawStr(&u8g2, title_x, 8, title);

    // 绘制变量列表
    for (uint8_t i = 0; i < MAX_INDEX_COUNT; i++) {
        if (menu_variables[i].name != NULL) {
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%s: %.2f", menu_variables[i].name, menu_variables[i].value);
            u8g2_DrawStr(&u8g2, 10, 20 + i * MENU_LINE_HEIGHT, buffer);
        }
    }

    u8g2_SendBuffer(&u8g2);
}

static void draw_menu(void) {	
    u8g2_ClearBuffer(&u8g2);
    // 设置字体为英文小字体
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tr);

    // 检查当前界面类型
		
		if (current_menu->callback != NULL) return; //如果当前的菜单有回调那么渲染的逻辑交给回调执行 
		
    if (current_menu->child_count == 0) {
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

float a = 1, b = 2, c = 3, d = 4;

static void menu_init(void) {
	u8g2_Init();
	init_all_menu_nodes();
	create_listening_variable_timer();
	stop_listening_variable_timer();
	add_variable("NAME1", a);
	add_variable("NAME2", b);
	add_variable("NAME3", c);
	add_variable("NAME4", d);
#if SHOW_OPENING_ANIMATION
  show_oled_opening_animation();
#endif
  user_button_init(&btn_single_click_callback, &btn_long_press_cb);    
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
			if (current_menu == &menu_view_variables) {
        draw_variables_menu();
			} else {
				stop_listening_variable_timer();
			}
    }
}

void create_oled_menu(void) {
    xTaskCreate(vOLEDTask, "OLED_MENU", 512, NULL, tskIDLE_PRIORITY + 1, &xOLEDTaskHandle);
}

static inline void btn_single_click_callback(void* btn)
{
    struct Button* button = (struct Button*) btn;
    if (button == &buttons[BUTTON_UP]) {  
			select_previous();
    } else if (button == &buttons[BUTTON_DOWN]) { 
			select_next();
    } else if (button == &buttons[BUTTON_LEFT]) {  
			enter_current(); 
    } else if (button == &buttons[BUTTON_RIGHT]) {
			return_previous();
    } else if (button == &buttons[BUTTON_MIDDLE]) {

    }
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(xOLEDTaskHandle, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static inline void btn_long_press_cb(void *btn) {
		static uint32_t count = 0;
		if ((++count % LONG_PRESS_COUNTER) != 0) return;
    struct Button* button = (struct Button*) btn;
		if (button == &buttons[BUTTON_UP]) {  
			select_previous();
    } else if (button == &buttons[BUTTON_DOWN]) { 
			select_next();
    } 
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(xOLEDTaskHandle, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void add_variable(const char *name, float val) {
    static uint8_t data_index = 0;
    if (data_index < MAX_INDEX_COUNT) {
        menu_variables[data_index].name = name;
        menu_variables[data_index].value = val;
        data_index++;
    }
}


static TimerHandle_t xVariableUpdateTimer = NULL;
#define VARIABLE_UPDATE_PERIOD_MS 1000  // 变量更新周期，1秒

// 定时器回调函数，用于更新变量显示
static void vVariableUpdateTimerCallback(TimerHandle_t xTimer) {
    // 简单示例：更新变量值（你可以根据实际需求修改）
		for (uint8_t i = 0; i < MAX_INDEX_COUNT; i++) {
				if (menu_variables[i].name != NULL) {
						// 生成一个 -1.0 到 1.0 的随机值
						float random_value = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
						menu_variables[i].value += random_value;
				}
		}
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(xOLEDTaskHandle, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// 创建定时器
static void create_listening_variable_timer(void) {
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
static void start_listening_variable_timer(void) {
    if (xVariableUpdateTimer != NULL) {
        xTimerStart(xVariableUpdateTimer, 0);
    }
}

// 停止定时器
static void stop_listening_variable_timer(void) {
    if (xVariableUpdateTimer != NULL) {
        xTimerStop(xVariableUpdateTimer, 0);
    }
}