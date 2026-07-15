/**
 * uart1_drv.c
 *
 * 描述: UART1设备驱动实现文件
 * 功能: 实现UART1驱动的核心功能
 */

#include "uart1_drv.h"
#include "uart1_pin.h"
#include "soc_C6748.h"
#include "interrupt.h"
#include "uart.h"


/* 变量定义 */
// 接收缓冲区长度
volatile unsigned char Uart1_rxLength = 32;
// 接收缓冲区计数
volatile unsigned char Uart1_rxCount = 0;
// 接收缓冲区标志位
volatile unsigned char FLAG_UART1_RX = 0;
// 接收缓冲区CTI标志位
volatile unsigned char FLAG_UART1_RX_CTI = 0;
// 发送缓冲区长度
volatile unsigned char Uart1_txLength = 32;
// 发送缓冲区计数
volatile unsigned char Uart1_txCount = 0;
// 发送缓冲区标志位
volatile unsigned char FLAG_UART1_TX = 0;
// 发送缓冲区
volatile unsigned char Uart1_txBuffer[32] = {0};
// 接收缓冲区
volatile unsigned char Uart1_rxBuffer[32] = {0};


/* 函数声明 */
static void UART1FIFOInit(unsigned int fifoLevel);
static void UART1InterruptInit(void);
static void UART1Isr(void);
static void Uart1_Int_Send(void);
static void Uart1_Int_Recv(void);

/**
 * @brief 初始化UART1
 * @param baudRate: 波特率
 * @param fifoLevel: FIFO触发级别 (1, 4, 8, 14)
 * @return 无
 */
void Uart1_Init(unsigned int baudRate, unsigned int fifoLevel)
{
    Uart1_Init_Lite(baudRate);
    UART1FIFOInit(fifoLevel);
    UART1InterruptInit();
}

/**
 * @brief UART1 FIFO 初始化
 * @param fifoLevel: FIFO触发级别 (1, 4, 8, 14)
 * @return 无
 */
static void UART1FIFOInit(unsigned int fifoLevel)
{
    /* 使能接收 / 发送 FIFO */
    UARTFIFOEnable(SOC_UART_1_REGS);

    /* 设置 FIFO 级别 */
    switch(fifoLevel)
    {
        case 1:
            UARTFIFOLevelSet(SOC_UART_1_REGS, UART_RX_TRIG_LEVEL_1);
            break;
        case 4:
            UARTFIFOLevelSet(SOC_UART_1_REGS, UART_RX_TRIG_LEVEL_4);
            break;
        case 8:
            UARTFIFOLevelSet(SOC_UART_1_REGS, UART_RX_TRIG_LEVEL_8);
            break;
        case 14:
            UARTFIFOLevelSet(SOC_UART_1_REGS, UART_RX_TRIG_LEVEL_14);
            break;
        default:
            UARTFIFOLevelSet(SOC_UART_1_REGS, UART_RX_TRIG_LEVEL_1);
            break;
    }
}
/**
 * @brief UART1 中断初始化
 * @return 无
 */
static void UART1InterruptInit(void)
{
    /* 注册中断服务函数 */
    IntRegister(C674X_MASK_INT13, UART1Isr);
    /* 映射中断到 DSP 可屏蔽中断 */
    IntEventMap(C674X_MASK_INT13, SYS_INT_UART1_INT);
    /* 使能 DSP 可屏蔽中断 */
    IntEnable(C674X_MASK_INT13);

    /* 使能中断 */
    unsigned int intFlags = 0;
    // 开启线路状态中断 UART_INT_LINE_STAT
    // 开启接收超时中断 UART_INT_RXDATA_CTI
    // 不开启发送中断   UART_INT_TX_EMPTY
    intFlags |= (UART_INT_LINE_STAT  |  \
                 UART_INT_RXDATA_CTI);
    UARTIntEnable(SOC_UART_1_REGS, intFlags);
}

/**
 * @brief UART1 中断服务函数
 * @return 无
 */
static void UART1Isr(void)
{
    unsigned int int_id = 0;

    /* 确定中断源 */
    int_id = UARTIntStatus(SOC_UART_1_REGS);

    /* 清除 UART1 系统中断 */
    IntEventClear(SYS_INT_UART1_INT);

    /* 发送中断 */
    if(UART_INTID_TX_EMPTY == int_id)
    {
        // 多次进中断，每次发送一字节数据
        Uart1_Int_Send();
    }

    /* 接收中断 */
    if(UART_INTID_RX_DATA == int_id)
    {
        // 一次进中断接收FIFO数据
        Uart1_Int_Recv();
        FLAG_UART1_RX = 1;
    }

    /* 接收超时中断 */
    if(UART_INTID_CTI == int_id)
    {
        // 一次进中断接收FIFO数据
        Uart1_Int_Recv();
        FLAG_UART1_RX_CTI = 1;
    }
        
    /* 接收错误 */
    if(UART_INTID_RX_LINE_STAT == int_id)
    {
        while(UARTRxErrorGet(SOC_UART_1_REGS))
        {
            /* 从 RBR 读一个字节 */
            UARTCharGetNonBlocking(SOC_UART_1_REGS);
        }
    }

    return;
}


/**
 * @brief UART1 中断发送函数
 * @note 该函数在发送中断触发时被调用，每次发送一字节数据
 * @note 发送完成后，禁用发送中断
 * @return 无
 */
static void Uart1_Int_Send(void)
{
    if(0 < Uart1_txLength)
    {
        /* 写一个字节到 THR */
        UARTCharPutNonBlocking(SOC_UART_1_REGS, Uart1_txBuffer[Uart1_txCount]);
        Uart1_txLength--;
        Uart1_txCount++;
    }
    if(0 == Uart1_txLength)
    {
        /* 禁用发送中断 */
        UARTIntDisable(SOC_UART_1_REGS, UART_INT_TX_EMPTY);
        FLAG_UART1_TX = 1;
    }
}

/**
 * @brief UART1 中断接收函数
 * @note 该函数在接收中断触发时被调用，接收FIFO的全部数据
 * @return 无
 */
static void Uart1_Int_Recv(void)
{
    int data;
    // 从 FIFO 逐个读取字符并输出到接收缓冲区
    while((data = UARTCharGetNonBlocking(SOC_UART_1_REGS)) != -1)
    {
        Uart1_rxBuffer[Uart1_rxCount] = (unsigned char)data;
        Uart1_rxCount = (Uart1_rxCount + 1) % Uart1_rxLength;
    }
}

/**
 * @brief 发送数据到指定缓冲区
 * @param buffer: 数据缓冲区
 * @param length: 数据长度
 * @return 无
 */
void Uart1_SendBuffer(unsigned char *buffer, unsigned int length)
{
    /* 复制数据到发送缓冲区 */
    unsigned int i;
    for(i = 0; i < length; i++)
    {
        Uart1_txBuffer[i] = buffer[i];
    }
    
    /* 设置发送长度和计数器 */
    Uart1_txLength = length;
    Uart1_txCount = 0;
    
    /* 重置发送完成标志 */
    FLAG_UART1_TX = 0;

    /* 启用发送中断 */
    UARTIntEnable(SOC_UART_1_REGS, UART_INT_TX_EMPTY);
}

/**
 * @brief 设置UART1接收缓冲区长度
 * @param length: 缓冲区长度
 * @return 无
 */
void Uart1_SetRxBuffer(unsigned int length)
{
    /* 设置缓冲区长度和计数器 */
    Uart1_rxLength = length;
    Uart1_rxCount = 0;
    
    /* 重置接收标志 */
    FLAG_UART1_RX = 0;
}
