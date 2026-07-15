/**
 * @file lcd_ctrl.c
 * @brief LCD控制器配置实现文件
 * @details 实现LCD控制器的初始化功能，包括时钟配置。
 * @ingroup LCD_CTRL
 */

/* 头文件包含 */
#include "lcd_ctrl.h"            // LCD控制器控制

#include "soc_C6748.h"           // 定义芯片所有外设寄存器的物理基地址和宏
#include "raster.h"              // 提供光栅控制器相关的API函数


/* 函数定义 */
// 初始化LCD控制器
void Lcd_CTRL_Init(void) 
{
    // 配置LCD控制器时钟，设置像素时钟为30MHz，模块时钟为228MHz
    RasterClkConfig(SOC_LCDC_0_REGS, 30000000, 228000000);
}
