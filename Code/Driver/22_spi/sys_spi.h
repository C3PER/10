/**
 * @file sys_spi.h
 * @brief SPI驱动头文件
 * @details 定义SPI驱动的接口和宏定义，实现SPI的初始化和数据传输功能。
 * @ingroup SPI_DRV
 */

#ifndef _SYS_SPI_H_
#define _SYS_SPI_H_

/**
 * @defgroup SPI SPI模块
 * @brief SPI完整功能模块
 * @details 
 * SPI模块是一个完整的功能模块，包含以下子模块：
 * - 设备驱动模块：实现SPI的初始化和数据传输逻辑
 * 
 * 本模块为用户提供统一的SPI控制接口，支持SPI的初始化和数据的连续读写操作。
 */

/**
 * @defgroup SPI_DRV SPI外设驱动
 * @ingroup SPI
 * @brief SPI外设驱动模块
 * @details 
 * 定义SPI驱动的接口和宏定义，实现SPI的初始化和数据传输功能。
 * 
 * 主要职责包括：
 * - 定义SPI引脚宏
 * - 定义SPI设备类型枚举
 * - 实现SPI的初始化逻辑
 * - 实现SPI的数据传输功能
 * 
 * 本模块为其他依赖SPI的设备（如Flash、DAC）提供底层支持。
 * @{
 */


/// @cond INTERNAL
/* 引脚定义 */
// SPI1 引脚定义
// CLK 引脚定义
#define PINMUX5_SPI1_CLK_ENABLE   (SYSCFG_PINMUX5_PINMUX5_11_8_SPI1_CLK << \
                                   SYSCFG_PINMUX5_PINMUX5_11_8_SHIFT)
// SIMO 引脚定义
#define PINMUX5_SPI1_SIMO_ENABLE  (SYSCFG_PINMUX5_PINMUX5_23_20_SPI1_SIMO0 << \
                                   SYSCFG_PINMUX5_PINMUX5_23_20_SHIFT)
// SOMI 引脚定义
#define PINMUX5_SPI1_SOMI_ENABLE  (SYSCFG_PINMUX5_PINMUX5_19_16_SPI1_SOMI0 << \
                                   SYSCFG_PINMUX5_PINMUX5_19_16_SHIFT)
// ENA 引脚定义
#define PINMUX5_SPI1_ENA_ENABLE   (SYSCFG_PINMUX5_PINMUX5_15_12_NSPI1_ENA << \
                                   SYSCFG_PINMUX5_PINMUX5_15_12_SHIFT)
/// @endcond

/**
 * @brief SPI设备类型
 * @details 定义SPI总线上的设备类型，用于选择不同的片选信号
 */
typedef enum {
    SPI_DEVICE_DAC,      //!< DAC设备，使用CS0
    SPI_DEVICE_FLASH,    //!< FLASH设备，使用CS1
} SPI_DeviceType;


/* 函数声明 */
/**
 * @brief SPI初始化函数
 * @details 初始化SPI接口，配置引脚和寄存器，为SPI操作做准备。
 */
void Sys_SPI_Init(void);

/**
 * @brief 向指定SPI设备连续写入多个字节数据
 * @details 向指定的SPI设备连续写入多个字节的数据，使用中断方式实现。
 * @param device 设备类型
 * @param send_dat 要写入的数据
 * @param send_len 数据长度
 */
void SPI_SeqWrite_Reg(SPI_DeviceType device, unsigned char *send_dat, unsigned int send_len);

/**
 * @brief 从指定SPI设备连续读取多个字节数据
 * @details 从指定的SPI设备连续读取多个字节的数据，使用中断方式实现。
 * @param device 设备类型
 * @param send_dat 发送的数据（用于触发读取）
 * @param send_len 发送数据长度
 * @param recv_dat 接收的数据
 * @param recv_len 接收数据长度
 */
void SPI_SeqRead_Reg(SPI_DeviceType device, unsigned char *send_dat, unsigned int send_len, unsigned char *recv_dat, unsigned int recv_len);

/** @} */

#endif /* _SYS_SPI_H_ */
