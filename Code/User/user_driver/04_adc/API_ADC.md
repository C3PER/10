# ADC API使用指南

## 1. 设备介绍

### 1.1 ADC设备概述

ADC（模数转换器）是数字信号处理（DSP）系统的“感官”。

它负责将连续变化的模拟世界（如声音、温度、电压）转换为离散的数字信号，为后续的算法处理提供原始数据。

在本项目中，我们使用的是一款高性能的8通道ADC，是连接物理世界与数字算法的桥梁。

### 1.2 ADC设备参数

| 参数   | 值     | 说明                              |
| ---- | ----- | ------------------------------- |
| 通道数  | 8     | 支持8个通道同步采样，适用于多路信号采集场景。         |
| 分辨率  | 16位   | 提供65,536个量化等级，确保信号转换的精度。        |
| 采样频率 | 50kHz | 可配置的采样频率（不使用自带数字滤波功能最高支持200kHz） |
| 电压量程 | ±5V   | 更改电路电阻可调为±10V                   |

### 1.3 ADC设备工作原理

ADC的工作流程遵循经典的DSP信号链路：

1. 信号调理：外部模拟信号经过ADC芯片内部的抗混叠滤波器，去除高频噪声。
2. 采样与量化：ADC以设定的频率对信号进行采样，并将每个采样点的电压值量化为16位数字。
3. 数据传输：量化后的数据通过并行EMIF接口高速传输，并由EDMA（增强型直接内存访问）控制器自动搬移至内存。

### 1.4 ADC数据缓冲区

ADC数据缓冲区是ADC设备的一个重要组件，用于存储采集到的模拟信号数据。缓冲区通常由EDMA控制器和CPU访问。

数据类型：无符号16位数据类型，每个数据点占用2个字节。

数据缓冲区通常分为A和B两个部分，每个部分都有自己的地址空间，用于存储当前和上一帧数据。

### 1.5 乒乓缓冲机制

为避免CPU在数据处理时与高速数据流发生冲突，ADC接口采用了乒乓缓冲（Ping-Pong Buffer）设计：

- 工作流程：EDMA将新采集的数据写入缓冲区A，同时CPU从缓冲区B读取上一帧数据进行处理。下一帧数据则写入缓冲区B，CPU从缓冲区A读取，如此循环。
- 优势：实现了数据采集与处理的并行，确保了数据的连续性和实时性，是实时DSP系统的关键设计。

### 1.6 ADC采样频率

AD7606数据是并行传输的，使用EMIF接口一次读取16位数据，再通过EDMA将数据传输到设计的接口中。

采样频率受到芯片本身的配置和驱动程序的影响，经过测试，最少支持100kHz的采样频率。

由于DAC的输出频率受限为50kHz，又因为ADC高速采样达到极限200kHz比较勉强。因此开启AD7606的过采样功能，将采样频率控制到50kHz。

备注：AD7606内置的数字一阶sinc滤波器和过采样功能，开启后可以提升信噪比 (SNR)、扩展动态范围。更高的SNR意味着系统能够更清晰地分辨出微弱的信号，而不会被噪声淹没。

### 1.7 ADC采样长度

AD7606采集数据需要用到，EDMA的数据搬移功能。

由于EDMA搬移数据的跨度有限制，因此经过推算，单缓冲区最大支持8192个16位数据（0x4000字节）。

采样长度建议：虽然可以设置任意长度，但为了优化DSP算法（如FFT）的效率，强烈建议将采样长度设置为2的整数次幂（如128, 256, 512, 1024）。

### 1.8 ADC采样周期

采样周期是DSP系统设计的关键参数，它决定了算法必须在多长时间内完成处理。

计算公式：采样周期 = (采样长度) / (采样频率)

示例：当采样频率为50kHz，采样长度为1024时，采样周期为 1024 / 50000 = 20.48ms。这意味着您的DSP算法必须在20.48ms内完成处理，否则将导致数据丢失。

### 1.9 CPU资源占用分析

ADC模块在配置完成后，其数据采集和传输过程几乎不占用CPU资源。

数据由PRU（可编程实时单元）触发，通过EMIF和EDMA自动完成。

CPU仅在数据准备好后，从乒乓缓冲区读取数据进行处理，从而解放了CPU以专注于核心算法。

## 2. 接口结构

### 2.1 头文件包含

使用ADC API前，需要包含以下头文件：

```c
#include "adc_api.h"
```

### 2.2 宏定义

#### 2.2.1 采样频率宏定义

| 宏名称         | 值     | 说明       |
| ----------- | ----- | -------- |
| `ADC_10KHZ` | 10000 | 10kHz采样率 |
| `ADC_20KHZ` | 20000 | 20kHz采样率 |
| `ADC_50KHZ` | 50000 | 50kHz采样率 |

#### 2.2.2 采样长度宏定义

