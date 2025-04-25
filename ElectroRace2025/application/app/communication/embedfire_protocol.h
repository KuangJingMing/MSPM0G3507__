#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stddef.h>
#include <string.h> // For memcpy and memmove
//==============================================================================
// 协议常量定义 (与之前相同)
//==============================================================================
#define PROTOCOL_HEADER 0x59485A53
// 下位机 -> 上位机 指令码 (与之前相同)
#define CMD_TX_SET_TARGET_VALUE    0x01
#define CMD_TX_SET_ACTUAL_VALUE    0x02
#define CMD_TX_SET_PID_PARAMS      0x03
#define CMD_TX_SYNC_START          0x04
#define CMD_TX_SYNC_STOP           0x05
#define CMD_TX_SET_PERIOD          0x06
// 上位机 -> 下位机 指令码 (与之前相同)
#define CMD_RX_SET_PID_PARAMS      0x10
#define CMD_RX_SET_TARGET_VALUE    0x11
#define CMD_RX_START               0x12
#define CMD_RX_STOP                0x13
#define CMD_RX_RESET               0x14
#define CMD_RX_SET_PERIOD          0x15
//==============================================================================
// 数据包结构定义 (与之前相同)
//==============================================================================
// 所有数据包的通用头部
typedef struct {
    uint32_t head;      // 4 bytes, 包头 0x59485A53
    uint8_t  channel;   // 1 byte, 通道地址
    uint32_t length;    // 4 bytes, 包长度 (从包头到校验)
    uint8_t  command;   // 1 byte, 指令码
} __attribute__((packed)) ProtocolHeader;
// 下位机 -> 上位机 数据包结构 (与之前相同)
typedef struct { ProtocolHeader header; int32_t target_value;  uint8_t  checksum; } __attribute__((packed)) TxSetTargetValuePacket;
typedef struct { ProtocolHeader header; int32_t actual_value;  uint8_t  checksum; } __attribute__((packed)) TxSetActualValuePacket;
typedef struct { ProtocolHeader header; float p_param; float i_param; float d_param; uint8_t checksum; } __attribute__((packed)) TxSetPIDParamsPacket;
typedef struct { ProtocolHeader header; uint8_t  checksum; } __attribute__((packed)) TxSyncStatePacket;
typedef struct { ProtocolHeader header; uint32_t period; uint8_t  checksum; } __attribute__((packed)) TxSetPeriodPacket;
// 上位机 -> 下位机 数据包结构 (需要定义 RX 方向的结构体)
// 设置下位机PID值 (0x10)
typedef struct {
    ProtocolHeader header;
    float    p_param;      // 4 bytes, P参数
    float    i_param;      // 4 bytes, I参数
    float    d_param;      // 4 bytes, D参数
    uint8_t  checksum;     // 1 byte, 校验和
} __attribute__((packed)) RxSetPIDParamsPacket;
// 设置下位机的目标值 (0x11)
typedef struct {
    ProtocolHeader header;
    int32_t  target_value; // 4 bytes, 目标值 (int 类型)
    uint8_t  checksum;     // 1 byte, 校验和
} __attribute__((packed)) RxSetTargetValuePacket;
// 启动指令 (0x12), 停止指令 (0x13), 复位指令 (0x14)
typedef struct {
    ProtocolHeader header;
    uint8_t  checksum;     // 1 byte, 校验和
} __attribute__((packed)) RxSimpleCommandPacket; // 这些指令只有头部和校验和
// 设置下位机周期 (0x15)
typedef struct {
    ProtocolHeader header;
    uint32_t period;       // 4 bytes, 周期 (unsigned int 类型)
    uint8_t  checksum;     // 1 byte, 校验和
} __attribute__((packed)) RxSetPeriodPacket;


//==============================================================================
// 校验和计算函数声明 (与之前相同)
//==============================================================================
uint8_t protocol_calculate_checksum(const uint8_t* data, size_t length);

//==============================================================================
// 协议组包和发送函数声明 (TX方向，与之前相同)
//==============================================================================
void protocol_send_set_target_value(uint8_t channel, int32_t target_value);
void protocol_send_set_actual_value(uint8_t channel, int32_t actual_value);
void protocol_send_set_pid_params(uint8_t channel, float p_param, float i_param, float d_param);
void protocol_send_sync_start(uint8_t channel);
void protocol_send_sync_stop(uint8_t channel);
void protocol_send_set_period(uint8_t channel, uint32_t period);


//==============================================================================
// 协议接收和解析函数声明 (RX方向)
//==============================================================================

/**
 * @brief 处理从UART接收到的一个字节数据.
 *        这个函数通常在UART接收中断服务程序中被调用，或者在主循环中
 *        轮询接收缓冲区并调用.
 * @param received_byte: 从UART接收到的一个字节.
 * @retval 无.
 */
void protocol_receive_byte(uint8_t received_byte);


// 回调函数类型定义 (与之前相同)
typedef void (*SetPIDParamsCallback)(uint8_t channel, float p, float i, float d);
typedef void (*SetTargetValueCallback)(uint8_t channel, int32_t target);
typedef void (*SimpleCommandCallback)(uint8_t channel);
typedef void (*SetPeriodCallback)(uint8_t channel, uint32_t period);

// 接收指令回调函数结构体
typedef struct {
    SetPIDParamsCallback   set_pid_params;
    SetTargetValueCallback set_target_value;
    SimpleCommandCallback  start;
    SimpleCommandCallback  stop;
    SimpleCommandCallback  reset;
    SetPeriodCallback      set_period;
    // 如果有其他接收指令，在这里添加对应的回调函数指针
} ProtocolRxCallbacks;

/**
 * @brief 注册接收指令回调函数集合.
 * @param callbacks: 包含所有回调函数指针的结构体.
 * @retval 无.
 */
void protocol_register_rx_callbacks(const ProtocolRxCallbacks* callbacks);


#endif // PROTOCOL_H
