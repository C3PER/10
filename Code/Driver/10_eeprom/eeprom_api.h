/**
 * @file eeprom_api.h
 * @brief EEPROM用户接口文件
 * @details 定义EEPROM的用户接口和使用案例，为用户提供统一的EEPROM操作入口。
 * @ingroup EEPROM_API
 */

#ifndef _EEPROM_API_H_
#define _EEPROM_API_H_

/**
 * @defgroup EEPROM EEPROM模块
 * @brief EEPROM完整功能模块
 * @details 
 * EEPROM模块是一个完整的功能模块，包含以下子模块：
 * - 用户接口模块：定义EEPROM的用户接口和使用案例
 * - 设备驱动模块：实现EEPROM的初始化和操作逻辑
 * - 应用示例模块：提供EEPROM使用示例，展示如何初始化和使用EEPROM
 * 
 * 本模块为用户提供统一的EEPROM控制接口，支持EEPROM的初始化、擦除、写入和读取操作。
 */


/**
 * @defgroup EEPROM_API EEPROM用户接口
 * @ingroup EEPROM
 * @brief EEPROM用户接口模块
 * @details 
 * 定义EEPROM的用户接口，为用户提供统一的EEPROM操作入口。
 * 
 * 主要包含以下内容：
 * - 初始化函数：初始化IIC接口，为EEPROM操作做准备
 * - 测试函数：执行EEPROM的读写操作测试
 * 
 * 用户通过此模块可以方便地使用EEPROM功能，实现数据的存储和读取。
 * @{
 */

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

#endif /* _EEPROM_API_H_ */
