# LCD API使用指南

## 1. 设备介绍

### 1.1 LCD设备概述

LCD（液晶显示器）是数字信号处理（DSP）系统的“窗口”。

它负责将数字信号转换为可视化的图像，为用户提供直观的交互界面。

在本项目中，我们使用的是一款800x480分辨率的LCD显示屏，是用户与系统交互的主要界面。

### 1.2 LCD设备参数

| 参数 | 值 | 说明 |
| ---- | ----- | ------------------------------- |
| 分辨率 | 800x480 | 支持800像素宽、480像素高的显示分辨率。 |
| 颜色深度 | 16位 | 提供65,536种颜色，确保图像显示的丰富性。 |
| 显示接口 | RGB | 使用RGB并行接口，高速传输图像数据。 |
| 显示模式 | 单缓冲 | 使用单帧缓冲机制，简化系统设计。 |

### 1.3 LCD设备工作原理

LCD的工作流程遵循经典的显示链路：

1. **帧缓冲准备**：应用程序将图像数据写入帧缓冲区。
2. **DMA传输**：DMA控制器自动将帧缓冲区的数据传输到LCD控制器。
3. **显示刷新**：LCD控制器按照配置的时序参数，将数据转换为屏幕显示。

### 1.4 LCD单缓冲机制

为简化系统设计，LCD接口采用了单缓冲设计：

- **工作流程**：应用程序直接在帧缓冲区中绘制图像，LCD控制器从同一缓冲区读取数据进行显示。
- **优势**：简化了系统设计，降低了内存占用，适用于非高速图像处理场景。

## 2. 接口结构

### 2.1 头文件包含

使用LCD API前，需要包含以下头文件：

```c
#include "lcd_api.h"
```

### 2.2 全局变量

#### 2.2.1 LCD显示对象和上下文

| 变量名 | 类型 | 说明 |
| ------- | ---- | ---- |
| `Lcd_Display` | `tDisplay` | LCD显示对象，封装硬件操作函数。 |
| `Lcd_Context` | `tContext` | LCD绘图上下文，保存绘图参数。 |
| `Lcd_Rectangle` | `tRectangle` | 矩形区域结构，用于绘制矩形和简单清屏。 |

#### 2.2.2 LCD UI控件

| 变量名 | 类型 | 说明 |
| ------- | ---- | ---- |
| `g_sMainPanel` | `tContainerWidget` | 主面板容器控件。 |
| `g_sHeading` | `tCanvasWidget` | 标题栏画布控件。 |
| `g_sTxt1` | `tCanvasWidget` | 文本1画布控件。 |
| `g_sTxt2` | `tCanvasWidget` | 文本2画布控件。 |
| `g_sBtn1` ~ `g_sBtn8` | `tPushButtonWidget` | 按钮控件1-8。 |

### 2.3 函数接口

#### 2.3.1 LCD初始化函数

```c
void Lcd_Init(void);
```

**功能**：初始化LCD显示系统，包括引脚、控制器、光栅、DMA和图形库。

**使用场景**：在使用LCD前必须调用此函数，通常在系统初始化时调用。

#### 2.3.2 LCD控件初始化函数

```c
void Lcd_Widget_Init(void);
```

**功能**：创建UI控件并初始化显示。

**使用场景**：在需要使用UI控件时调用，通常在`Lcd_Init()`之后调用。

## 3. 使用示例

### 3.1 基本绘图示例

以下示例展示了如何初始化LCD并进行基本绘图操作：

```c
#include "lcd_api.h"            // LCD用户接口
#include "system.h"             // 系统初始化
#include "key_api.h"            // 按键接口

void Lcd_Example1(void)
{
    unsigned char number = 0;
    
    // 初始化系统
    Sys_Init();
    
    // 初始化按键
    Key_Init();
    
    // 初始化LCD
    Lcd_Init();

    for (;;)
    {
        if (FLAG_KEY1)
        {
            FLAG_KEY1 = 0;
            // 绘制图形测试
            GrContextForegroundSet(&Lcd_Context, ClrWhite);
            GrCircleDraw(&Lcd_Context, 100, 240, 80);
            GrCircleFill(&Lcd_Context, 300, 240, 80);
        }
        if (FLAG_KEY2)
        {
            FLAG_KEY2 = 0;
            // 字体显示测试
            GrContextFontSet(&Lcd_Context, &g_sFontCm48);
            GrStringDrawCentered(&Lcd_Context, "Hello LCD", -1, 400, 240, 0);
        }
        if (FLAG_KEY3)
        {
            FLAG_KEY3 = 0;
            // 显示数字
            char Buffer[8];
            sprintf(Buffer, "Num: %d", ++number);
            GrStringDrawCentered(&Lcd_Context, Buffer, -1, 400, 240, 0);
        }
        if (FLAG_KEY5)
        {
            FLAG_KEY5 = 0;
            // 清除屏幕
            Lcd_Rectangle.sXMin = 0;
            Lcd_Rectangle.sYMin = 0;
            Lcd_Rectangle.sXMax = 800;
            Lcd_Rectangle.sYMax = 480;
            GrContextForegroundSet(&Lcd_Context, ClrBlack);
            GrRectFill(&Lcd_Context, &Lcd_Rectangle);
        }
    }
}
```

### 3.2 控件使用示例

以下示例展示了如何使用LCD控件：

```c
#include "lcd_api.h"            // LCD用户接口
#include "system.h"             // 系统初始化

void Lcd_Example2(void)
{
    // 初始化系统
    Sys_Init();
    
    // 初始化LCD
    Lcd_Init();
    
    // 初始化控件
    Lcd_Widget_Init();
    
    for (;;)
    {
        // 处理控件消息
        WidgetMessageQueueProcess();
    }
}
```

## 4. 问题解答

### 4.1 常见问题

#### Q1: LCD显示不正常怎么办？

**A1**：请按照以下分层排查顺序进行检查：
- **应用层**：是否调用了`Lcd_Init()`函数初始化LCD？是否正确设置了绘图上下文？
- **设备层**：请参考`lcd_api.h`中的接口定义，确保API调用正确。
- **硬件层**：检查LCD引脚连接是否正确，电源是否正常。

#### Q2: 如何绘制中文？

**A2**：当前版本的LCD驱动使用grlib图形库，默认不支持中文字体。如果需要显示中文，需要添加中文字体库并实现相应的绘制函数。

#### Q3: LCD显示有闪烁怎么办？

**A3**：请检查以下几点：
- 帧缓冲区的配置是否正确？
- LCD控制器的时序参数是否配置正确？
- 图形绘制是否在合适的时机进行？

### 4.2 故障排查

- **应用层**：请基于`lcd_api.h`和`user_lcd.c`中的代码内容进行测试，确保API调用正确。
- **设备层**：如果遇到问题，请阅读驱动部分代码尝试排查或咨询工程师。
- **硬件层**：检查LCD引脚连接和电源供应。

## 5. 总结

LCD API提供了简单易用的接口，用于实现LCD显示功能。通过使用`Lcd_Init()`函数初始化LCD，然后使用grlib图形库提供的函数进行图形绘制，可以实现各种显示效果。

本指南介绍了LCD设备的基本信息、API接口的使用方法和常见问题的解答，希望能够帮助开发者快速上手LCD控制功能，编写出高质量的应用程序。
