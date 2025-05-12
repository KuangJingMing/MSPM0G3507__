#include "car_controller.h"
#include "encoder_app.h"
#include "common_defines.h"
#include "log_config.h"
#include "log.h"
#include "fast_math.h"
#include "imu_app.h"

// 预计算常量以提高性能
static const float 	CIRCLE_TO_RPM 	= (60.0f / (ENCODER_PERIOD_MS * 0.001f)); // 采样周期转换为 RPM 的系数
static const float  RPM_TO_CMPS 		= (2.0f * 3.1415926f * WHEEL_RADIUS_CM / 60.0f); // RPM 转换为 cm/s 的系数
static const float  TIME_INTERVAL_S = (ENCODER_PERIOD_MS * 0.001f); // 采样周期，单位：秒


// 角度控制相关常量
#define ANGLE_ERROR_THRESHOLD 5.0f     // 角度误差阈值，小于该值时使用PID精细控制
#define ANGLE_TURN_SPEED 10.0f         // 旋转时的基础速度
#define MIN_ANGLE_TURN_SPEED 5.0f      // 最小有效旋转速度

// 角度PID参数
#define ANGLE_KP 0.3f
#define ANGLE_KI 0.0f
#define ANGLE_KD 0.1f

#define MILEAGE_ERROR_THRESHOLD 5.0f
#define CRUISE_SPEED 10.0f
#define MIN_EFFECTIVE_SPEED 5.0f

#define MIN_PWM_OUTPUT -2500
#define MAX_PWM_OUTPUT 2500

#define SPEED_KP 600
#define SPEED_KI 1
#define SPEED_KD 0

#define MILEAGE_KP 0.5
#define MILEAGE_KI 0
#define MILEAGE_KD 1

#define KEEP_STRAIGHT_KP 0.6
#define KEEP_STRAIGHT_KI 0
#define KEEP_STRAIGHT_KD 0

#define DISTANCE_THRESHOLD_CM 0.5
#define ANGLE_THRESHOLD_DEG 1

// 电机类型
MotorType type = MOTOR_TYPE_TWO_WHEEL;

// 定义 encoder 结构体实例
encoder_t encoder = {0};

car_t car = {
	.state = STOP,
};

void car_task(void) {
		if (car.state == STOP) return;
    update_speed();
    update_distance();
    if (car.state == GO_STRAIGHT) {
        // 1. 先计算目标基础速度 (里程PID)
        float32_t base_speed = calculate_target_base_speed();
        
        // 2. 基于航向差异调整左右轮速度
        apply_direction_correction(base_speed);
        
        // 3. 应用速度PID控制电机
    } else if (car.state == TURN){
				update_angle_pid();
    }
		update_speed_pid();
}



/**
 * @brief 控制小车直线行驶指定里程
 * @param mileage 目标里程（单位：厘米）
 * @return true 表示已达到目标里程，false 表示尚未达到
 */
bool car_go_cm(float mileage)
{
    if (car.state != GO_STRAIGHT)
    {
        car.state = GO_STRAIGHT;
        car.target_mileage_cm = mileage;
        clear_mileage();
    }
    float current_mileage = get_mileage_cm();
    if (fabsf(car.target_mileage_cm - current_mileage) <= DISTANCE_THRESHOLD_CM)
    {
        return true; 
    }
    else
    {
        return false; 
    }
}
/**
 * @brief 控制小车原地旋转指定角度
 * @param angle 目标角度（单位：度）
 * @return true 表示已达到目标角度，false 表示尚未达到
 */
