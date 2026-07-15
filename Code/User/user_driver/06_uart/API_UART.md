# UART API使用指南

## 1. 设备介绍

### 1.1 UART设备概述

本项目支持的UART设备包括：

- **UART2**：系统UART接口，用于串行通信。

### 1.2 UART设备参数

| UART名称 | 波特率范围  | 数据位 | 停止位 | 校验位 |
| ------- | ----------- | --- | --- | --- |
| `UART2` | 9600 / 115200 | 8   | 1   | 无   |

## 2. 接口结构

### 2.1 头文件包含

使用UART API前，需要包含以下头文件：

```c
#include "uart_api.h"
```

### 2.2 宏定义

#### 2.2.1 波特率宏定义

| 宏名称                | 值      | 说明        |
| ------------------ | ------ | --------- |
| `UART_BAUD_9600`   | 9600   | 9600波特率   |
| `UART_BAUD_115200` | 115200 | 115200波特率 |

#### 2.2.2 FIFO触发级别宏定义

| 宏名称            | 值  | 说明         |
| -------------- | -- | ---------- |
| `UART_FIFO_1`  | 1  | FIFO触发级别1  |
| `UART_FIFO_4`  | 4  | FIFO触发级别4  |
| `UART_FIFO_8`  | 8  | FIFO触发级别8  |
| `UART_FIFO_14` | 14 | FIFO触发级别14 |

### 2.3 全局变量

| 变量名                 | 类型                         | 说明          |
| ------------------- | -------------------------- | ----------- |
| `Uart2_rxLength`    | `volatile unsigned char`   | 接收缓冲区长度     |
| `Uart2_rxCount`     | `volatile unsigned char`   | 接收缓冲区计数     |
| `FLAG_UART2_RX`     | `volatile unsigned char`   | 接收缓冲区标志位    |
| `FLAG_UART2_RX_CTI` | `volatile unsigned char`   | 接收缓冲区CTI标志位 |
| `Uart2_txLength`    | `volatile unsigned char`   | 发送缓冲区长度     |
| `Uart2_txCount`     | `volatile unsigned char`   | 发送缓冲区计数     |
| `FLAG_UART2_TX`     | `volatile unsigned char`   | 发送缓冲区标志位    |
| `Uart2_txBuffer`    | `volatile unsigned char[]` | 发送缓冲区       |
| `Uart2_rxBuffer`    | `volatile unsigned char[]` | 接收缓冲区       |

### 2.4 函数接口

#### 2.4.1 UART初始化函数

```c
void Uart2_Init(unsigned int baudRate, unsigned int fifoLevel);
```

**功能**：初始化UART2，设置波特率和FIFO触发级别。

**参数**：

- `baudRate`：波特率，可使用 `UART_BAUD_9600` 或 `UART_BAUD_115200`。
- `fifoLevel`：FIFO触发级别，可使用 `UART_FIFO_1`、`UART_FIFO_4`、`UART_FIFO_8` 或 `UART_FIFO_14`。


**使用场景**：在使用UART前必须调用此函数，通常在系统初始化时调用。

#### 2.4.2 简化版UART初始化函数

```c
void Uart2_Init_Lite(unsigned int baudRate);
```

**功能**：简化版UART2初始化，使用默认的FIFO触发级别。

**参数**：

- `baudRate`：波特率，可使用 `UART_BAUD_9600` 或 `UART_BAUD_115200`。


**使用场景**：在不需要自定义FIFO触发级别的情况下使用。

#### 2.4.3 发送数据函数

```c
void Uart2_SendBuffer(unsigned char *buffer, unsigned int length);
```

**功能**：发送数据到UART2发送缓冲区，等待发送完成。

**参数**：

- `buffer`：数据缓冲区。
- `length`：数据长度。


**使用场景**：用于发送数据到UART。

#### 2.4.4 设置接收缓冲区函数

```c
void Uart2_SetRxBuffer(unsigned int length);
```

**功能**：设置UART2接收缓冲区的长度，用于接收数据。

**参数**：

- `length`：缓冲区长度。


**使用场景**：在使用UART接收数据前调用，设置接收缓冲区的大小。

## 3. 使用示例

### 3.1 基本使用示例

以下示例参考自 `user_uart.c` 文件，展示了如何初始化和使用UART：

