.origin 1
.entrypoint START

#include "PRU1.hp"

//////////////////////////////////////////////////////////////////
//                          主程序							    //
//////////////////////////////////////////////////////////////////
START:

LOOP_FOR:				// 循环开始

//    CALL    DELAY_500MS
    CALL    LOOP_TIMER12
    CALL    AD_CONVST

    QBA		LOOP_FOR									// loop flash for times
    HALT


// ***************************************
// *     定时器等待     *
// ***************************************
LOOP_TIMER12:
    // 添加延时，防止pru频繁读取寄存器，防止dsp无法debug
    DELAY_TIM:
        MOV		r0, 456//1140
    TIM_DELAY:
        SUB		r0, r0, 1				// 1 CPI
        QBNE	TIM_DELAY, r0, 0		// 1 CPI，r0 != 0 则跳转到 TIM_DELAY ；直到 r0 = 0 顺序执行

    // 检查AD使能标志
    CHECK_AD_ENABLE:
        // 读取AD使能标志
        MOV     r_ptr_cfg,0
        MOV     r_ad_en,0
        MOV     r_ptr_cfg, AD_CONFIG_AD_EN_ADDR
        LBBO    r_ad_en, r_ptr_cfg, 0, 1
        QBEQ    LOOP_TIMER12, r_ad_en, 0                             // 如果未使能，继续检查
    
    // 处理定时器
    MOV     r3, SOC_TMR_1_REGS + TMR_INTCTLSTAT + 2  		// 复制 SYSCFG_BASE + PINMUX1_OFFSET数值 到r3
    LBBO    r2, r3, 0, 1                      			// 将 r3+0 的内存地址里的数 复制 4 bytes 到 r2
    AND		r2, r2, TMR_INTCTLSTAT_PRDINTSTAT12
    QBEQ    LOOP_TIMER12, r2, 0                    		// r2 = 0 则跳转到 LOOP_TIMER12 ；直到 r0 != 0 顺序执行
    SBBO    r2, r3, 0, 1                      			// 从r2取 4 bytes 写到 r3+0 的内存地址里    
    RET


AD_CONVST:
    MOV     r5, SOC_GPIO_0_REGS + GPIO_CLR_DATA  		// 复制 SYSCFG_BASE + PINMUX1_OFFSET 数值到r5
    MOV     r4, GPIO5_13
    SBBO    r4, r5, 0, 4                      			// 从r4取 4 bytes 写到 r5+0 的内存地址里

DELAY_MS:
        MOV		r0, 114
MS_DELAY:
        SUB		r0, r0, 1				// 1 CPI
        QBNE	MS_DELAY, r0, 0		// 1 CPI，r0 != 0 则跳转到 LOOP_DELAY ；直到 r0 = 0 顺序执行

    MOV     r5, SOC_GPIO_0_REGS + GPIO_SET_DATA  		// 复制 SYSCFG_BASE + PINMUX1_OFFSET 数值到r5
    MOV     r4, GPIO5_13
    SBBO    r4, r5, 0, 4                      			// 从r4取 4 bytes 写到 r5+0 的内存地址里
    RET

// ***************************************
// *     延时函数     *
// ***************************************
// 延时子程序包含1条减法指令和1条条件跳转指令
// PRU CORE CLK: 228MHz, cycle:8.77ns
// 1s延时:    228000000/2 = 114000000	
// 500ms延时: 114000000/2 =  57000000

DELAY_500MS:
        MOV		r0, 1140   //570  //1140
LOOP_DELAY:
        SUB		r0, r0, 1				// 1 CPI
        QBNE	LOOP_DELAY, r0, 0		// 1 CPI，r0 != 0 则跳转到 LOOP_DELAY ；直到 r0 = 0 顺序执行
        RET	

