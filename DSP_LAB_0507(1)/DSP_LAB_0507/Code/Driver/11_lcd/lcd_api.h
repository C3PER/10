/**
 * @file lcd_api.h
 * @brief LCD用户接口文件
 * @details 定义LCD的用户接口，为用户提供统一的LCD操作入口。
 * @ingroup LCD_API
 */

#ifndef _LCD_API_H_
#define _LCD_API_H_

#include "grlib.h"          // LCD图形库
#include "widget.h"         // LCD控件
#include "container.h"      // 提供容器控件
#include "canvas.h"         // 提供画布控件
#include "pushbutton.h"     // 提供按钮控件

/**
 * @defgroup LCD_API LCD用户接口
 * @ingroup LCD
 * @brief LCD用户接口模块
 * @details 
 * 定义LCD的用户接口，为用户提供统一的LCD操作入口。
 * 
 * LCD_API模块包含以下子模块：
 * - @ref LCD_DRV ：LCD设备驱动模块，实现LCD的初始化逻辑
 * - @ref LCD_PIN ：LCD引脚控制模块，负责LCD引脚的初始化
 * - @ref LCD_CTRL ：LCD控制器控制模块，配置LCD控制器的时钟参数
 * - @ref LCD_RASTER ：LCD光栅控制模块，配置LCD显示模式和参数
 * - @ref LCD_DMA ：LCD DMA控制模块，管理单缓冲显示机制
 * - @ref LCD_GRLIB ：LCD图形库模块，初始化grlib图形库
 * - @ref LCD_WIDGET ：LCD控件模块，创建UI控件
 * 
 * 主要包含以下内容：
 * - 显示对象和上下文：用于图形绘制
 * - UI控件：主面板、标题栏、文本框、按钮等
 * - 初始化函数：初始化LCD显示系统
 * 
 * 用户通过此模块可以方便地使用LCD显示功能。
 * @{
 */

/**
 * @name LCD显示对象和上下文
 * @{
 */
extern tDisplay Lcd_Display;
extern tContext Lcd_Context;
extern tRectangle Lcd_Rectangle;
/** @} */

/**
 * @name LCD UI控件
 * @{
 */
extern tContainerWidget     g_sMainPanel;
extern tCanvasWidget        g_sHeading;
extern tCanvasWidget        g_sTxt1;
extern tCanvasWidget        g_sTxt2;
extern tPushButtonWidget    g_sBtn1;
extern tPushButtonWidget    g_sBtn2;
extern tPushButtonWidget    g_sBtn3;
extern tPushButtonWidget    g_sBtn4;
extern tPushButtonWidget    g_sBtn5;
extern tPushButtonWidget    g_sBtn6;
extern tPushButtonWidget    g_sBtn7;
extern tPushButtonWidget    g_sBtn8;
/** @} */

/**
 * @name LCD函数声明
 * @{
 */

/**
 * @brief 初始化LCD
 * @details 初始化LCD显示系统，包括引脚、控制器、光栅、DMA和图形库
 */
void Lcd_Init(void);

/**
 * @brief 初始化LCD控件
 * @details 创建UI控件并初始化显示
 */
void Lcd_Widget_Init(void);

/** @} */

/** @} */

#endif /* _LCD_API_H_ */

/**
 * @defgroup LCD LCD模块
 * @brief LCD完整功能模块
 * @details 
 * LCD模块为用户提供统一的LCD控制接口，支持LCD的初始化、显示。
 * 
 * LCD模块包含以下子模块：
 * - @ref LCD_API ：LCD用户接口模块，包含设备驱动和引脚控制。
 * - @ref LCD_EXAMPLE ：LCD应用示例模块，提供LCD使用示例。建议参考其中的代码，循序渐进的学习。
 * 
 * @par 快速使用
 * LCD显示有两种方式，分别是：
 * - 一种是直接绘制图形 @ref Lcd_Example1() ，用于绘制简单的图形和文本。
 * - 另一种是使用控件 @ref Lcd_Example2() ，用于创建和管理UI控件，如按钮、文本框等。
 * 详细使用请参考 @ref LCD_EXAMPLE 模块的示例代码。
 * 
 * @note 
 * 程序中部分说明：
 * - 使用屏幕显示功能，需要包含 @ref lcd_api.h 头文件，使用LCD用户接口。
 * - 调用 @ref Lcd_Init() 初始化LCD显示。
 * - 如果需要使用控件，需要调用 @ref Lcd_Widget_Init() 初始化控件。
 * - 其它使用方法请参考 @ref LCD_EXAMPLE 模块的示例代码。
 * 
 */

