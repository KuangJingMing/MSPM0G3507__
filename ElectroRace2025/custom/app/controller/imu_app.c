#include "imu_app.h"
#include "eeprom.h"
#include "log_config.h"
#include "log.h"

lpf_param accel_lpf_param,gyro_lpf_param;
lpf_buf gyro_filter_buf[3],accel_filter_buf[3];

sensor smartcar_imu;
FusionAhrs ahrs;
FusionOffset offset;

#define sampling_frequent 200
#define gyro_delta_dps  3.0f


/***************************************
函数名:	void imu_calibration_params_init(void)
说明: 加速度计/陀螺仪标定数据初始化
入口:	无
出口:	无
备注:	先从eeprom读取数据,若数据不存在，等待温度就位后再标定
作者:	无名创新
***************************************/
void imu_calibration_params_init(void)
{
	vector3f gyro_offset_temp;
	ReadFlashParameterOne(GYRO_X_OFFSET,&gyro_offset_temp.x);
	ReadFlashParameterOne(GYRO_Y_OFFSET,&gyro_offset_temp.y);
	ReadFlashParameterOne(GYRO_Z_OFFSET,&gyro_offset_temp.z);	
	if(isnan(gyro_offset_temp.x)==0
		&&isnan(gyro_offset_temp.y)==0
		 &&isnan(gyro_offset_temp.z)==0)//如果之前已经温度校准过，开机时直接用之前校准的数据 
	{
		smartcar_imu.gyro_offset.x=gyro_offset_temp.x;
		smartcar_imu.gyro_offset.y=gyro_offset_temp.y;
		smartcar_imu.gyro_offset.z=gyro_offset_temp.z;
		smartcar_imu.imu_cal_flag=1;
	}
	else
	{
		smartcar_imu.gyro_offset.x=0;
		smartcar_imu.gyro_offset.y=0;
		smartcar_imu.gyro_offset.z=0;
	}
	//待温度稳定后，自动校准陀螺仪零偏
	smartcar_imu.accel_scale.x=1.0f;
	smartcar_imu.accel_scale.y=1.0f;
	smartcar_imu.accel_scale.z=1.0f;
	vector3f accel_offset_temp;
	ReadFlashParameterOne(ACCEL_X_OFFSET,&accel_offset_temp.x);
	ReadFlashParameterOne(ACCEL_Y_OFFSET,&accel_offset_temp.y);
	ReadFlashParameterOne(ACCEL_Z_OFFSET,&accel_offset_temp.z);	
	if(isnan(accel_offset_temp.x)==0
		&&isnan(accel_offset_temp.y)==0
		 &&isnan(accel_offset_temp.z)==0)//如果之前已经温度校准过，开机时直接用之前校准的数据 
	{
		smartcar_imu.accel_offset.x=accel_offset_temp.x;
		smartcar_imu.accel_offset.y=accel_offset_temp.y;
		smartcar_imu.accel_offset.z=accel_offset_temp.z;
		smartcar_imu.imu_cal_flag=1;
	}
	else
	{
		smartcar_imu.accel_offset.x=0;
		smartcar_imu.accel_offset.y=0;
		smartcar_imu.accel_offset.z=0;
	}	
} 



