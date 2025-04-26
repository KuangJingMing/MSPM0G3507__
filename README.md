# MSPM0G3507 小车控制项目

## 项目概述

本项目是为2025年全国大学生电子设计竞赛（电赛）小车控制系统备赛而设计，基于 **TI MSPM0G3507** 微控制器。旨在实现小车的运动控制、传感器数据采集、路径规划等竞赛相关功能。

## 项目目标

- 实现小车稳定运动控制（直线、转弯、避障）。
- 集成传感器模块进行环境感知。
- 完成竞赛任务。
- 提升控制算法与代码效率。

## 硬件平台

- **MCU**: MSPM0G3507
- **小车**: 自主设计双轮差速
- **传感器**: ICM20608 (陀螺仪/加速度计)、灰度传感器、编码器
- **驱动**: TB6612FNG 电机驱动
- **其他**: OLED, W25QXX Flash, EEPROM, PCA9555 扩展IO

## 软件环境

- **IDE**: Keil MDK v5
- **语言**: C
- **框架**: TI MSPM0 SDK Driverlib, FreeRTOS
- **调试**: 仿真器, 串口

## 项目结构

```
C:.
├─application                   # 应用层代码 (传感器处理, 电机控制, 通信协议, 传感器融合)
│  ├─app
│  ├─fonts
│  ├─hardware                   # 硬件驱动 (I2C, SPI, UART, EEPROM, 编码器, ICM20608, 电机, OLED, PCA9555, W25QXX)
│  ├─include
│  ├─math                       # 数学及滤波器
│  ├─user                       # 用户代码 (main)
│  └─utils                      # 工具函数 (delay, log)
├─kernel                        # FreeRTOS 核心及适配层
├─project                       # Keil 工程文件 (.uvprojx, .syscfg, 启动文件, syscfg 生成代码)
├─source                        # TI MSPM0 SDK 源码 (CMSIS, Driverlib)
└─tools                         # 工具脚本
```

## 功能模块

- **电机控制**: TB6612FNG 驱动小车运动。
- **传感器采集**: 采集 ICM20608, 灰度, 编码器数据。
- **控制算法**: PID 等算法，可能包含 AHRS 传感器融合。
- **通信**: Embedfire, Serialplot 协议，用于调试可视化。
- **显示**: OLED 显示状态信息。
- **存储**: W25QXX, EEPROM 存储数据。
- **扩展 IO**: PCA9555 扩展 IO 口。

## 使用说明

### 1. 环境搭建

- 安装 Keil MDK v5。
- 安装 TI MSPM0 SDK Driverlib。
- 在 Keil 中导入 `project/freertos_demo.uvprojx`。
- 安装 MSPM0G3507 支持包。

### 2. 硬件连接

根据项目原理图连接 MSPM0G3507 开发板与小车硬件（电机驱动、传感器、显示屏、存储、扩展IO）。

### 3. 代码编译与下载

在 Keil 中编译项目，使用仿真器烧录程序到 MSPM0G3507。

### 4. 测试与调试

使用串口工具通过 Embedfire 或 Serialplot 协议进行数据监控。使用 ROV 查看 FreeRTOS 状态。根据竞赛任务需求测试调试。

## 贡献者

- **团队**: 湖南汽车工程职业大学电赛团队
- **成员**: kjm, hhg, 蛤蛤蛤
- **指导老师**: f老师

## 联系方式

- **邮箱**: [请填写团队或负责人联系邮箱]
- **GitHub**: [请填写开源仓库链接，如果适用]

## 许可证

本项目遵循 **MIT License**。详情参见 LICENSE 文件。