/**
 * @file flash.c
 * @brief Flash驱动实现文件
 * @details 实现Flash的初始化、擦除、写入、读取和校验操作。
 * @ingroup FLASH_DRV
 */

#include "flash.h"
#include "sys_spi.h"
#include "uartStdio.h"  // 提供UARTprintf、UARTPuts函数

/* 全局变量 */
static unsigned char vrf_data[256]={0};   // 用于校验的数据
static unsigned char spi_tx_data[256]; // 发送缓冲区
static unsigned char spi_rx_data[256]; // 接收缓冲区

/* 函数声明 */
// 写使能
static void WriteEnable(void);
// 扇区擦除
static void SectorErase(void);
// 向Flash写入数据
static void WritetoFlash(void);
// 从Flash读取数据
static void ReadFromFlash(void);
// 校验数据
static void VerifyData(void);
// 检查Flash是否忙
static void IsFlashBusy(void);



/* 函数定义 */
// 初始化Flash
void Flash_Init(void)
{
    // 初始化SPI
    Sys_SPI_Init();
    
    UARTprintf("Flash initialized successfully!\r\n");
}

// Flash测试函数
void Flash_Test(void)
{
    UARTprintf("\r\n=== Flash Test Started ===\r\n");
    
    // 1. 写使能
    UARTprintf("1. Sending write enable command...\r\n");
    WriteEnable();
    
    // 2. 扇区擦除
    UARTprintf("2. Erasing sector...\r\n");
    SectorErase();
    UARTprintf("   Sector erase completed.\r\n");
    
    // 3. 写使能
    UARTprintf("3. Sending write enable command...\r\n");
    WriteEnable();
    
    // 4. 写入数据
    UARTprintf("4. Writing data to flash...\r\n");
    WritetoFlash();
    UARTprintf("   Data write completed.\r\n");
    
    // 5. 读取数据
    UARTprintf("5. Reading data from flash...\r\n");
    ReadFromFlash();
    UARTprintf("   Data read completed.\r\n");
    
    // 6. 校验数据
    UARTprintf("6. Verifying data...\r\n");
    VerifyData();
    
    UARTprintf("=== Flash Test Completed ===\r\n\r\n");
}


// 写使能
static void WriteEnable(void)
{
    // 准备写使能命令
    spi_tx_data[0] = SPI_FLASH_WRITE_EN;
    
    // 发送命令
//    SPI_SeqWrite_Reg(SPI_DEVICE_FLASH, spi_tx_data, 1);
    SPI_SeqRead_Reg(SPI_DEVICE_FLASH, spi_tx_data, 1, spi_rx_data, 1);
}

// 扇区擦除
static void SectorErase(void)
{
    // 准备扇区擦除命令和地址
    spi_tx_data[0] = SPI_FLASH_SECTOR_ERASE;
    spi_tx_data[1] = SPI_FLASH_ADDR_MSB1;
    spi_tx_data[2] = SPI_FLASH_ADDR_MSB0;
    spi_tx_data[3] = SPI_FLASH_ADDR_LSB;
    
    // 发送命令
//    SPI_SeqWrite_Reg(SPI_DEVICE_FLASH, spi_tx_data, 4);
    SPI_SeqRead_Reg(SPI_DEVICE_FLASH, spi_tx_data, 4, spi_rx_data, 4);
    
    // 等待擦除完成
    IsFlashBusy();
}

// 检查Flash是否忙
static void IsFlashBusy(void)
{
    do {
        // 读取状态寄存器
        spi_tx_data[0] = SPI_FLASH_STATUS_RX;
        spi_tx_data[1] = SPI_FLASH_ADDR_MSB1;
        SPI_SeqRead_Reg(SPI_DEVICE_FLASH, spi_tx_data, 2, spi_rx_data, 2);
    } while (spi_rx_data[1] & WRITE_IN_PROGRESS);
}

// 向Flash写入数据
static void WritetoFlash(void)
{
    unsigned int index;
    
    // 准备页写入命令和地址
    spi_tx_data[0] = SPI_FLASH_PAGE_WRITE;
    spi_tx_data[1] = SPI_FLASH_ADDR_MSB1;
    spi_tx_data[2] = SPI_FLASH_ADDR_MSB0;
    spi_tx_data[3] = SPI_FLASH_ADDR_LSB;
    
    // 准备要写入的数据
    for (index = 4; index < 256; index++) {
        spi_tx_data[index] = index;
        vrf_data[index] = index;  // 保存用于校验的数据
    }
    
    // 发送数据
//    SPI_SeqWrite_Reg(SPI_DEVICE_FLASH, spi_tx_data, index);
    SPI_SeqRead_Reg(SPI_DEVICE_FLASH, spi_tx_data, index, spi_rx_data, index);
    
    // 等待写入完成
    IsFlashBusy();
}

// 从Flash读取数据
static void ReadFromFlash(void)
{
    unsigned int index;
    
    // 准备读命令和地址
    spi_tx_data[0] = SPI_FLASH_READ;
    spi_tx_data[1] = SPI_FLASH_ADDR_MSB1;
    spi_tx_data[2] = SPI_FLASH_ADDR_MSB0;
    spi_tx_data[3] = SPI_FLASH_ADDR_LSB;
    
    // 清空发送缓冲区
    for (index = 4; index < 256; index++) {
        spi_tx_data[index] = 0;
    }
    
    // 发送命令并接收数据
    SPI_SeqRead_Reg(SPI_DEVICE_FLASH, spi_tx_data, index, spi_rx_data, index);
}

// 校验数据
static void VerifyData(void)
{
    unsigned int index;
    unsigned char verify_success = 1;
    
    // 比较写入和读取的数据
    for (index = 4; index < 256; index++) {
        if (vrf_data[index] != spi_rx_data[index]) {
            // 数据不一致
            UARTprintf("\r\nVerify failed: Mismatch at index %d\r\n", index - 3);
            verify_success = 0;
            break;
        }
    }
    
    // 打印校验结果
    if (verify_success) {
        UARTprintf("\r\nVerify successfully! Data matches.\r\n");
    } else {
        UARTprintf("\r\nVerify failed! Data mismatch.\r\n");
    }
}
