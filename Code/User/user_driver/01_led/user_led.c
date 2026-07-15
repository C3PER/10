/**
 * @file user_led.c
 * @brief LED应用示例实现文件
 * @details 实现LED应用示例，提供LED使用示例。
 * @ingroup LED_EXAMPLE
 */

/* 头文件包含 */
#include "led_api.h"            // LED用户接口

#include "system.h"             // 中断和延时初始化
#include "delay.h"              // 延时函数


/* 函数定义 */
// LED使用示例
void Led_Example(void)
{
    // 初始化delay函数
    Sys_Init();
    // 初始化LED
    Led_Init();

    for (;;)
    {
        // 点亮核心板LED，翻转开发板蓝色LED
        Led_Control(LED1_CORE, LED_ON);
        Led_Control(LED2_CORE, LED_ON);
        Led_Control(LED1_BLUE, LED_TOGGLE);
        Led_Control(LED2_BLUE, LED_TOGGLE);
        Led_Control(LED3_BLUE, LED_TOGGLE);
        Led_Control(LED4_BLUE, LED_TOGGLE);
        delay(500);

        // 关闭核心板LED，翻转开发板红色LED
        Led_Control(LED1_CORE, LED_OFF);
        Led_Control(LED2_CORE, LED_OFF);
        Led_Control(LED1_RED,  LED_TOGGLE);
        Led_Control(LED2_RED,  LED_TOGGLE);
        Led_Control(LED3_RED,  LED_TOGGLE);
        Led_Control(LED4_RED,  LED_TOGGLE);
        delay(500);
    }
}

