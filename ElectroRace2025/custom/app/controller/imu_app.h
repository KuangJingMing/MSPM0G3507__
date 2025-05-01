#ifndef __IMU_APP_H
#define __IMU_APP_H

#include "Fusion/Fusion.h" // Fusion AHRS 库头文件
#include "data_type.h"     // 确保包含 vector3f 和 euler_angle_t 的定义
#include "icm20608.h"      // IMU 驱动头文件
#include "filter.h"
#include "fast_math.h"


void imu_data_sampling(void);
void trackless_ahrs_update(void);
void temperature_state_check(void);

extern sensor smartcar_imu;

/***************************************************
函数名: vector3f_sub(vector3f a,vector3f b,vector3f *c)
说明:	向量作差
入口:	vector3f a-被减数
			vector3f b-减数
			vector3f *c-差
出口:	无
备注:	无
作者:	无名创新
****************************************************/
static inline void vector3f_sub(vector3f a,vector3f b,vector3f *c)
{
	c->x=a.x-b.x;
  c->y=a.y-b.y;
  c->z=a.z-b.z;
}

/***************************************************
函数名: void euler_to_quaternion(float *rpy,float *q)
说明:	欧拉角转四元数
入口:	float *rpy-欧拉角
			float *q-四元数
出口:	无
备注:	无
作者:	无名创新
****************************************************/
static inline void euler_to_quaternion(float *rpy,float *q)
{
	float sPitch2, cPitch2; // sin(phi/2) and cos(phi/2)
	float sRoll2 , cRoll2;  // sin(theta/2) and cos(theta/2)
	float sYaw2  , cYaw2;   // sin(psi/2) and cos(psi/2)
	//calculate sines and cosines
	
	FastSinCos(0.5f * rpy[0]*DEG2RAD, &sRoll2, &cRoll2);//roll
	FastSinCos(0.5f * rpy[1]*DEG2RAD, &sPitch2,&cPitch2);//pitch
	FastSinCos(0.5f * rpy[2]*DEG2RAD, &sYaw2,  &cYaw2);//yaw
	
	// compute the quaternion elements
	q[0] = cPitch2*cRoll2*cYaw2+sPitch2*sRoll2*sYaw2;
	q[1] = sPitch2*cRoll2*cYaw2-cPitch2*sRoll2*sYaw2;
	q[2] = cPitch2*sRoll2*cYaw2+sPitch2*cRoll2*sYaw2;
	q[3] = cPitch2*cRoll2*sYaw2-sPitch2*sRoll2*cYaw2;

  // Normalise quaternion
  float recipNorm = invSqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);
  q[0] *= recipNorm;
  q[1] *= recipNorm;
  q[2] *= recipNorm;
  q[3] *= recipNorm;
}

/***************************************************
函数名: void quaternion_to_cb2n(float *q,float *cb2n)
说明: 通过四元数计算方向余弦矩阵
入口:	float *q-四元数
			float *cb2n-方向余弦矩阵
出口:	无
备注:	无
作者:	无名创新
****************************************************/
static inline void quaternion_to_cb2n(float *q,float *cb2n)
{
	float a=q[0];
	float b=q[1];
	float c=q[2];
	float d=q[3];
	float bc=b*c;
	float ad=a*d;
	float bd=b*d;
	float ac=a*c;
	float cd=c*d;
  float ab=a*b;
	float a2=a*a;
	float b2=b*b;
	float c2=c*c;
	float d2=d*d;
  cb2n[0]=a2+b2-c2-d2;
  cb2n[1]=2*(bc-ad);
  cb2n[2]=2*(bd+ac);  
  cb2n[3]=2*(bc+ad);
  cb2n[4]=a2-b2+c2-d2;
  cb2n[5]=2*(cd-ab);
  cb2n[6]=2*(bd-ac);
  cb2n[7]=2*(cd+ab);
  cb2n[8]=a2-b2-c2+d2;	
}

/***************************************************
函数名: void calculate_quaternion_init(vector3f a,vector3f m,float *q)
说明: 通过加速度计、磁力计数据计算初始四元数
入口:	vector3f a-三轴加速度计
			vector3f m-三轴磁力计
			float *q-四元数
出口:	无
备注:	无
作者:	无名创新
****************************************************/
static inline void calculate_quaternion_init(vector3f a,vector3f m,float *q)
{
	float ax,ay,az,mx,my,mz;
	float rpy_obs_deg[3],_sin_rpy[2],_cos_rpy[2];
	
	ax= a.x;	ay= a.y;	az= a.z;
	mx= m.x;	my= m.y;	mz= m.z;	
	float norm = invSqrt(mx * mx + my * my + az * az);
	mx *=	norm;
	my *=	norm;
	mz *=	norm;
	
  rpy_obs_deg[0]=-57.3f*atan(ax*invSqrt(ay*ay+az*az)); //横滚角
  rpy_obs_deg[1]= 57.3f*atan(ay*invSqrt(ax*ax+az*az)); //俯仰角
	_sin_rpy[_PIT] =	sinf(rpy_obs_deg[1]*DEG2RAD);
	_cos_rpy[_PIT] =	cosf(rpy_obs_deg[1]*DEG2RAD);
	_sin_rpy[_ROL] =	sinf(rpy_obs_deg[0]*DEG2RAD);
	_cos_rpy[_ROL] =	cosf(rpy_obs_deg[0]*DEG2RAD);

  /************磁力计倾角补偿*****************/
	vector2f magn;	
	magn.y=  my * _cos_rpy[_PIT]- mz * _sin_rpy[_PIT];
	magn.x=  mx * _cos_rpy[_ROL]
					+my * _sin_rpy[_ROL] * _cos_rpy[_PIT]
					+mz * _sin_rpy[_ROL] * _cos_rpy[_PIT];
  /***********反正切得到磁力计观测角度*********/
  rpy_obs_deg[2] = atan2f(magn.x,magn.y)*57.296f;
	if(rpy_obs_deg[2]<0) rpy_obs_deg[2] = rpy_obs_deg[2]+360;
	rpy_obs_deg[2] = constrain_float(rpy_obs_deg[2],0.0f,360);
	//
	euler_to_quaternion(rpy_obs_deg,q);//计算观测四元数
}


#endif // __IMU_APP_H