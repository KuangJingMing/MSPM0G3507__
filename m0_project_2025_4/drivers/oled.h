#ifndef __OLED_H__
#define __OLED_H__

#include "stdarg.h"
#include "stdio.h"
#include "oled_data.h"
#include "delay.h"
#include "stdlib.h"

//I2C驱动
//#define I2C_OLED
//SPI驱动
#define SPI_OLED

#define OLED_MAX_BUFFER_SIZE 256

void OLED_Test(void);
void OLED_WR_CMD(uint8_t cmd);
void OLED_WR_DATA(uint8_t data);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_On(void);
void OLED_ShowNum(uint8_t x,uint8_t y,unsigned int num,uint8_t len,uint8_t size2,uint8_t Color_Turn);
void OLED_Showdecimal(uint8_t x,uint8_t y,float num,uint8_t z_len,uint8_t f_len,uint8_t size2, uint8_t Color_Turn);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size,uint8_t Color_Turn);
void OLED_ShowString(uint8_t x,uint8_t y,char*chr,uint8_t Char_Size,uint8_t Color_Turn);
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no,uint8_t Color_Turn);
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t *  BMP,uint8_t Color_Turn);
void OLED_HorizontalShift(uint8_t direction);
void OLED_Some_HorizontalShift(uint8_t direction,uint8_t start,uint8_t end);
void OLED_VerticalAndHorizontalShift(uint8_t direction);
void OLED_DisplayMode(uint8_t mode);
void OLED_IntensityControl(uint8_t intensity);
void OLED_Printf(uint8_t x, uint8_t y, uint8_t Char_Size, uint8_t Color_Turn, const char* format, ...);


#endif /* OLED_OLED_H_ */

