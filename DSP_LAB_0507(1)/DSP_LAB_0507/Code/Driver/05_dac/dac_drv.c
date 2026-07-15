/**
 * @file dac_drv.c
 * @brief DAC驱动实现文件
 * @details 实现DAC的驱动功能，包括初始化、启动和停止操作。
 * @ingroup DAC_DRV
 */

#include "dac_drv.h"
#include "dac_api.h"
#include "dac_pin.h"
#include "dac_pru.h"
#include "dac_tim1.h"
#include "sys_spi.h"
#include "delay.h"

#include "soc_C6748.h"
#include "gpio.h"


/* 变量声明 */
static unsigned char spi_tx_data[4]; // 发送缓冲区

/* 函数声明 */
static void DAC8568_Init(void);
static void DAC8568_Update_Reg(void);

// DAC驱动初始化函数
void Dac_Init(unsigned int clkFreq, unsigned int sampleLen, unsigned char dacSel)
{
    // 初始化引脚
    Dac_Pin_Init();

    // 初始化SPI
    Sys_SPI_Init();

    // 初始化DAC8568
    DAC8568_Init();
    
    // 初始化定时器1
    Dac_Timer1_Init(clkFreq);

    // DAC输出使能关闭
    DA_En = 0x00;

    // 选择DAC的输出通道
    DA_Sel = dacSel;

    // 设置DAC输出长度，PRU0根据该长度进行数据传输
    DA_Len = sampleLen;

    // 初始化PRU0
    Dac_Pru0_Init();
}

// DAC8568初始化函数
static void DAC8568_Init(void)
{
    Dac_Load_PinSet(1);

    // 复位DAC8568
    spi_tx_data[0] = 0x1C;
    spi_tx_data[1] = 0x00;
    spi_tx_data[2] = 0x00;
    spi_tx_data[3] = 0x00;
    // DAC8568写寄存器函数
    SPI_SeqWrite_Reg(SPI_DEVICE_DAC, spi_tx_data, 4);
    DAC8568_Update_Reg();


    // 配置内部参考电压
    spi_tx_data[0] = 0x08;
    spi_tx_data[1] = 0x00;
    spi_tx_data[2] = 0x00;
    spi_tx_data[3] = 0x01;
    // DAC8568写寄存器函数
    SPI_SeqWrite_Reg(SPI_DEVICE_DAC, spi_tx_data, 4);
    DAC8568_Update_Reg();
}

// AD8568硬件更新DA
static void DAC8568_Update_Reg(void)
{
    Dac_Load_PinSet(0);
    Dac_Load_PinSet(1);
}

// DAC启动函数
void Dac_Start(void)
{
    // 定时器中断打开
    Dac_Timer1_Start();
    // DAC输出使能
    DA_En = 0x01;
}

// DAC停止函数
void Dac_Stop(void)
{
    // 定时器中断关闭
    Dac_Timer1_Stop();
    // DAC输出使能关闭
    DA_En = 0x00;
}
