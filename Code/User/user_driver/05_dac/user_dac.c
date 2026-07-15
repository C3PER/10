/**
 * @file user_dac.c
 * @brief DAC 用户使用示例文件
 * @details 展示 DAC 的使用方法和示例，包括初始化、启动、停止和数据输出。
 * @ingroup DAC_EXAMPLE
 */

#include "dac_api.h"
#include "system.h"
#include "math.h"
#include "key_api.h"

// 定义用户缓冲区
static short User_Buffer[DAC_SAMPLE_1024];

// DAC 输出示例
// 展示如何初始化和使用DAC进行数据输出
void Dac_Example(void)
{
    unsigned int i = 0;

    // 准备输出数据
    // 示例：生成正弦波数据
    for(i = 0; i < DAC_SAMPLE_1024; i++)
    {
        // 生成 0-65535 范围的正弦波数据
        User_Buffer[i] = (short)(32767.0f * sin(2 * 3.14159 * i / DAC_SAMPLE_1024));
    }

    // 初始化系统
    Sys_Init();
    
    // 初始化按键
    Key_Init();
    
    // 初始化 DAC，设置输出频率为 50kHz，输出长度为 1024，通道选择为全部通道
    Dac_Init(DAC_50KHZ, DAC_SAMPLE_1024, DAC_CHANNEL_ALL);

    // 开始 DAC 输出
    Dac_Start();

    while(1)
    {
        // 检查 DAC 输出完成标志
        if (FLAG_DA == 1)
        {
            // 清除标志位
            FLAG_DA = 0;
            
            // 根据 Ping-Pong 标志确定当前使用的缓冲区
            if (DA_Ping_Pong == DA_BUFFER_PONG)
            {
                // 使用 Ping 缓冲区数据
                // 可以在这里更新 Ping 缓冲区的数据
                memcpy(DA_CH1_Buf0, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH2_Buf0, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH3_Buf0, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH4_Buf0, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH5_Buf0, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH6_Buf0, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH7_Buf0, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH8_Buf0, &User_Buffer, 2*DAC_SAMPLE_1024);
            }
            else
            {
                // 使用 Pong 缓冲区数据
                // 可以在这里更新 Pong 缓冲区的数据
                memcpy(DA_CH1_Buf1, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH2_Buf1, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH3_Buf1, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH4_Buf1, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH5_Buf1, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH6_Buf1, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH7_Buf1, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH8_Buf1, &User_Buffer, 2*DAC_SAMPLE_1024);
            }
        }
        // 检查按键标志位
        if (FLAG_KEY1 == 1)
        {
            FLAG_KEY1 = 0;
            // 开始 DAC 输出
            Dac_Start();
        }
        // 检查按键标志位
        if (FLAG_KEY2 == 1)
        {
            FLAG_KEY2 = 0;
            // 停止 DAC 输出
            Dac_Stop();
        }
    }
}