/***************************************
函数名:	void imu_calibration(vector3f *gyro,vector3f *accel)
说明: 加速度计/陀螺仪标定
入口:	无
出口:	无
备注:	无
作者:	无名创新
***************************************/
void imu_calibration(vector3f *gyro,vector3f *accel)
{
	if(smartcar_imu.imu_cal_flag==1)  return;
	
	static uint16_t cnt=0;
	static vector3f last_gyro;
	static vector3f accel_sum,gyro_sum;
	if(ABS(gyro->x-last_gyro.x)<=gyro_delta_dps
	 &&ABS(gyro->y-last_gyro.y)<=gyro_delta_dps
	 &&ABS(gyro->z-last_gyro.z)<=gyro_delta_dps
	 &&smartcar_imu.temperature_stable_flag==1)
	{
		gyro_sum.x+=gyro->x;
		gyro_sum.y+=gyro->y;
		gyro_sum.z+=gyro->z;
		accel_sum.x+=accel->x;
		accel_sum.y+=accel->y;
		accel_sum.z+=accel->z;
		cnt++;
	}
  else
	{
		gyro_sum.x=0;
		gyro_sum.y=0;
		gyro_sum.z=0;
		accel_sum.x=0;
		accel_sum.y=0;
		accel_sum.z=0;
		cnt=0;
	}
  last_gyro.x=gyro->x;
	last_gyro.y=gyro->y;
	last_gyro.z=gyro->z;

	if(cnt>=400)//持续满足2s
	{
		smartcar_imu.gyro_offset.x =(gyro_sum.x/cnt);//得到标定偏移
		smartcar_imu.gyro_offset.y =(gyro_sum.y/cnt);
		smartcar_imu.gyro_offset.z =(gyro_sum.z/cnt);
		
		smartcar_imu.accel_offset.x =(accel_sum.x/cnt);//得到标定偏移
		smartcar_imu.accel_offset.y =(accel_sum.y/cnt);
		smartcar_imu.accel_offset.z =(accel_sum.z/cnt)-safe_sqrt(1-sq2(smartcar_imu.accel_offset.x)-sq2(smartcar_imu.accel_offset.y));
			
		WriteFlashParameter_Three(GYRO_X_OFFSET,
															smartcar_imu.gyro_offset.x,
															smartcar_imu.gyro_offset.y,
															smartcar_imu.gyro_offset.z,
															&Trackless_Params);		
		WriteFlashParameter_Three(ACCEL_X_OFFSET,
															smartcar_imu.accel_offset.x,
															smartcar_imu.accel_offset.y,
															smartcar_imu.accel_offset.z,
															&Trackless_Params);	
		
		gyro_sum.x=0;
		gyro_sum.y=0;
		gyro_sum.z=0;
		accel_sum.x=0;
		accel_sum.y=0;
		accel_sum.z=0;		
		cnt=0;
			
		
		smartcar_imu.imu_cal_flag=1;
	}
}

/***************************************************
函数名: void imu_data_sampling(void)
说明:	IMU数据采样/校准/滤波
入口:	无
出口:	无
备注:	无
作者:	无名创新
****************************************************/
void imu_data_sampling(void)
{
	if(smartcar_imu.lpf_init==0)
	{
		set_cutoff_frequency(200, 50,&gyro_lpf_param); //姿态角速度反馈滤波参数 
		set_cutoff_frequency(200, 30,&accel_lpf_param);//姿态解算加计修正滤波值
	  smartcar_imu.lpf_init=1;
	}
	smartcar_imu.last_temperature_raw=smartcar_imu.temperature_raw;
	//陀螺仪/陀螺仪数据采集
	ICM206xx_Read_Data(&smartcar_imu._gyro_dps_raw,&smartcar_imu._accel_g_raw,&smartcar_imu.temperature_raw);
	//陀螺仪数据低通滤波
	smartcar_imu.gyro_dps_raw.x=LPButterworth(smartcar_imu._gyro_dps_raw.x,&gyro_filter_buf[0],&gyro_lpf_param);
  smartcar_imu.gyro_dps_raw.y=LPButterworth(smartcar_imu._gyro_dps_raw.y,&gyro_filter_buf[1],&gyro_lpf_param);
  smartcar_imu.gyro_dps_raw.z=LPButterworth(smartcar_imu._gyro_dps_raw.z,&gyro_filter_buf[2],&gyro_lpf_param);		
	//加速度数据低通滤波
	smartcar_imu.accel_g_raw.x=LPButterworth(smartcar_imu._accel_g_raw.x,&accel_filter_buf[0],&accel_lpf_param);
	smartcar_imu.accel_g_raw.y=LPButterworth(smartcar_imu._accel_g_raw.y,&accel_filter_buf[1],&accel_lpf_param);
	smartcar_imu.accel_g_raw.z=LPButterworth(smartcar_imu._accel_g_raw.z,&accel_filter_buf[2],&accel_lpf_param);	
	//温度传感器数据一阶低通滤波
	smartcar_imu.temperature_filter=0.75f*smartcar_imu.temperature_raw+0.25f*smartcar_imu.temperature_filter;
  //得到校准后的角速度、加速度、磁力计数据
	vector3f_sub(smartcar_imu.gyro_dps_raw,smartcar_imu.gyro_offset,&smartcar_imu.gyro_dps);
  
	smartcar_imu.accel_g.x=smartcar_imu.accel_scale.x*smartcar_imu.accel_g_raw.x-smartcar_imu.accel_offset.x;
  smartcar_imu.accel_g.y=smartcar_imu.accel_scale.y*smartcar_imu.accel_g_raw.y-smartcar_imu.accel_offset.y;
  smartcar_imu.accel_g.z=smartcar_imu.accel_scale.z*smartcar_imu.accel_g_raw.z-smartcar_imu.accel_offset.z;  	
	//加速度计/陀螺仪校准检测
	imu_calibration(&smartcar_imu.gyro_dps_raw,&smartcar_imu.accel_g_raw);
	
	//通过三轴加速度计,计算水平观测角度
	float ax,ay,az;
	ax=smartcar_imu.accel_g.x;
	ay=smartcar_imu.accel_g.y;
	az=smartcar_imu.accel_g.z;
	
  smartcar_imu.rpy_obs_deg[0]=-57.3f*atan(ax*invSqrt(ay*ay+az*az)); //横滚角
  smartcar_imu.rpy_obs_deg[1]= 57.3f*atan(ay*invSqrt(ax*ax+az*az)); //俯仰角
	//俯仰轴姿态角卡尔曼滤波
	smartcar_imu.rpy_kalman_deg[1]=kalman_filter(smartcar_imu.rpy_obs_deg[1],smartcar_imu.gyro_dps.x);
}

