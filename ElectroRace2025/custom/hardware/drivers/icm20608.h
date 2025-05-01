#ifndef __ICM20608_H
#define __ICM20608_H

// IMU ID 定义
#define IMU_ICM20689_ID  0x98
// IMU I2C 地址 (通常为 0x68 或 0x69)
#define ICM20689_ADRESS		0x68

#include "ti_msp_dl_config.h"
#include "data_type.h" // 确保包含 vector3f 的定义

// 陀螺仪量程转换系数 (将原始 LSB 转换为 deg/s)
// 需要根据 GYRO_CONFIG 寄存器设置的量程来确定
// 例如：对于 +/-500 deg/s 量程，灵敏度为 65.5 LSB/deg/s，则系数为 1 / 65.5 = 0.015267...
//#define GYRO_CALIBRATION_COFF 0.060976f  // 对应 +/-2000 deg/s 量程
//#define GYRO_CALIBRATION_COFF 0.030488f; // 对应 +/-1000 deg/s 量程
#define GYRO_CALIBRATION_COFF 0.0152672f  // 对应 +/-500 deg/s 量程 (请核对数据手册精确值)

// 地球重力加速度标准值 (m/s/s)
#define GRAVITY_MSS 9.80665f
// 加速度计量程对应的原始值 (例如 +/-4g 量程，灵敏度 8192 LSB/g)
// 需要根据 ACCEL_CONFIG 寄存器设置的量程来确定
#define GRAVITY_RAW 8192.0f // 对应 +/-4g 量程 (请核对数据手册精确值)

// 将原始加速度计值转换为单位 g (RAW / GRAVITY_RAW)
#define RAW_TO_G     (1.0f / GRAVITY_RAW) // 修改为直接的除法，更清晰
// 将单位 g 转换为原始加速度计值 (g * GRAVITY_RAW)
#define G_TO_RAW  	 GRAVITY_RAW

// IMU 寄存器地址定义
#define	SMPLRT_DIV		0x19 // 采样率分频寄存器
#define	MPU_CONFIG		0x1A // 配置寄存器 (DLPF 设置等)
#define	GYRO_CONFIG		0x1B // 陀螺仪配置寄存器 (量程和 DLPF)
#define	ACCEL_CONFIG  0x1C // 加速度计配置寄存器 (量程)
#define ACCEL_CONFIG2 0x1D // 加速度计配置寄存器 2 (DLPF)
#define	ACCEL_XOUT_H	0x3B // 加速度计 X 轴高 8 位
#define	ACCEL_XOUT_L	0x3C // 加速度计 X 轴低 8 位
#define	ACCEL_YOUT_H	0x3D // 加速度计 Y 轴高 8 位
#define	ACCEL_YOUT_L	0x3E // 加速度计 Y 轴低 8 位
#define	ACCEL_ZOUT_H	0x3F // 加速度计 Z 轴高 8 位
#define	ACCEL_ZOUT_L	0x40 // 加速度计 Z 轴低 8 位
#define	TEMP_OUT_H		0x41 // 温度高 8 位
#define	TEMP_OUT_L		0x42 // 温度低 8 位
#define	GYRO_XOUT_H		0x43 // 陀螺仪 X 轴高 8 位
#define	GYRO_XOUT_L		0x44 // 陀螺仪 X 轴低 8 位
#define	GYRO_YOUT_H		0x45 // 陀螺仪 Y 轴高 8 位
#define	GYRO_YOUT_L		0x46 // 陀螺仪 Y 轴低 8 位
#define	GYRO_ZOUT_H		0x47 // 陀螺仪 Z 轴高 8 位
#define	GYRO_ZOUT_L		0x48 // 陀螺仪 Z 轴低 8 位
#define	PWR_MGMT_1		0x6B // 电源管理寄存器 1
#define	WHO_AM_I		  0x75 // WHO_AM_I 寄存器 (芯片 ID)
#define USER_CTRL		  0x6A // 用户控制寄存器
#define INT_PIN_CFG		0x37 // 中断引脚配置寄存器

// 已知 IMU 芯片的 WHO_AM_I 值
typedef enum
{
	WHO_AM_I_MPU6050  =0x68,
	WHO_AM_I_ICM20689 =0x98,
	WHO_AM_I_ICM20608D=0xAE, // 假设为 0xAE，请根据数据手册确认
	WHO_AM_I_ICM20608G=0xAF, // 假设为 0xAF，请根据数据手册确认
	WHO_AM_I_ICM20602=0x12,  // 假设为 0x12，请根据数据手册确认
    WHO_AM_I_UNKNOWN  =0xFF, // 未知 ID
}IMU_ID_READ;

/**
 * @brief 初始化 ICM206xx 系列 IMU 传感器
 * @return uint8_t 0: 成功, 1: 失败
 */
uint8_t ICM206xx_Init(void);

/**
 * @brief 从 ICM206xx 传感器读取加速度计、陀螺仪和温度数据
 * @param gyro 指向存储陀螺仪数据的结构体 (单位 deg/s)
 * @param accel 指向存储加速度计数据的结构体 (单位 g)
 * @param temperature 指向存储温度数据的变量 (单位 摄氏度)
 */
void ICM206xx_Read_Data(vector3f *gyro,vector3f *accel,float *temperature);




#endif // __ICM20608_H