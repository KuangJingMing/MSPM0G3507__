#ifndef BUTTON_USER_H__
#define BUTTON_USER_H__

#include "multi_button.h"

typedef enum {
	BUTTON_UP = 0,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_MIDDLE
} BUTTON_ID;


void user_button_init(BtnCallback cb);

extern struct Button btn1, btn2, btn3, btn4, btn5;

#endif
