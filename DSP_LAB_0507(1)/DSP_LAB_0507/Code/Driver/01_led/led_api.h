/**
 * @file led_api.h
 * @brief LED用户接口文件
 * @details 定义LED的用户接口、枚举类型。
 *          本模块统领底层驱动与引脚控制，为用户提供统一入口。
 * @ingroup LED_API
 */

#ifndef _LED_API_H_
#define _LED_API_H_

/**
 * @defgroup LED_API LED用户接口
 * @ingroup LED
 * @brief LED用户接口模块
 * @details 
 * 定义LED的控制参数、函数声明，为用户提供统一入口。
 * 
 * 主要包含以下内容：
 * - LED名称定义：开发板三色LED1~LED4、核心板LED1~LED2。
 * - LED状态定义：LED灯点亮、关闭、翻转，三种状态。
 * - 初始化函数：初始化LED相关的GPIO引脚，设置为输出模式
 * - 控制函数：控制指定LED的状态，支持点亮、关闭和翻转操作
 * 
 * 用户通过此模块可以方便地控制各种LED的状态。
 * 
 * LED_API模块包含以下子模块：
 * - @ref LED_DRV ：LED设备驱动模块，实现LED的初始化和控制逻辑
 * - @ref LED_PIN ：LED引脚控制模块，负责LED引脚的初始化和状态设置
 * 
 * @{
 */


/**
 * @name LED 控制参数
 * @details 
 * - LED名称定义：开发板三色LED1~LED4、核心板LED1~LED2。 
 * - LED状态定义：LED灯点亮、关闭、翻转，三种状态。
 * @{
 */

/**
 * @brief LED索引枚举
 * @details 定义所有LED的索引值，用于标识不同的LED硬件节点
 */
typedef enum {
    LED1_BLUE = 0,     //!< 开发板LED1蓝色
    LED1_RED,          //!< 开发板LED1红色
    LED2_BLUE,         //!< 开发板LED2蓝色
    LED2_RED,          //!< 开发板LED2红色
    LED3_BLUE,         //!< 开发板LED3蓝色
    LED3_RED,          //!< 开发板LED3红色
    LED4_BLUE,         //!< 开发板LED4蓝色
    LED4_RED,          //!< 开发板LED4红色
    LED1_CORE,         //!< 核心板LED1
    LED2_CORE,         //!< 核心板LED2
    LED_MAX            //!< LED数量上限
} LedIndex;

/**
 * @brief LED状态枚举
 * @details 定义LED的控制状态指令
 */
typedef enum {
    LED_ON = 0,        //!< LED点亮
    LED_OFF,           //!< LED关闭
    LED_TOGGLE         //!< LED状态翻转
} LedState;

/** @} */


/**
 * @name LED 函数声明
 * @details 
 * - 初始化函数：初始化LED相关的GPIO引脚，设置为输出模式
 * - 控制函数：控制指定LED的状态，支持点亮、关闭和翻转操作
 * @{
 */
/**
 * @brief LED初始化函数
 * @details 初始化LED相关的GPIO引脚，设置为输出模式
 */
void Led_Init(void);

/**
 * @brief LED控制函数
 * @details 控制指定LED的状态，支持点亮、关闭和翻转操作
 * @param ledNumber LED编号(LedIndex枚举值)
 * @param ledState 状态(LedState枚举值)
 */
void Led_Control(LedIndex ledNumber, LedState ledState);

/** @} */

/** @} */

#endif /* _LED_API_H_ */


/**
 * @defgroup LED LED模块
 * @brief LED完整功能模块
 * @details 
 * LED模块为用户提供统一的LED控制接口，支持LED的初始化和状态控制。
 * 
 * LED模块包含以下子模块：
 * - @ref LED_API ：LED用户接口模块，包含设备驱动和引脚控制。
 * - @ref LED_EXAMPLE ：LED应用示例模块，提供LED使用示例。
 * 
 * @par 快速使用
 * 最基础的LED的闪烁效果流程如下。
 * @code{.c}
 * #include "led_api.h"    // LED用户接口
 * #include "system.h"     // 中断和延时初始化
 * #include "delay.h"      // 延时函数
 * int main(void)
 * {
 *     Sys_Init();         // 初始化delay函数
 *     Led_Init();         // 初始化LED
 *     for (;;)
 *     {
 *         // 翻转开发板蓝色LED1
 *         Led_Control(LED1_BLUE, LED_TOGGLE);
 *         delay(500);     // 延时500ms
 *     }
 * }
 * @endcode
 * 
 * @note 
 * 程序中部分说明：
 * - 包含 @ref led_api.h 头文件，使用LED用户接口。
 * - 调用 @ref Led_Init() 初始化LED灯。
 * - 通过 @ref Led_Control() 实现LED的闪烁效果。
 * - @ref LED1_BLUE 代表开发板LED1蓝色灯。
 * - @ref LED_TOGGLE 代表状态翻转操作。
 * 
 * @warning
 * 毫秒级延时函数 “delay”，使用前需要调用 @ref Sys_Init() 进行初始化。
 * 
 * @todo
 * 请读者参考 @ref LED 完成以下步骤，实现LED的闪烁效果：
 * -# 调用 @ref Led_Init() 初始化LED灯。
 * -# 通过 @ref Led_Control() 实现LED的闪烁效果。
 * 
 */
