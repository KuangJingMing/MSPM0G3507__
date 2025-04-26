# MSPM0G3507 小车控制项目
## 项目概述
此项目是为2025年全国大学生电子设计竞赛（电赛）小车控制系统备赛而设计的，基于 **MSPM0G3507** 微控制器开发。项目旨在实现小车的运动控制、传感器数据采集与处理、路径规划以及其他竞赛相关功能。本项目由湖南汽车工程职业大学电赛团队开发，旨在提升团队成员的嵌入式开发能力与竞赛实战经验。
## 项目目标
- 实现小车的稳定运动控制（直线行驶、转弯、避障等）。
- 集成传感器模块（如红外、超声波、陀螺仪等）进行环境感知。
- 完成竞赛任务（如路径跟踪、特定动作执行等）。
- 编写高效的控制算法与代码，提升系统实时性与稳定性。
## 硬件平台
- **微控制器**：MSPM0G3507（TI 低功耗 MCU）
- **小车平台**：自主设计双轮差速小车
- **传感器**：ICM20608（六轴陀螺仪/加速度计）、灰度传感器、编码器
- **驱动模块**：TB6612FNG 电机驱动模块
- **其他硬件**：OLED 显示屏、W25QXX Flash、EEPROM、PCA9555 扩展IO
## 软件环境
- **开发工具**：Keil MDK v5
- **编程语言**：C
- **框架/库**：TI MSPM0 SDK Driverlib, FreeRTOS
- **调试工具**：仿真器、串口调试工具
## 项目结构
C:.
├─application                   # 应用层代码
│  ├─app                        # 应用逻辑
│  │  │  gray_detection.c       # 灰度传感器处理
│  │  │  gray_detection.h
│  │  │  motor_controller.c     # 电机控制逻辑
│  │  │  motor_controller.h
│  │  │
│  │  ├─communication          # 通信协议
│  │  │      embedfire_protocol.h
│  │  │      embedfire_protocol_receive.c
│  │  │      embedfire_protocol_send.c
│  │  │      serialplot_protocol.c  # Serialplot 可视化协议
│  │  │      serialplot_protocol.h
│  │  │
│  │  └─fusion                 # 传感器数据融合 (例如 AHRS)
│  │          FusionAhrs.c
│  │          FusionAhrs.h
│  │          FusionCompass.c
│  │          FusionCompass.h
│  │          FusionMath.h
│  │          FusionOffset.c
│  │          FusionOffset.h
│  │
│  ├─fonts                      # 字体数据 (例如 OLED 使用)
│  │      oled_data.c
│  │      oled_data.h
│  │
│  ├─hardware                   # 硬件驱动层
│  │  ├─bsp                     # 板级支持包
│  │  │      hardware_i2c.c     # 硬件 I2C 驱动
│  │  │      hardware_i2c.h
│  │  │      software_i2c.c     # 软件 I2C 驱动
│  │  │      software_i2c.h
│  │  │      spi.c              # SPI 驱动
│  │  │      spi.h
│  │  │      uart_driver.c      # UART 驱动
│  │  │      uart_driver.h
│  │  │
│  │  └─drivers                # 外设驱动
│  │          eeprom.c
│  │          eeprom.h
│  │          encoder.c          # 编码器驱动
│  │          encoder.h
│  │          icm20608.c         # ICM20608 驱动
│  │          icm20608.h
│  │          imu_data_type.h
│  │          motor_hardware.h   # 电机硬件抽象层
│  │          motor_l298n.c      # L298N 电机驱动 (未使用)
│  │          motor_tb6612.c     # TB6612 电机驱动
│  │          oled.c             # OLED 驱动
│  │          oled.h
│  │          pca9555.c          # PCA9555 驱动
│  │          pca9555.h
│  │          w25qxx.c           # W25QXX 驱动
│  │          w25qxx.h
│  │
│  ├─include                    # 全局包含目录
│  │      common_include.h
│  │      global_config.h
│  │
│  ├─math                       # 数学及滤波器库
│  │      fast_math.c
│  │      fast_math.h
│  │      filter.c               # 滤波器实现
│  │      filter.h
│  │      sensor.c               # 传感器数据处理 (例如校准)
│  │      sensor.h
│  │
│  ├─user                       # 用户代码 (通常包含 main 函数和任务)
│  │      main.c
│  │
│  └─utils                      # 工具函数
│          delay.c
│          delay.h
│          log.c                  # 日志功能 (未启用)
│          log.h
│          log_config.h
│
├─kernel                        # FreeRTOS 操作系统核心及适配层 (简要)
│  └─freertos
│      ├─builds
│      │  └─LP_MSPM0G3507
│      │      └─release
│      │          └─keil         # Keil 项目文件及编译生成文件
│      │              # ...
│      ├─dpl                     # FreeRTOS 对 TI DPL 的适配层
│      │  # ... (互斥锁、信号量、任务等实现)
│      └─Source                  # FreeRTOS 核心源码
│          ├─include            # FreeRTOS 核心头文件
│          │  # ...
│          └─portable           # FreeRTOS 移植层
│              └─GCC
│                  └─ARM_CM0    # 针对 Cortex-M0 移植
│                      # ...
│
├─project                       # 项目主目录 (包含 Keil 工程文件和配置)
│  │  freertos_demo.uvprojx      # Keil 项目文件
│  │  freertos_demo.syscfg       # syscfg 配置文件
│  │  startup_mspm0g350x_uvision.s # 启动文件
│  │  ti_msp_dl_config.c         # syscfg 生成的配置代码
│  │  ti_msp_dl_config.h
│  │  # ... (其他 Keil 相关文件)
│  │
│  └─Objects                    # 编译生成的对象文件和可执行文件 (简要)
│      # ...
│
├─source                        # TI MSPM0 SDK 源码 (简要)
│  ├─third_party
│  │  └─CMSIS
│  │      └─Core
│  │          └─Include        # CMSIS 核心头文件
│  │              # ...
│  └─ti
│      ├─devices              # 器件相关文件 (简要)
│      │  # ...
│      └─driverlib            # TI Driverlib 库 (简要)
│          │  dl_gpio.h          # GPIO 驱动头文件
│          │  dl_timer.h         # 定时器驱动头文件
│          │  # ... (其他 Driverlib 文件)
│          │  driverlib.h
│          └─lib                # 编译好的 Driverlib 库文件 (简要)
│              # ...
│
└─tools                         # 工具脚本 (简要)
# ...

