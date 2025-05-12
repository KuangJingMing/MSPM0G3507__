
#ifndef CAR_CONTROLL_H__
#define CAR_CONTROLL_H__

#include "motor_app.h"
#include "arm_math.h"


typedef enum {
	GO_STRAIGHT = 0,
	TURN,
	STOP
} CAR_STATES;

typedef struct encoder_t {
	float distance_cm[MOTOR_TYPE_TWO_WHEEL];
	int32_t counts[MOTOR_TYPE_TWO_WHEEL]; 
	float rpms[MOTOR_TYPE_TWO_WHEEL];
	float cmps[MOTOR_TYPE_TWO_WHEEL];
} encoder_t;


typedef struct car_t {
	CAR_STATES state;
	float target_speed[MOTOR_TYPE_TWO_WHEEL];
	float target_mileage_cm;
	float target_angle;
	arm_pid_instance_f32 angle_pid;
	arm_pid_instance_f32 keep_straight_pid;
	arm_pid_instance_f32 mileage_pid; 
	arm_pid_instance_f32 speed_pid[MOTOR_TYPE_TWO_WHEEL];
} car_t;

void car_task(void);
void car_init(void);
void update_speed(void);
void update_distance(void);
void update_speed_pid(void);
float get_mileage_cm(void);
void clear_mileage(void);
void update_angle_pid(void);
bool car_go_cm(float mileage);
bool spin_turn(float angle);
float32_t calculate_target_base_speed(void);
void apply_direction_correction(float32_t base_speed);


extern encoder_t encoder;

#endif 
