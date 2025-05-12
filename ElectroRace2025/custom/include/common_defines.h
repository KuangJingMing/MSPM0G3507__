// application/include/common_defines.h

#ifndef COMMON_DEFINES
#define COMMON_DEFINES

#include "stdint.h"

#define int16  short
#define uint16 unsigned short
#define int32  int
#define uint32 unsigned int
#define uint8  unsigned char
#define s32    int32	
#define u32    uint32_t

#define UNIT_TEST_MODE 1
#define MAX_TX_BUFFER_SIZE 256
#define MAX_RX_BUFFER_SIZE MAX_TX_BUFFER_SIZE

#define UART_RX_QUEUE_SIZE MAX_TX_BUFFER_SIZE
#define UART_TX_QUEUE_SIZE 2

#define TEMPERATURE_CTRL_ENABLE 0 // IMU温度控制使能
#define USE_EEPROOM 0 					  // 使用EEPROOM （鸡肋）

#define ENCODER_PERIOD_MS          20              // 20ms 采样周期
#define WHEEL_RADIUS_CM            3.5f          // 轮胎半径，单位：cm
#define PULSE_NUM_PER_CIRCLE       1040           // 轮胎一圈的编码器计数

#endif // common_defines_H
