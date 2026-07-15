/**
 * @file rs485_api.h
 * @brief RS485用户接口文件
 * @details 定义RS485的用户接口和使用案例，为用户提供统一的RS485操作入口。
 * @ingroup RS485_API
 */

#ifndef _RS485_API_H_
#define _RS485_API_H_

/**
 * @defgroup RS485_API RS485用户接口
 * @ingroup RS485
 * @brief RS485用户接口模块
 * @details 
 * 定义RS485的用户接口，为用户提供统一的RS485操作入口。
 * 
 * 主要包含以下内容：
 * - 波特率宏定义：定义常用的RS485波特率
 * - FIFO触发级别宏定义：定义RS485 FIFO的触发级别
 * - 传输模式宏定义：定义RS485的传输模式（接收/发送）
 * - 缓冲区变量：定义RS485的发送和接收缓冲区
 * - 初始化函数：初始化RS485，设置波特率和FIFO触发级别
 * - 数据传输函数：发送数据到RS485和设置接收缓冲区
 * - 模式控制函数：设置RS485的传输模式
 * 
 * RS485_API模块包含以下子模块：
 * - @ref RS485_DRV ：RS485设备驱动模块，实现RS485的初始化和数据传输逻辑
 * - @ref RS485_PIN ：RS485引脚控制模块，负责RS485引脚的初始化和配置
 * - @ref UART1_DRV ：UART1设备驱动模块，提供RS485的串行通信支持
 * - @ref UART1_PIN ：UART1引脚控制模块，负责UART1引脚的初始化和配置
 * 
 * 用户通过此模块可以方便地使用RS485功能，实现数据的发送和接收。
 * @{
 */

/**
 * @name RS485 波特率宏定义
 * @{
 * 这些宏定义了RS485模块支持的波特率。
 */
#define RS485_BAUD_9600      9600      //!< 9600波特率
#define RS485_BAUD_115200    115200    //!< 115200波特率
/** @} */

/**
 * @name RS485 FIFO触发级别宏定义
 * @{
 * 这些宏定义了RS485模块支持的FIFO触发级别。
 */
#define RS485_FIFO_1   1    //!< FIFO触发级别1
#define RS485_FIFO_4   4    //!< FIFO触发级别4
#define RS485_FIFO_8   8    //!< FIFO触发级别8
#define RS485_FIFO_14  14   //!< FIFO触发级别14
/** @} */

/**
 * @name RS485 传输模式宏定义
 * @{
 * 这些宏定义了RS485的传输模式。
 */
#define RS485_MODE_RX    0    //!< 接收模式
#define RS485_MODE_TX    1    //!< 发送模式
/** @} */

/**
 * @name RS485 标志位
 * @{
 * 这些是用于控制RS485状态和同步的标志位。
 */
/** @brief 接收缓冲区长度 */
extern volatile unsigned char Uart1_rxLength;
/** @brief 接收缓冲区计数 */
extern volatile unsigned char Uart1_rxCount;
/** @brief 接收缓冲区标志位 */
extern volatile unsigned char FLAG_UART1_RX;
/** @brief 接收缓冲区CTI标志位 */
extern volatile unsigned char FLAG_UART1_RX_CTI;
/** @brief 发送缓冲区长度 */
extern volatile unsigned char Uart1_txLength;
/** @brief 发送缓冲区计数 */
extern volatile unsigned char Uart1_txCount;
/** @brief 发送缓冲区标志位 */
extern volatile unsigned char FLAG_UART1_TX;
/** @} */

/**
 * @name RS485 缓冲区
 * @{
 * 这些是用于存储RS485数据的缓冲区。
 */
/** @brief 发送缓冲区 */
extern volatile unsigned char Uart1_txBuffer[32];
/** @brief 接收缓冲区 */
extern volatile unsigned char Uart1_rxBuffer[32];
/** @} */

/**
 * @name RS485 函数声明
 * @{
 */
/**
 * @brief 初始化RS485
 * @details 初始化RS485，设置波特率和FIFO触发级别。
 * @param baudRate 波特率
 * @param fifoLevel FIFO触发级别
 */
void Rs485_Init(unsigned int baudRate, unsigned int fifoLevel);

/**
 * @brief 简化版RS485初始化
 * @details 简化版RS485初始化，使用默认的FIFO触发级别。
 * @param baudRate 波特率
 */
void Rs485_Init_Lite(unsigned int baudRate);

/**
 * @brief 发送数据到RS485
 * @details 发送数据到RS485发送缓冲区，等待发送完成。
 * @param buffer 数据缓冲区
 * @param length 数据长度
 */
void Rs485_SendBuffer(unsigned char *buffer, unsigned int length);

/**
 * @brief 设置RS485接收缓冲区长度
 * @details 设置RS485接收缓冲区的长度，用于接收数据。
 * @param length 缓冲区长度
 */
void Rs485_SetRxBuffer(unsigned int length);

/**
 * @brief 设置RS485传输模式
 * @details 设置RS485的传输模式，切换接收或发送模式。
 * @param mode 传输模式 (RS485_MODE_RX 或 RS485_MODE_TX)
 */
void Rs485_SetMode(unsigned int mode);

/** @} */

/** @} */

#endif /* _RS485_API_H_ */

