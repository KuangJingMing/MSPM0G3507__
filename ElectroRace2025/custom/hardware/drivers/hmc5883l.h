/* hmc5883l.h */
#ifndef HMC5883L_H
#define HMC5883L_H

#include <stdint.h>

void hmc5883l_init(void);
void hmc5883l_soft_reset(void);
void hmc5883l_read_xyz(int16_t *x, int16_t *y, int16_t *z);

// 新增函数
void hmc5883l_set_calibration(float offset_x, float offset_y, float offset_z, 
                             float scale_x, float scale_y, float scale_z);
void hmc5883l_get_calibrated_xyz(float *x, float *y, float *z);
float hmc5883l_get_heading(void);
float hmc5883l_get_heading_tilt_compensated(float pitch, float roll);
float hmc5883l_get_true_heading(float magnetic_heading, float declination);

#endif /* HMC5883L_H */
