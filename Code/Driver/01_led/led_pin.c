/**
 * @file led_pin.c
 * @brief LED引脚配置实现文件
 * @details 实现LED引脚的底层控制功能，包括初始化和状态设置。
 * @ingroup LED_PIN
 */

/* 头文件包含 */
#include "led_pin.h"            // LED引脚控制

#include "soc_C6748.h"          // 定义芯片所有外设寄存器的物理基地址和宏
#include "hw_types.h"           // 定义通用数据类型和寄存器读写宏
#include "hw_syscfg0_C6748.h"   // 定义芯片系统配置模块硬件寄存器及字段
#include "psc.h"                // 提供电源和睡眠控制器的API函数
#include "gpio.h"               // 提供通用输入输出端口控制的API函数


/* 函数声明 */
// 初始化电源和睡眠控制器
static void PSCInit(void);
// 配置引脚复用
static void GPIOBankPinMuxSet(void);
// 初始化GPIO引脚
static void GPIOBankPinInit(void);


/* 函数定义 */
// 初始化LED引脚
void LED_PinInit(void) 
{
    // 初始化PSC，使能GPIO模块
    PSCInit();
    // 配置引脚复用，设置为GPIO功能
    GPIOBankPinMuxSet();
    // 初始化GPIO引脚，设置为输出模式
    GPIOBankPinInit();
}

// 初始化电源和睡眠控制器
static void PSCInit(void) 
{
    // 使能GPIO模块的电源
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_GPIO, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);
}

// 配置引脚复用
static void GPIOBankPinMuxSet(void) 
{
    // 设置LED相关引脚为GPIO功能
    volatile unsigned int savePinMux = 0;
    
    // 配置 PINMUX(0) 寄存器
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(0)) & ~(LED2_BLUE_MASK | LED2_RED_MASK | LED3_BLUE_MASK | LED3_RED_MASK | LED4_BLUE_MASK | LED4_RED_MASK);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(0)) = (LED2_BLUE_ENABLE | LED2_RED_ENABLE | LED3_BLUE_ENABLE | LED3_RED_ENABLE | LED4_BLUE_ENABLE | LED4_RED_ENABLE | savePinMux);

    // 配置 PINMUX(1) 寄存器
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) & ~(LED1_BLUE_MASK | LED1_RED_MASK);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) = (LED1_BLUE_ENABLE | LED1_RED_ENABLE | savePinMux);

    // 配置 PINMUX(13) 寄存器
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(13)) & ~(LED1_CORE_MASK | LED2_CORE_MASK);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(13)) = (LED1_CORE_ENABLE | LED2_CORE_ENABLE | savePinMux);
}

// 初始化GPIO引脚
static void GPIOBankPinInit(void) 
{
    // 初始化GPIO引脚，设置为输出模式
    // 核心板 LED
    GPIODirModeSet(SOC_GPIO_0_REGS, LED1_CORE_PIN, GPIO_DIR_OUTPUT);
    GPIODirModeSet(SOC_GPIO_0_REGS, LED2_CORE_PIN, GPIO_DIR_OUTPUT);

    // 开发板 LED 1-4
    GPIODirModeSet(SOC_GPIO_0_REGS, LED1_BLUE_PIN, GPIO_DIR_OUTPUT);
    GPIODirModeSet(SOC_GPIO_0_REGS, LED1_RED_PIN, GPIO_DIR_OUTPUT);
    GPIODirModeSet(SOC_GPIO_0_REGS, LED2_BLUE_PIN, GPIO_DIR_OUTPUT);
    GPIODirModeSet(SOC_GPIO_0_REGS, LED2_RED_PIN, GPIO_DIR_OUTPUT);
    GPIODirModeSet(SOC_GPIO_0_REGS, LED3_BLUE_PIN, GPIO_DIR_OUTPUT);
    GPIODirModeSet(SOC_GPIO_0_REGS, LED3_RED_PIN, GPIO_DIR_OUTPUT);
    GPIODirModeSet(SOC_GPIO_0_REGS, LED4_BLUE_PIN, GPIO_DIR_OUTPUT);
    GPIODirModeSet(SOC_GPIO_0_REGS, LED4_RED_PIN, GPIO_DIR_OUTPUT);
}

// 设置LED引脚状态
void LED_PinSet(unsigned int pinNumber, unsigned int bitState) 
{
    // 根据指定的状态控制LED引脚
    switch(bitState) 
    {
        case LED_STA_ON:     GPIOPinWrite(SOC_GPIO_0_REGS, pinNumber, GPIO_PIN_HIGH);              break;
        case LED_STA_OFF:    GPIOPinWrite(SOC_GPIO_0_REGS, pinNumber, GPIO_PIN_LOW);               break;
        case LED_STA_TOGGLE: GPIOPinWrite(SOC_GPIO_0_REGS, pinNumber, !GPIOPinRead(SOC_GPIO_0_REGS, pinNumber)); break;
        default: break;
    }
}
