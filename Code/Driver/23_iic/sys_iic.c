/**
 * @file sys_iic.c
 * @brief IIC驱动实现文件
 * @details 实现IIC驱动的核心功能，使用中断方式实现数据传输。
 * @ingroup IIC_DRV
 */

#include "sys_iic.h"

#include "soc_C6748.h"
#include "hw_types.h"
#include "hw_syscfg0_C6748.h"
#include "interrupt.h"
#include "i2c.h"


/* 函数声明 */
// GPIO 管脚复用配置
static void GPIOBankPinMuxSet(void);
// IIC 初始化
static void IICInit(void);
// IIC 中断初始化
static void IICInterruptInit(void);
// IIC 中断服务函数
static void IICIsr(void);


/* 变量定义 */
volatile unsigned char IIC_slaveData[300];  // IIC数据收发缓冲区
volatile unsigned int  IIC_dataIdx = 0;     // 数据索引
volatile unsigned char IIC_txCompFlag = 1;  // 发送完成标志
extern volatile unsigned int AckRolling;    // 应答标志




/* 函数定义 */

// 系统IIC初始化
void Sys_IIC_Init(void)
{
    // I2C0外设的时钟来源于PLL旁路时钟
    // 不需要PSCInit
    // I2C管脚配置
    GPIOBankPinMuxSet();
    // 中断初始化
    IICInterruptInit();
    // IIC初始化
    IICInit();
}

// GPIO 管脚复用配置
static void GPIOBankPinMuxSet(void)
{
    // I2CPinMuxSetup(0);
    unsigned int savePinMux = 0;
    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) & \
                       ~(SYSCFG_PINMUX4_PINMUX4_15_12 | \
                         SYSCFG_PINMUX4_PINMUX4_11_8);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) = \
         (PINMUX4_I2C0_SDA_ENABLE | \
          PINMUX4_I2C0_SCL_ENABLE | \
          savePinMux);
}


// IIC配置
static void IICInit(void)
{
    //IIC 复位 / 禁用
    I2CMasterDisable(SOC_I2C_0_REGS);

    // 配置总线速度为 100KHz
    I2CMasterInitExpClk(SOC_I2C_0_REGS, 24000000, 8000000, 100000);

    // 设置从设备地址，默认用来开启触摸屏
    // 注意：在与不同I2C设备通信前，需要调用 I2CMasterSlaveAddrSet 函数设置对应设备的地址
    //I2CMasterSlaveAddrSet(SOC_I2C_0_REGS, GT1151_ADDRESS);
    IIC_SelectDevice(IIC_DEVICE_TOUCH);

    // IIC 使能
    I2CMasterEnable(SOC_I2C_0_REGS);
}

// 中断服务初始化
static void IICInterruptInit(void)
{
    IntRegister(C674X_MASK_INT8, IICIsr);
    IntEventMap(C674X_MASK_INT8, SYS_INT_I2C0_INT);
    IntEnable(C674X_MASK_INT8);
}

