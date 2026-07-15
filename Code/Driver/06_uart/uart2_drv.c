/**
 * @file uart2_drv.c
 * @brief UART2设备驱动实现文件
 * @details 实现UART2驱动的核心功能，包括初始化、数据发送和接收。
 * @ingroup UART2_DRV
 */

#include "uart2_drv.h"
#include "uart2_pin.h"
#include "soc_C6748.h"
#include "interrupt.h"
#include "uart.h"
#include "uart_api.h"


// UART最大数据长度
#define UART_MAX_DATA_LEN        0xFF

/* 变量定义 */
// 发送缓冲区长度
volatile unsigned char Uart2_txLength = 32;
// 发送缓冲区计数
volatile unsigned char Uart2_txCount = 0;
// 发送缓冲区标志位
volatile unsigned char FLAG_UART2_TX = 0;
// 接收缓冲区长度
volatile unsigned char Uart2_rxLength = 32;
// 接收缓冲区计数
volatile unsigned char Uart2_rxCount = 0;
// 接收缓冲区标志位
volatile unsigned char FLAG_UART2_RX = 0;
// 接收缓冲区CTI标志位
volatile unsigned char FLAG_UART2_RX_CTI = 0;
// 发送缓冲区
volatile unsigned char Uart2_txBuffer[UART_MAX_DATA_LEN] = {0};
// 接收缓冲区
volatile unsigned char Uart2_rxBuffer[UART_MAX_DATA_LEN] = {0};


/* 函数声明 */
// UART2 FIFO 初始化
static void UART2FIFOInit(unsigned int fifoLevel);
// UART2 中断初始化
static void UART2InterruptInit(void);
// UART2 中断服务函数
static void UART2Isr(void);
// UART2 中断发送函数
static void Uart2_Int_Send(void);
// UART2 中断接收函数
static void Uart2_Int_Recv(void);

/* 函数定义 */
// 初始化UART2
void Uart2_Init(unsigned int baudRate, unsigned int fifoLevel)
{
    Uart2_Init_Lite(baudRate);
    UART2FIFOInit(fifoLevel);
    UART2InterruptInit();
}

// UART2 FIFO 初始化
static void UART2FIFOInit(unsigned int fifoLevel)
{
    // 使能接收 / 发送 FIFO
    UARTFIFOEnable(SOC_UART_2_REGS);

    // 设置 FIFO 级别
    switch(fifoLevel)
    {
        case 1:
            UARTFIFOLevelSet(SOC_UART_2_REGS, UART_RX_TRIG_LEVEL_1);
            break;
        case 4:
            UARTFIFOLevelSet(SOC_UART_2_REGS, UART_RX_TRIG_LEVEL_4);
            break;
        case 8:
            UARTFIFOLevelSet(SOC_UART_2_REGS, UART_RX_TRIG_LEVEL_8);
            break;
        case 14:
            UARTFIFOLevelSet(SOC_UART_2_REGS, UART_RX_TRIG_LEVEL_14);
            break;
        default:
            UARTFIFOLevelSet(SOC_UART_2_REGS, UART_RX_TRIG_LEVEL_1);
            break;
    }
}

// UART 中断初始化
static void UART2InterruptInit(void)
{
    // 注册中断服务函数
    IntRegister(C674X_MASK_INT12, UART2Isr);
    // 映射中断到 DSP 可屏蔽中断
    IntEventMap(C674X_MASK_INT12, SYS_INT_UART2_INT);
    // 使能 DSP 可屏蔽中断
    IntEnable(C674X_MASK_INT12);

    // 使能中断
    unsigned int intFlags = 0;
    // 开启线路状态中断 UART_INT_LINE_STAT
    // 开启接收超时中断 UART_INT_RXDATA_CTI
    // 不开启发送中断   UART_INT_TX_EMPTY
    intFlags |= (UART_INT_LINE_STAT  |  \
                 UART_INT_RXDATA_CTI);
    UARTIntEnable(SOC_UART_2_REGS, intFlags);
}

// UART2 中断服务函数
static void UART2Isr(void)
{
    unsigned int int_id = 0;

    // 确定中断源
    int_id = UARTIntStatus(SOC_UART_2_REGS);

    // 清除 UART2 系统中断
    IntEventClear(SYS_INT_UART2_INT);

    // 发送中断
    if(UART_INTID_TX_EMPTY == int_id)
    {
        // 多次进中断，每次发送一字节数据
        Uart2_Int_Send();
    }

    // 接收中断
    if(UART_INTID_RX_DATA == int_id)
    {
        // 一次进中断接收FIFO数据
        Uart2_Int_Recv();
        FLAG_UART2_RX = 1;
    }

    // 接收超时中断
    if(UART_INTID_CTI == int_id)
    {
        // 一次进中断接收FIFO数据
        Uart2_Int_Recv();
        FLAG_UART2_RX_CTI = 1;
    }

    // 接收错误中断
    if(UART_INTID_RX_LINE_STAT == int_id)
    {
        while(UARTRxErrorGet(SOC_UART_2_REGS))
        {
            // 从 RBR 读一个字节
            UARTCharGetNonBlocking(SOC_UART_2_REGS);
        }
    }

    return;
}

// UART2 中断发送函数
static void Uart2_Int_Send(void)
{
    if(0 < Uart2_txLength)
    {
        // 写一个字节到 THR
        UARTCharPutNonBlocking(SOC_UART_2_REGS, Uart2_txBuffer[Uart2_txCount]);
        Uart2_txLength--;
        Uart2_txCount++;
    }
    if(0 == Uart2_txLength)
    {
        // 禁用发送中断
        UARTIntDisable(SOC_UART_2_REGS, UART_INT_TX_EMPTY);
        FLAG_UART2_TX = 1;
    }
}

// UART2 中断接收函数
static void Uart2_Int_Recv(void)
{
    int data;
    // 从 FIFO 逐个读取字符并输出到接收缓冲区
    while((data = UARTCharGetNonBlocking(SOC_UART_2_REGS)) != -1)
    {
        Uart2_rxBuffer[Uart2_rxCount] = (unsigned char)data;
        Uart2_rxCount = (Uart2_rxCount + 1) % Uart2_rxLength;
    }
}

// 发送数据到指定缓冲区
void Uart2_SendBuffer(unsigned char *buffer, unsigned int length)
{
    // 复制数据到发送缓冲区
    unsigned int i;
    for(i = 0; i < length; i++)
    {
        Uart2_txBuffer[i] = buffer[i];
    }
    
    // 设置发送长度和计数器
    Uart2_txLength = length;
    Uart2_txCount = 0;
    
    // 重置发送完成标志
    FLAG_UART2_TX = 0;

    // 启用发送中断
    UARTIntEnable(SOC_UART_2_REGS, UART_INT_TX_EMPTY);
}

// 设置接收缓冲区长度
void Uart2_SetRxBuffer(unsigned int length)
{
    // 设置缓冲区长度和计数器
    Uart2_rxLength = length;
    Uart2_rxCount = 0;
    
    // 重置接收标志
    FLAG_UART2_RX = 0;
}
