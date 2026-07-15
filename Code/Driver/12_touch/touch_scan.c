/**
 * @file touch_scan.c
 * @brief Touch扫描实现文件
 * @details 实现Touch扫描功能，读取触摸坐标并更新状态。
 * @ingroup TOUCH_SCAN
 */

/* 头文件包含 */
#include "touch_scan.h"           // Touch扫描
#include "touch_drv.h"            // Touch设备驱动
#include "touch_btn.h"            // Touch按钮
#include "sys_iic.h"              // 系统IIC接口


// 触摸状态和坐标
unsigned char Touch_Sta = 0;    // 触摸状态 0: 未触摸 1: 触摸
unsigned int Touch_X;           // 触摸点的X坐标
unsigned int Touch_Y;           // 触摸点的Y坐标


/* 函数定义 */
// 扫描触摸并处理按钮响应
void Touch_Scan(void)
{
    GT1151_Scan();
    Widget_Btn_App(Touch_Sta, Touch_X, Touch_Y);
}

// 扫描GT1151触摸屏
void GT1151_Scan(void)
{
    unsigned char State = 0;
    unsigned char Temp_XY[4] = {0};

    // 读取触摸状态寄存器
    GT1151_RD_Reg(GT_GSTID_REG, &State, 1);

    // 写0清寄存器来开启下一次检测
    GT1151_WR_Reg(GT_GSTID_REG, 0, 1);

    if(State == 0X81) {
        GT1151_RD_Reg(GT_TP1_REG, Temp_XY, 4);
        Touch_X = ((unsigned int)Temp_XY[1] << 8) + Temp_XY[0];
        Touch_Y = ((unsigned int)Temp_XY[3] << 8) + Temp_XY[2];
        Touch_Sta = 1;
    }
    else if(State == 0X80) {
        Touch_Sta = 0;
    }
}
