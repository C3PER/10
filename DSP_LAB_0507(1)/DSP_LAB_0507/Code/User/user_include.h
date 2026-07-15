/**
 * user_include.h
 *
 * 描述: 用户层统一头文件
 * 功能: 包含所有用户层模块的头文件
 */

#ifndef _USER_INCLUDE_H_
#define _USER_INCLUDE_H_

/* 用户驱动 */
// LED驱动
#include "user_led.h"
// KEY驱动
#include "user_key.h"
// TIMER驱动
#include "user_timer.h"
// ADC驱动
#include "user_adc.h"
// DAC驱动
#include "user_dac.h"
// UART驱动
#include "user_uart.h"
// RS485驱动
#include "user_rs485.h"
// FLASH驱动
#include "user_flash.h"
// EEPROM驱动
#include "user_eeprom.h"
// LCD驱动
#include "user_lcd.h"
// 屏幕触摸驱动
#include "user_touch.h"
// ADDA驱动
#include "user_adda.h"



#endif /* __USER_INCLUDE_H__ */
