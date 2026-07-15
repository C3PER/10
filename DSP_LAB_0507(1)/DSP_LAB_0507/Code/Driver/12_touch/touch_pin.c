/**
 * @file touch_pin.c
 * @brief Touch引脚配置实现文件
 * @details 实现Touch引脚的初始化和中断配置，为上层驱动提供硬件支持。
 * @ingroup TOUCH_PIN
 */

/* 头文件包含 */
#include "touch_pin.h"              // Touch引脚控制

#include "soc_C6748.h"              // 定义芯片所有外设寄存器的物理基地址和宏
#include "hw_types.h"               // 定义通用数据类型和寄存器读写宏
#include "hw_syscfg0_C6748.h"       // 定义芯片系统配置模块硬件寄存器及字段
#include "interrupt.h"              // 提供中断控制的API函数
#include "psc.h"                    // 提供电源和睡眠控制器的API函数
#include "gpio.h"                   // 提供通用输入输出端口控制的API函数


unsigned char FLAG_TOUCH;

/* 函数声明 */
static void PSCInit(void);
static void GPIOBankPinMuxSet(void);
static void GPIOBankPinInit_Out(void);
static void GPIOBankPinInit_In(void);
static void GPIOBankPinInterruptInit(void);
static void TouchIsr(void);


/* 函数定义 */
// 触摸中断引脚需要先输出
void Touch_Pin_Init_Out(void)
{
    PSCInit();
    GPIOBankPinMuxSet();
    GPIOBankPinInit_Out();
}

// 触摸中断引脚需要先输出再输入
void Touch_Pin_Init_In(void)
{
    GPIOBankPinInit_In();
    GPIOBankPinInterruptInit();
}

// PSC 初始化
static void PSCInit(void)
{
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_GPIO, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);
}

// GPIO 管脚复用配置
static void GPIOBankPinMuxSet(void)
{
    volatile unsigned int savePinMux = 0;

    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(0)) & ~(TOUCH_INT_MASK);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(0)) = (TOUCH_INT_ENABLE | savePinMux);
}

// GPIO 管脚初始化（输出模式）
static void GPIOBankPinInit_Out(void)
{
    GPIODirModeSet(SOC_GPIO_0_REGS, TOUCH_INT_PIN, GPIO_DIR_OUTPUT);
}

// GPIO 管脚初始化（输入模式）
static void GPIOBankPinInit_In(void)
{
    GPIODirModeSet(SOC_GPIO_0_REGS, TOUCH_INT_PIN, GPIO_DIR_INPUT);
}

// 设置触摸中断引脚状态
void Touch_Int_PinSet(unsigned int bitState)
{
    if(bitState == 1)
        GPIOPinWrite(SOC_GPIO_0_REGS, TOUCH_INT_PIN, GPIO_PIN_HIGH); // 清除
    else
        GPIOPinWrite(SOC_GPIO_0_REGS, TOUCH_INT_PIN, GPIO_PIN_LOW);  // 不清除
}


// GPIO 管脚中断初始化
static void GPIOBankPinInterruptInit(void)
{
    // 配置触摸中断为下降沿触发
    GPIOIntTypeSet(SOC_GPIO_0_REGS, TOUCH_INT_PIN, GPIO_INT_TYPE_FALLEDGE);
    // 使能 GPIO BANK 中断
    GPIOBankIntEnable(SOC_GPIO_0_REGS, 8);
    // 注册中断服务函数
    IntRegister(C674X_MASK_INT13, TouchIsr);
    // 映射中断到 DSP 可屏蔽中断
    IntEventMap(C674X_MASK_INT13, SYS_INT_GPIO_B8INT);
    // 使能 DSP 可屏蔽中断
    IntEnable(C674X_MASK_INT13);
}

unsigned char Flag_Touch_Done;

#include "touch_api.h"

// 触摸中断处理函数
void TouchHandler(void)
{
    FLAG_TOUCH = 1;
}

// 触摸中断服务函数
static void TouchIsr(void)
{
    // 禁用 GPIO BANK 中断
    GPIOBankIntDisable(SOC_GPIO_0_REGS, 8);

    // 清除中断状态
    IntEventClear(SYS_INT_GPIO_B8INT);

    if(GPIOPinIntStatus(SOC_GPIO_0_REGS, TOUCH_INT_PIN) == GPIO_INT_PEND)
    {
        // 清除 GPIO 中断状态
        GPIOPinIntClear(SOC_GPIO_0_REGS, TOUCH_INT_PIN);

        TouchHandler();
    }

    // 使能 GPIO BANK 中断
    GPIOBankIntEnable(SOC_GPIO_0_REGS, 8);
}
