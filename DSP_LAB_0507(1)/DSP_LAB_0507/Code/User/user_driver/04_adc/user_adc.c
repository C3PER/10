/**
 * @file user_adc.c
 * @brief ADC 用户使用示例文件
 * @details 展示 ADC 的使用方法和示例，包括初始化、启动、停止和数据采集。
 * @ingroup ADC_EXAMPLE
 */

// #include "user_config.h"
#include "adc_api.h"
#include "system.h"
#include "key_api.h"

// 定义用户缓冲区
static short User_Buffer1[ADC_SAMPLE_1024];
static short User_Buffer2[ADC_SAMPLE_1024];
static short User_Buffer3[ADC_SAMPLE_1024];
static short User_Buffer4[ADC_SAMPLE_1024];
static short User_Buffer5[ADC_SAMPLE_1024];
static short User_Buffer6[ADC_SAMPLE_1024];
static short User_Buffer7[ADC_SAMPLE_1024];
static short User_Buffer8[ADC_SAMPLE_1024];

// ADC 采样示例
// 展示如何初始化和使用ADC进行数据采集
void Adc_Example(void)
{
    // 初始化系统
    Sys_Init();

    // 初始化按键
    Key_Init();

    // 初始化 ADC，设置采样频率为 50kHz，采样长度为 1024
    Adc_Init(ADC_50KHZ, ADC_SAMPLE_1024);

    // 开始 ADC 采样
    Adc_Start();
    
    while(1)
    {
        // 检查 ADC 采集完成标志
        if (FLAG_AD == 1)
        {
            // 清除标志位
            FLAG_AD = 0;
            
            // 根据 Ping-Pong 标志确定当前使用的缓冲区
            if (AD_Ping_Pong == AD_BUFFER_PONG)
            {
                // 使用 Ping 缓冲区数据
                memcpy(&User_Buffer1, AD_CH1_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer2, AD_CH2_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer3, AD_CH3_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer4, AD_CH4_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer5, AD_CH5_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer6, AD_CH6_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer7, AD_CH7_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer8, AD_CH8_Buf0, 2*ADC_SAMPLE_1024);
            }
            else
            {
                // 使用 Pong 缓冲区数据
                memcpy(&User_Buffer1, AD_CH1_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer2, AD_CH2_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer3, AD_CH3_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer4, AD_CH4_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer5, AD_CH5_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer6, AD_CH6_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer7, AD_CH7_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer8, AD_CH8_Buf1, 2*ADC_SAMPLE_1024);
            }
        }
        // 检查按键标志位
        if (FLAG_KEY1 == 1)
        {
            FLAG_KEY1 = 0;
            // 开始 ADC 采样
            Adc_Start();
        }
        // 检查按键标志位
        if (FLAG_KEY2 == 1)
        {
            FLAG_KEY2 = 0;
            // 停止 ADC 采样
            Adc_Stop();
        }
    }
}
