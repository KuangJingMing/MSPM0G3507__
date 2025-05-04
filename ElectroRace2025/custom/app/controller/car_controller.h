
#ifndef CAR_CONTROLL_H__
#define CAR_CONTROLL_H__

#include "motor_app.h"
#include "arm_math.h"

void car_task(void);
void car_init(void);
void update_speed(void);
void update_distance(void);
void update_speed_pid(void);

typedef struct encoder_t {
	float distance_cm[MOTOR_TYPE_TWO_WHEEL];
	int32_t counts[MOTOR_TYPE_TWO_WHEEL]; 
	float rpms[MOTOR_TYPE_TWO_WHEEL];
	float cmps[MOTOR_TYPE_TWO_WHEEL];
} encoder_t;


typedef struct car_t {
	arm_pid_instance_f32 speed_pid[MOTOR_TYPE_TWO_WHEEL]; 
	float target_speed[MOTOR_TYPE_TWO_WHEEL];
} car_t;

extern encoder_t encoder;

#endif 
