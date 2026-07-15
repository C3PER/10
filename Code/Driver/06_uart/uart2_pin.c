/**
 * @file uart2_pin.c
 * @brief UART2引脚配置实现文件
 * @details 实现UART2引脚的初始化，包括PSC初始化、GPIO引脚复用配置和UART初始化。
 * @ingroup UART2_PIN
 */

#include "uart2_pin.h"

#include "soc_C6748.h"
#include "hw_types.h"
#include "hw_syscfg0_C6748.h"
#include "psc.h"
#include "uart.h"



/* 函数声明 */
// PSC 初始化
static void PSCInit(void);
// GPIO 管脚复用配置
static void GPIOBankPinMuxSet(void);
// UART 初始化
static void UARTInit(unsigned int baudRate);


/* 函数定义 */
// 简化版UART2初始化
void Uart2_Init_Lite(unsigned int baudRate)
{
    PSCInit();
    GPIOBankPinMuxSet();
    UARTInit(baudRate);
}

// PSC 初始化
static void PSCInit(void)
{
    // 使能 UART2 模块
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_UART2, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);
}

// GPIO 管脚复用配置
static void GPIOBankPinMuxSet(void)
{
    volatile unsigned int svPinMuxTxdRxd = 0;

    // 清除 UART2_TXD 和 UART2_RXD 引脚的复用配置
    svPinMuxTxdRxd = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) & \
                      ~(SYSCFG_PINMUX4_PINMUX4_23_20 | \
                        SYSCFG_PINMUX4_PINMUX4_19_16));

    // 配置 UART2_TXD 和 UART2_RXD 引脚的复用配置
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) = \
         (PINMUX4_UART2_TXD_ENABLE | \
          PINMUX4_UART2_RXD_ENABLE | \
          svPinMuxTxdRxd);
}

// UART 初始化
static void UARTInit(unsigned int baudRate)
{
    // 配置 UART2 参数
    // 波特率 115200 数据位 8 停止位 1 无校验位
    UARTConfigSetExpClk(SOC_UART_2_REGS, UART_2_FREQ, baudRate,
                          UART_WORDL_8BITS, UART_OVER_SAMP_RATE_16);
    // 使能 UART2
    UARTEnable(SOC_UART_2_REGS);
}



/* 下面展示了一些函数原型，来自于StarterWare库，本工程中这些函数已经被封装为lib文件 */
/* 完成串口初始化后（不开中断）即可调用以下函数，部分函数用法可参考user_uart.c */
/* 为了充分利用中断和FIFO，在uart_drv.c中封装了一些其它发送接收函数 */

/* 以下内容来自uart.c */

// /**
//  * \brief    This function attempts to write a byte into Transmitter Holding 
//  *           Register (THR). This API checks only once if the transmitter
//  *           is empty. If yes, it puts the byte, else it returns FALSE.
//  *
//  * \param    baseAdd     Memory address of the UART instance being used.
//  * \param    byteWrite   Byte to be written into the THR register.
//  *
//  * \return   TRUE if the transmitter FIFO(or THR register in non-FIFO mode)
//  *           is empty and the character was written. Else it returns FALSE.
//  */

// unsigned int UARTCharPutNonBlocking(unsigned int baseAdd, 
//                                     unsigned char byteWrite)
// {
//     unsigned int retVal = FALSE;
    
//     if (HWREG(baseAdd + UART_LSR) & (UART_THR_TSR_EMPTY | UART_THR_EMPTY))
//     {
//        HWREG(baseAdd + UART_THR) = byteWrite;
//        retVal = TRUE;
//     }
    
//     return retVal;
//  }

 
// /**
//  * \brief    This function reads a byte from the Receiver Buffer Register
//  *           (RBR). This API checks if any character is ready to be read.
//  *            If ready then returns the read byte, else it returns -1.
//  *
//  * \param    baseAdd   Memory address of the UART instance being used.
//  * 
//  * \return   The character in the RBR register typecasted as integer.
//  *           If no character is found in the RBR register, -1
//  *           is returned.
//  */

