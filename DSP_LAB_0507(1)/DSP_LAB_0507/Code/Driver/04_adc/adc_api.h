/**
 * @file adc_api.h
 * @brief ADC用户接口文件
 * @details 定义ADC的用户接口和宏定义，为用户提供ADC的初始化和控制功能。
 * @ingroup ADC_API
 */

#ifndef _ADC_API_H_
#define _ADC_API_H_

/**
 * @defgroup ADC_API ADC用户接口
 * @ingroup ADC
 * @brief ADC用户接口模块
 * @details 
 * 定义ADC的用户接口和宏定义，为用户提供ADC的初始化和控制功能。
 * 
 * 主要职责包括：
 * - 定义ADC采集频率宏
 * - 定义ADC采样长度宏
 * - 定义ADC乒乓索引宏
 * - 定义ADC标志位
 * - 定义ADC数据缓冲区
 * - 提供ADC的初始化、启动和停止函数
 * 
 * ADC_API模块包含以下子模块：
 * - @ref ADC_DRV ：ADC设备驱动模块，实现ADC的底层驱动逻辑
 * - @ref ADC_PIN ：ADC引脚控制模块，负责ADC相关引脚的初始化和配置
 * - @ref ADC_EMIF ：ADC EMIF模块，负责CPU的EMIF接口配置
 * - @ref ADC_EDMA ：ADC EDMA模块，实现ADC数据的DMA传输
 * - @ref ADC_TIM1 ：ADC定时器模块，实现ADC的定时采样控制
 * - @ref ADC_PRU ：ADC PRU模块，负责CPU的PRU配置
 * 
 * @{
 */

/**
 * @name ADC 采样频率定义
 * @{
 * 这些宏定义了ADC模块支持的采样频率。
 */
#define ADC_10KHZ     10000    //!< 10kHz采样率
#define ADC_20KHZ     20000    //!< 20kHz采样率
#define ADC_48KHZ     48000    //!< 48kHz采样率
#define ADC_50KHZ     50000    //!< 50kHz采样率
/** @} */

/**
 * @name ADC 采样长度定义
 * @{
 * 这些宏定义了ADC模块支持的采样长度（即一次采集的数据点数量）。
 * 
 * 建议使用2的整数次幂，以优化DSP算法（如FFT）的效率。
 */
#define ADC_SAMPLE_128   128    //!< 128个采样点
#define ADC_SAMPLE_256   256    //!< 256个采样点
#define ADC_SAMPLE_512   512    //!< 512个采样点
#define ADC_SAMPLE_1024  1024   //!< 1024个采样点
#define ADC_SAMPLE_8192  8192   //!< 8192个采样点
/** @} */

/**
 * @name ADC 乒乓索引
 * @{
 * 这些宏定义了ADC乒乓缓冲区的索引。
 */
#define AD_BUFFER_PING   0     //!< Ping缓冲区
#define AD_BUFFER_PONG   1     //!< Pong缓冲区
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
#define BUF_SIZE_MAX    0x4000  // 由EDMA外设的最大搬移地址决定，用于EDMA传输参数配置
#define ADC_MAX_LEN     0x2000  // 用于创建最大长度的数组，用户ADC采样长度要小于此

/**
 * @brief 缓冲区地址定义
 */
#define AD_Buffer_Ping   ((short *)0xC7000000) //!< Ping缓冲区地址
#define AD_Buffer_Pong   ((short *)0xC7020000) //!< Pong缓冲区地址
/** @} */
/// @endcond /* INTERNAL_MACROS 结束 */


/**
 * @name ADC 标志位
 * @{
 * 这些是用于控制ADC状态和同步的标志位。
 */
extern volatile far unsigned char AD_En;            //!< ADC使能标志
extern volatile far unsigned char FLAG_AD;          //!< ADC采集完成标志位
extern volatile far unsigned char AD_Ping_Pong;     //!< ADC Ping/Pong缓冲区切换标志位
extern volatile far unsigned short AD_Len;          //!< ADC采样长度
/** @} */

/**
 * @name ADC 数据缓冲区
 * @{
 * 这些是用于存储ADC采集数据的乒乓缓冲区。
 */
extern short AD_CH1_Buf0[];  //!< 通道1 Ping缓冲区
extern short AD_CH2_Buf0[];  //!< 通道2 Ping缓冲区
extern short AD_CH3_Buf0[];  //!< 通道3 Ping缓冲区
extern short AD_CH4_Buf0[];  //!< 通道4 Ping缓冲区
extern short AD_CH5_Buf0[];  //!< 通道5 Ping缓冲区
extern short AD_CH6_Buf0[];  //!< 通道6 Ping缓冲区
extern short AD_CH7_Buf0[];  //!< 通道7 Ping缓冲区
extern short AD_CH8_Buf0[];  //!< 通道8 Ping缓冲区
extern short AD_CH1_Buf1[];  //!< 通道1 Pong缓冲区
extern short AD_CH2_Buf1[];  //!< 通道2 Pong缓冲区
extern short AD_CH3_Buf1[];  //!< 通道3 Pong缓冲区
extern short AD_CH4_Buf1[];  //!< 通道4 Pong缓冲区
extern short AD_CH5_Buf1[];  //!< 通道5 Pong缓冲区
extern short AD_CH6_Buf1[];  //!< 通道6 Pong缓冲区
extern short AD_CH7_Buf1[];  //!< 通道7 Pong缓冲区
extern short AD_CH8_Buf1[];  //!< 通道8 Pong缓冲区
/** @} */

