
.origin 0
.entrypoint MAIN_INIT 

#include "PRU0.hp"             // 包含 PRU 硬件相关的头文件定义

// ==========================================
// 1. 系统入口
// ==========================================
MAIN_INIT:
    // 配置 GPIO, SPI, 定时器
    // 初始化 全局指针, 计数器, 乒乓标志
    CALL    DATA_Buffer_Init
    JMP     MAIN_LOOP

// ==========================================
// 2. 主循环
// ==========================================
MAIN_LOOP:

    // --- [Step 01] 等待同步 ---
    // 阻塞等待 1ms 定时器中断，保证严格的时间基准
    CALL    LOOP_Timer_Wait

    // --- [Step 02] 硬件更新 DA ---
    // 触发 LDAC，将上一帧已写入 DAC 寄存器的数据同步更新到模拟输出端
    CALL    DAC_Update_All

    // ==========================================
    // 3. 小循环
    // ==========================================
    // --- [Step 03] 发送 8 通道数据 ---
    // 遍历 8 个通道，依次计算地址、打包数据、通过 SPI 发送
    DAC_Transmit_8Ch:
        TX_LOOP_BODY:

            // [Sub-Step 3.0] 通道选择
            CALL    Channel_Select
            QBEQ    TX_LOOP_EXIT, r_state, #9

            // [Sub-Step 3.1] 通道数据加载
            // 根据当前基地址、帧索引、通道号计算内存地址
            // 读取 DA 数据值，并准备好 DAC 命令字
            CALL    DATA_Load_Channel

            // [Sub-Step 3.2] 数据打包
            // 将 DAC 命令、通道地址、16位数据值组合成 24/32 位 SPI 数据包
            CALL    DATA_Pack_Data

            // [Sub-Step 3.3] SPI 输出
            // 驱动 SPI 时序，将数据包发送到 DAC8568
            CALL    DATA_SPI_Out

        TX_LOOP_CHECK:
            // [Sub-Step 3.4] 通道循环控制
            ADD     r_state, r_state, #1
            QBEQ    TX_LOOP_EXIT, r_state, #9
            //QBGE    TX_LOOP_EXIT, r_state, #9
            JMP     TX_LOOP_BODY

        TX_LOOP_EXIT:
            MOV     r_state, #1

    // --- [Step 04] 缓冲区管理 ---
    // 更新发送计数器 (0-999)，并在满 1000 次时切换乒乓缓冲区指针
    CALL    LOOP_Buffer_Update

    // --- [Loop End] ---
    JMP     MAIN_LOOP



///* ==========================================================================
// * Function: DATA_Buffer_Init
// * Description: 寄存器全部初始化
// * Params: None
// * Return: None
// * ==========================================================================
// */
DATA_Buffer_Init:
    // 延时
        MOV     r_delay,        #0
    // 定时器
        MOV     r_tim_addr,     SOC_TMR_1_REGS + TMR_INTCTLSTAT
        MOV     r_tim_data,     #0  // TMR_INTCTLSTAT_PRDINTSTAT12
    // GPIO
        MOV     r_gpio_addr,    #0  // GPIO_CLR_ADDR  // GPIO_SET_ADDR
        MOV     r_gpio_data,    GPIO_DATA
    // SPI
        MOV     r_spi_cfg,      #0  // SPIDAT_H12
        MOV     r_spi_tx_buf,   #0  // r_spi_cfg + r_pkt
        MOV     r_spi_tx_mask,  #0  // 掩码
        MOV     r_spi_reg_ptr,  SPIDAT_REG_ADDR
    // 数据指针
        MOV     r_ptr_data1,    DA_DATA1_ADDR_PING
        MOV     r_ptr_data2,    DA_DATA2_ADDR_PING
        MOV     r_ptr_data3,    DA_DATA3_ADDR_PING
        MOV     r_ptr_data4,    DA_DATA4_ADDR_PING
        MOV     r_ptr_data5,    DA_DATA5_ADDR_PING
        MOV     r_ptr_data6,    DA_DATA6_ADDR_PING
        MOV     r_ptr_data7,    DA_DATA7_ADDR_PING
        MOV     r_ptr_data8,    DA_DATA8_ADDR_PING
    // 数据打包
        MOV     r_ptr_src,      #0  // r_ptr_data12345678
        MOV     r_val_raw,      #0  // r_ptr_src的数据
        MOV     r_ch_id,        #0  // CH1_ID_VAL
        MOV     r_pkt,          #0  // r_ch_id + r_val_raw
    // 逻辑控制
        MOV     r_state,        #1  // DA通道选择1-8
        MOV     r_send_count,   #0  // 数组计数0-r_data_len
        MOV     r_ch_sel,       #0
        MOV     r_ptr_cfg, DA_CONFIG_SEL_ADDR
        LBBO    r_ch_sel, r_ptr_cfg, 0, 1
        MOV     r_da_en,        #1  // 默认使能
        MOV     r_ptr_cfg, DA_CONFIG_DA_EN_ADDR
        LBBO    r_da_en, r_ptr_cfg, 0, 1
        MOV     r_data_len,     #0
        MOV     r_ptr_cfg, DA_CONFIG_LEN_ADDR
        LBBO    r_data_len, r_ptr_cfg, 0, 2
        MOV     r_ping_pong,    #0  // 乒乓标志
        MOV     r_ptr_cfg, DA_CONFIG_PP_ADDR
        SBBO    r_ping_pong, r_ptr_cfg, 0, 1
        MOV     r_flag_done, 0
        MOV     r_ptr_cfg, DA_CONFIG_FLAG_ADDR
        SBBO    r_flag_done, r_ptr_cfg, 0, 1

        MOV     r_ch_temp,     #0
    RET

