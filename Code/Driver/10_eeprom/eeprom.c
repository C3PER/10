/**
 * @file eeprom.c
 * @brief EEPROM驱动实现文件
 * @details 实现EEPROM的初始化、读写操作和测试。
 * @ingroup EEPROM_DRV
 */

#include "eeprom.h"
#include "sys_iic.h"
#include "system.h"
#include "uart.h"
#include "delay.h"
#include "uartStdio.h"  // 提供UARTprintf、UARTPuts函数
#include "i2c.h"
#include "string.h"


/* 全局变量 */
volatile unsigned int AckRolling = 0; // EEPROM应答轮询标志

/* 函数声明 */
// EEPROM应答轮询
static void EEPROMAckPolling(void);


/* 函数定义 */
// EEPROM应答轮询
static void EEPROMAckPolling(void)
{
    do {
        AckRolling = 0;
        delay(1);
        EEPROMCurrentAddressWrite(EEPROM_ADDRESS);
        delay(1);
    } while (AckRolling == 1);
}

// 初始化EEPROM
void EEPROM_Init(void)
{
    // 初始化IIC
    Sys_IIC_Init();
    
    // 设置EEPROM从机地址
    IIC_SelectDevice(IIC_DEVICE_EEPROM);
    
    UARTprintf("EEPROM initialized successfully!\r\n");
}

// EEPROM测试函数
void EEPROM_Test(void)
{
    int i, result;
    unsigned char buf_send[8];
    unsigned char buf_recv[8];
    char hex_buffer[16];
    
    UARTprintf("\r\n=== EEPROM Test Started ===\r\n");
    
    // 设置EEPROM从机地址
    IIC_SelectDevice(IIC_DEVICE_EEPROM);
    
    // 1. 测试单字节写入和读取
    UARTprintf("1. Testing single byte write/read...\r\n");
    UARTprintf("   Writing 0x55 to address 0x00...\r\n");
    EEPROMByteWrite(0x00, 0x55);
    EEPROMAckPolling();
    
    memset(buf_recv, 0, 8);
    EEPROMRandomRead(0x00, &buf_recv[0]);
    UARTprintf("   Read from address 0x00: 0x%02X\r\n", buf_recv[0]);
    
    // 2. 测试当前地址读取
    UARTprintf("2. Testing current address read...\r\n");
    EEPROMCurrentAddressRead(&buf_recv[1]);
    UARTprintf("   Read from current address: 0x%02X\r\n", buf_recv[1]);
    
    // 3. 测试页写入和顺序读取
    UARTprintf("3. Testing page write/sequential read...\r\n");
    
    // 准备测试数据
    for (i = 0; i < 8; i++) {
        if (i % 2 == 0)
            buf_send[i] = 0xAA;
        else
            buf_send[i] = 0x55;
    }
    
    // 打印发送数据
    UARTprintf("   Data to send: ");
    for (i = 0; i < 8; i++) {
        sprintf(hex_buffer, "0x%02X ", buf_send[i]);
        UARTprintf(hex_buffer);
    }
    UARTprintf("\r\n");
    
    // 写入一页数据
    UARTprintf("   Writing page data to address 0x00...\r\n");
    EEPROMPageWrite(0x00, buf_send, 8);
    EEPROMAckPolling();
    
    // 读取一页数据
    UARTprintf("   Reading page data from address 0x00...\r\n");
    memset(buf_recv, 0, 8);
    EEPROMSequentialRead(0x00, buf_recv, 8);
    
    // 打印接收数据
    UARTprintf("   Data received: ");
    for (i = 0; i < 8; i++) {
        sprintf(hex_buffer, "0x%02X ", buf_recv[i]);
        UARTprintf(hex_buffer);
    }
    UARTprintf("\r\n");
    
    // 校验数据
    result = 1;
    for (i = 0; i < 8; i++) {
        if (buf_send[i] != buf_recv[i]) {
            result = 0;
            break;
        }
    }
    
    // 打印校验结果
    if (result) {
        UARTprintf("   Verify successfully! Data matches.\r\n");
    } else {
        UARTprintf("   Verify failed! Data mismatch.\r\n");
    }
    
    UARTprintf("=== EEPROM Test Completed ===\r\n\r\n");
    
    // 恢复IIC地址为触摸屏地址
    IIC_SelectDevice(IIC_DEVICE_TOUCH);
}
