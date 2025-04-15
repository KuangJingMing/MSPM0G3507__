/*
 * encoder.c
 *
 *  Created on: [Current Date]
 *      Author: [Your Name]
 *
 *  Implementation file for encoder counting on MSPM0G350X.
 *  Uses GPIO interrupts to count pulse signals (P1, P2) and read direction (D1, D2).
 *  Pin mapping (based on configuration):
 *    - Motor 1: P1 (Pulse) -> PB4 (Encoder_GPIO_Encoder_P1_PIN)
 *    - Motor 1: D1 (Direction) -> PB5 (Encoder_GPIO_Encoder_D1_PIN)
 *    - Motor 2: P2 (Pulse) -> PB6 (Encoder_GPIO_Encoder_P2_PIN)
 *    - Motor 2: D2 (Direction) -> PB7 (Encoder_GPIO_Encoder_D2_PIN)
 */

#include "encoder.h"

// 定义全局变量
int left_motor_period_cnt = 0;     // 左电机脉冲计数
int right_motor_period_cnt = 0;    // 右电机脉冲计数

// 定义全局变量 NEncoder
encoder NEncoder = {
    .left_motor_period_ms = 20,
    .right_motor_period_ms = 20,
    .left_motor_speed_rpm = 0.0f,
    .left_motor_speed_cmps = 0.0f,
    .right_motor_speed_rpm = 0.0f,
    .right_motor_speed_cmps = 0.0f
};

// 方向状态数组
uint8_t D_State[2];

typedef struct {
    float pulse_num_per_circle; // 每一圈的脉冲数
    float wheel_radius_cm;      // 轮子的半径
} TracklessMotor;

TracklessMotor trackless_motor = {
    .pulse_num_per_circle = 570.0f, 
    .wheel_radius_cm = 3.0f     
};

void Encoder_init(void)
{
    NVIC_ClearPendingIRQ(ENCOPER_INT_IRQN);
    NVIC_EnableIRQ(ENCOPER_INT_IRQN);
}

void QEI0_IRQHandler(void)
{
    D_State[0] = DL_GPIO_readPins(ENCOPER_PORT, ENCOPER_D1_PIN);
    if (!D_State[0]) {
        left_motor_period_cnt--;
    } else {
        left_motor_period_cnt++;
    }
}

void QEI1_IRQHandler(void)
{
    D_State[1] = DL_GPIO_readPins(ENCOPER_PORT, ENCOPER_D2_PIN);
    if (!D_State[1]) {
        right_motor_period_cnt++;
    } else {
        right_motor_period_cnt--;
    }
}

void GROUP1_IRQHandler(void)
{
    if (DL_Interrupt_getStatusGroup(DL_INTERRUPT_GROUP_1, DL_INTERRUPT_GROUP1_GPIOB)) {
        if (DL_GPIO_getEnabledInterruptStatus(ENCOPER_PORT, ENCOPER_P1_PIN)) {
            QEI0_IRQHandler();
            DL_GPIO_clearInterruptStatus(ENCOPER_PORT, ENCOPER_P1_PIN);
        }

        if (DL_GPIO_getEnabledInterruptStatus(ENCOPER_PORT, ENCOPER_P2_PIN)) {
            QEI1_IRQHandler();
            DL_GPIO_clearInterruptStatus(ENCOPER_PORT, ENCOPER_P2_PIN);
        }
        DL_Interrupt_clearGroup(DL_INTERRUPT_GROUP_1, DL_INTERRUPT_GROUP1_GPIOB);
    }
}

float get_left_motor_speed(void)
{
    static uint16_t cnt1 = 0;
    cnt1++;
    if (cnt1 >= 4) {
        cnt1 = 0;
        NEncoder.left_motor_period_ms = 20;
        // 将速度转化成转每分钟（RPM）
        NEncoder.left_motor_speed_rpm = 60 * (left_motor_period_cnt * 1.0f / trackless_motor.pulse_num_per_circle) / (NEncoder.left_motor_period_ms * 0.001f);
        // 将 RPM 转化为 cm/s
        NEncoder.left_motor_speed_cmps = 2 * 3.14f * trackless_motor.wheel_radius_cm * (NEncoder.left_motor_speed_rpm / 60.0f);
        // 重置计数器
        left_motor_period_cnt = 0;
    }
    return NEncoder.left_motor_speed_cmps;
}

float get_right_motor_speed(void)
{
    static uint16_t cnt2 = 0;
    cnt2++;
    if (cnt2 >= 4) {
        cnt2 = 0;
        NEncoder.right_motor_period_ms = 20;
        // 将速度转化成转每分钟（RPM）
        NEncoder.right_motor_speed_rpm = 60 * (right_motor_period_cnt * 1.0f / trackless_motor.pulse_num_per_circle) / (NEncoder.right_motor_period_ms * 0.001f);
        // 将 RPM 转化为 cm/s
        NEncoder.right_motor_speed_cmps = 2 * 3.14f * trackless_motor.wheel_radius_cm * (NEncoder.right_motor_speed_rpm / 60.0f);
        // 重置计数器
        right_motor_period_cnt = 0;
    }
    return NEncoder.right_motor_speed_cmps;
}