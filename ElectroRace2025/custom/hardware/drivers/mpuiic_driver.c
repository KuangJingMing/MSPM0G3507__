#include "mpuiic_driver.h"
#include "sw_i2c.h"

sw_i2c_t mpu_i2c_cfg = {
    .sclPort = PORTA_PORT,
    .sclPin  = PORTA_HMC5883L_SCL_PIN,
    .sclIOMUX= PORTA_HMC5883L_SCL_IOMUX,
    .sdaPort = PORTA_PORT,
    .sdaPin  = PORTA_HMC5883L_SDA_PIN,
    .sdaIOMUX= PORTA_HMC5883L_SDA_IOMUX,
};

uint8_t MPU_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	return SOFT_IIC_Write_Len(&mpu_i2c_cfg, addr, reg, len, buf);
}
uint8_t MPU_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{ 
	return SOFT_IIC_Write_Len(&mpu_i2c_cfg, addr, reg, len, buf);
}