///* ==========================================================================
// * Function: LOOP_Timer_Wait
// * Description: 阻塞等待定时器中断标志。用于将主循环锁定在 1ms 周期上
// * Params: None (依赖全局定时器寄存器)
// * Return: None
// * ==========================================================================
// */
LOOP_Timer_Wait:
    // 添加延时，防止pru频繁读取寄存器，防止dsp无法debug
    DELAY_TIM:
        MOV     r_delay, 456//1140
    TIM_DELAY:
        SUB     r_delay, r_delay, 1				// 1 CPI
        QBNE    TIM_DELAY, r_delay, 0		// 1 CPI，r0 != 0 则跳转到 TIM_DELAY ；直到 r0 = 0 顺序执行

    // 检查DA使能标志
    CHECK_DA_ENABLE:
        // 读取DA使能标志
        MOV     r_ptr_cfg, DA_CONFIG_DA_EN_ADDR
        LBBO    r_da_en, r_ptr_cfg, 0, 1
        QBEQ    LOOP_Timer_Wait, r_da_en, 0                             // 如果未使能，继续检查
    
    // 处理定时器
        MOV     r_tim_addr, SOC_TMR_1_REGS + TMR_INTCTLSTAT             // 复制 SYSCFG_BASE + PINMUX1_OFFSET数值 到r3
        LBBO    r_tim_data, r_tim_addr, 0, 1                            // 将 r2+0 的内存地址里的数 复制 4 bytes 到 r1
        AND     r_tim_data, r_tim_data, TMR_INTCTLSTAT_PRDINTSTAT12     // r1 = r1 & r4
        QBEQ    LOOP_Timer_Wait, r_tim_data, 0                          // r1 = 0 则跳转到 LOOP_TIMER11 ；直到 r0 != 0 顺序执行
        SBBO    r_tim_data, r_tim_addr, 0, 1                            // 从r2取 4 bytes 写到 r2+0 的内存地址里
    RET

///* ==========================================================================
// * Function: DAC_Update_All
// * Description: 触发 DAC8568 硬件更新。拉低 /LDAC 引脚，使上一帧写入寄存器的数据生效
// * Params: None
// * Return: None
// * ==========================================================================
// */
DAC_Update_All:
    GPIO_DA_0:
        MOV     r_gpio_addr, GPIO_CLR_ADDR
        MOV     r_gpio_data, GPIO_DATA
        SBBO    r_gpio_data, r_gpio_addr, 0, 4

//    DELAY_MS:
//        MOV     r_delay, 114
//    MS_DELAY:
//        SUB     r_delay, r_delay, 1
//        QBNE    MS_DELAY, r_delay, 0
    USER_DELAY  114     // 手册最小80ns，实际测试用1us比较稳定

    GPIO_DA_1:
        MOV     r_gpio_addr, GPIO_SET_ADDR
        MOV     r_gpio_data, GPIO_DATA
        SBBO    r_gpio_data, r_gpio_addr, 0, 4

    USER_DELAY  57 //114

    RET

