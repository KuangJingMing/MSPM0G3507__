#include "imu_temp_control.h"
#include "common_include.h"
#include "log_config.h"
#include "log.h"
#include "arm_math.h" // 需要包含 CMSIS-DSP 头文件



#define Simulation_PWM_Period_MAX  100
#define BASE_PWM_VAL 7.5  //对p项进行补偿


// PID 参数 (使用 float32_t 类型)
static float32_t temp_kp = 1.0f;
static float32_t temp_ki = 0.0f;
static float32_t temp_kd = 0.0f;

// 目标温度
static float32_t temp_expect = 50.0f;

// PID 控制器实例
arm_pid_instance_f32 S_temp; // 声明一个浮点数 PID 结构体实例

// 温度反馈和输出
static float32_t temp_feedback = 0;
static float32_t temp_error = 0; // Keep this for temperature_state_get
static float32_t temp_output = 0; // PID 计算的输出

void imu_temperature_ctrl_init(void) {
	simulation_pwm_init();
}

void imu_temperature_ctrl_task(void) {
	imu_temperature_ctrl();
	simulation_pwm_output();
}

/***************************************************
函数名: void imu_temperature_ctrl(void)
说明:	IMU恒温控制
入口:	无
出口:	无
备注:	无
作者:	无名创新
****************************************************/
void imu_temperature_ctrl(void)
{
    // 确保PID初始化
    static uint8_t pid_initialized = 0;
    if (!pid_initialized) {
        init_temp_pid();
        pid_initialized = 1;
    }
    temperature_state_check();
    temp_feedback = smartcar_imu.temperature_filter;
    temp_error = temp_expect - temp_feedback;      
    temp_output = BASE_PWM_VAL;
		temp_output += arm_pid_f32(&S_temp, temp_error);
}


void init_temp_pid(void)
{
    S_temp.Kp = temp_kp;
    S_temp.Ki = temp_ki;
    S_temp.Kd = temp_kd;
    arm_pid_init_f32(&S_temp, 1); 
}


/***************************************************
函数名: void simulation_pwm_init(void)
说明:	模拟pwm初始化
入口:	无
出口:	无
备注:	无
作者:	无名创新
****************************************************/
void simulation_pwm_init(void)
{
    DL_GPIO_clearPins(PORTB_PORT, PORTB_HEATER_PIN);
}


/***************************************************
函数名: void simulation_pwm_output(void)
说明:	模拟pwm输出
入口:	无
出口:	无
备注:	模拟 PWM 通常需要在一个固定频率的任务中运行，
        并且这个频率应该比你的 PID 控制器运行频率高，
        或者至少与 PID 控制器运行频率一致。
        如果这个函数在 5ms 任务中运行，并且 PWM 周期是 100ms (20个 5ms 滴答)，
        这是可以工作的。
作者:	无名创新
****************************************************/
void simulation_pwm_output(void)
{
#if TEMPERATURE_CTRL_ENABLE
    // 将浮点数输出转换为 int16_t，用于模拟 PWM 宽度
    // 需要考虑负值，如果你的加热器只支持正向输出，需要处理负值。
    int16_t width = (int16_t)temp_output;

    // 将宽度限制在 0 到 Simulation_PWM_Period_MAX 之间，因为 PWM 占空比是正的
    width = constrain_int16(width, 0, Simulation_PWM_Period_MAX);


    static uint16_t cnt = 0;
    cnt++;
    if (cnt >= Simulation_PWM_Period_MAX) {
        cnt = 0;
    }

    // 如果当前计数小于或等于宽度，则打开加热器 (高电平)
    if (cnt < width) { // 使用 '<' 而不是 '<=' 以匹配计数从 0 到 MAX-1 的习惯
        DL_GPIO_setPins(PORTB_PORT, PORTB_HEATER_PIN);
    } else {
        DL_GPIO_clearPins(PORTB_PORT, PORTB_HEATER_PIN);
    }
#else
    DL_GPIO_clearPins(PORTB_PORT, PORTB_HEATER_PIN);
#endif
}

/***************************************************
函数名: uint8_t temperature_state_get(void)
说明:	温度接近目标值检测
入口:	无
出口:	uint8_t 就位标志
备注:	无
作者:	无名创新
****************************************************/
uint8_t temperature_state_get(void)
{
#if TEMPERATURE_CTRL_ENABLE
    // 使用计算出的误差来判断是否接近目标值
    return (ABS(temp_error) <= 2.5f) ? 1 : 0;
#else
    return 1;
#endif
}

/***************************************************
函数名: void temperature_state_check(void)
说明:	温度恒定检测
入口:	无
出口:	无
备注:	无
作者:	无名创新
****************************************************/
void temperature_state_check(void)
{
    static uint16_t _cnt = 0;
    static uint16_t temperature_crash_cnt = 0;

    // 检查温度是否接近目标值并计数
    if (temperature_state_get() == 1) {
        _cnt++;
        // 持续接近目标值一段时间后认为稳定
        if (_cnt >= 400) smartcar_imu.temperature_stable_flag = 1;
    } else {
        // 如果不接近目标值，计数减半 (快速衰减)
        _cnt /= 2;
        smartcar_imu.temperature_stable_flag = 0; // 如果不再接近，清除稳定标志
    }

    // 检查温度传感器是否异常 (读数长时间不变)
    if (temperature_crash_cnt < 400) {
        if (smartcar_imu.last_temperature_raw == smartcar_imu.temperature_raw) {
            temperature_crash_cnt++;
        } else {
            temperature_crash_cnt /= 2; // 如果变化了，计数减半
        }
        smartcar_imu.imu_health = 1; // 传感器似乎正常
    } else {
        smartcar_imu.imu_health = 0; // 传感器可能异常
    }
     smartcar_imu.last_temperature_raw = smartcar_imu.temperature_raw; // 更新上次原始温度值
}
