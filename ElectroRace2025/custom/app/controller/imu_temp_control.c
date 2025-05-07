#include "imu_temp_control.h"
#include "common_include.h"
//#include "log_config.h" // 默认不使用log宏功能，保持注释
#include "log.h"
#include "arm_math.h" // CMSIS-DSP 头文件

#define SIMULATION_PWM_PERIOD_MAX 100
#define BASE_PWM_VAL 7.5f // 对P项进行补偿

// PID参数
static float32_t temp_kp = 1.0f;
static float32_t temp_ki = 0.0f;
static float32_t temp_kd = 0.0f;

// 目标温度
static float32_t temp_expect = 50.0f;

// PID控制器实例
static arm_pid_instance_f32 s_temp;

// 温度反馈和输出
static float32_t temp_feedback = 0.0f;
static float32_t temp_error = 0.0f;
static float32_t temp_output = 0.0f;

// 初始化IMU温度控制
void imu_temperature_ctrl_init(void) {
#if TEMPERATURE_CTRL_ENABLE
    simulation_pwm_init();
#endif
}

// IMU温度控制任务
void imu_temperature_ctrl_task(void) {
#if TEMPERATURE_CTRL_ENABLE
    imu_temperature_ctrl();
    simulation_pwm_output();
#endif
}

// IMU恒温控制核心逻辑
void imu_temperature_ctrl(void) {
    static uint8_t pid_initialized = 0;
    if (!pid_initialized) {
        init_temp_pid();
        pid_initialized = 1;
    }
    temperature_state_check();
    temp_feedback = smartcar_imu.temperature_filter;
    temp_error = temp_expect - temp_feedback;
    temp_output = BASE_PWM_VAL + arm_pid_f32(&s_temp, temp_error);
}

// 初始化PID控制器
void init_temp_pid(void) {
    s_temp.Kp = temp_kp;
    s_temp.Ki = temp_ki;
    s_temp.Kd = temp_kd;
    arm_pid_init_f32(&s_temp, 1);
}

// 模拟PWM初始化
void simulation_pwm_init(void) {
    DL_GPIO_clearPins(PORTB_PORT, PORTB_HEATER_PIN);
}

// 模拟PWM输出
void simulation_pwm_output(void) {
#if TEMPERATURE_CTRL_ENABLE
    int16_t width = (int16_t)temp_output;
    width = constrain_int16(width, 0, SIMULATION_PWM_PERIOD_MAX);

    static uint16_t cnt = 0;
    cnt = (cnt >= SIMULATION_PWM_PERIOD_MAX) ? 0 : cnt + 1;

    if (cnt < width) {
        DL_GPIO_setPins(PORTB_PORT, PORTB_HEATER_PIN);
    } else {
        DL_GPIO_clearPins(PORTB_PORT, PORTB_HEATER_PIN);
    }
#else
    DL_GPIO_clearPins(PORTB_PORT, PORTB_HEATER_PIN);
#endif
}

// 获取温度状态，判断是否接近目标值
uint8_t temperature_state_get(void) {
#if TEMPERATURE_CTRL_ENABLE
    return (ABS(temp_error) <= 2.5f) ? 1 : 0;
#else
    return 1;
#endif
}

// 温度状态检测，简化计数逻辑
void temperature_state_check(void) {
    static uint16_t stable_cnt = 0;
    static uint16_t crash_cnt = 0;

    // 温度稳定检测
    if (temperature_state_get()) {
        stable_cnt = (stable_cnt < 400) ? stable_cnt + 1 : 400;
        if (stable_cnt >= 400) {
            smartcar_imu.temperature_stable_flag = 1;
        }
    } else {
        stable_cnt = 0;
        smartcar_imu.temperature_stable_flag = 0;
    }

    // 温度传感器异常检测
    if (smartcar_imu.last_temperature_raw == smartcar_imu.temperature_raw) {
        crash_cnt = (crash_cnt < 400) ? crash_cnt + 1 : 400;
    } else {
        crash_cnt = 0;
    }
    smartcar_imu.imu_health = (crash_cnt >= 400) ? 0 : 1;
    smartcar_imu.last_temperature_raw = smartcar_imu.temperature_raw;
}