/**
 * @defgroup RS485 RS485模块
 * @brief RS485完整功能模块
 * @details 
 * RS485模块为用户提供统一的RS485控制接口，支持RS485的初始化、数据发送和接收。
 * 
 * RS485模块包含以下子模块：
 * - @ref RS485_API ：RS485用户接口模块，包含设备驱动和引脚控制
 * - @ref RS485_EXAMPLE ：RS485应用示例模块，提供RS485使用示例。建议参考其中的代码，循序渐进的学习。
 * 
 * @par 快速使用
 * 最基础的RS485数据收发流程如下。
 * （不够循序渐进，可能后续再做完善。）
 * 
 * @code{.c}
 * #include "rs485_api.h"   // RS485用户接口
 * #include "system.h"      // 中断和延时初始化
 * int main(void)
 * {
 *     unsigned char sendData[] = "Hello, RS485!";
 *     unsigned char txBuffer2[] = "Uart1_rxBuffer: ";
 *     unsigned char txBuffer3[] = "\n\r";
 * 
 *     Sys_Init();          // 初始化系统
 *     // 初始化RS485，设置为9600波特率，FIFO触发级别为8
 *     Rs485_Init(RS485_BAUD_9600, RS485_FIFO_8); 
 * 
 *     // 发送数据
 *     Rs485_SendBuffer(sendData, sizeof(sendData));
 * 
 *     // 等待发送完成
 *     while (!FLAG_UART1_TX);
 *     FLAG_UART1_TX = 0; // 清除发送完成标志位
 * 
 *     // 设置接收缓冲区长度为20字节
 *     Rs485_SetRxBuffer(20);
 * 
 *     for (;;)
 *     {
 *         // 检查接收标志
 *         if(FLAG_UART1_RX || FLAG_UART1_RX_CTI)
 *         {
 *             // 重置接收标志
 *             FLAG_UART1_RX = 0;
 *             FLAG_UART1_RX_CTI = 0;
 *     
 *             // 延时1ms，等待数据接收完成
 *             delay(1);
 *             // 发送接收到的数据（使用中断方式）
 *             Rs485_SendBuffer(txBuffer2, sizeof(txBuffer2));
 *             Rs485_SendBuffer((unsigned char *)Uart1_rxBuffer, 20);
 *             Rs485_SendBuffer(txBuffer3, sizeof(txBuffer3));
 *         }
 *     }
 * }
 * @endcode
 * 
 * @note 
 * 程序中部分说明：
 * -# 包含 @ref rs485_api.h 头文件，使用RS485用户接口。
 * -# 调用 @ref Rs485_Init() 初始化RS485，设置波特率(@ref RS485_BAUD_9600)和FIFO触发级别(@ref RS485_FIFO_8)。
 *      - FIFO缓冲是串口外设的可选功能，开启后可减少CPU被中断的次数。
 *      - 中断处理函数会把接收数据从FIFO中读出，之后FIFO清空等待下一次接收。
 * -# 调用 @ref Rs485_SendBuffer() 发送数据到RS485。
 *      - 该函数采用了UART1的中断发送功能，每发送一个字节后会触发发送空中断，然后CPU再发送下一个字节。节省了CPU的等待时间。
 * -# 通过检查 @ref FLAG_UART1_TX 标志位判断是否发送完成。
 *      - 当全部数据发送完成后，会将发送完成标志位设置为1，通过检查该标志位判断发送是否完成。\
 * -# 调用 @ref Rs485_SetRxBuffer() 设置接收缓冲区长度。
 *      - 接收缓冲区的功能是存储UART1接收的数据，如果存储满后会从头覆盖旧数据。
 *      - 接收缓冲区和上述的UART1的FIFO功能没有关系。
 * -# 通过检查 @ref FLAG_UART1_RX 和 @ref FLAG_UART1_RX_CTI 标志位判断是否接收到数据。
 *      - 当UART1的FIFO达到指定的触发级别时，会将FIFO中断标志位设置为1。
 *      - 当UART1的FIFO一段时间内没有接收满数据时，会将FIFO接收超时中断标志位设置为1。
 * -# 接收到的数据存储在 @ref Uart1_rxBuffer 中，计数为 @ref Uart1_rxCount 。
 *      - 接收缓冲区的内容，可以根据计数值和发送数据的长度，调用数据做应用。
 *      - 计数指的是接收缓冲区中实际存储的数据字节数。
 * -# 有两种使用方法建议。
 *      - 让接收缓冲区长度和FIFO触发级别相同，直接调用数据做应用，省去检查计数的步骤。
 *      - 让接收缓冲区长度大于FIFO触发级别，通过检查计数判断是否有数据接收。
 * 
 * @warning
 * RS485模块功能较为复杂，需要注意以下内容：
 * - 需要调用 @ref Sys_Init() 初始化系统中断。
 * - 需要选择常用的波特率，以便与通信设备一致。
 * - 发送函数 @ref Rs485_SendBuffer() 说明：
 *      - RS485是半双工通信，发送数据前需切换到发送模式，发送完成后需切换回接收模式。
 *      - 函数内部先切换为发送模式，发送完成后切换回接收模式。
 *      - 在循环中，大部分时间RS485芯片都处于接收模式，等待接收数据。
 * 
 */
//  * @todo
//  * 请读者参考 @ref RS485 完成以下步骤，实现RS485数据收发功能：
//  * -# 调用 @ref Rs485_Init() 初始化RS485，设置合适的波特率和FIFO触发级别。
//  * -# 调用 @ref Rs485_SendBuffer() 发送数据。
//  * -# 通过检查发送完成标志位 @ref FLAG_UART1_TX 判断是否发送完成。
//  * -# 调用 @ref Rs485_SetRxBuffer() 设置接收缓冲区长度。
//  * -# 通过检查 @ref FLAG_UART1_RX 和 @ref FLAG_UART1_RX_CTI 标志位判断是否接收到数据。
//  * -# 接收到的数据存储在 @ref Uart1_rxBuffer 中，长度为 @ref Uart1_rxCount 。
//  * 
