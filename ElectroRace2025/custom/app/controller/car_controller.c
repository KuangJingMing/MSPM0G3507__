#include "car_controller.h"
#include "encoder_app.h"
#include "common_defines.h"
#include "log_config.h"
#include "log.h"


// 预计算常量以提高性能
static const float 	CIRCLE_TO_RPM 	= (60.0f / (ENCODER_PERIOD_MS * 0.001f)); // 采样周期转换为 RPM 的系数
static const float  RPM_TO_CMPS 		= (2.0f * 3.1415926f * WHEEL_RADIUS_CM / 60.0f); // RPM 转换为 cm/s 的系数
static const float  TIME_INTERVAL_S = (ENCODER_PERIOD_MS * 0.001f); // 采样周期，单位：秒

// 电机类型
MotorType type = MOTOR_TYPE_TWO_WHEEL;

// 定义 encoder 结构体实例
encoder_t encoder = {0};

car_t car = {
	.target_speed[0] = 7,
	.target_speed[1] = 7
};

void car_task(void) {
    update_speed();
    update_distance();
		update_speed_pid();
}

// 初始化 PID 控制器
void speed_pid_init(float32_t Kp, float32_t Ki, float32_t Kd) {
    for (int i = 0; i < type; i++) {
        car.speed_pid[i].Kp = Kp;
				car.speed_pid[i].Ki = Ki;
				car.speed_pid[i].Kd = Kd;
        arm_pid_init_f32(&car.speed_pid[i], 1); // 1 表示重置 PID 状态
    }
}

void car_init(void) {
    encoder_application_init();
		motor_init();
		speed_pid_init(600, 1, 0);
}

void update_speed(void) {
    for (int i = 0; i < type; i++) {
        encoder.counts[i] = encoder_manager_read_and_reset(&robot_encoder_manager, i);
        encoder.rpms[i] = encoder.counts[i] * CIRCLE_TO_RPM / PULSE_NUM_PER_CIRCLE;
        encoder.cmps[i] = encoder.rpms[i] * RPM_TO_CMPS;
    }
}

void update_distance(void) {
    for (int i = 0; i < type; i++) {
        encoder.distance_cm[i] += encoder.cmps[i] * TIME_INTERVAL_S;
    }
}


void update_speed_pid(void) {
    float32_t error;  // 速度误差
		float32_t outputs[2];
    for (int i = 0; i < type; i++) {
        // 计算速度误差 = 目标速度 - 当前速度
        error = car.target_speed[i] - encoder.cmps[i];
        // 计算 PID 输出
        outputs[i] = arm_pid_f32(&car.speed_pid[i], error);
        // 设置电机 PWM 输出（假设有 motor_set_pwm 函数）
		}
		motor_set_pwm(0, (int)outputs[0]);
		motor_set_pwm(1, (int)outputs[1]);
}



