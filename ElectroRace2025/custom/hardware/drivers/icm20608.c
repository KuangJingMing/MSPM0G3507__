//系统库
#include "ti_msp_dl_config.h"
//硬件I2C
#include "hardware_i2c.h"
//头文件
#include "icm20608.h"
//延时
#include "delay.h"
//MATH库
#include "fast_math.h"

#include "w25qxx.h"

#include "eeprom.h"

#include "log_config.h"
#include "log.h"

static IMU_ID_READ IMU_ID=WHO_AM_I_ICM20608D;
static uint8_t imu_address=ICM20689_ADRESS;
static uint8_t icm_read_register[5]={0x00,0x02,0x08,0x08,0x03};
static uint8_t icm_read_check[5]={0};

/***************************************
函数名:	void ICM206xx_Init(void)
说明: 加速度计/陀螺仪初始化
入口:	无
出口:	uint8_t 返回失败标志
备注:	无
作者:	无名创新
***************************************/
uint8_t ICM206xx_Init(void)//ICM20689初始化
{
	uint8_t fault=0;
	single_writei2c(imu_address,PWR_MGMT_1, 0x81);//软件强制复位81
	delay_ms(100);	
	IMU_ID=(IMU_ID_READ)(single_readi2c(imu_address,WHO_AM_I));
	switch(IMU_ID)
	{
		case WHO_AM_I_MPU6050:
		{
			single_writei2c(imu_address,PWR_MGMT_1  , 0x80);//软件复位
			delay_ms(200);
			single_writei2c(imu_address,PWR_MGMT_1  , 0x00);//软件唤醒
			delay_ms(10);
			single_writei2c(imu_address,SMPLRT_DIV  , 0x00); // sample rate.  Fsample= 1Khz/(<this value>+1) = 1000Hz
			single_writei2c(imu_address,PWR_MGMT_1  , 0x03); //时钟源 PLL with Z Gyro reference
			single_writei2c(imu_address,MPU_CONFIG  , 0x02); //内部低通滤波频率，加速度计184hz，陀螺仪188hz  //默认0x03	  
			single_writei2c(imu_address,GYRO_CONFIG , 0x08); //500deg/s
			single_writei2c(imu_address,ACCEL_CONFIG, 0x08); // Accel scale 4g (8192 LSB/g)			
		}
		break;
		case WHO_AM_I_ICM20689:
		{
			single_writei2c(imu_address,PWR_MGMT_1  , 0x00);//关闭所有中断,解除休眠
			single_writei2c(imu_address,SMPLRT_DIV  , 0x00); // sample rate.  Fsample= 1Khz/(<this value>+1) = 1000Hz	
			delay_ms(100);			
			single_writei2c(imu_address,MPU_CONFIG  , 0x02);//0x00设置陀螺仪、温度内部低通滤波频率范围，陀螺仪250hz，噪声带宽306.6hz，温度4000hz
																														//0x01设置陀螺仪、温度内部低通滤波频率范围，陀螺仪176hz，噪声带宽177hz，温度188hz
																														//0x02设置陀螺仪、温度内部低通滤波频率范围，陀螺仪92hz，噪声带宽108.6hz，温度98hz
																														//0x03设置陀螺仪、温度内部低通滤波频率范围，陀螺仪41hz，噪声带宽59hz，温度42hz		
																														//0x04设置陀螺仪、温度内部低通滤波频率范围，陀螺仪20hz，噪声带宽30.5hz，温度20hz
																														//0x05设置陀螺仪、温度内部低通滤波频率范围，陀螺仪10hz，噪声带宽15.6hz，温度10hz
			single_writei2c(imu_address,GYRO_CONFIG , 0x08);//设置陀螺仪量程，500deg/s
			single_writei2c(imu_address,ACCEL_CONFIG, 0x08);// Accel scale 4g (8192 LSB/g)	
			single_writei2c(imu_address,ACCEL_CONFIG2,0x03);
																										 //0x00设置加速度计内部低通滤波频率范围，加速度218.1hz，噪声带宽235hz		
																										 //0x01设置加速度计内部低通滤波频率范围，加速度218.1hz，噪声带宽235hz
																										 //0x02设置加速度计内部低通滤波频率范围，加速度99.0hz，噪声带宽121.3hz		
																										 //0x03设置加速度计内部低通滤波频率范围，加速度44.8hz，噪声带宽61.5hz
																										 //0x04设置加速度计内部低通滤波频率范围，加速度21.2hz，噪声带宽31.0hz
																										 //0x05设置加速度计内部低通滤波频率范围，加速度10.2hz，噪声带宽15.5hz	
			//Single_WriteI2C(imu_address,ACCEL_CONFIG2,0xC3);//设置加速度计内部低通滤波频率范围，加速度1046.0hz，噪声带宽1100.0hz
		}
		break;	
		case WHO_AM_I_ICM20608D:
		case WHO_AM_I_ICM20608G:
		case WHO_AM_I_ICM20602:
		{
			single_writei2c(imu_address,PWR_MGMT_1,0X80);	//复位ICM20608
			delay_ms(200);
			single_writei2c(imu_address,PWR_MGMT_1, 0X01);	//唤醒ICM20608
			delay_ms(10);
			single_writei2c(imu_address,0x19, 0x00);   /* 输出速率是内部采样率 */
			single_writei2c(imu_address,0x1A, 0x02);   /* 陀螺仪低通滤波BW=92Hz */
			single_writei2c(imu_address,0x1B, 0x08);   /* 陀螺仪±500dps量程 */
			single_writei2c(imu_address,0x1C, 0x08);   /* 加速度计±16G量程 */
			single_writei2c(imu_address,0x1D, 0x03);   /* 加速度计低通滤波BW=44.8Hz */
			
			single_writei2c(imu_address,0x6C, 0x00);   /* 打开加速度计和陀螺仪所有轴 */
			single_writei2c(imu_address,0x1E, 0x00);   /* 关闭低功耗 */
			single_writei2c(imu_address,0x23, 0x00);   /* 关闭FIFO */ 
			
			delay_ms(200);	
			icm_read_check[0]=single_readi2c(imu_address,0x19);
			icm_read_check[1]=single_readi2c(imu_address,0x1A);
			icm_read_check[2]=single_readi2c(imu_address,0x1B);
			icm_read_check[3]=single_readi2c(imu_address,0x1C);
			icm_read_check[4]=single_readi2c(imu_address,0x1D);
			for(uint8_t i=0;i<5;i++)
			{
				if(icm_read_check[i]!=icm_read_register[i]) fault=1;
			}	
		}
		break;
		default:
		{
			fault=1;
		}			
	}
	if (fault == 1) {
		log_e("init failed");
	}  else {
		log_i("success");
	}
	delay_ms(500);
	return fault;
}

