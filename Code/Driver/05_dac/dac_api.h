/**
 * @file dac_api.h
 * @brief DAC用户接口文件
 * @details 定义DAC的用户接口和宏定义，为用户提供DAC的初始化和控制功能。
 * @ingroup DAC_API
 */

#ifndef _DAC_API_H_
#define _DAC_API_H_

/**
 * @defgroup DAC_API DAC用户接口
 * @ingroup DAC
 * @brief DAC用户接口模块
 * @details 
 * 定义DAC的用户接口和宏定义，为用户提供DAC的初始化和控制功能。
 * 
 * 主要职责包括：
 * - 定义DAC输出频率宏
 * - 定义DAC输出长度宏
 * - 定义DAC通道宏
 * - 定义DAC乒乓索引宏
 * - 定义DAC标志位
 * - 定义DAC数据缓冲区
 * - 提供DAC的初始化、启动和停止函数
 * 
 * DAC_API模块包含以下子模块：
 * - @ref DAC_DRV ：DAC设备驱动模块，实现DAC的底层驱动逻辑
 * - @ref DAC_PIN ：DAC引脚控制模块，负责DAC引脚的初始化和配置
 * - @ref DAC_TIM1 ：DAC定时器模块，实现DAC的定时输出控制
 * - @ref DAC_PRU ：DAC PRU模块，实现DAC的并行输出控制
 * 
 * 本模块为用户提供简单易用的API，屏蔽底层实现细节。
 * @{
 */

/**
 * @name DAC 输出频率宏定义
 * @{
 * 这些宏定义了DAC模块支持的输出频率。
 */
#define DAC_10KHZ     10000    //!< 10kHz输出率
#define DAC_20KHZ     20000    //!< 20kHz输出率
#define DAC_25KHZ     25000    //!< 25kHz输出率
#define DAC_50KHZ     50000    //!< 50kHz输出率
/** @} */

/**
 * @name DAC 输出长度宏定义
 * @{
 * 这些宏定义了DAC模块支持的输出长度。
 */
#define DAC_SAMPLE_128   128    //!< 128个输出点
#define DAC_SAMPLE_256   256    //!< 256个输出点
#define DAC_SAMPLE_512   512    //!< 512个输出点
#define DAC_SAMPLE_1024  1024   //!< 1024个输出点
#define DAC_SAMPLE_8192  8192   //!< 8192个输出点
/** @} */

/**
 * @name DAC 通道宏定义
 * @{
 * 这些宏定义了DAC的通道选择。
 */
#define DAC_CHANNEL_1    0x01    //!< 通道1
#define DAC_CHANNEL_2    0x02    //!< 通道2
#define DAC_CHANNEL_3    0x04    //!< 通道3
#define DAC_CHANNEL_4    0x08    //!< 通道4
#define DAC_CHANNEL_5    0x10    //!< 通道5
#define DAC_CHANNEL_6    0x20    //!< 通道6
#define DAC_CHANNEL_7    0x40    //!< 通道7
#define DAC_CHANNEL_8    0x80    //!< 通道8

#define DAC_CHANNEL_12    0x03    //!< 通道1-2
#define DAC_CHANNEL_34    0x0C    //!< 通道3-4
#define DAC_CHANNEL_56    0x30    //!< 通道5-6
#define DAC_CHANNEL_78    0x60    //!< 通道7-8
#define DAC_CHANNEL_1234  0x0F    //!< 通道1-4
#define DAC_CHANNEL_5678  0xF0    //!< 通道5-8
#define DAC_CHANNEL_ALL   0xFF    //!< 所有通道
/** @} */

/**
 * @name DAC 缓冲区类型定义
 * @{
 * 这些宏定义了DAC缓冲区的类型。
 */
#define DA_BUFFER_PING   0 //!< Ping缓冲区
#define DA_BUFFER_PONG   1 //!< Pong缓冲区
/** @} */

/// @cond INTERNAL_MACROS
/**
 * @name DAC 内部宏定义
 * @{
 * 这些是内部使用的宏，用于配置缓冲区大小和地址。
 */
