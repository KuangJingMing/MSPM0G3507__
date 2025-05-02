#include "common_include.h"
#include "tests.h"
#include "log_config.h"
#include "log.h"

// 要写入和读取的数据长度
#define TEST_DATA_LEN 8 // 减少数据量，简化测试
// 要写入和读取的 EEPROM 地址
#define TEST_START_ADDR 0x0000 // 限制在 0x0000-0x00FF 范围内

static uint8_t tx_buf[TEST_DATA_LEN];
static uint8_t rx_buf[TEST_DATA_LEN];
// 定义一个全局的 AT24CXX 句柄
static at24cxx_handle_t at24cxx_handle;

/**
 * @brief 单次测试 AT24CXX 的函数
 */
void at24cxx_single_test(void)
{
    uint8_t res;
    uint32_t i;

    log_i("Starting AT24CXX single test...");

    // 1. 填充 AT24CXX 句柄结构体
    DRIVER_AT24CXX_LINK_INIT(&at24cxx_handle, at24cxx_handle_t); // 清零句柄结构体
    at24cxx_handle.debug_print = at24cxx_interface_debug_print;
    at24cxx_handle.iic_init = at24cxx_interface_iic_init;
    at24cxx_handle.iic_deinit = at24cxx_interface_iic_deinit;
    at24cxx_handle.iic_read = at24cxx_interface_iic_read;
    at24cxx_handle.iic_write = at24cxx_interface_iic_write;
    at24cxx_handle.iic_read_address16 = at24cxx_interface_iic_read_address16;
    at24cxx_handle.iic_write_address16 = at24cxx_interface_iic_write_address16;
    at24cxx_handle.delay_ms = at24cxx_interface_delay_ms;

    // 确保设置 I2C 地址
    at24cxx_handle.iic_addr = 0xA0; // A0, A1, A2 接地时的写地址
    log_i("I2C address set to 0xA0 (8-bit write address).");

    // 2. 设置芯片类型
    res = at24cxx_set_type(&at24cxx_handle, AT24C16);
    if (res != 0) {
        log_e("AT24CXX set type failed.");
        return;
    }
    log_i("AT24CXX set type success (AT24C16).");

    // 3. 初始化 AT24C16 驱动
    res = at24cxx_init(&at24cxx_handle);
    if (res != 0) {
        log_e("AT24CXX driver init failed. Test aborted.");
        return;
    }
    log_i("AT24CXX driver init success.");

    // 4. 准备测试数据
    memset(tx_buf, 0x00, sizeof(tx_buf));
    for (i = 0; i < TEST_DATA_LEN; i++) {
        tx_buf[i] = (uint8_t)(i + 1); // 示例数据：1, 2, 3, ...
    }
    log_i("Prepare data to write:");
    for (i = 0; i < TEST_DATA_LEN; i++) {
        log_i("tx_buf[%lu] = 0x%02X", i, tx_buf[i]);
    }

    // 5. 写入数据到 EEPROM
    log_i("Writing data to EEPROM at address 0x%04X, length=%d...", TEST_START_ADDR, TEST_DATA_LEN);
    res = at24cxx_write(&at24cxx_handle, TEST_START_ADDR, tx_buf, TEST_DATA_LEN);
    if (res != 0) {
        log_e("AT24CXX driver write failed.");
    } else {
        log_i("AT24CXX driver write success.");
    }

    // 6. 等待写周期完成
    at24cxx_interface_delay_ms(10); // 增加写周期延迟到 20ms
    log_i("Waiting 20ms for write cycle to complete.");

    // 7. 读取数据进行验证
    memset(rx_buf, 0x00, sizeof(rx_buf));
    log_i("Reading data from EEPROM at address 0x%04X, length=%d...", TEST_START_ADDR, TEST_DATA_LEN);
    res = at24cxx_read(&at24cxx_handle, TEST_START_ADDR, rx_buf, TEST_DATA_LEN);
    if (res != 0) {
        log_e("AT24CXX driver read failed.");
    } else {
        log_i("AT24CXX driver read success.");
        log_i("Data read from EEPROM:");
        for (i = 0; i < TEST_DATA_LEN; i++) {
            log_i("rx_buf[%lu] = 0x%02X", i, rx_buf[i]);
        }

        // 验证数据是否一致
        if (memcmp(tx_buf, rx_buf, TEST_DATA_LEN) == 0) {
            log_i("Write and read data match. Test passed!");
        } else {
            log_e("Write and read data mismatch. Test failed!");
        }
    }

    // 8. 去初始化
    res = at24cxx_deinit(&at24cxx_handle);
    if (res != 0) {
        log_e("AT24CXX driver deinit failed.");
    } else {
        log_i("AT24CXX driver deinit success.");
    }

    log_i("AT24CXX single test completed.");
}