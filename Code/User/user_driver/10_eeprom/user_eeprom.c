/**
 * @file user_eeprom.c
 * @brief EEPROM用户使用实现文件
 * @details 实现EEPROM的用户示例函数，展示如何初始化和使用EEPROM。
 * @ingroup EEPROM_EXAMPLE
 */

#include "user_eeprom.h"
#include "system.h"
#include "uart_api.h"
#include "eeprom_api.h"
#include "delay.h"

/* 函数定义 */
// EEPROM示例函数
// 演示EEPROM的基本操作流程，包括初始化、单字节读写、当前地址读取和页读写。
// 操作步骤:
// 1. 初始化系统
// 2. 初始化EEPROM
// 3. 执行EEPROM测试
// 4. 测试完成后进入循环
void EEPROM_Example(void)
{
    /* 1. 初始化系统
     *  - 初始化中断控制器
     *  - 使能全局中断
     *  - 初始化延时功能
     */
    Sys_Init();
    
    /* 初始化UART，波特率115200 */
    Uart2_Init_Lite(UART_BAUD_115200);
    /* 2. 初始化EEPROM
     *  - 初始化IIC接口
     *  - 配置EEPROM相关参数
     */
    EEPROM_Init();
    
    /* 3. 执行EEPROM测试
     *  - 单字节写入和读取
     *  - 当前地址读取
     *  - 页写入和顺序读取
     *  - 数据校验
     */
    EEPROM_Test();
    
    /* 4. 测试完成后进入循环
     *  - 可以在这里添加其他EEPROM操作
     */
    while(1)
    {
        // 可以添加其他EEPROM操作
        delay(1000);
    }
}
