/**
 * @file user_flash.c
 * @brief Flash用户使用实现文件
 * @details 实现Flash的用户示例函数，展示如何初始化和使用Flash。
 * @ingroup FLASH_EXAMPLE
 */

#include "user_flash.h"
#include "system.h"
#include "uart_api.h"
#include "flash_api.h"
#include "delay.h"

/* 函数定义 */
// Flash示例函数
// 演示Flash的基本操作流程，包括初始化、擦除、写入、读取和校验。
// 操作步骤:
// 1. 初始化系统
// 2. 初始化Flash
// 3. 执行Flash测试
// 4. 测试完成后进入循环
void Flash_Example(void)
{
    /* 1. 初始化系统
     *  - 初始化中断控制器
     *  - 使能全局中断
     *  - 初始化延时功能
     */
    Sys_Init();
    
    /* 初始化UART，波特率115200 */
    Uart2_Init_Lite(UART_BAUD_115200);

    /* 2. 初始化Flash
     *  - 初始化SPI接口
     *  - 配置Flash相关参数
     */
    Flash_Init();
    
    /* 3. 执行Flash测试
     *  - 写使能
     *  - 扇区擦除
     *  - 写入数据
     *  - 读取数据
     *  - 校验数据
     */
    Flash_Test();
    
    /* 4. 测试完成后进入循环
     *  - 可以在这里添加其他Flash操作
     */
    while(1)
    {
        // 可以添加其他Flash操作
        delay(1000);
    }
}
