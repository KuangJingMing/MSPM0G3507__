#ifndef IMU_TEMP_CONTROL_H__
#define IMU_TEMP_CONTROL_H__

#include "common_include.h" // 包含项目通用的头文件，例如 FreeRTOS、DSP 库、GPIO 等
#include "arm_math.h"       // 需要包含 CMSIS-DSP 头文件，因为我们使用了 arm_pid_instance_f32 等类型


// 声明 PID 控制器实例
extern arm_pid_instance_f32 S_temp;

void temperature_state_check(void); // 温度恒定检测
void init_temp_pid(void); // 初始化温度 PID 控制器

void imu_temperature_ctrl_init(void);
void imu_temperature_ctrl_task(void);



#endif // PERIODIC_EVENT_TASK_H