///* ==========================================================================
// * Function: CHANNEL_Select
// * Description: 通道选择函数
// * Detail: 检查当前通道是否启用，如果未启用则跳过
// * Params: r_state (通道号)
// * Return: r_state (更新后的通道号)
// * ==========================================================================
// */
Channel_Select:
    // 通道筛选循环
    CHANNEL_SELECT_LOOP:
        // 检查通道号是否超过8
        QBEQ    CHANNEL_SELECT_EXIT, r_state, #9
        
        // 根据通道号检查对应位
        QBEQ    CHECK_CH_1, r_state, #1
        QBEQ    CHECK_CH_2, r_state, #2
        QBEQ    CHECK_CH_3, r_state, #3
        QBEQ    CHECK_CH_4, r_state, #4
        QBEQ    CHECK_CH_5, r_state, #5
        QBEQ    CHECK_CH_6, r_state, #6
        QBEQ    CHECK_CH_7, r_state, #7
        QBEQ    CHECK_CH_8, r_state, #8
        
        // 检查通道 1
        CHECK_CH_1:
            AND     r_ch_temp, r_ch_sel, #0x01
            QBNE    CHANNEL_SELECT_EXIT, r_ch_temp, #0
            ADD     r_state, r_state, #1
            JMP     CHANNEL_SELECT_LOOP
        
        // 检查通道 2
        CHECK_CH_2:
            AND     r_ch_temp, r_ch_sel, #0x02
            QBNE    CHANNEL_SELECT_EXIT, r_ch_temp, #0
            ADD     r_state, r_state, #1
            JMP     CHANNEL_SELECT_LOOP
        
        // 检查通道 3
        CHECK_CH_3:
            AND     r_ch_temp, r_ch_sel, #0x04
            QBNE    CHANNEL_SELECT_EXIT, r_ch_temp, #0
            ADD     r_state, r_state, #1
            JMP     CHANNEL_SELECT_LOOP
        
        // 检查通道 4
        CHECK_CH_4:
            AND     r_ch_temp, r_ch_sel, #0x08
            QBNE    CHANNEL_SELECT_EXIT, r_ch_temp, #0
            ADD     r_state, r_state, #1
            JMP     CHANNEL_SELECT_LOOP
        
        // 检查通道 5
        CHECK_CH_5:
            AND     r_ch_temp, r_ch_sel, #0x10
            QBNE    CHANNEL_SELECT_EXIT, r_ch_temp, #0
            ADD     r_state, r_state, #1
            JMP     CHANNEL_SELECT_LOOP
        
        // 检查通道 6
        CHECK_CH_6:
            AND     r_ch_temp, r_ch_sel, #0x20
            QBNE    CHANNEL_SELECT_EXIT, r_ch_temp, #0
            ADD     r_state, r_state, #1
            JMP     CHANNEL_SELECT_LOOP
        
        // 检查通道 7
        CHECK_CH_7:
            AND     r_ch_temp, r_ch_sel, #0x40
            QBNE    CHANNEL_SELECT_EXIT, r_ch_temp, #0
            ADD     r_state, r_state, #1
            JMP     CHANNEL_SELECT_LOOP
        
        // 检查通道 8
        CHECK_CH_8:
            AND     r_ch_temp, r_ch_sel, #0x80
            QBNE    CHANNEL_SELECT_EXIT, r_ch_temp, #0
            ADD     r_state, r_state, #1
            JMP     CHANNEL_SELECT_LOOP
    
    CHANNEL_SELECT_EXIT:
    RET

