/*
 * encoder.c
 *    - Motor 1: P1 (Pulse) -> PB4 (Encoder_GPIO_Encoder_P1_PIN)
 *    - Motor 1: D1 (Direction) -> PB5 (Encoder_GPIO_Encoder_D1_PIN)
 *    - Motor 2: P2 (Pulse) -> PB6 (Encoder_GPIO_Encoder_P2_PIN)
 *    - Motor 2: D2 (Direction) -> PB7 (Encoder_GPIO_Encoder_D2_PIN)
 */

#include "encoder.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "semphr.h"
//#include "log_config.h"
#include "log.h"

// 定义全局变量
int left_motor_period_cnt = 0;     // 左电机脉冲计数
int right_motor_period_cnt = 0;    // 右电机脉冲计数

// 定义全局变量 NEncoder
encoder NEncoder = {
    .left_motor_period_ms = 20,
    .right_motor_period_ms = 20,
    .left_motor_speed_rpm = 0.0f,
    .left_motor_speed_mps = 0.0f,    // 速度单位改为 m/s
    .right_motor_speed_rpm = 0.0f,
    .right_motor_speed_mps = 0.0f    // 速度单位改为 m/s
};

// 方向状态数组
uint8_t D_State[2];

typedef struct {
    float pulse_num_per_circle; // 每一圈的脉冲数
    float wheel_radius_cm;      // 轮子的半径，单位为 cm
} TracklessMotor;

TracklessMotor trackless_motor = {
    .pulse_num_per_circle = 570.0f, 
    .wheel_radius_cm = 3.5f     
};

// 定义互斥锁，用于保护脉冲计数变量
SemaphoreHandle_t xMotorCountMutex = NULL;

// 定义软件定时器句柄
TimerHandle_t xSpeedTimer = NULL;

// 软件定时器回调函数
void SpeedTimerCallback(TimerHandle_t xTimer)
{
    int temp_left_cnt = 0;
    int temp_right_cnt = 0;
    
    // 获取脉冲计数，保护共享数据
    if (xSemaphoreTake(xMotorCountMutex, portMAX_DELAY) == pdTRUE) {
        temp_left_cnt = left_motor_period_cnt;
        temp_right_cnt = right_motor_period_cnt;
        left_motor_period_cnt = 0;  // 重置计数
        right_motor_period_cnt = 0;
        xSemaphoreGive(xMotorCountMutex);
    }
    
    // 更新左电机速度
    NEncoder.left_motor_period_ms = 20;
    NEncoder.left_motor_speed_rpm = 60.0f * (temp_left_cnt / trackless_motor.pulse_num_per_circle) / (NEncoder.left_motor_period_ms * 0.001f);
    // 速度单位为 m/s，轮子半径单位为 cm，转为 m/s 需要除以 100
    NEncoder.left_motor_speed_mps = (2.0f * 3.14f * trackless_motor.wheel_radius_cm * (NEncoder.left_motor_speed_rpm / 60.0f)) / 100.0f;
    
    // 更新右电机速度
    NEncoder.right_motor_period_ms = 20;
    NEncoder.right_motor_speed_rpm = 60.0f * (temp_right_cnt / trackless_motor.pulse_num_per_circle) / (NEncoder.right_motor_period_ms * 0.001f);
    // 速度单位为 m/s，轮子半径单位为 cm，转为 m/s 需要除以 100
    NEncoder.right_motor_speed_mps = (2.0f * 3.14f * trackless_motor.wheel_radius_cm * (NEncoder.right_motor_speed_rpm / 60.0f)) / 100.0f;
    
    // 添加日志输出，记录左右电机速度
    log_i("Motor Speeds - Left: %.3f m/s, Right: %.3f m/s", NEncoder.left_motor_speed_mps, NEncoder.right_motor_speed_mps);
}

void Encoder_init(void)
{
    // 初始化编码器中断
    NVIC_ClearPendingIRQ(ENCODER_INT_IRQN);
    NVIC_EnableIRQ(ENCODER_INT_IRQN);
    
    // 创建互斥锁
    xMotorCountMutex = xSemaphoreCreateMutex();
    if (xMotorCountMutex == NULL) {
        // 错误处理：互斥锁创建失败
        while (1);
    }
    
    // 创建并启动软件定时器，周期20ms
    xSpeedTimer = xTimerCreate("SpeedTimer", pdMS_TO_TICKS(20), pdTRUE, NULL, SpeedTimerCallback);
    if (xSpeedTimer == NULL) {
        // 错误处理：定时器创建失败
        while (1);
    }
    if (xTimerStart(xSpeedTimer, 0) != pdPASS) {
        // 错误处理：定时器启动失败
        while (1);
    }
}

void QEI0_IRQHandler(void)
{
    D_State[0] = DL_GPIO_readPins(ENCODER_PORT, ENCODER_D1_PIN);
    // 在中断中使用ISR版本的互斥锁函数
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (xSemaphoreTakeFromISR(xMotorCountMutex, &xHigherPriorityTaskWoken) == pdTRUE) {
        if (!D_State[0]) {
            left_motor_period_cnt--;
        } else {
            left_motor_period_cnt++;
        }
        xSemaphoreGiveFromISR(xMotorCountMutex, &xHigherPriorityTaskWoken);
    }
    // 如果有更高优先级的任务被唤醒，请求上下文切换
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void QEI1_IRQHandler(void)
{
    D_State[1] = DL_GPIO_readPins(ENCODER_PORT, ENCODER_D2_PIN);
    // 在中断中使用ISR版本的互斥锁函数
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (xSemaphoreTakeFromISR(xMotorCountMutex, &xHigherPriorityTaskWoken) == pdTRUE) {
        if (!D_State[1]) {
            right_motor_period_cnt++;
        } else {
            right_motor_period_cnt--;
        }
        xSemaphoreGiveFromISR(xMotorCountMutex, &xHigherPriorityTaskWoken);
    }
    // 如果有更高优先级的任务被唤醒，请求上下文切换
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void GROUP1_IRQHandler(void)
{
    if (DL_Interrupt_getStatusGroup(DL_INTERRUPT_GROUP_1, DL_INTERRUPT_GROUP1_GPIOB)) {
        if (DL_GPIO_getEnabledInterruptStatus(ENCODER_PORT, ENCODER_P1_PIN)) {
            QEI0_IRQHandler();
            DL_GPIO_clearInterruptStatus(ENCODER_PORT, ENCODER_P1_PIN);
        }

        if (DL_GPIO_getEnabledInterruptStatus(ENCODER_PORT, ENCODER_P2_PIN)) {
            QEI1_IRQHandler();
            DL_GPIO_clearInterruptStatus(ENCODER_PORT, ENCODER_P2_PIN);
        }
        DL_Interrupt_clearGroup(DL_INTERRUPT_GROUP_1, DL_INTERRUPT_GROUP1_GPIOB);
    }
}

float get_left_motor_speed(void)
{
    return NEncoder.left_motor_speed_mps; // 返回速度，单位为 m/s
}

float get_right_motor_speed(void)
{
    return NEncoder.right_motor_speed_mps; // 返回速度，单位为 m/s
}