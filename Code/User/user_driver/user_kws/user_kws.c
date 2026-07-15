/**
 * @file user_kws.c
 * @brief KWS 主循环 + LCD 显示（按住识别，松开停止）
 *
 *   按住 KEY1 → 连续采集 2×8000 帧 → 推理 → 显示结果
 *   松开 KEY1 → 停止，显示最后结果
 */

#include "kws_inference.h"
#include "adc_api.h"
#include "lcd_api.h"
#include "grlib.h"
#include "system.h"
#include "key_api.h"
#include <string.h>
#include <stdio.h>

/* ===== 全局波形缓冲区 ===== */
#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION(g_waveform, ".far")
#endif
static short g_waveform[KWS_WAVEFORM_SAMPLES];

/* ===== 布局常量 ===== */
#define STATUS_Y    70
#define STATUS_H    40
#define RESULT_Y    160
#define RESULT_H    240

/* ===== 采集状态机 ===== */
static int  g_blk      = 0;
static int  g_running  = 0;
static int  g_ready    = 0;      /* kws_init 是否已执行 */


/* ===== 初始化 ===== */
void Kws_Init(void)
{
    /* 系统 + 按键 + LCD */
    Sys_Init();
    Key_Init();
    Lcd_Init();

    /* 清屏 */
    Lcd_Rectangle.sXMin = 0;
    Lcd_Rectangle.sYMin = 0;
    Lcd_Rectangle.sXMax = 799;
    Lcd_Rectangle.sYMax = 479;
    GrContextForegroundSet(&Lcd_Context, ClrBlack);
    GrRectFill(&Lcd_Context, &Lcd_Rectangle);

    /* 标题栏 */
    Lcd_Rectangle.sXMin = 0;
    Lcd_Rectangle.sYMin = 0;
    Lcd_Rectangle.sXMax = 799;
    Lcd_Rectangle.sYMax = 55;
    GrContextForegroundSet(&Lcd_Context, ClrDarkBlue);
    GrRectFill(&Lcd_Context, &Lcd_Rectangle);
    GrContextFontSet(&Lcd_Context, &g_sFontCm22);
    GrContextForegroundSet(&Lcd_Context, ClrWhite);
    GrStringDrawCentered(&Lcd_Context, "BC-ResNet KWS", -1, 400, 28, 0);

    /* 底部 */
    GrContextFontSet(&Lcd_Context, &g_sFontCm16);
    GrContextForegroundSet(&Lcd_Context, ClrGray);
    GrStringDrawCentered(&Lcd_Context, "TMS320C6748 DSP", -1, 400, 440, 0);

    /* 加载模型 */
    {
        Lcd_Rectangle.sXMin = 0;
        Lcd_Rectangle.sYMin = STATUS_Y;
        Lcd_Rectangle.sXMax = 799;
        Lcd_Rectangle.sYMax = STATUS_Y + STATUS_H;
        GrContextForegroundSet(&Lcd_Context, ClrBlack);
        GrRectFill(&Lcd_Context, &Lcd_Rectangle);
        GrContextFontSet(&Lcd_Context, &g_sFontCm18);
        GrContextForegroundSet(&Lcd_Context, ClrWhiteSmoke);
        GrStringDrawCentered(&Lcd_Context, "Loading model...", -1, 400, STATUS_Y + 10, 0);
    }

    {
        Lcd_Rectangle.sXMin = 0;
        Lcd_Rectangle.sYMin = STATUS_Y;
        Lcd_Rectangle.sXMax = 799;
        Lcd_Rectangle.sYMax = STATUS_Y + STATUS_H;
        GrContextForegroundSet(&Lcd_Context, ClrBlack);
        GrRectFill(&Lcd_Context, &Lcd_Rectangle);
        GrContextFontSet(&Lcd_Context, &g_sFontCm18);
        GrContextForegroundSet(&Lcd_Context, ClrWhiteSmoke);
        GrStringDrawCentered(&Lcd_Context, "Init ADC...", -1, 400, STATUS_Y + 10, 0);
    }

    /* ADC 初始化 + 启动（只启动一次） */
    Adc_Init(KWS_ADC_FREQ, KWS_ADC_SAMPLES);
    Adc_Start();

    /* Ready 提示 */
    {
        Lcd_Rectangle.sXMin = 0;
        Lcd_Rectangle.sYMin = STATUS_Y;
        Lcd_Rectangle.sXMax = 799;
        Lcd_Rectangle.sYMax = STATUS_Y + STATUS_H;
        GrContextForegroundSet(&Lcd_Context, ClrBlack);
        GrRectFill(&Lcd_Context, &Lcd_Rectangle);
        GrContextFontSet(&Lcd_Context, &g_sFontCm18);
        GrContextForegroundSet(&Lcd_Context, ClrGreen);
        GrStringDrawCentered(&Lcd_Context, "Ready.  Hold KEY1", -1, 400, STATUS_Y + 10, 0);
    }
}


