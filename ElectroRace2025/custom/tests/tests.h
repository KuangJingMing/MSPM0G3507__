#ifndef TESTS_H__
#define TESTS_H__

// AT24CXX 单独测试
void at24cxx_single_test(void);
// 陀螺仪单独测试
void imu_task_create(void) ;
// 巡线模块单独测试
void gd_task_create(void);
// 磁力计单独测试
void hmc5883l_test_task_start(void);
// MPU6050单独测试
void mpu6050_task_create(void);

#endif
