/**
 * @file dac_tim1.c
 * @brief DAC定时器1实现文件
 * @details 实现DAC定时器1的初始化和控制。
 * @ingroup DAC_TIM1
 */

#include "dac_tim1.h"

#include "soc_C6748.h"
#include "timer.h"


/* 函数定义 */
// 定时器1初始化函数
// freq 定时器频率
void Dac_Timer1_Init(unsigned int freq)
{
    // 配置定时器1为64位模式
    TimerConfigure(SOC_TMR_1_REGS, TMR_CFG_32BIT_UNCH_CLK_BOTH_INT);

    // 设置定时器周期
    TimerPeriodSet(SOC_TMR_1_REGS, TMR_TIMER12, ((1 * 24 * 1000 * 1000) / freq));

//    // 使能定时器1
//    TimerEnable(SOC_TMR_1_REGS, TMR_TIMER12, TMR_ENABLE_CONT);

    // 使能定时器/计数器中断
    TimerIntEnable(SOC_TMR_1_REGS, TMR_INT_TMR12_NON_CAPT_MODE);
}

// 启动定时器1
void Dac_Timer1_Start(void)
{
    // 使能定时器1
    TimerEnable(SOC_TMR_1_REGS, TMR_TIMER12, TMR_ENABLE_CONT);
}

// 停止定时器1
void Dac_Timer1_Stop(void)
{
    // 禁用定时器1
    TimerDisable(SOC_TMR_1_REGS, TMR_TIMER12);
}
