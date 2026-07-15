/**
 * @file touch_drv.h
 * @brief Touch设备驱动文件
 * @details 定义Touch驱动的核心功能接口，是连接用户API和底层硬件的桥梁。
 * @ingroup TOUCH_DRV
 */

#ifndef _TOUCH_DRV_H_
#define _TOUCH_DRV_H_

/**
 * @defgroup TOUCH_DRV Touch设备驱动
 * @ingroup TOUCH_API
 * @brief Touch设备驱动模块
 * @details 
 * 定义Touch驱动的核心功能接口，是连接用户API和底层硬件的桥梁。
 * 
 * 主要职责包括：
 * - 实现Touch的初始化逻辑
 * - 控制GT1151触摸芯片
 * - 提供寄存器读写接口
 * - 调用底层的引脚控制函数
 * 
 * 详细内容请参考API接口文件touch_api.h或实现源文件touch_drv.c。
 * 
 * @{
 */

/// @cond INTERNAL_MACROS
//GT1151 部分寄存器定义
#define GT_CTRL_REG     0X8040      //GT1151控制寄存器
#define GT_CFGS_REG     0X8050      //GT1151配置起始地址寄存器
#define GT_CHECK_REG    0X813C      //GT1151校验和寄存器
#define GT_PID_REG      0X8140      //GT1151产品ID寄存器
#define GT_FW_REG       0X8145      //GT1151 IC FW寄存器
#define GT_GSTID_REG    0X814E      //GT1151当前检测到的触摸情况
#define GT_TP1_REG      0X8150      //第一个触摸点数据地址
#define GT_TP2_REG      0X8158      //第二个触摸点数据地址
#define GT_TP3_REG      0X8160      //第三个触摸点数据地址
#define GT_TP4_REG      0X8168      //第四个触摸点数据地址
#define GT_TP5_REG      0X8170      //第五个触摸点数据地址

#define TP_PRES_DOWN    0x80        //触屏被按下
#define TP_CATH_PRES    0x40        //有按键按下了
#define CT_MAX_TOUCH    5           //电容屏支持的点数,固定为5点

// GT1151寄存器读写宏
#define GT1151_WR_Reg(REG_ADDR, Data, len)  IIC_SeqWrite_Reg(REG16, REG_ADDR, Data, len)
#define GT1151_RD_Reg(REG_ADDR, Data, len)  IIC_SeqRead_Reg(REG16, REG_ADDR, Data, len)
/// @endcond /* INTERNAL_MACROS 结束 */


/**
 * @brief 初始化Touch
 * @details 初始化Touch相关的IIC接口、引脚和GT1151芯片
 */
void Touch_Init(void);

/**
 * @brief 扫描GT1151触摸屏
 * @details 读取触摸状态和坐标
 */
void GT1151_Scan(void);

/** @} */

#endif /* _TOUCH_DRV_H_ */
