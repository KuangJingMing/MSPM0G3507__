/*
 * oled.c
 *
 *  Created on: 2022年7月24日
 *      Author: Unicorn_Li
 */
#include "oled.h"
#include "uart_driver.h"
#include "software_i2c.h"
#include "spi.h"

/**********************************************************
 * 初始化命令,根据芯片手册书写，详细步骤见上图以及注意事项
 ***********************************************************/
#define OLED_I2C_ADDRESS 0x3C

SoftwareI2C oled_i2c = {
		#ifdef I2C_OLED
	  .sclPort = OLED_PORT,
    .sdaPort = OLED_PORT,
    .sclPin = OLED_SCL2_PIN,
    .sdaPin = OLED_SDA2_PIN,
    .sclIOMUX = OLED_SCL2_IOMUX,
    .sdaIOMUX = OLED_SDA2_IOMUX,
	  .delay_us = 0,
	  .timeout_us = 1000          // 超时时间
	 #endif
 };


void OLED_WR_CMD(uint8_t cmd)
{
		#ifdef I2C_OLED
			uint8_t datas[1] = {cmd}; // 只需要一个字节
			SoftWareI2C_Write_Len(&oled_i2c, OLED_I2C_ADDRESS, 0x00, 1, datas);
		#elif defined(SPI_OLED)
			SPI_LCD_WrCmd(cmd);
		#endif
}

/**
 * @function: void OLED_WR_DATA(uint8_t data)
 * @description: 向设备写控制数据
 * @param {uint8_t} data 数据
 * @return {*}
 */
void OLED_WR_DATA(uint8_t data)
{
		#ifdef I2C_OLED
			uint8_t datas[1] = {data}; // 只需要一个字节
			SoftWareI2C_Write_Len(&oled_i2c, OLED_I2C_ADDRESS, 0x40, 1, datas);
		#elif defined(SPI_OLED)
			SPI_LCD_WrDat(data);
		#endif
}