/**
 * @file sys_spi.c
 * @brief SPI驱动实现文件
 * @details 实现SPI驱动的核心功能，支持多设备切换，使用中断方式实现数据传输。
 * @ingroup SPI_DRV
 */

#include "sys_spi.h"

#include "soc_C6748.h"
#include "hw_types.h"
#include "hw_syscfg0_C6748.h"
#include "interrupt.h"
#include "psc.h"
#include "spi.h"
#include "string.h"


// 字符长度
#define CHAR_LENGTH             8
// SPI最大数据长度
#define SPI_MAX_DATA_LEN        0xFF

/* 变量定义 */
// 发送数据长度
volatile unsigned char SPI_txLength;
// 发送缓冲区计数
volatile unsigned char SPI_txCount;
// 发送缓冲区标志位
volatile unsigned char FLAG_SPI_TX;
// 接收数据长度
volatile unsigned char SPI_rxLength;
// 接收缓冲区计数
volatile unsigned char SPI_rxCount;
// 接收缓冲区标志位
volatile unsigned char FLAG_SPI_RX;
volatile unsigned char SPI_txBuffer[SPI_MAX_DATA_LEN] = {0}; // 发送缓冲区
volatile unsigned char SPI_rxBuffer[SPI_MAX_DATA_LEN] = {0}; // 接收缓冲区

/* 函数声明 */
// PSC初始化
static void PSCInit(void);
// GPIO引脚复用配置
static void GPIOBankPinMuxSet(void);
// 配置SPI数据格式
static void SPIDataFormatConfig(unsigned int dataFormat);
// SPI初始化
static void SPI_Init(void);
// 中断服务初始化
static void SPIInterruptInit(void);
// SPI中断服务函数
static void SPIIsr(void);
// SPI片选拉低，选中设备
static void SPI_CS_Assert(SPI_DeviceType device);
// SPI片选拉高，不选中设备
static void SPI_CS_Deassert(SPI_DeviceType device);


/* 函数定义 */

// SPI初始化函数
void Sys_SPI_Init(void)
{
    // 外设使能配置
    PSCInit();
    // GPIO 管脚复用配置
    GPIOBankPinMuxSet();
    // 中断初始化
    SPIInterruptInit();
    // SPI 初始化
    SPI_Init();
}

// PSC 初始化
static void PSCInit(void)
{
    // 使能 SPI 模块
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_SPI1, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);
}

// GPIO 管脚复用配置
static void GPIOBankPinMuxSet(void)
{
    // 配置 SPI1 引脚
    unsigned int savePinMux = 0;
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) & \
                        ~(SYSCFG_PINMUX5_PINMUX5_11_8 | \
                          SYSCFG_PINMUX5_PINMUX5_23_20 | \
                          SYSCFG_PINMUX5_PINMUX5_19_16);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) = \
         (PINMUX5_SPI1_CLK_ENABLE | PINMUX5_SPI1_SIMO_ENABLE | \
          PINMUX5_SPI1_SOMI_ENABLE | \
          savePinMux);

    // 配置 SPI1 CS0 引脚（用于 DAC 设备）
    unsigned int spi1CSPinMux = 0;
    spi1CSPinMux = (SYSCFG_PINMUX5_PINMUX5_7_4_NSPI1_SCS0 << \
                    SYSCFG_PINMUX5_PINMUX5_7_4_SHIFT);
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) & \
                       ~(SYSCFG_PINMUX5_PINMUX5_7_4);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) = \
         (spi1CSPinMux | savePinMux);

    // 配置 SPI1 CS1 引脚（用于 FLASH 设备）
    spi1CSPinMux = (SYSCFG_PINMUX5_PINMUX5_3_0_NSPI1_SCS1 << \
                    SYSCFG_PINMUX5_PINMUX5_3_0_SHIFT);
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) & \
                       ~(SYSCFG_PINMUX5_PINMUX5_3_0);
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) = \
         (spi1CSPinMux | savePinMux);
}

