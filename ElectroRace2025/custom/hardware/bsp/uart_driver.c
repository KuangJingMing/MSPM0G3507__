#include "uart_driver.h"
#include "embedfire_protocol.h"
#include <string.h>
#include <stdarg.h>

// 任务和队列句柄
TaskHandle_t uart_tx_task_handle = NULL;
TaskHandle_t uart_rx_task_handle = NULL;
QueueHandle_t uart_rx_queue = NULL;
QueueHandle_t uart_tx_queue = NULL;

void uart_tx_task(void *param);
void uart_rx_task(void *param);

// 缓冲区
static char tx_buffer[MAX_TX_BUFFER_SIZE];

/**
 * @brief 初始化 UART 相关的 FreeRTOS 资源
 */
void uart_init(void) {
    uart_rx_queue = xQueueCreate(UART_RX_QUEUE_SIZE, sizeof(uint8_t));
    uart_tx_queue = xQueueCreate(UART_TX_QUEUE_SIZE, sizeof(uint8_t));
    xTaskCreate(uart_rx_task, "UART_RX", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &uart_rx_task_handle);
    xTaskCreate(uart_tx_task, "UART_TX", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 2, &uart_tx_task_handle);
}

/**
 * @brief 使用 DMA 发送单个字节
 * @param uart UART 实例
 * @param byte 要发送的字节
 */
void usart_send_byte(UART_Regs *uart, uint8_t byte) {
    uart->TXDATA = byte;
}

/**
 * @brief 使用 DMA 发送多个字节
 * @param uart UART 实例
 * @param data 数据指针
 * @param length 数据长度
 */
void usart_send_bytes(UART_Regs *uart, const uint8_t *data, size_t length) {
    // 设置源地址
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)(data));
    // 设置目标地址
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)(&uart->TXDATA));
    // 设置要搬运的字节数
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, length);
    // 使能 DMA 通道
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
}

/**
 * @brief 格式化并通过队列发送字符串
 * @param uart UART 实例
 * @param format 格式化字符串
 * @param ... 格式化参数
 */
void usart_printf(UART_Regs *uart, const char *format, ...) {
    memset(tx_buffer, 0, sizeof(tx_buffer));
    va_list args;
    va_start(args, format);
    vsnprintf(tx_buffer, sizeof(tx_buffer), format, args);
    va_end(args);

    size_t len = strlen(tx_buffer);
    for (size_t i = 0; i < len; i++) {
        xQueueSend(uart_tx_queue, &tx_buffer[i], portMAX_DELAY);
    }
    // 通知发送任务有数据需要发送
    xTaskNotifyGive(uart_tx_task_handle);
}

/**
 * @brief UART 发送任务
 * @param param 任务参数（未使用）
 */
void uart_tx_task(void *param) {
    uint8_t data;
    static uint8_t tx_temp_buffer[MAX_TX_BUFFER_SIZE];
    uint32_t buffer_index = 0;
    BaseType_t dma_transfer_active = pdFALSE;
    while (1) {
        // 等待任务通知（可能是有新数据或DMA传输完成）
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        // 如果没有正在进行的DMA传输，准备新的数据
        if (dma_transfer_active == pdFALSE) {
            // 从队列中读取数据，直到队列为空
            buffer_index = 0;
            while (xQueueReceive(uart_tx_queue, &data, 0) == pdPASS) {
                if (buffer_index < MAX_TX_BUFFER_SIZE) {
                    tx_temp_buffer[buffer_index++] = data;
                }
            }
            // 如果有数据，则通过 DMA 发送
            if (buffer_index > 0) {
                dma_transfer_active = pdTRUE;
                usart_send_bytes(UART_DEBUG_INST, tx_temp_buffer, buffer_index);
            }
        } else {
            // DMA传输已完成
            dma_transfer_active = pdFALSE;
        }
    }
}


/**
 * @brief UART 接收任务
 * @param param 任务参数（未使用）
 */
void uart_rx_task(void *param) {
    uint8_t data;

    while (1) {
        // 从接收队列中读取数据
        if (xQueueReceive(uart_rx_queue, &data, portMAX_DELAY) == pdPASS) {
            // 处理接收到的数据（调用协议处理函数）
            protocol_receive_byte(data);
        }
    }
}
