/**
 * @file user_touch.c
 * @brief Touch用户使用实现文件
 * @details 实现Touch的用户示例函数，展示如何初始化和使用Touch。
 * @ingroup TOUCH_EXAMPLE
 */

/* 头文件包含 */
#include "user_touch.h"           // Touch用户使用接口
#include "system.h"               // 系统初始化
#include "led_api.h"              // LED用户接口
#include "lcd_api.h"              // LCD用户接口
#include "touch_api.h"            // Touch用户接口

#include <stdio.h>                // 标准输入输出

#include "grlib.h"                // 提供图形库核心API
#include "widget.h"               // 提供控件基础框架
#include "pushbutton.h"           // 提供按钮控件


// 声明外部UI控件
extern tCanvasWidget        g_sHeading;
extern tCanvasWidget        g_sTxt1;
extern tCanvasWidget        g_sTxt2;
extern tPushButtonWidget    g_sBtn1;
extern tPushButtonWidget    g_sBtn2;
extern tPushButtonWidget    g_sBtn3;
extern tPushButtonWidget    g_sBtn4;
extern tPushButtonWidget    g_sBtn5;
extern tPushButtonWidget    g_sBtn6;
extern tPushButtonWidget    g_sBtn7;
extern tPushButtonWidget    g_sBtn8;


/* 函数定义 */
// Touch示例函数
void Touch_Example(void)
{
    char Buffer[64];
    
    // 初始化系统
    Sys_Init();
    Led_Init();
    Lcd_Init();
    Lcd_Widget_Init();
    Touch_Init();
    
    // 主循环
    for(;;)
    {
        // 检测触摸中断
        if(FLAG_TOUCH)
        {
            FLAG_TOUCH = 0;
            Touch_Scan();
        }

        // 处理按钮1事件
        if(FLAG_BUTTON_1)
        {
            FLAG_BUTTON_1 = 0;
            Led_Control(LED1_BLUE, LED_TOGGLE);
            CanvasTextSet(&g_sTxt1, "Button_1_Press");
            WidgetPaint((tWidget *)&g_sTxt1);
            CanvasTextSet(&g_sTxt2, "Button_1_Press");
            WidgetPaint((tWidget *)&g_sTxt2);
            WidgetMessageQueueProcess();
        }
        
        // 处理按钮2事件
        if(FLAG_BUTTON_2)
        {
            FLAG_BUTTON_2 = 0;
            Led_Control(LED1_RED, LED_TOGGLE);
            CanvasTextSet(&g_sTxt1, "Button_2_Press");
            WidgetPaint((tWidget *)&g_sTxt1);
            CanvasTextSet(&g_sTxt2, "Button_2_Press");
            WidgetPaint((tWidget *)&g_sTxt2);
            WidgetMessageQueueProcess();
        }
        
        // 处理按钮3事件
        if(FLAG_BUTTON_3)
        {
            FLAG_BUTTON_3 = 0;
            Led_Control(LED2_BLUE, LED_TOGGLE);
            CanvasTextSet(&g_sTxt1, "Button_3_Press");
            WidgetPaint((tWidget *)&g_sTxt1);
            CanvasTextSet(&g_sTxt2, "Button_3_Press");
            WidgetPaint((tWidget *)&g_sTxt2);
            WidgetMessageQueueProcess();
        }
        
        // 处理按钮4事件
        if(FLAG_BUTTON_4)
        {
            FLAG_BUTTON_4 = 0;
            Led_Control(LED2_RED, LED_TOGGLE);
            CanvasTextSet(&g_sTxt1, "Button_4_Press");
            WidgetPaint((tWidget *)&g_sTxt1);
            CanvasTextSet(&g_sTxt2, "Button_4_Press");
            WidgetPaint((tWidget *)&g_sTxt2);
            WidgetMessageQueueProcess();
        }
        
        // 处理按钮5事件
        if(FLAG_BUTTON_5)
        {
            FLAG_BUTTON_5 = 0;
            Led_Control(LED3_BLUE, LED_TOGGLE);
            sprintf(Buffer, "F1:%d F3:%d F5:%d F7:%d", FLAG_BUTTON_1, FLAG_BUTTON_3, FLAG_BUTTON_5, FLAG_BUTTON_7);
            CanvasTextSet(&g_sTxt2, Buffer);
            WidgetPaint((tWidget *)&g_sTxt2);
            WidgetMessageQueueProcess();
        }
        
        // 处理按钮6事件
        if(FLAG_BUTTON_6)
        {
            FLAG_BUTTON_6 = 0;
            Led_Control(LED3_RED, LED_TOGGLE);
            sprintf(Buffer, "F2:%d F4:%d F6:%d F8:%d", FLAG_BUTTON_2, FLAG_BUTTON_4, FLAG_BUTTON_6, FLAG_BUTTON_8);
            CanvasTextSet(&g_sTxt2, Buffer);
            WidgetPaint((tWidget *)&g_sTxt2);
            WidgetMessageQueueProcess();
        }
        
        // 处理按钮7事件
        if(FLAG_BUTTON_7)
        {
            FLAG_BUTTON_7 = 0;
            Led_Control(LED4_BLUE, LED_TOGGLE);
        }
        
        // 处理按钮8事件
        if(FLAG_BUTTON_8)
        {
            FLAG_BUTTON_8 = 0;
            Led_Control(LED4_RED, LED_TOGGLE);
        }
    }
}