/**
 * @name ADC 函数声明
 * @details 
 * - 初始化函数：初始化ADC模块，配置采样频率和采样长度
 * - 启动函数：启动ADC采集
 * - 停止函数：停止ADC采集
 * @{
 */
/**
 * @brief ADC初始化函数
 * @details 初始化ADC模块，配置采样频率和采样长度。
 * @param clkFreq 采样频率
 * @param sampleLen 采样长度
 */
void Adc_Init(unsigned int clkFreq, unsigned int sampleLen);

/**
 * @brief ADC启动函数
 * @details 启动ADC采集。
 */
void Adc_Start(void);

/**
 * @brief ADC停止函数
 * @details 停止ADC采集。
 */
void Adc_Stop(void);

/** @} */

/** @} */

#endif /* _ADC_API_H_ */

/**
 * @defgroup ADC ADC模块
 * @brief ADC完整功能模块
 * @details 
 * ADC模块为用户提供统一的ADC控制接口，支持ADC的初始化、启动和停止操作。
 * 
 * ADC模块包含以下子模块：
 * - @ref ADC_API ：ADC用户接口模块，包含设备驱动、引脚控制和EDMA传输
 * - @ref ADC_EXAMPLE ：ADC应用示例模块，提供ADC使用示例
 * 
 * @par 快速使用
 * 最基础的ADC采集流程如下。
 * @code{.c}
 * #include "adc_api.h"                 // ADC用户接口
 * #include "system.h"                  // 中断和延时初始化
 * int main(void)
 * {
 *     // 定义一个int16_t类型的数组来存储采集到的数据
 *     short adc_data[ADC_SAMPLE_128];
 *     Sys_Init();                      // 初始化系统
 *     // 初始化ADC，设置为10kHz采样率，128个采样点
 *     Adc_Init(ADC_10KHZ, ADC_SAMPLE_128); 
 *     Adc_Start();                     // 启动ADC采集
 * 
 *     for (;;)
 *     {
 *         // 检测ADC采集完成
 *         if (FLAG_AD)                 // 检查ADC是否采集完成
 *         {
 *             FLAG_AD = 0;             // 清除标志位
 *             // 处理采集数据
 *             // 根据AD_Ping_Pong标志位判断当前使用的缓冲区
 *             if (AD_Ping_Pong == AD_BUFFER_PONG)
 *             {
 *                 // 使用Ping缓冲区数据
 *                 // 例如：处理AD_CH1_Buf0[]中的数据
 *                 memcpy(adc_data, AD_CH1_Buf0, ADC_SAMPLE_128*2);
 *             }
 *             else
 *             {
 *                 // 使用Pong缓冲区数据
 *                 // 例如：处理AD_CH1_Buf1[]中的数据
 *                 memcpy(adc_data, AD_CH1_Buf1, ADC_SAMPLE_128*2);
 *             }
 *         }
 *     }
 * }
 * @endcode
 * 
 * @note 
 * 程序中部分说明：
 * - 包含 @ref adc_api.h 头文件，使用ADC用户接口。
 * - 调用 @ref Adc_Init() 初始化ADC。
 * - 设置为10kHz采样率（ @ref ADC_10KHZ ）。
 * - 设置为128个采样点（ @ref ADC_SAMPLE_128 ）。
 * - 调用 @ref Adc_Start() 启动ADC采集。
 * - 通过检查 @ref FLAG_AD 标志位来响应ADC采集完成事件。
 * - 通过 @ref AD_Ping_Pong 标志位（ @ref AD_BUFFER_PING 或 @ref AD_BUFFER_PONG ）判断当前使用的缓冲区。
 * - 根据缓冲区标志位处理采集数据（ @ref AD_CH1_Buf0 或 @ref AD_CH1_Buf1 ）。
 * 
 * @warning
 * ADC功能较为复杂，需要注意以下内容：
 * - 需要调用 @ref Sys_Init() 初始化系统中断。
 * - 需要注意ADC采样频率不能超过 @ref ADC_50KHZ 。
 * - 需要注意ADC采样长度不能超过 @ref ADC_SAMPLE_8192 。
 * - 需要注意乒乓缓冲区原理，按照示例中的处理逻辑进行。避免处理数据和EDMA传输发生冲突。
 * - 如果不及时处理采集数据，两个周期后新采集的数据会覆盖旧数据。
 * - 使用memcpy()拷贝数据时, 注意数据类型为short, 所以需要采样长度乘2得到字节数量。
 * 
 * @todo
 * 请读者参考 @ref ADC 完成以下步骤，实现ADC采集功能：
 * -# 调用 @ref Adc_Init() 初始化ADC，设置合适的采样频率和采样长度。
 * -# 调用 @ref Adc_Start() 启动ADC采集。
 * -# 通过检查 @ref FLAG_AD 标志位来响应ADC采集完成事件。
 * -# 通过 @ref AD_Ping_Pong 标志位处理采集数据（ @ref AD_CH1_Buf0 或 @ref AD_CH1_Buf1 ）。
 * 
 */