// int UARTCharGetNonBlocking(unsigned int baseAdd)
// {
//     int retVal = -1;

//     if(HWREG(baseAdd + UART_LSR) & UART_DATA_READY)
//     {
//        retVal = HWREG(baseAdd + UART_RBR);
//     }
    
//     return retVal;
// }


// /**
//  * \brief      This function waits indefinitely for the arrival of a byte in   
//  *             the receiver FIFO. Once a byte has arrived, it returns that 
//  *             byte typecasted as integer.
//  *
//  * \param      baseAdd     Memory address of the UART instance being used.
//  *
//  * \return     The byte in the RBR register typecasted as integer.
//  *
//  */

// int UARTCharGet(unsigned int baseAdd)
// {
//     int data = 0;
    
//     /*
//     ** Busy check if data is available in receiver FIFO(RBR regsiter in non-FIFO
//     ** mode) so that data could be read from the RBR register.
//     */
//     while ((HWREG(baseAdd + UART_LSR) & UART_DATA_READY) == 0);
    
//     data = (int)HWREG(baseAdd + UART_RBR);

//     return data;
// }

// /**
//  * \brief   This function checks indefinitely whether the Transmitter FIFO
//  *          (THR regsiter in non-FIFO mode)is empty. If found empty, a byte
//  *          is written into the THR register.
//  *
//  * \param   baseAdd         Memory address of the UART instance being used.
//  * \param   byteTx          Byte to be transmitted.
//  *
//  * \return  None.
//  *
//  */


// void UARTCharPut(unsigned int baseAdd, unsigned char byteTx)
// {
//    unsigned int txEmpty;

//    txEmpty = (UART_THR_TSR_EMPTY | UART_THR_EMPTY);

//    /*
//    ** Here we check for the emptiness of both the Trasnsmitter Holding
//    ** Register(THR) and Transmitter Shift Register(TSR) before writing
//    ** data into the Transmitter FIFO(THR for non-FIFO mode).
//    */
   
//    while (txEmpty != (HWREG(baseAdd + UART_LSR) & txEmpty));
   
//    /*
//    ** Transmitter FIFO(THR register in non-FIFO mode) is empty.
//    ** Write the byte onto the THR register.
//    */
//    HWREG(baseAdd + UART_THR) = byteTx;
// }





/* 以下内容来自uartStdio.c */

// /**
//  * \brief  This function writes data from a specified buffer onto the
//  *         transmitter FIFO of UART.
//  *
//  * \param  pTxBuffer        Pointer to a buffer in the transmitter.  
//  * \param  numBytesToWrite  Number of bytes to be transmitted to the 
//  *                          transmitter FIFO. The user has the freedom to not 
//  *                          specify any valid value for this if he wants to
//  *                          print until the occurence of a NULL character.
//  *                          In this case, he has to pass a negative value as
//  *                          this parameter. 
//  *  
//  * \return  Number of bytes written to the transmitter FIFO.
//  *
//  * \note   1> Whenever a null character(\0) is encountered in the 
//  *            data to be transmitted, the transmission is stopped. \n
//  *         2> Whenever the transmitter data has a new line character(\n), 
//  *            it is interpreted as a new line(\n) + carraige return(\r)
//  *            characters. This is because the serial console interprets a 
//  *            new line character as it is and does not introduce a carraige 
//  *            return. \n
//  *            
//  *         Some example function calls of this function are: \n
//  *
//  *         UARTPuts(txArray, -2): This shall print the contents of txArray[]
//  *         until the occurence of a NULL character. \n
//  *
//  *         UARTPuts("Hello World", 8): This shall print the first 8 characters
//  *         of the string shown. \n
//  *
//  *         UARTPuts("Hello World", 20): This shall print the string shown until
//  *         the occurence of the NULL character. Here, the NULL character is
//  *         encountered earlier than the length of 20 bytes.\n
//  * 
//  */
// unsigned int UARTPuts(char *pTxBuffer, int numBytesToWrite)
// {
//      unsigned int count = 0;
//      unsigned int flag = 0;

