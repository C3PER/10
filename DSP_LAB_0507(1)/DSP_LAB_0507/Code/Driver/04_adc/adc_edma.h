/**
 * @file adc_edma.h
 * @brief ADC EDMA控制实现文件
 * @details 定义ADC的EDMA相关宏和函数，实现ADC数据的DMA传输。
 * @ingroup ADC_EDMA
 */

#ifndef _ADC_EDMA_H_
#define _ADC_EDMA_H_

/**
 * @defgroup ADC_EDMA ADC EDMA控制
 * @ingroup ADC_API
 * @brief ADC EDMA控制模块
 * @details 
 * 定义ADC的EDMA相关宏和函数，实现ADC数据的EDMA传输。
 * 
 * 主要职责包括：
 * - 定义EDMA相关的宏
 * - 实现EDMA的初始化配置
 * - 实现ADC数据的EDMA传输
 * 
 * 本模块为ADC驱动提供EDMA传输支持，提高数据传输效率。
 * @{
 */

/// @cond INTERNAL_MACROS
/* EDMA相关宏定义 */
// 最大 ACOUNT
#define MAX_ACOUNT        (2u)            // 16bit = 2*8bit
// 最大 BCOUNT
#define MAX_BCOUNT        (8u)            // 采集8个ADC通道
// 最大 CCOUNT（此参数在初始化edma传输时会被修改，让用户配置需要的采样长度）
#define MAX_CCOUNT        (1024u)         // 每个ADC通道采集ADC_SAMPLE_LEN个样本
// ADC一个通道最大长度
#define MAX_BUFFER_SIZE   (MAX_ACOUNT * MAX_BCOUNT * MAX_CCOUNT)
/// @endcond /* INTERNAL_MACROS 结束 */

/**
 * @brief 初始化ADC EDMA
 * @details 初始化ADC的EDMA传输，配置传输参数。
 * @param sampleLen 采样长度
 */
void Adc_EDMA_Init(unsigned int sampleLen);

/** @} */

#endif /* _ADC_EDMA_H_ */
