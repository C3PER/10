/**
 * main.c
 *
 * 描述: 主函数文件
 * 功能: 系统初始化和主循环
 */

#include "driver_include.h"
#include "user_include.h"

#define SW_BREAKPOINT     asm(" SWBP 0 ")


/**
 * @brief 主函数
 * @return 无
 */
int main(void)
{
//    Led_Example();
//    Key_Example();
//    Timer_Example();
//    Uart_Example();
//    Uart_Example_NoInterrupt();
//    Uart_Example_Interrupt();
//    Rs485_Example();
//    Rs485_Example_Interrupt();
//    Flash_Example();
//    EEPROM_Example();
//    Adc_Example();
//    Dac_Example();
//    Adda_Example();
//    Adda_Different_Freq_Example();
    // LCD_Example1();
    // LCD_Example2();
    Touch_Example();


    while (1)
    {

    }
}