<TEXT>
## 功能模块
1. **电机控制模块**：实现小车前后移动、转弯、速度调节等功能，通过 TB6612FNG 驱动。
2. **传感器数据采集模块**：采集 ICM20608 的陀螺仪和加速度计数据、灰度传感器数据以及编码器数据。
3. **控制算法模块**：实现 PID 控制或其他算法，用于小车运动的精准控制，并可能包含传感器数据融合（如 AHRS）。
4. **通信模块**：实现了 Embedfire 协议和 Serialplot 协议，用于与上位机进行数据交互和调试可视化。
5. **显示模块**：通过 OLED 显示屏显示小车状态或调试信息。
6. **存储模块**：使用 W25QXX Flash 和 EEPROM 进行数据存储（例如配置参数或日志）。
7. **扩展 IO 模块**：通过 PCA9555 扩展 IO 口。
## 使用说明
### 1. 环境搭建
- 安装 Keil MDK v5 开发工具（请提供具体版本号）。
- 下载并安装 TI MSPM0 SDK Driverlib（请提供具体版本号及下载链接）。
- 在 Keil 中导入项目文件 `project/freertos_demo.uvprojx`。
- 确保 MSPM0G3507 支持包已安装。
### 2. 硬件连接
根据项目中的原理图连接 MSPM0G3507 开发板与小车硬件，包括：
- TB6612FNG 电机驱动与 GPIO 连接。
- ICM20608 传感器与 I2C 或 SPI 连接。
- 灰度传感器与 ADC 或 GPIO 连接。
- 编码器与定时器或 GPIO 连接。
- OLED 显示屏与 I2C 或 SPI 连接。
- W25QXX Flash 与 SPI 连接。
- EEPROM 与 I2C 连接。
- PCA9555 与 I2C 连接。
### 3. 代码编译与下载
- 在 Keil 中打开项目。
- 点击 "Build" 按钮编译整个项目。
- 使用仿真器将编译生成的程序烧录到 MSPM0G3507 开发板。
### 4. 测试与调试
- 使用串口工具连接小车的通信接口，通过 Embedfire 协议或 Serialplot 协议进行数据监控和调试。
- 使用 Real-time Object Viewer (ROV) 工具查看 FreeRTOS 任务、队列、信号量等状态。
- 根据竞赛任务需求，逐步测试和调试各功能模块。
## 贡献者
- **团队名称**：湖南汽车工程职业大学电赛团队
- **成员**：kjm，hhg，蛤蛤蛤
- **指导老师**：f老师
## 联系方式
- **邮箱**：[请填写团队或负责人联系邮箱]
- **GitHub**：[请填写开源仓库链接，如果适用]
## 许可证
本项目代码和设计遵循 **MIT License**。详情参见 LICENSE 文件。