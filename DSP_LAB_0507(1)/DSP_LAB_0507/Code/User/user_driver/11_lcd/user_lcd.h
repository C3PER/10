/**
 * @file user_lcd.h
 * @brief LCD用户使用头文件
 * @details 定义LCD用户使用接口，为用户提供LCD使用示例。
 * @ingroup LCD_EXAMPLE
 */

#ifndef _USER_LCD_H_
#define _USER_LCD_H_

/**
 * @defgroup LCD_EXAMPLE LCD应用示例
 * @ingroup LCD
 * @brief LCD应用示例模块
 * @details 
 * 定义LCD用户使用接口，为用户提供LCD使用示例。
 * 
 * 主要包含以下内容：
 * - LCD使用示例函数：展示如何初始化和使用LCD
 * 
 * 用户通过此模块可以学习如何在实际应用中使用LCD功能。
 * @{
 */

/**
 * @brief LCD示例函数
 * @details 演示LCD的基本操作流程，包括初始化、图形和字符显示。
 */
void Lcd_Example1(void);

/**
 * @brief LCD示例函数
 * @details 演示LCD的控件使用流程，包括初始化、控件按钮显示。
 */
void Lcd_Example2(void);




/** @} */

#endif /* _USER_LCD_H_ */
