/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3505

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)



#define CPUCLK_FREQ                                                     32000000



/* Defines for Motor_PWM1 */
#define Motor_PWM1_INST                                                    TIMA0
#define Motor_PWM1_INST_IRQHandler                              TIMA0_IRQHandler
#define Motor_PWM1_INST_INT_IRQN                                (TIMA0_INT_IRQn)
#define Motor_PWM1_INST_CLK_FREQ                                         4000000
/* GPIO defines for channel 0 */
#define GPIO_Motor_PWM1_C0_PORT                                            GPIOB
#define GPIO_Motor_PWM1_C0_PIN                                    DL_GPIO_PIN_14
#define GPIO_Motor_PWM1_C0_IOMUX                                 (IOMUX_PINCM31)
#define GPIO_Motor_PWM1_C0_IOMUX_FUNC                IOMUX_PINCM31_PF_TIMA0_CCP0
#define GPIO_Motor_PWM1_C0_IDX                               DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_Motor_PWM1_C1_PORT                                            GPIOA
#define GPIO_Motor_PWM1_C1_PIN                                     DL_GPIO_PIN_3
#define GPIO_Motor_PWM1_C1_IOMUX                                  (IOMUX_PINCM8)
#define GPIO_Motor_PWM1_C1_IOMUX_FUNC                 IOMUX_PINCM8_PF_TIMA0_CCP1
#define GPIO_Motor_PWM1_C1_IDX                               DL_TIMER_CC_1_INDEX

/* Defines for Motor_PWM2 */
#define Motor_PWM2_INST                                                    TIMG8
#define Motor_PWM2_INST_IRQHandler                              TIMG8_IRQHandler
#define Motor_PWM2_INST_INT_IRQN                                (TIMG8_INT_IRQn)
#define Motor_PWM2_INST_CLK_FREQ                                         4000000
/* GPIO defines for channel 0 */
#define GPIO_Motor_PWM2_C0_PORT                                            GPIOA
#define GPIO_Motor_PWM2_C0_PIN                                     DL_GPIO_PIN_7
#define GPIO_Motor_PWM2_C0_IOMUX                                 (IOMUX_PINCM14)
#define GPIO_Motor_PWM2_C0_IOMUX_FUNC                IOMUX_PINCM14_PF_TIMG8_CCP0
#define GPIO_Motor_PWM2_C0_IDX                               DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_Motor_PWM2_C1_PORT                                            GPIOA
#define GPIO_Motor_PWM2_C1_PIN                                     DL_GPIO_PIN_4
#define GPIO_Motor_PWM2_C1_IOMUX                                  (IOMUX_PINCM9)
#define GPIO_Motor_PWM2_C1_IOMUX_FUNC                 IOMUX_PINCM9_PF_TIMG8_CCP1
#define GPIO_Motor_PWM2_C1_IDX                               DL_TIMER_CC_1_INDEX



/* Defines for TIMER_delay */
#define TIMER_delay_INST                                                 (TIMA1)
#define TIMER_delay_INST_IRQHandler                             TIMA1_IRQHandler
#define TIMER_delay_INST_INT_IRQN                               (TIMA1_INT_IRQn)
#define TIMER_delay_INST_LOAD_VALUE                                         (0U)




/* Defines for I2C_0 */
#define I2C_0_INST                                                          I2C1
#define I2C_0_INST_IRQHandler                                    I2C1_IRQHandler
#define I2C_0_INST_INT_IRQN                                        I2C1_INT_IRQn
#define I2C_0_BUS_SPEED_HZ                                                800000
#define GPIO_I2C_0_SDA_PORT                                                GPIOA
#define GPIO_I2C_0_SDA_PIN                                        DL_GPIO_PIN_30
#define GPIO_I2C_0_IOMUX_SDA                                      (IOMUX_PINCM5)
#define GPIO_I2C_0_IOMUX_SDA_FUNC                       IOMUX_PINCM5_PF_I2C1_SDA
#define GPIO_I2C_0_SCL_PORT                                                GPIOA
#define GPIO_I2C_0_SCL_PIN                                        DL_GPIO_PIN_29
#define GPIO_I2C_0_IOMUX_SCL                                      (IOMUX_PINCM4)
#define GPIO_I2C_0_IOMUX_SCL_FUNC                       IOMUX_PINCM4_PF_I2C1_SCL


/* Defines for UART_DEBUG */
#define UART_DEBUG_INST                                                    UART0
#define UART_DEBUG_INST_FREQUENCY                                       32000000
#define UART_DEBUG_INST_IRQHandler                              UART0_IRQHandler
#define UART_DEBUG_INST_INT_IRQN                                  UART0_INT_IRQn
#define GPIO_UART_DEBUG_RX_PORT                                            GPIOA
#define GPIO_UART_DEBUG_TX_PORT                                            GPIOA
#define GPIO_UART_DEBUG_RX_PIN                                    DL_GPIO_PIN_11
#define GPIO_UART_DEBUG_TX_PIN                                    DL_GPIO_PIN_10
#define GPIO_UART_DEBUG_IOMUX_RX                                 (IOMUX_PINCM22)
#define GPIO_UART_DEBUG_IOMUX_TX                                 (IOMUX_PINCM21)
#define GPIO_UART_DEBUG_IOMUX_RX_FUNC                  IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_DEBUG_IOMUX_TX_FUNC                  IOMUX_PINCM21_PF_UART0_TX
#define UART_DEBUG_BAUD_RATE                                            (115200)
#define UART_DEBUG_IBRD_32_MHZ_115200_BAUD                                  (17)
#define UART_DEBUG_FBRD_32_MHZ_115200_BAUD                                  (23)




