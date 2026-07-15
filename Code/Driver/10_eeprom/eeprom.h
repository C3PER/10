/**
 * @file eeprom.h
 * @brief EEPROM驱动头文件
 * @details 定义EEPROM驱动的接口和宏定义，是连接用户API和底层硬件的桥梁。
 * @ingroup EEPROM_DRV
 */

#ifndef _EEPROM_H_
#define _EEPROM_H_

/**
 * @defgroup EEPROM_DRV EEPROM设备驱动
 * @ingroup EEPROM_API
 * @brief EEPROM设备驱动模块
 * @details 
 * 定义EEPROM驱动的接口和宏定义，是连接用户API和底层硬件的桥梁。
 * 
 * 主要职责包括：
 * - 定义EEPROM操作宏
 * - 实现EEPROM的初始化逻辑
 * - 实现EEPROM的读写操作
 * 
 * 本模块作为中间层，既接收用户API的调用，又直接操作底层IIC接口。
 * @{
 */

/**
 * @brief EEPROM操作宏定义
 */
// EEPROM 读/写当前地址值
#define EEPROMCurrentAddressWrite(Data)            IIC_Write(Data)
#define EEPROMCurrentAddressRead(Data)             IIC_Read(Data)

// EEPROM 从指定地址读/写一个字节
#define EEPROMByteWrite(REG_ADDR, Data)            IIC_Write_Reg(REG8, REG_ADDR, Data)
#define EEPROMRandomRead(REG_ADDR, Data)           IIC_Read_Reg(REG8, REG_ADDR, Data)

// EEPROM 从指定起始地址读/写多个字节
#define EEPROMPageWrite(REG_ADDR, Data, len)       IIC_SeqWrite_Reg(REG8, REG_ADDR, Data, len)
#define EEPROMSequentialRead(REG_ADDR, Data, len)  IIC_SeqRead_Reg(REG8, REG_ADDR, Data, len)

/**
 * @brief 初始化EEPROM
 * @details 初始化IIC接口，为EEPROM操作做准备。
 */
void EEPROM_Init(void);

/**
 * @brief EEPROM测试函数
 * @details 执行EEPROM的读写操作测试。
 */
void EEPROM_Test(void);

/** @} */

#endif /* _EEPROM_H_ */