///* ==========================================================================
// * Function: DATA_Load_Channel
// * Description: 加载指定通道的数据
// * Detail: 1. 计算内存地址 (Base + FrameOffset + ChnOffset)
// *         2. 读取 DA 数值
// *         3. 准备 DAC 命令字
// * Params: r_state (通道号), r_curr_base (基地址), r_send_count (帧索引)
// * Return: r_tx_data (准备好的数值与命令信息)
// * ==========================================================================
// */
DATA_Load_Channel:
    // 等于跳转
        QBEQ    DA_CH1, r_state, 1
        QBEQ    DA_CH2, r_state, 2
        QBEQ    DA_CH3, r_state, 3
        QBEQ    DA_CH4, r_state, 4
        QBEQ    DA_CH5, r_state, 5
        QBEQ    DA_CH6, r_state, 6
        QBEQ    DA_CH7, r_state, 7
        QBEQ    DA_CH8, r_state, 8

    DA_CH1:
        MOV     r_ptr_src, r_ptr_data1
        MOV     r_ch_id, CH1_ID_VAL
        RET
    DA_CH2:
        MOV     r_ptr_src, r_ptr_data2
        MOV     r_ch_id, CH2_ID_VAL
        RET
    DA_CH3:
        MOV     r_ptr_src, r_ptr_data3
        MOV     r_ch_id, CH3_ID_VAL
        RET
    DA_CH4:
        MOV     r_ptr_src, r_ptr_data4
        MOV     r_ch_id, CH4_ID_VAL
        RET
    DA_CH5:
        MOV     r_ptr_src, r_ptr_data5
        MOV     r_ch_id, CH5_ID_VAL
        RET
    DA_CH6:
        MOV     r_ptr_src, r_ptr_data6
        MOV     r_ch_id, CH6_ID_VAL
        RET
    DA_CH7:
        MOV     r_ptr_src, r_ptr_data7
        MOV     r_ch_id, CH7_ID_VAL
        RET
    DA_CH8:
        MOV     r_ptr_src, r_ptr_data8
        MOV     r_ch_id, CH8_ID_VAL
        RET

///* ==========================================================================
// * Function: DATA_Pack_Data
// * Description: 数据打包。将数值和命令组装成 DAC8568 要求的 SPI 协议帧格式
// * Params: r_tx_data (原始数值与命令信息)
// * Return: r_spi_packet (打包好的 24/32 位数据)
// * ==========================================================================
// */
DATA_Pack_Data:
    // 数据读取与打包
    MOV     r_val_raw, 0
    LBBO    r_val_raw, r_ptr_src, 0, 2
    XOR     r_val_raw.b1, r_val_raw.b1, (1<<7) //0x0080
    LSL     r_pkt, r_val_raw, 4
    OR      r_pkt, r_pkt, r_ch_id
    
    RET

///* ==========================================================================
// * Function: DATA_SPI_Out
// * Description: SPI 物理输出。通过 GPIO 模拟 SPI 时序发送数据
// * Params: r_spi_packet (待发送数据)
// * Return: None (修改 SPI 引脚电平)
// * ==========================================================================
// */
DATA_SPI_Out:
    // 发送字节一(高到低)
        MOV         r_spi_tx_buf, r_pkt
        MOV         r_spi_tx_mask, 0xFF000000
        AND         r_spi_tx_buf, r_spi_tx_buf, r_spi_tx_mask
        LSR         r_spi_tx_buf, r_spi_tx_buf, 24
        MOV         r_spi_cfg, SPIDAT_H1
        OR          r_spi_tx_buf, r_spi_tx_buf, r_spi_cfg
        MOV         r_spi_reg_ptr, SPIDAT_REG_ADDR
        SBBO        r_spi_tx_buf, r_spi_reg_ptr, #0x00, 4
        USER_DELAY  10

    // 发送字节二
        MOV         r_spi_tx_buf, r_pkt
        MOV         r_spi_tx_mask, 0x00FF0000
        AND         r_spi_tx_buf, r_spi_tx_buf, r_spi_tx_mask
        LSR         r_spi_tx_buf, r_spi_tx_buf, 16
        MOV         r_spi_cfg, SPIDAT_H1
        OR          r_spi_tx_buf, r_spi_tx_buf, r_spi_cfg
        MOV         r_spi_reg_ptr, SPIDAT_REG_ADDR
        SBBO        r_spi_tx_buf, r_spi_reg_ptr, #0x00, 4
        USER_DELAY  10

    // 发送字节三
        MOV         r_spi_tx_buf, r_pkt
        MOV         r_spi_tx_mask, 0x0000FF00
        AND         r_spi_tx_buf, r_spi_tx_buf, r_spi_tx_mask
        LSR         r_spi_tx_buf, r_spi_tx_buf, 8
        MOV         r_spi_cfg, SPIDAT_H1
        OR          r_spi_tx_buf, r_spi_tx_buf, r_spi_cfg
        MOV         r_spi_reg_ptr, SPIDAT_REG_ADDR
        SBBO        r_spi_tx_buf, r_spi_reg_ptr, #0x00, 4
        USER_DELAY  10

    // 发送字节四
        MOV         r_spi_tx_buf, r_pkt
        MOV         r_spi_tx_mask, 0x000000FF
        AND         r_spi_tx_buf, r_spi_tx_buf, r_spi_tx_mask
        LSR         r_spi_tx_buf, r_spi_tx_buf, 0
        MOV         r_spi_cfg, SPIDAT_H2
        OR          r_spi_tx_buf, r_spi_tx_buf, r_spi_cfg
        MOV         r_spi_reg_ptr, SPIDAT_REG_ADDR
        SBBO        r_spi_tx_buf, r_spi_reg_ptr, #0x00, 4
        USER_DELAY  10
        
    // spi发送完延时一会
        USER_DELAY  57 //114
    // 发送完毕
    RET

