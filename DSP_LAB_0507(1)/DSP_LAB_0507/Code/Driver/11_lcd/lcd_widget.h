/**
 * @file lcd_widget.h
 * @brief LCD控件配置文件
 * @details 定义LCD控件的接口和布局宏定义。
 * @ingroup LCD_WIDGET
 */

#ifndef _LCD_WIDGET_H_
#define _LCD_WIDGET_H_


/**
 * @defgroup LCD_WIDGET LCD控件
 * @ingroup LCD_API
 * @brief LCD控件模块
 * @details 
 * 定义LCD控件的接口和布局宏定义。
 * 
 * 主要职责包括：
 * - 创建UI控件（按钮、文本框、容器等）
 * - 初始化控件布局和样式
 * - 提供控件事件处理能力
 * 
 * 本模块构建完整的UI界面布局。
 * @{
 */

/// @cond INTERNAL_MACROS
// 布局坐标 (基于 800x480 屏幕的假设布局)
// 容器
#define PANEL_X         0
#define PANEL_Y         0
#define PANEL_W         800  // 界面宽度
#define PANEL_H         480  // 界面高度
#define PANEL_FC        ClrNavy
#define PANEL_TC        ClrWhite
// 标题
#define HEADING_H       40
#define HEADING_FC      ClrMidnightBlue
#define HEADING_TC      ClrWhite
// 文本
#define TXT1_X          160
#define TXT1_Y          400
#define TXT2_X          160
#define TXT2_Y          430
#define TXT_W           480
#define TXT_H           30
#define TXT_FC          ClrDarkSlateGray
#define TXT_OC          ClrLightSteelBlue
#define TXT_TC          ClrLightSkyBlue
// 按钮
#define BTN_W           80
#define BTN_H           60
#define BTN1_X          40
#define BTN1_Y          80
#define BTN2_X          680
#define BTN2_Y          80
#define BTN3_X          40
#define BTN3_Y          160
#define BTN4_X          680
#define BTN4_Y          160
#define BTN5_X          40
#define BTN5_Y          240
#define BTN6_X          680
#define BTN6_Y          240
#define BTN7_X          40
#define BTN7_Y          320
#define BTN8_X          680
#define BTN8_Y          320
#define BTN_FC          ClrSteelBlue
#define BTN_PFC         ClrRoyalBlue
#define BTN_OC          ClrWhite
#define BTN_TC          ClrWhite
/// @endcond /* INTERNAL_MACROS 结束 */


/**
 * @brief 初始化LCD控件
 * @details 创建UI控件并初始化显示
 */
void Lcd_Widget_Init(void);

/** @} */

#endif /* _LCD_WIDGET_H_ */
