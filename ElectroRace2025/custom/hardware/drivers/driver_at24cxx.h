#ifndef _BSP_EEPROM_H__
#define _BSP_EEPROM_H__


#include "math.h"
#include "stdbool.h"
#include "stdint.h"

#define AT24C01 127
#define AT24C02 255
#define AT24C04 511
#define AT24C08 1023
#define AT24C16 2047
#define AT24C32 4095
#define AT24C64 8191
#define AT24C128 16383
#define AT24C256 32767

#define EE_TYPE AT24C16						    //AT24C16
#define AT24C0X_IIC_ADDRESS  0xA0     //1 0 1 0 A2 A1 A0 R/W 八位地址

#define AT24C0X_IIC_BASE_ADDR   0x50	//七位地址
#define AT24CXX_BLOCK(addr)    (((addr) / 256) & 0x07)   // 取高位页号



void AT24CXX_Init(void);
uint8_t AT24CXX_Check(void);
void AT24CXX_Erase_All(void);
void AT24CXX_WriteOneByte(uint16_t WriteAddr, uint8_t DataToWrite);
uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr);
void AT24CXX_Read(uint16_t ReadAddr, uint8_t *pBuffer, uint16_t NumToRead);
void AT24CXX_Write(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumToWrite);


												 

												 
#endif



												 