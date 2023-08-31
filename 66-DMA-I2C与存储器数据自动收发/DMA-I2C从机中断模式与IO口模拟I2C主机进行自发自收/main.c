/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于STC32G为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

内部集成I2C串行总线控制器做从机模式，SDA->P2.4, SCL->P2.5;
IO口模拟I2C做主机模式，SDA->P0.0, SCL->P0.1;
通过外部飞线连接 P0.0->P2.4, P0.1->P2.5，实现I2C自发自收功能。

用STC的MCU的IO方式驱动8位数码管。
使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下,用户修改MCU主时钟频率时,自动定时于1ms.
计数器每秒钟加1, 计数范围为0~9999.

显示效果为: 上电后主机每秒钟发送一次计数数据，并在左边4个数码管上显示发送内容；从机接收到数据后在右边4个数码管显示。

下载时, 选择时钟 24MHz (用户可自行修改频率).

******************************************/

#include "..\..\comm\STC32G.h"

#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL

/****************************** 用户定义宏 ***********************************/

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

#define DIS_DOT     0x20
#define DIS_BLACK   0x10
#define DIS_        0x11

/*****************************************************************************/

#define SLAW    0x5A
#define SLAR    0x5B

sbit    SDA = P0^0; //定义SDA
sbit    SCL = P0^1; //定义SCL


