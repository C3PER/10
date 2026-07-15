/**
 * @file key_pin.h
 * @brief 按键引脚配置文件
 * @details 定义按键相关的引脚宏和底层引脚控制函数。
 * @ingroup KEY_PIN
 */

#ifndef _KEY_PIN_H_
#define _KEY_PIN_H_

/**
 * @defgroup KEY_PIN 按键引脚控制
 * @ingroup KEY_API
 * @brief 按键引脚控制模块
 * @details 
 * 定义按键相关的引脚宏和底层引脚控制函数。
 * 
 * 主要职责包括：
 * - 定义按键相关的引脚宏，包括引脚编号、掩码和使能值
 * - 负责按键引脚的初始化，包括PSC初始化、引脚复用配置和GPIO方向设置
 * - 配置按键的中断触发方式和使能中断
 * - 直接操作硬件寄存器，实现底层的GPIO控制
 * 
 * 本模块为上层驱动提供硬件支持，是整个按键驱动的基础。
 * @{
 */

/// @cond INTERNAL_MACROS
/* 按键引脚定义 */
//KEY1 GPIO0[0]
#define KEY1_PIN                1   // (0 * 16 + 0 + 1)
#define KEY1_MASK               SYSCFG_PINMUX1_PINMUX1_31_28
#define KEY1_ENABLE             (SYSCFG_PINMUX1_PINMUX1_31_28_GPIO0_0 << SYSCFG_PINMUX1_PINMUX1_31_28_SHIFT)
//KEY2 GPIO0[1]
#define KEY2_PIN                2   // (0 * 16 + 1 + 1)
#define KEY2_MASK               SYSCFG_PINMUX1_PINMUX1_27_24
#define KEY2_ENABLE             (SYSCFG_PINMUX1_PINMUX1_27_24_GPIO0_1 << SYSCFG_PINMUX1_PINMUX1_27_24_SHIFT)
//KEY3 GPIO0[2]
#define KEY3_PIN                3   // (0 * 16 + 2 + 1)
#define KEY3_MASK               SYSCFG_PINMUX1_PINMUX1_23_20
#define KEY3_ENABLE             (SYSCFG_PINMUX1_PINMUX1_23_20_GPIO0_2 << SYSCFG_PINMUX1_PINMUX1_23_20_SHIFT)
//KEY4 GPIO0[3]
#define KEY4_PIN                4   // (0 * 16 + 3 + 1)
#define KEY4_MASK               SYSCFG_PINMUX1_PINMUX1_19_16
#define KEY4_ENABLE             (SYSCFG_PINMUX1_PINMUX1_19_16_GPIO0_3 << SYSCFG_PINMUX1_PINMUX1_19_16_SHIFT)
//KEY5 GPIO0[4]
#define KEY5_PIN                5   // (0 * 16 + 4 + 1)
#define KEY5_MASK               SYSCFG_PINMUX1_PINMUX1_15_12
#define KEY5_ENABLE             (SYSCFG_PINMUX1_PINMUX1_15_12_GPIO0_4 << SYSCFG_PINMUX1_PINMUX1_15_12_SHIFT)
/// @endcond


/**
 * @brief 初始化按键引脚
 * @details 初始化按键相关的GPIO引脚，配置为输入模式并设置中断。
 */
void KEY_PinInit(void);

/** @} */

#endif /* _KEY_PIN_H_ */
