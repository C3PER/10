/**
 * @file adc_pin.c
 * @brief ADC引脚配置实现文件
 * @details 初始化ADC的引脚，包括使能PSC模块、配置GPIO管脚复用和方向。
 * @ingroup ADC_PIN
 */

/* 头文件包含 */
#include "adc_api.h"
#include "adc_pin.h"

#include "soc_C6748.h"
#include "hw_types.h"
#include "hw_syscfg0_C6748.h"
#include "interrupt.h"
#include "psc.h"
#include "gpio.h"

/* 函数声明 */
// PSC 初始化
static void PSCInit(void);
// GPIO 管脚复用配置
static void GPIOBankPinMuxSet(void);
// GPIO 管脚方向配置
static void GPIOBankPinInit(void);
// GPIO 管脚中断配置
static void GPIOBankPinInterruptInit(void);


// 初始化ADC部分引脚
void Adc_Pin_Init(void)
{
    // 外设使能配置
    PSCInit();
    // GPIO 管脚复用配置
    GPIOBankPinMuxSet();
    // GPIO 管脚方向配置
    GPIOBankPinInit();
    // GPIO 管脚中断配置
    GPIOBankPinInterruptInit();
}

// 初始化PSC模块
static void PSCInit(void)
{
    // 使能 GPIO 模块
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_GPIO, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);
}

// GPIO 管脚复用配置
static void GPIOBankPinMuxSet(void)
{
    volatile unsigned int savePinMux = 0;
    // 配置 PINMUX11
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(11)) & \
                ~(ADC_BUSY_MASK | ADC_RESET_MASK | ADC_CONVST_MASK);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(11)) = \
                (ADC_BUSY_ENABLE | ADC_RESET_ENABLE | ADC_CONVST_ENABLE | savePinMux);
}

// GPIO 管脚方向配置
static void GPIOBankPinInit(void)
{
    // 设置 GPIO5[11] 为输入模式
    GPIODirModeSet(SOC_GPIO_0_REGS, ADC_BUSY_PIN, GPIO_DIR_INPUT);

    // 设置 GPIO5[12] 为输出模式
    GPIODirModeSet(SOC_GPIO_0_REGS, ADC_RESET_PIN, GPIO_DIR_OUTPUT);
    GPIOPinWrite(SOC_GPIO_0_REGS, ADC_RESET_PIN, GPIO_PIN_HIGH);

    // 设置 GPIO5[13] 为输出模式
    GPIODirModeSet(SOC_GPIO_0_REGS, ADC_CONVST_PIN, GPIO_DIR_OUTPUT);
    GPIOPinWrite(SOC_GPIO_0_REGS, ADC_CONVST_PIN, GPIO_PIN_LOW);
}

// GPIO 管脚中断配置
// 因为EDMA来获取GPIO中断，不用配置CPU中断
static void GPIOBankPinInterruptInit(void)
{
    // 设置GPIO5[11]为下降沿触发中断模式
    GPIOIntTypeSet(SOC_GPIO_0_REGS, ADC_BUSY_PIN, GPIO_INT_TYPE_FALLEDGE);

    // 设置允许 GPIO5[15：0] 产生中断
    GPIOBankIntEnable(SOC_GPIO_0_REGS, 5);
}

// 设置ADC复位引脚状态
void Adc_Reset_PinSet(unsigned int bitState)
{
    if(bitState == 1)
        GPIOPinWrite(SOC_GPIO_0_REGS, ADC_RESET_PIN, GPIO_PIN_HIGH); // 复位
    else
        GPIOPinWrite(SOC_GPIO_0_REGS, ADC_RESET_PIN, GPIO_PIN_LOW);  // 正常
}

// 设置ADC转换开始引脚状态
void Adc_Convst_PinSet(unsigned int bitState)
{
    if(bitState == 1)
        GPIOPinWrite(SOC_GPIO_0_REGS, ADC_CONVST_PIN, GPIO_PIN_HIGH); // 开始转换
    else
        GPIOPinWrite(SOC_GPIO_0_REGS, ADC_CONVST_PIN, GPIO_PIN_LOW);  // 停止转换
}
