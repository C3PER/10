/**
 * @file lcd_drv.c
 * @brief LCD设备驱动实现文件
 * @details 实现LCD驱动的核心功能，包括初始化LCD显示系统。
 * @ingroup LCD_DRV
 */

/* 头文件包含 */
#include "lcd_api.h"            // LCD用户接口
#include "lcd_drv.h"            // LCD设备驱动
#include "lcd_pin.h"            // LCD引脚控制
#include "lcd_ctrl.h"           // LCD控制器控制
#include "lcd_raster.h"         // LCD光栅控制
#include "lcd_dma.h"            // LCD DMA控制
#include "lcd_grlib.h"          // LCD图形库
#include "lcd_widget.h"         // LCD控件


/* 函数定义 */
// 初始化LCD驱动
void Lcd_Init(void) 
{
    Lcd_Pin_Init();
    Lcd_CTRL_Init();
    Lcd_Raster_Init();
    Lcd_DMA_Init();
    Lcd_Raster_Enable();
    Lcd_Grlib_Init();
    // Lcd_Widget_Init();
}
