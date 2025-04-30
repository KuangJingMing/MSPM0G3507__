#ifndef _W25QXX_H__
#define _W25QXX_H__
#include "stdint.h"

//指令表
#define W25X_WriteEnable				0x06 
#define W25X_WriteDisable				0x04 
#define W25X_ReadStatusReg1			0x05 
#define W25X_ReadStatusReg2			0x35 
#define W25X_ReadStatusReg3			0x15 
#define W25X_WriteStatusReg1    0x01 
#define W25X_WriteStatusReg2    0x31 
#define W25X_WriteStatusReg3    0x11 
#define W25X_ReadData						0x03 
#define W25X_FastReadData				0x0B 
#define W25X_FastReadDual				0x3B 
#define W25X_PageProgram				0x02 
#define W25X_BlockErase					0xD8 
#define W25X_SectorErase				0x20 
#define W25X_ChipErase					0xC7 
#define W25X_PowerDown					0xB9 
#define W25X_ReleasePowerDown		0xAB 
#define W25X_DeviceID						0xAB 
#define W25X_ManufactDeviceID		0x90 
#define W25X_JedecDeviceID			0x9F 
#define W25X_Enable4ByteAddr    0xB7
#define W25X_Exit4ByteAddr      0xE9

//W25X系列/Q系列芯片列表	   
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17
#define W25Q256 0XEF18



uint16_t W25Q64_readID(void);


void w25qxx_gpio_init(void);
	

void W25QXX_Read_f(float *data, uint32_t ui32Address, uint32_t ui32Count);
void W25QXX_Write_f(float *data, uint32_t ui32Address, uint32_t ui32Count);


void w25qxx_erase_all(void);
	

extern uint16_t w25qxx_id;




//#define SPI_CLK0   DL_GPIO_clearPins(PORTA_PORT,   PORTA_W25Q_SCLK_PIN)		//PA12
//#define SPI_CLK1   DL_GPIO_setPins(PORTA_PORT,     PORTA_W25Q_SCLK_PIN)
//#define SPI_MOSI0  DL_GPIO_clearPins(PORTA_PORT,   PORTA_W25Q_MOSI_PIN)		//PA14
//#define SPI_MOSI1  DL_GPIO_setPins(PORTA_PORT,     PORTA_W25Q_MOSI_PIN)

//#define  SPI_MISO_IN (bool)((PORTA_PORT->DIN31_0 & PORTA_W25Q_MISO_PIN)==PORTA_W25Q_MISO_PIN)

//unsigned char spi_read_write_byte2(unsigned char data_in)
//{
//  unsigned char data = 0x00;
//  for(uint8_t i = 0; i < 8; i++)
//  {	
//		SPI_CLK0;
//		//delay_us(5);
//		if((data_in&(0x80 >> i))!=0) SPI_MOSI1;
//		else SPI_MOSI0;
//		//delay_us(5);
//    SPI_CLK1;
//		delay_us(2);
//    if(SPI_MISO_IN)
//    {
//      data = data | (0x80 >> i);
//    }
//  }
//	SPI_CLK0;
//  return data;
//}
//    while (DL_SPI_isBusy(SPI_0_INST));//等待发送区空
//    DL_SPI_transmitData8(SPI_0_INST, Byte);//通过外设SPIx发送一个数据
//    while(  DL_SPI_isRXFIFOEmpty(SPI_0_INST));
//    return DL_SPI_receiveData8(SPI_0_INST);//返回通过SPIx最近接收的数据

//  DL_SPI_fillTXFIFO8(SPI_0_INST,&Byte, 1);
//  while (DL_SPI_isBusy(SPI_0_INST)) ;
//  return DL_SPI_receiveDataBlocking8(SPI_0_INST);  



//uint16_t spi_retry_cnt = 0;
//#elif fast_spi_enable==2
//  spi_retry_cnt=0;
//	DL_SPI_transmitData8(SPI_0_INST, Byte);  //发送1字节命令
//	while(DL_SPI_isBusy(SPI_0_INST))        //等待发送完成
//	{
//    spi_retry_cnt++;
//    if(spi_retry_cnt > 200)	
//		{
//			return 0;//超时
//		}			
//	}
//	return DL_SPI_receiveData8(SPI_0_INST);  //清空接收FIFO中的数据
	
	

#endif






