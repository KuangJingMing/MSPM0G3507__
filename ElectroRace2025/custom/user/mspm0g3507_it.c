#include "mspm0g3507_it.h"
#include "ti_msp_dl_config.h"
#include "embedfire_protocol.h"

extern volatile bool  gConsoleTxDMATransmitted;
extern volatile bool  gConsoleTxTransmitted;

//==============================================================================
// 协议接收状态机和处理 (RX方向)
//==============================================================================
void UART_DEBUG_INST_IRQHandler(void)
{
    uint8_t uart_data;
		DL_UART_IIDX idx = DL_UART_getPendingInterrupt(UART_DEBUG_INST);
    switch (idx)
    {
    case DL_UART_IIDX_RX: // 如果是接收中断
        // 接发送过来的数据保存在变量中
        uart_data = DL_UART_Main_receiveData(UART_DEBUG_INST);
        protocol_receive_byte(uart_data);

        break;
		case DL_UART_MAIN_IIDX_EOT_DONE:
				gConsoleTxTransmitted = true;
				break;
		case DL_UART_MAIN_IIDX_DMA_DONE_TX:
				gConsoleTxDMATransmitted = true;
				break;
    default: // 其他的串口中断
        break;
    }
		DL_UART_clearInterruptStatus(UART_DEBUG_INST, idx);
}