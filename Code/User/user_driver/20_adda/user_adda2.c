/**
 * user_adda2.c
 *
 * 描述: ADDA 用户使用示例文件 - 不同频率ADDA输出
 * 功能: 展示不同频率的ADDA使用方法和示例
 */

#include "adc_api.h"
#include "dac_api.h"
#include "system.h"
#include "math.h"
#include "key_api.h"


// 定义用户缓冲区 - AD使用较大的缓冲区
static short User_AD_Buffer1[ADC_SAMPLE_1024];  // 50kHz, 1024点
static short User_AD_Buffer2[ADC_SAMPLE_1024];
static short User_AD_Buffer3[ADC_SAMPLE_1024];
static short User_AD_Buffer4[ADC_SAMPLE_1024];
static short User_AD_Buffer5[ADC_SAMPLE_1024];
static short User_AD_Buffer6[ADC_SAMPLE_1024];
static short User_AD_Buffer7[ADC_SAMPLE_1024];
static short User_AD_Buffer8[ADC_SAMPLE_1024];

// DA使用较小的缓冲区
static short User_DA_Buffer1[DAC_SAMPLE_512];   // 25kHz, 512点
static short User_DA_Buffer2[DAC_SAMPLE_512];
static short User_DA_Buffer3[DAC_SAMPLE_512];
static short User_DA_Buffer4[DAC_SAMPLE_512];
static short User_DA_Buffer5[DAC_SAMPLE_512];
static short User_DA_Buffer6[DAC_SAMPLE_512];
static short User_DA_Buffer7[DAC_SAMPLE_512];
static short User_DA_Buffer8[DAC_SAMPLE_512];


/**
 * @brief 下采样函数 - 将50kHz的AD数据下采样为25kHz的DA数据
 * @param ad_buf AD数据缓冲区
 * @param da_buf DA数据缓冲区
 * @param ad_len AD数据长度
 * @param da_len DA数据长度
 * @return 无
 */
void Downsample_50k_to_25k(short *ad_buf, short *da_buf, unsigned int ad_len, unsigned int da_len)
{
    unsigned int i, j;
    
    // 每2个AD采样点取1个作为DA输出点
    for (i = 0, j = 0; i < ad_len && j < da_len; i += 2, j++)
    {
        // 简单平均法下采样
        da_buf[j] = (ad_buf[i] + ad_buf[i+1]) / 2;
    }
}


/**
 * @brief 不同频率ADDA输出示例 - 50kAD配合25kDA
 * @return 无
 */
