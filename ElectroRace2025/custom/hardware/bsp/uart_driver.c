#include "uart_driver.h"
#include <string.h>

volatile bool  gConsoleTxDMATransmitted = false;
volatile bool  gConsoleTxTransmitted = true;

/**
 * @brief 使用 DMA 发送单个字节
 * @param uart UART 实例
 * @param byte 要发送的字节
 */
void uart_send_byte(UART_Regs* uart, uint8_t byte) {
	uart->TXDATA = byte;
}

/**
 * @brief 使用 DMA 发送多个字节
 * @param uart UART 实例
 * @param data 数据指针
 * @param length 数据长度
 */
void usart_send_bytes(UART_Regs* uart, const uint8_t* data, size_t length) {
    //当串口发送完毕后，才可再次发送
    if(gConsoleTxTransmitted)
    {
        //设置源地址
        DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)(data));
    
        //设置目标地址
        DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)(&uart->TXDATA));
    
        //设置要搬运的字节数
        DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, length);
    
        //使能DMA通道
        DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
    
        gConsoleTxTransmitted    = false;
        gConsoleTxDMATransmitted = false;
    }
}

/**
 * @brief 格式化并使用 DMA 发送字符串
 * @param uart UART 实例
 * @param format 格式化字符串
 * @param ... 格式化参数
 */
void usart_printf(UART_Regs* uart, const char* format, ...) {
    char buffer[MAX_TX_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    usart_send_bytes(uart, (uint8_t*)buffer, strlen(buffer));
}