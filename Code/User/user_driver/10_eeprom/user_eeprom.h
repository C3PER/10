/**
 * @file user_eeprom.h
 * @brief EEPROM用户使用头文件
 * @details 定义EEPROM用户使用接口，为用户提供EEPROM使用示例。
 * @ingroup EEPROM_EXAMPLE
 */

#ifndef _USER_EEPROM_H_
#define _USER_EEPROM_H_

/**
 * @defgroup EEPROM_EXAMPLE EEPROM应用示例
 * @ingroup EEPROM
 * @brief EEPROM应用示例模块
 * @details 
 * 定义EEPROM用户使用接口，为用户提供EEPROM使用示例。
 * 
 * 主要包含以下内容：
 * - EEPROM使用示例函数：展示如何初始化和使用EEPROM
 * 
 * 用户通过此模块可以学习如何在实际应用中使用EEPROM功能。
 * @{
 */

/**
 * @brief EEPROM示例函数
 * @details 演示EEPROM的基本操作流程，包括初始化、单字节读写、当前地址读取和页读写。
 */
void EEPROM_Example(void);

/** @} */

#endif /* _USER_EEPROM_H_ */
