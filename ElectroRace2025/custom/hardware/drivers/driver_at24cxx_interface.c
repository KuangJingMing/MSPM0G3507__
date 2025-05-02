#include "log_config.h"
#include "log.h"
#include "driver_at24cxx_interface.h"
#include "software_i2c.h"
#include "stdarg.h"
#include "stdio.h"

SoftwareI2C at24cxx_software_i2c = {
    .sdaPort = GPIO_I2C_1_SDA_PORT,
    .sdaPin = GPIO_I2C_1_SDA_PIN,
    .sdaIOMUX = GPIO_I2C_1_IOMUX_SDA,
    .sclPort = GPIO_I2C_1_SCL_PORT,
    .sclPin = GPIO_I2C_1_SCL_PIN,
    .sclIOMUX = GPIO_I2C_1_IOMUX_SCL,
    .delay_us = 1,
    .timeout_us = 1000
};

uint8_t at24cxx_interface_iic_init(void)
{
    log_i("AT24CXX interface iic init.");
    SoftwareI2C_Init(&at24cxx_software_i2c);
    return 0;
}

uint8_t at24cxx_interface_iic_deinit(void)
{
    log_i("AT24CXX interface iic deinit.");
    return 0;
}

uint8_t at24cxx_interface_iic_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    log_i("AT24CXX interface iic read: addr=0x%02X (8-bit), reg=0x%02X, len=%d", addr >> 1, reg, len);
    uint8_t result = SoftWareI2C_Read_Len(&at24cxx_software_i2c, addr >> 1, reg, (uint8_t)len, buf);
    if (result != I2C_SUCCESS) {
        log_e("Software I2C read failed with error code: %d", result);
        return 1;
    }
    return 0;
}

uint8_t at24cxx_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    log_i("AT24CXX interface iic write: addr=0x%02X (8-bit), reg=0x%02X, len=%d", addr >> 1, reg, len);
    uint8_t result = SoftWareI2C_Write_Len(&at24cxx_software_i2c, addr >> 1, reg, (uint8_t)len, buf);
    if (result != I2C_SUCCESS) {
        log_e("Software I2C write failed with error code: %d", result);
        return 1;
    }
    return 0;
}

uint8_t at24cxx_interface_iic_read_address16(uint8_t addr, uint16_t reg, uint8_t *buf, uint16_t len)
{
    log_w("AT24CXX interface iic read with 16-bit address called. AT24C16 only supports 8-bit address.");
    return 1;
}

uint8_t at24cxx_interface_iic_write_address16(uint8_t addr, uint16_t reg, uint8_t *buf, uint16_t len)
{
    log_w("AT24CXX interface iic write with 16-bit address called. AT24C16 only supports 8-bit address.");
    return 1;
}

void at24cxx_interface_delay_ms(uint32_t ms)
{
    if (ms > 0) {
        delay_ms(ms);
    }
}

void at24cxx_interface_debug_print(const char *const fmt, ...)
{
    #define DEBUG_BUFFER_SIZE 256
    char buffer[DEBUG_BUFFER_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, DEBUG_BUFFER_SIZE, fmt, args);
    va_end(args);
    log_i("%s", buffer);
}
