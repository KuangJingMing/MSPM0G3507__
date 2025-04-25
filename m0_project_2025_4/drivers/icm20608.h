#ifndef __ICM20608_H
#define __ICM20608_H

#define IMU_ICM20689_ID  0x98


#include "ti_msp_dl_config.h"
#include "imu_data_type.h"

#define	SMPLRT_DIV		0x19
#define	MPU_CONFIG		0x1A
#define	GYRO_CONFIG		0x1B
#define	ACCEL_CONFIG  0x1C
#define ACCEL_CONFIG2 0x1D
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B
#define	WHO_AM_I		  0x75
#define USER_CTRL		  0x6A
#define INT_PIN_CFG		0x37

#define ICM20689_ADRESS		0x68


uint8_t ICM206xx_Init(void);
void ICM206xx_Read_Data(vector3f *gyro,vector3f *accel,float *temperature);

void imu_calibration_params_init(void);

void imu_calibration(vector3f *gyro,vector3f *accel);

#endif
