/**
 * @file adc_emif.c
 * @brief ADC EMIF接口配置实现文件
 * @details 实现ADC的EMIF接口配置。
 * @ingroup ADC_EMIF
 */

/* 头文件包含 */
#include "adc_emif.h"

#include "soc_C6748.h"
#include "hw_types.h"
#include "hw_syscfg0_C6748.h"
#include "psc.h"
#include "emifa.h"

/* 函数声明 */
// PSC 初始化
static void PSCInit(void);
// GPIO 管脚复用配置
static void GPIOBankPinMuxSet(void);
// EMIFA 初始化
static void Emif_Init(void);


// 初始化ADC EMIF接口
void Adc_Emif_Init(void)
{
    // 外设使能配置
    PSCInit();

    // GPIO 管脚复用配置
    GPIOBankPinMuxSet();

    // EMIFA 初始化
    Emif_Init();
}

// PSC 初始化
static void PSCInit(void)
{
    // 使能 EMIFA 模块
    PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_EMIFA, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);
}

// GPIO 管脚复用配置
static void GPIOBankPinMuxSet(void)
{
    volatile unsigned int savePinMux = 0;
    /*PINMUX7*/
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(7)) & ~(ADC_RDCS_MASK);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(7)) = (ADC_RDCS_ENABLE | savePinMux);
    /*PINMUX8*/
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(8)) & \
                ~(ADC_DATA8_MASK | ADC_DATA9_MASK  | ADC_DATA10_MASK | ADC_DATA11_MASK \
                |ADC_DATA12_MASK | ADC_DATA13_MASK | ADC_DATA14_MASK | ADC_DATA15_MASK);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(8)) = \
                (ADC_DATA8_ENABLE  | ADC_DATA9_ENABLE  | ADC_DATA10_ENABLE | ADC_DATA11_ENABLE \
                |ADC_DATA12_ENABLE | ADC_DATA13_ENABLE | ADC_DATA14_ENABLE | ADC_DATA15_ENABLE | savePinMux);
    /*PINMUX9*/
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(9)) & \
                ~(ADC_DATA0_MASK | ADC_DATA1_MASK | ADC_DATA2_MASK | ADC_DATA3_MASK \
                | ADC_DATA4_MASK | ADC_DATA5_MASK | ADC_DATA6_MASK | ADC_DATA7_MASK);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(9)) = \
                (ADC_DATA0_ENABLE | ADC_DATA1_ENABLE | ADC_DATA2_ENABLE | ADC_DATA3_ENABLE \
                |ADC_DATA4_ENABLE | ADC_DATA5_ENABLE | ADC_DATA6_ENABLE | ADC_DATA7_ENABLE | savePinMux);
}

// EMIFA 初始化
static void Emif_Init(void)
{
    // 配置数据总线 16bit
    EMIFAAsyncDevDataBusWidthSelect(SOC_EMIFA_0_REGS,EMIFA_CHIP_SELECT_2, EMIFA_DATA_BUSWITTH_16BIT);

    // 选择 Normal 模式
    // EMIFAAsyncDevOpModeSelect(SOC_EMIFA_0_REGS,EMIFA_CHIP_SELECT_2, EMIFA_ASYNC_INTERFACE_NORMAL_MODE);
    // 选通脉冲模式
    EMIFAAsyncDevOpModeSelect(SOC_EMIFA_0_REGS,EMIFA_CHIP_SELECT_2, EMIFA_ASYNC_INTERFACE_STROBE_MODE);

    // 禁止WAIT引脚
    EMIFAExtendedWaitConfig(SOC_EMIFA_0_REGS,EMIFA_CHIP_SELECT_2, EMIFA_EXTENDED_WAIT_DISABLE);

    // 配置 W_SETUP/R_SETUP W_STROBE/R_STROBE W_HOLD/R_HOLD    TA 等参数
    EMIFAWaitTimingConfig(SOC_EMIFA_0_REGS,EMIFA_CHIP_SELECT_2, EMIFA_ASYNC_WAITTIME_CONFIG(1, 2, 1, 1, 2, 1, 0 ));
}
