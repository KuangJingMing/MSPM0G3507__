#include "imu_app.h"
#include "log_config.h"
#include "log.h"
#include "rgb_led.h"
#include "beep.h"
#include "alert.h"
#include <math.h>
#include "periodic_event_task.h"
#include "common_defines.h"

// 生成一个单精度浮点数 NaN 的宏，按照 IEEE 754 标准
#define MY_NAN ( *(float *)(uint32_t[]){0x7FC00000} )

// 定义IMU基础参数
#define IMU_SAMPLING_PERIOD_MS          5.0f    // 采样周期 5ms (基础参数，用户只需修改此值)

// 基于采样周期自动计算的参数
#define IMU_SAMPLING_FREQUENCY_HZ       (1000.0f / IMU_SAMPLING_PERIOD_MS) // 采样频率 (1000ms / 周期)
#define IMU_GYRO_DELTA_DPS              3.0f    // 陀螺仪变化阈值
#define IMU_GYRO_LPF_CUTOFF_HZ         50.0f   // 陀螺仪低通滤波截止频率
#define IMU_ACCEL_LPF_CUTOFF_HZ        30.0f   // 加速度计低通滤波截止频率
#define IMU_CALIBRATION_TIME_S          2.0f    // 校准时间 2秒
#define IMU_CALIBRATION_SAMPLES         (uint16_t)(IMU_CALIBRATION_TIME_S * IMU_SAMPLING_FREQUENCY_HZ) // 校准采样次数
#define IMU_CONVERGENCE_TIME_S          5.0f    // IMU收敛时间 5秒
#define IMU_CONVERGENCE_SAMPLES         (uint32_t)(IMU_CONVERGENCE_TIME_S * IMU_SAMPLING_FREQUENCY_HZ) // IMU收敛采样次数
#define IMU_RECOVERY_TRIGGER_TIME_S     5.0f    // 恢复触发时间 5秒
#define IMU_RECOVERY_TRIGGER_PERIOD     (uint32_t)(IMU_RECOVERY_TRIGGER_TIME_S * IMU_SAMPLING_FREQUENCY_HZ) // 恢复触发周期
#define IMU_TEMPERATURE_FILTER_ALPHA    0.75f   // 温度滤波系数
#define IMU_TEMPERATURE_FILTER_BETA     0.25f   // 温度滤波系数补值
#define IMU_AHRS_GAIN                   0.5f    // AHRS增益
#define IMU_ACCEL_REJECTION             10.0f   // 加速度拒绝值
#define IMU_MAGNETIC_REJECTION          20.0f   // 磁力计拒绝值
#define IMU_GRAVITY_MSS                 9.81f   // 重力加速度 (m/s^2)
#define IMU_GRAVITY_CMSS                (IMU_GRAVITY_MSS * 100.0f) // 重力加速度 (cm/s^2)
#define IMU_DEG_TO_RAD                  0.0174533f // 角度转弧度 (π/180)
#define IMU_RAD_TO_DEG                  57.29578f  // 弧度转角度 (180/π)

imu_data_t smartcar_imu;

static lpf_param accel_lpf_param, gyro_lpf_param;
static lpf_buf gyro_filter_buf[3], accel_filter_buf[3];
static FusionAhrs ahrs;
static FusionOffset offset;

void imu_init_blocking(void) {
		static uint8_t cnt = 0;
    while (ICM206xx_Init()) {
        log_i("imu init ...");
				if (++cnt >= 5) {
						log_i("imu init time_out");
						return;
				}
        delay_ms(500);
    }
    imu_calibration_params_init();
		log_i("imu_init ok");
}

void imu_update_task(void) {
    imu_data_sampling();
    trackless_ahrs_update();
}

