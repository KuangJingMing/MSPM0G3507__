#ifndef OLED_MENU_H
#define OLED_MENU_H

#include <stdint.h>
#include <stdbool.h>

static inline void btn_single_click_callback(void* btn);
static inline void btn_long_press_cb(void *btn);

void menu_init(void);

#endif // OLED_MENU_H