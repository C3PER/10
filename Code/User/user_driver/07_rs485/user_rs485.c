/**
 * @file user_rs485.c
 * @brief RS485用户使用实现文件
 * @details 实现RS485的用户使用示例，展示如何初始化和使用RS485。
 * @ingroup RS485_EXAMPLE
 */
#include "user_rs485.h"

#include "system.h"
#include "rs485_api.h"

#include "soc_C6748.h"
#include "uart.h"       // 提供printf、sprintf、UARTCharGet、UARTCharPut函数
#include "delay.h"
//#include "led_api.h"
//#include "key_api.h"
//#include "rs485_api.h"



/* 函数定义 */
// RS485用户使用示例
void Rs485_Example(void)
{
    char Receive;

    /* 中断和延时初始化 */
    Sys_Init();  

    /* 初始化RS485 */
    Rs485_Init_Lite(RS485_BAUD_115200);
    // 主循环
    for(;;)
    {
        // 说明，RS485和串口通信不同，
        // RS485为半双工通信模式，串口是全双工通信模式。
        // 因此类似串口的写法，RS485每次只能自发自收一字节信息。

        /* 发送接收的数据 */
        // 程序等待接收一字节数据，一直等待直到有数据可读
        Receive=UARTCharGet(SOC_UART_1_REGS);
        delay(1);   // 等待数据接收完成
        // 设置为发送模式
        Rs485_SetMode(RS485_MODE_TX);
        // 发送接收到的数据
        UARTCharPut(SOC_UART_1_REGS, Receive);
        delay(1);   // 等待数据发送完成
        // 恢复为接收模式
        Rs485_SetMode(RS485_MODE_RX);
    }
}

void Rs485_Example_Interrupt(void)
{
    unsigned char txBuffer[] = "Hello from interrupt mode!\n\r";
    unsigned char txBuffer2[] = "Uart1_rxBuffer: ";
    unsigned char txBuffer3[] = "\n\r";

    /* 中断和延时初始化 */
    Sys_Init();  
    /* 初始化RS485 */
    Rs485_Init(RS485_BAUD_115200, RS485_FIFO_8);

    Rs485_SetRxBuffer(20);

    Rs485_SendBuffer(txBuffer, sizeof(txBuffer));
//    while(!FLAG_UART1_TX);
//    FLAG_UART1_TX = 0;

    while(1)
    {
        /* 检查接收标志 */
        if(FLAG_UART1_RX || FLAG_UART1_RX_CTI)
        {
            /* 重置接收标志 */
            FLAG_UART1_RX = 0;
            FLAG_UART1_RX_CTI = 0;

            // 延时1ms，等待数据接收完成
            delay(1);
            /* 发送接收到的数据（使用中断方式） */
            Rs485_SendBuffer(txBuffer2, sizeof(txBuffer2));
            Rs485_SendBuffer((unsigned char *)Uart1_rxBuffer, 20);
            Rs485_SendBuffer(txBuffer3, sizeof(txBuffer3));
        }
    }
}

// /**
//  * @brief RS485发送测试
//  * @param baudRate: 波特率
//  * @return 无
//  */
// void Rs485_Send_Test(unsigned int baudRate)
// {
//     unsigned char sendBuffer[] = "RS485 Send Test\r\n";
//     unsigned int sendLength = sizeof(sendBuffer) - 1;
    
//     /* 初始化RS485 */
//     Rs485_Init(baudRate, RS485_FIFO_8);
    
//     /* 发送数据 */
//     while(1)
//     {
//         /* 发送数据 */
//         Rs485_SendBuffer(sendBuffer, sendLength);
        
//         /* 等待发送完成 */
//         while(!FLAG_RS485_TX);
//         FLAG_RS485_TX = 0;
        
//         /* 闪烁LED1_BLUE */
//         Led_Control(LED1_BLUE, LED_TOGGLE);
        
//         /* 延时 */
//         delay(1000);
//     }
// }

// /**
//  * @brief RS485接收测试
//  * @param baudRate: 波特率
//  * @return 无
//  */
// void Rs485_Recv_Test(unsigned int baudRate)
// {
//     /* 初始化RS485 */
//     Rs485_Init(baudRate, RS485_FIFO_8);
    
//     /* 设置接收缓冲区长度 */
//     Rs485_SetRxBuffer(100);
    
//     /* 接收数据 */
//     while(1)
//     {
//         /* 检查接收标志 */
//         if(FLAG_RS485_RX)
//         {
//             /* 闪烁LED1_RED */
//             Led_Control(LED1_RED, LED_TOGGLE);
            
//             /* 重置接收标志 */
//             FLAG_RS485_RX = 0;
//         }
//     }
// }

// /**
//  * @brief RS485双向通信测试
//  * @param baudRate: 波特率
//  * @return 无
//  */
// void Rs485_Bidirectional_Test(unsigned int baudRate)
// {
//     unsigned char sendBuffer[] = "RS485 Bidirectional Test\r\n";
//     unsigned int sendLength = sizeof(sendBuffer) - 1;
    
//     /* 初始化RS485 */
//     Rs485_Init(baudRate, RS485_FIFO_8);
    
//     /* 设置接收缓冲区长度 */
//     Rs485_SetRxBuffer(100);
    
//     /* 双向通信 */
//     while(1)
//     {
//         /* 发送数据 */
//         Rs485_SendBuffer(sendBuffer, sendLength);
        
//         /* 等待发送完成 */
//         while(!FLAG_RS485_TX);
//         FLAG_RS485_TX = 0;
        
//         /* 闪烁LED1_BLUE */
//         Led_Control(LED1_BLUE, LED_TOGGLE);
        
//         /* 检查接收标志 */
//         if(FLAG_RS485_RX)
//         {
//             /* 闪烁LED1_RED */
//             Led_Control(LED1_RED, LED_TOGGLE);
            
//             /* 重置接收标志 */
//             FLAG_RS485_RX = 0;
//         }
        
//         /* 延时 */
//         delay(1000);
//     }
// }
