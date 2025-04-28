/*
 * oled.c
 *
 *  Created on: 2022年7月24日
 *      Author: Unicorn_Li
 */
#include "oled_driver.h"
#include "freertos.h"
#include "task.h"
#include "software_i2c.h"

u8g2_t u8g2;

uint8_t SPI_WriteByte(uint8_t Byte)
{
  while (DL_SPI_isBusy(SPI_0_INST));
  DL_SPI_transmitData8(SPI_0_INST, Byte);
  while (DL_SPI_isRXFIFOEmpty(SPI_0_INST));
  // while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_RBNE));
  return DL_SPI_receiveData8(SPI_0_INST);
}

void oled_spi_init(void)
{
  OLED_RST_Clr();
  delay_ms(10);
  OLED_RST_Set();
  delay_ms(10);
  OLED_CS_Set();
}

void oled_i2c_init(void)
{
    // 初始化 SCL 引脚 (GPIO_SPI_0_IOMUX_SCLK 对应的 GPIO 引脚) 为推挽输出
    // SysConfig 中使用 DL_GPIO_initDigitalOutput(iomux_address) 初始化数字输出
    // 我们使用 IOMUX 地址来确保使用的是正确的引脚
    DL_GPIO_initDigitalOutput(GPIO_SPI_0_IOMUX_SCLK);

    // 初始化 SDA 引脚 (GPIO_SPI_0_IOMUX_PICO 对应的 GPIO 引脚) 为推挽输出
    DL_GPIO_initDigitalOutput(GPIO_SPI_0_IOMUX_PICO);

    // 设置 SCL 和 SDA 初始状态为高电平 (I2C空闲状态)
    // SysConfig 中使用 DL_GPIO_setPins(port, pins) 来设置引脚高电平
    DL_GPIO_setPins(GPIO_SPI_0_SCLK_PORT, GPIO_SPI_0_SCLK_PIN);
    DL_GPIO_setPins(GPIO_SPI_0_PICO_PORT, GPIO_SPI_0_PICO_PIN);

    // 使能 SCL 和 SDA 引脚输出
    // SysConfig 中使用 DL_GPIO_enableOutput(port, pins) 来使能输出
    DL_GPIO_enableOutput(GPIO_SPI_0_SCLK_PORT, GPIO_SPI_0_SCLK_PIN);
    DL_GPIO_enableOutput(GPIO_SPI_0_PICO_PORT, GPIO_SPI_0_PICO_PIN);

    // 您之前的代码中也有设置和使能输出的操作，这里只是参照SysConfig的风格重新组织
    // 原来的代码中 DL_GPIO_initDigitalOutputFeatures(..., DL_GPIO_HIZ_ENABLE)
    // 可能导致引脚行为不确定，去掉features版本直接使用DL_GPIO_initDigitalOutput
    // 是更明确的推挽输出初始化方式，参照SysConfig生成代码。
}

uint8_t u8x8_gpio_and_delay_mspm0(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    // 初始化GPIO
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
				oled_i2c_init();
        break;

    // 毫秒延时
    case U8X8_MSG_DELAY_MILLI:
				delay_ms(arg_int);
        break;

    // 10微秒延时
    case U8X8_MSG_DELAY_I2C:
        delay_us(arg_int <= 2 ? 5 : 1);
        break;

    // I2C时钟信号控制
    case U8X8_MSG_GPIO_I2C_CLOCK:
        if (arg_int == 0)
            OLED_SCL_Clr();
        else
            OLED_SCL_Set();
        break;

    // I2C数据信号控制
    case U8X8_MSG_GPIO_I2C_DATA:
        if (arg_int == 0)
            OLED_SDA_Clr();
        else
            OLED_SDA_Set();
        break;
				
    // 以下是UI控制按钮，通常不需要实现
    case U8X8_MSG_GPIO_MENU_SELECT:
    case U8X8_MSG_GPIO_MENU_NEXT:
    case U8X8_MSG_GPIO_MENU_PREV:
    case U8X8_MSG_GPIO_MENU_HOME:
        return 0;  // 返回0表示不支持
    }
    return 1;  // 未知消息时返回0
}

uint8_t u8x8_byte_3wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  uint8_t *p = (uint8_t *)arg_ptr;
  switch (msg)
  {
  /*通过SPI发送arg_int个字节数据*/
  case U8X8_MSG_BYTE_SEND:
    for (int i = 0; i < arg_int; i++)
      SPI_WriteByte((uint8_t)*(p + i));
    break;
  /*设置DC引脚，DC引脚控制发送的是数据还是命令*/
  case U8X8_MSG_BYTE_SET_DC:
    if (arg_int)
      OLED_DC_Set();
    else
      OLED_DC_Clr();
    break;
  case U8X8_MSG_BYTE_INIT:
    oled_spi_init();
    break;
  case U8X8_MSG_BYTE_START_TRANSFER:
    OLED_CS_Clr(); // 拉低CS，开始传输
    break;
  case U8X8_MSG_BYTE_END_TRANSFER:
    OLED_CS_Set(); // 拉高CS，结束传输
    break;
  default:
    return 0;
  }
  return 1;
}

