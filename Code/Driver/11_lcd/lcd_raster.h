/**
 * @file lcd_raster.h
 * @brief LCD光栅配置文件
 * @details 定义LCD光栅控制的函数接口。
 * @ingroup LCD_RASTER
 */

#ifndef _LCD_RASTER_H_
#define _LCD_RASTER_H_

/**
 * @defgroup LCD_RASTER LCD光栅控制
 * @ingroup LCD_API
 * @brief LCD光栅控制模块
 * @details 
 * 定义LCD光栅控制的函数接口。
 * 
 * 主要职责包括：
 * - 配置LCD光栅显示模式和参数
 * - 设置显示分辨率和时序
 * - 提供光栅使能控制
 * 
 * 本模块负责LCD显示的核心光栅控制功能。
 * @{
 */


/**
 * @brief 初始化LCD光栅
 * @details 配置LCD光栅显示参数，包括模式、时序等
 */
void Lcd_Raster_Init(void);

/**
 * @brief 使能LCD光栅
 * @details 使能LCDC控制器，启动显示输出
 */
void Lcd_Raster_Enable(void);

/** @} */

#endif /* _LCD_RASTER_H_ */
