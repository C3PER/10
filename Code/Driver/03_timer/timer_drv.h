/**
 * @file timer_drv.h
 * @brief 定时器设备驱动文件
 * @details 定义定时器驱动的核心功能接口，是连接用户API和底层硬件的桥梁。
 * @ingroup TIMER_DRV
 */

#ifndef _TIMER_DRV_H_
#define _TIMER_DRV_H_

/**
 * @defgroup TIMER_DRV 定时器外设驱动
 * @ingroup TIMER_API
 * @brief 定时器外设驱动模块
 * @details 
 * 定义定时器驱动的核心功能接口，是连接用户API和底层硬件的桥梁。
 * 
 * 主要职责包括：
 * - 定义定时器相关的时钟和频率宏
 * - 实现定时器的初始化逻辑
 * - 处理定时器中断事件
 * - 提供定时器状态的标志位
 * 
 * 详细内容请参考API接口文件timer_api.h或实现源文件timer_drv.c。
 * 
 * @{
 */

/* 时钟定义 */
/// @cond INTERNAL_MACROS

#define SYSCLK_1_FREQ     (456000000)
#define SYSCLK_2_FREQ     (SYSCLK_1_FREQ/2)
#define TIM2_FREQ         (SYSCLK_2_FREQ)

/// @endcond

/** @} */

#endif /* _TIMER_DRV_H_ */