/***************************************
函数名: void imu_calibration_params_init(void)
说明: 加速度计/陀螺仪标定数据初始化
入口: 无
出口: 无
备注: 先从eeprom读取数据,若数据不存在，等待温度就位后再标定
作者: 无名创新
***************************************/
void imu_calibration_params_init(void) {
    vector3f gyro_offset_temp = {MY_NAN, MY_NAN, MY_NAN};
    vector3f accel_offset_temp = {MY_NAN, MY_NAN, MY_NAN};
#if USE_EEPROOM
    ReadFlashParameterOne(GYRO_X_OFFSET, &gyro_offset_temp.x);
    ReadFlashParameterOne(GYRO_Y_OFFSET, &gyro_offset_temp.y);
    ReadFlashParameterOne(GYRO_Z_OFFSET, &gyro_offset_temp.z);      
    ReadFlashParameterOne(ACCEL_X_OFFSET, &accel_offset_temp.x);
    ReadFlashParameterOne(ACCEL_Y_OFFSET, &accel_offset_temp.y);
    ReadFlashParameterOne(ACCEL_Z_OFFSET, &accel_offset_temp.z);
#endif
    if (isnan(gyro_offset_temp.x) == 0
        && isnan(gyro_offset_temp.y) == 0
        && isnan(gyro_offset_temp.z) == 0) { // 如果之前已经温度校准过，开机时直接用之前校准的数据
        smartcar_imu.gyro_offset.x = gyro_offset_temp.x;
        smartcar_imu.gyro_offset.y = gyro_offset_temp.y;
        smartcar_imu.gyro_offset.z = gyro_offset_temp.z;
        smartcar_imu.imu_cal_flag = 1;
    } else {
        smartcar_imu.gyro_offset.x = 0;
        smartcar_imu.gyro_offset.y = 0;
        smartcar_imu.gyro_offset.z = 0;
    }
    // 待温度稳定后，自动校准陀螺仪零偏
    smartcar_imu.accel_scale.x = 1.0f;
    smartcar_imu.accel_scale.y = 1.0f;
    smartcar_imu.accel_scale.z = 1.0f;

    if (isnan(accel_offset_temp.x) == 0
        && isnan(accel_offset_temp.y) == 0
        && isnan(accel_offset_temp.z) == 0) { // 如果之前已经温度校准过，开机时直接用之前校准的数据
        smartcar_imu.accel_offset.x = accel_offset_temp.x;
        smartcar_imu.accel_offset.y = accel_offset_temp.y;
        smartcar_imu.accel_offset.z = accel_offset_temp.z;
        smartcar_imu.imu_cal_flag = 1;
    } else {
        smartcar_imu.accel_offset.x = 0;
        smartcar_imu.accel_offset.y = 0;
        smartcar_imu.accel_offset.z = 0;
    }   
    if (smartcar_imu.imu_cal_flag) {
        // 后续逻辑可添加
    }
}

/***************************************
函数名: void imu_calibration(vector3f *gyro, vector3f *accel)
说明: 加速度计/陀螺仪标定
入口: 无
出口: 无
备注: 无
作者: 无名创新
***************************************/
void imu_calibration(vector3f *gyro, vector3f *accel) {
    if (smartcar_imu.imu_cal_flag == 1) return;
#if TEMPERATURE_CTRL_ENABLE
    if (smartcar_imu.temperature_stable_flag == 0) return;
#endif	
    static uint16_t cnt = 0;
    static vector3f last_gyro;
    static vector3f accel_sum, gyro_sum;
    if (ABS(gyro->x - last_gyro.x) <= IMU_GYRO_DELTA_DPS
        && ABS(gyro->y - last_gyro.y) <= IMU_GYRO_DELTA_DPS
        && ABS(gyro->z - last_gyro.z) <= IMU_GYRO_DELTA_DPS
        ) {
        gyro_sum.x += gyro->x;
        gyro_sum.y += gyro->y;
        gyro_sum.z += gyro->z;
        accel_sum.x += accel->x;
        accel_sum.y += accel->y;
        accel_sum.z += accel->z;
        cnt++;
    } else {
        gyro_sum.x = 0;
        gyro_sum.y = 0;
        gyro_sum.z = 0;
        accel_sum.x = 0;
        accel_sum.y = 0;
        accel_sum.z = 0;
        cnt = 0;
    }
    last_gyro.x = gyro->x;
    last_gyro.y = gyro->y;
    last_gyro.z = gyro->z;

    if (cnt >= IMU_CALIBRATION_SAMPLES) { // 持续满足指定采样次数
        smartcar_imu.gyro_offset.x = (gyro_sum.x / cnt); // 得到标定偏移
        smartcar_imu.gyro_offset.y = (gyro_sum.y / cnt);
        smartcar_imu.gyro_offset.z = (gyro_sum.z / cnt);
        
        smartcar_imu.accel_offset.x = (accel_sum.x / cnt); // 得到标定偏移
        smartcar_imu.accel_offset.y = (accel_sum.y / cnt);
        smartcar_imu.accel_offset.z = (accel_sum.z / cnt) - safe_sqrt(1 - sq2(smartcar_imu.accel_offset.x) - sq2(smartcar_imu.accel_offset.y));
#if USE_EEPROOM            
        WriteFlashParameter_Three(GYRO_X_OFFSET,
                                  smartcar_imu.gyro_offset.x,
                                  smartcar_imu.gyro_offset.y,
                                  smartcar_imu.gyro_offset.z);       
        WriteFlashParameter_Three(ACCEL_X_OFFSET,
                                  smartcar_imu.accel_offset.x,
                                  smartcar_imu.accel_offset.y,
                                  smartcar_imu.accel_offset.z);   
#endif       
        gyro_sum.x = 0;
        gyro_sum.y = 0;
        gyro_sum.z = 0;
        accel_sum.x = 0;
        accel_sum.y = 0;
        accel_sum.z = 0;        
        cnt = 0;
        
        set_alert_count(1);
        start_alert();
        
        smartcar_imu.imu_cal_flag = 1;
    }
}

