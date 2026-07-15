/**
 * @file user_adc.h
 * @brief ADC用户应用头文件
 * @details 声明ADC的用户应用函数，提供ADC的使用示例。
 * @ingroup ADC_EXAMPLE
 */

#ifndef _USER_ADC_H_
#define _USER_ADC_H_

/**
 * @defgroup ADC_EXAMPLE ADC应用示例
 * @ingroup ADC
 * @brief ADC应用示例模块
 * @details 
 * 提供ADC的使用示例，展示如何初始化和使用ADC进行数据采集。
 * 
 * 主要职责包括：
 * - 提供ADC采样示例函数
 * - 展示ADC的初始化和配置方法
 * - 展示ADC数据采集和处理流程
 * 
 * 本模块为用户提供ADC的使用参考，帮助用户快速上手ADC功能。
 * @{
 */

/**
 * @brief ADC 采样示例
 * @details 展示如何初始化和使用ADC进行数据采集。
 * ADC使用方法是：
 * 1. 初始化ADC，包括配置采样频率、采样长度等参数。
 * 2. 启动ADC，开始采集数据。
 * 3. 等待采集完成，获取采集到的数据。
 * 4. 处理采集到的数据，如进行数据转换、存储等操作。
 * 5. 使用调试功能，在Graph窗口查看采集到的数据。
 */
void Adc_Example(void);

/** @} */

#endif /* _USER_ADC_H_ */
