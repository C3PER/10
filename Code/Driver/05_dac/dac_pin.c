/**
 * @file dac_pin.c
 * @brief DAC引脚配置实现文件
 * @details 实现DAC引脚的初始化和控制。
 * @ingroup DAC_PIN
 */

#include "dac_pin.h"

#include "soc_C6748.h"
#include "hw_types.h"
#include "hw_syscfg0_C6748.h"
#include "psc.h"
#include "gpio.h"


/* 函数声明 */
static void PSCInit(void);
static void GPIOBankPinMuxSet(void);
static void GPIOBankPinInit(void);


/* 函数定义 */
// DAC引脚初始化
void Dac_Pin_Init(void)
{
    // 外设使能配置
    PSCInit();
    // GPIO 管脚复用配置
    GPIOBankPinMuxSet();
    // GPIO 管脚方向配置
    GPIOBankPinInit();
}

// 电源和睡眠控制器PSC，开启GPIO功能
static void PSCInit(void)
{
    // 使能 GPIO 模块
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_GPIO, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);
}

// 引脚复用控制器PINMUX，配置引脚为GPIO功能
static void GPIOBankPinMuxSet(void)
{
    volatile unsigned int savePinMux = 0;
    
    /* PINMUX2 */
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(2)) & 
                ~(DAC_CLR_MASK);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(2)) = 
                (DAC_CLR_ENABLE | savePinMux);
    
    /* PINMUX5 */
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) & 
                ~(DAC_LOAD_MASK);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) = 
                (DAC_LOAD_ENABLE | savePinMux);
}

// GPIO外设，初始化引脚为输出模式
static void GPIOBankPinInit(void)
{
    GPIODirModeSet(SOC_GPIO_0_REGS, DAC_LOAD_PIN, GPIO_DIR_OUTPUT);
    GPIODirModeSet(SOC_GPIO_0_REGS, DAC_CLR_PIN, GPIO_DIR_OUTPUT);
    GPIOPinWrite(SOC_GPIO_0_REGS, DAC_LOAD_PIN, GPIO_PIN_LOW);
    GPIOPinWrite(SOC_GPIO_0_REGS, DAC_CLR_PIN, GPIO_PIN_HIGH);
}

// 设置DAC加载引脚状态
// bitState 状态(1: 加载, 0: 不加载)
void Dac_Load_PinSet(unsigned int bitState)
{
    if(bitState == 1)
        GPIOPinWrite(SOC_GPIO_0_REGS, DAC_LOAD_PIN, GPIO_PIN_HIGH); // 加载
    else
        GPIOPinWrite(SOC_GPIO_0_REGS, DAC_LOAD_PIN, GPIO_PIN_LOW);  // 不加载
}

// 设置DAC清除引脚状态
// bitState 状态(1: 清除, 0: 不清除)
void Dac_Clr_PinSet(unsigned int bitState)
{
    if(bitState == 1)
        GPIOPinWrite(SOC_GPIO_0_REGS, DAC_CLR_PIN, GPIO_PIN_HIGH); // 清除
    else
        GPIOPinWrite(SOC_GPIO_0_REGS, DAC_CLR_PIN, GPIO_PIN_LOW);  // 不清除
}
