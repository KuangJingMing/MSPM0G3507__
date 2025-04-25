#include "software_i2c.h"

//#include "log_config.h"
#include "log.h"

// Initialize the I2C pins and delay value
void SoftwareI2C_Init(SoftwareI2C* i2c) {
    if (i2c == NULL) {
        log_e("Invalid I2C instance during initialization\n");
        return;
    }
    INIT_OUTPUT(i2c->sdaIOMUX);
    INIT_OUTPUT(i2c->sclIOMUX);
    SET_PIN(i2c->sdaPort, i2c->sdaPin);
    SET_PIN(i2c->sclPort, i2c->sclPin);
    log_i("I2C initialized with delay=%lu us, timeout=%lu us\n", i2c->delay_us, i2c->timeout_us);
}

// Set SDA as output
void SoftwareI2C_SDA_OUT(SoftwareI2C* i2c) {
    if (i2c == NULL) return;
    INIT_OUTPUT(i2c->sdaIOMUX);
    SET_PIN(i2c->sdaPort, i2c->sdaPin);
}

// Set SDA as input
void SoftwareI2C_SDA_IN(SoftwareI2C* i2c) {
    if (i2c == NULL) return;
		SET_PIN(i2c->sdaPort, i2c->sdaPin);
		delay_us(i2c->delay_us);
    INIT_INPUT(i2c->sdaIOMUX);
}

// Write to SCL pin
void SoftwareI2C_WriteSCL(SoftwareI2C* i2c, uint8_t BitValue) {
    if (i2c == NULL) return;
    if (BitValue)
        SET_PIN(i2c->sclPort, i2c->sclPin);
    else
        CLEAR_PIN(i2c->sclPort, i2c->sclPin);
    delay_us(i2c->delay_us);
}

// Write to SDA pin
void SoftwareI2C_WriteSDA(SoftwareI2C* i2c, uint8_t BitValue) {
    if (i2c == NULL) return;
    SoftwareI2C_SDA_OUT(i2c);
    if (BitValue)
        SET_PIN(i2c->sdaPort, i2c->sdaPin);
    else
        CLEAR_PIN(i2c->sdaPort, i2c->sdaPin);
    delay_us(i2c->delay_us);
}

// Read SDA pin
uint8_t SoftwareI2C_ReadSDA(SoftwareI2C* i2c) {
    if (i2c == NULL) return 0;
    uint32_t BitValue;
    SoftwareI2C_SDA_IN(i2c);
    delay_us(i2c->delay_us);
    BitValue = READ_PIN(i2c->sdaPort, i2c->sdaPin);
    return BitValue ? 1 : 0;
}

// I2C Start condition
void SoftwareI2C_Start(SoftwareI2C* i2c) {
    if (i2c == NULL) return;
    SoftwareI2C_SDA_OUT(i2c);
    SoftwareI2C_WriteSDA(i2c, 1);
    delay_us(i2c->delay_us);
    SoftwareI2C_WriteSCL(i2c, 1);
    delay_us(i2c->delay_us);
    SoftwareI2C_WriteSDA(i2c, 0);
    delay_us(i2c->delay_us);
    SoftwareI2C_WriteSCL(i2c, 0);
    delay_us(i2c->delay_us);
}

// I2C Stop condition
void SoftwareI2C_Stop(SoftwareI2C* i2c) {
    if (i2c == NULL) return;
    SoftwareI2C_SDA_OUT(i2c);
    SoftwareI2C_WriteSDA(i2c, 0);
    delay_us(i2c->delay_us);
    SoftwareI2C_WriteSCL(i2c, 1);
    delay_us(i2c->delay_us);
    SoftwareI2C_WriteSDA(i2c, 1);
    delay_us(i2c->delay_us);
}

// Send a byte
void SoftwareI2C_SendByte(SoftwareI2C* i2c, uint8_t Byte) {
    if (i2c == NULL) return;
    SoftwareI2C_SDA_OUT(i2c);
    for (uint8_t i = 0; i < 8; i++) {
        SoftwareI2C_WriteSDA(i2c, Byte & (0x80 >> i));
        delay_us(i2c->delay_us);
        SoftwareI2C_WriteSCL(i2c, 1);
        delay_us(i2c->delay_us);
        SoftwareI2C_WriteSCL(i2c, 0);
        delay_us(i2c->delay_us);
    }
}

// Receive a byte
uint8_t SoftwareI2C_ReceiveByte(SoftwareI2C* i2c) {
    if (i2c == NULL) return 0;
    uint8_t Byte = 0x00;
    SoftwareI2C_SDA_OUT(i2c);
    SoftwareI2C_WriteSDA(i2c, 1);
    delay_us(i2c->delay_us);
    for (uint8_t i = 0; i < 8; i++) {
        SoftwareI2C_SDA_IN(i2c);
        delay_us(i2c->delay_us);
        SoftwareI2C_WriteSCL(i2c, 1);
        delay_us(i2c->delay_us);
        if (SoftwareI2C_ReadSDA(i2c) == 1) {
            Byte |= (0x80 >> i);
        }
        SoftwareI2C_WriteSCL(i2c, 0);
        delay_us(i2c->delay_us);
    }
    return Byte;
}

// Send ACK
void SoftwareI2C_SendAck(SoftwareI2C* i2c, uint8_t AckBit) {
    if (i2c == NULL) return;
    SoftwareI2C_SDA_OUT(i2c);
    SoftwareI2C_WriteSDA(i2c, AckBit);
    delay_us(i2c->delay_us);
    SoftwareI2C_WriteSCL(i2c, 1);
    delay_us(i2c->delay_us);
    SoftwareI2C_WriteSCL(i2c, 0);
    delay_us(i2c->delay_us);
}