/*************  本地常量声明    **************/
u8 code t_display[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

u8 code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //位码

/*************  本地变量声明    **************/

u8  LED8[8];        //显示缓冲
u8  display_index;  //显示位索引

bit B_1ms;          //1ms标志
bit DisplayFlag;
bit	DmaTxFlag=0;
bit	DmaRxFlag=0;

u8 addr;
u8 xdata DmaBuffer[32];
u16 msecond;
u16 Test_cnt;   //测试用的秒计数变量
u8  tmp[4];     //通用数组
u8  step;

void I2C_config(void);
void DMA_Config(void);
void Timer0_config(void);
void WriteNbyte(u8 addr, u8 *p, u8 number);
void ReadNbyte(u8 addr, u8 *p, u8 number);


void I2C_Isr() interrupt I2C_VECTOR
{
    u8 dat;
    
    if (I2CSLST & STAIF)
    {
        I2CSLST &= ~STAIF;                       //处理START事件
        step = 0;
        DMA_I2CT_CR = 0x00;
        DMA_I2CR_CR = 0x00;
        DMA_I2C_CR = 0x00;
    }
    else if (I2CSLST & RXIF)
    {
        I2CSLST &= ~RXIF;                       //处理RECV事件
        dat = I2CRXD;
        switch (step)
        {
        case 0:                                 //处理RECV事件（RECV DEVICE ADDR）
            if (dat & 0x01)
            {
                I2CTXD = DmaBuffer[addr];
                
                DMA_I2C_CR = 0x03;
                DMA_I2C_ST1 = 0x04;
                DMA_I2C_ST2 = 0x00;
                
                DMA_I2CT_CR = 0xc0;
            }
            step++;
            break;
        case 1:                                 //处理RECV事件（RECV MEMORY ADDR-HIBYTE）
            addr = dat * 256;
            step++;
            break;
        case 2:                                 //处理RECV事件（RECV MEMORY ADDR-LOBYTE）
            addr += dat;
            step++;
            {
                DMA_I2C_CR = 0x03;
                DMA_I2C_ST1 = 0x04;
                DMA_I2C_ST2 = 0x00;
                
                DMA_I2CR_CR = 0xc0;
            }
            break;
        case 3:                                 //处理RECV事件（RECV DATA）
//            DmaBuffer[addr++] = dat;
            break;
        }
    }
    else if (I2CSLST & TXIF)
    {
        I2CSLST &= ~TXIF;                       //处理SEND事件
        if (I2CSLST & SLACKI)
        {
            I2CTXD = 0xff;
        }
        else
        {
//            I2CTXD = DmaBuffer[++addr];
        }
    }
    else if (I2CSLST & STOIF)
    {
        I2CSLST &= ~STOIF;                       //处理STOP事件
        
        DMA_I2CT_CR = 0x00;
        DMA_I2CR_CR = 0x00;
        DMA_I2C_CR = 0x00;
        
        step = 0;
        addr = 0;
        
        DisplayFlag = 1;
    }

    if(DMA_I2C_CR & 0x04)   //ACKERR
    {
        DMA_I2C_CR &= ~0x04;  //发数据后收到NAK
    }
}

/********************* 主函数 *************************/
void main(void)
{
    u8  i;

    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x30;   P0M0 = 0x30;   //设置P0.4、P0.5为漏极开路(实验箱加了上拉电阻到3.3V)
    P1M1 = 0x32;   P1M0 = 0x32;   //设置P1.1、P1.4、P1.5为漏极开路(实验箱加了上拉电阻到3.3V), P1.1在PWM当DAC电路通过电阻串联到P2.3
    P2M1 = 0x3c;   P2M0 = 0x3c;   //设置P2.2~P2.5为漏极开路(实验箱加了上拉电阻到3.3V)，设置开漏模式需要断开PWM当DAC电路中的R2电阻
    P3M1 = 0x50;   P3M0 = 0x50;   //设置P3.4、P3.6为漏极开路(实验箱加了上拉电阻到3.3V)
    P4M1 = 0x3c;   P4M0 = 0x3c;   //设置P4.2~P4.5为漏极开路(实验箱加了上拉电阻到3.3V)
    P5M1 = 0x0c;   P5M0 = 0x0c;   //设置P5.2、P5.3为漏极开路(实验箱加了上拉电阻到3.3V)
    P6M1 = 0xff;   P6M0 = 0xff;   //设置为漏极开路(实验箱加了上拉电阻到3.3V)
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

    I2C_config();
    DMA_Config();

    Timer0_config();
    display_index = 0;
    DisplayFlag = 0;

    addr = 0;
    I2CTXD = DmaBuffer[addr];
    EA = 1;

    for(i=0; i<8; i++)  LED8[i] = DIS_; //上电全部显示-
    
    while (1)
    {
        if(DisplayFlag)
        {
            DisplayFlag = 0;
            LED8[4] = DmaBuffer[0];
            LED8[5] = DmaBuffer[1];
            LED8[6] = DmaBuffer[2];
            LED8[7] = DmaBuffer[3];
        }
        
        if(B_1ms)
        {
            B_1ms = 0;
            
            if(++msecond >= 1000)   //1秒到
            {
                msecond = 0;        //清1000ms计数
                Test_cnt++;         //秒计数+1
                if(Test_cnt >= 10000)    Test_cnt = 0;   //秒计数范围为0~9999

                tmp[0] = Test_cnt / 1000;
                tmp[1] = (Test_cnt % 1000) / 100;
                tmp[2] = (Test_cnt % 100) / 10;
                tmp[3] = Test_cnt % 10;
                LED8[0] = tmp[0];
                LED8[1] = tmp[1];
                LED8[2] = tmp[2];
                LED8[3] = tmp[3];

                WriteNbyte(0, tmp, 4);
            }
        }
    }
}


//========================================================================
// 函数: void Timer0_config(void)
// 描述: Timer0初始化函数。
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2020-6-10
//========================================================================
void Timer0_config(void)
{
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
}

/********************** 显示扫描函数 ************************/
void DisplayScan(void)
{   
    P7 = ~T_COM[7-display_index];
    P6 = ~t_display[LED8[display_index]];
    if(++display_index >= 8)    display_index = 0;  //8位结束回0
}

/********************** Timer0 1ms中断函数 ************************/
void timer0 (void) interrupt 1
{
    DisplayScan();  //1ms扫描显示一位
    B_1ms = 1;      //1ms标志
}

//========================================================================
// 函数: void I2C_config(void)
// 描述: I2C 初始化函数。
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2022-3-18
//========================================================================
void I2C_config(void)
{
    P_SW2 = (P_SW2 & ~(3<<4)) | (1<<4);     //IO口切换. 0: P1.4 P1.5, 1: P2.4 P2.5, 3: P3.3 P3.2

    I2CCFG = 0x80;                          //使能I2C从机模式
    I2CSLADR = 0x5a;                        //设置从机设备地址为5A
    I2CSLST = 0x00;
    I2CSLCR = ESTAI | ERXI | ETXI | ESTOI;  //使能从机模式中断
}

//========================================================================
// 函数: void DMA_Config(void)
// 描述: SPI DMA 功能配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2021-5-6
//========================================================================
void DMA_Config(void)
{
	DMA_I2CT_STA = 0x00;
	DMA_I2CT_CFG = 0x80;		//bit7 1:Enable Interrupt
	DMA_I2CT_AMT = 0xff;		//设置传输总字节数(低8位)：n+1
	DMA_I2CT_AMTH = 0x00;		//设置传输总字节数(高8位)：n+1
	DMA_I2CT_TXAH = (u8)((u16)&DmaBuffer >> 8);	//I2C发送数据存储地址
	DMA_I2CT_TXAL = (u8)((u16)&DmaBuffer);
	DMA_I2CT_CR = 0x80;		//bit7 1:使能 I2CT_DMA, bit6 1:开始 I2CT_DMA

	DMA_I2CR_STA = 0x00;
	DMA_I2CR_CFG = 0x80;		//bit7 1:Enable Interrupt
	DMA_I2CR_AMT = 0xff;		//设置传输总字节数(低8位)：n+1
	DMA_I2CR_AMTH = 0x00;		//设置传输总字节数(高8位)：n+1
	DMA_I2CR_RXAH = (u8)((u16)&DmaBuffer >> 8);	//I2C接收数据存储地址
	DMA_I2CR_RXAL = (u8)((u16)&DmaBuffer);
	DMA_I2CR_CR = 0x81;		//bit7 1:使能 I2CT_DMA, bit5 1:开始 I2CT_DMA, bit0 1:清除 FIFO

	DMA_I2C_ST1 = 0xff;		//设置需要传输字节数(低8位)：n+1
	DMA_I2C_ST2 = 0x00;		//设置需要传输字节数(高8位)：n+1
}

/****************************/
void I2C_Delay(void) //for normal MCS51, delay (2 * dly + 4) T, for STC12Cxxxx delay (4 * dly + 10) T
{
    u16  dly;
    dly = MAIN_Fosc / 2000000UL;        //按2us计算
    while(--dly);
}

/****************************/
void I2C_Start(void)               //start the I2C, SDA High-to-low when SCL is high
{
    SDA = 1;
    I2C_Delay();
    SCL = 1;
    I2C_Delay();
    SDA = 0;
    I2C_Delay();
    SCL = 0;
    I2C_Delay();
}       


void I2C_Stop(void)                 //STOP the I2C, SDA Low-to-high when SCL is high
{
    SDA = 0;
    I2C_Delay();
    SCL = 1;
    I2C_Delay();
    SDA = 1;
    I2C_Delay();
}

void S_ACK(void)              //Send ACK (LOW)
{
    SDA = 0;
    I2C_Delay();
    SCL = 1;
    I2C_Delay();
    SCL = 0;
    I2C_Delay();
}

void S_NoACK(void)           //Send No ACK (High)
{
    SDA = 1;
    I2C_Delay();
    SCL = 1;
    I2C_Delay();
    SCL = 0;
    I2C_Delay();
}
        
void I2C_Check_ACK(void)         //Check ACK, If F0=0, then right, if F0=1, then error
{
    SDA = 1;
    I2C_Delay();
    SCL = 1;
    I2C_Delay();
    F0  = SDA;
    SCL = 0;
    I2C_Delay();
}

/****************************/
void I2C_WriteAbyte(u8 dat)     //write a byte to I2C
{
    u8 i;
    i = 8;
    do
    {
        if(dat & 0x80)  SDA = 1;
        else            SDA = 0;
        dat <<= 1;
        I2C_Delay();
        SCL = 1;
        I2C_Delay();
        SCL = 0;
        I2C_Delay();
    }
    while(--i);
}

/****************************/
u8 I2C_ReadAbyte(void)          //read A byte from I2C
{
    u8 i,dat;
    i = 8;
    SDA = 1;
    do
    {
        SCL = 1;
        I2C_Delay();
        dat <<= 1;
        if(SDA)     dat++;
        SCL  = 0;
        I2C_Delay();
    }
    while(--i);
    return(dat);
}

/****************************/
void WriteNbyte(u8 addr, u8 *p, u8 number)          /*  WordAddress,First Data Address,Byte lenth   */
                                                            //F0=0,right, F0=1,error
{
    I2C_Start();
    I2C_WriteAbyte(SLAW);
    I2C_Check_ACK();
    if(!F0)
    {
        I2C_WriteAbyte(addr);
        I2C_Check_ACK();
        if(!F0)
        {
            do
            {
                I2C_WriteAbyte(*p);     p++;
                I2C_Check_ACK();
                if(F0)  break;
            }
            while(--number);
        }
    }
    I2C_Stop();
}


/****************************/
void ReadNbyte(u8 addr, u8 *p, u8 number)       /*  WordAddress,First Data Address,Byte lenth   */
{
    I2C_Start();
    I2C_WriteAbyte(SLAW);
    I2C_Check_ACK();
    if(!F0)
    {
        I2C_WriteAbyte(addr);
        I2C_Check_ACK();
        if(!F0)
        {
            I2C_Start();
            I2C_WriteAbyte(SLAR);
            I2C_Check_ACK();
            if(!F0)
            {
                do
                {
                    *p = I2C_ReadAbyte();   p++;
                    if(number != 1)     S_ACK();    //send ACK
                }
                while(--number);
                S_NoACK();          //send no ACK
            }
        }
    }
    I2C_Stop();
}

//========================================================================
// 函数: void I2C_DMA_Interrupt (void) interrupt 60/61
// 描述: I2C DMA中断函数
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-5-8
// 备注: 
//========================================================================
void I2C_DMA_Interrupt(void) interrupt 13
{
	if(DMA_I2CT_STA & 0x01)   //发送完成
	{
		DMA_I2CT_STA &= ~0x01;  //清除标志位
		DmaTxFlag = 0;
	}
	if(DMA_I2CT_STA & 0x04)   //数据覆盖
	{
		DMA_I2CT_STA &= ~0x04;  //清除标志位
	}
	
	if(DMA_I2CR_STA & 0x01)   //接收完成
	{
		DMA_I2CR_STA &= ~0x01;  //清除标志位
		DmaRxFlag = 0;
	}
	if(DMA_I2CR_STA & 0x02)   //数据丢弃
	{
		DMA_I2CR_STA &= ~0x02;  //清除标志位
	}
}
