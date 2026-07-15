/**
 * uart1_drv.h
 *
 * 描述: UART1设备驱动头文件
 * 功能: 定义UART1驱动的核心功能和数据结构
 */

#ifndef _UART1_DRV_H_
#define _UART1_DRV_H_


/**
 * @brief 初始化UART1
 * @param baudRate: 波特率
 * @param fifoLevel: FIFO触发级别 (1, 4, 8, 14)
 * @return 无
 */
void Uart1_Init(unsigned int baudRate, unsigned int fifoLevel);

/**
 * @brief 发送数据到UART1
 * @param buffer: 数据缓冲区
 * @param length: 数据长度
 * @return 无
 */
void Uart1_SendBuffer(unsigned char *buffer, unsigned int length);

/**
 * @brief 设置UART1接收缓冲区长度
 * @param length: 缓冲区长度
 * @return 无
 */
void Uart1_SetRxBuffer(unsigned int length);


#endif /* _UART1_DRV_H_ */
