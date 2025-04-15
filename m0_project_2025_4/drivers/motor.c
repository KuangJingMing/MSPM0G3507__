#include "motor.h"
/*
*@brief 电机初始化
*/
void Motor_Init(void)
{
    // 确保所有PWM通道初始值为0，防止电机在初始化时意外运转
    LEFT_MOTOR_SET_CC(0, LEFT_MOTOR_FORWARD_INDEX);
    LEFT_MOTOR_SET_CC(0, LEFT_MOTOR_BACKWARD_INDEX);
    RIGHT_MOTOR_SET_CC(0, RIGHT_MOTOR_FORWARD_INDEX);
    RIGHT_MOTOR_SET_CC(0, RIGHT_MOTOR_BACKWARD_INDEX);
}
/**
 * @brief 电机控制函数
 * 
 * @param leftSpeed  左电机速度，范围-3000~3000，正值表示正转，负值表示反转，0表示停止
 * @param rightSpeed 右电机速度，范围-3000~3000，正值表示正转，负值表示反转，0表示停止
 */
void Motor_Control(int32_t leftSpeed, int32_t rightSpeed)
{
    // 限制速度范围在-MOTOR_PWM_MAX_VALUE~MOTOR_PWM_MAX_VALUE
    if (leftSpeed > MOTOR_PWM_MAX_VALUE)
        leftSpeed = MOTOR_PWM_MAX_VALUE;
    else if (leftSpeed < -MOTOR_PWM_MAX_VALUE)
        leftSpeed = -MOTOR_PWM_MAX_VALUE;
        
    if (rightSpeed > MOTOR_PWM_MAX_VALUE)
        rightSpeed = MOTOR_PWM_MAX_VALUE;
    else if (rightSpeed < -MOTOR_PWM_MAX_VALUE)
        rightSpeed = -MOTOR_PWM_MAX_VALUE;
    
    // 控制左电机 - 修改后的逻辑
    if (leftSpeed >= 0) {
        // 正转：通道1输出PWM，通道0输出0（交换了正反转通道）
        LEFT_MOTOR_SET_CC(0, LEFT_MOTOR_FORWARD_INDEX);
        LEFT_MOTOR_SET_CC(leftSpeed, LEFT_MOTOR_BACKWARD_INDEX);
    } else {
        // 反转：通道1输出0，通道0输出PWM（交换了正反转通道）
        LEFT_MOTOR_SET_CC(-leftSpeed, LEFT_MOTOR_FORWARD_INDEX);
        LEFT_MOTOR_SET_CC(0, LEFT_MOTOR_BACKWARD_INDEX);
    }
    
    // 控制右电机 (保持不变，因为右电机工作正常)
    if (rightSpeed >= 0) {
        // 正转：通道0输出PWM，通道1输出0
        RIGHT_MOTOR_SET_CC(rightSpeed, RIGHT_MOTOR_FORWARD_INDEX);
        RIGHT_MOTOR_SET_CC(0, RIGHT_MOTOR_BACKWARD_INDEX);
    } else {
        // 反转：通道0输出0，通道1输出PWM
        RIGHT_MOTOR_SET_CC(0, RIGHT_MOTOR_FORWARD_INDEX);
        RIGHT_MOTOR_SET_CC(-rightSpeed, RIGHT_MOTOR_BACKWARD_INDEX);
    }
}

/**
 * @brief 停止所有电机
 */
void Motor_Stop(void)
{
    // 将所有PWM通道设置为0
    LEFT_MOTOR_SET_CC(0, LEFT_MOTOR_FORWARD_INDEX);
    LEFT_MOTOR_SET_CC(0, LEFT_MOTOR_BACKWARD_INDEX);
    RIGHT_MOTOR_SET_CC(0, RIGHT_MOTOR_FORWARD_INDEX);
    RIGHT_MOTOR_SET_CC(0, RIGHT_MOTOR_BACKWARD_INDEX);
}

/**
 * @brief 设置电机向前行驶
 * 
 * @param speed 速度值，范围0~3000
 */
void Motor_Forward(uint32_t speed)
{
    // 限制速度范围
    if (speed > MOTOR_PWM_MAX_VALUE)
        speed = MOTOR_PWM_MAX_VALUE;
        
    // 两个电机都向前
    Motor_Control(speed, speed);
}

/**
 * @brief 设置电机向后行驶
 * 
 * @param speed 速度值，范围0~3000
 */
void Motor_Backward(uint32_t speed)
{
    // 限制速度范围
    if (speed > MOTOR_PWM_MAX_VALUE)
        speed = MOTOR_PWM_MAX_VALUE;
        
    // 两个电机都向后
    Motor_Control(-speed, -speed);
}