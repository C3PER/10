/**
 * @file rs485_drv.h
 * @brief RS485设备驱动头文件
 * @details 定义RS485驱动的核心功能和数据结构，是连接用户API和底层硬件的桥梁。
 * @ingroup RS485_DRV
 */

#ifndef _RS485_DRV_H_
#define _RS485_DRV_H_

/**
 * @defgroup RS485_DRV RS485设备驱动
 * @ingroup RS485_API
 * @brief RS485设备驱动模块
 * @details 
 * 定义RS485驱动的核心功能和数据结构，是连接用户API和底层硬件的桥梁。
 * 
 * 主要职责包括：
 * - 实现RS485的初始化逻辑
 * - 处理RS485的数据发送和接收
 * - 管理RS485的缓冲区
 * - 处理RS485的中断事件
 * - 控制RS485的传输模式
 * 
 * 本模块作为中间层，既接收用户API的调用，又直接操作底层硬件寄存器。
 * @{
 */

/**
 * @brief 初始化RS485
 * @details 初始化RS485，设置波特率和FIFO触发级别。
 * @param baudRate 波特率
 * @param fifoLevel FIFO触发级别 (1, 4, 8, 14)
 */
void Rs485_Init(unsigned int baudRate, unsigned int fifoLevel);

/**
 * @brief 简化版RS485初始化
 * @details 简化版RS485初始化，使用默认的FIFO触发级别。
 * @param baudRate 波特率
 */
void Rs485_Init_Lite(unsigned int baudRate);

/**
 * @brief 发送数据到RS485
 * @details 发送数据到RS485发送缓冲区，等待发送完成。
 * @param buffer 数据缓冲区
 * @param length 数据长度
 */
void Rs485_SendBuffer(unsigned char *buffer, unsigned int length);

/**
 * @brief 设置RS485接收缓冲区长度
 * @details 设置RS485接收缓冲区的长度，用于接收数据。
 * @param length 缓冲区长度
 */
void Rs485_SetRxBuffer(unsigned int length);

/**
 * @brief 设置RS485传输模式
 * @details 设置RS485的传输模式，切换接收或发送模式。
 * @param mode 传输模式 (RS485_MODE_RX 或 RS485_MODE_TX)
 */
void Rs485_SetMode(unsigned int mode);

/** @} */

#endif /* _RS485_DRV_H_ */