/***************************************************
函数名: void trackless_ahrs_update(void)
说明:	姿态更新
入口:	无
出口:	无
备注:	无
作者:	无名创新
****************************************************/
void trackless_ahrs_update(void)
{
	/****************************************************/
	FusionVector gyroscope={0.0f, 0.0f, 0.0f};
	FusionVector accelerometer = {0.0f, 0.0f, 1.0f};
	FusionVector earthacceleration = {0.0f, 0.0f, 0.0f}; 
	
	gyroscope.axis.x=smartcar_imu.gyro_dps.x;
	gyroscope.axis.y=smartcar_imu.gyro_dps.y;
	gyroscope.axis.z=smartcar_imu.gyro_dps.z;
	
	accelerometer.axis.x=smartcar_imu.accel_g.x;
	accelerometer.axis.y=smartcar_imu.accel_g.y;
	accelerometer.axis.z=smartcar_imu.accel_g.z;
	if(smartcar_imu.quaternion_init_ok==0)
	{
		if(smartcar_imu.temperature_stable_flag==1)//温度稳定
		{
			calculate_quaternion_init(smartcar_imu.accel_g,smartcar_imu.mag_tesla,smartcar_imu.quaternion_init);
			smartcar_imu.quaternion_init_ok	=	1;
			//AHRS初始化
			FusionOffsetInitialise(&offset, sampling_frequent);
			FusionAhrsInitialise(&ahrs);
			//Set AHRS algorithm settings
			const FusionAhrsSettings settings = {
							.gain = 0.5f,
							.accelerationRejection = 10.0f,
							.magneticRejection = 20.0f,
							.recoveryTriggerPeriod = 5 * sampling_frequent, /* 5 seconds */
			};
			FusionAhrsSetSettings(&ahrs, &settings);
		}		
	}

	if(smartcar_imu.quaternion_init_ok==1)
	{
		gyroscope = FusionOffsetUpdate(&offset, gyroscope);
		FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer,0.005f);//FusionAhrsUpdateExternalHeading(&ahrs, gyroscope, accelerometer,uav_ahrs.rpy_obs_deg[2],0.005f);
		FusionEuler euler=FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
		earthacceleration=FusionAhrsGetEarthAcceleration(&ahrs);
		smartcar_imu.rpy_deg[_ROL]=euler.angle.pitch;
		smartcar_imu.rpy_deg[_PIT]=euler.angle.roll;
		smartcar_imu.rpy_deg[_YAW]=euler.angle.yaw;
		//获取导航系系统运动加速度
		smartcar_imu.accel_earth_cmpss.x=earthacceleration.axis.x*GRAVITY_MSS*100.0f;
		smartcar_imu.accel_earth_cmpss.y=earthacceleration.axis.y*GRAVITY_MSS*100.0f;
		smartcar_imu.accel_earth_cmpss.z=earthacceleration.axis.z*GRAVITY_MSS*100.0f;
		
		if(smartcar_imu.imu_convergence_cnt<200*5)  smartcar_imu.imu_convergence_cnt++;
		else if(smartcar_imu.imu_convergence_cnt==200*5)
		{
			smartcar_imu.imu_convergence_cnt++;
		}
		else 
		{
			smartcar_imu.imu_convergence_flag=1;
		}
	}

  smartcar_imu.rpy_gyro_dps[_PIT]=smartcar_imu.gyro_dps.x;
  smartcar_imu.rpy_gyro_dps[_ROL]=smartcar_imu.gyro_dps.y;
  smartcar_imu.rpy_gyro_dps[_YAW]=smartcar_imu.gyro_dps.z;
  //计算姿态相关三角函数
  smartcar_imu.sin_rpy[_PIT]=FastSin(smartcar_imu.rpy_deg[_PIT]*DEG2RAD);
  smartcar_imu.cos_rpy[_PIT]=FastCos(smartcar_imu.rpy_deg[_PIT]*DEG2RAD);
  smartcar_imu.sin_rpy[_ROL]=FastSin(smartcar_imu.rpy_deg[_ROL]*DEG2RAD);
  smartcar_imu.cos_rpy[_ROL]=FastCos(smartcar_imu.rpy_deg[_ROL]*DEG2RAD);
  smartcar_imu.sin_rpy[_YAW]=FastSin(smartcar_imu.rpy_deg[_YAW]*DEG2RAD);
  smartcar_imu.cos_rpy[_YAW]=FastCos(smartcar_imu.rpy_deg[_YAW]*DEG2RAD);
	//提取姿态四元数
  smartcar_imu.quaternion[0]=ahrs.quaternion.element.w;
	smartcar_imu.quaternion[1]=ahrs.quaternion.element.x;
  smartcar_imu.quaternion[2]=ahrs.quaternion.element.y;
	smartcar_imu.quaternion[3]=ahrs.quaternion.element.z;
	//计算载体系到导航系旋转矩阵
	quaternion_to_cb2n(smartcar_imu.quaternion,smartcar_imu.cb2n);						  //通过四元数求取旋转矩阵	
	//将无人机在导航坐标系下的沿着正东、正北方向的运动加速度旋转到当前航向的运动加速度:机头(俯仰)+横滚 
  smartcar_imu.accel_body_cmpss.x= smartcar_imu.accel_earth_cmpss.x*smartcar_imu.cos_rpy[_YAW]+smartcar_imu.accel_earth_cmpss.y*smartcar_imu.sin_rpy[_YAW];  //横滚正向运动加速度  X轴正向
  smartcar_imu.accel_body_cmpss.y=-smartcar_imu.accel_earth_cmpss.x*smartcar_imu.sin_rpy[_YAW]+smartcar_imu.accel_earth_cmpss.y*smartcar_imu.cos_rpy[_YAW];  //机头正向运动加速度  Y轴正向
	
	//{-sinθ          cosθsin Φ                          cosθcosΦ                   }
  smartcar_imu.yaw_gyro_enu=-smartcar_imu.sin_rpy[_ROL]*smartcar_imu.gyro_dps.x
														+smartcar_imu.cos_rpy[_ROL]*smartcar_imu.sin_rpy[_PIT]*smartcar_imu.gyro_dps.y
														+smartcar_imu.cos_rpy[_PIT]*smartcar_imu.cos_rpy[_ROL]*smartcar_imu.gyro_dps.z;	