/* ===== 主循环 ===== */
void Kws_Main(void)
{
    int result;
    unsigned int sc;
    char buf[64];

    while (1)
    {
        /* KEY1 切换 采集开/关 */
        if (FLAG_KEY1)
        {
            FLAG_KEY1 = 0;
            g_running = !g_running;
            g_blk = 0;

            if (g_running)
            {
                /* 首次按 ON 时加载模型（ADC 已先启动） */
                if (!g_ready)
                {
                    Lcd_Rectangle.sXMin = 0;
                    Lcd_Rectangle.sYMin = STATUS_Y;
                    Lcd_Rectangle.sXMax = 799;
                    Lcd_Rectangle.sYMax = STATUS_Y + STATUS_H;
                    GrContextForegroundSet(&Lcd_Context, ClrBlack);
                    GrRectFill(&Lcd_Context, &Lcd_Rectangle);
                    GrContextFontSet(&Lcd_Context, &g_sFontCm18);
                    GrContextForegroundSet(&Lcd_Context, ClrWhiteSmoke);
                    GrStringDrawCentered(&Lcd_Context, "Loading model...", -1, 400, STATUS_Y + 10, 0);

                    kws_init();
                    g_ready = 1;
                }

                Lcd_Rectangle.sXMin = 0;
                Lcd_Rectangle.sYMin = STATUS_Y;
                Lcd_Rectangle.sXMax = 799;
                Lcd_Rectangle.sYMax = STATUS_Y + STATUS_H;
                GrContextForegroundSet(&Lcd_Context, ClrBlack);
                GrRectFill(&Lcd_Context, &Lcd_Rectangle);
                GrContextFontSet(&Lcd_Context, &g_sFontCm18);
                GrContextForegroundSet(&Lcd_Context, ClrGreen);
                GrStringDrawCentered(&Lcd_Context, "ON  - Press KEY1 to STOP",
                                     -1, 400, STATUS_Y + 10, 0);
            }
            else
            {
                Lcd_Rectangle.sXMin = 0;
                Lcd_Rectangle.sYMin = STATUS_Y;
                Lcd_Rectangle.sXMax = 799;
                Lcd_Rectangle.sYMax = STATUS_Y + STATUS_H;
                GrContextForegroundSet(&Lcd_Context, ClrBlack);
                GrRectFill(&Lcd_Context, &Lcd_Rectangle);
                GrContextFontSet(&Lcd_Context, &g_sFontCm18);
                GrContextForegroundSet(&Lcd_Context, ClrYellow);
                GrStringDrawCentered(&Lcd_Context, "OFF - Press KEY1 to START",
                                     -1, 400, STATUS_Y + 10, 0);
            }
        }

        /* ADC 数据到来（非阻塞） */
        if (FLAG_AD && g_running)
        {
            FLAG_AD = 0;

            if (AD_Ping_Pong == AD_BUFFER_PONG)
                memcpy(&g_waveform[g_blk * KWS_ADC_SAMPLES], AD_CH1_Buf0,
                       KWS_ADC_SAMPLES * sizeof(short));
            else
                memcpy(&g_waveform[g_blk * KWS_ADC_SAMPLES], AD_CH1_Buf1,
                       KWS_ADC_SAMPLES * sizeof(short));

            g_blk++;

            if (g_blk >= 2)
            {
                g_blk = 0;

                /* 推理 */
                result = kws_recognize(g_waveform, KWS_WAVEFORM_SAMPLES);

                /* 状态栏 */
                if (result >= 2)      sc = ClrGreen;
                else if (result == 1) sc = ClrOrange;
                else                  sc = ClrGray;

                sprintf(buf, "%s (%d)", kws_label_name(result), result);

                Lcd_Rectangle.sXMin = 0;
                Lcd_Rectangle.sYMin = STATUS_Y;
                Lcd_Rectangle.sXMax = 799;
                Lcd_Rectangle.sYMax = STATUS_Y + STATUS_H;
                GrContextForegroundSet(&Lcd_Context, ClrBlack);
                GrRectFill(&Lcd_Context, &Lcd_Rectangle);
                GrContextFontSet(&Lcd_Context, &g_sFontCm18);
                GrContextForegroundSet(&Lcd_Context, sc);
                GrStringDrawCentered(&Lcd_Context, buf, -1, 400, STATUS_Y + 10, 0);

                /* 结果区 */
                {
                    const char* label = kws_label_name(result);
                    unsigned int rc;

                    if (result == 0)      rc = ClrGray;
                    else if (result == 1) rc = ClrOrange;
                    else                  rc = ClrGreen;

                    GrContextFontSet(&Lcd_Context, &g_sFontCm22);
                    GrContextForegroundSet(&Lcd_Context, rc);
                    sprintf(buf, "%s [%d]", label, result);
                    GrStringDrawCentered(&Lcd_Context, buf, -1, 400, 250, 0);
                }
            }
        }
    }
}
