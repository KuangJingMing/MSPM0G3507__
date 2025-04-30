#ifndef SERIALPLOT_PROTOCOL_H
#define SERIALPLOT_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

// 包含你的 UART 驱动头文件
#include "uart_driver.h"

// 定义用于 serialplot 通信的 UART 实例
// 你需要在你的项目配置头文件或其他地方定义 UART_DEBUG_INST
// 例如： #define UART_DEBUG_INST UART0
#ifdef UART_DEBUG_INST
#define SERIALPLOT_UART_INST UART_DEBUG_INST
#else
// 如果没有定义 UART_DEBUG_INST，这里不会强制报错，只会导致后续使用时产生警告
// 这通常不是一个好的做法，建议定义 UART_DEBUG_INST 或者使用默认值
#warning "UART_DEBUG_INST is not defined. Serialplot communication may not work correctly."
// #define SERIALPLOT_UART_INST NULL // 或者定义一个默认值，但 NULL 可能不适用你的 UART_Regs* 类型
#endif


// 定义一个宏，用于指定格式化浮点数的小数位数
// 根据你的需求调整
#define SERIALPLOT_FLOAT_PRECISION 2

// 移除 serialplot_send_data 的声明，因为它不再符合逗号分隔的格式需求
// 如果你希望保留它用于其他目的，请取消下面声明的注释
/*
void serialplot_send_data(char channel_char, float value);
*/

/**
 * @brief 向 hyOzd 的 serialplot 发送多个数据点（在一行），使用逗号分隔。
 *
 * 这个函数允许你在同一行发送多个数据点，每个数据点之间使用逗号分隔。
 * 所有数据点发送完毕后，会自动发送一个换行符。
 * 数据格式: <value1>,<value2>,...,<valueN>\n
 * Serialplot 将根据逗号解析出多个数据点，并按顺序对应通道。
 *
 * @param num_channels 要发送的数据通道数量。
 * @param ... 可变参数列表，包含 num_channels 个 float (数据值)。
 *            例如: serialplot_send_multi_data(3, 1.23f, 4.56f, 7.89f);
 */
void serialplot_send_multi_data(size_t num_channels, ...);


#endif // SERIALPLOT_PROTOCOL_H