/***************************************
函数名:	void ICM206xx_Read_Data(vector3f *gyro,vector3f *accel,float *temperature)
说明: 读取传感器加速度计/陀螺仪/温度数据
入口:	vector3f *gyro-读取三轴陀螺仪数据指针
			vector3f *accel-读取三轴加速度数据指针
			float *temperature-读取温度数据指针
出口:	无
备注:	陀螺仪单位deg/s,加速度计单位g,温度单位℃
作者:	无名创新
***************************************/
void ICM206xx_Read_Data(vector3f *gyro,vector3f *accel,float *temperature)
{
	uint8_t buf[14];
	int16_t temp;
	i2creadnbyte(imu_address,ACCEL_XOUT_H,buf,14);
	switch(IMU_ID)
	{
		case WHO_AM_I_MPU6050:
		{
			accel->x=-(int16_t)((buf[0]<<8)|buf[1]);
			accel->y=-(int16_t)((buf[2]<<8)|buf[3]);
			accel->z= (int16_t)((buf[4]<<8)|buf[5]);	
			temp		=(int16_t)((buf[6]<<8)|buf[7]);
			gyro->x	=-(int16_t)((buf[8]<<8)|buf[9]);
			gyro->y	=-(int16_t)((buf[10]<<8)|buf[11]);
			gyro->z	= (int16_t)((buf[12]<<8)|buf[13]);	
			*temperature=36.53f+(float)(temp/340.0f);	
		}
		break;
		case WHO_AM_I_ICM20689:
		{
			accel->x=(int16_t)((buf[0]<<8)|buf[1]);
			accel->y=-(int16_t)((buf[2]<<8)|buf[3]);
			accel->z=-(int16_t)((buf[4]<<8)|buf[5]);	
			temp		=(int16_t)((buf[6]<<8)|buf[7]);
			gyro->x	=(int16_t)((buf[8]<<8)|buf[9]);
			gyro->y	=-(int16_t)((buf[10]<<8)|buf[11]);
			gyro->z	=-(int16_t)((buf[12]<<8)|buf[13]);	
			*temperature=25.0f+(double)((temp-25.0f)/326.8f);	
		}
		break;	
		case WHO_AM_I_ICM20608D:
		case WHO_AM_I_ICM20608G:
		case WHO_AM_I_ICM20602:				
		{
			accel->x=-(int16_t)((buf[0]<<8)|buf[1]);
			accel->y= (int16_t)((buf[2]<<8)|buf[3]);
			accel->z=-(int16_t)((buf[4]<<8)|buf[5]);	
			temp		=(int16_t)((buf[6]<<8)|buf[7]);
			gyro->x	=-(int16_t)((buf[8]<<8)|buf[9]);
			gyro->y	= (int16_t)((buf[10]<<8)|buf[11]);
			gyro->z	=-(int16_t)((buf[12]<<8)|buf[13]);	
			
			*temperature=25.0f+(double)((temp-25.0f)/326.8f);		
		}
		break;
		default:
		{
			accel->x=-(int16_t)((buf[0]<<8)|buf[1]);
			accel->y=-(int16_t)((buf[2]<<8)|buf[3]);
			accel->z= (int16_t)((buf[4]<<8)|buf[5]);	
			temp		=(int16_t)((buf[6]<<8)|buf[7]);
			gyro->x	=-(int16_t)((buf[8]<<8)|buf[9]);
			gyro->y	=-(int16_t)((buf[10]<<8)|buf[11]);
			gyro->z	= (int16_t)((buf[12]<<8)|buf[13]);	
			*temperature=36.53f+(float)(temp/340.0f);				
		}			
	}
	gyro->x*=GYRO_CALIBRATION_COFF;
	gyro->y*=GYRO_CALIBRATION_COFF;
	gyro->z*=GYRO_CALIBRATION_COFF;
	
  accel->x/=GRAVITY_RAW;
	accel->y/=GRAVITY_RAW;
	accel->z/=GRAVITY_RAW;
}