// SPI配置
static void SPI_Init(void)
{
    // 复位
    SPIReset(SOC_SPI_1_REGS);
    // 取消复位
    SPIOutOfReset(SOC_SPI_1_REGS);
    // 配置 主 / 从模式
    SPIModeConfigure(SOC_SPI_1_REGS, SPI_MASTER_MODE);
    // 使能 SIMO SOMI CLK 引脚 和 CS0、CS1引脚
    unsigned int  val = 0x00000E03;
    SPIPinControl(SOC_SPI_1_REGS, 0, 0, &val);
    //设置CS0、CS1空闲时为高电平
    SPIDefaultCSSet(SOC_SPI_1_REGS, (1 << 0) | (1 << 1));
    // 配置数据格式
    SPIDataFormatConfig(SPI_DATA_FORMAT0); // 供 DAC 设备使用
    SPIDataFormatConfig(SPI_DATA_FORMAT1); // 供 FLASH 设备使用
    // 映射中断到 INT1
    SPIIntLevelSet(SOC_SPI_1_REGS, SPI_RECV_INTLVL | SPI_TRANSMIT_INTLVL);
    // 使能 SPI
    SPIEnable(SOC_SPI_1_REGS);
}

// 配置 SPI 数据格式
static void SPIDataFormatConfig(unsigned int dataFormat)
{
    // 配置时钟
    SPIClkConfigure(SOC_SPI_1_REGS, 228000000, 50*1000000, dataFormat);

    if(dataFormat == SPI_DATA_FORMAT0)
        // 配置 SPI 时钟（用于 DAC 设备）
        SPIConfigClkFormat(SOC_SPI_1_REGS,
                           (SPI_CLK_POL_LOW  | SPI_CLK_INPHASE   ), dataFormat);  // POLARITY = 0 和 PHASE = 0  下降沿读数据
                           //(SPI_CLK_POL_LOW  | SPI_CLK_OUTOFPHASE), dataFormat);  // POLARITY = 0 和 PHASE = 1  上升沿读数据
                           //(SPI_CLK_POL_HIGH | SPI_CLK_INPHASE   ), dataFormat);  // POLARITY = 1 和 PHASE = 0  上升沿读数据
                           //(SPI_CLK_POL_HIGH | SPI_CLK_OUTOFPHASE), dataFormat);  // POLARITY = 1 和 PHASE = 1  下降沿读数据
    else if(dataFormat == SPI_DATA_FORMAT1)
        // 配置 SPI 时钟（用于 FLASH 设备）
        SPIConfigClkFormat(SOC_SPI_1_REGS,
                           //(SPI_CLK_POL_LOW  | SPI_CLK_INPHASE   ), dataFormat);  // POLARITY = 0 和 PHASE = 0  下降沿读数据
                           (SPI_CLK_POL_LOW  | SPI_CLK_OUTOFPHASE), dataFormat);  // POLARITY = 0 和 PHASE = 1  上升沿读数据
                           //(SPI_CLK_POL_HIGH | SPI_CLK_INPHASE   ), dataFormat);  // POLARITY = 1 和 PHASE = 0  上升沿读数据
                           //(SPI_CLK_POL_HIGH | SPI_CLK_OUTOFPHASE), dataFormat);  // POLARITY = 1 和 PHASE = 1  下降沿读数据
    else
        // 配置 SPI 时钟
        SPIConfigClkFormat(SOC_SPI_1_REGS,
                           (SPI_CLK_POL_LOW  | SPI_CLK_INPHASE   ), dataFormat);  // POLARITY = 0 和 PHASE = 0  下降沿读数据
                           //(SPI_CLK_POL_LOW  | SPI_CLK_OUTOFPHASE), dataFormat);  // POLARITY = 0 和 PHASE = 1  上升沿读数据
                           //(SPI_CLK_POL_HIGH | SPI_CLK_INPHASE   ), dataFormat);  // POLARITY = 1 和 PHASE = 0  上升沿读数据
                           //(SPI_CLK_POL_HIGH | SPI_CLK_OUTOFPHASE), dataFormat);  // POLARITY = 1 和 PHASE = 1  下降沿读数据

    // 配置 SPI 发送时 MSB 优先
    SPIShiftMsbFirst(SOC_SPI_1_REGS, dataFormat);

    // 设置字符长度
    SPICharLengthSet(SOC_SPI_1_REGS, CHAR_LENGTH, dataFormat);
}

// 中断服务初始化
static void SPIInterruptInit(void)
{
    // 注册中断服务函数
    IntRegister(C674X_MASK_INT9, SPIIsr);

    // 映射中断事件
    IntEventMap(C674X_MASK_INT9, SYS_INT_SPI1_INT);

    // 使能可屏蔽中断
    IntEnable(C674X_MASK_INT9);
}

