/**
 * @file sys_iic.h
 * @brief IIC驱动头文件
 * @details 定义IIC驱动的接口和宏定义，实现IIC的初始化和数据传输功能。
 * @ingroup IIC_DRV
 */

#ifndef _SYS_IIC_H_
#define _SYS_IIC_H_

/**
 * @defgroup IIC IIC模块
 * @brief IIC完整功能模块
 * @details 
 * IIC模块是一个完整的功能模块，包含以下子模块：
 * - 设备驱动模块：实现IIC的初始化和数据传输逻辑
 * 
 * 本模块为用户提供统一的IIC控制接口，支持IIC的初始化和数据的读写操作。
 */

/**
 * @defgroup IIC_DRV IIC外设驱动
 * @ingroup IIC
 * @brief IIC外设驱动模块
 * @details 
 * 定义IIC驱动的接口和宏定义，实现IIC的初始化和数据传输功能。
 * 
 * 主要职责包括：
 * - 定义IIC引脚宏
 * - 定义IIC设备类型枚举
 * - 定义设备地址宏
 * - 定义寄存器位数宏
 * - 实现IIC的初始化逻辑
 * - 实现IIC的数据传输功能
 * 
 * 本模块为其他依赖IIC的设备（如EEPROM、触摸屏）提供底层支持。
 * @{
 */

/**
 * @brief 引脚定义
 * @details I2C0 引脚复用配置
 */
// SDA 引脚定义
#define PINMUX4_I2C0_SDA_ENABLE    (SYSCFG_PINMUX4_PINMUX4_15_12_I2C0_SDA << \
                                    SYSCFG_PINMUX4_PINMUX4_15_12_SHIFT)
// SCL 引脚定义
#define PINMUX4_I2C0_SCL_ENABLE    (SYSCFG_PINMUX4_PINMUX4_11_8_I2C0_SCL << \
                                    SYSCFG_PINMUX4_PINMUX4_11_8_SHIFT)


/**
 * @brief IIC设备类型枚举
 * @details 定义支持的IIC设备类型，用于选择不同的从设备地址
 */
typedef enum {
    IIC_DEVICE_TOUCH,   //!< LCD_TOUCH设备
    IIC_DEVICE_EEPROM,  //!< EEPROM设备
} IIC_DeviceType;


/**
 * @brief 设备地址宏定义
 */
#define GT1151_ADDRESS      (0x14)    // IIC GT1151 地址
#define EEPROM_ADDRESS      (0x50)    // IIC EEPROM 地址


/**
 * @brief 寄存器位数宏定义
 */
#define REG8    1    // 8位寄存器
#define REG16   2    // 16位寄存器


/* 函数声明 */
/**
 * @brief IIC初始化函数
 * @details 初始化IIC接口，配置引脚和寄存器，为IIC操作做准备。
 */
void Sys_IIC_Init(void);

/**
 * @brief IIC选择设备函数
 * @details 选择要操作的IIC设备，设置对应的从设备地址。
 * @param device 设备类型
 */
void IIC_SelectDevice(IIC_DeviceType device);

/**
 * @brief IIC发送函数
 * @details 发送指定长度的数据。
 * @param dataCnt 数据长度
 */
void IICSend(unsigned int dataCnt);

/**
 * @brief IIC接收函数
 * @details 接收指定长度的数据。
 * @param dataCnt 数据长度
 */
void IICReceive(unsigned int dataCnt);

/**
 * @brief IIC写入函数
 * @details 写入一个字节的数据。
 * @param dat 要写入的数据
 */
void IIC_Write(unsigned char dat);

/**
 * @brief IIC读取函数
 * @details 读取一个字节的数据。
 * @param recv_dat 接收数据的缓冲区
 */
void IIC_Read(unsigned char *recv_dat);

/**
 * @brief IIC写入寄存器函数
 * @details 向指定寄存器写入一个字节的数据。
 * @param RegBIT 寄存器位数
 * @param reg 寄存器地址
 * @param dat 要写入的数据
 */
void IIC_Write_Reg(unsigned char RegBIT, unsigned int reg, unsigned char dat);

/**
 * @brief IIC读取寄存器函数
 * @details 从指定寄存器读取一个字节的数据。
 * @param RegBIT 寄存器位数
 * @param reg 寄存器地址
 * @param recv_dat 接收数据的缓冲区
 */
void IIC_Read_Reg(unsigned char RegBIT, unsigned int reg, unsigned char *recv_dat);

/**
 * @brief IIC顺序写入寄存器函数
 * @details 向指定寄存器连续写入多个字节的数据。
 * @param RegBIT 寄存器位数
 * @param reg 寄存器地址
 * @param send_dat 要写入的数据
 * @param send_len 数据长度
 */
void IIC_SeqWrite_Reg(unsigned char RegBIT, unsigned int reg, unsigned char *send_dat, unsigned int send_len);

/**
 * @brief IIC顺序读取寄存器函数
 * @details 从指定寄存器连续读取多个字节的数据。
 * @param RegBIT 寄存器位数
 * @param reg 寄存器地址
 * @param recv_dat 接收数据的缓冲区
 * @param recv_len 数据长度
 */
void IIC_SeqRead_Reg(unsigned char RegBIT, unsigned int reg, unsigned char *recv_dat, unsigned int recv_len);

/** @} */



#endif /* _SYS_IIC_H_ */
