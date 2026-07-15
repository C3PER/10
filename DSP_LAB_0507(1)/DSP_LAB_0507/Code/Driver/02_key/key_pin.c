/**
 * @file key_pin.c
 * @brief 按键引脚配置实现文件
 * @details 实现按键引脚的初始化和中断配置，为上层驱动提供硬件支持。
 * @ingroup KEY_PIN
 */

/* 头文件包含 */
#include "key_pin.h"

#include "soc_C6748.h"          // 定义芯片所有外设寄存器的物理基地址和宏
#include "hw_types.h"           // 定义通用数据类型和寄存器读写宏
#include "hw_syscfg0_C6748.h"   // 定义芯片系统配置模块硬件寄存器及字段
#include "psc.h"                // 提供电源和睡眠控制器的API函数
#include "gpio.h"               // 提供通用输入输出端口控制的API函数
#include "interrupt.h"          // 提供中断控制的API函数


/* 函数声明 */
// 初始化电源和睡眠控制器
static void PSCInit(void);
// 配置引脚复用
static void GPIOBankPinMuxSet(void);
// 初始化GPIO引脚
static void GPIOBankPinInit(void);
// 配置中断
static void GPIOBankPinInterruptInit(void);

/* 函数定义 */
// 初始化按键引脚
void KEY_PinInit(void)
{
    // 初始化PSC，使能GPIO模块
    PSCInit();
    // 配置引脚复用，设置为GPIO功能
    GPIOBankPinMuxSet();
    // 初始化GPIO引脚，设置为输入模式
    GPIOBankPinInit();
    // 配置按键中断
    GPIOBankPinInterruptInit();
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
    // 设置按键相关引脚为GPIO功能
    volatile unsigned int savePinMux = 0;

    // 配置 PINMUX(1) 寄存器
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) & \
                            ~(KEY1_MASK | KEY2_MASK | KEY3_MASK | KEY4_MASK | KEY5_MASK);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) = \
                            (KEY1_ENABLE | KEY2_ENABLE | KEY3_ENABLE | KEY4_ENABLE | KEY5_ENABLE | savePinMux);
}

// 初始化GPIO引脚
static void GPIOBankPinInit(void)
{
    // 初始化GPIO引脚，设置为输入模式
    GPIODirModeSet(SOC_GPIO_0_REGS, KEY1_PIN, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_0_REGS, KEY2_PIN, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_0_REGS, KEY3_PIN, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_0_REGS, KEY4_PIN, GPIO_DIR_INPUT);
    GPIODirModeSet(SOC_GPIO_0_REGS, KEY5_PIN, GPIO_DIR_INPUT);
}

// 配置中断
static void GPIOBankPinInterruptInit(void)
{
    // 配置KEY为下降沿触发
    GPIOIntTypeSet(SOC_GPIO_0_REGS, KEY1_PIN, GPIO_INT_TYPE_FALLEDGE);
    GPIOIntTypeSet(SOC_GPIO_0_REGS, KEY2_PIN, GPIO_INT_TYPE_FALLEDGE);
    GPIOIntTypeSet(SOC_GPIO_0_REGS, KEY3_PIN, GPIO_INT_TYPE_FALLEDGE);
    GPIOIntTypeSet(SOC_GPIO_0_REGS, KEY4_PIN, GPIO_INT_TYPE_FALLEDGE);
    GPIOIntTypeSet(SOC_GPIO_0_REGS, KEY5_PIN, GPIO_INT_TYPE_FALLEDGE);
    // 使能 GPIO BANK 中断
    GPIOBankIntEnable(SOC_GPIO_0_REGS, 0);
}
