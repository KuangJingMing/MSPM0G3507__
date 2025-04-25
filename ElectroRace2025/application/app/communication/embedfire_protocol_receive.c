#include "embedfire_protocol.h"

//==============================================================================
// 协议接收状态机和处理 (优化回调函数管理)
//==============================================================================

// 接收状态机的枚举 (与之前相同)
typedef enum {
    STATE_WAITING_FOR_HEADER,
    STATE_RECEIVING_DATA,
} ReceiveState;

static ReceiveState rx_state = STATE_WAITING_FOR_HEADER;

// 接收缓冲区 (与之前相同)
#define MAX_RX_PACKET_SIZE sizeof(RxSetPIDParamsPacket)
static uint8_t rx_buffer[MAX_RX_PACKET_SIZE];
static size_t rx_buffer_index = 0;
static size_t expected_packet_length = 0;

// 回调函数结构体变量 (全局或静态)
static ProtocolRxCallbacks rx_callbacks = {0}; // 初始化为零，所有指针为NULL

// 回调函数注册函数实现
void protocol_register_rx_callbacks(const ProtocolRxCallbacks* callbacks) {
    if (callbacks) {
        rx_callbacks = *callbacks; // 复制结构体内容
    } else {
        // 如果传入NULL，可以将所有回调函数指针置为NULL
        memset(&rx_callbacks, 0, sizeof(ProtocolRxCallbacks));
    }
}


/**
 * @brief 根据接收到的完整数据包处理指令.
 * @param packet_data: 完整数据包的起始地址.
 * @param packet_length: 完整数据包的长度.
 * @retval 无.
 */
void process_received_packet(const uint8_t* packet_data, size_t packet_length) {
    const ProtocolHeader* header = (const ProtocolHeader*)packet_data;
    uint8_t channel = header->channel;
    uint8_t command = header->command;

    // 根据指令码和注册的回调函数来处理数据
    switch (command) {
        case CMD_RX_SET_PID_PARAMS: {
            if (packet_length == sizeof(RxSetPIDParamsPacket)) {
                 const RxSetPIDParamsPacket* packet = (const RxSetPIDParamsPacket*)packet_data;
                 // 调用注册的回调函数 (通过结构体访问)
                 if (rx_callbacks.set_pid_params) {
                     rx_callbacks.set_pid_params(channel, packet->p_param, packet->i_param, packet->d_param);
                 }
            } else { /* 包长度错误 */ }
            break;
        }
        case CMD_RX_SET_TARGET_VALUE: {
            if (packet_length == sizeof(RxSetTargetValuePacket)) {
                const RxSetTargetValuePacket* packet = (const RxSetTargetValuePacket*)packet_data;
                // 调用注册的回调函数 (通过结构体访问)
                if (rx_callbacks.set_target_value) {
                    rx_callbacks.set_target_value(channel, packet->target_value);
                }
            } else { /* 包长度错误 */ }
            break;
        }
        case CMD_RX_START: {
             if (packet_length == sizeof(RxSimpleCommandPacket)) {
                 // 调用注册的回调函数 (通过结构体访问)
                 if (rx_callbacks.start) {
                     rx_callbacks.start(channel);
                 }
             } else { /* 包长度错误 */ }
            break;
        }
        case CMD_RX_STOP: {
             if (packet_length == sizeof(RxSimpleCommandPacket)) {
                 // 调用注册的回ombro function (accediendo a través de la estructura)
                 if (rx_callbacks.stop) {
                     rx_callbacks.stop(channel);
                 }
             } else { /* 包长度错误 */ }
            break;
        }
        case CMD_RX_RESET: {
             if (packet_length == sizeof(RxSimpleCommandPacket)) {
                 // 调用注册的回调函数 (通过结构体访问)
                 if (rx_callbacks.reset) {
                     rx_callbacks.reset(channel);
                 }
             } else { /* 包长度错误 */ }
            break;
        }
        case CMD_RX_SET_PERIOD: {
            if (packet_length == sizeof(RxSetPeriodPacket)) {
                const RxSetPeriodPacket* packet = (const RxSetPeriodPacket*)packet_data;
                // 调用注册的回调函数 (通过结构体访问)
                if (rx_callbacks.set_period) {
                    rx_callbacks.set_period(channel, packet->period);
                }
            } else { /* 包长度错误 */ }
            break;
        }
        default:
            // 未知指令，忽略
            break;
    }
}

