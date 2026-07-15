/**
 * @file adc_drv.c
 * @brief ADC驱动实现文件
 * @details 实现ADC的驱动功能，包括初始化、启动和停止操作。
 * @ingroup ADC_DRV
 */

/* 头文件包含 */
#include "adc_api.h"
#include "adc_drv.h"
#include "adc_pin.h"
#include "adc_emif.h"
#include "adc_pru.h"
#include "adc_tim1.h"
#include "adc_edma.h"
#include "edma0_common.h"
#include "delay.h"
#include "soc_C6748.h"
#include "gpio.h"


/* 函数声明 */
// AD7606复位函数
static void AD7606Init(void);

// ADC驱动初始化函数
void Adc_Init(unsigned int clkFreq, unsigned int sampleLen)
{
    // 初始化引脚
    Adc_Pin_Init();
    
    // 初始化EMIF
    Adc_Emif_Init();

    // 复位AD7606
    AD7606Init();
    
    // 初始化 EDMA0 公共资源
    Edma0_Common_Init();

    // 初始化EDMA
    Adc_EDMA_Init(sampleLen);
    
    // 初始化定时器1
    Adc_Timer1_Init(clkFreq);

    // ADC采集使能关闭
    AD_En = 0x00;

    // 初始化PRU1
    Adc_Pru1_Init();
}

// AD7606复位函数
static void AD7606Init(void)
{
    // 复位AD7606
    Adc_Reset_PinSet(1);
    delay(1);
    Adc_Reset_PinSet(0);
}

// AD7606启动函数
// 通过启动定时器1来启动ADC采样
void Adc_Start(void)
{
    // 定时器中断打开
    Adc_Timer1_Start();
    // ADC采集使能
    AD_En = 0x01;
}

// AD7606停止函数
// 通过停止定时器1来停止ADC采样
void Adc_Stop(void)
{
    // 定时器中断关闭
    Adc_Timer1_Stop();
    // ADC采集使能关闭
    AD_En = 0x00;
}
