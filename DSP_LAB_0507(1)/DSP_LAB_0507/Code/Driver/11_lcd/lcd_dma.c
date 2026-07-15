/**
 * @file lcd_dma.c
 * @brief LCD DMA配置实现文件
 * @details 实现LCD DMA的初始化和单缓冲配置。
 * @ingroup LCD_DMA
 */

/* 头文件包含 */
#include "lcd_dma.h"            // LCD DMA控制
#include "lcd_grlib.h"          // LCD图形库

#include "interrupt.h"           // 提供中断控制器相关API
#include "soc_C6748.h"           // 定义芯片所有外设寄存器的物理基地址和宏
#include "raster.h"              // 提供光栅控制器相关的API函数
#include "grlib.h"               // 提供图形库相关函数

// 声明外部显示对象，用于在ISR中更新缓冲区指针
extern tDisplay Lcd_Display;


// 定义显存缓冲区数组，大小根据分辨率和色深计算（16BPP）
#pragma DATA_SECTION(Lcd_Buffer, "offscreen_buffer")
#pragma DATA_ALIGN(Lcd_Buffer, 4);
unsigned char Lcd_Buffer[GrOffScreen16BPPSize(LCD_WIDTH, LCD_HEIGHT)];

// 调色板数据（用于8位色深模式）
unsigned short palette_32b[PALETTE_SIZE/2] =
            {0x4000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u,
             0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u};


// 初始化LCD DMA
void Lcd_DMA_Init(void) 
{
    // 配置DMA控制寄存器：单缓冲、16字节突发、FIFO阈值8、禁用大端
    RasterDMAConfig(SOC_LCDC_0_REGS, RASTER_SINGLE_FRAME_BUFFER,
                    RASTER_BURST_SIZE_16, RASTER_FIFO_THRESHOLD_8,
                    RASTER_BIG_ENDIAN_DISABLE);
                    
    // 使能帧结束中断
    RasterEndOfFrameIntEnable(SOC_LCDC_0_REGS);

    // 配置帧缓冲器0地址
    RasterDMAFBConfig(SOC_LCDC_0_REGS,
                    (unsigned int)(Lcd_Buffer + PALETTE_OFFSET),
                    (unsigned int)(Lcd_Buffer + PALETTE_OFFSET) + sizeof(Lcd_Buffer) - 2 - PALETTE_OFFSET,
                    LCD_FRAME_0);

    // 拷贝调色板到两个缓冲区
    unsigned int i = 0;
    unsigned char *dest;
    unsigned char *src;
    
    src = (unsigned char *)palette_32b;
    dest = (unsigned char *)(Lcd_Buffer + PALETTE_OFFSET);
    for(i = PALETTE_OFFSET; i < (PALETTE_SIZE + PALETTE_OFFSET); i++)
    {
        *dest++ = *src++;
    }

}