//      if(numBytesToWrite < 0)
//      {
//           flag = 1;
//      }
     
//      while('\0' != *pTxBuffer)
//      {
//           /* Checks if data is a newline character. */
//           if('\n' == *pTxBuffer)
//           {
//                /* Ensuring applicability to serial console.*/
//                UARTPutc('\r');
//                UARTPutc('\n');
//           }
//           else
//           {
//                UARTPutc((unsigned char)*pTxBuffer);
//           }
//           pTxBuffer++;
//           count++;

//           if((0 == flag) && (count == numBytesToWrite))
//           {
//                break;
//           }

//      }
//    /* Returns the number of bytes written onto the transmitter FIFO. */
//    return count;
// }


// /**
//  * \brief  This function reads data from the receiver FIFO to a buffer 
//  *         in the receiver. 
//  *
//  * \param  pRxBuffer        Pointer to a buffer in the receiver.
//  * \param  numBytesToRead   The number of bytes the receiver buffer can hold.
//  *                          However, if the user wishes not to specify this
//  *                          parameter, he has freedom to do so. In that case,
//  *                          the user has to pass a negative value for this 
//  *                          parameter.
//  *
//  * \return  Number of bytes read from the receiver FIFO.
//  *
//  * \note   The two exit points for this function are:
//  *         1> To enter an ESC character or a carraige return character('Enter'
//  *            key on the Keyboard).\n
//  *         2> Specify a limit to the number of bytes to be read. \n
//  * 
//  *         Some example function calls of this function are:
//  *
//  *         UARTGets(rxBuffer, -2): This reads characters from
//  *         the receiver FIFO of UART until the occurence of a carriage return
//  *         ('Enter' key on the keyboard pressed) or an ESC character.
//  *           
//  *         UARTGets(rxBuffer, 12): This reads characters until
//  *         12 characters have been read or until an occurence of a carriage 
//  *         return or an ESC character, whichever occurs first.
//  */

// unsigned int UARTGets(char *pRxBuffer, int numBytesToRead)
// {     
//      unsigned int count = 0;
//      unsigned int flag = 0;

//      if(numBytesToRead < 0)
//      {
//           flag = 1;
//      }
//      do
//      {
//           *pRxBuffer = UARTGetc();
          
//           /*
//           ** 0xD - ASCII value of Carriage Return.
//           ** 0x1B - ASCII value of ESC character.
//           */ 
//           if(0xD == *pRxBuffer || 0x1B == *pRxBuffer)
//           {
//                *pRxBuffer = '\0';
//                break;
//           }

//           /* Echoing the typed character back to the serial console. */
//           UARTPutc((unsigned char)*pRxBuffer);
//           pRxBuffer++;     
//           count++;

//           if(0 == flag && (count == numBytesToRead))
//           {
//                break;
//           }

//      }while(1);
     
//      return count;
// }


