/**
 * @file user_touch.h
 * @brief Touch用户使用接口文件
 * @details 定义Touch用户示例的接口。
 * @ingroup TOUCH_EXAMPLE
 */

#ifndef _USER_TOUCH_H_
#define _USER_TOUCH_H_

/**
 * @defgroup TOUCH_EXAMPLE Touch用户示例
 * @ingroup TOUCH
 * @brief Touch用户示例模块
 * @details 
 * 定义Touch用户示例的接口，展示如何初始化和使用Touch。
 * 
 * 本模块提供一个完整的Touch使用示例，包括：
 * - 系统初始化
 * - LED、LCD、Touch的初始化
 * - 触摸事件处理循环
 * - 按钮事件响应
 * 
 * @{
 */


/**
 * @brief Touch示例函数
 * @details 展示Touch的初始化和使用方法，包含触摸事件处理和按钮响应
 */
void Touch_Example(void);

/** @} */

#endif /* _USER_TOUCH_H_ */
