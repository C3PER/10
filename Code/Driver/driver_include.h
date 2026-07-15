/**
 * driver_include.h
 *
 * 描述: 驱动统一头文件
 * 功能: 包含所有驱动模块的头文件
 */

#ifndef _DRIVER_INCLUDE_H_
#define _DRIVER_INCLUDE_H_

/* 库 */
#include "delay.h"      // 提供delay函数
#include "uart.h"       // 提供printf、sprintf、UARTCharGet、UARTCharPut函数
#include "uartStdio.h"  // 提供UARTprintf、UARTPuts函数
#include "c6x.h"        // 提供_itoll函数，用于测量时间
#include "dspcache.h"   // 提供CacheWB、CacheInv，回写和使无效

// 中断和延时驱动
#include "system.h"
// LED驱动
#include "led_api.h"
// KEY驱动
#include "key_api.h"
// TIMER驱动
#include "timer_api.h"
// ADC驱动
#include "adc_api.h"
// DAC驱动
#include "dac_api.h"
// UART驱动
#include "uart_api.h"
// RS485驱动
#include "rs485_api.h"
// FLASH驱动
#include "flash_api.h"
// EEPROM驱动
#include "eeprom_api.h"
// LCD驱动
#include "lcd_api.h"
// 屏幕触摸驱动
#include "touch_api.h"


#endif /* _DRIVER_INCLUDE_H_ */
