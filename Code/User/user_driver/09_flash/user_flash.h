/**
 * @file user_flash.h
 * @brief Flash用户使用头文件
 * @details 定义Flash用户使用接口，为用户提供Flash使用示例。
 * @ingroup FLASH_EXAMPLE
 */

#ifndef _USER_FLASH_H_
#define _USER_FLASH_H_

/**
 * @defgroup FLASH_EXAMPLE Flash应用示例
 * @ingroup FLASH
 * @brief Flash应用示例模块
 * @details 
 * 定义Flash用户使用接口，为用户提供Flash使用示例。
 * 
 * 主要包含以下内容：
 * - Flash使用示例函数：展示如何初始化和使用Flash
 * 
 * 用户通过此模块可以学习如何在实际应用中使用Flash功能。
 * @{
 */

/**
 * @brief Flash示例函数
 * @details 演示Flash的基本操作流程，包括初始化、擦除、写入、读取和校验。
 */
void Flash_Example(void);

/** @} */

#endif /* _USER_FLASH_H_ */