//	smartcar_imu.vbat=get_battery_voltage();
}


/***************************************************
函数名: uint8_t temperature_state_get(void)
说明:	温度接近目标值检测
入口:	无
出口:	uint8_t 就位标志
备注:	无
作者:	无名创新
****************************************************/
uint8_t temperature_state_get(void)
{
#if temperature_ctrl_enable
	return (ABS(temp_error))<=3.0f?1:0;
#else
	return 1;
#endif
  
	
}

/***************************************************
函数名: void temperature_state_check(void)
说明:	温度恒定检测
入口:	无
出口:	无
备注:	无
作者:	无名创新
****************************************************/
void temperature_state_check(void)
{
	static uint16_t _cnt=0;
	static uint16_t temperature_crash_cnt=0;
	if(temperature_state_get()==1){
		_cnt++;
		if(_cnt>=400) smartcar_imu.temperature_stable_flag=1;
	}
	else{
		_cnt/=2;
	}
	
	if(temperature_crash_cnt<400)
	{
		if(smartcar_imu.last_temperature_raw==smartcar_imu.temperature_raw)	temperature_crash_cnt++;
		else temperature_crash_cnt/=2;
		smartcar_imu.imu_health=1;		
	}
	else
	{
		smartcar_imu.imu_health=0;
	}	
}	
