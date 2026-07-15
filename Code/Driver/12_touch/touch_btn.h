/**
 * @file touch_btn.h
 * @brief Touch按钮控制文件
 * @details 定义Touch按钮控制的接口。
 * @ingroup TOUCH_BUTTON
 */

#ifndef _TOUCH_BTN_H_
#define _TOUCH_BTN_H_

/**
 * @defgroup TOUCH_BUTTON Touch按钮控制
 * @ingroup TOUCH_API
 * @brief Touch按钮控制模块
 * @details 
 * 定义Touch按钮控制的接口。
 * 
 * 主要职责包括：
 * - 处理UI按钮的触摸响应
 * - 更新按钮标志位
 * - 提供按钮回调处理函数
 * 
 * 本模块实现触摸按钮的事件处理功能。
 * @{
 */


/**
 * @brief 按钮控件响应函数
 * @details 根据触摸状态和坐标处理按钮事件
 * @param Touch_Sta 触摸状态(0: 未触摸 1: 触摸)
 * @param Touch_X 触摸点的X坐标
 * @param Touch_Y 触摸点的Y坐标
 */
void Widget_Btn_App(unsigned char Touch_Sta, unsigned int Touch_X, unsigned int Touch_Y);



/**
 * @name 按钮控制标志位
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

/** @} */

#endif /* _TOUCH_BTN_H_ */
