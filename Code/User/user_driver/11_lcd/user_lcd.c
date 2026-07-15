/**
 * @file user_lcd.c
 * @brief LCD用户使用实现文件
 * @details 实现LCD的用户示例函数，展示如何初始化和使用LCD。
 * @ingroup LCD_EXAMPLE
 */

#include "user_lcd.h"

#include "system.h"
#include "key_api.h"
#include "lcd_api.h"

#include <stdio.h>



void Lcd_Draw_Test(void);
void Lcd_Font_Test(void);
void Lcd_Show_Num(unsigned char num);
void Lcd_Clear_Test(void);
void Lcd_Btn_Test(void);


// LCD使用示例
void LCD_Example1(void)
{
    unsigned char number;
    // 初始化
    Sys_Init();
    // 初始化按键
    Key_Init();
    // 初始化LCD
    Lcd_Init();

    for (;;)
    {
        if (FLAG_KEY1)
        {
            FLAG_KEY1 = 0;
            Lcd_Draw_Test();
        }
        if (FLAG_KEY2)
        {
            FLAG_KEY2 = 0;
            Lcd_Font_Test();
        }
        if (FLAG_KEY3)
        {
            FLAG_KEY3 = 0;
            number++;
            Lcd_Show_Num(number);
        }
        if (FLAG_KEY4)
        {
            FLAG_KEY4 = 0;
            number--;
            Lcd_Show_Num(number);
        }
        if (FLAG_KEY5)
        {
            FLAG_KEY5 = 0;
            Lcd_Clear_Test();
        }
    }

}

void Lcd_Draw_Test(void)
{
    // 设置绘图的颜色
    GrContextForegroundSet(&Lcd_Context, ClrWhite);
    // 绘制空心圆和实心圆
    GrCircleDraw(&Lcd_Context,100,240,80);
    GrCircleFill(&Lcd_Context,300,240,80);
    // 绘制实心矩形和空心矩形
    Lcd_Rectangle.sXMin = 420;
    Lcd_Rectangle.sYMin = 160;
    Lcd_Rectangle.sXMax = 580;
    Lcd_Rectangle.sYMax = 320;
    GrContextForegroundSet(&Lcd_Context, ClrSkyBlue);
    GrRectFill(&Lcd_Context, &Lcd_Rectangle);
    GrContextForegroundSet(&Lcd_Context, ClrWhite);
    GrRectDraw(&Lcd_Context, &Lcd_Rectangle);
    // 绘制线条
    GrLineDraw(&Lcd_Context,650,100,750,100);
    GrLineDraw(&Lcd_Context,750,100,750,380);
}

// 测试字体显示情况
void Lcd_Font_Test(void)
{
    GrContextForegroundSet(&Lcd_Context, ClrWhite);

    // b-bold-粗体 i-italic-斜体
    // 1. 衬线字体 (Serif) - Computer Modern
    GrContextFontSet(&Lcd_Context, &g_sFontCm12 );      GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 100, 30,  0);
    GrContextFontSet(&Lcd_Context, &g_sFontCm18b);      GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 100, 90,  0);
    GrContextFontSet(&Lcd_Context, &g_sFontCm24i);      GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 100, 150, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCm30 );      GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 100, 210, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCm36b);      GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 100, 270, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCm42i);      GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 100, 330, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCm48 );      GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 100, 390, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCm32 );      GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 100, 450, 0);
    
    // 2. 小型大写字母衬线字体 (Small Caps) - Computer Modern Small Caps
    GrContextFontSet(&Lcd_Context, &g_sFontCmsc12);     GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 300, 30,  0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmsc18);     GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 300, 90,  0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmsc24);     GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 300, 150, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmsc30);     GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 300, 210, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmsc36);     GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 300, 270, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmsc42);     GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 300, 330, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmsc48);     GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 300, 390, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmsc32);     GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 300, 450, 0);
    
    // 3. 无衬线字体 (Sans-Serif) - Computer Modern Sans-Serif
    GrContextFontSet(&Lcd_Context, &g_sFontCmss12 );    GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 500, 30,  0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmss18b);    GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 500, 90,  0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmss24i);    GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 500, 150, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmss30 );    GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 500, 210, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmss36b);    GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 500, 270, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmss42i);    GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 500, 330, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmss48 );    GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 500, 390, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmss32 );    GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 500, 450, 0);
    
    // 3. 等宽字体 (Monospace) - Computer Modern Typewriter
    GrContextFontSet(&Lcd_Context, &g_sFontCmtt12);     GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 700, 30,  0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmtt18);     GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 700, 90,  0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmtt24);     GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 700, 150, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmtt30);     GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 700, 210, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmtt36);     GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 700, 270, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmtt42);     GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 700, 330, 0);
    GrContextFontSet(&Lcd_Context, &g_sFontCmtt48);     GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 700, 390, 0);
    
    // 5. 固定尺寸等宽字体
    GrContextFontSet(&Lcd_Context, &g_sFontFixed6x8);   GrStringDrawCentered(&Lcd_Context, "Font Test", -1, 700, 450, 0);


}

