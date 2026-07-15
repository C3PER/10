#include "system.h"

#include "interrupt.h"
#include "delay.h"



/* 系统初始化，中断和延时(delay) */
void Sys_Init(void)
{
    IntDSPINTCInit();       // 初始化 DSP 中断控制器
    IntGlobalEnable();      // 使能 DSP 全局中断
    DelayTimerSetup();      // 使能延时，占用TIM0中断10
}
