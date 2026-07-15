/**
 * @file rs485_drv.c
 * @brief RS485设备驱动实现文件
 * @details 实现RS485驱动的核心功能，包括初始化、数据发送和接收、模式控制等。
 * @ingroup RS485_DRV
 */

#include "rs485_drv.h"
#include "rs485_api.h"
#include "uart1_drv.h"
#include "uart1_pin.h"
#include "rs485_pin.h"
#include "delay.h"


/* 函数定义 */
// 初始化RS485
void Rs485_Init(unsigned int baudRate, unsigned int fifoLevel)
{
    /* 初始化UART1 */
    Uart1_Init(baudRate, fifoLevel);
    
    /* 初始化RS485引脚 */
    RS485_PinInit();
    /* 默认设置为接收模式 */
    Rs485_SetMode(RS485_MODE_RX);
}

// 简化版RS485初始化
void Rs485_Init_Lite(unsigned int baudRate)
{
    /* 初始化UART1 */
    Uart1_Init_Lite(baudRate);
    
    /* 初始化RS485引脚 */
    RS485_PinInit();
    /* 默认设置为接收模式 */
    Rs485_SetMode(RS485_MODE_RX);
}

// 发送数据到RS485
void Rs485_SendBuffer(unsigned char *buffer, unsigned int length)
{
    /* 设置为发送模式 */
    Rs485_SetMode(RS485_MODE_TX);
    
    /* 发送数据 */
    Uart1_SendBuffer(buffer, length);
    
    /* 等待发送完成 */
    while(!FLAG_UART1_TX);
    FLAG_UART1_TX = 0;
    
    /* 延时1ms，等待数据发送完成 */
    delay(1);
    /* 恢复为接收模式 */
    Rs485_SetMode(RS485_MODE_RX);
}

// 设置RS485接收缓冲区长度
void Rs485_SetRxBuffer(unsigned int length)
{
    /* 设置UART1接收缓冲区长度 */
    Uart1_SetRxBuffer(length);
}

// 设置RS485传输模式
void Rs485_SetMode(unsigned int mode)
{
    if(mode == RS485_MODE_TX)
    {
        /* 设置为发送模式 */
        RS485_EN_PinSet(1);
    }
    else
    {
        /* 设置为接收模式 */
        RS485_EN_PinSet(0);
    }
}
