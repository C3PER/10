# RS485 API使用指南

## 1. 设备介绍

### 1.1 RS485设备概述

RS485是一种串行通信标准，支持半双工通信模式，适用于工业控制和长距离通信场景。

在本项目中，RS485模块基于UART1实现，通过控制DE/RE引脚实现发送和接收模式的切换。

### 1.2 RS485设备参数

| RS485参数 | 值      | 说明        |
| -------- | ------ | --------- |
| 波特率     | 9600 / 115200 | 支持两种波特率   |
| 数据位     | 8      | 8位数据位     |
| 停止位     | 1      | 1位停止位     |
| 校验位     | 无      | 无校验       |
| 通信模式    | 半双工    | 同一时刻只能发送或接收 |

### 1.3 RS485通信原理

RS485是半双工通信接口，通信双方需要协商好通信协议：

1. **发送数据**：主机将DE/RE引脚置高，切换到发送模式，然后发送数据。
2. **接收数据**：主机将DE/RE引脚置低，切换到接收模式，等待接收数据。
3. **数据帧格式**：通常使用标准的UART帧格式，包含起始位、数据位、可选的校验位和停止位。

## 2. 接口结构

### 2.1 头文件包含

使用RS485 API前，需要包含以下头文件：

```c
#include "rs485_api.h"
```

### 2.2 宏定义

#### 2.2.1 波特率宏定义

| 宏名称                | 值      | 说明        |
| ------------------ | ------ | --------- |
| `RS485_BAUD_9600`   | 9600   | 9600波特率   |
| `RS485_BAUD_115200` | 115200 | 115200波特率 |

#### 2.2.2 FIFO触发级别宏定义

| 宏名称              | 值  | 说明         |
| ---------------- | -- | ---------- |
| `RS485_FIFO_1`   | 1  | FIFO触发级别1  |
| `RS485_FIFO_4`   | 4  | FIFO触发级别4  |
| `RS485_FIFO_8`   | 8  | FIFO触发级别8  |
| `RS485_FIFO_14`  | 14 | FIFO触发级别14 |

#### 2.2.3 传输模式宏定义

| 宏名称            | 值  | 说明       |
| -------------- | -- | -------- |
| `RS485_MODE_RX` | 0  | 接收模式     |
| `RS485_MODE_TX` | 1  | 发送模式     |

### 2.3 全局变量

| 变量名                 | 类型                         | 说明          |
| ------------------- | -------------------------- | ----------- |
| `Uart1_rxLength`    | `volatile unsigned char`   | 接收缓冲区长度     |
| `Uart1_rxCount`     | `volatile unsigned char`   | 接收缓冲区计数     |
| `FLAG_UART1_RX`     | `volatile unsigned char`   | 接收缓冲区标志位    |
| `FLAG_UART1_RX_CTI` | `volatile unsigned char`   | 接收缓冲区CTI标志位 |
| `Uart1_txLength`    | `volatile unsigned char`   | 发送缓冲区长度     |
| `Uart1_txCount`     | `volatile unsigned char`   | 发送缓冲区计数     |
| `FLAG_UART1_TX`     | `volatile unsigned char`   | 发送缓冲区标志位    |
| `Uart1_txBuffer`    | `volatile unsigned char[]` | 发送缓冲区       |
| `Uart1_rxBuffer`    | `volatile unsigned char[]` | 接收缓冲区       |

### 2.4 函数接口

#### 2.4.1 RS485初始化函数

```c
void Rs485_Init(unsigned int baudRate, unsigned int fifoLevel);
```

**功能**：初始化RS485，设置波特率和FIFO触发级别。

**参数**：

- `baudRate`：波特率，可使用 `RS485_BAUD_9600` 或 `RS485_BAUD_115200`。
- `fifoLevel`：FIFO触发级别，可使用 `RS485_FIFO_1`、`RS485_FIFO_4`、`RS485_FIFO_8` 或 `RS485_FIFO_14`。


**使用场景**：在使用RS485前必须调用此函数，通常在系统初始化时调用。

#### 2.4.2 简化版RS485初始化函数

```c
void Rs485_Init_Lite(unsigned int baudRate);
```

**功能**：简化版RS485初始化，使用默认的FIFO触发级别。

**参数**：

- `baudRate`：波特率，可使用 `RS485_BAUD_9600` 或 `RS485_BAUD_115200`。


**使用场景**：在不需要自定义FIFO触发级别的情况下使用。

#### 2.4.3 发送数据函数

```c
void Rs485_SendBuffer(unsigned char *buffer, unsigned int length);
```

**功能**：发送数据到RS485发送缓冲区，等待发送完成。

**参数**：

- `buffer`：数据缓冲区。
- `length`：数据长度。


**使用场景**：用于发送数据到RS485。

#### 2.4.4 设置接收缓冲区函数

```c
void Rs485_SetRxBuffer(unsigned int length);
```

**功能**：设置RS485接收缓冲区的长度，用于接收数据。

**参数**：

- `length`：缓冲区长度。


**使用场景**：在使用RS485接收数据前调用，设置接收缓冲区的大小。

#### 2.4.5 设置传输模式函数

```c
void Rs485_SetMode(unsigned int mode);
```

**功能**：设置RS485的传输模式，切换接收或发送模式。

**参数**：

- `mode`：传输模式，可使用 `RS485_MODE_RX` 或 `RS485_MODE_TX`。


