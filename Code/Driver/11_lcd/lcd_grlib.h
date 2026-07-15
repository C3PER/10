/**
 * @file lcd_grlib.h
 * @brief LCD图形库配置文件
 * @details 定义LCD图形库的接口和宏定义。
 * @ingroup LCD_GRLIB
 */

#ifndef _LCD_GRLIB_H_
#define _LCD_GRLIB_H_

/**
 * @defgroup LCD_GRLIB LCD图形库
 * @ingroup LCD_API
 * @brief LCD图形库模块
 * @details 
 * 定义LCD图形库的接口和宏定义。
 * 
 * 主要职责包括：
 * - 提供显示分辨率定义
 * - 初始化grlib图形库
 * - 创建显示设备对象和绘图上下文
 * 
 * 本模块为UI控件提供图形绘制能力。
 * @{
 */


/**
 * @brief 初始化LCD图形库
 * @details 配置LCD图形库，初始化显示设备对象
 */
void Lcd_Grlib_Init(void);

/** @} */

#endif /* _LCD_GRLIB_H_ */
