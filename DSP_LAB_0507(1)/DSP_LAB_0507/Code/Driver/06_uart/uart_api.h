/**
 * @file uart_api.h
 * @brief UART用户接口文件
 * @details 定义UART的用户接口，为用户提供统一的UART操作入口。
 * @ingroup UART_API
 */

#ifndef _UART_API_H_
#define _UART_API_H_

/**
 * @defgroup UART_API UART用户接口
 * @ingroup UART
 * @brief UART用户接口模块
 * @details 
 * 定义UART的用户接口，为用户提供统一的UART操作入口。
 * 主要包含以下内容：
 * - 波特率宏定义：定义常用的UART波特率
 * - FIFO触发级别宏定义：定义UART FIFO的触发级别
 * - 缓冲区变量：定义UART的发送和接收缓冲区
 * - 初始化函数：初始化UART，设置波特率和FIFO触发级别
 * - 数据传输函数：发送数据到UART和设置接收缓冲区
 * 
 * UART_API模块包含以下子模块：
 * - @ref UART_DRV ：UART设备驱动模块，实现UART的初始化和数据传输逻辑
 * - @ref UART_PIN ：UART引脚控制模块，负责UART引脚的初始化和配置
 * 
 * 用户通过此模块可以方便地使用UART功能，实现数据的发送和接收。
 * @{
 */


/* UART波特率宏定义 */
/** @brief 9600波特率 */
#define UART_BAUD_9600      9600
/** @brief 115200波特率 */
#define UART_BAUD_115200    115200

/* UART FIFO触发级别宏定义 */
/** @brief FIFO触发级别1 */
#define UART_FIFO_1   1
/** @brief FIFO触发级别4 */
#define UART_FIFO_4   4
/** @brief FIFO触发级别8 */
#define UART_FIFO_8   8
/** @brief FIFO触发级别14 */
#define UART_FIFO_14  14

/* 变量定义 */
/** @brief 接收缓冲区长度 */
extern volatile unsigned char Uart2_rxLength;
/** @brief 接收缓冲区计数 */
extern volatile unsigned char Uart2_rxCount;
/** @brief 接收缓冲区标志位 */
extern volatile unsigned char FLAG_UART2_RX;
/** @brief 接收缓冲区CTI标志位 */
extern volatile unsigned char FLAG_UART2_RX_CTI;
/** @brief 发送缓冲区长度 */
extern volatile unsigned char Uart2_txLength;
/** @brief 发送缓冲区计数 */
extern volatile unsigned char Uart2_txCount;
/** @brief 发送缓冲区标志位 */
extern volatile unsigned char FLAG_UART2_TX;
/** @brief 发送缓冲区 */
extern volatile unsigned char Uart2_txBuffer[];
/** @brief 接收缓冲区 */
extern volatile unsigned char Uart2_rxBuffer[];

/**
 * @brief 初始化UART2
 * @details 初始化UART2，设置波特率和FIFO触发级别。
 * @param baudRate 波特率
 * @param fifoLevel FIFO触发级别
 */
void Uart2_Init(unsigned int baudRate, unsigned int fifoLevel);

/**
 * @brief 发送数据到指定缓冲区
 * @details 发送数据到UART2发送缓冲区，等待发送完成。
 * @param buffer 数据缓冲区
 * @param length 数据长度
 */
void Uart2_SendBuffer(unsigned char *buffer, unsigned int length);

/**
 * @brief 设置接收缓冲区长度
 * @details 设置UART2接收缓冲区的长度，用于接收数据。
 * @param length 缓冲区长度
 */
void Uart2_SetRxBuffer(unsigned int length);

/**
 * @brief 简化版UART2初始化
 * @details 简化版UART2初始化，使用默认的FIFO触发级别。
 * @param baudRate 波特率
 */
void Uart2_Init_Lite(unsigned int baudRate);

/// @cond INTERNAL_MACROS
/**
 * @brief 从UART接收一个字符
 * @param baseAdd UART寄存器基地址
 * @return 接收到的字符，如果没有接收到字符则返回-1
 */
//int UARTCharGet(unsigned int baseAdd);

/**
 * @brief 向UART发送一个字符
 * @param baseAdd UART寄存器基地址
 * @param byteTx 要发送的字符
 */
//void UARTCharPut(unsigned int baseAdd, unsigned char byteTx);

/**
 * @brief 向UART发送字符串
 * @param pTxBuffer 要发送的字符串
 * @param numBytesToWrite 要发送的字节数，如果为负数则发送直到遇到NULL字符
 * @return 实际发送的字节数
 */
//unsigned int UARTPuts(char *pTxBuffer, int numBytesToWrite);

/**
 * @brief 从UART接收字符串
 * @param pRxBuffer 接收缓冲区
 * @param numBytesToRead 要接收的字节数，如果为负数则接收直到遇到回车或ESC字符
 * @return 实际接收的字节数
 */
//unsigned int UARTGets(char *pRxBuffer, int numBytesToRead);

/**
 * @brief UART printf函数，支持格式化输出
 * @param pcString 格式字符串
 * @param ... 可变参数列表
 * @details 支持的格式说明符：
 * - %c: 打印字符
 * - %d: 打印十进制整数
 * - %s: 打印字符串
 * - %u: 打印无符号十进制整数
 * - %x, %X: 打印十六进制整数
 * - %p: 打印指针
 * - %%: 打印%字符
 */
//void UARTprintf(const char *pcString, ...);

/// @endcond

/** @} */

#endif /* _UART_API_H_ */

