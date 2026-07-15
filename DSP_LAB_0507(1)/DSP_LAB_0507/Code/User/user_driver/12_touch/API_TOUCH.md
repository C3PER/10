# TOUCH API使用指南

## 1. 设备介绍

### 1.1 Touch设备概述

Touch（触摸屏）是数字信号处理（DSP）系统的“触觉”。

它负责将用户的触摸操作转换为数字信号，为用户提供直观的交互方式。

在本项目中，我们使用的是GT1151电容式触摸屏控制器，支持多点触摸，是用户与系统交互的主要输入设备。

### 1.2 Touch设备参数

| 参数 | 值 | 说明 |
| ---- | ----- | ------------------------------- |
| 触摸点数 | 5点 | 支持最多5个触摸点同时检测，程序中仅使用1个触摸点。 |
| 接口类型 | I2C | 使用I2C接口与主控芯片通信。 |
| 中断方式 | 下降沿触发 | 触摸发生时触发中断，通知主控芯片。 |
| 分辨率 | 800x480 | 与LCD显示屏分辨率匹配。 |

### 1.3 Touch设备工作原理

Touch的工作流程遵循以下步骤：

1. **触摸检测**：GT1151芯片实时检测触摸屏上的触摸点。
2. **中断触发**：当检测到触摸时，GT1151触发中断引脚，通知主控芯片。
3. **数据读取**：主控芯片通过I2C接口读取触摸坐标数据。
4. **坐标处理**：应用层根据触摸坐标判断用户操作（如点击按钮）。

### 1.4 Touch中断机制

为实现实时响应，Touch接口采用了中断驱动设计：

- **中断触发**：触摸发生时，GT1151将中断引脚拉低，触发GPIO中断。
- **中断处理**：中断服务函数设置触摸标志位，通知应用层有触摸事件发生。
- **低功耗设计**：只有在触摸发生时才产生中断，平时不占用CPU资源。

## 2. 接口结构

### 2.1 头文件包含

使用Touch API前，需要包含以下头文件：

```c
#include "touch_api.h"
```

### 2.2 全局变量

#### 2.2.1 Touch标志位和状态

| 变量名 | 类型 | 说明 |
| ------- | ---- | ---- |
| `FLAG_TOUCH` | `unsigned char` | 触摸中断标志位，触摸发生时置1。 |
| `Touch_Sta` | `unsigned char` | 触摸状态，0表示未触摸，1表示触摸中。 |
| `Touch_X` | `unsigned int` | 触摸点的X坐标。 |
| `Touch_Y` | `unsigned int` | 触摸点的Y坐标。 |

#### 2.2.2 Touch按钮标志位

| 变量名 | 类型 | 说明 |
| ------- | ---- | ---- |
| `FLAG_BUTTON` | `volatile unsigned char` | 通用按钮标志位。 |
| `FLAG_BUTTON_1` | `volatile unsigned char` | 按钮1标志位。 |
| `FLAG_BUTTON_2` | `volatile unsigned char` | 按钮2标志位。 |
| `FLAG_BUTTON_3` | `volatile unsigned char` | 按钮3标志位。 |
| `FLAG_BUTTON_4` | `volatile unsigned char` | 按钮4标志位。 |
| `FLAG_BUTTON_5` | `volatile unsigned char` | 按钮5标志位。 |
| `FLAG_BUTTON_6` | `volatile unsigned char` | 按钮6标志位。 |
| `FLAG_BUTTON_7` | `volatile unsigned char` | 按钮7标志位。 |
| `FLAG_BUTTON_8` | `volatile unsigned char` | 按钮8标志位。 |

### 2.3 函数接口

#### 2.3.1 Touch初始化函数

```c
void Touch_Init(void);
```

**功能**：初始化Touch相关的IIC接口、引脚和GT1151芯片。

**使用场景**：在使用Touch前必须调用此函数，通常在系统初始化时调用。

#### 2.3.2 Touch扫描函数

```c
void Touch_Scan(void);
```

**功能**：扫描触摸屏状态，更新触摸坐标和按钮标志位。

**使用场景**：在检测到触摸中断后调用，读取触摸数据。

## 3. 使用示例

以下示例展示了如何初始化和使用Touch：

```c
#include "touch_api.h"            // Touch用户接口
#include "lcd_api.h"              // LCD用户接口
#include "led_api.h"              // LED用户接口
#include "system.h"               // 系统初始化

void Touch_Example(void)
{
    char Buffer[64];
    
    // 初始化系统
    Sys_Init();
    
    // 初始化LED
    Led_Init();
    
    // 初始化LCD
    Lcd_Init();
    
    // 初始化控件
    Lcd_Widget_Init();
    
    // 初始化Touch
    Touch_Init();
    
    for (;;)
    {
        // 检测触摸中断
        if (FLAG_TOUCH)
        {
            FLAG_TOUCH = 0;
            Touch_Scan();
        }

        // 处理按钮1事件
        if (FLAG_BUTTON_1)
        {
            FLAG_BUTTON_1 = 0;
            Led_Control(LED1_BLUE, LED_TOGGLE);
            CanvasTextSet(&g_sTxt1, "Button_1_Press");
            WidgetPaint((tWidget *)&g_sTxt1);
            WidgetMessageQueueProcess();
        }
        
        // 处理按钮2事件
        if (FLAG_BUTTON_2)
        {
            FLAG_BUTTON_2 = 0;
            Led_Control(LED1_RED, LED_TOGGLE);
            CanvasTextSet(&g_sTxt1, "Button_2_Press");
            WidgetPaint((tWidget *)&g_sTxt1);
            WidgetMessageQueueProcess();
        }
        
        // 处理其他按钮事件...
    }
}
```

## 4. 问题解答

### 4.1 常见问题

#### Q1: Touch不响应怎么办？

**A1**：请按照以下分层排查顺序进行检查：
- **应用层**：是否调用了`Touch_Init()`函数初始化Touch？是否正确检查了`FLAG_TOUCH`标志位？
- **设备层**：请参考`touch_api.h`中的接口定义，确保API调用正确。
- **硬件层**：检查Touch引脚连接是否正确，I2C总线是否正常，GT1151芯片是否正常工作。

#### Q2: 如何实现多点触摸？

**A2**：当前版本的Touch驱动支持单点触摸。如果需要多点触摸功能，需要修改扫描函数，读取多个触摸点的坐标数据。

#### Q3: Touch中断过于频繁怎么办？

**A3**：手指触摸后尽量快一些离开屏幕，减少IIC通信次数。如需较快的响应，需要使用KEY按键功能。

### 4.2 故障排查

- **应用层**：请基于`touch_api.h`和`user_touch.c`中的代码内容进行测试，确保API调用正确。
- **设备层**：如果遇到问题，请阅读驱动部分代码尝试排查或咨询工程师。
- **硬件层**：检查Touch引脚连接、I2C总线和GT1151芯片。

## 5. 总结

Touch API提供了简单易用的接口，用于实现触摸交互功能。通过使用`Touch_Init()`函数初始化Touch，然后通过检查`FLAG_TOUCH`标志位检测触摸事件，调用`Touch_Scan()`函数读取触摸数据，可以实现各种触摸交互效果。

本指南介绍了Touch设备的基本信息、API接口的使用方法和常见问题的解答，希望能够帮助开发者快速上手Touch控制功能，编写出高质量的应用程序。
