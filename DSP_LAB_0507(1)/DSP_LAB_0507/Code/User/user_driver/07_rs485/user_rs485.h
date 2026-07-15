/**
 * @file user_rs485.h
 * @brief RS485用户使用头文件
 * @details 定义RS485用户使用的函数和宏，为用户提供RS485使用示例。
 * @ingroup RS485_EXAMPLE
 */

#ifndef _USER_RS485_H_
#define _USER_RS485_H_

/**
 * @defgroup RS485_EXAMPLE RS485应用示例
 * @ingroup RS485
 * @brief RS485应用示例模块
 * @details 
 * 定义RS485用户使用的函数和宏，为用户提供RS485使用示例。
 * 
 * 主要包含以下内容：
 * - RS485使用示例函数：展示如何初始化和使用RS485
 * - RS485中断使用示例函数：展示如何使用RS485的中断功能
 * 
 * 用户通过此模块可以学习如何在实际应用中使用RS485功能。
 * @{
 */

/**
 * @brief RS485用户使用示例
 * @details 展示如何初始化和使用RS485进行数据传输。
 */
void Rs485_Example(void);

/**
 * @brief RS485中断使用示例
 * @details 展示如何使用RS485的中断功能进行数据传输。
 */
void Rs485_Example_Interrupt(void);

/** @} */

// /**
//  * @brief RS485发送测试
//  * @param baudRate: 波特率
//  * @return 无
//  */
// void Rs485_Send_Test(unsigned int baudRate);

// /**
//  * @brief RS485接收测试
//  * @param baudRate: 波特率
//  * @return 无
//  */
// void Rs485_Recv_Test(unsigned int baudRate);

// /**
//  * @brief RS485双向通信测试
//  * @param baudRate: 波特率
//  * @return 无
//  */
// void Rs485_Bidirectional_Test(unsigned int baudRate);

#endif /* _USER_RS485_H_ */
