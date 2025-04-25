#ifndef __DATATYPE_H
#define __DATATYPE_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>





typedef   signed          char int8;
typedef unsigned          char _u8;
typedef unsigned          char uint8;
typedef unsigned          char byte;
typedef   signed short    int int16;
typedef unsigned short    int uint16;
typedef unsigned short    int _u16;
typedef unsigned long     int _u32; 


enum 
{
	_ROL=0,
	_PIT,
	_YAW
};

enum 
{
	ROL=0,
	PIT,
	YAW
};

typedef struct
{
  float x;
  float y;
  float z;
}vector3f;

typedef struct
{
  float x;
  float y;
}vector2f;


typedef struct
{
	uint8_t bit1	:1;
	uint8_t bit2	:1;
	uint8_t bit3	:1;
	uint8_t bit4	:1;
	uint8_t bit5	:1;
	uint8_t bit6	:1;
	uint8_t bit7	:1;
	uint8_t bit8	:1;
	uint8_t bit9	:1;
	uint8_t bit10	:1;
	uint8_t bit11	:1;
	uint8_t bit12	:1;
	uint8_t bit13	:1;
	uint8_t bit14	:1;
	uint8_t bit15	:1;
	uint8_t bit16	:1;
}gray_flags;


typedef struct
{
  volatile float last_time;
  volatile float current_time;
  volatile float period;
  volatile uint16_t period_int;//单位ms
}systime;



typedef struct
{
	uint8_t update_flag;
  uint16_t buf[8];
	uint8_t online_flag;
}mppm;

typedef enum
{
	WHO_AM_I_MPU6050  =0x68,
	WHO_AM_I_ICM20689 =0x98,
	WHO_AM_I_ICM20608D=0xAE,
	WHO_AM_I_ICM20608G=0xAF,
	WHO_AM_I_ICM20602=0x12,
}IMU_ID_READ;


typedef struct
{
	int8_t left_encoder_dir_config,right_encoder_dir_config;//编码器方向配置
	int8_t left_motion_dir_config	,right_motion_dir_config; //电机运动方向配置
	float wheel_radius_cm;				//轮胎半径,单位为cm
	uint16_t pulse_num_per_circle;//轮胎转动一圈累计的脉冲数量
	uint16_t servo_median_value1,servo_median_value2;
}motor_config;


typedef struct
{
	float value;
	uint8_t enable;
	float upper;
	float lower;
	uint16_t low_vbat_cnt;
}low_voltage;


typedef struct
{
	float speed;
	float azimuth;
	float w;
	vector2f pos;
	vector2f vel;
	float distance;
}two_wheel_model;



typedef struct
{
  vector3f _gyro_dps_raw,gyro_dps_raw;
  vector3f _accel_g_raw,accel_g_raw;
  vector3f mag_tesla_raw;
  vector3f last_mag_raw;
	float temperature_raw,last_temperature_raw;
  float temperature_filter;
	float vbat;
	//校准后的数据
	vector3f gyro_dps;
	vector3f accel_g;
	vector3f mag_tesla;
	
	//
	vector3f gyro_offset;
	vector3f accel_scale,accel_offset;
	
	//
	float left_motor_speed_cmps;
	float right_motor_speed_cmps;
	float average_speed_cmps;
	
	
	uint8_t quaternion_init_ok;
	float quaternion_init[4];//初始四元数
	float quaternion[4];//四元数
	float rpy_deg[3];
	float rpy_gyro_dps[3];
	float rpy_gyro_dps_enu[3];
	vector3f accel_earth_cmpss;
	vector2f accel_body_cmpss;
	float sin_rpy[3];
	float cos_rpy[3];
	float cb2n[9];
	float rpy_obs_deg[3];//观测姿态角度
	float rpy_kalman_deg[3];
	//
	float yaw_gyro_enu;
	//
	uint16_t imu_convergence_cnt;
	uint8_t imu_convergence_flag;
	uint8_t temperature_stable_flag;
	uint8_t imu_cal_flag;
	uint8_t imu_health;
	uint8_t lpf_init;
	two_wheel_model state_estimation;
}sensor;


typedef enum
{
	LOCK 		=0x00,
	UNLOCK  =0x01,
}LOCK_STATE;


#define ABS(X)  (((X)>0)?(X):-(X))
#define MAX(a,b)  ((a)>(b)?(a):(b))
#define MIN(a,b)  ((a)>(b)?(b):(a))


typedef enum 
{
  BODY_FRAME=0,//机体坐标系
	MAP_FRAME,	 //导航坐标系
}Navigation_Frame;

typedef enum 
{
  RELATIVE_MODE=0,//相对模式
	GLOBAL_MODE,	  //全局模式
	CMD_VEL_MODE,   //速度模式
	TRANSITION_MODE //过渡模式
}Navigation_Mode;


typedef struct{
	uint16_t number;								//任务编号
	float x;												//x方向位置/速度期望
	float y;												//y方向位置/速度期望
	float z;												//z方向位置/偏航角速度期望
	uint8_t nav_mode;								//导航模式
	uint8_t frame_id;								//坐标系类型
	
	uint8_t update_flag;						//指令更新标志位
	uint8_t ctrl_finish_flag;				//控制完毕标志位
	
	uint16_t cnt; 									//位置控制完毕判断计数器
	float dis_cm;										//总位置偏差
	const float dis_limit_cm;       //位置阈值
	const float cmd_vel_max;        //最大线速度限制
	const float cmd_angular_max;    //最大角速度限制
	
	uint8_t cmd_vel_update;					//速度控制指令更新标志位
	float cmd_vel_x;								//期望x方向速度
	float cmd_vel_y;                //期望y方向速度
	float cmd_vel_angular_z;				//期望偏航方向角速度
	uint32_t cmd_vel_during_cnt;    //速度控制计数器
	uint8_t cmd_vel_suspend_flag;   //控制中止标志位
}nav_ctrl;


#endif


