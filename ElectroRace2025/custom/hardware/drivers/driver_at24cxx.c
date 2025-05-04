#include "driver_at24cxx.h" 
#include "log_config.h"
#include "log.h"
#include "ti_msp_dl_config.h"
#include "delay.h"
#include "sw_i2c.h"

#include "log_config.h"
#include "log.h"

// 实例化一个 sw_i2c_t 结构体，用于您的 IIC 外设
const sw_i2c_t at24cxx_i2c = {
    .sdaPort = EPM_PORT,      // 替换为您的 SDA 端口
    .sdaPin = EPM_EPM_SDA_PIN, // 替换为您的 SDA 引脚
    .sdaIOMUX = EPM_EPM_SDA_IOMUX, // 替换为您的 SDA IOMUX 配置
    .sclPort = EPM_PORT,      // 替换为您的 SCL 端口
    .sclPin = EPM_EPM_SCL_PIN, // 替换为您的 SCL 引脚
    .sclIOMUX = EPM_EPM_SCL_IOMUX, // 替换为您的 SCL IOMUX 配置
};

// 初始化IIC接口
void AT24CXX_Init(void)
{
    SOFT_IIC_Init(&at24cxx_i2c);
    int attempts = 0;
		static uint8_t at24cxx_initialized_ok;
    const int max_attempts = 800;
    const uint32_t delay_ms_between_attempts = 20;
    while(attempts < max_attempts)
    {
        if (AT24CXX_Check() == 0) { // 假设 0 表示成功
            at24cxx_initialized_ok = 1; // 设置初始化成功标志
            break; // 检测成功，跳出循环
        }
        attempts++;
        delay_ms(delay_ms_between_attempts);
    }
    if (at24cxx_initialized_ok != 1) {
        // EEPROM 检测失败，进行错误处理
        at24cxx_initialized_ok = 2; // 设置初始化失败标志
        // TODO: Add error logging or reporting here
        log_e("AT24C02 initialization failed after %d attempts.", max_attempts);
    } else {
        // EEPROM 初始化成功
        log_i("AT24C02 initialized successfully.");
    }
    // 如果需要，取消注释进行擦除
    // if (at24cxx_initialized_ok == 1) {
    //     AT24CXX_Erase_All();
    // }
}


// Read one byte from AT24CXX
uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr)
{
    uint8_t temp = 0;
    uint8_t dev_addr = AT24C0X_IIC_BASE_ADDR;
    uint16_t reg_addr = ReadAddr;
    if(EE_TYPE <= AT24C16) { // 小容量，页号塞到地址高3bit（8页支持16Kbit）
        dev_addr = AT24C0X_IIC_BASE_ADDR + AT24CXX_BLOCK(ReadAddr);
        reg_addr = ReadAddr & 0xFF;
    }
    SOFT_IIC_Read_Len(&at24cxx_i2c, dev_addr, reg_addr, 1, &temp);
    return temp;
}

// Write one byte to AT24CXX
void AT24CXX_WriteOneByte(uint16_t WriteAddr, uint8_t DataToWrite)
{
    uint8_t dev_addr = AT24C0X_IIC_BASE_ADDR;
    uint16_t reg_addr = WriteAddr;
    if(EE_TYPE <= AT24C16) { // 小容量
        dev_addr = AT24C0X_IIC_BASE_ADDR + AT24CXX_BLOCK(WriteAddr);
        reg_addr = WriteAddr & 0xFF;
    }
    SOFT_IIC_Write_Len(&at24cxx_i2c, dev_addr, reg_addr, 1, (uint8_t*)&DataToWrite);
    delay_ms(10);
}

void AT24CXX_Erase_All(void)
{
  for(uint16_t i = 0; i <= EE_TYPE; i++)
  {
    AT24CXX_WriteOneByte(i, 0xff);
    delay_ms(10);
  }
}

uint8_t at24cxx_debug[2048];

void AT24CXX_Read_All(void)
{
  for(uint16_t i = 0; i <= EE_TYPE; i++)
  {
    at24cxx_debug[i] = AT24CXX_ReadOneByte(i);
  }
}

//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AT24CXX_WriteLenByte(uint16_t WriteAddr, u32 DataToWrite, uint8_t Len)
{
  uint8_t t;

  for (t = 0; t < Len; t++)
  {
    AT24CXX_WriteOneByte(WriteAddr + t, (DataToWrite >> (8 * t)) & 0xff);
  }
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 AT24CXX_ReadLenByte(uint16_t ReadAddr, uint8_t Len)
{
  uint8_t t;
  u32 temp = 0;

  for (t = 0; t < Len; t++)
  {
    temp <<= 8;
    temp += AT24CXX_ReadOneByte(ReadAddr + Len - t - 1);
  }

  return temp;
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(uint16_t ReadAddr, uint8_t *pBuffer, uint16_t NumToRead)
{
  while (NumToRead)
  {
    *pBuffer++ = AT24CXX_ReadOneByte(ReadAddr++);
    NumToRead--;
  }
}

//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24CXX_Write(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumToWrite)
{
  while (NumToWrite--)
  {
    AT24CXX_WriteOneByte(WriteAddr, *pBuffer);
    WriteAddr++;
    pBuffer++;
  }
}

//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
#define at24cx_check_address 1600//1600  240
#define at24cx_end_address 	 2047//2047  255 

uint8_t AT24CXX_Check(void)
{
  volatile uint8_t temp;
  temp = AT24CXX_ReadOneByte(at24cx_end_address); //避免每次开机都写AT24CXX

  if (temp == 0X55)	return 0;
  else //排除第一次初始化的情况
  {
    AT24CXX_Erase_All();//先全部擦除
    AT24CXX_WriteOneByte(at24cx_end_address, 0X55);
    temp = AT24CXX_ReadOneByte(at24cx_end_address);

    if (temp == 0X55)	return 0;
  }

  return 1;
}



