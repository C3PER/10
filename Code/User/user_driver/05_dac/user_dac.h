/**
 * @file user_dac.h
 * @brief DAC 用户使用示例头文件
 * @details 定义 DAC 用户使用示例的函数声明，提供DAC的使用示例。
 * @ingroup DAC_EXAMPLE
 */

#ifndef _USER_DAC_H_
#define _USER_DAC_H_

/**
 * @defgroup DAC_EXAMPLE DAC应用示例
 * @ingroup DAC
 * @brief DAC应用示例模块
 * @details 
 * 提供DAC的使用示例，展示如何初始化和使用DAC进行数据输出。
 * 
 * 主要职责包括：
 * - 提供DAC输出示例函数
 * - 展示DAC的初始化和配置方法
 * - 展示DAC数据输出和处理流程
 * 
 * 本模块为用户提供DAC的使用参考，帮助用户快速上手DAC功能。
 * @{
 */

/**
 * @brief DAC 输出示例
 * @details 展示如何初始化和使用DAC进行数据输出。
 * DAC使用方法是：
 * 1. 初始化DAC，包括配置输出频率、输出长度等参数。
 * 2. 启动DAC，开始输出数据。
 * 3. 等待输出完成，更新输出数据。
 */
void Dac_Example(void);

/** @} */

#endif /* _USER_DAC_H_ */
