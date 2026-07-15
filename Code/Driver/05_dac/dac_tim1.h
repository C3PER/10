/**
 * @file dac_tim1.h
 * @brief DAC定时器1配置头文件
 * @details 定义DAC定时器1相关的函数声明，实现DAC的定时输出控制。
 * @ingroup DAC_TIM1
 */

#ifndef _DAC_TIM1_H_
#define _DAC_TIM1_H_

/**
 * @defgroup DAC_TIM1 DAC定时器模块
 * @ingroup DAC_API
 * @brief DAC定时器模块
 * @details 
 * 定义DAC定时器1相关的函数声明，实现DAC的定时输出控制。
 * 
 * 主要职责包括：
 * - 实现定时器1的初始化配置
 * - 实现定时器1的启动和停止
 * - 为DAC提供定时输出触发信号
 * 
 * 本模块为DAC驱动提供定时控制支持，实现DAC的定时输出功能。
 * @{
 */

/**
 * @brief 初始化DAC定时器1
 * @details 初始化定时器1，设置定时频率。
 * @param freq 定时器频率
 */
void Dac_Timer1_Init(unsigned int freq);

/**
 * @brief 启动DAC定时器1
 * @details 启动定时器1，开始计时。
 */
void Dac_Timer1_Start(void);

/**
 * @brief 停止DAC定时器1
 * @details 停止定时器1，停止计时。
 */
void Dac_Timer1_Stop(void);

/** @} */

#endif /* _DAC_TIM1_H_ */
