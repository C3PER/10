/**
 * @file adc_pin.h
 * @brief ADC引脚配置文件
 * @details 定义ADC的引脚配置和相关函数，负责ADC引脚的初始化和配置。
 * @ingroup ADC_PIN
 */

#ifndef _ADC_PIN_H_
#define _ADC_PIN_H_

/**
 * @defgroup ADC_PIN ADC引脚控制
 * @ingroup ADC_API
 * @brief ADC引脚控制模块
 * @details 
 * 定义ADC的引脚配置和相关函数，负责ADC引脚的初始化和配置。
 * 
 * 主要职责包括：
 * - 定义ADC相关的引脚宏
 * - 实现ADC引脚的初始化
 * - 实现ADC复位引脚的控制
 * - 实现ADC转换开始引脚的控制
 * 
 * 本模块为ADC驱动提供引脚配置支持，是ADC驱动的重要组成部分。
 * @{
 */


/// @cond INTERNAL_MACROS
/* 引脚定义 */
//ADC_BUSY GPIO5[11]
#define ADC_BUSY_PIN            92
#define ADC_BUSY_MASK           SYSCFG_PINMUX11_PINMUX11_19_16
#define ADC_BUSY_ENABLE         (SYSCFG_PINMUX11_PINMUX11_19_16_GPIO5_11 << SYSCFG_PINMUX11_PINMUX11_19_16_SHIFT)
//ADC_RESET GPIO5[12]
#define ADC_RESET_PIN           93
#define ADC_RESET_MASK          SYSCFG_PINMUX11_PINMUX11_15_12
#define ADC_RESET_ENABLE        (SYSCFG_PINMUX11_PINMUX11_15_12_GPIO5_12 << SYSCFG_PINMUX11_PINMUX11_15_12_SHIFT)
//ADC_CONVST GPIO5[13]
#define ADC_CONVST_PIN          94
#define ADC_CONVST_MASK         SYSCFG_PINMUX11_PINMUX11_11_8
#define ADC_CONVST_ENABLE       (SYSCFG_PINMUX11_PINMUX11_11_8_GPIO5_13 << SYSCFG_PINMUX11_PINMUX11_11_8_SHIFT)
/// @endcond /* INTERNAL_MACROS 结束 */


/**
 * @brief 初始化ADC引脚
 * @details 初始化ADC相关的引脚，配置引脚复用功能。
 */
void Adc_Pin_Init(void);

/**
 * @brief 设置ADC复位引脚状态
 * @param bitState: 状态(1: 复位, 0: 正常)
 */
void Adc_Reset_PinSet(unsigned int bitState);

/**
 * @brief 设置ADC转换开始引脚状态
 * @param bitState: 状态(1: 开始转换, 0: 停止转换)
 */
void Adc_Convst_PinSet(unsigned int bitState);

/** @} */

#endif /* _ADC_PIN_H_ */