// IIC 中断服务函数
// 处理IIC中断事件，包括发送就绪、接收就绪、停止条件和非应答等
static void IICIsr(void)
{
    volatile unsigned int intCode = 0;

    // 取得中断代码
    intCode = I2CInterruptVectorGet(SOC_I2C_0_REGS);

    // 处理所有待处理的中断
    while(intCode!=0)
    {
        // 清除中断事件
        IntEventClear(SYS_INT_I2C0_INT);

        // 发送就绪中断：发送一个字节数据
        if (intCode == I2C_INTCODE_TX_READY)
        {
            // 将数据从缓冲区发送到IIC数据寄存器
            I2CMasterDataPut(SOC_I2C_0_REGS, IIC_slaveData[IIC_dataIdx]);
            // 移动数据索引到下一个位置
            IIC_dataIdx++;
        }

        // 接收就绪中断：接收一个字节数据
        if(intCode == I2C_INTCODE_RX_READY)
        {
            // 从IIC数据寄存器读取数据到缓冲区
            IIC_slaveData[IIC_dataIdx] = I2CMasterDataGet(SOC_I2C_0_REGS);
            // 移动数据索引到下一个位置
            IIC_dataIdx++;
        }

        // 停止条件中断：数据传输完成
        if (intCode == I2C_INTCODE_STOP)
        {
            // 禁用所有IIC中断
            I2CMasterIntDisableEx(SOC_I2C_0_REGS, I2C_INT_TRANSMIT_READY |
                                                I2C_INT_DATA_READY |
                                                I2C_INT_NO_ACK |
                                                I2C_INT_STOP_CONDITION);
            // 设置传输完成标志
            IIC_txCompFlag = 0;
        }

        // 非应答中断：从设备未应答
        if (intCode == I2C_INTCODE_NACK)
        {
            // 禁用所有IIC中断
            I2CMasterIntDisableEx(SOC_I2C_0_REGS, I2C_INT_TRANSMIT_READY |
                                                I2C_INT_DATA_READY |
                                                I2C_INT_NO_ACK |
                                                I2C_INT_STOP_CONDITION);
            // 产生停止位，结束通信
            I2CMasterStop(SOC_I2C_0_REGS);

            // 清除停止条件状态
            I2CStatusClear(SOC_I2C_0_REGS, I2C_CLEAR_STOP_CONDITION);

            // 清除中断
            IntEventClear(SYS_INT_I2C0_INT);
            // 设置传输完成标志
            IIC_txCompFlag = 0;
            // 设置非应答标志
            AckRolling = 1;
        }

        // 检查是否发送了非应答
        if (I2CMasterIntStatus(SOC_I2C_0_REGS) & I2C_ICSTR_NACKSNT)
        {
            // 禁用所有IIC中断
            I2CMasterIntDisableEx(SOC_I2C_0_REGS, I2C_INT_TRANSMIT_READY |
                                                I2C_INT_DATA_READY |
                                                I2C_INT_NO_ACK |
                                                I2C_INT_STOP_CONDITION);

            // 产生停止位，结束通信
            I2CMasterStop(SOC_I2C_0_REGS);

            // 清除非应答发送和停止条件状态
            I2CStatusClear(SOC_I2C_0_REGS, (I2C_CLEAR_NO_ACK_SENT |
                                            I2C_CLEAR_STOP_CONDITION));

            // 清除中断
            IntEventClear(SYS_INT_I2C0_INT);
            // 设置传输完成标志
            IIC_txCompFlag = 0;
        }

        // 获取下一个中断代码
        intCode = I2CInterruptVectorGet(SOC_I2C_0_REGS);
    }
}



// IIC选择设备函数
void IIC_SelectDevice(IIC_DeviceType device)
{
    if (device == IIC_DEVICE_TOUCH)
    {
        // 设置触摸屏从机地址
        I2CMasterSlaveAddrSet(SOC_I2C_0_REGS, GT1151_ADDRESS);
    }
    else if (device == IIC_DEVICE_EEPROM)
    {
        // 设置EEPROM从机地址
        I2CMasterSlaveAddrSet(SOC_I2C_0_REGS, EEPROM_ADDRESS);
    }
}



// IIC 发送数据函数
void IICSend( unsigned int dataCnt)
{
    IIC_txCompFlag = 1;
    IIC_dataIdx = 0;

    while(I2CMasterBusBusy(SOC_I2C_0_REGS));

    I2CSetDataCount(SOC_I2C_0_REGS, dataCnt);

    I2CMasterControl(SOC_I2C_0_REGS, I2C_CFG_MST_TX | I2C_CFG_STOP);

    I2CMasterIntEnableEx(SOC_I2C_0_REGS, I2C_INT_TRANSMIT_READY | I2C_INT_STOP_CONDITION | I2C_INT_NO_ACK);

    I2CMasterStart(SOC_I2C_0_REGS);

    // 等待数据发送完成
    while(IIC_txCompFlag);

    while(I2CMasterBusBusy(SOC_I2C_0_REGS));
}