```c
#include "system.h"
#include "key_api.h"
#include "uart_api.h"
#include "uart.h"       // 提供printf、sprintf、UARTCharGet、UARTCharPut函数
#include "uartStdio.h"  // 提供UARTprintf、UARTPuts函数
#include "soc_C6748.h"  // UARTCharPut函数接近底层，需要

void Uart_Example(void)
{
    int num = 123456789;
    char buffer[128];
    char Receive;

    Sys_Init();
    Key_Init();
    /* 初始化UART，波特率115200 */
    Uart2_Init_Lite(UART_BAUD_115200);

    // 函数发送测试1
    UARTPuts("123456789\r\n", 5);
    // 函数发送测试2
    UARTPuts("123456789\r\n", -1);
    // 函数发送测试3
    sprintf(buffer, "Num:%d\r\n", num);
    UARTPuts(buffer, -1);
    // 函数发送测试4
    UARTprintf("Num:%d\r\n", num);

    // 主循环
    for(;;)
    {
        /* 发送接收的数据 */
        // 程序等待接收一字节数据，一直等待直到有数据可读
        Receive=UARTCharGet(SOC_UART_2_REGS);
        // 发送接收到的数据
        UARTCharPut(SOC_UART_2_REGS, Receive);
    }
}
```

### 3.2 无中断使用示例

以下示例展示了如何在不使用中断的情况下使用UART：

```c
#include "system.h"
#include "uart_api.h"
#include "uart.h"
#include "uartStdio.h"
#include "delay.h"

void Uart_Example_NoInterrupt(void)
{
    unsigned char rxBuffer[100];
    unsigned int rxLength;
    
    Sys_Init();
    /* 初始化UART，波特率115200 */
    Uart2_Init_Lite(UART_BAUD_115200);
    
    /* 发送欢迎信息 */
    UARTprintf("UART Example (No Interrupt) Started!\n\r");
    UARTprintf("This example demonstrates basic UART functions without interrupts.\n\r");
    UARTprintf("Type something and press Enter...\n\r");
    
    /* 主循环 */
    while(1)
    {
        /* 发送提示信息 */
        UARTprintf("\nEnter text: ");
        
        /* 接收字符串 */
        // 程序等待接收数据，一直等待直到有数据可读
        rxLength = UARTGets((char *)rxBuffer, 100);
        
        /* 发送接收到的数据 */
        UARTprintf("You entered: ");
        UARTPuts((char *)rxBuffer, rxLength);
        UARTprintf("\n");
        
        /* 发送字符 */
        UARTprintf("Sending individual characters: ");
        UARTCharPut(SOC_UART_2_REGS, 'H');
        UARTCharPut(SOC_UART_2_REGS, 'e');
        UARTCharPut(SOC_UART_2_REGS, 'l');
        UARTCharPut(SOC_UART_2_REGS, 'l');
        UARTCharPut(SOC_UART_2_REGS, 'o');
        UARTCharPut(SOC_UART_2_REGS, '!');
        UARTprintf("\n");
        
        /* 发送格式化数据 */
        UARTprintf("Formatted output: %s %d %x\n", "Test", 123, 0xABCD);

        delay(1000);
    }
}
```

### 3.3 中断使用示例

以下示例展示了如何在使用中断的情况下使用UART：

