/**
 * main.c — BC-ResNet KWS
 * TMS320C6748 DSP
 * KEY1 切换 采集/停止
 */

#include "driver_include.h"
#include "user_include.h"

int main(void)
{
    Kws_Init();
    Kws_Main();
    while (1);
}
