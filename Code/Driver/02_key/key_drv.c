/**
 * @file key_drv.c
 * @brief 按键设备驱动实现文件
 * @details 实现按键驱动的核心功能，包括初始化和中断处理。
 * @ingroup KEY_DRV
 */

/* 头文件包含 */
#include "key_api.h"            // 按键用户接口
#include "key_drv.h"            // 按键设备驱动
#include "key_pin.h"            // 按键引脚控制

#include "soc_C6748.h"          // 定义芯片所有外设寄存器的物理基地址和宏
#include "interrupt.h"          // 提供中断控制的API函数
#include "gpio.h"               // 提供通用输入输出端口控制的API函数


/* 变量声明 */
// 按键控制标志位
volatile unsigned char FLAG_KEY1 = 0;
volatile unsigned char FLAG_KEY2 = 0;
volatile unsigned char FLAG_KEY3 = 0;
volatile unsigned char FLAG_KEY4 = 0;
volatile unsigned char FLAG_KEY5 = 0;

/* 函数声明 */
static void Key_Isr(void);        // 按键中断服务函数
static void KEY1_Handler(void);   // KEY1按键处理函数
static void KEY2_Handler(void);   // KEY2按键处理函数
static void KEY3_Handler(void);   // KEY3按键处理函数
static void KEY4_Handler(void);   // KEY4按键处理函数
static void KEY5_Handler(void);   // KEY5按键处理函数

/* 函数定义 */
// 初始化按键驱动
void Key_Init(void)
{
    // 初始化按键引脚
    KEY_PinInit();
    // 注册中断服务函数
    IntRegister(C674X_MASK_INT15, Key_Isr);
    // 映射中断到 DSP 可屏蔽中断
    IntEventMap(C674X_MASK_INT15, SYS_INT_GPIO_B0INT);
    // 使能 DSP 可屏蔽中断
    IntEnable(C674X_MASK_INT15);
}

// KEY1按键处理函数
static void KEY1_Handler(void)
{
    FLAG_KEY1 = 1;
}

// KEY2按键处理函数
static void KEY2_Handler(void)
{
    FLAG_KEY2 = 1;
}

// KEY3按键处理函数
static void KEY3_Handler(void)
{
    FLAG_KEY3 = 1;
}

// KEY4按键处理函数
static void KEY4_Handler(void)
{
    FLAG_KEY4 = 1;
}

// KEY5按键处理函数
static void KEY5_Handler(void)
{
    FLAG_KEY5 = 1;
}

// 按键中断服务函数
static void Key_Isr(void)
{
    GPIOBankIntDisable(SOC_GPIO_0_REGS, 0);
    IntEventClear(SYS_INT_GPIO_B0INT);
    
    if(GPIOPinIntStatus(SOC_GPIO_0_REGS, KEY1_PIN) == GPIO_INT_PEND)
    {
        GPIOPinIntClear(SOC_GPIO_0_REGS, KEY1_PIN);
        while(!GPIOPinRead(SOC_GPIO_0_REGS, KEY1_PIN));
        KEY1_Handler();
    }
    else if(GPIOPinIntStatus(SOC_GPIO_0_REGS, KEY2_PIN) == GPIO_INT_PEND)
    {
        GPIOPinIntClear(SOC_GPIO_0_REGS, KEY2_PIN);
        while(!GPIOPinRead(SOC_GPIO_0_REGS, KEY2_PIN));
        KEY2_Handler();
    }
    else if(GPIOPinIntStatus(SOC_GPIO_0_REGS, KEY3_PIN) == GPIO_INT_PEND)
    {
        GPIOPinIntClear(SOC_GPIO_0_REGS, KEY3_PIN);
        while(!GPIOPinRead(SOC_GPIO_0_REGS, KEY3_PIN));
        KEY3_Handler();
    }
    else if(GPIOPinIntStatus(SOC_GPIO_0_REGS, KEY4_PIN) == GPIO_INT_PEND)
    {
        GPIOPinIntClear(SOC_GPIO_0_REGS, KEY4_PIN);
        while(!GPIOPinRead(SOC_GPIO_0_REGS, KEY4_PIN));
        KEY4_Handler();
    }
    else if(GPIOPinIntStatus(SOC_GPIO_0_REGS, KEY5_PIN) == GPIO_INT_PEND)
    {
        GPIOPinIntClear(SOC_GPIO_0_REGS, KEY5_PIN);
        while(!GPIOPinRead(SOC_GPIO_0_REGS, KEY5_PIN));
        KEY5_Handler();
    }
    
    GPIOBankIntEnable(SOC_GPIO_0_REGS, 0);
}
