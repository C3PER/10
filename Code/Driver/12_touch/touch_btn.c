/**
 * @file touch_btn.c
 * @brief Touch按钮控制实现文件
 * @details 实现Touch按钮的响应处理和标志位更新。
 * @ingroup TOUCH_BUTTON
 */

/* 头文件包含 */
#include "touch_btn.h"           // Touch按钮

#include "grlib.h"               // 提供图形库核心API
#include "widget.h"              // 提供控件基础框架
#include "pushbutton.h"          // 提供按钮控件


extern tPushButtonWidget    g_sBtn1;
extern tPushButtonWidget    g_sBtn2;
extern tPushButtonWidget    g_sBtn3;
extern tPushButtonWidget    g_sBtn4;
extern tPushButtonWidget    g_sBtn5;
extern tPushButtonWidget    g_sBtn6;
extern tPushButtonWidget    g_sBtn7;
extern tPushButtonWidget    g_sBtn8;

// 按钮控制标志位
volatile unsigned char FLAG_BUTTON;
volatile unsigned char FLAG_BUTTON_1;
volatile unsigned char FLAG_BUTTON_2;
volatile unsigned char FLAG_BUTTON_3;
volatile unsigned char FLAG_BUTTON_4;
volatile unsigned char FLAG_BUTTON_5;
volatile unsigned char FLAG_BUTTON_6;
volatile unsigned char FLAG_BUTTON_7;
volatile unsigned char FLAG_BUTTON_8;


/* 函数定义 */
// 按钮控件响应
void Widget_Btn_App(unsigned char Touch_Sta, unsigned int Touch_X, unsigned int Touch_Y)
{
    static unsigned char Flag_Touch = 0;
    static unsigned int Old_X, Old_Y = 0;
    if((Touch_Sta == 1) && (Flag_Touch == 0))
    {
        WidgetPointerMessage(WIDGET_MSG_PTR_DOWN, Touch_X, Touch_Y);
        WidgetMessageQueueProcess();
        Old_X = Touch_X; Old_Y = Touch_Y; 
        Flag_Touch = 1;
    } 
    else if((Touch_Sta == 0) && (Flag_Touch == 1))
    {
        WidgetPointerMessage(WIDGET_MSG_PTR_UP, Old_X, Old_Y);
        WidgetMessageQueueProcess();
        Flag_Touch = 0;
    }
}

// 通用按钮回调处理函数
void OnButtonPress(tWidget *pWidget)
{
    FLAG_BUTTON = 1;
    
    // 通过比较指针地址来判断是哪个控件
    if(pWidget == (tWidget *)&g_sBtn1)
    {
        FLAG_BUTTON_1 = 1;
    }
    else if(pWidget == (tWidget *)&g_sBtn2)
    {
        FLAG_BUTTON_2 = 1;
    }
    else if(pWidget == (tWidget *)&g_sBtn3)
    {
        FLAG_BUTTON_3 = 1;
    }
    else if(pWidget == (tWidget *)&g_sBtn4)
    {
        FLAG_BUTTON_4 = 1;
    }
    else if(pWidget == (tWidget *)&g_sBtn5)
    {
        FLAG_BUTTON_5 = 1;
    }
    else if(pWidget == (tWidget *)&g_sBtn6)
    {
        FLAG_BUTTON_6 = 1;
    }
    else if(pWidget == (tWidget *)&g_sBtn7)
    {
        FLAG_BUTTON_7 = 1;
    }
    else if(pWidget == (tWidget *)&g_sBtn8)
    {
        FLAG_BUTTON_8 = 1;
    }
}

