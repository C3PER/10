/**
 * @file dac_pin.h
 * @brief DAC引脚配置头文件
 * @details 定义DAC引脚相关的宏和函数声明，负责DAC引脚的初始化和配置。
 * @ingroup DAC_PIN
 */

#ifndef _DAC_PIN_H_
#define _DAC_PIN_H_

/**
 * @defgroup DAC_PIN DAC引脚控制
 * @ingroup DAC_API
 * @brief DAC引脚控制模块
 * @details 
 * 定义DAC引脚相关的宏和函数声明，负责DAC引脚的初始化和配置。
 * 
 * 主要职责包括：
 * - 定义DAC相关的引脚宏
 * - 实现DAC引脚的初始化
 * - 实现DAC加载引脚的控制
 * - 实现DAC清除引脚的控制
 * 
 * 本模块为DAC驱动提供引脚配置支持，是DAC驱动的重要组成部分。
 * @{
 */


/// @cond INTERNAL_MACROS
/**
 * @brief DAC引脚定义
 */
//DAC_LOAD GPIO2[12]
#define DAC_LOAD_PIN            (45)
#define DAC_LOAD_MASK           SYSCFG_PINMUX5_PINMUX5_15_12
#define DAC_LOAD_ENABLE         (SYSCFG_PINMUX5_PINMUX5_15_12_GPIO2_12 << SYSCFG_PINMUX5_PINMUX5_15_12_SHIFT)
//DAC_CLR GPIO1[15]
#define DAC_CLR_PIN             (32)
#define DAC_CLR_MASK            SYSCFG_PINMUX2_PINMUX2_3_0
#define DAC_CLR_ENABLE          (SYSCFG_PINMUX2_PINMUX2_3_0_GPIO1_15 << SYSCFG_PINMUX2_PINMUX2_3_0_SHIFT)
/// @endcond


/**
 * @brief 初始化DAC引脚
 * @details 初始化DAC相关的引脚，配置引脚复用功能。
 */
void Dac_Pin_Init(void);

/**
 * @brief 设置DAC加载引脚状态
 * @param bitState: 状态(1: 加载, 0: 不加载)
 */
void Dac_Load_PinSet(unsigned int bitState);

/**
 * @brief 设置DAC清除引脚状态
 * @param bitState: 状态(1: 清除, 0: 不清除)
 */
void Dac_Clr_PinSet(unsigned int bitState);

/** @} */

#endif /* _DAC_PIN_H_ */
