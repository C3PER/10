/**
 * edma1_common.h
 *
 * 描述: EDMA1 公共函数头文件
 * 功能: 定义 EDMA3 的公共函数和变量
 */

#ifndef _EDMA1_COMMON_H_
#define _EDMA1_COMMON_H_

/**
 * @brief EDMA 回调函数类型定义
 */
typedef void (*EDMA_Callback)(unsigned int tccNum, unsigned int status, void *appData);

/**
 * @brief EDMA 回调函数数组
 */
extern void (*edma1_cb_Fxn[])(unsigned int tccNum, unsigned int status, void *appData);

/**
 * @brief EDMA3 初始化函数
 * @return 无
 */
void Edma1_Common_Init(void);

#endif /* _EDMA1_COMMON_H_ */
