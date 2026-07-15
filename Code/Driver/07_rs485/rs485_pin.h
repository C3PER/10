/**
 * @file rs485_pin.h
 * @brief RS485引脚配置头文件
 * @details 定义RS485引脚配置相关的宏和函数声明，负责RS485引脚的初始化和配置。
 * @ingroup RS485_PIN
 */

#ifndef _RS485_PIN_H_
#define _RS485_PIN_H_

/**
 * @defgroup RS485_PIN RS485引脚控制
 * @ingroup RS485_API
 * @brief RS485引脚控制模块
 * @details 
 * 定义RS485引脚配置相关的宏和函数声明，负责RS485引脚的初始化和配置。
 * 
 * 主要职责包括：
 * - 定义RS485 Enable引脚的宏
 * - 实现RS485引脚的初始化函数
 * - 实现RS485 Enable引脚的状态设置函数
 * 
 * 本模块为RS485驱动提供引脚配置支持，是RS485驱动的重要组成部分。
 * @{
 */

/* RS485 Enable引脚定义 */
/// @cond INTERNAL_MACROS

//RS485_RE GPIO3[8]
#define RS485_RE_PIN           57
#define RS485_RE_MASK          SYSCFG_PINMUX7_PINMUX7_31_28
#define RS485_RE_ENABLE        (SYSCFG_PINMUX7_PINMUX7_31_28_GPIO3_8 << SYSCFG_PINMUX7_PINMUX7_31_28_SHIFT)

/// @endcond

/**
 * @brief RS485引脚初始化
 * @details 初始化RS485相关的引脚，包括Enable引脚的配置。
 */
void RS485_PinInit(void);

/**
 * @brief 设置RS485 Enable引脚状态
 * @details 设置RS485 Enable引脚的状态，用于切换RS485的传输模式。
 * @param bitState 引脚状态 (0: 接收模式, 1: 发送模式)
 */
void RS485_EN_PinSet(unsigned int bitState);

/** @} */

#endif /* _RS485_PIN_H_ */
