/**
 * @file dac_pru.h
 * @brief DAC PRU配置头文件
 * @details 定义DAC PRU相关的函数声明，实现DAC的并行输出控制。
 * @ingroup DAC_PRU
 */

#ifndef _DAC_PRU_H_
#define _DAC_PRU_H_

/**
 * @defgroup DAC_PRU DAC PRU模块
 * @ingroup DAC_API
 * @brief DAC PRU模块
 * @details 
 * 定义DAC PRU相关的函数声明，实现DAC的并行输出控制。
 * 
 * 主要职责包括：
 * - 实现PRU0的初始化配置
 * - 实现DAC的并行输出控制
 * - 提高DAC输出的速度和效率
 * 
 * 本模块为DAC驱动提供PRU支持，实现DAC的高速并行输出功能。
 * @{
 */

/**
 * @brief 初始化DAC PRU0
 * @details 初始化PRU0，配置其用于DAC的并行输出控制。
 */
void Dac_Pru0_Init(void);

/** @} */

#endif /* _DAC_PRU_H_ */
