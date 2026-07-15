/**
 * @file adc_pru.c
 * @brief ADC PRU控制实现文件
 * @details 实现ADC的PRU相关功能。
 * @ingroup ADC_PRU
 */

/* 头文件包含 */
#include "adc_pru.h"
#include "pru.h"
#include "PRU1_bin.h"

// 初始化协处理器 PRU1
void Adc_Pru1_Init(void)
{
    /* 关闭PRU1 */
    PruDisable(1);
    /* 启用PRU1 */
    PruEnable (1);
    /* 加载PRU1代码 */
    PruLoad(1, (unsigned int*)PRU1_Code, (sizeof(PRU1_Code)/sizeof(unsigned int)));
    /* 运行PRU1 */
    PruRun(1);
}
