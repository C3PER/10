/**
 * @file timer_api.h
 * @brief 定时器用户接口文件
 * @details 定义定时器的用户接口，为用户提供统一的定时器操作入口。
 * @ingroup TIMER_API
 */

#ifndef _TIMER_API_H_
#define _TIMER_API_H_

/**
 * @defgroup TIMER_API 定时器用户接口
 * @ingroup TIMER
 * @brief 定时器用户接口模块
 * @details 
 * 定义定时器的用户接口，为用户提供统一的定时器操作入口。
 * 
 * TIMER_API模块包含以下子模块：
 * - @ref TIMER_DRV ：定时器外设驱动模块，实现定时器的初始化和中断处理逻辑
 * 
 * 主要包含以下内容：
 * - 定时器频率宏定义：定义定时器支持的频率
 * - 定时器控制标志位：用于标识定时器的中断状态
 * - 初始化函数：初始化定时器，设置定时器频率
 * 
 * 用户通过此模块可以方便地使用定时器功能，通过检查标志位来响应定时器中断事件。
 * @{
 */

/**
 * @name 定时器频率宏定义
 * @details 
 * 定时器外设驱动模块部分频率的示例。
 * @{
 */
#define TIMER_1HZ      1        //!< 1Hz频率
#define TIMER_20HZ     20       //!< 20Hz频率
#define TIMER_500HZ    500      //!< 500Hz频率
#define TIMER_1kHZ     1000     //!< 1kHz频率
/** @} */

/**
 * @name TIM2 标志位
 * @details 
 * 定时器中断标志位，用于标识定时器的中断状态。
 * @{
 */
/* 定时器控制标志位 */
/** @brief 定时器2中断标志位 
 * @details 定时器2中断时置1，需要用户使用后手动置为0。
*/
extern volatile unsigned char FLAG_TIM2;
/** @} */

/**
 * @name TIM2 函数声明
 * @details 
 * 定时器初始化函数，用于初始化定时器和设置频率。
 * @{
 */
/**
 * @brief 初始化定时器2
 * @details 初始化定时器2，设置定时器频率和中断。
 * @param freq 定时器频率
 */
void Tim2_Init(unsigned int freq);
/** @} */

/** @} */

#endif /* _TIMER_API_H_ */

/**
 * @defgroup TIMER TIMER模块
 * @brief 定时器完整功能模块
 * @details 
 * 定时器模块为用户提供统一的定时器控制接口，支持定时器的初始化和中断处理。
 * 
 * 定时器模块包含以下子模块：
 * - @ref TIMER_API ：定时器用户接口模块，包含设备驱动
 * - @ref TIMER_EXAMPLE ：定时器应用示例模块，提供定时器使用示例
 * 
 * @par 快速使用
 * 最基础的定时器中断检测流程如下。
 * @code{.c}
 * #include "timer_api.h"    // 定时器用户接口
 * #include "system.h"       // 中断和延时初始化
 * #include "delay.h"        // 延时函数
 * int main(void)
 * {
 *     Sys_Init();           // 初始化系统中断
 *     Tim2_Init(TIMER_1HZ); // 初始化定时器2，设置为1Hz
 *     for (;;)
 *     {
 *         // 检测定时器中断
 *         if (FLAG_TIM2)    // 检查定时器2是否中断
 *         {
 *             FLAG_TIM2 = 0; // 清除标志位
 *             // 执行定时器中断处理逻辑
 *         }
 *     }
 * }
 * @endcode
 * 
 * @note 
 * 程序中部分说明：
 * - 包含 @ref timer_api.h 头文件，使用定时器用户接口。
 * - 调用 @ref Tim2_Init() 初始化定时器2，设置频率1Hz（如 @ref TIMER_1HZ ）。
 * - 通过检查标志位（如 @ref FLAG_TIM2 ）来响应定时器中断事件。
 * - 标志位在定时器中断时置1，需要用户使用后手动置为0。
 * 
 * @warning
 * 定时器控制涉及中断处理函数，使用前需要调用 @ref Sys_Init() 初始化系统中断。
 * 
 * @todo
 * 请读者参考 @ref TIMER 完成以下步骤，实现定时器中断功能：
 * -# 调用 @ref Tim2_Init() 初始化定时器2，设置合适的频率。
 * -# 通过检查标志位 @ref FLAG_TIM2 来响应定时器中断事件。
 * 
 */
