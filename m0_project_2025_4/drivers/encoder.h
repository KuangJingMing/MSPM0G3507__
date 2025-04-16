/*
 * encoder.h
 *  Pin mapping (based on configuration):
 *    - Motor 1: P1 (Pulse) -> PB4 (Encoder_GPIO_Encoder_P1_PIN)
 *    - Motor 1: D1 (Direction) -> PB5 (Encoder_GPIO_Encoder_D1_PIN)
 *    - Motor 2: P2 (Pulse) -> PB6 (Encoder_GPIO_Encoder_P2_PIN)
 *    - Motor 2: D2 (Direction) -> PB7 (Encoder_GPIO_Encoder_D2_PIN)
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdint.h>
#include "ti_msp_dl_config.h"
#include "FreeRTOS.h"

// 编码器数据结构体
typedef struct {
    int left_motor_period_ms;       // 左电机计算周期（ms）
    int right_motor_period_ms;      // 右电机计算周期（ms）
    float left_motor_speed_rpm;     // 左电机速度（转/分钟）
    float left_motor_speed_mps;     // 左电机速度（m/s）
    float right_motor_speed_rpm;    // 右电机速度（转/分钟）
    float right_motor_speed_mps;    // 右电机速度（m/s）
} encoder;

// 外部变量声明（如果需要在其他文件中访问）
extern encoder NEncoder;

// 函数声明
void Encoder_init(void);
float get_left_motor_speed(void);
float get_right_motor_speed(void);

#endif /* ENCODER_H_ */