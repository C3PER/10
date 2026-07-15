/**
 * @file adc_tim1.c
 * @brief ADC定时器实现文件
 * @details 实现ADC的定时器相关功能。
 * @ingroup ADC_TIM1
 */

/* 头文件包含 */
#include "adc_tim1.h"
#include "soc_C6748.h"
#include "timer.h"


// 初始化定时器 1
// 定时器频率（Hz）
void Adc_Timer1_Init(unsigned int freq)
{
    // 配置定时器/计数器1为32位模式
    TimerConfigure(SOC_TMR_1_REGS, TMR_CFG_32BIT_UNCH_CLK_BOTH_INT);
    
    // 设置周期
    TimerPeriodSet(SOC_TMR_1_REGS, TMR_TIMER34, ((1 * 24 * 1000 * 1000) / freq));
    
//    // 使能定时器/计数器1
//    TimerEnable(SOC_TMR_1_REGS, TMR_TIMER34, TMR_ENABLE_CONT);
    
    // 使能定时器/计数器中断
    TimerIntEnable(SOC_TMR_1_REGS, TMR_INT_TMR34_NON_CAPT_MODE);
}

// 启动定时器 1
void Adc_Timer1_Start(void)
{
    // 使能定时器/计数器1
    TimerEnable(SOC_TMR_1_REGS, TMR_TIMER34, TMR_ENABLE_CONT);
}

// 停止定时器 1
void Adc_Timer1_Stop(void)
{
    // 禁用定时器/计数器1
    TimerDisable(SOC_TMR_1_REGS, TMR_TIMER34);
}
