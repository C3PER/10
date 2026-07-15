/**
 * @file flash_api.h
 * @brief Flash用户接口文件
 * @details 定义Flash的用户接口和使用案例，为用户提供统一的Flash操作入口。
 * @ingroup FLASH_API
 */

#ifndef _FLASH_API_H_
#define _FLASH_API_H_

/**
 * @defgroup FLASH Flash模块
 * @brief Flash完整功能模块
 * @details 
 * Flash模块是一个完整的功能模块，包含以下子模块：
 * - 用户接口模块：定义Flash的用户接口和使用案例
 * - 设备驱动模块：实现Flash的初始化和操作逻辑
 * - 应用示例模块：提供Flash使用示例，展示如何初始化和使用Flash
 * 
 * 本模块为用户提供统一的Flash控制接口，支持Flash的初始化、擦除、写入和读取操作。
 */


/**
 * @defgroup FLASH_API Flash用户接口
 * @ingroup FLASH
 * @brief Flash用户接口模块
 * @details 
 * 定义Flash的用户接口，为用户提供统一的Flash操作入口。
 * 
 * 主要包含以下内容：
 * - 初始化函数：初始化SPI接口，为Flash操作做准备
 * - 测试函数：执行Flash的擦除、写入、读取和校验操作
 * 
 * 用户通过此模块可以方便地使用Flash功能，实现数据的存储和读取。
 * @{
 */

/**
 * @brief 初始化Flash
 * @details 初始化SPI接口，为Flash操作做准备。
 */
void Flash_Init(void);

/**
 * @brief Flash测试函数
 * @details 执行Flash的擦除、写入、读取和校验操作。
 */
void Flash_Test(void);

/** @} */

#endif /* _FLASH_API_H_ */
