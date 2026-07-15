/**
 * @file uart2_drv.h
 * @brief UART2设备驱动文件
 * @details 定义UART2驱动的核心功能接口，是连接用户API和底层硬件的桥梁。
 * @ingroup UART2_DRV
 */

#ifndef _UART2_DRV_H_
#define _UART2_DRV_H_

/**
 * @defgroup UART2_DRV UART2设备驱动
 * @ingroup UART_API
 * @brief UART2设备驱动模块
 * @details 
 * 定义UART2驱动的核心功能接口，是连接用户API和底层硬件的桥梁。
 * 
 * 主要职责包括：
 * - 实现UART2的初始化逻辑
 * - 处理UART2的数据发送和接收
 * - 管理UART2的缓冲区
 * - 处理UART2的中断事件
 * 
 * 本模块作为中间层，既接收用户API的调用，又直接操作底层硬件寄存器。
 * @{
 */

/**
 * @brief 初始化UART2
 * @details 初始化UART2，设置波特率和FIFO触发级别。
 * @param baudRate 波特率
 * @param fifoLevel FIFO触发级别
 *                  (UART_RX_TRIG_LEVEL_1, UART_RX_TRIG_LEVEL_4, 
 *                  UART_RX_TRIG_LEVEL_8, UART_RX_TRIG_LEVEL_14)
 */
// void Uart2_Init(unsigned int baudRate, unsigned int fifoLevel);

/**
 * @brief 发送数据到指定缓冲区
 * @details 发送数据到UART2发送缓冲区，等待发送完成。
 * @param buffer 数据缓冲区
 * @param length 数据长度
 */
// void Uart2_SendBuffer(unsigned char *buffer, unsigned int length);

/**
 * @brief 设置接收缓冲区长度
 * @details 设置UART2接收缓冲区的长度，用于接收数据。
 * @param length 缓冲区长度
 */
// void Uart2_SetRxBuffer(unsigned int length);

/** @} */

#endif /* _UART2_DRV_H_ */