/* Defines for SPI_0 */
#define SPI_0_INST                                                         SPI1
#define SPI_0_INST_IRQHandler                                   SPI1_IRQHandler
#define SPI_0_INST_INT_IRQN                                       SPI1_INT_IRQn
#define GPIO_SPI_0_PICO_PORT                                              GPIOB
#define GPIO_SPI_0_PICO_PIN                                      DL_GPIO_PIN_15
#define GPIO_SPI_0_IOMUX_PICO                                   (IOMUX_PINCM32)
#define GPIO_SPI_0_IOMUX_PICO_FUNC                   IOMUX_PINCM32_PF_SPI1_PICO
#define GPIO_SPI_0_POCI_PORT                                              GPIOA
#define GPIO_SPI_0_POCI_PIN                                      DL_GPIO_PIN_16
#define GPIO_SPI_0_IOMUX_POCI                                   (IOMUX_PINCM38)
#define GPIO_SPI_0_IOMUX_POCI_FUNC                   IOMUX_PINCM38_PF_SPI1_POCI
/* GPIO configuration for SPI_0 */
#define GPIO_SPI_0_SCLK_PORT                                              GPIOA
#define GPIO_SPI_0_SCLK_PIN                                      DL_GPIO_PIN_17
#define GPIO_SPI_0_IOMUX_SCLK                                   (IOMUX_PINCM39)
#define GPIO_SPI_0_IOMUX_SCLK_FUNC                   IOMUX_PINCM39_PF_SPI1_SCLK



/* Port definition for Pin Group PCA9555 */
#define PCA9555_PORT                                                     (GPIOA)

/* Defines for SCL1: GPIOA.0 with pinCMx 1 on package pin 33 */
#define PCA9555_SCL1_PIN                                         (DL_GPIO_PIN_0)
#define PCA9555_SCL1_IOMUX                                        (IOMUX_PINCM1)
/* Defines for SDA1: GPIOA.1 with pinCMx 2 on package pin 34 */
#define PCA9555_SDA1_PIN                                         (DL_GPIO_PIN_1)
#define PCA9555_SDA1_IOMUX                                        (IOMUX_PINCM2)
/* Port definition for Pin Group OLED */
#define OLED_PORT                                                        (GPIOB)

/* Defines for SCL2: GPIOB.2 with pinCMx 15 on package pin 50 */
#define OLED_SCL2_PIN                                            (DL_GPIO_PIN_2)
#define OLED_SCL2_IOMUX                                          (IOMUX_PINCM15)
/* Defines for SDA2: GPIOB.3 with pinCMx 16 on package pin 51 */
#define OLED_SDA2_PIN                                            (DL_GPIO_PIN_3)
#define OLED_SDA2_IOMUX                                          (IOMUX_PINCM16)
/* Port definition for Pin Group PORTB */
#define PORTB_PORT                                                       (GPIOB)

/* Defines for RST: GPIOB.16 with pinCMx 33 on package pin 4 */
#define PORTB_RST_PIN                                           (DL_GPIO_PIN_16)
#define PORTB_RST_IOMUX                                          (IOMUX_PINCM33)
/* Defines for DC: GPIOB.17 with pinCMx 43 on package pin 14 */
#define PORTB_DC_PIN                                            (DL_GPIO_PIN_17)
#define PORTB_DC_IOMUX                                           (IOMUX_PINCM43)
/* Defines for CS: GPIOB.20 with pinCMx 48 on package pin 19 */
#define PORTB_CS_PIN                                            (DL_GPIO_PIN_20)
#define PORTB_CS_IOMUX                                           (IOMUX_PINCM48)
/* Port definition for Pin Group ENCODER */
#define ENCODER_PORT                                                     (GPIOB)

/* Defines for D1: GPIOB.6 with pinCMx 23 on package pin 58 */
#define ENCODER_D1_PIN                                           (DL_GPIO_PIN_6)
#define ENCODER_D1_IOMUX                                         (IOMUX_PINCM23)
/* Defines for D2: GPIOB.7 with pinCMx 24 on package pin 59 */
#define ENCODER_D2_PIN                                           (DL_GPIO_PIN_7)
#define ENCODER_D2_IOMUX                                         (IOMUX_PINCM24)
/* Defines for P1: GPIOB.4 with pinCMx 17 on package pin 52 */
// pins affected by this interrupt request:["P1","P2"]
#define ENCODER_INT_IRQN                                        (GPIOB_INT_IRQn)
#define ENCODER_INT_IIDX                        (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define ENCODER_P1_IIDX                                      (DL_GPIO_IIDX_DIO4)
#define ENCODER_P1_PIN                                           (DL_GPIO_PIN_4)
#define ENCODER_P1_IOMUX                                         (IOMUX_PINCM17)
/* Defines for P2: GPIOB.5 with pinCMx 18 on package pin 53 */
#define ENCODER_P2_IIDX                                      (DL_GPIO_IIDX_DIO5)
#define ENCODER_P2_PIN                                           (DL_GPIO_PIN_5)
#define ENCODER_P2_IOMUX                                         (IOMUX_PINCM18)
/* Defines for W25Q64_CS: GPIOB.25 with pinCMx 56 on package pin 27 */
#define PORTB_W25Q64_CS_PIN                                     (DL_GPIO_PIN_25)
#define PORTB_W25Q64_CS_IOMUX                                    (IOMUX_PINCM56)

/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_Motor_PWM1_init(void);
void SYSCFG_DL_Motor_PWM2_init(void);
void SYSCFG_DL_TIMER_delay_init(void);
void SYSCFG_DL_I2C_0_init(void);
void SYSCFG_DL_UART_DEBUG_init(void);
void SYSCFG_DL_SPI_0_init(void);


bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
