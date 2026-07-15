/**
 * edma0_common.h
 *
 * 描述: EDMA0 公共函数头文件
 * 功能: 定义 EDMA3 的公共函数和变量
 */

#ifndef _EDMA0_COMMON_H_
#define _EDMA0_COMMON_H_

/**
 * @brief EDMA 回调函数类型定义
 */
typedef void (*EDMA_Callback)(unsigned int tccNum, unsigned int status, void *appData);

/**
 * @brief EDMA 回调函数数组
 */
extern void (*cb_Fxn[])(unsigned int tccNum, unsigned int status, void *appData);

/**
 * @brief EDMA3 初始化函数
 * @return 无
 */
void Edma0_Common_Init(void);

#endif /* _EDMA0_COMMON_H_ */
