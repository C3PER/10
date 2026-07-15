/**
 * uart1_pin.h
 *
 * 描述: UART1引脚配置头文件
 * 功能: 定义UART1引脚配置相关的宏和函数声明
 */

#ifndef _UART1_PIN_H_
#define _UART1_PIN_H_

// 时钟
#define SYSCLK_1_FREQ     (456000000)
#define SYSCLK_2_FREQ     (SYSCLK_1_FREQ/2)
#define UART_1_FREQ       (SYSCLK_2_FREQ)

// 引脚定义
#define PINMUX4_UART1_TXD_ENABLE     (SYSCFG_PINMUX4_PINMUX4_31_28_UART1_TXD << \
                                      SYSCFG_PINMUX4_PINMUX4_31_28_SHIFT)
#define PINMUX4_UART1_RXD_ENABLE     (SYSCFG_PINMUX4_PINMUX4_27_24_UART1_RXD << \
                                      SYSCFG_PINMUX4_PINMUX4_27_24_SHIFT)

/**
 * @brief 简化版UART1初始化
 * @param baudRate: 波特率
 * @return 无
 */
void Uart1_Init_Lite(unsigned int baudRate);


#endif /* _UART1_PIN_H_ */
