#ifndef __DATATYPE_H
#define __DATATYPE_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>


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
  vector3f _gyro_dps_raw,gyro_dps_raw;
  vector3f _accel_g_raw,accel_g_raw;

	float temperature_raw, last_temperature_raw;
  float temperature_filter;
	
	//校准后的数据
	vector3f gyro_dps;
	vector3f accel_g;
	vector3f mag_tesla;
	
	vector3f gyro_offset;
	vector3f accel_scale,accel_offset;
	
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
	
	float yaw_gyro_enu;

	uint16_t imu_convergence_cnt;
	uint8_t imu_convergence_flag;
	uint8_t temperature_stable_flag;
	uint8_t imu_cal_flag;
	uint8_t imu_health;
	uint8_t lpf_init;

} imu_data_t;



#endif


