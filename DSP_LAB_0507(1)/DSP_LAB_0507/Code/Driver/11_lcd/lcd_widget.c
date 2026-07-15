/**
 * @file lcd_widget.c
 * @brief LCD控件配置实现文件
 * @details 实现LCD用户界面控件的创建和初始化。
 * @ingroup LCD_WIDGET
 */

/* 头文件包含 */
#include "lcd_widget.h"            // LCD控件
// #include "lcd_button.h"            // LCD按钮

#include "grlib.h"                 // 提供图形库核心API
#include "widget.h"                // 提供控件基础框架
#include "container.h"             // 提供容器控件
#include "canvas.h"                // 提供画布控件
#include "pushbutton.h"            // 提供按钮控件


extern tDisplay Lcd_Display;
extern void OnButtonPress(tWidget *pWidget);

tContainerWidget     g_sMainPanel;
tCanvasWidget        g_sHeading;
tCanvasWidget        g_sTxt1;
tCanvasWidget        g_sTxt2;
tPushButtonWidget    g_sBtn1;
tPushButtonWidget    g_sBtn2;
tPushButtonWidget    g_sBtn3;
tPushButtonWidget    g_sBtn4;
tPushButtonWidget    g_sBtn5;
tPushButtonWidget    g_sBtn6;
tPushButtonWidget    g_sBtn7;
tPushButtonWidget    g_sBtn8;

// 创建主容器（父节点: WIDGET_ROOT）
Container(g_sMainPanel, WIDGET_ROOT, 0, &g_sHeading,
        &Lcd_Display, PANEL_X, PANEL_Y, PANEL_W, PANEL_H,
        (CTR_STYLE_FILL | CTR_STYLE_TEXT),
        PANEL_FC, PANEL_FC, PANEL_TC, 
        &g_sFontCmss22b, "");

// 创建标题栏
Canvas(g_sHeading, &g_sMainPanel, &g_sTxt1, 0,
    &Lcd_Display, PANEL_X, PANEL_Y, PANEL_W, HEADING_H,
    (CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_VCENTER),
    HEADING_FC, HEADING_FC, HEADING_TC, 
    &g_sFontCmss38b, "Text Heading", 0, 0);

// 创建文本框1
Canvas(g_sTxt1, &g_sMainPanel, &g_sTxt2, 0,
    &Lcd_Display, TXT1_X, TXT1_Y, TXT_W, TXT_H, 
    (CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE | CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_VCENTER),
    TXT_FC, TXT_OC, TXT_TC, 
    &g_sFontCmss22b, "Text Area 1", 0, 0);

// 创建文本框2
Canvas(g_sTxt2, &g_sMainPanel, &g_sBtn1, 0,
    &Lcd_Display, TXT2_X, TXT2_Y, TXT_W, TXT_H, 
    (CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE | CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_VCENTER),
    TXT_FC, TXT_OC, TXT_TC, 
    &g_sFontCmss22b, "Text Area 2", 0, 0);

// 创建按钮1（左侧第一行）
RectangularButton(g_sBtn1, &g_sMainPanel, &g_sBtn2, 0,
                &Lcd_Display, BTN1_X, BTN1_Y, BTN_W, BTN_H,
                (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_RELEASE_NOTIFY),
                BTN_FC, BTN_PFC, BTN_OC, BTN_TC,
                &g_sFontCmss22b, "F1", 0, 0, 0, 0, OnButtonPress);

// 创建按钮2（右侧第一行）
RectangularButton(g_sBtn2, &g_sMainPanel, &g_sBtn3, 0,
                &Lcd_Display, BTN2_X, BTN2_Y, BTN_W, BTN_H,
                (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_RELEASE_NOTIFY),
                BTN_FC, BTN_PFC, BTN_OC, BTN_TC,
                &g_sFontCmss22b, "F2", 0, 0, 0, 0, OnButtonPress);

// 创建按钮3（左侧第二行）
RectangularButton(g_sBtn3, &g_sMainPanel, &g_sBtn4, 0,
                &Lcd_Display, BTN3_X, BTN3_Y, BTN_W, BTN_H,
                (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_RELEASE_NOTIFY),
                BTN_FC, BTN_PFC, BTN_OC, BTN_TC,
                &g_sFontCmss22b, "F3", 0, 0, 0, 0, OnButtonPress);

// 创建按钮4（右侧第二行）
RectangularButton(g_sBtn4, &g_sMainPanel, &g_sBtn5, 0,
                &Lcd_Display, BTN4_X, BTN4_Y, BTN_W, BTN_H,
                (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_RELEASE_NOTIFY),
                BTN_FC, BTN_PFC, BTN_OC, BTN_TC,
                &g_sFontCmss22b, "F4", 0, 0, 0, 0, OnButtonPress);

// 创建按钮5（左侧第三行）
RectangularButton(g_sBtn5, &g_sMainPanel, &g_sBtn6, 0,
                &Lcd_Display, BTN5_X, BTN5_Y, BTN_W, BTN_H,
                (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_RELEASE_NOTIFY),
                BTN_FC, BTN_PFC, BTN_OC, BTN_TC,
                &g_sFontCmss22b, "F5", 0, 0, 0, 0, OnButtonPress);

// 创建按钮6（右侧第三行）
RectangularButton(g_sBtn6, &g_sMainPanel, &g_sBtn7, 0,
                &Lcd_Display, BTN6_X, BTN6_Y, BTN_W, BTN_H,
                (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_RELEASE_NOTIFY),
                BTN_FC, BTN_PFC, BTN_OC, BTN_TC,
                &g_sFontCmss22b, "F6", 0, 0, 0, 0, OnButtonPress);

// 创建按钮7（左侧第四行）
RectangularButton(g_sBtn7, &g_sMainPanel, &g_sBtn8, 0,
                &Lcd_Display, BTN7_X, BTN7_Y, BTN_W, BTN_H,
                (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_RELEASE_NOTIFY),
                BTN_FC, BTN_PFC, BTN_OC, BTN_TC,
                &g_sFontCmss22b, "F7", 0, 0, 0, 0, OnButtonPress);

// 创建按钮8（右侧第四行，最后一个控件）
RectangularButton(g_sBtn8, &g_sMainPanel, 0, 0,
                &Lcd_Display, BTN8_X, BTN8_Y, BTN_W, BTN_H,
                (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT | PB_STYLE_RELEASE_NOTIFY),
                BTN_FC, BTN_PFC, BTN_OC, BTN_TC,
                &g_sFontCmss22b, "F8", 0, 0, 0, 0, OnButtonPress);


#include "lcd_dma.h"
#include "lcd_grlib.h"
#include "delay.h"

/* 函数定义 */
// 初始化LCD控件
void Lcd_Widget_Init(void) 
{
    // 将主面板加入控件树
    WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sMainPanel);
    // 绘制控件树中的所有控件
    WidgetPaint(WIDGET_ROOT);
    
    WidgetMessageQueueProcess();

}

