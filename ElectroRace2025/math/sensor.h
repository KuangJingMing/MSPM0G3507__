#ifndef __SENSOR_H
#define __SENSOR_H
#include "stdint.h"

#include "imu_data_type.h"

float kalman_filter(float angle,float gyro);

void imu_data_sampling(void);

extern sensor smartcar_imu;

#endif
