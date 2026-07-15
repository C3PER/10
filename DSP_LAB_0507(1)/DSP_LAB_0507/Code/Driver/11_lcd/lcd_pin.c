/**
 * @file lcd_pin.c
 * @brief LCD引脚配置实现文件
 * @details 实现LCD引脚的底层控制功能，包括初始化和配置。
 * @ingroup LCD_PIN
 */

/* 头文件包含 */
#include "lcd_pin.h"            // LCD引脚控制

#include "soc_C6748.h"          // 定义芯片所有外设寄存器的物理基地址和宏
#include "hw_types.h"           // 定义通用数据类型和寄存器读写宏
#include "hw_syscfg0_C6748.h"   // 定义芯片系统配置模块硬件寄存器及字段
#include "psc.h"                // 提供电源和睡眠控制器的API函数


/* 函数声明 */
// 初始化电源和睡眠控制器
static void PSCInit(void);
// 配置引脚复用
static void GPIOBankPinMuxSet(void);


/* 函数定义 */
// 初始化LCD引脚
void Lcd_Pin_Init(void) 
{
    // 初始化PSC，使能LCDC模块
    PSCInit();
    // 配置引脚复用，设置为LCDC功能
    GPIOBankPinMuxSet();
}

// 初始化电源和睡眠控制器
static void PSCInit(void) 
{
    // 使能LCDC模块的电源
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_LCDC, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);
}

// 配置引脚复用
static void GPIOBankPinMuxSet(void) 
{
    // 设置LCD相关引脚为LCDC功能
    volatile unsigned int savePinMux = 0;
    
    /* PINMUX16 */
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(16)) & \
                ~(LCD_DATA2_MASK | LCD_DATA3_MASK | LCD_DATA4_MASK \
                | LCD_DATA5_MASK | LCD_DATA6_MASK | LCD_DATA7_MASK);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(16)) = \
                (LCD_DATA2_ENABLE | LCD_DATA3_ENABLE | LCD_DATA4_ENABLE \
                |LCD_DATA5_ENABLE | LCD_DATA6_ENABLE | LCD_DATA7_ENABLE | savePinMux);

    /* PINMUX17 */
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(17)) & \
                ~(LCD_DATA0_MASK  | LCD_DATA1_MASK  | LCD_DATA10_MASK | LCD_DATA11_MASK \
                | LCD_DATA12_MASK | LCD_DATA13_MASK | LCD_DATA14_MASK | LCD_DATA15_MASK);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(17)) = \
                (LCD_DATA0_ENABLE  | LCD_DATA1_ENABLE  | LCD_DATA10_ENABLE | LCD_DATA11_ENABLE \
                |LCD_DATA12_ENABLE | LCD_DATA13_ENABLE | LCD_DATA14_ENABLE | LCD_DATA15_ENABLE | savePinMux);

    /* PINMUX18 */
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(18)) & \
                ~(LCD_DATA8_MASK | LCD_DATA9_MASK | LCD_PCLK_MASK);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(18)) = \
                (LCD_DATA8_ENABLE | LCD_DATA9_ENABLE | LCD_PCLK_ENABLE | savePinMux);

    /* PINMUX19 */
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(19)) & \
                ~(LCD_HSYNC_MASK | LCD_VSYNC_MASK | LCD_CS_MASK);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(19)) = \
                (LCD_HSYNC_ENABLE | LCD_VSYNC_ENABLE | LCD_CS_ENABLE | savePinMux);
}
