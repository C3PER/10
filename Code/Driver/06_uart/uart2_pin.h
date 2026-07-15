/**
 * @file uart2_pin.h
 * @brief UART2引脚配置文件
 * @details 定义UART2相关的引脚宏和配置函数，负责UART2引脚的初始化和配置。
 * @ingroup UART2_PIN
 */

#ifndef _UART2_PIN_H_
#define _UART2_PIN_H_

/**
 * @defgroup UART2_PIN UART2引脚控制
 * @ingroup UART_API
 * @brief UART2引脚控制模块
 * @details 
 * 定义UART2相关的引脚宏和配置函数，负责UART2引脚的初始化和配置。
 * 
 * 主要职责包括：
 * - 定义UART2相关的时钟频率宏
 * - 定义UART2引脚复用宏
 * - 实现UART2的简化初始化函数
 * 
 * 本模块为UART2驱动提供引脚配置支持，是UART2驱动的重要组成部分。
 * @{
 */

/* 时钟定义 */
/// @cond INTERNAL_MACROS

#define SYSCLK_1_FREQ     (456000000)
#define SYSCLK_2_FREQ     (SYSCLK_1_FREQ/2)
#define UART_2_FREQ       (SYSCLK_2_FREQ)

/// @endcond

/* UART引脚定义 */
/// @cond INTERNAL_MACROS

#define PINMUX4_UART2_TXD_ENABLE    (SYSCFG_PINMUX4_PINMUX4_23_20_UART2_TXD << \
                                     SYSCFG_PINMUX4_PINMUX4_23_20_SHIFT)

#define PINMUX4_UART2_RXD_ENABLE    (SYSCFG_PINMUX4_PINMUX4_19_16_UART2_RXD << \
                                     SYSCFG_PINMUX4_PINMUX4_19_16_SHIFT)

/// @endcond

/**
 * @brief 简化版UART2初始化
 * @details 初始化UART2，设置波特率和引脚配置。
 * @param baudRate 波特率
 */
void Uart2_Init_Lite(unsigned int baudRate);

/** @} */

#endif /* _UART2_PIN_H_ */
