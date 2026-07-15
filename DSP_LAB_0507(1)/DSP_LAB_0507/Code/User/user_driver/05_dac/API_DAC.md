# DAC API使用指南

## 1. 设备介绍

### 1.1 DAC设备概述

DAC（数模转换器）是DSP系统的“执行器”，是ADC的逆过程。

它负责将DSP算法处理后的数字信号还原为模拟电压或电流，以驱动扬声器、电机、传感器等外部设备。

在本项目中，我们使用的是一款8通道DAC，是数字世界控制物理世界的出口。

### 1.2 DAC设备参数

| 参数 | 值 | 说明 |
|------|------|------|
| 通道数 | 8 | 支持8个通道同步输出，适用于多路信号生成场景。 |
| 分辨率 | 16位 | DAC转换精度 |
| 输出频率 | 50kHz | 可配置的输出频率（八通道输出时最高支持50kHz） |
| 电压量程 | ±5V | 由芯片型号和外接电路决定，无法更改 |

### 1.3 DAC设备应用原理

DAC的工作流程是ADC的逆过程：
1. 数据准备：CPU将处理好的数字信号（通常为float类型）映射并转换为16位short类型。
2. 数据传输：数据通过SPI接口发送至DAC芯片。
3. 信号重建：DAC芯片根据接收到的数字值，输出对应的模拟电压。通过外部触发信号，可实现多通道的同步更新。

### 1.4 DAC数据缓冲区

DAC数据缓冲区是DAC设备的一个重要组件，用于输出存储数据。缓冲区通常由PRU控制器和CPU访问。

数据类型：无符号16位数据类型，每个数据点占用2个字节。

数据缓冲区通常分为A和B两个部分，每个部分都有自己的地址空间，用于存储当前和上一帧数据。

### 1.5 乒乓缓冲原理

与ADC类似，DAC也采用乒乓缓冲设计，确保数据输出的连续性：
- 工作流程：CPU向非活跃的缓冲区（如缓冲区B）写入新的输出数据，同时PRU从活跃的缓冲区（如缓冲区A）读取数据并发送至DAC。下一帧则切换缓冲区。
- 优势：避免了CPU写入数据时对当前输出造成的中断，保证了模拟信号的平滑过渡。


### 1.6 DAC输出频率

DAC8568是SPI通信串行通信的，输出频率与通道数成反比，8通道同步输出频率最大为50kHz。

PRU需要在DAC输出周期内，完成8通道的数据打包、发送和触发。50kHz的输出频率，几乎达到PRU性能的极限。

减少输出通道数可以提高单个通道的最高输出频率（需要联系工程师调整PRU代码）。

### 1.7 DAC输出长度

从技术上，SPI驱动的DAC输出可以更灵活，输出长度不受限制。

为了工程一致性，将DAC最大输出长度和ADC接口设计为相同的0x4000字节。

### 1.8 DAC输出周期

输出周期的计算方式与采样周期一致，是DSP系统实时性的另一个关键指标。

计算公式：输出周期 = (输出长度) / (输出频率)

示例：当输出频率为50kHz，输出长度为1024时，输出周期为 1024 / 50000 = 20.48ms。您的DSP算法必须在此周期内准备好下一批数据。

### 1.9 CPU占用情况

DAC模块在配置完成后，其数据发送和输出过程同样不占用CPU资源。

PRU负责从乒乓缓冲区读取数据，通过SPI发送至DAC，并在定时器中断时触发输出。

CPU仅需在缓冲区准备好后更新数据，专注于算法计算。

## 2. 接口结构

### 2.1 头文件包含

使用DAC API前，需要包含以下头文件：

```c
#include "dac_api.h"
```

### 2.2 宏定义

#### 2.2.1 输出频率宏定义

| 宏名称 | 值 | 说明 |
|-------|------|------|
| `DAC_10KHZ` | 10000 | 10kHz输出率 |
| `DAC_20KHZ` | 20000 | 20kHz输出率 |
| `DAC_25KHZ` | 25000 | 25kHz输出率 |
| `DAC_50KHZ` | 50000 | 50kHz输出率 |

#### 2.2.2 输出长度宏定义

| 宏名称 | 值 | 说明 |
|-------|------|------|
| `DAC_SAMPLE_128` | 128 | 128个输出点 |
| `DAC_SAMPLE_256` | 256 | 256个输出点 |
| `DAC_SAMPLE_512` | 512 | 512个输出点 |
| `DAC_SAMPLE_1024` | 1024 | 1024个输出点 |

#### 2.2.3 通道宏定义