// /**
//  * A simple UART based printf function supporting \%c, \%d, \%p, \%s, \%u,
//  * \%x, and \%X.
//  *
//  * \param pcString is the format string.
//  * \param ... are the optional arguments, which depend on the contents of the
//  * format string.
//  *
//  * This function is very similar to the C library <tt>fprintf()</tt> function.
//  * All of its output will be sent to the UART.  Only the following formatting
//  * characters are supported:
//  *
//  * - \%c to print a character
//  * - \%d to print a decimal value
//  * - \%s to print a string
//  * - \%u to print an unsigned decimal value
//  * - \%x to print a hexadecimal value using lower case letters
//  * - \%X to print a hexadecimal value using lower case letters (not upper case
//  * letters as would typically be used)
//  * - \%p to print a pointer as a hexadecimal value
//  * - \%\% to print out a \% character
//  *
//  * For \%s, \%d, \%u, \%p, \%x, and \%X, an optional number may reside
//  * between the \% and the format character, which specifies the minimum number
//  * of characters to use for that value; if preceded by a 0 then the extra
//  * characters will be filled with zeros instead of spaces.  For example,
//  * ``\%8d'' will use eight characters to print the decimal value with spaces
//  * added to reach eight; ``\%08d'' will use eight characters as well but will
//  * add zeroes instead of spaces.
//  *
//  * The type of the arguments after \e pcString must match the requirements of
//  * the format string.  For example, if an integer was passed where a string
//  * was expected, an error of some kind will most likely occur.
//  *
//  * \return None.
//  */
// void UARTprintf(const char *pcString, ...)
// {
//     unsigned int idx, pos, count, base, neg;
//     char *pcStr, pcBuf[16], cFill;
//     va_list vaArgP;
//     int value;

//     /* Start the varargs processing. */
//     va_start(vaArgP, pcString);

//     /* Loop while there are more characters in the string. */
//     while(*pcString)
//     {
//         /* Find the first non-% character, or the end of the string. */
//         for(idx = 0; (pcString[idx] != '%') && (pcString[idx] != '\0'); idx++)
//         {
//         }

//         /* Write this portion of the string. */
//         UARTwrite(pcString, idx);

//         /* Skip the portion of the string that was written. */
//         pcString += idx;

//         /* See if the next character is a %. */
//         if(*pcString == '%')
//         {
//             /* Skip the %. */
//             pcString++;

//             /* Set the digit count to zero, and the fill character to space
//              * (i.e. to the defaults). */
//             count = 0;
//             cFill = ' ';

//             /* It may be necessary to get back here to process more characters.
//              * Goto's aren't pretty, but effective.  I feel extremely dirty for
//              * using not one but two of the beasts. */
// again:

//             /* Determine how to handle the next character. */
//             switch(*pcString++)
//             {
//                 /* Handle the digit characters. */
//                 case '0':
//                 case '1':
//                 case '2':
//                 case '3':
//                 case '4':
//                 case '5':
//                 case '6':
//                 case '7':
//                 case '8':
//                 case '9':
//                 {
//                     /* If this is a zero, and it is the first digit, then the
//                      * fill character is a zero instead of a space. */
//                     if((pcString[-1] == '0') && (count == 0))
//                     {
//                         cFill = '0';
//                     }

//                     /* Update the digit count. */
//                     count *= 10;
//                     count += pcString[-1] - '0';

//                     /* Get the next character. */
//                     goto again;
//                 }

//                 /* Handle the %c command. */
//                 case 'c':
//                 {
//                     /* Get the value from the varargs. */
//                     value = va_arg(vaArgP, unsigned int);

//                     /* Print out the character. */
//                     UARTwrite((char *)&value, 1);

//                     /* This command has been handled. */
//                     break;
//                 }

//                 /* Handle the %d command. */
//                 case 'd':
//                 {
//                     /* Get the value from the varargs. */
//                     value = va_arg(vaArgP, unsigned int);

//                     /* Reset the buffer position. */
//                     pos = 0;

//                     /* If the value is negative, make it positive and indicate
//                      * that a minus sign is needed. */
//                     if((int)value < 0)
//                     {
//                         /* Make the value positive. */
//                         value = -(int)value;

//                         /* Indicate that the value is negative. */
//                         neg = 1;
//                     }
//                     else
//                     {
//                         /* Indicate that the value is positive so that a minus
//                          * sign isn't inserted. */
//                         neg = 0;
//                     }

//                     /* Set the base to 10. */
//                     base = 10;

//                     /* Convert the value to ASCII. */
//                     goto convert;
//                 }

