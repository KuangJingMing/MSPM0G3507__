#ifndef OLED_MENU_H
#define OLED_MENU_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_CHILD 10
#define MENU_LINE_HEIGHT 13

typedef void (*MenuCallback)(void *user_data);

// 定义菜单节点的属性，便于批量初始化
typedef struct {
    const char *name;
    MenuCallback callback;
    struct MenuNode *parent;
    uint8_t child_count;
    struct MenuNode **children; // 动态分配或静态数组
} MenuNodeConfig;

/**
 * @brief 菜单节点结构体，表示一个菜单项（可能有子菜单）
 */
typedef struct MenuNode {
		int current_index;
		int window_start_index;
    const char     *name;													// 菜单项显示名称
    MenuCallback    callback;                     // 此菜单项对应的回调，可以执行相应任务，渲染界面
    struct MenuNode *parent;                      // 指向父菜单节点，根节点为NULL
    struct MenuNode *children[MAX_CHILD];         // 子菜单节点指针数组
    uint8_t         child_count;                  // 当前有效子节点数
} MenuNode;

void create_oled_menu(void);

#endif // OLED_MENU_H