/**
 * @brief 缓冲区大小定义
 */
#define DAC_MAX_LEN     0x2000  // 用于创建最大长度的数组，用户DAC输出长度要小于此

/**
 * @brief 缓冲区地址定义
 */
#define DA_Buffer_Ping   ((short *)0xC7040000) //!< Ping缓冲区地址
#define DA_Buffer_Pong   ((short *)0xC7060000) //!< Pong缓冲区地址
/** @} */
/// @endcond /* INTERNAL_MACROS 结束 */

/**
 * @name DAC 标志位
 * @{
 * 这些是用于控制DAC状态和同步的标志位。
 */
extern volatile far unsigned char DA_En;               //!< DAC使能标志
extern volatile far unsigned char FLAG_DA;          //!< DAC输出完成标志位
extern volatile far unsigned char DA_Ping_Pong;     //!< DAC Ping/Pong缓冲区切换标志位
extern volatile far unsigned short DA_Len;          //!< DAC输出长度
extern volatile far unsigned char DA_Sel;           //!< DAC通道选择
/** @} */

/**
 * @name DAC 缓冲区
 * @{
 * 这些是用于存储DAC输出数据的乒乓缓冲区。
 */
extern short DA_CH1_Buf0[];  //!< 通道1 Ping缓冲区
extern short DA_CH2_Buf0[];  //!< 通道2 Ping缓冲区
extern short DA_CH3_Buf0[];  //!< 通道3 Ping缓冲区
extern short DA_CH4_Buf0[];  //!< 通道4 Ping缓冲区
extern short DA_CH5_Buf0[];  //!< 通道5 Ping缓冲区
extern short DA_CH6_Buf0[];  //!< 通道6 Ping缓冲区
extern short DA_CH7_Buf0[];  //!< 通道7 Ping缓冲区
extern short DA_CH8_Buf0[];  //!< 通道8 Ping缓冲区
extern short DA_CH1_Buf1[];  //!< 通道1 Pong缓冲区
extern short DA_CH2_Buf1[];  //!< 通道2 Pong缓冲区
extern short DA_CH3_Buf1[];  //!< 通道3 Pong缓冲区
extern short DA_CH4_Buf1[];  //!< 通道4 Pong缓冲区
extern short DA_CH5_Buf1[];  //!< 通道5 Pong缓冲区
extern short DA_CH6_Buf1[];  //!< 通道6 Pong缓冲区
extern short DA_CH7_Buf1[];  //!< 通道7 Pong缓冲区
extern short DA_CH8_Buf1[];  //!< 通道8 Pong缓冲区
/** @} */

/**
 * @name DAC 函数声明
 * @{
 */
/**
 * @brief DAC初始化函数
 * @details 初始化DAC模块，配置输出频率、输出长度和通道选择。
 * @param clkFreq 输出频率
 * @param sampleLen 输出长度
 * @param dacSel DAC通道选择
 */
void Dac_Init(unsigned int clkFreq, unsigned int sampleLen, unsigned char dacSel);

/**
 * @brief DAC启动函数
 * @details 启动DAC输出。
 */
void Dac_Start(void);

/**
 * @brief DAC停止函数
 * @details 停止DAC输出。
 */
void Dac_Stop(void);

/** @} */

/** @} */

#endif /* _DAC_API_H_ */

