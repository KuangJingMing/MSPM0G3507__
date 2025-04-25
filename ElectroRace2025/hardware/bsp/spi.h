#ifndef SPI_H__
#define SPI_H__

#include "ti_msp_dl_config.h"
#include "oled.h"

#ifdef SPI_OLED

#define OLED_RST_Clr() DL_GPIO_clearPins(PORTB_PORT, PORTB_RST_PIN)
#define OLED_RST_Set() DL_GPIO_setPins(PORTB_PORT, PORTB_RST_PIN)

#define OLED_DC_Clr() DL_GPIO_clearPins(PORTB_PORT, PORTB_DC_PIN)
#define OLED_DC_Set() DL_GPIO_setPins(PORTB_PORT, PORTB_DC_PIN)


#define OLED_CS_Clr()  DL_GPIO_clearPins(PORTB_PORT, PORTB_CS_PIN)
#define OLED_CS_Set()  DL_GPIO_setPins(PORTB_PORT, PORTB_CS_PIN)

void SPI_LCD_WrDat(unsigned char dat);
void SPI_LCD_WrCmd(unsigned char cmd);

#endif

#endif