// SPI 中断服务函数
static void SPIIsr(void)
{
    unsigned int intCode = 0;
    // 清除中断事件
    IntEventClear(SYS_INT_SPI1_INT);
    // 取得中断代码
    intCode = SPIInterruptVectorGet(SOC_SPI_1_REGS);

    while (intCode)
    {
        // 发送缓冲区为空中断
        if(intCode == SPI_TX_BUF_EMPTY)
        {
            SPI_txLength--;
            // SPITransmitData1(SOC_SPI_1_REGS, *SPI_txCount);
            SPITransmitData1(SOC_SPI_1_REGS, SPI_txBuffer[SPI_txCount]);
            SPI_txCount++;
            if (!SPI_txLength)
            {
                FLAG_SPI_TX = 1;
                SPIIntDisable(SOC_SPI_1_REGS, SPI_TRANSMIT_INT);
            }
        }

        // 接收缓冲区满中断
        if(intCode == SPI_RECV_FULL)
        {
            SPI_rxLength--;
            //*SPI_rxCount = (char)SPIDataReceive(SOC_SPI_1_REGS);
            SPI_rxBuffer[SPI_rxCount] = (char)SPIDataReceive(SOC_SPI_1_REGS);
            SPI_rxCount++;
            if (!SPI_rxLength)
            {
                FLAG_SPI_RX = 1;
                SPIIntDisable(SOC_SPI_1_REGS, SPI_RECV_INT);
            }
        }

        // 取得中断代码
        intCode = SPIInterruptVectorGet(SOC_SPI_1_REGS);
    }
}

// SPI片选拉低，选中设备
static void SPI_CS_Assert(SPI_DeviceType device)
{
    if(device == SPI_DEVICE_DAC)
    {
        SPIDat1Config(SOC_SPI_1_REGS, (SPI_CSHOLD | SPI_DATA_FORMAT0), (1<<0));
    }
    else if(device == SPI_DEVICE_FLASH)
    {
        SPIDat1Config(SOC_SPI_1_REGS, (SPI_CSHOLD | SPI_DATA_FORMAT1), (1<<1));
    }
}

// SPI片选拉高，不选中设备
static void SPI_CS_Deassert(SPI_DeviceType device)
{
    if(device == SPI_DEVICE_DAC)
    {
        SPIDat1Config(SOC_SPI_1_REGS, SPI_DATA_FORMAT0, (1<<0));
    }
    else if(device == SPI_DEVICE_FLASH)
    {
        SPIDat1Config(SOC_SPI_1_REGS, SPI_DATA_FORMAT1, (1<<1));
    }
}

// 向指定SPI设备连续写入多个字节数据
void SPI_SeqWrite_Reg(SPI_DeviceType device, unsigned char *send_dat, unsigned int send_len)
{
    // 复制数据到全局缓冲区
    memcpy((void *)SPI_txBuffer, send_dat, send_len);
    SPI_txLength = send_len;
    SPI_rxLength = 0;//send_len;//0;
    // SPI_txCount = &SPI_txBuffer[0];
    SPI_txCount = 0;
    //SPI_rxCount = &SPI_rxBuffer[0];
    SPI_rxCount = 0;
    
    // 选择设备
    SPI_CS_Assert(device);
    
    // 使能中断
    FLAG_SPI_TX = 0;
    SPIIntEnable(SOC_SPI_1_REGS, SPI_TRANSMIT_INT);//(SPI_RECV_INT | SPI_TRANSMIT_INT));
    
    // 等待传输完成
    while(!FLAG_SPI_TX);
    FLAG_SPI_TX = 0;
    
    // 取消选择设备
    SPI_CS_Deassert(device);
}

// 从指定SPI设备连续读取多个字节数据
void SPI_SeqRead_Reg(SPI_DeviceType device, unsigned char *send_dat, unsigned int send_len, unsigned char *recv_dat, unsigned int recv_len)
{
    // 复制数据到全局缓冲区
    memcpy((void *)SPI_txBuffer, send_dat, send_len);
    SPI_txLength = send_len;
    SPI_rxLength = recv_len;
    //SPI_txCount = &SPI_txBuffer[0];
    SPI_txCount = 0;
    //SPI_rxCount = &SPI_rxBuffer[0];
    SPI_rxCount = 0;
    
    // 选择设备
    SPI_CS_Assert(device);
    
    // 使能中断
    FLAG_SPI_RX = 0;
    SPIIntEnable(SOC_SPI_1_REGS, (SPI_RECV_INT | SPI_TRANSMIT_INT));
    
    // 等待传输完成
    while(!FLAG_SPI_RX);
    FLAG_SPI_RX = 0;
    
    // 取消选择设备
    SPI_CS_Deassert(device);
    
    // 复制接收数据
    if(recv_dat != NULL)
    {
        memcpy(recv_dat, (const void *)SPI_rxBuffer, recv_len);
    }
}