/**
 * @defgroup DAC DAC模块
 * @brief DAC完整功能模块
 * @details 
 * DAC模块为用户提供统一的DAC控制接口，支持DAC的初始化、启动和停止操作。
 * 
 * DAC模块包含以下子模块：
 * - @ref DAC_API ：DAC用户接口模块，包含设备驱动、引脚控制和定时器控制
 * - @ref DAC_EXAMPLE ：DAC应用示例模块，提供DAC使用示例
 * 
 * @par 快速使用
 * 最基础的DAC输出流程如下。
 * @code{.c}
 * #include "dac_api.h"    // DAC用户接口
 * #include "system.h"     // 中断和延时初始化
 * int main(void)
 * {
 *     // 定义一个int16_t类型的数组来存储采集到的数据
 *     short dac_data[DAC_SAMPLE_128];
 *     // 准备输出数据（示例：生成正弦波）
 *     for (int i = 0; i < DAC_SAMPLE_128; i++)
 *     {
 *         // 生成正弦波数据，范围0-4095
 *         short value = (short)(2048 + 2047 * sin(2 * 3.14159 * i / DAC_SAMPLE_128));
 *         DA_CH1_Buf0[i] = value;
 *         DA_CH2_Buf0[i] = value;
 *         dac_data[i] = value;
 *         // 其他通道同理...
 *     }
 * 
 *     Sys_Init();         // 初始化系统
 *     // 初始化DAC，设置为10kHz输出率，128个输出点，选择所有通道
 *     Dac_Init(DAC_10KHZ, DAC_SAMPLE_128, DAC_CHANNEL_ALL);
 *     Dac_Start();        // 启动DAC输出
 *     
 *     for (;;)
 *     {
 *         // 检测DAC输出完成
 *         if (FLAG_DA)     // 检查DAC是否输出完成
 *         {
 *             FLAG_DA = 0; // 清除标志位
 *             // 处理输出完成事件
 *             // 根据DA_Ping_Pong标志位判断当前使用的缓冲区
 *             if (DA_Ping_Pong == DA_BUFFER_PONG)
 *             {
 *                 // 使用Ping缓冲区数据
 *                 // 可以在这里更新Ping缓冲区的内容
 *                 memcpy(DA_CH1_Buf0, dac_data, DAC_SAMPLE_128*2);
 *             }
 *             else
 *             {
 *                 // 使用Pong缓冲区数据
 *                 // 可以在这里更新Pong缓冲区的内容
 *                 memcpy(DA_CH1_Buf1, dac_data, DAC_SAMPLE_128*2);
 *             }
 *         }
 *     }
 * }
 * @endcode
 * 
 * @note 
 * 程序中部分说明：
 * - 包含 @ref dac_api.h 头文件，使用DAC用户接口。
 * - 调用 @ref Dac_Init() 初始化DAC。
 * - 设置为10kHz输出率（ @ref DAC_10KHZ ）。
 * - 设置为128个输出点（ @ref DAC_SAMPLE_128 ）。
 * - 设置为所有通道输出（ @ref DAC_CHANNEL_ALL ）。
 * - 调用 @ref Dac_Start() 启动DAC输出。
 * - 通过检查 @ref FLAG_DA 标志位来响应DAC输出完成事件。
 * - 通过 @ref DA_Ping_Pong（ @ref DA_BUFFER_PING 或 @ref DA_BUFFER_PONG ）更新输出数据。
 * - 根据缓冲区标志位更新输出数据（ @ref DA_CH1_Buf0 或 @ref DA_CH1_Buf1 ）。
 * 
 * @warning
 * DAC功能较为复杂，需要注意以下内容：
 * - 需要调用 @ref Sys_Init() 初始化系统中断。
 * - 需要注意DAC输出频率不能超过 @ref DAC_50KHZ 。
 * - 需要注意DAC输出长度不能超过 @ref DAC_SAMPLE_8192 。
 * - 需要注意乒乓缓冲区原理，按照示例中的处理逻辑进行。避免CPU存储和PRU输出发生冲突。
 * - 如果不及时更新输出数据，两个周期后新输出的电压值会重复旧输出。
 * - 使用memcpy()拷贝数据时, 注意数据类型为short, 所以需要输出长度乘2得到字节数量。
 * 
 * @todo
 * 请读者参考 @ref DAC 完成以下步骤，实现DAC输出功能：
 * -# 准备输出数据，如正弦波、方波等。
 * -# 调用 @ref Dac_Init() 初始化DAC，设置合适的输出频率、输出长度和通道选择。
 * -# 调用 @ref Dac_Start() 启动DAC输出。
 * -# 通过检查 @ref FLAG_DA 标志位来响应DAC输出完成事件。
 * -# 根据 @ref DA_Ping_Pong 标志位更新输出数据（ @ref DA_CH1_Buf0 或 @ref DA_CH1_Buf1 ）。
 * 
 */
