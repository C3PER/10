/**
 * @file flash.h
 * @brief Flash驱动头文件
 * @details 定义Flash驱动的接口和宏定义，是连接用户API和底层硬件的桥梁。
 * @ingroup FLASH_DRV
 */

#ifndef _FLASH_H_
#define _FLASH_H_

/**
 * @defgroup FLASH_DRV Flash设备驱动
 * @ingroup FLASH_API
 * @brief Flash设备驱动模块
 * @details 
 * 定义Flash驱动的接口和宏定义，是连接用户API和底层硬件的桥梁。
 * 
 * 主要职责包括：
 * - 定义Flash操作命令宏
 * - 定义Flash状态寄存器位定义
 * - 定义Flash地址定义
 * - 实现Flash的初始化逻辑
 * - 实现Flash的擦除、写入、读取和校验操作
 * 
 * 本模块作为中间层，既接收用户API的调用，又直接操作底层SPI接口。
 * @{
 */

/**
 * @brief Flash操作命令宏定义
 */
#define SPI_FLASH_WRITE_EN      0x06    // 写使能命令
#define SPI_FLASH_SECTOR_ERASE  0xD8    // 扇区擦除命令
#define SPI_FLASH_PAGE_WRITE    0x02    // 页写入命令
#define SPI_FLASH_READ          0x03    // 读命令
#define SPI_FLASH_STATUS_RX     0x05    // 读状态寄存器命令

/**
 * @brief Flash状态寄存器位定义
 */
#define WRITE_IN_PROGRESS       0x01    // 写操作执行中

/**
 * @brief Flash地址定义
 */
#define SPI_FLASH_ADDR_MSB1     0x0A    // Flash地址高位1
#define SPI_FLASH_ADDR_MSB0     0x00    // Flash地址高位0
#define SPI_FLASH_ADDR_LSB      0x00    // Flash地址低位

/**
 * @brief 初始化Flash
 * @details 初始化SPI接口，为Flash操作做准备。
 */
void Flash_Init(void);

/**
 * @brief Flash测试函数
 * @details 执行Flash的擦除、写入、读取和校验操作。
 */
void Flash_Test(void);

/** @} */

#endif /* _FLASH_H_ */
