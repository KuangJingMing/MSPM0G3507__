#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>
#include "ti_msp_dl_config.h"

/* 电机控制引脚宏定义 */
// 左电机PWM配置
#define LEFT_MOTOR_PWM_INST           Motor_PWM1_INST   // 左电机PWM定时器实例
#define LEFT_MOTOR_FORWARD_INDEX      DL_TIMER_CC_0_INDEX  // 左电机正转通道
#define LEFT_MOTOR_BACKWARD_INDEX     DL_TIMER_CC_1_INDEX  // 左电机反转通道
#define LEFT_MOTOR_SET_CC(value, index)  DL_TimerA_setCaptureCompareValue(LEFT_MOTOR_PWM_INST, value, index)

// 右电机PWM配置
#define RIGHT_MOTOR_PWM_INST          Motor_PWM2_INST   // 右电机PWM定时器实例
#define RIGHT_MOTOR_FORWARD_INDEX     DL_TIMER_CC_0_INDEX  // 右电机正转通道
#define RIGHT_MOTOR_BACKWARD_INDEX    DL_TIMER_CC_1_INDEX  // 右电机反转通道
#define RIGHT_MOTOR_SET_CC(value, index)  DL_TimerG_setCaptureCompareValue(RIGHT_MOTOR_PWM_INST, value, index)

// 电机PWM参数配置
#define MOTOR_PWM_MAX_VALUE           3000              // PWM最大值

/**
*@brief 电机初始化
*/
void Motor_Init(void);
/**
 * @brief 电机控制函数
 * 
 * @param leftSpeed  左电机速度，范围-3000~3000，正值表示正转，负值表示反转，0表示停止
 * @param rightSpeed 右电机速度，范围-3000~3000，正值表示正转，负值表示反转，0表示停止
 */
void Motor_Control(int32_t leftSpeed, int32_t rightSpeed);

/**
 * @brief 停止所有电机
 */
void Motor_Stop(void);

/**
 * @brief 设置电机向前行驶
 * 
 * @param speed 速度值，范围0~3000
 */
void Motor_Forward(uint32_t speed);

/**
 * @brief 设置电机向后行驶
 * 
 * @param speed 速度值，范围0~3000
 */
void Motor_Backward(uint32_t speed);

#endif /* MOTOR_H */