uint8_t u8g2_gpio_and_delay_mspm0(U8X8_UNUSED u8x8_t *u8x8,
                                  U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
                                  U8X8_UNUSED void *arg_ptr)
{
  return 1;
}

void show_oled_opening_animation(void)
{
    float angle = 0.0f;
    uint32_t startTime = xTaskGetTickCount();
    do {
        u8g2_ClearBuffer(&u8g2);

        // 计算已过ms
        uint32_t elapsedTime = (xTaskGetTickCount() - startTime) * portTICK_PERIOD_MS;

        // 1. --- MSPM0 波浪主标题 + “高亮字母动画” ---
        u8g2_SetFont(&u8g2, u8g2_font_inb16_mr);
        const char* text = "MSPM0";
        int textWidth = u8g2_GetStrWidth(&u8g2, text);

        // 每一帧随机一个字符用白底黑字高亮
        int highlight_i = ((int)(angle * 1.5f)) % 5;
        for (int i = 0, x = 64 - textWidth/2; text[i] != '\0'; i++)
        {
            char c[2] = {text[i], '\0'};
            int charWidth = u8g2_GetStrWidth(&u8g2, c);
            int y = 30 + 8 * sinf(angle + i * 0.7f);
            if(i == highlight_i) {
                // 椭圆高亮底框
                u8g2_SetDrawColor(&u8g2, 1);
                u8g2_DrawBox(&u8g2, x-2, y-15, charWidth+4, 19);
                u8g2_SetDrawColor(&u8g2, 0);
                u8g2_DrawStr(&u8g2, x, y, c);
                u8g2_SetDrawColor(&u8g2, 1);
            } else {
                u8g2_DrawStr(&u8g2, x, y, c);
            }
            x += charWidth;
        }

        // 2. --- 副标题 WELCOME，淡入 ---
        uint32_t fadein = elapsedTime < 1500 ? elapsedTime : 1500;
        float alpha = (float)fadein / 1500.f;
        u8g2_SetFont(&u8g2, u8g2_font_profont12_tr);
        const char* subtext = "Welcome";
        int subw = u8g2_GetStrWidth(&u8g2, subtext);
        // 点阵淡入，随机点分布
        for(int i=0,x=64-subw/2;subtext[i]!='\0';i++) {
            char c[2]={subtext[i],'\0'};
            int cw = u8g2_GetStrWidth(&u8g2,c);
            int y = 50 + 2*sinf(angle + i*0.5f);  //副标题也微微波动
            // 字符显示点阵按alpha填补，制造淡入
            if((i*13+y+(int)angle)%10 < (int)(10*alpha)) // 随机采样点数呈现淡入
                u8g2_DrawStr(&u8g2,x,y,c);
            x+=cw;
        }

        // 3. --- 顶部&底部波浪，交错+加星点 ---
        for (int x = 0; x < 128; x++)
        {
            int y1 = 60 + 3 * sinf(angle + x * 0.09f);
            u8g2_DrawPixel(&u8g2, x, y1);

            // 底部点星星
            if (((x*15 + (int)angle*8) % 113) == 0 && (elapsedTime%600)<400)
                u8g2_DrawPixel(&u8g2, x, y1-2);

            int y2 = 7 + 3 * sinf(angle + x * 0.1f + 3.14159f);
            u8g2_DrawPixel(&u8g2, x, y2);

            // 顶部点星星
            if (((x*9+(int)angle*11)%127)==0 && (elapsedTime%700)<300)
                u8g2_DrawPixel(&u8g2, x, y2+2);
        }

        // 4. --- 进度条波浪填充 ---
        int bar_len = (elapsedTime * 118) / 3000;
        if (bar_len > 118) bar_len = 118;
        u8g2_DrawFrame(&u8g2, 5, 62, 118, 2);
        // 用小波浪做进度条填充
        for(int i=0;i<bar_len;i++) {
            int y = 62 + (int)(sinf(angle+i*0.25f)*1.1f);
            u8g2_DrawPixel(&u8g2, 5+i, y);
            u8g2_DrawPixel(&u8g2, 5+i, 63); //下沿也补齐
        }
        u8g2_SendBuffer(&u8g2);
        angle += 0.2f;
        vTaskDelay(pdMS_TO_TICKS(50)); // 20FPS
    } while((xTaskGetTickCount() - startTime) * portTICK_PERIOD_MS < 2000);
    u8g2_ClearBuffer(&u8g2);
    u8g2_SendBuffer(&u8g2);
}

void u8g2_Init(void)
{
  //u8g2_Setup_ssd1306_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_3wire_hw_spi, u8g2_gpio_and_delay_mspm0);
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_sw_i2c, &u8x8_gpio_and_delay_mspm0);
  u8g2_InitDisplay(&u8g2);
  u8g2_SetPowerSave(&u8g2, 0);
	u8g2_ClearBuffer(&u8g2);
}
