#include "pca9555.h"
#include "gray_detection.h"

#include "log_config.h"
#include "log.h"

SoftwareI2C pca9555_i2c = {
	  .sclPort = PCA9555_PORT,
    .sdaPort = PCA9555_PORT,
    .sclPin = PCA9555_SCL1_PIN,
    .sdaPin = PCA9555_SDA1_PIN,
    .sclIOMUX = PCA9555_SCL1_IOMUX,
    .sdaIOMUX = PCA9555_SDA1_IOMUX,
	  .delay_us = 0,
	  .timeout_us = 1000          // 超时时间
};

uint8_t gray_datas[12] = {0};

void Gray_Detection_Init(void) {
	SoftwareI2C_Init(&pca9555_i2c);
}

void Gray_ReadData(void) {
    uint16_t pca_data = pca9555_read_bit12(&pca9555_i2c, PCA9555_ADDR);
	
    // 将 16 位数据按位压入 gray_datas 数组（只取低 12 位）
    for (int i = 0; i < 12; i++) {
        gray_datas[i] = (pca_data >> i) & 0x01; // 提取每一位并存入数组
    }
		    // 使用日志工具输出 gray_datas 数组内容
    log_i("Gray Data Array: %d %d %d %d %d %d %d %d %d %d %d %d",
          gray_datas[0], gray_datas[1], gray_datas[2], gray_datas[3],
          gray_datas[4], gray_datas[5], gray_datas[6], gray_datas[7],
          gray_datas[8], gray_datas[9], gray_datas[10], gray_datas[11]);
}    