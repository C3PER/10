/**
 * @file user_timer.c
 * @brief 用户层定时器操作实现文件
 * @details 实现用户层定时器使用示例，展示如何初始化和使用定时器。
 * @ingroup TIMER_EXAMPLE
 */

/* 头文件包含 */
#include "timer_api.h"          // 定时器用户接口
#include "led_api.h"            // LED用户接口

#include "system.h"             // 中断和延时初始化


/* 函数定义 */
// 定时器使用示例
void Timer_Example(void)
{
    // 初始化delay函数
    Sys_Init();
    // 初始化LED
    Led_Init();
    
    // 初始化定时器，设置为1Hz（1秒）
    Tim2_Init(TIMER_1HZ);

    for (;;)
    {
        // 检查定时器标志位是否为1
        if (FLAG_TIM2)
        {
            // 清除定时器标志位
            FLAG_TIM2 = 0;
            // 每秒钟切换所有LED的状态
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

// 定时器使用示例2：多任务执行
void Timer_Example2(void)
{
    // 定义任务计数器
    unsigned int task1_count = 0;
    unsigned int task2_count = 0;

    // 初始化系统（开启中断）
    Sys_Init();
    // 初始化LED
    Led_Init();
    // 初始化定时器，设置为100Hz（10ms）
    Tim2_Init(100);

    for (;;)
    {
        // 检查定时器标志位
        if (FLAG_TIM2)
        {
            FLAG_TIM2 = 0;
            
            // 任务1：每1秒执行一次
            task1_count++;
            if (task1_count >= 100)  // 100 * 10ms = 1s
            {
                task1_count = 0;
                Led_Control(LED1_BLUE, LED_TOGGLE);
                Led_Control(LED1_RED, LED_TOGGLE);
            }
            
            // 任务2：每500ms执行一次
            task2_count++;
            if (task2_count >= 50)  // 50 * 10ms = 500ms
            {
                task2_count = 0;
                Led_Control(LED2_BLUE, LED_TOGGLE);
                Led_Control(LED2_RED, LED_TOGGLE);
            }
        }
    }
}
