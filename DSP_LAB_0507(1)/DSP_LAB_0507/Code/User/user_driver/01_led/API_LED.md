# LED API使用指南

## 1. 设备介绍

### 1.1 LED设备概述

本项目支持的LED设备包括：

- **开发板LED**：共4组，每组包含蓝色和红色两个LED，共8个LED。
- **核心板LED**：共2个LED，位于核心板上。

### 1.2 LED设备驱动原理

LED设备驱动原理基于GPIO引脚的控制。每个LED对应一个GPIO引脚，通过设置引脚的电平来控制LED的状态（点亮或关闭）。

## 2. 接口结构

### 2.1 头文件包含

使用LED API前，需要包含以下头文件：

```c
#include "led_api.h"
```

### 2.2 枚举类型

#### 2.2.1 LED索引枚举

```c
typedef enum {
    LED1_BLUE = 0,     //!< 开发板LED1蓝色
    LED1_RED,          //!< 开发板LED1红色
    LED2_BLUE,         //!< 开发板LED2蓝色
    LED2_RED,          //!< 开发板LED2红色
    LED3_BLUE,         //!< 开发板LED3蓝色
    LED3_RED,          //!< 开发板LED3红色
    LED4_BLUE,         //!< 开发板LED4蓝色
    LED4_RED,          //!< 开发板LED4红色
    LED1_CORE,         //!< 核心板LED1
    LED2_CORE,         //!< 核心板LED2
    LED_MAX            //!< LED数量上限
} LedIndex;
```

#### 2.2.2 LED状态枚举

```c
typedef enum {
    LED_ON = 0,        //!< LED点亮
    LED_OFF,           //!< LED关闭
    LED_TOGGLE         //!< LED状态翻转
} LedState;
```

### 2.3 函数接口

#### 2.3.1 LED初始化函数

```c
void Led_Init(void);
```

**功能**：初始化LED相关的GPIO引脚，设置为输出模式。

**使用场景**：在使用LED前必须调用此函数，通常在系统初始化时调用。

#### 2.3.2 LED控制函数

```c
void Led_Control(LedIndex ledNumber, LedState ledState);
```

**功能**：控制指定LED的状态，支持点亮、关闭和翻转操作。

**参数**：
- `ledNumber`：LED编号，使用 `LedIndex` 枚举值。
- `ledState`：状态，使用 `LedState` 枚举值。

**使用场景**：用于控制LED的状态，如点亮、关闭或翻转。

## 3. 使用示例

### 3.1 基本使用示例

以下示例参考自 `user_led.c` 文件，展示了如何初始化和控制LED。

LED灯控制亮灭的方法是：
- 在程序初始化阶段，调用初始化LED函数 `Led_Init()` 初始化LED。
- 在合适的时机，调用 `Led_Control` 函数控制指定LED的状态。

```c
#include "led_api.h"            // LED用户接口
#include "system.h"             // 中断和延时初始化
#include "delay.h"              // 延时函数

void Led_Example(void)
{
    // 初始化系统（开启中断并初始化delay函数）
    Sys_Init();
    // 初始化LED
    Led_Init();

    for (;;)
    {
        // 点亮核心板LED，翻转开发板蓝色LED
        Led_Control(LED1_CORE, LED_ON);
        Led_Control(LED2_CORE, LED_ON);
        Led_Control(LED1_BLUE, LED_TOGGLE);
        Led_Control(LED2_BLUE, LED_TOGGLE);
        Led_Control(LED3_BLUE, LED_TOGGLE);
        Led_Control(LED4_BLUE, LED_TOGGLE);
        delay(500);

        // 关闭核心板LED，翻转开发板红色LED
        Led_Control(LED1_CORE, LED_OFF);
        Led_Control(LED2_CORE, LED_OFF);
        Led_Control(LED1_RED,  LED_TOGGLE);
        Led_Control(LED2_RED,  LED_TOGGLE);
        Led_Control(LED3_RED,  LED_TOGGLE);
        Led_Control(LED4_RED,  LED_TOGGLE);
        delay(500);
    }
}
```

## 4. 问题解答

### 4.1 常见问题

#### Q1: LED不亮怎么办？

**A1**：请按照以下分层排查顺序进行检查：
- **应用层**：是否调用了 `Led_Init()` 函数初始化LED？LED索引是否正确？LED状态是否设置正确？
- **设备层**：请参考 `led_api.h` 中的接口定义，确保API调用正确。
- **硬件层**：使用万用表测量LED引脚的电压，确认硬件连接是否正常。

#### Q2: 如何同时控制多个LED？

**A2**：可以通过多次调用 `Led_Control` 函数来控制多个LED，每次控制一个LED的状态。

#### Q3: LED控制函数的执行时间是多少？

**A3**：`Led_Control` 函数的执行时间非常短，因为它只是简单地操作GPIO寄存器，通常在微秒级别。

#### Q4: 是否支持PWM控制LED亮度？

**A4**：当前版本的LED驱动不支持PWM控制，只能实现点亮、关闭和翻转操作。如果需要PWM控制，请咨询工程师。

### 4.2 故障排查

- **应用层**：请基于 `led_api.h` 和 `user_led.c` 中的代码内容进行测试，确保API调用正确。
- **设备层**：如果遇到问题，请阅读驱动部分代码尝试排查或咨询工程师。
- **硬件层**：使用万用表和示波器排查硬件问题。

## 5. 总结

LED API提供了简单易用的接口，用于控制开发板和核心板上的LED。通过使用 `Led_Init()` 函数初始化LED，然后使用 `Led_Control()` 函数控制LED的状态，可以实现各种LED控制效果。

本指南介绍了LED设备的基本信息、API接口的使用方法和常见问题的解答，希望能够帮助开发者快速上手LED控制功能，编写出高质量的应用程序。