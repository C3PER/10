/**
 * @file user_uart.h
 * @brief UART用户应用文件
 * @details 提供UART的使用案例，展示如何初始化和使用UART。
 * @ingroup UART_EXAMPLE
 */

#ifndef _USER_UART_H_
#define _USER_UART_H_

/**
 * @defgroup UART_EXAMPLE UART应用示例
 * @ingroup UART
 * @brief UART应用示例模块
 * @details 
 * 提供UART使用示例，展示如何初始化和使用UART。
 * 
 * 主要职责包括：
 * - 提供UART使用示例函数
 * - 展示如何初始化UART
 * - 展示如何使用UART发送和接收数据
 * - 展示如何处理UART中断
 * 
 * 本模块为用户提供UART使用的参考示例，帮助用户快速上手UART功能。
 * @{
 */

/**
 * @brief UART示例函数
 * @details 展示UART的基本使用方法。
 */
void Uart_Example(void);

/**
 * @brief UART示例函数（不开中断版本）
 * @details 展示不使用中断的UART使用方法。
 */
void Uart_Example_NoInterrupt(void);

/**
 * @brief UART示例函数（开中断版本）
 * @details 展示使用中断的UART使用方法。
 */
void Uart_Example_Interrupt(void);

/** @} */

#endif /* _USER_UART_H_ */
