/**
 * @file lcd_dma.h
 * @brief LCD DMA配置文件
 * @details 定义LCD DMA控制的函数接口和宏定义。
 * @ingroup LCD_DMA
 */

#ifndef _LCD_DMA_H_
#define _LCD_DMA_H_

/**
 * @defgroup LCD_DMA LCD DMA控制
 * @ingroup LCD_API
 * @brief LCD DMA控制模块
 * @details 
 * 定义LCD DMA控制的函数接口和宏定义。
 * 
 * 主要职责包括：
 * - 配置LCD DMA传输参数
 * - 管理单缓冲显示机制
 * - 提供显示缓冲区管理
 * 
 * 本模块实现高效的显示数据传输，使用单帧缓冲技术。
 * @{
 */

/// @cond INTERNAL_MACROS
#define LCD_WIDTH       800
#define LCD_HEIGHT      480

#define PALETTE_SIZE    32
#define PALETTE_OFFSET  4

#define LCD_FRAME_0  0
#define LCD_FRAME_1  1

/// @endcond /* INTERNAL_MACROS 结束 */


extern unsigned char Lcd_Buffer[];


/**
 * @brief LCD DMA初始化函数
 * @details 配置LCD DMA寄存器参数，包括单帧缓冲器、突发传输等
 */
void Lcd_DMA_Init(void);

/** @} */

#endif /* _LCD_DMA_H_ */
