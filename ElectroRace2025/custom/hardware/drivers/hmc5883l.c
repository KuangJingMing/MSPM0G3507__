/* hmc5883l.c */
#include "hmc5883l.h"
#include "math.h"
#include "sw_i2c.h"
#include "ti_msp_dl_config.h"

#define HMC5883L_ADDR    0x1E  // 7位I2C地址
#define M_PI 3.1415926

// 软件I2C配置。请根据自己硬件更改端口引脚
static sw_i2c_t hmc5883l_i2c = {
    .sclPort = PORTA_PORT,
    .sclPin  = PORTA_HMC5883L_SCL_PIN,
    .sclIOMUX= PORTA_HMC5883L_SCL_IOMUX,
    .sdaPort = PORTA_PORT,
    .sdaPin  = PORTA_HMC5883L_SDA_PIN,
    .sdaIOMUX= PORTA_HMC5883L_SDA_IOMUX,
};

// 校准参数(需要根据实际环境调整)
static float mag_offset_x = 0.0f;
static float mag_offset_y = 0.0f;
static float mag_offset_z = 0.0f;
static float mag_scale_x = 1.0f;
static float mag_scale_y = 1.0f;
static float mag_scale_z = 1.0f;

static void hmc5883l_write_reg(uint8_t reg, uint8_t data)
{
    SOFT_IIC_Write_Len(&hmc5883l_i2c, HMC5883L_ADDR, reg, 1, &data);
}

static void hmc5883l_read(uint8_t reg, uint8_t *buf, uint8_t len)
{
    SOFT_IIC_Read_Len(&hmc5883l_i2c, HMC5883L_ADDR, reg, len, buf);
}

void hmc5883l_soft_reset(void)
{
    // 更保险：重新初始化所有寄存器
    hmc5883l_write_reg(0x00, 0x58);  // Configuration Register A: 8采样平均, 15Hz输出率
    hmc5883l_write_reg(0x01, 0x20);  // Configuration Register B: 增益设置为1.3Ga
    hmc5883l_write_reg(0x02, 0x00);  // Mode Register: continuous mode
}

void hmc5883l_init(void)
{
    SOFT_IIC_Init(&hmc5883l_i2c);
    hmc5883l_soft_reset();
}

void hmc5883l_read_xyz(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t buf[6];
    hmc5883l_read(0x03, buf, 6); // 数据顺序 X:[0][1],Z:[2][3],Y:[4][5]
    *x = (int16_t)((buf[0] << 8) | buf[1]);
    *z = (int16_t)((buf[2] << 8) | buf[3]);
    *y = (int16_t)((buf[4] << 8) | buf[5]);
}

void hmc5883l_set_calibration(float offset_x, float offset_y, float offset_z, 
                             float scale_x, float scale_y, float scale_z)
{
    // 设置校准参数
    mag_offset_x = offset_x;
    mag_offset_y = offset_y;
    mag_offset_z = offset_z;
    mag_scale_x = scale_x;
    mag_scale_y = scale_y;
    mag_scale_z = scale_z;
}

void hmc5883l_get_calibrated_xyz(float *x, float *y, float *z)
{
    int16_t raw_x, raw_y, raw_z;
    
    // 获取原始数据
    hmc5883l_read_xyz(&raw_x, &raw_y, &raw_z);
    
    // 应用校准
    *x = (float)raw_x - mag_offset_x;
    *y = (float)raw_y - mag_offset_y;
    *z = (float)raw_z - mag_offset_z;
    
    *x *= mag_scale_x;
    *y *= mag_scale_y;
    *z *= mag_scale_z;
}

float hmc5883l_get_heading(void)
{
    float x, y, z;
    float heading;
    
    // 获取校准后的数据
    hmc5883l_get_calibrated_xyz(&x, &y, &z);
    
    // 计算航向角(yaw)，使用反正切函数计算x-y平面上的角度
    heading = atan2f(y, x);
    
    // 调整为角度值
    heading = heading * 180.0f / M_PI;
    
    // 确保角度范围在0-360度
    if (heading < 0) {
        heading += 360.0f;
    }
    
    return heading;
}

float hmc5883l_get_heading_tilt_compensated(float pitch, float roll)
{
    float x, y, z;
    float cos_roll, sin_roll, cos_pitch, sin_pitch;
    float x_h, y_h;
    float heading;
    
    // 获取校准后的数据
    hmc5883l_get_calibrated_xyz(&x, &y, &z);
    
    // 转换弧度
    cos_roll = cosf(roll * M_PI / 180.0f);
    sin_roll = sinf(roll * M_PI / 180.0f);
    cos_pitch = cosf(pitch * M_PI / 180.0f);
    sin_pitch = sinf(pitch * M_PI / 180.0f);
    
    // 倾斜补偿计算
    x_h = x * cos_pitch + z * sin_pitch;
    y_h = x * sin_roll * sin_pitch + y * cos_roll - z * sin_roll * cos_pitch;
    
    // 计算航向角
    heading = atan2f(y_h, x_h);
    
    // 转换为角度
    heading = heading * 180.0f / M_PI;
    
    // 确保角度范围在0-360度
    if (heading < 0) {
        heading += 360.0f;
    }
    
    return heading;
}

// 计算磁偏角校正后的真北方向
float hmc5883l_get_true_heading(float magnetic_heading, float declination)
{
    // 加上磁偏角得到真北方向
    float true_heading = magnetic_heading + declination;
    
    // 确保角度在0-360度范围内
    if (true_heading < 0) {
        true_heading += 360.0f;
    } else if (true_heading >= 360.0f) {
        true_heading -= 360.0f;
    }
    
    return true_heading;
}
