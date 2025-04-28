#ifndef __OLED_H__
#define __OLED_H__


#include "stdint.h"
#include "delay.h"
#include "spi.h"
#include "u8g2.h"

#define OLED_RST_Clr() DL_GPIO_clearPins(OLED_SPI_PORT, OLED_SPI_RST_OLED_PIN)
#define OLED_RST_Set() DL_GPIO_setPins(OLED_SPI_PORT, OLED_SPI_RST_OLED_PIN)
#define OLED_DC_Clr() DL_GPIO_clearPins(OLED_SPI_PORT, OLED_SPI_DC_OLED_PIN)
#define OLED_DC_Set() DL_GPIO_setPins(OLED_SPI_PORT, OLED_SPI_DC_OLED_PIN)
#define OLED_CS_Clr()  DL_GPIO_clearPins(OLED_SPI_PORT, OLED_SPI_CS_OLED_PIN)
#define OLED_CS_Set()  DL_GPIO_setPins(OLED_SPI_PORT, OLED_SPI_CS_OLED_PIN)

#define OLED_SDA_Clr() DL_GPIO_clearPins(GPIO_SPI_0_PICO_PORT, GPIO_SPI_0_PICO_PIN)
#define OLED_SDA_Set() DL_GPIO_setPins(GPIO_SPI_0_PICO_PORT, GPIO_SPI_0_PICO_PIN)

#define OLED_SCL_Clr() DL_GPIO_clearPins(GPIO_SPI_0_SCLK_PORT, GPIO_SPI_0_SCLK_PIN)
#define OLED_SCL_Set() DL_GPIO_setPins(GPIO_SPI_0_SCLK_PORT, GPIO_SPI_0_SCLK_PIN)

void u8g2_Init(void);
void show_oled_opening_animation(void);

extern u8g2_t u8g2;

#endif 


