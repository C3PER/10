/**
 * @file dac_pru.c
 * @brief DAC PRU实现文件
 * @details 实现DAC PRU的初始化和控制。
 * @ingroup DAC_PRU
 */

#include "dac_pru.h"
#include "pru.h"
#include "PRU0_bin.h"

/* 函数定义 */
// 协处理器0初始化-DA
void Dac_Pru0_Init(void)
{
    /* 关闭PRU0 */
    PruDisable(0);
    /* 启用PRU0 */
    PruEnable (0);
    /* 加载PRU0代码 */
    PruLoad(0, (unsigned int*)PRU0_Code, (sizeof(PRU0_Code)/sizeof(unsigned int)));
    /* 运行PRU0 */
    PruRun(0);
}