/***************************************************
函数名: void imu_data_sampling(void)
说明: IMU数据采样/校准/滤波
入口: 无
出口: 无
备注: 无
作者: 无名创新
****************************************************/
void imu_data_sampling(void) {
    if (smartcar_imu.lpf_init == 0) {
        set_cutoff_frequency(IMU_SAMPLING_FREQUENCY_HZ, IMU_GYRO_LPF_CUTOFF_HZ, &gyro_lpf_param); // 姿态角速度反馈滤波参数
        set_cutoff_frequency(IMU_SAMPLING_FREQUENCY_HZ, IMU_ACCEL_LPF_CUTOFF_HZ, &accel_lpf_param); // 姿态解算加计修正滤波值
        smartcar_imu.lpf_init = 1;
    }
    smartcar_imu.last_temperature_raw = smartcar_imu.temperature_raw;
    // 陀螺仪/加速度计数据采集
    ICM206xx_Read_Data(&smartcar_imu._gyro_dps_raw, &smartcar_imu._accel_g_raw, &smartcar_imu.temperature_raw);
    // 陀螺仪数据低通滤波
    smartcar_imu.gyro_dps_raw.x = LPButterworth(smartcar_imu._gyro_dps_raw.x, &gyro_filter_buf[0], &gyro_lpf_param);
    smartcar_imu.gyro_dps_raw.y = LPButterworth(smartcar_imu._gyro_dps_raw.y, &gyro_filter_buf[1], &gyro_lpf_param);
    smartcar_imu.gyro_dps_raw.z = LPButterworth(smartcar_imu._gyro_dps_raw.z, &gyro_filter_buf[2], &gyro_lpf_param);      
    // 加速度数据低通滤波
    smartcar_imu.accel_g_raw.x = LPButterworth(smartcar_imu._accel_g_raw.x, &accel_filter_buf[0], &accel_lpf_param);
    smartcar_imu.accel_g_raw.y = LPButterworth(smartcar_imu._accel_g_raw.y, &accel_filter_buf[1], &accel_lpf_param);
    smartcar_imu.accel_g_raw.z = LPButterworth(smartcar_imu._accel_g_raw.z, &accel_filter_buf[2], &accel_lpf_param);   
    // 温度传感器数据一阶低通滤波
    smartcar_imu.temperature_filter = IMU_TEMPERATURE_FILTER_ALPHA * smartcar_imu.temperature_raw + IMU_TEMPERATURE_FILTER_BETA * smartcar_imu.temperature_filter;
    // 得到校准后的角速度、加速度数据
    vector3f_sub(smartcar_imu.gyro_dps_raw, smartcar_imu.gyro_offset, &smartcar_imu.gyro_dps);
    
    smartcar_imu.accel_g.x = smartcar_imu.accel_scale.x * smartcar_imu.accel_g_raw.x - smartcar_imu.accel_offset.x;
    smartcar_imu.accel_g.y = smartcar_imu.accel_scale.y * smartcar_imu.accel_g_raw.y - smartcar_imu.accel_offset.y;
    smartcar_imu.accel_g.z = smartcar_imu.accel_scale.z * smartcar_imu.accel_g_raw.z - smartcar_imu.accel_offset.z;     
    // 加速度计/陀螺仪校准检测
    imu_calibration(&smartcar_imu.gyro_dps_raw, &smartcar_imu.accel_g_raw);
    
    // 通过三轴加速度计,计算水平观测角度
    float ax, ay, az;
    ax = smartcar_imu.accel_g.x;
    ay = smartcar_imu.accel_g.y;
    az = smartcar_imu.accel_g.z;
    
    smartcar_imu.rpy_obs_deg[0] = -IMU_RAD_TO_DEG * atan(ax * invSqrt(ay * ay + az * az)); // 横滚角
    smartcar_imu.rpy_obs_deg[1] = IMU_RAD_TO_DEG * atan(ay * invSqrt(ax * ax + az * az)); // 俯仰角
    // 俯仰轴姿态角卡尔曼滤波
    smartcar_imu.rpy_kalman_deg[1] = kalman_filter(smartcar_imu.rpy_obs_deg[1], smartcar_imu.gyro_dps.x);
}

