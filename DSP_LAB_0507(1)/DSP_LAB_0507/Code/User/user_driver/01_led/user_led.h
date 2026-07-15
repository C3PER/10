/**
 * @file user_led.h
 * @brief LED应用示例文件
 * @details 定义LED应用示例，提供LED使用示例。
 * @ingroup LED_EXAMPLE
 */

#ifndef _USER_LED_H_
#define _USER_LED_H_

/**
 * @defgroup LED_EXAMPLE LED应用示例
 * @ingroup LED
 * @brief LED应用示例模块
 * @details 
 * 定义LED应用示例，提供LED使用示例。
 * 
 * 主要功能包括：
 * - 提供LED使用示例函数，展示如何初始化和控制LED
 * - 演示LED的各种控制方式，如点亮、关闭和翻转
 * 
 * 本模块是LED驱动的应用示例，展示了如何使用LED驱动接口。
 * @{
 */

/**
 * @brief LED使用示例
 * @details 演示如何初始化和控制LED，实现LED的闪烁效果。
 * 
 * 具体实现：
 * 1. 初始化系统和LED
 * 2. 进入无限循环，交替执行以下操作：
 *    - 点亮核心板LED，翻转开发板蓝色LED
 *    - 关闭核心板LED，翻转开发板红色LED
 *    - 每次操作后延迟500毫秒
 */
void Led_Example(void);

/** @} */

#endif /* _USER_LED_H_ */
