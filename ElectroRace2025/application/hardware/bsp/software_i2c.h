#ifndef __SOFTWARE_I2C_H__
#define __SOFTWARE_I2C_H__

#include "ti_msp_dl_config.h"
#include "delay.h"

// 错误码定义
typedef enum {
    I2C_SUCCESS = 0,            // 操作成功
    I2C_ERROR_TIMEOUT,          // 超时错误
    I2C_ERROR_ACK_FAIL,         // ACK 失败
    I2C_ERROR_INVALID_PARAM,    // 参数错误
    I2C_ERROR_UNKNOWN           // 未知错误
} SoftwareI2C_ErrorCode;

typedef struct {
    GPIO_Regs* sdaPort;         // SDA端口 (如 GPIOB)
    uint32_t sdaPin;            // SDA引脚（如 DL_GPIO_PIN_3）
    uint32_t sdaIOMUX;          // SDA IOMUX配置
    GPIO_Regs* sclPort;         // SCL端口 (如 GPIOB)
    uint32_t sclPin;            // SCL引脚（如 DL_GPIO_PIN_4）
    uint32_t sclIOMUX;          // SCL IOMUX配置
    uint32_t delay_us;          // 延时时间（微秒）
    uint32_t timeout_us;        // 超时时间（微秒）
} SoftwareI2C;

// GPIO操作宏定义
#define SET_PIN(port, pin)          DL_GPIO_setPins(port, pin)
#define CLEAR_PIN(port, pin)        DL_GPIO_clearPins(port, pin)
#define READ_PIN(port, pin)         DL_GPIO_readPins(port, pin)
#define INIT_OUTPUT(iomux)          DL_GPIO_initDigitalOutput(iomux)
#define INIT_INPUT(iomux)           DL_GPIO_initDigitalInputFeatures(iomux, \
                                    DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP, \
                                    DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE)

// 函数声明，保持原有签名
void SoftwareI2C_Init(SoftwareI2C* i2c);
void SoftwareI2C_SDA_OUT(SoftwareI2C* i2c);
void SoftwareI2C_SDA_IN(SoftwareI2C* i2c);
void SoftwareI2C_WriteSCL(SoftwareI2C* i2c, uint8_t BitValue);
void SoftwareI2C_WriteSDA(SoftwareI2C* i2c, uint8_t BitValue);
uint8_t SoftwareI2C_ReadSDA(SoftwareI2C* i2c);
void SoftwareI2C_Start(SoftwareI2C* i2c);
void SoftwareI2C_Stop(SoftwareI2C* i2c);
void SoftwareI2C_SendByte(SoftwareI2C* i2c, uint8_t Byte);
uint8_t SoftwareI2C_ReceiveByte(SoftwareI2C* i2c);
void SoftwareI2C_SendAck(SoftwareI2C* i2c, uint8_t AckBit);
uint8_t SoftwareI2C_ReceiveAck(SoftwareI2C* i2c);
unsigned char SoftwareI2C_WaitAck(SoftwareI2C* i2c);
uint8_t SoftWareI2C_Write_Len(SoftwareI2C* i2c, uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);
uint8_t SoftWareI2C_Read_Len(SoftwareI2C* i2c, uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);

#endif