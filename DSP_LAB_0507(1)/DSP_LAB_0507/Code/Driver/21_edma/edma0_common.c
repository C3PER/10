/**
 * edma0_common.c
 *
 * 描述: EDMA0 公共函数实现文件
 * 功能: 实现 EDMA3 的初始化、中断处理等公共功能
 */

#include "edma0_common.h"

#include "hw_types.h"               // 宏命令
#include "soc_C6748.h"              // DSP C6748 外设寄存器

#include "psc.h"                    // 电源与睡眠控制宏及设备抽象层函数声明
#include "interrupt.h"              // DSP C6748 中断相关应用程序接口函数声明及系统事件号定义

#include "edma.h"

/**
 * @brief 函数声明
 */
static void PSCInit(void);
static void EDMA3InterruptInit(void);
static void Edma3CCComplHandlerIsr(void);
static void Edma3CCErrHandlerIsr(void);

/**
 * @brief EDMA 回调函数数组
 */
void (*cb_Fxn[])(unsigned int tccNum, unsigned int status, void *appData);

/**
 * @brief EDMA3 初始化函数
 * @return 无
 */
void Edma0_Common_Init(void)
{
    // 初始化 PSC，使能 EDMA3 相关模块
    PSCInit();
    
    // 初始化 EDMA3 中断
    EDMA3InterruptInit();
    
    // 初始化 EDMA3 控制器
    EDMA3Init(SOC_EDMA30CC_0_REGS, 0);
}

/**
 * @brief PSC 初始化函数
 * @return 无
 */
static void PSCInit(void)
{
    // 使能 EDMA3CC_0
    PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_CC0, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // 使能 EDMA3TC_0
    PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_TC0, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);
}

/**
 * @brief EDMA3 中断初始化函数
 * @return 无
 */
static void EDMA3InterruptInit(void)
{
    // 注册 EDMA3 完成中断处理函数
    IntRegister(C674X_MASK_INT5, Edma3CCComplHandlerIsr);
    
    // 注册 EDMA3 错误中断处理函数
    IntRegister(C674X_MASK_INT6, Edma3CCErrHandlerIsr);

    // 映射 EDMA3 完成中断到 CPU 中断 5
    IntEventMap(C674X_MASK_INT5, SYS_INT_EDMA3_0_CC0_INT1);
    
    // 映射 EDMA3 错误中断到 CPU 中断 6
    IntEventMap(C674X_MASK_INT6, SYS_INT_EDMA3_0_CC0_ERRINT);

    // 使能 CPU 中断 5
    IntEnable(C674X_MASK_INT5);
    
    // 使能 CPU 中断 6
    IntEnable(C674X_MASK_INT6);
}

/**
 * @brief EDMA3 中断完成服务函数
 * @return 无
 */
static void Edma3CCComplHandlerIsr(void)
{
    volatile unsigned int pendingIrqs;
    volatile unsigned int isIPR = 0;

    volatile unsigned int indexl;
    volatile unsigned int Cnt = 0;
    indexl = 1u;

    // 清除系统中断标志
    IntEventClear(SYS_INT_EDMA3_0_CC0_INT1);

    // 获取 EDMA3 中断状态
    isIPR = EDMA3GetIntrStatus(SOC_EDMA30CC_0_REGS);
    if(isIPR)
    {
        // 循环处理中断，最多重试 EDMA3CC_COMPL_HANDLER_RETRY_COUNT 次
        while ((Cnt < EDMA3CC_COMPL_HANDLER_RETRY_COUNT)&& (indexl != 0u))
        {
            indexl = 0u;
            pendingIrqs = EDMA3GetIntrStatus(SOC_EDMA30CC_0_REGS);
            while (pendingIrqs)
            {
                if(TRUE == (pendingIrqs & 1u))
                {
                    // 清除 IPR 相应位
                    EDMA3ClrIntr(SOC_EDMA30CC_0_REGS, indexl);
                    
                    // 调用回调函数
                    (*cb_Fxn[indexl])(indexl, EDMA3_XFER_COMPLETE, NULL);
                }
                ++indexl;
                pendingIrqs >>= 1u;
            }
            Cnt++;
        }
    }
}

/**
 * @brief EDMA3 中断错误服务函数
 * @return 无
 */
static void Edma3CCErrHandlerIsr(void)
{
    volatile unsigned int pendingIrqs = 0u;
    unsigned int Cnt = 0u;
    unsigned int index = 1u;
    unsigned int evtqueNum = 0;  // 事件队列数目

    // 清除系统中断标志
    IntEventClear(SYS_INT_EDMA3_0_CC0_ERRINT);

    // 检查是否有错误中断
    if((EDMA3GetErrIntrStatus(SOC_EDMA30CC_0_REGS) != 0 )
        || (EDMA3QdmaGetErrIntrStatus(SOC_EDMA30CC_0_REGS) != 0)
        || (EDMA3GetCCErrStatus(SOC_EDMA30CC_0_REGS) != 0))
    {
        // 循环处理错误中断，最多重试 EDMA3CC_ERR_HANDLER_RETRY_COUNT 次
        while ((Cnt < EDMA3CC_ERR_HANDLER_RETRY_COUNT) && (index != 0u))
        {
            // 处理 DMA 错误中断
            index = 0u;
            pendingIrqs = EDMA3GetErrIntrStatus(SOC_EDMA30CC_0_REGS);
            while (pendingIrqs)
            {
                if(TRUE == (pendingIrqs & 1u))
                {
                    // 清除 SER
                    EDMA3ClrMissEvt(SOC_EDMA30CC_0_REGS, index);
                }
                ++index;
                pendingIrqs >>= 1u;
            }

            // 处理 QDMA 错误中断
            index = 0u;
            pendingIrqs = EDMA3QdmaGetErrIntrStatus(SOC_EDMA30CC_0_REGS);
            while (pendingIrqs)
            {
                if(TRUE == (pendingIrqs & 1u))
                {
                    // 清除 SER
                    EDMA3QdmaClrMissEvt(SOC_EDMA30CC_0_REGS, index);
                }
                ++index;
                pendingIrqs >>= 1u;
            }
            
            // 处理 CC 错误中断
            index = 0u;
            pendingIrqs = EDMA3GetCCErrStatus(SOC_EDMA30CC_0_REGS);
            if(pendingIrqs != 0u)
            {
                // 处理事件队列错误
                for (evtqueNum = 0u; evtqueNum < SOC_EDMA3_NUM_EVQUE; evtqueNum++)
                {
                    if((pendingIrqs & (1u << evtqueNum)) != 0u)
                    {
                        // 清除错误中断
                        EDMA3ClrCCErr(SOC_EDMA30CC_0_REGS, (1u << evtqueNum));
                    }
                }
                
                // 处理传输完成错误
                if ((pendingIrqs & (1 << EDMA3CC_CCERR_TCCERR_SHIFT)) != 0u)
                {
                    // 清除错误中断
                    EDMA3ClrCCErr(SOC_EDMA30CC_0_REGS, (0x01u << EDMA3CC_CCERR_TCCERR_SHIFT));
                }
                ++index;
            }
            Cnt++;
        }
    }
}
