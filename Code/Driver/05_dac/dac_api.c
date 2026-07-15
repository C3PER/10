/**
 * @file dac_api.c
 * @brief DAC用户接口实现文件
 * @details 实现DAC的用户接口功能。
 * @ingroup DAC_API
 */

#include "dac_api.h"


/* DAC标志位 */

// DA_Len: 数据长度
#pragma DATA_SECTION(DA_Len, "User_DALenFile")
volatile unsigned short DA_Len;

// DA_En: DAC使能标志位
#pragma DATA_SECTION(DA_En, "User_DAEnFile")
volatile unsigned char DA_En = 0x01;

// FLAG_DA: DAC标志位
#pragma DATA_SECTION(FLAG_DA, "User_DAFlagFile")
volatile unsigned char FLAG_DA;

// DA_Ping_Pong: Ping-Pong缓冲位
#pragma DATA_SECTION(DA_Ping_Pong, "User_DAPingPongFile")
volatile unsigned char DA_Ping_Pong = 0x00;

// DA_Sel: 选择通道
#pragma DATA_SECTION(DA_Sel, "User_DASelFile")
volatile unsigned char DA_Sel = 0xFF;



/* DAC缓冲区 */
// Buf0: Ping Buffer
// Buf1: Pong Buffer

/* --- Channel 1 --- */
#pragma DATA_SECTION(DA_CH1_Buf0, "DA_Ch1Buf0_File")
short DA_CH1_Buf0[DAC_MAX_LEN];
#pragma DATA_SECTION(DA_CH1_Buf1, "DA_Ch1Buf1_File")
short DA_CH1_Buf1[DAC_MAX_LEN];

/* --- Channel 2 --- */
#pragma DATA_SECTION(DA_CH2_Buf0, "DA_Ch2Buf0_File")
short DA_CH2_Buf0[DAC_MAX_LEN];
#pragma DATA_SECTION(DA_CH2_Buf1, "DA_Ch2Buf1_File")
short DA_CH2_Buf1[DAC_MAX_LEN];

/* --- Channel 3 --- */
#pragma DATA_SECTION(DA_CH3_Buf0, "DA_Ch3Buf0_File")
short DA_CH3_Buf0[DAC_MAX_LEN];
#pragma DATA_SECTION(DA_CH3_Buf1, "DA_Ch3Buf1_File")
short DA_CH3_Buf1[DAC_MAX_LEN];

/* --- Channel 4 --- */
#pragma DATA_SECTION(DA_CH4_Buf0, "DA_Ch4Buf0_File")
short DA_CH4_Buf0[DAC_MAX_LEN];
#pragma DATA_SECTION(DA_CH4_Buf1, "DA_Ch4Buf1_File")
short DA_CH4_Buf1[DAC_MAX_LEN];

/* --- Channel 5 --- */
#pragma DATA_SECTION(DA_CH5_Buf0, "DA_Ch5Buf0_File")
short DA_CH5_Buf0[DAC_MAX_LEN];
#pragma DATA_SECTION(DA_CH5_Buf1, "DA_Ch5Buf1_File")
short DA_CH5_Buf1[DAC_MAX_LEN];

/* --- Channel 6 --- */
#pragma DATA_SECTION(DA_CH6_Buf0, "DA_Ch6Buf0_File")
short DA_CH6_Buf0[DAC_MAX_LEN];
#pragma DATA_SECTION(DA_CH6_Buf1, "DA_Ch6Buf1_File")
short DA_CH6_Buf1[DAC_MAX_LEN];

/* --- Channel 7 --- */
#pragma DATA_SECTION(DA_CH7_Buf0, "DA_Ch7Buf0_File")
short DA_CH7_Buf0[DAC_MAX_LEN];
#pragma DATA_SECTION(DA_CH7_Buf1, "DA_Ch7Buf1_File")
short DA_CH7_Buf1[DAC_MAX_LEN];

/* --- Channel 8 --- */
#pragma DATA_SECTION(DA_CH8_Buf0, "DA_Ch8Buf0_File")
short DA_CH8_Buf0[DAC_MAX_LEN];
#pragma DATA_SECTION(DA_CH8_Buf1, "DA_Ch8Buf1_File")
short DA_CH8_Buf1[DAC_MAX_LEN];