/***************************************************
函数名: void trackless_ahrs_update(void)
说明: 姿态更新
入口: 无
出口: 无
备注: 无
作者: 无名创新
****************************************************/
void trackless_ahrs_update(void) {
    /****************************************************/
	  if (smartcar_imu.imu_cal_flag == 0) return; //没初始化完则不采集
	
    FusionVector gyroscope = {0.0f, 0.0f, 0.0f};
    FusionVector accelerometer = {0.0f, 0.0f, 1.0f};
    FusionVector earthacceleration = {0.0f, 0.0f, 0.0f}; 
    
    gyroscope.axis.x = smartcar_imu.gyro_dps.x;
    gyroscope.axis.y = smartcar_imu.gyro_dps.y;
    gyroscope.axis.z = smartcar_imu.gyro_dps.z;
    
    accelerometer.axis.x = smartcar_imu.accel_g.x;
    accelerometer.axis.y = smartcar_imu.accel_g.y;
    accelerometer.axis.z = smartcar_imu.accel_g.z;
    if (smartcar_imu.quaternion_init_ok == 0) {
#if TEMPERATURE_CTRL_ENABLE
        if (smartcar_imu.temperature_stable_flag == 0) return;
#endif
            calculate_quaternion_init(smartcar_imu.accel_g, smartcar_imu.mag_tesla, smartcar_imu.quaternion_init);
            smartcar_imu.quaternion_init_ok = 1;
            // AHRS初始化
            FusionOffsetInitialise(&offset, IMU_SAMPLING_FREQUENCY_HZ);
            FusionAhrsInitialise(&ahrs);
            // Set AHRS algorithm settings
            const FusionAhrsSettings settings = {
                .gain = IMU_AHRS_GAIN,
                .accelerationRejection = IMU_ACCEL_REJECTION,
                .magneticRejection = IMU_MAGNETIC_REJECTION,
                .recoveryTriggerPeriod = IMU_RECOVERY_TRIGGER_PERIOD, /* 基于采样频率计算 */
            };
            FusionAhrsSetSettings(&ahrs, &settings);
            set_alert_count(2);
            start_alert();
//            enable_periodic_task(EVENT_CAR);   
    }

    if (smartcar_imu.quaternion_init_ok == 1) {
        gyroscope = FusionOffsetUpdate(&offset, gyroscope);
        FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, IMU_SAMPLING_PERIOD_MS / 1000.0f); // 更新周期转换为秒
        FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
        earthacceleration = FusionAhrsGetEarthAcceleration(&ahrs);
        smartcar_imu.rpy_deg[_ROL] = euler.angle.pitch;
        smartcar_imu.rpy_deg[_PIT] = euler.angle.roll;
        smartcar_imu.rpy_deg[_YAW] = euler.angle.yaw;
        // 获取导航系系统运动加速度
        smartcar_imu.accel_earth_cmpss.x = earthacceleration.axis.x * IMU_GRAVITY_CMSS;
        smartcar_imu.accel_earth_cmpss.y = earthacceleration.axis.y * IMU_GRAVITY_CMSS;
        smartcar_imu.accel_earth_cmpss.z = earthacceleration.axis.z * IMU_GRAVITY_CMSS;
        
        if (smartcar_imu.imu_convergence_cnt < IMU_CONVERGENCE_SAMPLES) {
            smartcar_imu.imu_convergence_cnt++;
        } else if (smartcar_imu.imu_convergence_cnt == IMU_CONVERGENCE_SAMPLES) {
            smartcar_imu.imu_convergence_cnt++;
        } else {
            smartcar_imu.imu_convergence_flag = 1;
        }
    }

    smartcar_imu.rpy_gyro_dps[_PIT] = smartcar_imu.gyro_dps.x;
    smartcar_imu.rpy_gyro_dps[_ROL] = smartcar_imu.gyro_dps.y;
    smartcar_imu.rpy_gyro_dps[_YAW] = smartcar_imu.gyro_dps.z;
    // 计算姿态相关三角函数
    smartcar_imu.sin_rpy[_PIT] = FastSin(smartcar_imu.rpy_deg[_PIT] * IMU_DEG_TO_RAD);
    smartcar_imu.cos_rpy[_PIT] = FastCos(smartcar_imu.rpy_deg[_PIT] * IMU_DEG_TO_RAD);
    smartcar_imu.sin_rpy[_ROL] = FastSin(smartcar_imu.rpy_deg[_ROL] * IMU_DEG_TO_RAD);
    smartcar_imu.cos_rpy[_ROL] = FastCos(smartcar_imu.rpy_deg[_ROL] * IMU_DEG_TO_RAD);
    smartcar_imu.sin_rpy[_YAW] = FastSin(smartcar_imu.rpy_deg[_YAW] * IMU_DEG_TO_RAD);
    smartcar_imu.cos_rpy[_YAW] = FastCos(smartcar_imu.rpy_deg[_YAW] * IMU_DEG_TO_RAD);
    // 提取姿态四元数
    smartcar_imu.quaternion[0] = ahrs.quaternion.element.w;
    smartcar_imu.quaternion[1] = ahrs.quaternion.element.x;
    smartcar_imu.quaternion[2] = ahrs.quaternion.element.y;
    smartcar_imu.quaternion[3] = ahrs.quaternion.element.z;
    // 计算载体系到导航系旋转矩阵
    quaternion_to_cb2n(smartcar_imu.quaternion, smartcar_imu.cb2n); // 通过四元数求取旋转矩阵   
    // 将无人机在导航坐标系下的沿着正东、正北方向的运动加速度旋转到当前航向的运动加速度:机头(俯仰)+横滚
    smartcar_imu.accel_body_cmpss.x = smartcar_imu.accel_earth_cmpss.x * smartcar_imu.cos_rpy[_YAW] + smartcar_imu.accel_earth_cmpss.y * smartcar_imu.sin_rpy[_YAW]; // 横滚正向运动加速度 X轴正向
    smartcar_imu.accel_body_cmpss.y = -smartcar_imu.accel_earth_cmpss.x * smartcar_imu.sin_rpy[_YAW] + smartcar_imu.accel_earth_cmpss.y * smartcar_imu.cos_rpy[_YAW]; // 机头正向运动加速度 Y轴正向
    
    // {-sinθ cosθsin Φ cosθcosΦ}
    smartcar_imu.yaw_gyro_enu = -smartcar_imu.sin_rpy[_ROL] * smartcar_imu.gyro_dps.x
                              + smartcar_imu.cos_rpy[_ROL] * smartcar_imu.sin_rpy[_PIT] * smartcar_imu.gyro_dps.y
                              + smartcar_imu.cos_rpy[_PIT] * smartcar_imu.cos_rpy[_ROL] * smartcar_imu.gyro_dps.z;   
}
