/**
 * @file user_key.c
 * @brief 按键应用示例实现文件
 * @details 实现按键应用示例，展示如何使用按键驱动接口。
 * @ingroup KEY_EXAMPLE
 */

/* 头文件包含 */
#include "key_api.h"            // 按键用户接口
#include "led_api.h"            // LED用户接口

#include "system.h"             // 中断和延时初始化
#include "delay.h"              // 延时函数


/* 函数定义 */
// 按键使用示例
void Key_Example(void)
{
    // 初始化delay函数
    Sys_Init();
    // 初始化LED
    Led_Init();
    // 初始化按键
    Key_Init();

    for (;;)
    {
        // 检查按键标志位是否为1
        if (FLAG_KEY1)
        {
            // 将按键标志位设置为0
            FLAG_KEY1 = 0;
            Led_Control(LED1_BLUE, LED_TOGGLE);
            Led_Control(LED1_RED, LED_TOGGLE);
        }
        if (FLAG_KEY2)
        {
            FLAG_KEY2 = 0;
            Led_Control(LED2_BLUE, LED_TOGGLE);
            Led_Control(LED2_RED, LED_TOGGLE);
        }
        if (FLAG_KEY3)
        {
            FLAG_KEY3 = 0;
            Led_Control(LED3_BLUE, LED_TOGGLE);
            Led_Control(LED3_RED, LED_TOGGLE);
        }
        if (FLAG_KEY4)
        {
            FLAG_KEY4 = 0;
            Led_Control(LED4_BLUE, LED_TOGGLE);
            Led_Control(LED4_RED, LED_TOGGLE);
        }
        if (FLAG_KEY5)
        {
            FLAG_KEY5 = 0;
            Led_Control(LED1_CORE, LED_TOGGLE);
            Led_Control(LED2_CORE, LED_TOGGLE);
            Led_Control(LED1_BLUE, LED_TOGGLE);
            Led_Control(LED1_RED, LED_TOGGLE);
            Led_Control(LED2_BLUE, LED_TOGGLE);
            Led_Control(LED2_RED, LED_TOGGLE);
            Led_Control(LED3_BLUE, LED_TOGGLE);
            Led_Control(LED3_RED, LED_TOGGLE);
            Led_Control(LED4_BLUE, LED_TOGGLE);
            Led_Control(LED4_RED, LED_TOGGLE);
        }
    }
}

// 按键使用示例2
void Key_Example2(void)
{
    // 定义模式标志位
    unsigned char MODE_KEY1=0;

    // 初始化delay函数
    Sys_Init();
    // 初始化LED
    Led_Init();
    // 初始化按键
    Key_Init();
    
    for (;;)
    {
        // 使用按键切换模式
        if(FLAG_KEY1)
        {
            FLAG_KEY1 = 0;
            // 切换模式
            MODE_KEY1 = (MODE_KEY1 + 1) % 2;
        }
        // 不同模式执行不同函数
        if(MODE_KEY1){
            Led_Control(LED1_BLUE, LED_TOGGLE);
            Led_Control(LED2_BLUE, LED_TOGGLE);
            Led_Control(LED3_BLUE, LED_TOGGLE);
            Led_Control(LED4_BLUE, LED_TOGGLE);
        }
        else{
            Led_Control(LED1_RED, LED_TOGGLE);
            Led_Control(LED2_RED, LED_TOGGLE);
            Led_Control(LED3_RED, LED_TOGGLE);
            Led_Control(LED4_RED, LED_TOGGLE);
        }
        delay(500);
    }
}
