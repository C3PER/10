/**
 * @file timer_drv.c
 * @brief 定时器设备驱动实现文件
 * @details 实现定时器驱动的核心功能，包括初始化和中断处理。
 * @ingroup TIMER_DRV
 */

#include "timer_drv.h"
#include "soc_C6748.h"
#include "interrupt.h"
#include "timer.h"

/* 函数声明 */
// 初始化定时器2
static void Timer2Init(unsigned int freq);
// 初始化定时器2中断
static void Timer2InterruptInit(void);
// 定时器2中断处理函数
static void TIM2Handler(void);
// 定时器2中断服务函数
static void Timer2Isr(void);

/* 变量声明 */
// 定时器2中断标志位
volatile unsigned char FLAG_TIM2 = 0;

/* 函数定义 */
// 初始化定时器2
void Tim2_Init(unsigned int freq)
{
    // 初始化定时器
    Timer2Init(freq);
    
    // 初始化定时器中断
    Timer2InterruptInit();
}

// 初始化定时器2
static void Timer2Init(unsigned int freq)
{
    // 配置 定时器 / 计数器 2 为 64 位模式
    TimerConfigure(SOC_TMR_2_REGS, TMR_CFG_64BIT_CLK_INT);

    // 设置周期
    TimerPeriodSet(SOC_TMR_2_REGS, TMR_TIMER12, (TIM2_FREQ/freq));
    TimerPeriodSet(SOC_TMR_2_REGS, TMR_TIMER34, 0);

    // 使能 定时器 / 计数器 2
    TimerEnable(SOC_TMR_2_REGS, TMR_TIMER12, TMR_ENABLE_CONT);
}

// 初始化定时器2中断
static void Timer2InterruptInit(void)
{
    // 注册中断服务函数
    IntRegister(C674X_MASK_INT11, Timer2Isr);
    // 映射中断到 DSP 可屏蔽中断
    IntEventMap(C674X_MASK_INT11, SYS_INT_T64P2_TINTALL);
    // 使能 DSP 可屏蔽中断
    IntEnable(C674X_MASK_INT11);

    // 使能 定时器 / 计数器 中断
    TimerIntEnable(SOC_TMR_2_REGS, TMR_INT_TMR12_NON_CAPT_MODE);
}

// 定时器2中断处理函数
static void TIM2Handler(void)
{
    FLAG_TIM2 = 1;
}

// 定时器2中断服务函数
static void Timer2Isr(void)
{
    // 禁用定时器 / 计数器中断
    TimerIntDisable(SOC_TMR_2_REGS, TMR_INT_TMR12_NON_CAPT_MODE);

    // 清除中断标志
    IntEventClear(SYS_INT_T64P2_TINTALL);
    TimerIntStatusClear(SOC_TMR_2_REGS, TMR_INT_TMR12_NON_CAPT_MODE);

    // 中断执行内容
    TIM2Handler();

    // 使能 定时器 / 计数器 中断
    TimerIntEnable(SOC_TMR_2_REGS, TMR_INT_TMR12_NON_CAPT_MODE);
}
