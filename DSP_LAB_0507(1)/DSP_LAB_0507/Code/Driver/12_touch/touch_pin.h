/**
 * @file touch_pin.h
 * @brief Touch引脚配置文件
 * @details 定义Touch相关的引脚宏和底层引脚控制函数。
 * @ingroup TOUCH_PIN
 */

#ifndef _TOUCH_PIN_H_
#define _TOUCH_PIN_H_

/**
 * @defgroup TOUCH_PIN Touch引脚控制
 * @ingroup TOUCH_API
 * @brief Touch引脚控制模块
 * @details 
 * 定义Touch相关的引脚宏和底层引脚控制函数。
 * 
 * 主要职责包括：
 * - 定义Touch相关的引脚宏，包括引脚编号、掩码和使能值
 * - 负责Touch引脚的初始化，包括PSC初始化和引脚复用配置
 * - 配置Touch的中断触发方式和使能中断
 * - 直接操作硬件寄存器，实现底层的GPIO控制
 * 
 * 本模块为上层驱动提供硬件支持，是整个Touch驱动的基础。
 * @{
 */

/// @cond INTERNAL_MACROS
//TOUCH_INT GPIO8[10]
#define TOUCH_INT_PIN           139
#define TOUCH_INT_MASK          SYSCFG_PINMUX18_PINMUX18_31_28
#define TOUCH_INT_ENABLE        (SYSCFG_PINMUX18_PINMUX18_31_28_GPIO8_10 << SYSCFG_PINMUX18_PINMUX18_31_28_SHIFT)
/// @endcond /* INTERNAL_MACROS 结束 */


/** @brief 触摸中断标志位 
 * @details 触摸发生时置1，需要用户使用后手动置为0。
*/
extern unsigned char FLAG_TOUCH;


/**
 * @brief 初始化Touch引脚（输出模式）
 * @details 配置Touch中断引脚为输出模式，用于复位触摸芯片
 */
void Touch_Pin_Init_Out(void);

/**
 * @brief 初始化Touch引脚（输入模式）
 * @details 配置Touch中断引脚为输入模式，并配置中断
 */
void Touch_Pin_Init_In(void);

/**
 * @brief 设置触摸中断引脚状态
 * @details 设置触摸中断引脚的电平状态
 * @param bitState 状态(1: 高电平, 0: 低电平)
 */
void Touch_Int_PinSet(unsigned int bitState);

/** @} */

#endif /* _TOUCH_PIN_H_ */