```c
#include "system.h"
#include "uart_api.h"
#include "uart.h"
#include "uartStdio.h"

void Uart_Example_Interrupt(void)
{
    /* 发送缓冲区：存储要发送的欢迎信息 */
    unsigned char txBuffer[] = "Hello from interrupt mode!\n\r";
    
    /* 1. 系统初始化
     *  - 初始化DSP中断控制器
     *  - 使能全局中断
     *  - 初始化延时功能
     */
    Sys_Init();
    
    /* 2. UART2初始化
     *  - 配置UART2的GPIO引脚
     *  - 设置波特率为115200
     *  - 配置FIFO触发级别为8字节
     *  - 注册并配置UART2中断
     */
    Uart2_Init(UART_BAUD_115200, UART_FIFO_8);
    
    /* 3. 配置接收缓冲区
     *  - 设置接收缓冲区长度为20字节
     *  - 重置接收计数器和标志位
     */
    Uart2_SetRxBuffer(20);
    
    /* 4. 发送欢迎信息
     *  - 使用中断方式发送数据
     *  - 中断服务函数会自动处理数据发送
     */
    Uart2_SendBuffer(txBuffer, sizeof(txBuffer));
    
    /* 5. 等待发送完成
     *  - 轮询FLAG_UART2_TX标志位
     *  - 发送完成后重置标志位
     */
    while(!FLAG_UART2_TX);
    FLAG_UART2_TX = 0;
    
    /* 6. 发送提示信息
     *  - 使用UARTprintf函数（非中断方式）
     *  - 向用户说明示例功能
     */
    UARTprintf("UART Example (Interrupt) Started!\n\r");
    UARTprintf("This example demonstrates UART functions with interrupts.\n\r");
    UARTprintf("Type something and press Enter...\n\r");
    
    /* 7. 主循环
     *  - 持续检测接收中断标志
     *  - 处理接收到的数据
     */
    while(1)
    {
        /* 检查FIFO触发中断
         * 当UART2接收到数据并达到FIFO触发级别时
         * 中断服务函数会读取数据到Uart2_rxBuffer并设置FLAG_UART2_RX标志位
         */
        if(FLAG_UART2_RX)
        {
            /* 重置接收标志，准备下一次接收 */
            FLAG_UART2_RX = 0;

            /* 回显接收到的数据
             * 1. 打印接收类型标识
             * 2. 发送接收缓冲区的数据
             * 3. 打印确认信息
             */
            UARTprintf("Uart2_rxBuffer: ");
            Uart2_SendBuffer((unsigned char *)Uart2_rxBuffer, 20);
            UARTprintf("\n\r");
        }
        
        /* 检查接收超时中断（CTI）
         * 当UART2接收数据后一段时间没有新数据时触发
         * （即一次没有收满FIFO缓冲区）
         * 用于处理不完整的数据包
         */
        if(FLAG_UART2_RX_CTI)
        {
            /* 重置接收超时标志，准备下一次接收 */
            FLAG_UART2_RX_CTI = 0;

            /* 回显接收到的数据
             * 1. 打印接收类型标识
             * 2. 发送接收缓冲区的数据
             * 3. 打印确认信息
             */
            UARTprintf("Uart2_rxBuffer: ");
            Uart2_SendBuffer((unsigned char *)Uart2_rxBuffer, 20);
            UARTprintf("\n\r");
        }
    }
}
```

## 4. 问题解答

### 4.1 常见问题

#### Q1: UART初始化失败怎么办？

**A1**：请按照以下分层排查顺序进行检查：

- **应用层**：是否调用了 `Sys_Init()` 和 `Uart2_Init()` 函数初始化中断和UART功能？是否调用了  `Uart2_Init_Lite()` 函数初始化UART？参数是否正确？
- **设备层**：请参考 `uart_api.h` 中的接口定义，确保API调用正确。
- **硬件层**：检查UART引脚连接是否正确，波特率设置是否与通信设备匹配。

#### Q2: UART无法接收数据怎么办？

**A2**：请按照以下分层排查顺序进行检查：

- **应用层**：是否设置了接收缓冲区长度？是否正确检查了接收标志位？
- **设备层**：请参考 `uart_api.h` 中的接口定义，确保API调用正确。
- **硬件层**：检查UART接收引脚连接是否正确，通信设备是否发送了数据。

#### Q3: UART无法发送数据怎么办？

**A3**：请按照以下分层排查顺序进行检查：

- **应用层**：是否正确调用了 `Uart2_SendBuffer()` 函数？发送缓冲区是否正确设置？
- **设备层**：请参考 `uart_api.h` 中的接口定义，确保API调用正确。
- **硬件层**：检查UART发送引脚连接是否正确，通信设备是否正常接收。

#### Q4: 如何选择合适的FIFO触发级别？

**A4**：FIFO触发级别的选择取决于应用场景：

- 低触发级别（1、4）：适用于需要快速响应的场景，但会增加中断频率。
- 高触发级别（8、14）：适用于大数据量传输的场景，减少中断频率，提高效率。

#### Q5: 如何实现UART的双向通信？

**A5**：UART是全双工通信接口，可以同时发送和接收数据。在应用中，你可以：

- 使用中断方式：通过检查接收标志位来处理接收到的数据，同时使用 `Uart2_SendBuffer()` 发送数据。
- 使用查询方式：通过 `UARTCharGet()` 函数查询是否有数据可读，使用 `UARTCharPut()` 函数发送数据。

### 4.2 故障排查

- **应用层**：请基于 `uart_api.h` 中的内容进行测试，确保API调用正确。
- **设备层**：如果遇到问题，请阅读驱动部分代码尝试排查或咨询工程师。
- **硬件层**：检查UART引脚连接和波特率设置。

## 5. 总结

UART API提供了简单易用的接口，用于实现串行通信。通过使用 `Uart2_Init()` 或 `Uart2_Init_Lite()` 函数初始化UART，然后使用 `Uart2_SendBuffer()` 发送数据，通过检查标志位来接收数据，可以实现各种UART通信功能。

本指南介绍了UART设备的基本信息、API接口的使用方法和常见问题的解答，希望能够帮助开发者快速上手UART控制功能，编写出高质量的应用程序。
