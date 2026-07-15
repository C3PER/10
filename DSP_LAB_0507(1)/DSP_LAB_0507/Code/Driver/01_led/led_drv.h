/**
 * @file led_drv.h
 * @brief LED设备驱动文件
 * @details 定义LED驱动的核心功能接口，是连接用户API和底层引脚的桥梁。
 * @ingroup LED_DRV
 */

#ifndef _LED_DRV_H_
#define _LED_DRV_H_

/**
 * @defgroup LED_DRV LED设备驱动
 * @ingroup LED_API
 * @brief LED设备驱动模块
 * @details 
 * 定义LED驱动的核心功能接口，是连接用户API和底层引脚的桥梁。
 * 
 * 主要职责包括：
 * - 实现LED的初始化和控制逻辑
 * - 将用户的抽象请求转换为具体的引脚操作
 * - 维护LED索引到实际引脚的映射关系
 * - 提供统一的驱动接口，屏蔽底层硬件差异
 * 
 * 详细内容请参考API接口文件led_api.h或实现源文件led_drv.c。
 * 
 * @{
 */

/** @} */


#endif /* _LED_DRV_H_ */
