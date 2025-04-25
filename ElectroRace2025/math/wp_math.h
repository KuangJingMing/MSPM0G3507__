#ifndef __WP_MATH_H__
#define __WP_MATH_H__

#include "stdint.h"

//#define GYRO_CALIBRATION_COFF 0.060976f  //2000
//#define GYRO_CALIBRATION_COFF 0.030488f; //1000
#define GYRO_CALIBRATION_COFF 0.0152672f;  //500

// acceleration due to gravity in m/s/s
#define GRAVITY_MSS 9.80665f
#define GRAVITY_RAW 8192.0f
#define RAW_TO_G     GRAVITY_MSS/GRAVITY_RAW
#define G_TO_RAW  	 GRAVITY_RAW/GRAVITY_MSS

#ifndef M_PI_F
 #define M_PI_F 3.141592653589793f
#endif

#ifndef PI
 # define PI M_PI_F
#endif

#define DEG2RAD (PI / 180.0f)
#define RAD2DEG (180.0f / PI)

#define ABS(X)  (((X)>0)?(X):-(X))
#define MAX(a,b)  ((a)>(b)?(a):(b))
#define MIN(a,b)  ((a)>(b)?(b):(a))

float constrain_float(float amt, float low, float high); 
float sq(float v);
float safe_sqrt(float v);

float invSqrt(float x);

void FastSinCos(float x, float *sinVal, float *cosVal);
float FastSin(float x);
float FastCos(float x);


float FastSqrtI(float x);
float FastSqrt(float x);

int16_t constrain_int16(int16_t amt, int16_t low, int16_t high);

#define sq2(sq) (((float)sq) * ((float)sq))
	
#endif