| 宏名称               | 值    | 说明       |
| ----------------- | ---- | -------- |
| `ADC_SAMPLE_128`  | 128  | 128个采样点  |
| `ADC_SAMPLE_256`  | 256  | 256个采样点  |
| `ADC_SAMPLE_512`  | 512  | 512个采样点  |
| `ADC_SAMPLE_1024` | 1024 | 1024个采样点 |

#### 2.2.3 缓冲区类型宏定义

| 宏名称              | 值 | 说明      |
| ---------------- | - | ------- |
| `AD_BUFFER_PING` | 0 | Ping缓冲区 |
| `AD_BUFFER_PONG` | 1 | Pong缓冲区 |

### 2.3 全局变量

#### 2.3.1 ADC标志位

| <br />         | 变量名                           | 类型               | 说明         |
| :------------- | ----------------------------- | ---------------- | ---------- |
| `AD_Len`       | `volatile far unsigned short` | ADC输出长度          | 0xC6FFFFF0 |
| `AD_En`        | `volatile far unsigned char`  | ADC使能标志          | 0xC6FFFFF2 |
| `FLAG_AD`      | `volatile far unsigned char`  | ADC采集完成标志位       | 0xC6FFFFF3 |
| `AD_Ping_Pong` | `volatile far unsigned char`  | ADC Ping/Pong标志位 | 0xC6FFFFF4 |

#### 2.3.2 ADC数据缓冲区

| 变量名           | 类型        | 说明          | 地址         |
| ------------- | --------- | ----------- | ---------- |
| `AD_CH1_Buf0` | `short[]` | 通道1 Ping缓冲区 | 0xC7000000 |
| `AD_CH2_Buf0` | `short[]` | 通道2 Ping缓冲区 | 0xC7004000 |
| `AD_CH3_Buf0` | `short[]` | 通道3 Ping缓冲区 | 0xC7008000 |
| `AD_CH4_Buf0` | `short[]` | 通道4 Ping缓冲区 | 0xC700C000 |
| `AD_CH5_Buf0` | `short[]` | 通道5 Ping缓冲区 | 0xC7010000 |
| `AD_CH6_Buf0` | `short[]` | 通道6 Ping缓冲区 | 0xC7014000 |
| `AD_CH7_Buf0` | `short[]` | 通道7 Ping缓冲区 | 0xC7018000 |
| `AD_CH8_Buf0` | `short[]` | 通道8 Ping缓冲区 | 0xC701C000 |

| 变量名           | 类型        | 说明          | 地址         |
| ------------- | --------- | ----------- | ---------- |
| `AD_CH1_Buf1` | `short[]` | 通道1 Pong缓冲区 | 0xC7020000 |
| `AD_CH2_Buf1` | `short[]` | 通道2 Pong缓冲区 | 0xC7024000 |
| `AD_CH3_Buf1` | `short[]` | 通道3 Pong缓冲区 | 0xC7028000 |
| `AD_CH4_Buf1` | `short[]` | 通道4 Pong缓冲区 | 0xC702C000 |
| `AD_CH5_Buf1` | `short[]` | 通道5 Pong缓冲区 | 0xC7030000 |
| `AD_CH6_Buf1` | `short[]` | 通道6 Pong缓冲区 | 0xC7034000 |
| `AD_CH7_Buf1` | `short[]` | 通道7 Pong缓冲区 | 0xC7038000 |
| `AD_CH8_Buf1` | `short[]` | 通道8 Pong缓冲区 | 0xC703C000 |

### 2.4 函数接口

#### 2.4.1 ADC初始化函数

```c
void Adc_Init(unsigned int clkFreq, unsigned int sampleLen);
```

**功能**：初始化ADC模块，配置采样频率和采样长度。

**参数**：

- `clkFreq`：采样频率，可使用 `ADC_10KHZ`、`ADC_20KHZ` 或 `ADC_50KHZ`。
- `sampleLen`：采样长度，可使用 `ADC_SAMPLE_128`、`ADC_SAMPLE_256`、`ADC_SAMPLE_512` 或 `ADC_SAMPLE_1024`。

**使用场景**：在使用ADC前必须调用此函数，通常在系统初始化时调用。

#### 2.4.2 ADC启动函数

```c
void Adc_Start(void);
```

**功能**：启动ADC采集。

**使用场景**：在需要开始ADC采集时调用。

#### 2.4.3 ADC停止函数

```c
void Adc_Stop(void);
```

**功能**：停止ADC采集。

**使用场景**：在需要停止ADC采集时调用。

## 3. 使用示例

以下示例参考自 `user_adc.c` 文件，展示了如何初始化和使用ADC：

该程序的现象是，8通道ADC采集数据。可以通过按键控制ADC的启动和停止。

需要使用CCS的Graph数据可视化工具，将采集到的数据可视化展示。