void Adda_Different_Freq_Example(void)
{
    // 定义标志位
    unsigned char FLAG_AD_DONE = 0;

    // 初始化系统
    Sys_Init();
    
    // 初始化按键
    Key_Init();
    
    // 初始化 ADC，设置输入频率为 50kHz，输入长度为 1024
    Adc_Init(ADC_50KHZ, ADC_SAMPLE_1024);

    // 初始化 DAC，设置输出频率为 25kHz，输出长度为 512，通道选择为全部通道
    Dac_Init(DAC_25KHZ, DAC_SAMPLE_512, DAC_CHANNEL_ALL);

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
                memcpy(&User_AD_Buffer1, AD_CH1_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_AD_Buffer2, AD_CH2_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_AD_Buffer3, AD_CH3_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_AD_Buffer4, AD_CH4_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_AD_Buffer5, AD_CH5_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_AD_Buffer6, AD_CH6_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_AD_Buffer7, AD_CH7_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_AD_Buffer8, AD_CH8_Buf0, 2*ADC_SAMPLE_1024);
            }
            else
            {
                // 使用 Pong 缓冲区数据
                memcpy(&User_AD_Buffer1, AD_CH1_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_AD_Buffer2, AD_CH2_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_AD_Buffer3, AD_CH3_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_AD_Buffer4, AD_CH4_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_AD_Buffer5, AD_CH5_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_AD_Buffer6, AD_CH6_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_AD_Buffer7, AD_CH7_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_AD_Buffer8, AD_CH8_Buf1, 2*ADC_SAMPLE_1024);
            }
            
            // 执行下采样，将50kHz的AD数据转换为25kHz的DA数据
            Downsample_50k_to_25k(User_AD_Buffer1, User_DA_Buffer1, ADC_SAMPLE_1024, DAC_SAMPLE_512);
            Downsample_50k_to_25k(User_AD_Buffer2, User_DA_Buffer2, ADC_SAMPLE_1024, DAC_SAMPLE_512);
            Downsample_50k_to_25k(User_AD_Buffer3, User_DA_Buffer3, ADC_SAMPLE_1024, DAC_SAMPLE_512);
            Downsample_50k_to_25k(User_AD_Buffer4, User_DA_Buffer4, ADC_SAMPLE_1024, DAC_SAMPLE_512);
            Downsample_50k_to_25k(User_AD_Buffer5, User_DA_Buffer5, ADC_SAMPLE_1024, DAC_SAMPLE_512);
            Downsample_50k_to_25k(User_AD_Buffer6, User_DA_Buffer6, ADC_SAMPLE_1024, DAC_SAMPLE_512);
            Downsample_50k_to_25k(User_AD_Buffer7, User_DA_Buffer7, ADC_SAMPLE_1024, DAC_SAMPLE_512);
            Downsample_50k_to_25k(User_AD_Buffer8, User_DA_Buffer8, ADC_SAMPLE_1024, DAC_SAMPLE_512);
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
                memcpy(DA_CH1_Buf0, &User_DA_Buffer1, 2*DAC_SAMPLE_512);
                memcpy(DA_CH2_Buf0, &User_DA_Buffer2, 2*DAC_SAMPLE_512);
                memcpy(DA_CH3_Buf0, &User_DA_Buffer3, 2*DAC_SAMPLE_512);
                memcpy(DA_CH4_Buf0, &User_DA_Buffer4, 2*DAC_SAMPLE_512);
                memcpy(DA_CH5_Buf0, &User_DA_Buffer5, 2*DAC_SAMPLE_512);
                memcpy(DA_CH6_Buf0, &User_DA_Buffer6, 2*DAC_SAMPLE_512);
                memcpy(DA_CH7_Buf0, &User_DA_Buffer7, 2*DAC_SAMPLE_512);
                memcpy(DA_CH8_Buf0, &User_DA_Buffer8, 2*DAC_SAMPLE_512);
            }
            else
            {
                // 使用 Pong 缓冲区数据
                memcpy(DA_CH1_Buf1, &User_DA_Buffer1, 2*DAC_SAMPLE_512);
                memcpy(DA_CH2_Buf1, &User_DA_Buffer2, 2*DAC_SAMPLE_512);
                memcpy(DA_CH3_Buf1, &User_DA_Buffer3, 2*DAC_SAMPLE_512);
                memcpy(DA_CH4_Buf1, &User_DA_Buffer4, 2*DAC_SAMPLE_512);
                memcpy(DA_CH5_Buf1, &User_DA_Buffer5, 2*DAC_SAMPLE_512);
                memcpy(DA_CH6_Buf1, &User_DA_Buffer6, 2*DAC_SAMPLE_512);
                memcpy(DA_CH7_Buf1, &User_DA_Buffer7, 2*DAC_SAMPLE_512);
                memcpy(DA_CH8_Buf1, &User_DA_Buffer8, 2*DAC_SAMPLE_512);
            }
        }
        
        // 检查按键标志位
        if (FLAG_KEY1 == 1)
        {
            FLAG_KEY1 = 0;
            // 开始 ADC 输入
            Adc_Start();
        }
        
        // 检查按键标志位
        if (FLAG_KEY2 == 1)
        {
            FLAG_KEY2 = 0;
            // 停止 ADC 输入
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

