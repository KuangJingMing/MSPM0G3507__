#ifndef __OLED_H__
#define __OLED_H__


#include "stdint.h"
#include "delay.h"
#include "spi.h"
#include "u8g2.h"

#define OLED_RST_Clr() DL_GPIO_clearPins(PORTB_PORT, PORTB_RST_PIN)
#define OLED_RST_Set() DL_GPIO_setPins(PORTB_PORT, PORTB_RST_PIN)
#define OLED_DC_Clr() DL_GPIO_clearPins(PORTB_PORT, PORTB_DC_PIN)
#define OLED_DC_Set() DL_GPIO_setPins(PORTB_PORT, PORTB_DC_PIN)
#define OLED_CS_Clr()  DL_GPIO_clearPins(PORTB_PORT, PORTB_CS_PIN)
#define OLED_CS_Set()  DL_GPIO_setPins(PORTB_PORT, PORTB_CS_PIN)

void u8g2_Init(void);
void vOLEDOpeningAnimation(void);

extern u8g2_t u8g2;

#endif 


