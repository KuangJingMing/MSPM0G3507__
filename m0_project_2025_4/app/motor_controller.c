#include "motor_controller.h"
#include "icm20608.h"
#include "delay.h"
#include "sensor.h"
#include "log_config.h"
#include "log.h"
#include "FusionAhrs.h" // ??AHRS???
#include <math.h>

static vector3f gyro, accel;
static float temperature;
static FusionAhrs ahrs; // AHRS?????

/**
 * @brief ??????????(??:?)
 * @param q ??????
 * @param roll ?????(?)
 * @param pitch ?????(?)
 * @param yaw ?????(?)
 */
void QuaternionToEulerAngles(FusionQuaternion q, float *roll, float *pitch, float *yaw) {
    float qw = q.element.w;
    float qx = q.element.x;
    float qy = q.element.y;
    float qz = q.element.z;

    // Roll (x-axis rotation)
    float sinr_cosp = 2.0f * (qw * qx + qy * qz);
    float cosr_cosp = 1.0f - 2.0f * (qx * qx + qy * qy);
    *roll = atan2f(sinr_cosp, cosr_cosp);

    // Pitch (y-axis rotation)
    float sinp = 2.0f * (qw * qy - qz * qx);
    if (fabsf(sinp) >= 1.0f) {
        *pitch = copysignf(M_PI / 2.0f, sinp); // ??????,??90?
    } else {
        *pitch = asinf(sinp);
    }

    // Yaw (z-axis rotation)
    float siny_cosp = 2.0f * (qw * qz + qx * qy);
    float cosy_cosp = 1.0f - 2.0f * (qy * qy + qz * qz);
    *yaw = atan2f(siny_cosp, cosy_cosp);

    // ????????
    *roll = (*roll) * (180.0f / M_PI);
    *pitch = (*pitch) * (180.0f / M_PI);
    *yaw = (*yaw) * (180.0f / M_PI);
}

// ????:?????????OLED??,??????????????
void DisplayTask(void *pvParameters)
{
    // 1. ???IMU
    if(ICM206xx_Init() != 0) {
        log_e("IMU init failed");
        vTaskDelete(NULL); // ??????
    }
    
    // 2. ???????(?EEPROM??)
    imu_calibration_params_init();

    // 3. ???AHRS??
    FusionAhrsInitialise(&ahrs);

    for (;;) {
        // 4. ?????????
        ICM206xx_Read_Data(&gyro, &accel, &temperature);

        // 5. ???????????????(????)
        imu_calibration(&gyro, &accel);

        // 6. ????????????????(?????)
        vector3f corrected_gyro;
        corrected_gyro.x = gyro.x - smartcar_imu.gyro_offset.x;
        corrected_gyro.y = gyro.y - smartcar_imu.gyro_offset.y;
        corrected_gyro.z = gyro.z - smartcar_imu.gyro_offset.z;

        vector3f corrected_accel;
        corrected_accel.x = accel.x - smartcar_imu.accel_offset.x;
        corrected_accel.y = accel.y - smartcar_imu.accel_offset.y;
        corrected_accel.z = accel.z - smartcar_imu.accel_offset.z;
        FusionVector fusion_gyro = {
            .axis.x = corrected_gyro.x,
            .axis.y = corrected_gyro.y,
            .axis.z = corrected_gyro.z
        };
        FusionVector fusion_accel = {
            .axis.x = corrected_accel.x,
            .axis.y = corrected_accel.y,
            .axis.z = corrected_accel.z
        };
        FusionVector fusion_mag = FUSION_VECTOR_ZERO; // ???????,?????

        // 8. ??AHRS??(???????300ms,?0.3s)
        float deltaTime = 0.3f; // ????,??:?
        FusionAhrsUpdateNoMagnetometer(&ahrs, fusion_gyro, fusion_accel, deltaTime);

        // 9. ?????????(??)
        FusionQuaternion quaternion = FusionAhrsGetQuaternion(&ahrs);

        // 10. ??????????
        float roll, pitch, yaw;
        QuaternionToEulerAngles(quaternion, &roll, &pitch, &yaw);

        log_i("roll=%.2f pitch=%.2f yaw=%.2f", 
              roll, pitch, yaw);

        delay_ms(300);
    }
}