// 接收状态机处理函数
void protocol_receive_byte(uint8_t received_byte) {
    switch (rx_state) {
        case STATE_WAITING_FOR_HEADER:
            // 寻找包头 0x59 0x48 0x5A 0x53
            // 使用一个小的移位或状态序列来检测包头
            // 这里简化处理，只检查最后一个字节是否是包头的一部分
            // 更健壮的方法是实现一个状态机来检测完整的包头序列
            if (rx_buffer_index < sizeof(uint32_t)) {
                rx_buffer[rx_buffer_index++] = received_byte;
                if (rx_buffer_index == sizeof(uint32_t)) {
                    // 检查是否是完整的包头
                    if (*((uint32_t*)rx_buffer) == PROTOCOL_HEADER) {
                        // 包头匹配，进入接收数据状态
                        rx_state = STATE_RECEIVING_DATA;
                        // 已经接收了包头，索引清零准备接收后面的数据
                        rx_buffer_index = sizeof(ProtocolHeader); // 包头和通道地址，长度和指令
                        // 将已接收的包头复制到缓冲区头部
                        // memcpy(rx_buffer, rx_buffer, sizeof(uint32_t)); // 不需要复制，数据已经在缓冲区了
                    } else {
                        // 包头不匹配，移位，继续寻找包头
                        // 例如，将缓冲区中的字节向前移动
                        memmove(rx_buffer, rx_buffer + 1, sizeof(uint32_t) - 1);
                        rx_buffer_index = sizeof(uint32_t) - 1;
                    }
                }
            } else {
                 // 缓冲区已满但未找到包头，将旧数据移出，放入新数据
                 memmove(rx_buffer, rx_buffer + 1, sizeof(uint32_t) - 1);
                 rx_buffer[sizeof(uint32_t) - 1] = received_byte;
                 // 再次检查是否匹配包头
                 if (*((uint32_t*)(rx_buffer)) == PROTOCOL_HEADER) {
                     rx_state = STATE_RECEIVING_DATA;
                     rx_buffer_index = sizeof(ProtocolHeader);
                 } else {
                     rx_buffer_index = sizeof(uint32_t); // 保持缓冲区满
                 }
            }
            break;
        case STATE_RECEIVING_DATA:
            // 接收数据包的其余部分
            if (rx_buffer_index < MAX_RX_PACKET_SIZE) {
                rx_buffer[rx_buffer_index++] = received_byte;
                // 在接收到包头、通道、长度和指令后，可以确定期望的包总长度
                if (rx_buffer_index >= sizeof(ProtocolHeader)) {
                    const ProtocolHeader* header = (const ProtocolHeader*)rx_buffer;
                    // 从包长度字段获取期望的总长度
                    expected_packet_length = header->length;
                    // 检查长度是否合理 (防止恶意数据包)
                    if (expected_packet_length > MAX_RX_PACKET_SIZE || expected_packet_length < sizeof(ProtocolHeader) + sizeof(uint8_t)) { // 最小包长度是 包头+通道+长度+指令+校验和
                        // 非法长度，复位状态机
                        rx_state = STATE_WAITING_FOR_HEADER;
                        rx_buffer_index = 0;
                        expected_packet_length = 0;
                        // 可以添加错误日志或处理
                        return;
                    }
                }
                // 检查是否接收到完整的数据包 (包括校验和)
                if (rx_buffer_index >= sizeof(ProtocolHeader) && rx_buffer_index == expected_packet_length) {
                    // 接收到一个完整的数据包，现在进行校验和检查
                    // 计算接收到的数据部分的校验和 (不包含最后一个字节的校验和)
                    uint8_t received_checksum = rx_buffer[rx_buffer_index - 1];
                    uint8_t calculated_checksum = protocol_calculate_checksum(rx_buffer, rx_buffer_index - 1);
                    if (received_checksum == calculated_checksum) {
                        // 校验和匹配，处理数据包
                        process_received_packet(rx_buffer, rx_buffer_index);
                    } else {
                        // 校验和不匹配，数据包错误
                        // 可以添加错误日志或处理
                    }
                    // 数据包处理完毕或校验错误，复位状态机，准备接收下一个包
                    rx_state = STATE_WAITING_FOR_HEADER;
                    rx_buffer_index = 0;
                    expected_packet_length = 0;
                } else if (rx_buffer_index >= MAX_RX_PACKET_SIZE) {
                    // 缓冲区溢出，复位状态机
                    rx_state = STATE_WAITING_FOR_HEADER;
                    rx_buffer_index = 0;
                    expected_packet_length = 0;
                    // 可以添加错误日志或处理
                }
            }
            break;
    }
}