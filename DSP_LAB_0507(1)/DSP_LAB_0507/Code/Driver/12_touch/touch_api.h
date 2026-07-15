/**
 * @file touch_api.h
 * @brief Touch用户接口文件
 * @details 定义Touch的用户接口，为用户提供统一的Touch操作入口。
 * @ingroup TOUCH_API
 */

#ifndef _TOUCH_API_H_
#define _TOUCH_API_H_

/**
 * @defgroup TOUCH_API Touch用户接口
 * @ingroup TOUCH
 * @brief Touch用户接口模块
 * @details 
 * 定义Touch的用户接口，为用户提供统一的Touch操作入口。
 * 
 * TOUCH_API模块包含以下子模块：
 * - @ref TOUCH_DRV ：Touch驱动模块，实现Touch的初始化和GT1151芯片控制
 * - @ref TOUCH_PIN ：Touch引脚控制模块，负责Touch引脚的初始化和中断配置
 * - @ref TOUCH_SCAN ：Touch扫描模块，实现触摸屏扫描功能
 * - @ref TOUCH_BUTTON ：Touch按钮模块，处理UI按钮的触摸响应
 * 
 * 主要包含以下内容：
 * - 触摸标志位和状态变量
 * - 触摸坐标变量
 * - 按钮标志位
 * - 初始化和扫描函数
 * 
 * 用户通过此模块可以方便地使用Touch功能。
 * @{
 */

/**
 * @name Touch标志位和状态
 * @{
 */
/** @brief 触摸中断标志位 
 * @details 触摸发生时置1，需要用户使用后手动置为0。
*/
extern unsigned char FLAG_TOUCH;

/** @brief 触摸状态 
 * @details 0: 未触摸 1: 触摸
*/
extern unsigned char Touch_Sta;

/** @brief 触摸点的X坐标 */
extern unsigned int Touch_X;

/** @brief 触摸点的Y坐标 */
extern unsigned int Touch_Y;
/** @} */

/**
 * @name Touch按钮标志位
 * @{
 */
/** @brief 通用按钮标志位 */
extern volatile unsigned char FLAG_BUTTON;
/** @brief 按钮1标志位 */
extern volatile unsigned char FLAG_BUTTON_1;
/** @brief 按钮2标志位 */
extern volatile unsigned char FLAG_BUTTON_2;
/** @brief 按钮3标志位 */
extern volatile unsigned char FLAG_BUTTON_3;
/** @brief 按钮4标志位 */
extern volatile unsigned char FLAG_BUTTON_4;
/** @brief 按钮5标志位 */
extern volatile unsigned char FLAG_BUTTON_5;
/** @brief 按钮6标志位 */
extern volatile unsigned char FLAG_BUTTON_6;
/** @brief 按钮7标志位 */
extern volatile unsigned char FLAG_BUTTON_7;
/** @brief 按钮8标志位 */
extern volatile unsigned char FLAG_BUTTON_8;
/** @} */

/**
 * @name Touch函数声明
 * @{
 */

/**
 * @brief 初始化Touch
 * @details 初始化Touch相关的IIC接口、引脚和GT1151芯片
 */
void Touch_Init(void);

/**
 * @brief 扫描Touch
 * @details 扫描触摸屏状态，更新触摸坐标和按钮标志位
 */
void Touch_Scan(void);

/** @} */

/** @} */

#endif /* _TOUCH_API_H_ */

/**
 * @defgroup TOUCH Touch模块
 * @brief Touch完整功能模块
 * @details 
 * TOUCH模块为用户提供统一的Touch控制接口，支持Touch的初始化、扫描。
 * 
 * TOUCH模块包含以下子模块：
 * - @ref TOUCH_API ：Touch用户接口模块，包含设备驱动和引脚控制。
 * - @ref TOUCH_EXAMPLE ：Touch应用示例模块，提供Touch使用示例。建议参考其中的代码，循序渐进的学习。
 * 
 * @par 快速使用
 * 触摸的应用方法也很多，示例 @ref Touch_Example() 中使用的是和控件配合的触摸响应方式。
 * Touch触摸程序请参考 @ref TOUCH_EXAMPLE 模块的示例代码。
 * 
 * @note 
 * 程序中部分说明：
 * - 初始化Touch：调用 @ref Touch_Init() 初始化Touch。
 * - 检查 @ref FLAG_TOUCH 触摸标志位，判断是否有触摸发生。
 * - 如果触摸发生，调用 @ref Touch_Scan() 扫描Touch状态，更新相应的标志位。
 * - 根据 @ref FLAG_BUTTON 按钮标志位，判断按钮是否被按下。
 * - 根据 @ref FLAG_BUTTON_1 按钮1标志位，判断按钮1是否被按下，以此类推。
 * 
 */
