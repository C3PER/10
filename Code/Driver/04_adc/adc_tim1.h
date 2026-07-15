/**
 * @file adc_tim1.h
 * @brief ADC定时器配置文件
 * @details 定义ADC的定时器相关宏和函数，实现ADC的定时采样控制。
 * @ingroup ADC_TIM1
 */

#ifndef _ADC_TIM1_H_
#define _ADC_TIM1_H_

/**
 * @defgroup ADC_TIM1 ADC定时器控制
 * @ingroup ADC_API
 * @brief ADC定时器控制模块
 * @details 
 * 定义ADC的定时器相关宏和函数，实现ADC的定时采样控制。
 * 
 * 主要职责包括：
 * - 实现定时器1的初始化配置
 * - 实现定时器1的启动和停止
 * - 为ADC提供定时采样触发信号
 * 
 * 本模块为ADC驱动提供定时控制支持，实现ADC的定时采样功能。
 * @{
 */

/**
 * @brief 初始化ADC定时器1
 * @details 初始化定时器1，设置定时周期。
 * @param period 定时周期
 */
void Adc_Timer1_Init(unsigned int period);

/**
 * @brief 启动ADC定时器1
 * @details 启动定时器1，开始计时。
 */
void Adc_Timer1_Start(void);

/**
 * @brief 停止ADC定时器1
 * @details 停止定时器1，停止计时。
 */
void Adc_Timer1_Stop(void);

/** @} */

#endif /* _ADC_TIM1_H_ */