//                 /* Handle the %s command. */
//                 case 's':
//                 {
//                     /* Get the string pointer from the varargs. */
//                     pcStr = va_arg(vaArgP, char *);

//                     /* Determine the length of the string. */
//                     for(idx = 0; pcStr[idx] != '\0'; idx++)
//                     {
//                     }

//                     /* Write the string. */
//                     UARTwrite(pcStr, idx);

//                     /* Write any required padding spaces */
//                     if(count > idx)
//                     {
//                         count -= idx;
//                         while(count--)
//                         {
//                             UARTwrite((const char *)" ", 1);
//                         }
//                     }
//                     /* This command has been handled. */
//                     break;
//                 }

//                 /* Handle the %u command. */
//                 case 'u':
//                 {
//                     /* Get the value from the varargs. */
//                     value = va_arg(vaArgP, unsigned int);

//                     /* Reset the buffer position. */
//                     pos = 0;

//                     /* Set the base to 10. */
//                     base = 10;

//                     /* Indicate that the value is positive so that a minus sign
//                      * isn't inserted. */
//                     neg = 0;

//                     /* Convert the value to ASCII. */
//                     goto convert;
//                 }

//                 /* Handle the %x and %X commands.  Note that they are treated
//                  * identically; i.e. %X will use lower case letters for a-f
//                  * instead of the upper case letters is should use.  We also
//                  * alias %p to %x. */
//                 case 'x':
//                 case 'X':
//                 case 'p':
//                 {
//                     /* Get the value from the varargs. */
//                     value = va_arg(vaArgP, unsigned int);

//                     /* Reset the buffer position. */
//                     pos = 0;

//                     /* Set the base to 16. */
//                     base = 16;

//                     /* Indicate that the value is positive so that a minus sign
//                      * isn't inserted. */
//                     neg = 0;

//                     /* Determine the number of digits in the string version of
//                      * the value. */
// convert:
//                     for(idx = 1;
//                         (((idx * base) <= value) &&
//                          (((idx * base) / base) == idx));
//                         idx *= base, count--)
//                     {
//                     }

//                     /* If the value is negative, reduce the count of padding
//                      * characters needed. */
//                     if(neg)
//                     {
//                         count--;
//                     }

//                     /* If the value is negative and the value is padded with
//                      * zeros, then place the minus sign before the padding. */
//                     if(neg && (cFill == '0'))
//                     {
//                         /* Place the minus sign in the output buffer. */
//                         pcBuf[pos++] = '-';

//                         /* The minus sign has been placed, so turn off the
//                          * negative flag. */
//                         neg = 0;
//                     }

//                     /* Provide additional padding at the beginning of the
//                      * string conversion if needed. */
//                     if((count > 1) && (count < 16))
//                     {
//                         for(count--; count; count--)
//                         {
//                             pcBuf[pos++] = cFill;
//                         }
//                     }

//                     /* If the value is negative, then place the minus sign
//                      * before the number. */
//                     if(neg)
//                     {
//                         /* Place the minus sign in the output buffer. */
//                         pcBuf[pos++] = '-';
//                     }

//                     /* Convert the value into a string. */
//                     for(; idx; idx /= base)
//                     {
//                         pcBuf[pos++] = g_pcHex[(value / idx) % base];
//                     }

//                     /* Write the string. */
//                     UARTwrite(pcBuf, pos);

//                     /* This command has been handled. */
//                     break;
//                 }

//                 /* Handle the %% command. */
//                 case '%':
//                 {
//                     /* Simply write a single %. */
//                     UARTwrite(pcString - 1, 1);

//                     /* This command has been handled. */
//                     break;
//                 }

//                 /* Handle all other commands. */
//                 default:
//                 {
//                     /* Indicate an error. */
//                     UARTwrite((const char *)"ERROR", 5);

//                     /* This command has been handled. */
//                     break;
//                 }
//             }
//         }
//     }

//     /* End the varargs processing. */
//     va_end(vaArgP);
// }
