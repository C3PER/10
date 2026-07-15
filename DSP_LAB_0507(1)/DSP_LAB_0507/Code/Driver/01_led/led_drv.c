/**
 * @file led_drv.c
 * @brief LED设备驱动实现文件
 * @details 实现LED驱动的核心功能，包括初始化和控制LED。
 * @ingroup LED_DRV
 */

/* 头文件包含 */
#include "led_api.h"            // LED用户接口
#include "led_drv.h"            // LED设备驱动
#include "led_pin.h"            // LED引脚控制


/* 变量声明 */
// 映射LED索引到实际引脚编号
static const unsigned int ledPinTable[LED_MAX] = {
    LED1_BLUE_PIN, LED1_RED_PIN, 
    LED2_BLUE_PIN, LED2_RED_PIN, 
    LED3_BLUE_PIN, LED3_RED_PIN, 
    LED4_BLUE_PIN, LED4_RED_PIN, 
    LED1_CORE_PIN, LED2_CORE_PIN  
};


/* 函数定义 */
// 初始化LED驱动
void Led_Init(void) 
{
    // 调用底层LED_PinInit函数初始化GPIO引脚
    LED_PinInit();
}

// 控制LED状态
void Led_Control(LedIndex ledNumber, LedState ledState) 
{
    // 根据指定的LED索引和状态控制LED
    if (ledNumber < LED_MAX) 
    {
        unsigned int pinNumber = ledPinTable[ledNumber];
        switch(ledState) 
        {
            case LED_ON:     LED_PinSet(pinNumber, LED_STA_ON);     break;
            case LED_OFF:    LED_PinSet(pinNumber, LED_STA_OFF);    break;
            case LED_TOGGLE: LED_PinSet(pinNumber, LED_STA_TOGGLE); break;
            default: break;
        }
    }
}
