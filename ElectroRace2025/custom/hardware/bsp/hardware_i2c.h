#ifndef HARDWARE_I2C_H
#define HARDWARE_I2C_H

#include <stdint.h> // 包含 uint8_t 等类型定义
#include "ti_msp_dl_config.h" // 包含 I2C_Regs 类型定义和 I2C_0_INST 等实例定义

// 声明 I2C_Regs 结构体，如果 ti_msp_dl_config.h 没有直接定义
// 通常 ti_msp_dl_config.h 会通过包含其他 DriverLib 头文件来定义
// 如果编译时报错找不到 I2C_Regs，可能需要在这里或者其他 DriverLib 头文件中找到它的完整定义并包含进来
// 或者确保 ti_msp_dl_config.h 已经包含了正确的头文件。
// 例如，可能需要类似这样的包含：
// #include "ti/devices/msp/mspm0g/driverlib/dl_i2c.h"


#ifdef __cplusplus
extern "C" {
#endif

#if (i2c_miss_up_enable == 1)
/**
 * @brief 通过I2C写入寄存器数据（带超时机制）
 *
 * @param i2c 指向I2C外设寄存器结构的指针
 * @param DevAddr I2C设备地址
 * @param reg_addr 寄存器地址
 * @param reg_data 待写入数据的指针
 * @param count 待写入数据的字节数
 */
void I2C_WriteReg(I2C_Regs *i2c, uint8_t DevAddr, uint8_t reg_addr, uint8_t *reg_data, uint8_t count);

/**
 * @brief 通过I2C读取寄存器数据（带超时机制）
 *
 * @param i2c 指向I2C外设寄存器结构的指针
 * @param DevAddr I2C设备地址
 * @param reg_addr 寄存器地址
 * @param reg_data 存储读取数据的缓冲区指针
 * @param count 待读取数据的字节数
 */
void I2C_ReadReg(I2C_Regs *i2c, uint8_t DevAddr, uint8_t reg_addr, uint8_t *reg_data, uint8_t count);

#else

/**
 * @brief 通过I2C写入寄存器数据（不带超时机制）
 *
 * @param i2c 指向I2C外设寄存器结构的指针
 * @param DevAddr I2C设备地址
 * @param reg_addr 寄存器地址
 * @param reg_data 待写入数据的指针
 * @param count 待写入数据的字节数
 */
void I2C_WriteReg(I2C_Regs *i2c, uint8_t DevAddr, uint8_t reg_addr, uint8_t *reg_data, uint8_t count);

/**
 * @brief 通过I2C读取寄存器数据（不带超时机制）
 *
 * @param i2c 指向I2C外设寄存器结构的指针
 * @param DevAddr I2C设备地址
 * @param reg_addr 寄存器地址
 * @param reg_data 存储读取数据的缓冲区指针
 * @param count 待读取数据的字节数
 */
void I2C_ReadReg(I2C_Regs *i2c, uint8_t DevAddr, uint8_t reg_addr, uint8_t *reg_data, uint8_t count);

#endif // i2c_miss_up_enable


/**
 * @brief 通过I2C向设备单字节写入数据
 *
 * @param i2c 指向I2C外设寄存器结构的指针
 * @param SlaveAddress I2C设备地址
 * @param REG_Address 寄存器地址
 * @param REG_data 待写入的单字节数据
 */
void single_writei2c(I2C_Regs *i2c, unsigned char SlaveAddress, unsigned char REG_Address, unsigned char REG_data);

/**
 * @brief 通过I2C从设备单字节读取数据
 *
 * @param i2c 指向I2C外设寄存器结构的指针
 * @param SlaveAddress I2C设备地址
 * @param REG_Address 寄存器地址
 * @return unsigned char 读取到的单字节数据
 */
unsigned char single_readi2c(I2C_Regs *i2c, unsigned char SlaveAddress, unsigned char REG_Address);

/**
 * @brief 通过I2C从设备读取指定字节数的数据
 *
 * @param i2c 指向I2C外设寄存器结构的指针
 * @param addr I2C设备地址
 * @param regAddr 寄存器地址
 * @param data 存储读取数据的缓冲区指针
 * @param length 待读取数据的字节数
 */
void i2creadnbyte(I2C_Regs *i2c, uint8_t addr, uint8_t regAddr, uint8_t *data, uint8_t length);


#ifdef __cplusplus
}
#endif

#endif // HARDWARE_I2C_H
