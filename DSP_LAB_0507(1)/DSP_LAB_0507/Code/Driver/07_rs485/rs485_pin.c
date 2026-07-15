/**
 * @file rs485_pin.c
 * @brief RS485引脚配置实现文件
 * @details 实现RS485引脚的初始化，包括PSC初始化、GPIO引脚复用配置和GPIO引脚初始化。
 * @ingroup RS485_PIN
 */

#include "rs485_pin.h"

#include "soc_C6748.h"
#include "hw_types.h"
#include "hw_syscfg0_C6748.h"
#include "psc.h"
#include "gpio.h"



/* 函数声明 */
// PSC初始化
static void PSCInit(void);
// GPIO引脚复用配置
static void GPIOBankPinMuxSet(void);
// GPIO引脚初始化
static void GPIOBankPinInit(void);


/* 函数定义 */
// 初始化RS485引脚
void RS485_PinInit(void)
{
    PSCInit();
    GPIOBankPinMuxSet();
    GPIOBankPinInit();
}

// 电源和睡眠控制器PSC，开启GPIO功能
static void PSCInit(void)
{
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_GPIO, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);
}

// 引脚复用控制器PINMUX，配置引脚为GPIO功能
static void GPIOBankPinMuxSet(void)
{
    // RS485 Enable 管脚
    volatile unsigned int savePinMux = 0;
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(7)) & ~(RS485_RE_MASK);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(7)) = (RS485_RE_ENABLE | savePinMux);
}

// GPIO外设，初始化引脚为输出模式
static void GPIOBankPinInit(void)
{
    // 设置使能管脚为输出状态 GPIO0[11]
    GPIODirModeSet(SOC_GPIO_0_REGS, RS485_RE_PIN, GPIO_DIR_OUTPUT);
    RS485_EN_PinSet(0);
}

// 设置RS485使能引脚状态
void RS485_EN_PinSet(unsigned int bitState)
{
    if(bitState == 1)
        GPIOPinWrite(SOC_GPIO_0_REGS, RS485_RE_PIN, GPIO_PIN_HIGH); //发送
    else
        GPIOPinWrite(SOC_GPIO_0_REGS, RS485_RE_PIN, GPIO_PIN_LOW); //接收
}