// IIC 接收数据函数
// 接收指定大小的数据，将数据存储在IIC_slaveData数组中
void IICReceive(unsigned int dataCnt)
{
    IIC_txCompFlag = 1;
    IIC_dataIdx = 0;

    while(I2CMasterBusBusy(SOC_I2C_0_REGS));

    I2CSetDataCount(SOC_I2C_0_REGS, dataCnt);

    I2CMasterControl(SOC_I2C_0_REGS, I2C_CFG_MST_RX | I2C_CFG_STOP);

    I2CMasterIntEnableEx(SOC_I2C_0_REGS, I2C_INT_DATA_READY | I2C_INT_STOP_CONDITION | I2C_INT_NO_ACK);

    I2CMasterStart(SOC_I2C_0_REGS);

    // 等待数据接收完成
    while(IIC_txCompFlag);

    while(I2CMasterBusBusy(SOC_I2C_0_REGS));
}


// IIC 写入寄存器函数
// 写入指定寄存器地址的数据
void IIC_Write_Reg(unsigned char RegBIT, unsigned int reg, unsigned char dat)
{
    if(RegBIT == REG8)
    {
        IIC_slaveData[0] = reg;
        IIC_slaveData[1] = dat;
        IICSend(2);        //传输数据
    }
    else if(RegBIT == REG16)
    {
        IIC_slaveData[0] = reg>>8;		//取高八位
        IIC_slaveData[1] = reg&0x00FF;	//取低八位
        IIC_slaveData[2] = dat;
        IICSend(3);        //传输数据
    }
}

// IIC 写入函数
void IIC_Write(unsigned char dat)
{
    IIC_slaveData[0] = dat;
//    IICReceive(1);
    IICSend(1);
}


// IIC 读取寄存器函数
// 读取指定寄存器地址的数据
void IIC_Read_Reg(unsigned char RegBIT, unsigned int reg, unsigned char *recv_dat)
{
    if(RegBIT == REG8)
    {
        IIC_slaveData[0] = reg;
        IICSend(1);        //发送数据
    }
    else if(RegBIT == REG16)
    {
        IIC_slaveData[0] = reg>>8;		//取高八位
        IIC_slaveData[1] = reg&0x00FF;	//取低八位
        IICSend(2);        //发送数据
    }
    IICReceive(1);
    memcpy(recv_dat,(const void *)IIC_slaveData,1);
}

// IIC 读取函数
void IIC_Read(unsigned char *recv_dat)
{
    IICReceive(1);
    memcpy(recv_dat,(const void *)IIC_slaveData,1);
}

// IIC 连续写入寄存器函数
// 连续写入指定寄存器地址的数据
void IIC_SeqWrite_Reg(unsigned char RegBIT, unsigned int reg, unsigned char *send_dat, unsigned int send_len)
{
//    unsigned char cnt;
    if(RegBIT == REG8)
    {
        IIC_slaveData[0] = reg;
        // for(cnt=0;cnt<send_len;cnt++)
        // {
        // 	IIC_slaveData[cnt+1]=send_dat[cnt];
        // }
        memcpy((void *)(IIC_slaveData+1), send_dat, send_len);
        IICSend(send_len+1);		//传输数据
    }
    else if(RegBIT == REG16)
    {
        IIC_slaveData[0] = reg>>8;		//取高八位
        IIC_slaveData[1] = reg&0x00FF;	//取低八位
        // for(cnt=0;cnt<send_len;cnt++)
        // {
        // 	IIC_slaveData[cnt+2]=send_dat[cnt];
        // }
        memcpy((void *)(IIC_slaveData+2), send_dat, send_len);
        IICSend(send_len+2);		//传输数据
    }
}

// IIC 连续读取寄存器函数
// 连续读取指定寄存器地址的数据
void IIC_SeqRead_Reg(unsigned char RegBIT, unsigned int reg, unsigned char *recv_dat, unsigned int recv_len)
{
    if(RegBIT == REG8)
    {
        IIC_slaveData[0] = reg;
        IICSend(1);		//发送数据
    }
    else if(RegBIT == REG16)
    {
        IIC_slaveData[0] = reg>>8;		//取高八位
        IIC_slaveData[1] = reg&0x00FF;	//取低八位
        IICSend(2);		//发送数据
    }
    IICReceive(recv_len);
    memcpy(recv_dat,(const void *)IIC_slaveData,recv_len);
}

