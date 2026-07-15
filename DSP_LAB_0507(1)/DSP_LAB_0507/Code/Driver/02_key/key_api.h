/**
 * @file key_api.h
 * @brief 按键用户接口文件
 * @details 定义按键的用户接口，为用户提供统一的按键操作入口。
 * @ingroup KEY_API
 */

#ifndef _KEY_API_H_
#define _KEY_API_H_

/**
 * @defgroup KEY_API 按键用户接口
 * @ingroup KEY
 * @brief 按键用户接口模块
 * @details 
 * 定义按键的用户接口，为用户提供统一的按键操作入口。
 * 
 * KEY_API模块包含以下子模块：
 * - @ref KEY_DRV ：按键设备驱动模块，实现按键的初始化和中断处理逻辑
 * - @ref KEY_PIN ：按键引脚控制模块，负责按键引脚的初始化和中断设置
 * 
 * 主要包含以下内容：
 * - 按键控制标志位：用于标识按键的按下状态
 * - 初始化函数：初始化按键相关的GPIO引脚和中断
 * 
 * 用户通过此模块可以方便地使用按键功能，通过检查标志位来响应按键事件。
 * @{
 */

/**
 * @name 按键标志位
 * @details 
 * - 按键标志位：用于标识按键的按下状态
 * @{
 */

/* 按键控制标志位 */
/** @brief KEY1按键标志位 
 * @details 按键按下时置1，需要用户使用后手动置为0。
*/
extern volatile unsigned char FLAG_KEY1;
/** @brief KEY2按键标志位 
 * @details 按键按下时置1，需要用户使用后手动置为0。
*/
extern volatile unsigned char FLAG_KEY2;
/** @brief KEY3按键标志位 
 * @details 按键按下时置1，需要用户使用后手动置为0。
*/
extern volatile unsigned char FLAG_KEY3;
/** @brief KEY4按键标志位 
 * @details 按键按下时置1，需要用户使用后手动置为0。
*/ 
extern volatile unsigned char FLAG_KEY4;
/** @brief KEY5按键标志位 
 * @details 按键按下时置1，需要用户使用后手动置为0。
*/
extern volatile unsigned char FLAG_KEY5;

/** @} */

/**
 * @name 按键函数声明
 * @details 
 * - 初始化函数：初始化按键相关的GPIO引脚和中断
 * @{
 */

/**
 * @brief 初始化按键
 * @details 初始化按键相关的GPIO引脚和中断，开启按键功能。
 */
void Key_Init(void);

/** @} */

/** @} */

#endif /* _KEY_API_H_ */


/**
 * @defgroup KEY KEY模块
 * @brief 按键完整功能模块
 * @details 
 * 按键模块为用户提供统一的按键控制接口，支持按键的初始化和中断处理。
 * 
 * 按键模块包含以下子模块：
 * - @ref KEY_API ：按键用户接口模块，包含设备驱动和引脚控制
 * - @ref KEY_EXAMPLE ：按键应用示例模块，提供按键使用示例
 * 
 * @par 快速使用
 * 最基础的按键检测流程如下。
 * @code{.c}
 * #include "key_api.h"    // 按键用户接口
 * #include "system.h"     // 中断和延时初始化
 * int main(void)
 * {
 *     Sys_Init();         // 初始化系统中断
 *     Key_Init();         // 初始化按键
 *     for (;;)
 *     {
 *         // 检测按键状态
 *         if (FLAG_KEY1)   // 检查KEY1是否按下
 *         {
 *             FLAG_KEY1 = 0; // 清除标志位
 *             // 执行按键处理逻辑
 *         }
 *     }
 * }
 * @endcode
 * 
 * @note 
 * 程序中部分说明：
 * - 包含 @ref key_api.h 头文件，使用按键用户接口。
 * - 调用 @ref Key_Init() 初始化按键。
 * - 通过检查标志位（如 @ref FLAG_KEY1 ）来响应按键事件。
 * - 标志位在按键按下时置1，需要用户使用后手动置为0。
 * 
 * @warning
 * 按键控制涉及中断处理函数，使用前需要调用 @ref Sys_Init() 初始化系统中断。
 * 
 * @todo
 * 请读者参考 @ref KEY 完成以下步骤，实现按键检测功能：
 * -# 调用 @ref Key_Init() 初始化按键。
 * -# 通过检查标志位（如 @ref FLAG_KEY1 ）来响应按键事件。
 * 
 */