void Lcd_Show_Num(unsigned char num)
{
    char Buffer[8];
    static unsigned char num_old;
    
    // 设置一个测试字体
    GrContextFontSet(&Lcd_Context, &g_sFontCm48 );
    
    GrContextForegroundSet(&Lcd_Context, ClrBlack);
    sprintf(Buffer, "Num: %d", num_old);
    GrStringDrawCentered(&Lcd_Context, Buffer, -1, 400, 240, 0);

    GrContextForegroundSet(&Lcd_Context, ClrWhite);
    sprintf(Buffer, "Num: %d", num);
    GrStringDrawCentered(&Lcd_Context, Buffer, -1, 400, 240, 0);

    num_old = num;
}

void Lcd_Clear_Test(void)
{
    // 清除LCD
    Lcd_Rectangle.sXMin = 0;
    Lcd_Rectangle.sYMin = 0;
    Lcd_Rectangle.sXMax = 800;
    Lcd_Rectangle.sYMax = 480;
    GrContextForegroundSet(&Lcd_Context, ClrBlack);
    GrRectFill(&Lcd_Context, &Lcd_Rectangle);
}




// LCD使用示例2
// 剩余控件演示在触摸中展示
void LCD_Example2(void)
{
    // 初始化
    Sys_Init();
    // 初始化按键
    Key_Init();
    // 初始化LCD
    Lcd_Init();
    // 控件初始化
    Lcd_Widget_Init();
    
    for(;;)
    {
        if (FLAG_KEY1)
        {
            FLAG_KEY1 = 0;
            Lcd_Btn_Test();
        }
    }
}


void Lcd_Btn_Test(void)
{
    char Buffer[64];
    PushButtonTextSet(&g_sBtn1, "X-");
    PushButtonTextSet(&g_sBtn2, "X+");
    PushButtonTextSet(&g_sBtn3, "Y-");
    PushButtonTextSet(&g_sBtn4, "Y+");
    PushButtonTextSet(&g_sBtn5, "T-");
    PushButtonTextSet(&g_sBtn6, "T+");
    PushButtonTextSet(&g_sBtn7, "C-");
    PushButtonTextSet(&g_sBtn8, "C+");
    WidgetPaint((tWidget *)&g_sBtn1);
    WidgetPaint((tWidget *)&g_sBtn2);
    WidgetPaint((tWidget *)&g_sBtn3);
    WidgetPaint((tWidget *)&g_sBtn4);
    WidgetPaint((tWidget *)&g_sBtn5);
    WidgetPaint((tWidget *)&g_sBtn6);
    WidgetPaint((tWidget *)&g_sBtn7);
    WidgetPaint((tWidget *)&g_sBtn8);
    CanvasTextSet(&g_sTxt1, "Please Press Button");
    sprintf(Buffer, "scale_x:1 scale_y:2 trig_y:3 ch_num:4");
    CanvasTextSet(&g_sTxt2, Buffer);
    WidgetPaint((tWidget *)&g_sTxt1);
    WidgetPaint((tWidget *)&g_sTxt2);
    WidgetMessageQueueProcess();
}