| 宏名称 | 值 | 说明 |
|-------|------|------|
| `DAC_CHANNEL_1` | 0x01 | 通道1 |
| `DAC_CHANNEL_2` | 0x02 | 通道2 |
| `DAC_CHANNEL_3` | 0x04 | 通道3 |
| `DAC_CHANNEL_4` | 0x08 | 通道4 |
| `DAC_CHANNEL_5` | 0x10 | 通道5 |
| `DAC_CHANNEL_6` | 0x20 | 通道6 |
| `DAC_CHANNEL_7` | 0x40 | 通道7 |
| `DAC_CHANNEL_8` | 0x80 | 通道8 |
| `DAC_CHANNEL_12` | 0x03 | 通道1-2 |
| `DAC_CHANNEL_34` | 0x0C | 通道3-4 |
| `DAC_CHANNEL_56` | 0x30 | 通道5-6 |
| `DAC_CHANNEL_78` | 0x60 | 通道7-8 |
| `DAC_CHANNEL_1234` | 0x0F | 通道1-4 |
| `DAC_CHANNEL_5678` | 0xF0 | 通道5-8 |
| `DAC_CHANNEL_ALL` | 0xFF | 所有通道 |

#### 2.2.4 缓冲区类型宏定义

| 宏名称 | 值 | 说明 |
|-------|------|------|
| `DA_BUFFER_PING` | 0 | Ping缓冲区 |
| `DA_BUFFER_PONG` | 1 | Pong缓冲区 |


### 2.3 全局变量

#### 2.3.1 DAC标志位

| 变量名 | 类型 | 说明 | 地址 |
|-------|------|------|------|
| `DA_Len`          | `volatile far unsigned short` | DAC输出长度           | 0xC6FFFFF8 |
| `DA_En`           | `volatile far unsigned char`  | DAC使能标志           | 0xC6FFFFFA |
| `FLAG_DA`         | `volatile far unsigned char`  | DAC输出完成标志位      | 0xC6FFFFFB |
| `DA_Ping_Pong`    | `volatile far unsigned char`  | DAC Ping/Pong标志位   | 0xC6FFFFFC |
| `DA_Sel`          | `volatile far unsigned char`  | DAC通道选择           | 0xC6FFFFFD |

#### 2.3.2 DAC数据接口

| 变量名 | 类型 | 说明 | 地址 |
|-------|------|------|------|
| `DA_CH1_Buf0` | `short[]` | 通道1 Ping缓冲区 | 0xC7040000 |
| `DA_CH2_Buf0` | `short[]` | 通道2 Ping缓冲区 | 0xC7044000 |
| `DA_CH3_Buf0` | `short[]` | 通道3 Ping缓冲区 | 0xC7048000 |
| `DA_CH4_Buf0` | `short[]` | 通道4 Ping缓冲区 | 0xC704C000 |
| `DA_CH5_Buf0` | `short[]` | 通道5 Ping缓冲区 | 0xC7050000 |
| `DA_CH6_Buf0` | `short[]` | 通道6 Ping缓冲区 | 0xC7054000 |
| `DA_CH7_Buf0` | `short[]` | 通道7 Ping缓冲区 | 0xC7068000 |
| `DA_CH8_Buf0` | `short[]` | 通道8 Ping缓冲区 | 0xC706C000 |

| 变量名 | 类型 | 说明 | 地址 |
|-------|------|------|------|
| `DA_CH1_Buf1` | `short[]` | 通道1 Pong缓冲区 | 0xC7060000 |
| `DA_CH2_Buf1` | `short[]` | 通道2 Pong缓冲区 | 0xC7064000 |
| `DA_CH3_Buf1` | `short[]` | 通道3 Pong缓冲区 | 0xC7068000 |
| `DA_CH4_Buf1` | `short[]` | 通道4 Pong缓冲区 | 0xC706C000 |
| `DA_CH5_Buf1` | `short[]` | 通道5 Pong缓冲区 | 0xC7070000 |
| `DA_CH6_Buf1` | `short[]` | 通道6 Pong缓冲区 | 0xC7074000 |
| `DA_CH7_Buf1` | `short[]` | 通道7 Pong缓冲区 | 0xC7078000 |
| `DA_CH8_Buf1` | `short[]` | 通道8 Pong缓冲区 | 0xC707C000 |

### 2.4 函数接口

#### 2.4.1 DAC初始化函数

```c
void Dac_Init(unsigned int clkFreq, unsigned int sampleLen, unsigned char dacSel);
```

**功能**：初始化DAC模块，配置输出频率、输出长度和通道选择。

**参数**：
- `clkFreq`：输出频率，可使用 `DAC_10KHZ`、`DAC_20KHZ`、`DAC_25KHZ` 或 `DAC_50KHZ`。
- `sampleLen`：输出长度，可使用 `DAC_SAMPLE_128`、`DAC_SAMPLE_256`、`DAC_SAMPLE_512` 或 `DAC_SAMPLE_1024`。
- `dacSel`：通道选择，可使用 `DAC_CHANNEL_1` 到 `DAC_CHANNEL_ALL` 中的任意组合。


**使用场景**：在使用DAC前必须调用此函数，通常在系统初始化时调用。

#### 2.4.2 DAC启动函数

```c
void Dac_Start(void);
```

**功能**：启动DAC输出。

**使用场景**：在需要开始DAC输出时调用。

#### 2.4.3 DAC停止函数

```c
void Dac_Stop(void);
```

**功能**：停止DAC输出。

**使用场景**：在需要停止DAC输出时调用。

## 3. 使用示例

以下示例参考自 `user_dac.c` 文件，展示了如何初始化和使用DAC：