// Receive ACK
uint8_t SoftwareI2C_ReceiveAck(SoftwareI2C* i2c) {
    if (i2c == NULL) return 1; // 默认返回错误
    uint8_t AckBit;
    SoftwareI2C_SDA_OUT(i2c);
    SoftwareI2C_WriteSDA(i2c, 1);
    delay_us(i2c->delay_us);
    SoftwareI2C_WriteSCL(i2c, 1);
    delay_us(i2c->delay_us);
    SoftwareI2C_SDA_IN(i2c);
    AckBit = SoftwareI2C_ReadSDA(i2c);
    SoftwareI2C_WriteSCL(i2c, 0);
    delay_us(i2c->delay_us);
    return AckBit;
}

// Wait for ACK
unsigned char SoftwareI2C_WaitAck(SoftwareI2C* i2c) {
    if (i2c == NULL) return 0;
    long timeout = i2c->timeout_us / i2c->delay_us; // 使用配置的超时时间
    SoftwareI2C_SDA_IN(i2c);
    delay_us(i2c->delay_us);
    SoftwareI2C_WriteSCL(i2c, 1);
    delay_us(i2c->delay_us);
    while (SoftwareI2C_ReadSDA(i2c)) {
        if (timeout <= 0) {
            SoftwareI2C_SDA_OUT(i2c);
            SoftwareI2C_Stop(i2c);
            log_e("ACK timeout\n");
            return 0;
        }
        delay_us(i2c->delay_us);
				timeout--;
    }
    SoftwareI2C_SDA_OUT(i2c);
    SoftwareI2C_WriteSCL(i2c, 0);
    delay_us(i2c->delay_us);
    return 1;
}

uint8_t SoftWareI2C_Write_Len(SoftwareI2C* i2c, uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf) {
    if (i2c == NULL || buf == NULL || len == 0) {
        log_e("Invalid parameters for I2C write\n");
        return I2C_ERROR_INVALID_PARAM;
    }
    uint8_t i;
    SoftwareI2C_Start(i2c);
    SoftwareI2C_SendByte(i2c, addr << 1);
    log_i("Sending address: 0x%02X\n", addr << 1);
    if (SoftwareI2C_WaitAck(i2c) == 0) {
        log_e("Write_Len address error\n");
        SoftwareI2C_Stop(i2c);
        return I2C_ERROR_ACK_FAIL;
    }
    log_i("Sending register: 0x%02X\n", reg);
    SoftwareI2C_SendByte(i2c, reg);
    if (SoftwareI2C_WaitAck(i2c) == 0) {
        log_e("Write_Len register send error\n");
        SoftwareI2C_Stop(i2c);
        return I2C_ERROR_ACK_FAIL;
    }
    for (i = 0; i < len; i++) {
        log_i("Sending data[%d]: 0x%02X\n", i, buf[i]);
        SoftwareI2C_SendByte(i2c, buf[i]);
        if (SoftwareI2C_WaitAck(i2c) == 0) {
            log_e("Write_Len data send error at index %d\n", i);
            SoftwareI2C_Stop(i2c);
            return I2C_ERROR_ACK_FAIL;
        }
    }
    SoftwareI2C_Stop(i2c);
    return I2C_SUCCESS;
}

uint8_t SoftWareI2C_Read_Len(SoftwareI2C* i2c, uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf) {
    if (i2c == NULL || buf == NULL || len == 0) {
        log_e("Invalid parameters for I2C read\n");
        return I2C_ERROR_INVALID_PARAM;
    }
    // 第一次 START，发送设备地址（写模式）
    SoftwareI2C_Start(i2c);
    SoftwareI2C_SendByte(i2c, addr << 1); // Send device address with write command
    log_i("Sending address (write): 0x%02X\n", addr << 1);
    if (SoftwareI2C_WaitAck(i2c) == 0) { // Wait for ACK
        SoftwareI2C_Stop(i2c);
        log_e("Read_Len address error (write)\n");
        return I2C_ERROR_ACK_FAIL;
    }

    // 发送寄存器地址
    SoftwareI2C_SendByte(i2c, reg); // Send register address
    log_i("Sending register: 0x%02X\n", reg);
    if (SoftwareI2C_WaitAck(i2c) == 0) { // Wait for ACK
        SoftwareI2C_Stop(i2c);
        log_e("Read_Len register error\n");
        return I2C_ERROR_ACK_FAIL;
    }

    // 第二次 START，发送设备地址（读模式）
    SoftwareI2C_Start(i2c);
    SoftwareI2C_SendByte(i2c, (addr << 1) | 1); // Send device address with read command
    log_i("Sending address (read): 0x%02X\n", (addr << 1) | 1);
    if (SoftwareI2C_WaitAck(i2c) == 0) { // Wait for ACK
        SoftwareI2C_Stop(i2c);
        log_e("Read_Len address error (read)\n");
        return I2C_ERROR_ACK_FAIL;
    }

    // 读取数据
    while (len) {
        *buf = SoftwareI2C_ReceiveByte(i2c); // Read data
        if (len == 1) {
            SoftwareI2C_SendAck(i2c, 1); // Send NACK for the last byte
        } else {
            SoftwareI2C_SendAck(i2c, 0); // Send ACK for other bytes
        }
        len--;
        buf++;
    }

    SoftwareI2C_Stop(i2c);
    return I2C_SUCCESS;
}