///* ==========================================================================
// * Function: LOOP_Buffer_Update
// * Description: 缓冲区与计数器管理
// * Detail: 1. 发送计数器自增
// *         2. 检查计数器是否达到 1000
// *         3. 若达到，调用 DATA_Buffer_Update 切换缓冲区
// * Params: r_send_count (修改: 计数器), r_pingpong_flag (修改: 乒乓标志)
// * Return: None
// * ==========================================================================
// */
LOOP_Buffer_Update:
    // 通常只指针后移即可
        ADD     r_ptr_data1, r_ptr_data1, #2
        ADD     r_ptr_data2, r_ptr_data2, #2
        ADD     r_ptr_data3, r_ptr_data3, #2
        ADD     r_ptr_data4, r_ptr_data4, #2
        ADD     r_ptr_data5, r_ptr_data5, #2
        ADD     r_ptr_data6, r_ptr_data6, #2
        ADD     r_ptr_data7, r_ptr_data7, #2
        ADD     r_ptr_data8, r_ptr_data8, #2
        // r_send_count负责计数
        ADD     r_send_count, r_send_count, #1
        QBEQ    PING_PONG_CHANGE, r_send_count, r_data_len
        //QBGE    PING_PONG_CHANGE, r_send_count, r_data_len
        JMP     LOOP_Buffer_Update_End

    // 一组数据周期后，改变指针基地址
    PING_PONG_CHANGE:
        MOV     r_send_count, #0
        MOV     r_flag_done, 1
        MOV     r_ptr_cfg, DA_CONFIG_FLAG_ADDR
        SBBO    r_flag_done, r_ptr_cfg, 0, 1
        // 定义 r_ping_pong == 0 时，处于发送ping数据的状态。
        // 因此，每过一组数据周期后，需要把发送的数据调换一下。
        QBEQ    DATA_PONG, r_ping_pong, #1
    DATA_PING:
        MOV     r_ping_pong, #1
        MOV     r_ptr_cfg, DA_CONFIG_PP_ADDR
        SBBO    r_ping_pong, r_ptr_cfg, 0, 1
        MOV     r_ptr_data1, DA_DATA1_ADDR_PONG
        MOV     r_ptr_data2, DA_DATA2_ADDR_PONG
        MOV     r_ptr_data3, DA_DATA3_ADDR_PONG
        MOV     r_ptr_data4, DA_DATA4_ADDR_PONG
        MOV     r_ptr_data5, DA_DATA5_ADDR_PONG
        MOV     r_ptr_data6, DA_DATA6_ADDR_PONG
        MOV     r_ptr_data7, DA_DATA7_ADDR_PONG
        MOV     r_ptr_data8, DA_DATA8_ADDR_PONG
        JMP     LOOP_Buffer_Update_End
    DATA_PONG:
        MOV     r_ping_pong, #0
        MOV     r_ptr_cfg, DA_CONFIG_PP_ADDR
        SBBO    r_ping_pong, r_ptr_cfg, 0, 1
        MOV     r_ptr_data1, DA_DATA1_ADDR_PING
        MOV     r_ptr_data2, DA_DATA2_ADDR_PING
        MOV     r_ptr_data3, DA_DATA3_ADDR_PING
        MOV     r_ptr_data4, DA_DATA4_ADDR_PING
        MOV     r_ptr_data5, DA_DATA5_ADDR_PING
        MOV     r_ptr_data6, DA_DATA6_ADDR_PING
        MOV     r_ptr_data7, DA_DATA7_ADDR_PING
        MOV     r_ptr_data8, DA_DATA8_ADDR_PING
        JMP     LOOP_Buffer_Update_End

LOOP_Buffer_Update_End:
        RET

