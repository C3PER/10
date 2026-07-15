/**
 * @file adc_pru.h
 * @brief ADC PRU配置文件
 * @details 定义ADC的PRU相关宏和函数，负责ADC的PRU配置。
 * @ingroup ADC_PRU
 */

#ifndef _ADC_PRU_H_
#define _ADC_PRU_H_

/**
 * @defgroup ADC_PRU ADC PRU控制
 * @ingroup ADC_API
 * @brief ADC PRU控制模块
 * @details 
 * 定义ADC的PRU相关宏和函数，负责ADC的PRU配置。
 * 
 * 主要职责包括：
 * - 实现PRU1的初始化配置
 * - 为ADC提供PRU控制支持
 * 
 * 本模块为ADC驱动提供PRU控制支持，实现ADC的高性能数据采集。
 * @{
 */

/**
 * @brief ADC PRU1初始化函数
 * @details 初始化ADC的PRU1，包括使能PSC模块、配置GPIO管脚复用和方向。
 */
void Adc_Pru1_Init(void);

/** @} */

#endif /* _ADC_PRU_H_ */
