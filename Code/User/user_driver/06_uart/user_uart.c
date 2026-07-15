/**
 * @file user_uart.c
 * @brief UART用户应用实现文件
 * @details 实现UART的使用案例，展示如何初始化和使用UART。
 * @ingroup UART_EXAMPLE
 */

#include "system.h"
#include "key_api.h"
#include "uart_api.h"
#include "uart.h"       // 提供printf、sprintf、UARTCharGet、UARTCharPut函数
#include "uartStdio.h"  // 提供UARTprintf、UARTPuts函数
#include "soc_C6748.h"  // UARTCharPut函数接近底层，需要
#include "delay.h"


/* 函数定义 */
// UART示例函数
void Uart_Example(void)
{
    int num = 123456789;
    char buffer[128];
    char Receive;

    Sys_Init();
    Key_Init();
    /* 初始化UART，波特率115200 */
    Uart2_Init_Lite(UART_BAUD_115200);

    // 函数发送测试1
    UARTPuts("123456789\r\n", 5);
    // 函数发送测试2
    UARTPuts("123456789\r\n", -1);
    // 函数发送测试3
    sprintf(buffer, "Num:%d\r\n", num);
    UARTPuts(buffer, -1);
    // 函数发送测试4
    UARTprintf("Num:%d\r\n", num);

    // 主循环
    for(;;)
    {
        /* 发送接收的数据 */
        // 程序等待接收一字节数据，一直等待直到有数据可读
        Receive=UARTCharGet(SOC_UART_2_REGS);
        // 发送接收到的数据
        UARTCharPut(SOC_UART_2_REGS, Receive);
    }
}


// UART示例函数（不开中断版本）
void Uart_Example_NoInterrupt(void)
{
    unsigned char rxBuffer[100];
    unsigned int rxLength;
    
    Sys_Init();
    /* 初始化UART，波特率115200 */
    Uart2_Init_Lite(UART_BAUD_115200);
    
    /* 发送欢迎信息 */
    UARTprintf("UART Example (No Interrupt) Started!\n\r");
    UARTprintf("This example demonstrates basic UART functions without interrupts.\n\r");
    UARTprintf("Type something and press Enter...\n\r");
    
    /* 主循环 */
    while(1)
    {
        /* 发送提示信息 */
        UARTprintf("\nEnter text: ");
        
        /* 接收字符串 */
        // 程序等待接收数据，一直等待直到有数据可读
        rxLength = UARTGets((char *)rxBuffer, 100);
        
        /* 发送接收到的数据 */
        UARTprintf("You entered: ");
        UARTPuts((char *)rxBuffer, rxLength);
        UARTprintf("\n");
        
        /* 发送字符 */
        UARTprintf("Sending individual characters: ");
        UARTCharPut(SOC_UART_2_REGS, 'H');
        UARTCharPut(SOC_UART_2_REGS, 'e');
        UARTCharPut(SOC_UART_2_REGS, 'l');
        UARTCharPut(SOC_UART_2_REGS, 'l');
        UARTCharPut(SOC_UART_2_REGS, 'o');
        UARTCharPut(SOC_UART_2_REGS, '!');
        UARTprintf("\n");
        
        /* 发送格式化数据 */
        UARTprintf("Formatted output: %s %d %x\n", "Test", 123, 0xABCD);

        delay(1000);
    }
}

// UART2中断通信示例函数
void Uart_Example_Interrupt(void)
{
    /* 发送缓冲区：存储要发送的欢迎信息 */
    unsigned char txBuffer[] = "Hello from interrupt mode!\n\r";
    
    /* 1. 系统初始化
     *  - 初始化DSP中断控制器
     *  - 使能全局中断
     *  - 初始化延时功能
     */
    Sys_Init();
    
    /* 2. UART2初始化
     *  - 配置UART2的GPIO引脚
     *  - 设置波特率为115200
     *  - 配置FIFO触发级别为8字节
     *  - 注册并配置UART2中断
     */
    Uart2_Init(UART_BAUD_115200, UART_FIFO_8);
    
    /* 3. 配置接收缓冲区
     *  - 设置接收缓冲区长度为20字节
     *  - 重置接收计数器和标志位
     */
    Uart2_SetRxBuffer(20);
    
    /* 4. 发送欢迎信息
     *  - 使用中断方式发送数据
     *  - 中断服务函数会自动处理数据发送
     */
    Uart2_SendBuffer(txBuffer, sizeof(txBuffer));
    
    /* 5. 等待发送完成
     *  - 轮询FLAG_UART2_TX标志位
     *  - 发送完成后重置标志位
     */
    while(!FLAG_UART2_TX);
    FLAG_UART2_TX = 0;
    
    /* 6. 发送提示信息
     *  - 使用UARTprintf函数（非中断方式）
     *  - 向用户说明示例功能
     */
    UARTprintf("UART Example (Interrupt) Started!\n\r");
    UARTprintf("This example demonstrates UART functions with interrupts.\n\r");
    UARTprintf("Type something and press Enter...\n\r");
    
    /* 7. 主循环
     *  - 持续检测接收中断标志
     *  - 处理接收到的数据
     */
    while(1)
    {
        /* 检查FIFO触发中断
         * 当UART2接收到数据并达到FIFO触发级别时
         * 中断服务函数会读取数据到Uart2_rxBuffer并设置FLAG_UART2_RX标志位
         */
        if(FLAG_UART2_RX)
        {
            /* 重置接收标志，准备下一次接收 */
            FLAG_UART2_RX = 0;

            /* 回显接收到的数据
             * 1. 打印接收类型标识
             * 2. 发送接收缓冲区的数据
             * 3. 打印确认信息
             */
            UARTprintf("Uart2_rxBuffer: ");
            Uart2_SendBuffer((unsigned char *)Uart2_rxBuffer, 20);
            UARTprintf("\n\r");
        }
        
        /* 检查接收超时中断（CTI）
         * 当UART2接收数据后一段时间没有新数据时触发
         * （即一次没有收满FIFO缓冲区）
         * 用于处理不完整的数据包
         */
        if(FLAG_UART2_RX_CTI)
        {
            /* 重置接收超时标志，准备下一次接收 */
            FLAG_UART2_RX_CTI = 0;

            /* 回显接收到的数据
             * 1. 打印接收类型标识
             * 2. 发送接收缓冲区的数据
             * 3. 打印确认信息
             */
            UARTprintf("Uart2_rxBuffer: ");
            Uart2_SendBuffer((unsigned char *)Uart2_rxBuffer, 20);
            UARTprintf("\n\r");
        }
    }
}
