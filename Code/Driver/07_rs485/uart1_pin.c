/**
 * uart1_pin.c
 *
 * 描述: UART1引脚配置实现文件
 * 功能: 实现UART1引脚的初始化
 */

#include "uart1_pin.h"

#include "soc_C6748.h"
#include "hw_types.h"
#include "hw_syscfg0_C6748.h"
#include "psc.h"
#include "uart.h"



/* 函数声明 */
static void PSCInit(void);
static void GPIOBankPinMuxSet(void);
static void UARTInit(unsigned int baudRate);


/**
 * @brief 简化版UART1初始化
 * @param baudRate: 波特率
 * @return 无
 */
void Uart1_Init_Lite(unsigned int baudRate)
{
    PSCInit();
    GPIOBankPinMuxSet();
    UARTInit(baudRate);
}

/**
 * @brief PSC 初始化
 * @return 无
 */
static void PSCInit(void)
{
    /* 使能 UART1 模块 */
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_UART1, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);
}

/**
 * @brief GPIO 管脚复用配置
 * @return 无
 */
static void GPIOBankPinMuxSet(void)
{
    volatile unsigned int svPinMuxTxdRxd = 0;

    /*
    ** Clearing the pins in context and retaining the other pin values
    ** of PINMUX4 register.
    */
    svPinMuxTxdRxd = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) & \
                      ~(SYSCFG_PINMUX4_PINMUX4_31_28 | \
                        SYSCFG_PINMUX4_PINMUX4_27_24));

    /* Actual pin multiplexing for UART1_TXD and UART1_RXD. */
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) = \
         (PINMUX4_UART1_TXD_ENABLE | \
          PINMUX4_UART1_RXD_ENABLE | \
          svPinMuxTxdRxd);
}

/**
 * @brief UART 初始化
 * @param baudRate: 波特率
 * @return 无
 */
static void UARTInit(unsigned int baudRate)
{
    /* 配置 UART1 参数 */
    /* 波特率 115200 数据位 8 停止位 1 无校验位 */
    UARTConfigSetExpClk(SOC_UART_1_REGS, UART_1_FREQ, baudRate,
                          UART_WORDL_8BITS, UART_OVER_SAMP_RATE_16);
    /* 使能 UART1 */
    UARTEnable(SOC_UART_1_REGS);
}
