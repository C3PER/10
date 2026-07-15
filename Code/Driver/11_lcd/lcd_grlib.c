/**
 * @file lcd_grlib.c
 * @brief LCD图形库配置实现文件
 * @details 实现LCD图形库的初始化功能。
 * @ingroup LCD_GRLIB
 */

/* 头文件包含 */
#include "lcd_grlib.h"           // LCD图形库

#include "lcd_dma.h"             // 提供显示缓冲区定义

#include "grlib.h"               // 提供图形库核心API


// 声明图形库中定义的tDisplay显示设备对象，用于抽象屏幕操作
tDisplay Lcd_Display;

// 全局显示上下文，保存当前的绘图状态（如字体、前景色、背景色等）
tContext Lcd_Context;

tRectangle Lcd_Rectangle;




/* 函数定义 */
// 初始化LCD图形库
void Lcd_Grlib_Init(void) 
{
    // 初始化离屏显存
    GrOffScreen16BPPInit(&Lcd_Display, Lcd_Buffer, LCD_WIDTH, LCD_HEIGHT);
    
    // 初始化显存上下文.
    GrContextInit(&Lcd_Context, &Lcd_Display);
}
