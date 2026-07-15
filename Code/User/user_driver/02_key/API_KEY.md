# KEY API使用指南

## 1. 设备介绍

### 1.1 KEY设备概述

本项目支持的按键设备包括：

- **开发板按键**：共5个按键，分别为KEY1、KEY2、KEY3、KEY4、KEY5。

### 1.2 KEY设备驱动原理

KEY设备驱动原理基于GPIO引脚的控制。每个按键对应一个GPIO引脚，通过设置引脚的电平来检测按键状态（按下或松开）。程序中为每个按键引脚开启了中断功能，优化了按键检测的效率。

## 2. 接口结构

### 2.1 头文件包含

使用KEY API前，需要包含以下头文件：

```c
#include "key_api.h"
```

### 2.2 全局变量

| 变量名 | 类型 | 说明 |
|-------|------|------|
| `FLAG_KEY1` | `volatile unsigned char` | KEY1按键标志位，按键按下时置1 |
| `FLAG_KEY2` | `volatile unsigned char` | KEY2按键标志位，按键按下时置1 |
| `FLAG_KEY3` | `volatile unsigned char` | KEY3按键标志位，按键按下时置1 |
| `FLAG_KEY4` | `volatile unsigned char` | KEY4按键标志位，按键按下时置1 |
| `FLAG_KEY5` | `volatile unsigned char` | KEY5按键标志位，按键按下时置1 |

### 2.3 函数接口

#### 2.3.1 按键初始化函数

```c
void Key_Init(void);
```

**功能**：初始化按键相关的GPIO引脚和中断，使能按键功能。

**使用场景**：在使用按键前必须调用此函数，通常在系统初始化时调用。

## 3. 使用示例

### 3.1 基本使用示例

以下示例参考自 `user_key.c` 文件，展示了如何初始化和使用按键。

按键标志位的使用方法是：
- 在程序初始化阶段，调用初始化按键函数 `Key_Init()` 初始化按键。
- 在主循环中检查按键标志位是否为1，若为1则执行对应操作。
- 执行对应操作后，将按键标志位设置为0，避免重复执行。

```c
#include "key_api.h"            // 按键用户接口
#include "led_api.h"            // LED用户接口
#include "system.h"             // 中断和延时初始化
#include "delay.h"              // 延时函数

void Key_Example(void)
{
    // 初始化系统（开启中断并初始化delay函数）
    Sys_Init();
    // 初始化LED
    Led_Init();
    // 初始化按键
    Key_Init();

    for (;;)
    {
        // 检查按键标志位是否为1
        if (FLAG_KEY1)
        {
            // 将按键标志位设置为0
            FLAG_KEY1 = 0;
            Led_Control(LED1_BLUE, LED_TOGGLE);
            Led_Control(LED1_RED, LED_TOGGLE);
        }
        if (FLAG_KEY2)
        {
            FLAG_KEY2 = 0;
            Led_Control(LED2_BLUE, LED_TOGGLE);
            Led_Control(LED2_RED, LED_TOGGLE);
        }
        // 省略部分程序
    }
}
```

### 3.2 模式切换示例

以下示例展示了如何使用按键切换模式。

按键标志位切换状态的方法是：
- 创建一个模式变量，用于存储当前模式。
- 每次按键按下时，切换模式变量的值。
- 根据模式变量的值，执行不同的操作。

```c
#include "key_api.h"            // 按键用户接口
#include "led_api.h"            // LED用户接口
#include "system.h"             // 中断和延时初始化
#include "delay.h"              // 延时函数

void Key_Example2(void)
{
    // 定义模式标志位
    unsigned char MODE_KEY1=0;
    // 初始化系统（开启中断并初始化delay函数）
    Sys_Init();
    // 初始化LED
    Led_Init();
    // 初始化按键
    Key_Init();
    
    for (;;)
    {
        // 使用按键切换模式
        if(FLAG_KEY1)
        {
            FLAG_KEY1 = 0;
            MODE_KEY1 = (MODE_KEY1 + 1) % 2;
        }
        // 不同模式执行不同函数
        if(MODE_KEY1){
            Led_Control(LED1_BLUE, LED_TOGGLE);
            Led_Control(LED2_BLUE, LED_TOGGLE);
            Led_Control(LED3_BLUE, LED_TOGGLE);
            Led_Control(LED4_BLUE, LED_TOGGLE);
        }
        else{
            Led_Control(LED1_RED, LED_TOGGLE);
            Led_Control(LED2_RED, LED_TOGGLE);
            Led_Control(LED3_RED, LED_TOGGLE);
            Led_Control(LED4_RED, LED_TOGGLE);
        }
        delay(500);
    }
}
```

## 4. 问题解答

### 4.1 常见问题

#### Q1: 按键按下后标志位不置1怎么办？

**A1**：请按照以下分层排查顺序进行检查：
- **应用层**：是否调用了 `Sys_Init()` 函数初始化中断功能？是否调用了 `Key_Init()` 函数初始化按键？是否正确检查了标志位？
- **设备层**：请参考 `key_api.h` 中的接口定义，确保API调用正确。
- **硬件层**：使用万用表测量按键引脚的电压，确认按键硬件连接是否正常。

#### Q2: 按键按下后标志位一直为1怎么办？

**A2**：请按照以下分层排查顺序进行检查：
- **应用层**：是否在检查标志位后将其清零？
- **设备层**：请参考 `key_api.h` 中的接口定义，确保API调用正确。
- **硬件层**：检查按键是否卡住，导致一直处于按下状态。

#### Q3: 如何实现按键消抖？

**A3**：由于使用中断加轮询的方法，按键对应的事件需要在轮询中等待执行。因此无需在应用层中实现按键消抖。

#### Q4: 如何实现长按功能？

**A4**：如果添加长按功能，会大量占用CPU资源。不建议添加。


### 4.2 故障排查

- **应用层**：请基于 `key_api.h` 和 `user_key.c` 中的代码内容进行测试，确保API调用正确。
- **设备层**：如果遇到问题，请阅读驱动部分代码尝试排查或咨询工程师。
- **硬件层**：使用万用表和示波器排查硬件问题。

## 5. 总结

KEY API提供了简单易用的接口，用于检测开发板上的按键状态。通过使用 `Key_Init()` 函数初始化按键，然后通过检查标志位来响应按键事件，可以实现各种按键控制功能。

本指南介绍了KEY设备的基本信息、API接口的使用方法和常见问题的解答，希望能够帮助开发者快速上手按键控制功能，编写出高质量的应用程序。