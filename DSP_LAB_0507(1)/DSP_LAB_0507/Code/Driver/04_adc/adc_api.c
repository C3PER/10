/**
 * @file adc_api.c
 * @brief ADC用户接口实现文件
 * @details 实现ADC的用户接口功能。
 * @ingroup ADC_API
 */

#include "adc_api.h"


/* ADC标志位 */

// AD_Ping_Pong: Ping-Pong缓冲位
#pragma DATA_SECTION(AD_Ping_Pong, "User_ADPingPongFile")
volatile unsigned char AD_Ping_Pong = 0x00;

// AD_En: ADC使能标志位
#pragma DATA_SECTION(AD_En, "User_ADEnFile")
volatile unsigned char AD_En = 0x01;

// FLAG_AD: ADC标志位
#pragma DATA_SECTION(FLAG_AD, "User_ADFlagFile")
volatile unsigned char FLAG_AD;

// AD_Len: 数据长度
#pragma DATA_SECTION(AD_Len, "User_ADLenFile")
volatile unsigned short AD_Len;


/* ADC缓冲区 */
// Buf0: Ping Buffer
// Buf1: Pong Buffer

/* --- Channel 1 --- */
#pragma DATA_SECTION(AD_CH1_Buf0, "AD_Ch1Buf0_File")
short AD_CH1_Buf0[ADC_MAX_LEN];
#pragma DATA_SECTION(AD_CH1_Buf1, "AD_Ch1Buf1_File")
short AD_CH1_Buf1[ADC_MAX_LEN];

/* --- Channel 2 --- */
#pragma DATA_SECTION(AD_CH2_Buf0, "AD_Ch2Buf0_File")
short AD_CH2_Buf0[ADC_MAX_LEN];
#pragma DATA_SECTION(AD_CH2_Buf1, "AD_Ch2Buf1_File")
short AD_CH2_Buf1[ADC_MAX_LEN];

/* --- Channel 3 --- */
#pragma DATA_SECTION(AD_CH3_Buf0, "AD_Ch3Buf0_File")
short AD_CH3_Buf0[ADC_MAX_LEN];
#pragma DATA_SECTION(AD_CH3_Buf1, "AD_Ch3Buf1_File")
short AD_CH3_Buf1[ADC_MAX_LEN];

/* --- Channel 4 --- */
#pragma DATA_SECTION(AD_CH4_Buf0, "AD_Ch4Buf0_File")
short AD_CH4_Buf0[ADC_MAX_LEN];
#pragma DATA_SECTION(AD_CH4_Buf1, "AD_Ch4Buf1_File")
short AD_CH4_Buf1[ADC_MAX_LEN];

/* --- Channel 5 --- */
#pragma DATA_SECTION(AD_CH5_Buf0, "AD_Ch5Buf0_File")
short AD_CH5_Buf0[ADC_MAX_LEN];
#pragma DATA_SECTION(AD_CH5_Buf1, "AD_Ch5Buf1_File")
short AD_CH5_Buf1[ADC_MAX_LEN];

/* --- Channel 6 --- */
#pragma DATA_SECTION(AD_CH6_Buf0, "AD_Ch6Buf0_File")
short AD_CH6_Buf0[ADC_MAX_LEN];
#pragma DATA_SECTION(AD_CH6_Buf1, "AD_Ch6Buf1_File")
short AD_CH6_Buf1[ADC_MAX_LEN];

/* --- Channel 7 --- */
#pragma DATA_SECTION(AD_CH7_Buf0, "AD_Ch7Buf0_File")
short AD_CH7_Buf0[ADC_MAX_LEN];
#pragma DATA_SECTION(AD_CH7_Buf1, "AD_Ch7Buf1_File")
short AD_CH7_Buf1[ADC_MAX_LEN];

/* --- Channel 8 --- */
#pragma DATA_SECTION(AD_CH8_Buf0, "AD_Ch8Buf0_File")
short AD_CH8_Buf0[ADC_MAX_LEN];
#pragma DATA_SECTION(AD_CH8_Buf1, "AD_Ch8Buf1_File")
short AD_CH8_Buf1[ADC_MAX_LEN];


