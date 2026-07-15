/**
 * @file lcd_raster.c
 * @brief LCD光栅配置实现文件
 * @details 实现LCD光栅显示的初始化和使能功能。
 * @ingroup LCD_RASTER
 */

/* 头文件包含 */
#include "lcd_raster.h"           // LCD光栅控制

#include "soc_C6748.h"           // 定义芯片所有外设寄存器的物理基地址和宏
#include "raster.h"              // 提供光栅控制器相关的API函数


/* 函数定义 */
// 初始化LCD光栅
void Lcd_Raster_Init(void) 
{
    // 配置前先禁用LCDC控制器
    RasterDisable(SOC_LCDC_0_REGS);

    // 配置光栅控制寄存器：TFT模式、调色板数据、彩色、右对齐
    RasterModeConfig(SOC_LCDC_0_REGS, RASTER_DISPLAY_MODE_TFT,
                    RASTER_PALETTE_DATA, RASTER_COLOR, RASTER_RIGHT_ALIGNED);

    // 配置数据顺序为LSB小端格式
    RasterLSBDataOrderSelect(SOC_LCDC_0_REGS);

    // 禁用半字节模式，使用数据对齐模式
    RasterNibbleModeDisable(SOC_LCDC_0_REGS);

    // 配置FIFO DMA请求延时为2个时钟周期
    RasterFIFODMADelayConfig(SOC_LCDC_0_REGS, 2);

    // 配置时序参数2
    RasterTiming2Configure(SOC_LCDC_0_REGS, 
                            RASTER_FRAME_CLOCK_LOW |
                            RASTER_LINE_CLOCK_LOW  |
                            RASTER_PIXEL_CLOCK_HIGH |
                            RASTER_SYNC_EDGE_RISING |
                            RASTER_SYNC_CTRL_ACTIVE |
                            RASTER_AC_BIAS_HIGH, 
                            0, 255);

    // 配置水平定时参数：800x480分辨率
    // RasterHparamConfig(SOC_LCDC_0_REGS, 800, 30, 210, 16);
    // RasterVparamConfig(SOC_LCDC_0_REGS, 480, 13, 22, 10);
    RasterHparamConfig(SOC_LCDC_0_REGS, 800, 48, 40, 88);
    RasterVparamConfig(SOC_LCDC_0_REGS, 480, 3, 13, 32);
}

// 使能LCD光栅
void Lcd_Raster_Enable(void) 
{
    // 使能LCDC控制器，启动显示
    RasterEnable(SOC_LCDC_0_REGS);
}
