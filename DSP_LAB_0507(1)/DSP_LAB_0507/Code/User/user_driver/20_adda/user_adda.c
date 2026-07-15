/**
 * user_adda.c
 *
 * 描述: ADDA 用户使用示例文件
 * 功能: 展示 ADDA 的使用方法和示例
 */

#include "adc_api.h"
#include "dac_api.h"
#include "system.h"
#include "math.h"
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

/**
 * @brief DAC 输出示例
 * @return 无
 */
void Adda_Example(void)
{
    // 定义标志位
    unsigned char FLAG_AD_DONE = 0;

    // 初始化系统
    Sys_Init();
    
    // 初始化按键
    Key_Init();
    
    // 初始化 ADC，设置输入频率为 50kHz，输入长度为 1024
    Adc_Init(ADC_50KHZ, ADC_SAMPLE_1024);

    // 初始化 DAC，设置输出频率为 50kHz，输出长度为 1024，通道选择为全部通道
    Dac_Init(DAC_50KHZ, DAC_SAMPLE_1024, DAC_CHANNEL_ALL);//DAC_CHANNEL_ALL

    /* 同时启动 ADC 输入和 DAC 输出 */
    // 开始 ADC 输入
    Adc_Start();
    // 开始 DAC 输出
    Dac_Start();
    
    while(1)
    {
        // 检查 ADC 采集完成标志
        if (FLAG_AD == 1)
        {
            // 清除标志位
            FLAG_AD = 0;

            // 设置 ADC 采集完成标志位
            FLAG_AD_DONE = 1;

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
        // 检查 DAC 输出完成标志
        if (FLAG_DA == 1 && FLAG_AD_DONE == 1)
        {
            // 清除标志位
            FLAG_DA = 0;
            
            // 清除 ADC 采集完成标志位
            FLAG_AD_DONE = 0;

            // 根据 Ping-Pong 标志确定当前使用的缓冲区
            if (DA_Ping_Pong == DA_BUFFER_PONG)
            {
                // 使用 Ping 缓冲区数据
                // 可以在这里更新 Ping 缓冲区的数据
                memcpy(DA_CH1_Buf0, &User_Buffer1, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH2_Buf0, &User_Buffer2, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH3_Buf0, &User_Buffer3, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH4_Buf0, &User_Buffer4, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH5_Buf0, &User_Buffer5, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH6_Buf0, &User_Buffer6, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH7_Buf0, &User_Buffer7, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH8_Buf0, &User_Buffer8, 2*DAC_SAMPLE_1024);
            }
            else
            {
                // 使用 Pong 缓冲区数据
                // 可以在这里更新 Pong 缓冲区的数据
                memcpy(DA_CH1_Buf1, &User_Buffer1, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH2_Buf1, &User_Buffer2, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH3_Buf1, &User_Buffer3, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH4_Buf1, &User_Buffer4, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH5_Buf1, &User_Buffer5, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH6_Buf1, &User_Buffer6, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH7_Buf1, &User_Buffer7, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH8_Buf1, &User_Buffer8, 2*DAC_SAMPLE_1024);
            }
        }
        // 检查按键标志位
        if (FLAG_KEY1 == 1)
        {
            FLAG_KEY1 = 0;
            // 开始 DAC 输出
            Adc_Start();
        }
        // 检查按键标志位
        if (FLAG_KEY2 == 1)
        {
            FLAG_KEY2 = 0;
            // 停止 DAC 输出
            Adc_Stop();
        }
        // 检查按键标志位
        if (FLAG_KEY3 == 1)
        {
            FLAG_KEY3 = 0;
            // 开始 DAC 输出
            Dac_Start();
        }
        // 检查按键标志位
        if (FLAG_KEY4 == 1)
        {
            FLAG_KEY4 = 0;
            // 停止 DAC 输出
            Dac_Stop();
        }
    }
}