该程序的现象是，8通道DAC输出正弦波数据。可以通过按键控制DAC的启动和停止。

```c
#include "dac_api.h"
#include "system.h"
#include "math.h"
#include "key_api.h"

// 定义用户缓冲区
static short User_Buffer[DAC_SAMPLE_1024];

// DAC 输出示例
// 展示如何初始化和使用DAC进行数据输出
void Dac_Example(void)
{
    unsigned int i = 0;

    // 初始化系统
    Sys_Init();
    
    // 初始化按键
    Key_Init();
    
    // 初始化 DAC，设置输出频率为 50kHz，输出长度为 1024，通道选择为全部通道
    Dac_Init(DAC_50KHZ, DAC_SAMPLE_1024, DAC_CHANNEL_ALL);

    // 开始 DAC 输出
    Dac_Start();

    // 准备输出数据
    // 示例：生成正弦波数据
    for(i = 0; i < DAC_SAMPLE_1024; i++)
    {
        // 生成 0-65535 范围的正弦波数据
        User_Buffer[i] = (short)(32767.0f * sin(2 * 3.14159 * i / DAC_SAMPLE_1024));
    }
    
    while(1)
    {
        // 检查 DAC 输出完成标志
        if (FLAG_DA == 1)
        {
            // 清除标志位
            FLAG_DA = 0;
            
            // 根据 Ping-Pong 标志确定当前使用的缓冲区
            if (DA_Ping_Pong == DA_BUFFER_PONG)
            {
                // 使用 Ping 缓冲区数据
                // 可以在这里更新 Ping 缓冲区的数据
                memcpy(DA_CH1_Buf0, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH2_Buf0, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH3_Buf0, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH4_Buf0, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH5_Buf0, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH6_Buf0, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH7_Buf0, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH8_Buf0, &User_Buffer, 2*DAC_SAMPLE_1024);
            }
            else
            {
                // 使用 Pong 缓冲区数据
                // 可以在这里更新 Pong 缓冲区的数据
                memcpy(DA_CH1_Buf1, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH2_Buf1, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH3_Buf1, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH4_Buf1, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH5_Buf1, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH6_Buf1, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH7_Buf1, &User_Buffer, 2*DAC_SAMPLE_1024);
                memcpy(DA_CH8_Buf1, &User_Buffer, 2*DAC_SAMPLE_1024);
            }
        }
        // 检查按键标志位
        if (FLAG_KEY1 == 1)
        {
            FLAG_KEY1 = 0;
            // 开始 DAC 输出
            Dac_Start();
        }
        // 检查按键标志位
        if (FLAG_KEY2 == 1)
        {
            FLAG_KEY2 = 0;
            // 停止 DAC 输出
            Dac_Stop();
        }
    }
}
```

## 4. 问题解答

### 4.1 常见问题

#### Q1: DAC无法输出数据怎么办？

**A1**：请按照以下分层排查顺序进行检查：
- **应用层**：是否调用了 `Dac_Init()` 函数初始化DAC，参数是否正确？是否调用了 `Dac_Start()` 函数启动DAC？是否正确检查了 `FLAG_DA` 标志位？
- **设备层**：请参考 `dac_api.h` 中的接口定义，确保API调用正确。
- **硬件层**：检查DAC引脚连接是否正确，电源是否正常。

#### Q2: DAC输出数据不正确怎么办？

**A2**：请按照以下分层排查顺序进行检查：
- **应用层**：是否正确处理了 Ping-Pong 缓冲区？是否使用了正确的缓冲区地址？
- **设备层**：请参考 `dac_api.h` 中的接口定义，确保API调用正确。
- **硬件层**：检查DAC引脚连接是否正确，输出频率设置是否合理，输出通道选择是否正确。

#### Q3: 如何选择合适的输出频率？

**A3**：输出频率的选择取决于应用场景：
- 低频信号：可以使用较低的输出频率（如10kHz），减少系统资源占用。
- 高频信号：需要使用较高的输出频率（如50kHz），确保信号不失真。
- 注意：八通道输出时最高支持50kHz。

#### Q4: 如何选择合适的输出长度？

**A4**：输出长度的选择取决于应用场景：
- 简单波形：可以使用较短的输出长度（如128），减少内存占用。
- 复杂波形：需要使用较长的输出长度（如1024），提供更精细的波形细节。

### 4.2 故障排查

- **应用层**：请基于 `dac_api.h` 和 `user_dac.c` 中的代码内容进行测试，确保API调用正确。
- **设备层**：如果遇到问题，请阅读驱动部分代码尝试排查或咨询工程师。
- **硬件层**：检查DAC引脚连接和电源供应。

## 5. 总结

DAC API提供了简单易用的接口，用于实现数字信号到模拟信号的转换。通过使用 `Dac_Init()` 函数初始化DAC，然后使用 `Dac_Start()` 启动输出，通过检查 `FLAG_DA` 标志位来更新输出数据，可以实现各种DAC输出功能。

本指南介绍了DAC设备的基本信息、API接口的使用方法和常见问题的解答，希望能够帮助开发者快速上手DAC控制功能，编写出高质量的应用程序。