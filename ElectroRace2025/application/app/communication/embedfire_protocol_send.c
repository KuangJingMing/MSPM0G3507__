#include "embedfire_protocol.h"
#include "uart_driver.h"

//==============================================================================
// 校验和计算函数实现 (与之前相同)
//==============================================================================
uint8_t protocol_calculate_checksum(const uint8_t* data, size_t length) {
    uint8_t sum = 0;
    for (size_t i = 0; i < length; ++i) {
        sum += data[i];
    }
    return sum;
}

//==============================================================================
// 内部函数: 发送通用数据包 (与之前相同)
//==============================================================================
void protocol_send_packet(const uint8_t* data, size_t data_length) {
    uint8_t checksum = protocol_calculate_checksum(data, data_length);
    usart_send_bytes(UART_DEBUG_INST, data, data_length);
    uart_send_byte(UART_DEBUG_INST, checksum);
}

//==============================================================================
// 协议组包和发送函数实现 (TX方向，与之前相同)
//==============================================================================
void protocol_send_set_target_value(uint8_t channel, int32_t target_value) {
    TxSetTargetValuePacket packet;
    packet.header.head = PROTOCOL_HEADER;
    packet.header.channel = channel;
    packet.header.length = sizeof(TxSetTargetValuePacket);
    packet.header.command = CMD_TX_SET_TARGET_VALUE;
    packet.target_value = target_value;
    protocol_send_packet((const uint8_t*)&packet, sizeof(TxSetTargetValuePacket) - sizeof(packet.checksum));
}

void protocol_send_set_actual_value(uint8_t channel, int32_t actual_value) {
    TxSetActualValuePacket packet;
    packet.header.head = PROTOCOL_HEADER;
    packet.header.channel = channel;
    packet.header.length = sizeof(TxSetActualValuePacket);
    packet.header.command = CMD_TX_SET_ACTUAL_VALUE;
    packet.actual_value = actual_value;
    protocol_send_packet((const uint8_t*)&packet, sizeof(TxSetActualValuePacket) - sizeof(packet.checksum));
}

void protocol_send_set_pid_params(uint8_t channel, float p_param, float i_param, float d_param) {
    TxSetPIDParamsPacket packet;
    packet.header.head = PROTOCOL_HEADER;
    packet.header.channel = channel;
    packet.header.length = sizeof(TxSetPIDParamsPacket);
    packet.header.command = CMD_TX_SET_PID_PARAMS;
    packet.p_param = p_param;
    packet.i_param = i_param;
    packet.d_param = d_param;
    protocol_send_packet((const uint8_t*)&packet, sizeof(TxSetPIDParamsPacket) - sizeof(packet.checksum));
}

void protocol_send_sync_start(uint8_t channel) {
    TxSyncStatePacket packet;
    packet.header.head = PROTOCOL_HEADER;
    packet.header.channel = channel;
    packet.header.length = sizeof(TxSyncStatePacket);
    packet.header.command = CMD_TX_SYNC_START;
    protocol_send_packet((const uint8_t*)&packet, sizeof(TxSyncStatePacket) - sizeof(packet.checksum));
}

void protocol_send_sync_stop(uint8_t channel) {
    TxSyncStatePacket packet;
    packet.header.head = PROTOCOL_HEADER;
    packet.header.channel = channel;
    packet.header.length = sizeof(TxSyncStatePacket);
    packet.header.command = CMD_TX_SYNC_STOP;
    protocol_send_packet((const uint8_t*)&packet, sizeof(TxSyncStatePacket) - sizeof(packet.checksum));
}

void protocol_send_set_period(uint8_t channel, uint32_t period) {
     TxSetPeriodPacket packet;
    packet.header.head = PROTOCOL_HEADER;
    packet.header.channel = channel;
    packet.header.length = sizeof(TxSetPeriodPacket);
    packet.header.command = CMD_TX_SET_PERIOD;
    packet.period = period;
    protocol_send_packet((const uint8_t*)&packet, sizeof(TxSetPeriodPacket) - sizeof(packet.checksum));
}