/**
 * @defgroup UART UART模块
 * @brief UART完整功能模块
 * @details 
 * UART模块为用户提供统一的UART控制接口，支持UART的初始化、数据发送和接收。
 * 
 * UART模块包含以下子模块：
 * - @ref UART_API ：UART用户接口模块，包含设备驱动和引脚控制。
 * - @ref UART_EXAMPLE ：UART应用示例模块，提供UART使用示例。建议参考其中的代码，循序渐进的学习。
 * 
 * @par 快速使用
 * 最基础的UART数据收发流程如下。
 * （不够循序渐进，可能后续再做完善。）
 * 
 * @code{.c}
 * #include "uart_api.h"    // UART用户接口
 * #include "system.h"     // 中断和延时初始化
 * int main(void)
 * {
 *     unsigned char sendData[] = "Hello, UART!";
 *     
 *     Sys_Init();         // 初始化系统
 *     // 初始化UART2，设置为115200波特率，FIFO触发级别为8
 *     Uart2_Init(UART_BAUD_115200, UART_FIFO_8); 
 * 
 *     // 发送数据
 *     Uart2_SendBuffer(sendData, sizeof(sendData));
 *         
 *     // 等待发送完成
 *     while (!FLAG_UART2_TX);
 *     FLAG_UART2_TX = 0; // 清除发送完成标志位
 * 
 *     // 设置接收缓冲区长度为20字节
 *     Uart2_SetRxBuffer(20);
 * 
 *     for (;;)
 *     {
 *          // 检查FIFO触发中断或FIFO接收超时中断
 *          if(FLAG_UART2_RX || FLAG_UART2_RX_CTI)
 *          {
 *              FLAG_UART2_RX = 0;
 *              FLAG_UART2_RX_CTI = 0;
 * 
 *              // 打印接收到的数据
 *              UARTprintf("Uart2_rxBuffer: ");
 *              Uart2_SendBuffer((unsigned char *)Uart2_rxBuffer, 20);
 *              UARTprintf("\n\r");
 *          }
 *     }
 * }
 * @endcode
 * 
 * @note 
 * 程序中部分说明：
 * -# 包含 @ref uart_api.h 头文件，使用UART用户接口。
 * -# 调用 @ref Uart2_Init() 初始化UART，设置波特率(@ref UART_BAUD_115200)和FIFO触发级别(@ref UART_FIFO_8)。
 *      - FIFO缓冲是串口外设的可选功能，开启后可减少CPU被中断的次数。
 *      - 中断处理函数会把接收数据从FIFO中读出，之后FIFO清空等待下一次接收。
 * -# 调用 @ref Uart2_SendBuffer() 发送数据到UART2。
 *      - 该函数采用了UART2的中断发送功能，每发送一个字节后会触发发送空中断，然后CPU再发送下一个字节。节省了CPU的等待时间。
 * -# 通过检查 @ref FLAG_UART2_TX 标志位判断是否发送完成。
 *      - 当全部数据发送完成后，会将发送完成标志位设置为1，通过检查该标志位判断发送是否完成。
 * -# 调用 @ref Uart2_SetRxBuffer() 设置接收缓冲区 @ref Uart2_rxLength 长度。
 *      - 接收缓冲区长度默认为32字节。
 *      - 接收缓冲区的功能是存储UART2接收的数据，如果存储满后会从头覆盖旧数据，详细需要参考 @ref UART2_DRV 。
 *      - 接收缓冲区和上述的UART2的FIFO功能没有关系。
 * -# 通过检查 @ref FLAG_UART2_RX 和 @ref FLAG_UART2_RX_CTI 标志位判断是否接收到数据。
 *      - 当UART2的FIFO达到指定的触发级别时，会将FIFO中断标志位设置为1。
 *      - 当UART2的FIFO一段时间内没有接收满数据时，会将FIFO接收超时中断标志位设置为1。
 * -# 接收到的数据存储在 @ref Uart2_rxBuffer 中，计数为 @ref Uart2_rxCount 。
 *      - 接收缓冲区的内容，可以根据计数值和发送数据的长度，调用数据做应用。
 *      - 计数指的是接收缓冲区中实际存储的数据字节数。
 * -# 有两种使用方法建议。
 *      - 让接收缓冲区长度和FIFO触发级别相同，直接调用数据做应用，省去检查计数的步骤。
 *      - 让接收缓冲区长度大于FIFO触发级别，通过检查计数判断是否有数据接收。
 * 
 * @warning
 * UART模块功能较为复杂，需要注意以下内容：
 * - 需要调用 @ref Sys_Init() 初始化系统中断。
 * - 需要选择常用的波特率，以便与通信设备一致。
 * - 该模块功能仅为作者根据个人经验实现，读者可在学习透彻后根据实际情况进行调整。
 * 
 * @todo
 * 请读者参考 @ref UART 完成以下步骤，实现UART数据收发功能：
 * -# 调用 @ref Uart2_Init() 初始化UART2，设置合适的波特率和FIFO触发级别。
 * -# 调用 @ref Uart2_SendBuffer() 发送数据。
 * -# 通过检查发送完成标志位 @ref FLAG_UART2_TX 判断是否发送是否完成。
 * -# 调用 @ref Uart2_SetRxBuffer() 设置接收缓冲区长度。
 * -# 通过检查 @ref FLAG_UART2_RX 和 @ref FLAG_UART2_RX_CTI 标志位判断是否接收到数据。
 * -# 接收到的数据存储在 @ref Uart2_rxBuffer 中，长度为 @ref Uart2_rxCount 。
 * 
 */
