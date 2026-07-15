/**
 * @file lcd_ctrl.h
 * @brief LCD控制器配置文件
 * @details 定义LCD控制器的控制函数接口。
 * @ingroup LCD_CTRL
 */

#ifndef _LCD_CTRL_H_
#define _LCD_CTRL_H_

/**
 * @defgroup LCD_CTRL LCD控制器控制
 * @ingroup LCD_API
 * @brief LCD控制器控制模块
 * @details 
 * 定义LCD控制器的控制函数接口。
 * 
 * 主要职责包括：
 * - 配置LCD控制器的时钟参数
 * - 初始化LCD中断系统
 * - 提供控制器级别的硬件控制接口
 * 
 * 本模块为LCD显示系统提供底层控制器支持。
 * @{
 */


/**
 * @brief 初始化LCD控制器
 * @details 配置LCD控制器的时钟参数
 */
void Lcd_CTRL_Init(void);


/** @} */

#endif /* _LCD_CTRL_H_ */