**使用场景**：在发送或接收数据前调用，切换RS485的工作模式。

## 3. 使用示例

### 3.1 基本使用示例

以下示例展示了如何初始化和使用RS485：

```c
#include "system.h"
#include "rs485_api.h"

void Rs485_Example(void)
{
    unsigned char sendData[] = "Hello, RS485!";
    
    // 初始化系统
    Sys_Init();
    
    // 初始化RS485，波特率9600，FIFO触发级别8
    Rs485_Init(RS485_BAUD_9600, RS485_FIFO_8);
    
    // 设置接收缓冲区长度为32字节
    Rs485_SetRxBuffer(32);
    
    for(;;)
    {
        // 设置为发送模式
        Rs485_SetMode(RS485_MODE_TX);
        
        // 发送数据
        Rs485_SendBuffer(sendData, sizeof(sendData));
        
        // 等待发送完成
        while(!FLAG_UART1_TX);
        FLAG_UART1_TX = 0;
        
        // 设置为接收模式
        Rs485_SetMode(RS485_MODE_RX);
        
        // 检查接收中断
        if(FLAG_UART1_RX || FLAG_UART1_RX_CTI)
        {
            FLAG_UART1_RX = 0;
            FLAG_UART1_RX_CTI = 0;
            
            // 处理接收到的数据
            // 数据存储在 Uart1_rxBuffer 中，长度为 Uart1_rxCount
        }
    }
}
```

### 3.2 中断使用示例

以下示例展示了如何在使用中断的情况下使用RS485：

```c
#include "system.h"
#include "rs485_api.h"

void Rs485_Example_Interrupt(void)
{
    unsigned char txBuffer[] = "Hello from RS485 interrupt mode!\n";
    
    // 系统初始化
    Sys_Init();
    
    // RS485初始化
    Rs485_Init(RS485_BAUD_115200, RS485_FIFO_8);
    
    // 设置接收缓冲区长度
    Rs485_SetRxBuffer(64);
    
    while(1)
    {
        // 检查发送完成标志
        if(FLAG_UART1_TX)
        {
            FLAG_UART1_TX = 0;
            
            // 发送完成后的处理
        }
        
        // 检查接收中断
        if(FLAG_UART1_RX)
        {
            FLAG_UART1_RX = 0;
            
            // 处理FIFO触发中断数据
        }
        
        // 检查接收超时中断
        if(FLAG_UART1_RX_CTI)
        {
            FLAG_UART1_RX_CTI = 0;
            
            // 处理不完整的数据包
        }
    }
}
```

## 4. 问题解答

### 4.1 常见问题

#### Q1: RS485初始化失败怎么办？

**A1**：请按照以下分层排查顺序进行检查：

- **应用层**：是否调用了 `Sys_Init()` 函数初始化中断？是否调用了 `Rs485_Init()` 函数初始化RS485？参数是否正确？
- **设备层**：请参考 `rs485_api.h` 中的接口定义，确保API调用正确。
- **硬件层**：检查RS485引脚连接是否正确，波特率设置是否与通信设备匹配。

#### Q2: RS485无法接收数据怎么办？

**A2**：请按照以下分层排查顺序进行检查：

- **应用层**：是否设置了接收缓冲区长度？是否正确检查了接收标志位？是否设置为接收模式？
- **设备层**：请参考 `rs485_api.h` 中的接口定义，确保API调用正确。
- **硬件层**：检查RS485接收引脚连接是否正确，通信设备是否发送了数据。

#### Q3: RS485无法发送数据怎么办？

**A3**：请按照以下分层排查顺序进行检查：

- **应用层**：是否正确调用了 `Rs485_SendBuffer()` 函数？是否设置为发送模式？发送缓冲区是否正确设置？
- **设备层**：请参考 `rs485_api.h` 中的接口定义，确保API调用正确。
- **硬件层**：检查RS485发送引脚连接是否正确，通信设备是否正常接收。

#### Q4: 如何选择合适的FIFO触发级别？

**A4**：FIFO触发级别的选择取决于应用场景：

- 低触发级别（1、4）：适用于需要快速响应的场景，但会增加中断频率。
- 高触发级别（8、14）：适用于大数据量传输的场景，减少中断频率，提高效率。

#### Q5: RS485是半双工通信，如何避免数据冲突？

**A5**：在使用RS485时，需要注意以下几点：

- 发送数据前必须切换到发送模式，发送完成后必须切换回接收模式。
- 通信双方需要协商好通信协议，避免同时发送数据。
- 可以使用主从模式，由主机控制通信时序。

### 4.2 故障排查

- **应用层**：请基于 `rs485_api.h` 中的内容进行测试，确保API调用正确。
- **设备层**：如果遇到问题，请阅读驱动部分代码尝试排查或咨询工程师。
- **硬件层**：检查RS485引脚连接和波特率设置。

## 5. 总结

RS485 API提供了简单易用的接口，用于实现半双工串行通信。通过使用 `Rs485_Init()` 或 `Rs485_Init_Lite()` 函数初始化RS485，然后使用 `Rs485_SetMode()` 切换模式，使用 `Rs485_SendBuffer()` 发送数据，通过检查标志位来接收数据，可以实现各种RS485通信功能。

本指南介绍了RS485设备的基本信息、API接口的使用方法和常见问题的解答，希望能够帮助开发者快速上手RS485控制功能，编写出高质量的应用程序。