```c
#include "adc_api.h"
#include "system.h"
#include "key_api.h"

// 定义用户缓冲区
static short User_Buffer1[ADC_SAMPLE_1024];
static short User_Buffer2[ADC_SAMPLE_1024];
static short User_Buffer3[ADC_SAMPLE_1024];
static short User_Buffer4[ADC_SAMPLE_1024];
static short User_Buffer5[ADC_SAMPLE_1024];
static short User_Buffer6[ADC_SAMPLE_1024];
static short User_Buffer7[ADC_SAMPLE_1024];
static short User_Buffer8[ADC_SAMPLE_1024];

// ADC 采样示例
// 展示如何初始化和使用ADC进行数据采集
void Adc_Example(void)
{
    // 初始化系统
    Sys_Init();

    // 初始化按键
    Key_Init();

    // 初始化 ADC，设置采样频率为 50kHz，采样长度为 1024
    Adc_Init(ADC_50KHZ, ADC_SAMPLE_1024);

    // 开始 ADC 采样
    Adc_Start();
    
    while(1)
    {
        // 检查 ADC 采集完成标志
        if (FLAG_AD == 1)
        {
            // 清除标志位
            FLAG_AD = 0;
            
            // 根据 Ping-Pong 标志确定当前使用的缓冲区
            if (AD_Ping_Pong == AD_BUFFER_PONG)
            {
                // 使用 Ping 缓冲区数据
                memcpy(&User_Buffer1, AD_CH1_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer2, AD_CH2_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer3, AD_CH3_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer4, AD_CH4_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer5, AD_CH5_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer6, AD_CH6_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer7, AD_CH7_Buf0, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer8, AD_CH8_Buf0, 2*ADC_SAMPLE_1024);
            }
            else
            {
                // 使用 Pong 缓冲区数据
                memcpy(&User_Buffer1, AD_CH1_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer2, AD_CH2_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer3, AD_CH3_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer4, AD_CH4_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer5, AD_CH5_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer6, AD_CH6_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer7, AD_CH7_Buf1, 2*ADC_SAMPLE_1024);
                memcpy(&User_Buffer8, AD_CH8_Buf1, 2*ADC_SAMPLE_1024);
            }
        }
        // 检查按键标志位
        if (FLAG_KEY1 == 1)
        {
            FLAG_KEY1 = 0;
            // 开始 ADC 采样
            Adc_Start();
        }
        // 检查按键标志位
        if (FLAG_KEY2 == 1)
        {
            FLAG_KEY2 = 0;
            // 停止 ADC 采样
            Adc_Stop();
        }
    }
}
```

## 4. 问题解答

### 4.1 常见问题

#### Q1: ADC采集不到数据怎么办？

**A1**：请按照以下分层排查顺序进行检查：

- **应用层**：是否调用了 `Adc_Init()` 函数初始化ADC，参数是否正确？是否调用了 `Adc_Start()` 函数启动ADC？是否正确检查了 `FLAG_AD` 标志位？
- **设备层**：请参考 `adc_api.h` 中的接口定义，确保API调用正确。
- **硬件层**：检查ADC引脚连接是否正确，采样频率设置是否合理，模拟信号输入是否正常。

#### Q2: ADC采集数据不正确怎么办？

**A2**：请按照以下分层排查顺序进行检查：

- **应用层**：是否正确处理了 Ping-Pong 缓冲区？是否使用了正确的缓冲区地址？
- **设备层**：请参考 `adc_api.h` 中的接口定义，确保API调用正确。
- **硬件层**：检查ADC引脚连接是否正确，模拟信号输入是否在ADC的输入范围内。

#### Q3: 如何选择合适的采样频率？

**A3**：采样频率的选择取决于应用场景：

- 低频信号：可以使用较低的采样频率（如10kHz），减少数据量，增加CPU计算算法时间。
- 高频信号：需要使用较高的采样频率（如50kHz），确保信号不失真。

#### Q4: 如何选择合适的采样长度？

**A4**：采样长度的选择取决于应用场景：

- 实时性要求高的场景：可以使用较短的采样长度（如128），减少数据处理时间。
- 需要更多数据进行分析的场景：可以使用较长的采样长度（如1024），提供更多的数据分析样本。

### 4.2 故障排查

- **应用层**：请基于 `adc_api.h` 和 `user_adc.c` 中的代码内容进行测试，确保API调用正确。
- **设备层**：如果遇到问题，请阅读驱动部分代码尝试排查或咨询工程师。
- **硬件层**：检查ADC引脚连接和模拟信号输入。

## 5. 总结

ADC API提供了简单易用的接口，用于实现模拟信号的采集。通过使用 `Adc_Init()` 函数初始化ADC，然后使用 `Adc_Start()` 启动采集，通过检查 `FLAG_AD` 标志位来获取采集完成的数据，可以实现各种ADC采集功能。

本指南介绍了ADC设备的基本信息、API接口的使用方法和常见问题的解答，希望能够帮助开发者快速上手ADC控制功能，编写出高质量的应用程序。