bool spin_turn(float angle)
{
    if (car.state != TURN)
    {
        car.state = TURN;
        car.target_angle = angle;
    }
    float current_angle = smartcar_imu.rpy_deg[_YAW];
    float angle_error = calculate_angle_error(car.target_angle, current_angle);
    if (fabsf(angle_error) <= ANGLE_THRESHOLD_DEG)
    {
        return true;
    }
    else
    {
        return false;
    }
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

void mileage_pid_init(float32_t Kp, float32_t Ki, float32_t Kd) {
	car.mileage_pid.Kp = Kp;
	car.mileage_pid.Ki = Ki;
	car.mileage_pid.Kd = Kd;
	arm_pid_init_f32(&car.mileage_pid, 1);
}

void keep_straight_pid_init(float32_t Kp, float32_t Ki, float32_t Kd) {
	car.keep_straight_pid.Kp = Kp;
	car.keep_straight_pid.Ki = Ki;
	car.keep_straight_pid.Kd = Kd;
	arm_pid_init_f32(&car.keep_straight_pid, 1);
}

void car_init(void) {
    encoder_application_init();
		motor_init();
		speed_pid_init(SPEED_KP, SPEED_KI, SPEED_KD);
		mileage_pid_init(MILEAGE_KP, MILEAGE_KI, MILEAGE_KD);
		keep_straight_pid_init(KEEP_STRAIGHT_KP, KEEP_STRAIGHT_KI, KEEP_STRAIGHT_KD);
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
		float32_t outputs[MOTOR_TYPE_TWO_WHEEL];
    for (int i = 0; i < type; i++) {
        error = car.target_speed[i] - encoder.cmps[i];
        outputs[i] = arm_pid_f32(&car.speed_pid[i], error);
				outputs[i] = constrain_float(outputs[i], MIN_PWM_OUTPUT, MAX_PWM_OUTPUT);
				motor_set_pwm(i, (int)outputs[i]);
		}
}

// 计算目标基础速度 (从里程PID获取)
float32_t calculate_target_base_speed(void) {
    float32_t error;
    float32_t output;
    float32_t current_mileage = get_mileage_cm();
    // 计算误差
    error = car.target_mileage_cm - current_mileage;
    
    if (fabsf(error) > MILEAGE_ERROR_THRESHOLD) {
        // 误差大，匀速运行
        output = CRUISE_SPEED * (error > 0 ? 1.0f : -1.0f); // 保持正确的方向
    } else {
        // 误差小，使用PID控制减速
        output = arm_pid_f32(&car.mileage_pid, error);
        if (fabsf(output) < MIN_EFFECTIVE_SPEED && fabsf(output) > 0.1f) {
            output = MIN_EFFECTIVE_SPEED * (output > 0 ? 1.0f : -1.0f);
        }
    }
    
    return output;
}
// 应用方向修正 (保持直行)
void apply_direction_correction(float32_t base_speed) {
    float32_t yaw_error = calculate_angle_error(car.target_angle, smartcar_imu.rpy_deg[_YAW]);
    float32_t correction = arm_pid_f32(&car.keep_straight_pid, yaw_error);
    
    // 根据方向误差调整左右轮速度差
    // 通过相对调整保持总体速度
    car.target_speed[0] = base_speed + correction;  // 左轮
    car.target_speed[1] = base_speed - correction;  // 右轮
    
    // 防止修正导致某个轮子反向转动 (除非基础速度为0或负值)
    if (base_speed > 0.1f) {
        car.target_speed[0] = fmaxf(0.1f, car.target_speed[0]);
        car.target_speed[1] = fmaxf(0.1f, car.target_speed[1]);
    } else if (base_speed < -0.1f) {
        car.target_speed[0] = fminf(-0.1f, car.target_speed[0]);
        car.target_speed[1] = fminf(-0.1f, car.target_speed[1]);
    }
}

// 计算角度控制的输出速度
void update_angle_pid(void) {
    float32_t current_angle = smartcar_imu.rpy_deg[_YAW];
    float32_t angle_error = calculate_angle_error(car.target_angle, current_angle);
    
    // 如果角度误差很小，认为已到达目标角度，可以停止
    if (fabsf(angle_error) < 0.5f) {
        car.target_speed[0] = 0;
        car.target_speed[1] = 0;
			
        if (car.state == TURN) {
            clear_mileage(); // 重置里程计，准备直线行驶
        }
        return;
    }
    
    float32_t turn_speed;
    
    // 根据误差大小决定旋转速度
    if (fabsf(angle_error) > ANGLE_ERROR_THRESHOLD) {
        // 误差大，使用固定旋转速度
        turn_speed = ANGLE_TURN_SPEED;
    } else {
        // 误差小，使用PID控制精细调整
        turn_speed = arm_pid_f32(&car.angle_pid, angle_error);
        
        // 确保有足够的力矩使电机转动
        if (fabsf(turn_speed) < MIN_ANGLE_TURN_SPEED && fabsf(turn_speed) > 0.1f) {
            turn_speed = MIN_ANGLE_TURN_SPEED * (turn_speed > 0 ? 1.0f : -1.0f);
        }
    }
    
    // 设置左右轮差速以实现转向

    if (angle_error > 0) {
        car.target_speed[0] = turn_speed; // 左轮
        car.target_speed[1] = -turn_speed;  // 右轮
    } else {
        car.target_speed[0] = -turn_speed;  // 左轮
        car.target_speed[1] = turn_speed; // 右轮
    }
}

void clear_mileage(void) {
	for (int i = 0; i < type; i++) {
		encoder.distance_cm[i] = 0;
	}
}

float get_mileage_cm(void) {
	float output = 0;
	for (int i = 0; i < type; i++) {
		output += encoder.distance_cm[i];
	}
	return output / type;
}