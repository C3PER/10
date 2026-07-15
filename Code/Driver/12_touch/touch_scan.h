/**
 * @file touch_scan.h
 * @brief Touch扫描配置文件
 * @details 定义Touch扫描的接口。
 * @ingroup TOUCH_SCAN
 */

#ifndef _TOUCH_SCAN_H_
#define _TOUCH_SCAN_H_

/**
 * @defgroup TOUCH_SCAN Touch扫描
 * @ingroup TOUCH_API
 * @brief Touch扫描模块
 * @details 
 * 定义Touch扫描的接口。
 * 
 * 主要职责包括：
 * - 扫描触摸屏状态
 * - 读取触摸坐标
 * - 更新触摸状态变量
 * - 处理UI按钮的触摸响应
 * 
 * 本模块实现触摸屏的扫描和状态更新功能。
 * @{
 */


/** @brief 触摸状态 
 * @details 0: 未触摸 1: 触摸
*/
extern unsigned char Touch_Sta;

/** @brief 触摸点的X坐标 */
extern unsigned int Touch_X;

/** @brief 触摸点的Y坐标 */
extern unsigned int Touch_Y;

/** @} */

#endif /* _TOUCH_SCAN_H_ */
