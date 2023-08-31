/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/


/*************  本程序功能说明  **************

本例程基于STC32G为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

串口发指令通过I2C DMA读写AT24C02数据.

默认波特率:  115200,8,N,1. 

串口命令设置: (字母不区分大小写)
    W 0x12 1234567890 --> 写入操作  十六进制地址  写入内容.
    R 0x12 10         --> 读出操作  十六进制地址  读出字节数.

下载时, 选择时钟 22.1184MHz (用户可自行修改频率).

******************************************/


#include "..\..\comm\STC32G.h"
#include "intrins.h"
#include "stdio.h"

#define     MAIN_Fosc       22118400L   //定义主时钟（精确计算115200波特率）

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;


/****************************** 用户定义宏 ***********************************/

#define Baudrate        115200L
#define TM              (65536 -(MAIN_Fosc/Baudrate/4))

/*****************************************************************************/


/*************  本地常量声明    **************/

#define EE_BUF_LENGTH       255          //
#define UART2_BUF_LENGTH    (EE_BUF_LENGTH+7)   //串口缓冲长度

#define SLAW    0xA0
#define SLAR    0xA1

/*************  本地变量声明    **************/

u8 EEPROM_addr;
u8 xdata DmaTxBuffer[256];
u8 xdata DmaRxBuffer[256];

u8  RX2_TimeOut;
u16 RX2_Cnt;    //接收计数
bit B_TX2_Busy; //发送忙标志
bit	DmaTxFlag=0;
bit	DmaRxFlag=0;

u8  RX2_Buffer[UART2_BUF_LENGTH];   //接收缓冲

/*************  本地函数声明    **************/

void I2C_init(void);
void WriteNbyte(u8 addr, u8 number);
void ReadNbyte( u8 addr, u8 number);
void delay_ms(u8 ms);
void RX2_Check(void);
void DMA_Config(void);

/******************** 串口打印函数 ********************/
void UartInit(void)
{
    S2_S  = 1;          //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
    S2CFG |= 0x01;      //使用串口2时，W1位必需设置为1，否则可能会产生不可预期的错误
	S2CON = (S2CON & 0x3f) | 0x40; 
	T2L  = TM;
	T2H  = TM>>8;
	AUXR |= 0x14;       //定时器2时钟1T模式,开始计时
    S2REN = 1;          //允许接收
    ES2   = 1;          //允许中断
}

void UartPutc(unsigned char dat)
{
	S2BUF = dat; 
	B_TX2_Busy = 1;
	while(B_TX2_Busy);
}
 
char putchar(char c)
{
	UartPutc(c);
	return c;
}

/**********************************************/
void main(void)
{
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
    
    I2C_init();
    UartInit();
    DMA_Config();
    EA = 1;     //允许总中断

    printf("命令设置:\r\n");
    printf("W 0x12 1234567890 --> 写入操作  十六进制地址  写入内容\r\n");
    printf("R 0x12 10         --> 读出操作  十六进制地址  读出字节内容\r\n");

    while(1)
    {
        delay_ms(1);

        if(RX2_TimeOut > 0)
        {
            if(--RX2_TimeOut == 0)  //超时,则串口接收结束
            {
                if(RX2_Cnt > 0)
                {
                    RX2_Check();    //串口1处理数据
                }
                RX2_Cnt = 0;
            }
        }
    }
} 

//========================================================================
// 函数: void delay_ms(u8 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-3-9
// 备注: 
//========================================================================
void delay_ms(u8 ms)
{
    u16 i;
    do{
        i = MAIN_Fosc / 6000;
        while(--i);
    }while(--ms);
}

/**************** ASCII码转BIN ****************************/
u8 CheckData(u8 dat)
{
    if((dat >= '0') && (dat <= '9'))        return (dat-'0');
    if((dat >= 'A') && (dat <= 'F'))        return (dat-'A'+10);
    return 0xff;
}

/**************** 获取写入地址 ****************************/
u8 GetAddress(void)
{
    u8 address;
    u8  i,j;
    
    address = 0;
    if((RX2_Buffer[2] == '0') && (RX2_Buffer[3] == 'X'))
    {
        for(i=4; i<6; i++)
        {
            j = CheckData(RX2_Buffer[i]);
            if(j >= 0x10)   return 0;  //error
            address = (address << 4) + j;
        }
        return (address);
    }
    return  0; //error
}

/**************** 获取要读出数据的字节数 ****************************/
u8  GetDataLength(void)
{
    u8  i;
    u8  length;
    
    length = 0;
    for(i=7; i<RX2_Cnt; i++)
    {
        if(CheckData(RX2_Buffer[i]) >= 10)  break;
        length = length * 10 + CheckData(RX2_Buffer[i]);
    }
    return (length);
}

/**************** 串口2处理函数 ****************************/

void RX2_Check(void)
{
    u8  i,j;

    F0 = 0;
    if((RX2_Cnt >= 8) && (RX2_Buffer[1] == ' '))   //最短命令为8个字节
    {
        for(i=0; i<6; i++)
        {
            if((RX2_Buffer[i] >= 'a') && (RX2_Buffer[i] <= 'z'))    RX2_Buffer[i] = RX2_Buffer[i] - 'a' + 'A';//小写转大写
        }
        EEPROM_addr = GetAddress();
        if(EEPROM_addr <= 255)
        {
            if((RX2_Buffer[0] == 'W') && (RX2_Cnt >= 8) && (RX2_Buffer[6] == ' '))   //写入N个字节
            {
                j = RX2_Cnt - 7;
                
                for(i=0; i<j; i++)  DmaTxBuffer[i+2] = RX2_Buffer[i+7];
                WriteNbyte(EEPROM_addr, j);     //写N个字节 
                printf("已写入%d字节内容!\r\n",j);
                delay_ms(5);

                ReadNbyte(EEPROM_addr, j);
                printf("读出%d个字节内容如下：\r\n",j);
                for(i=0; i<j; i++)    printf("%c", DmaRxBuffer[i]);
                printf("\r\n");
                
                F0 = 1;
            }
            else if((RX2_Buffer[0] == 'R') && (RX2_Cnt >= 8) && (RX2_Buffer[6] == ' '))   //读出N个字节
            {
                j = GetDataLength();
                if((j > 0) && (j <= EE_BUF_LENGTH))
                {
                    ReadNbyte(EEPROM_addr, j);
                    printf("读出%d个字节内容如下：\r\n",j);
                    for(i=0; i<j; i++)    printf("%c", DmaRxBuffer[i]);
                    printf("\r\n");
                    F0 = 1;
                }
            }
        }
    }
    if(!F0) printf("命令错误!\r\n");
}

//========================================================================
// 函数: void DMA_Config(void)
// 描述: I2C DMA 功能配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2021-5-6
//========================================================================
void DMA_Config(void)
{
	DMA_I2CT_STA = 0x00;
	DMA_I2CT_CFG = 0x80;	//bit7 1:Enable Interrupt
	DMA_I2CT_AMT = 0xff;	//设置传输总字节数(低8位)：n+1
	DMA_I2CT_AMTH = 0x00;	//设置传输总字节数(高8位)：n+1
	DMA_I2CT_TXAH = (u8)((u16)&DmaTxBuffer >> 8);	//I2C发送数据存储地址
	DMA_I2CT_TXAL = (u8)((u16)&DmaTxBuffer);
	DMA_I2CT_CR = 0x80;		//bit7 1:使能 I2CT_DMA, bit6 1:开始 I2CT_DMA

	DMA_I2CR_STA = 0x00;
	DMA_I2CR_CFG = 0x80;	//bit7 1:Enable Interrupt
	DMA_I2CR_AMT = 0xff;	//设置传输总字节数(低8位)：n+1
	DMA_I2CR_AMTH = 0x00;	//设置传输总字节数(高8位)：n+1
	DMA_I2CR_RXAH = (u8)((u16)&DmaRxBuffer >> 8);	//I2C接收数据存储地址
	DMA_I2CR_RXAL = (u8)((u16)&DmaRxBuffer);
	DMA_I2CR_CR = 0x81;		//bit7 1:使能 I2CT_DMA, bit5 1:开始 I2CT_DMA, bit0 1:清除 FIFO

	DMA_I2C_ST1 = 0xff;		//设置需要传输字节数(低8位)：n+1
	DMA_I2C_ST2 = 0x00;		//设置需要传输字节数(高8位)：n+1
}

//========================================================================
// 函数: void UART2_int (void) interrupt UART2_VECTOR
// 描述: UART2中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART2_int (void) interrupt 8
{
    if(S2RI)
    {
        S2RI = 0;    //Clear Rx flag
        RX2_Buffer[RX2_Cnt] = S2BUF;
        if(++RX2_Cnt >= UART2_BUF_LENGTH)   RX2_Cnt = 0;
        RX2_TimeOut = 5;
    }

    if(S2TI)
    {
        S2TI = 0;    //Clear Tx flag
        B_TX2_Busy = 0;
    }
}

/********************** I2C函数 ************************/
void I2C_init(void)
{
    P_SW2 = (P_SW2 & ~(3<<4)) | (1<<4);     //IO口切换. 0: P1.4 P1.5, 1: P2.4 P2.5, 3: P3.3 P3.2
    I2CCFG = 0xe0;              //使能I2C主机模式
    I2CMSST = 0x00;
}

void WriteNbyte(u8 addr, u8 number)  /*  WordAddress,First Data Address,Byte lenth   */
{
    while(I2CMSST & 0x80);    //检查I2C控制器忙碌状态

    DmaTxFlag = 1;
    DmaTxBuffer[0] = SLAW;
    DmaTxBuffer[1] = addr;

    I2CMSST = 0x00;
    I2CMSCR = 0x89;             //set cmd //write_start_combo
    DMA_I2C_CR = 0x01;
    DMA_I2CT_AMT = number+1;	//设置传输总字节数(低8位)：number + 设备地址 + 存储地址
    DMA_I2CT_AMTH = 0x00;		//设置传输总字节数(高8位)：n+1
    DMA_I2C_ST1 = number+1;		//设置需要传输字节数(低8位)：number + 设备地址 + 存储地址
    DMA_I2C_ST2 = 0x00;			//设置需要传输字节数(高8位)：n+1
    DMA_I2CT_CR |= 0x40;		//bit7 1:使能 I2CT_DMA, bit6 1:开始 I2CT_DMA

    while(DmaTxFlag);         //DMA忙检测
    DMA_I2C_CR = 0x00;
}

void ReadNbyte(u8 addr, u8 number)   /*  WordAddress,First Data Address,Byte lenth   */
{
    while(I2CMSST & 0x80);    //检查I2C控制器忙碌状态
    DMA_I2C_CR = 0x00;
    I2CMSST = 0x00;

    //发送起始信号+设备地址+写信号
    I2CTXD = SLAW;
    I2CMSCR = 0x09;
    while ((I2CMSST & 0x40) == 0);
    I2CMSST = 0x00;

    //发送存储器地址
    I2CTXD = addr;
    I2CMSCR = 0x0a;
    while ((I2CMSST & 0x40) == 0);
    I2CMSST = 0x00;
    
    //发送起始信号+设备地址+读信号
    I2CTXD = SLAR;
    I2CMSCR = 0x09;
    while ((I2CMSST & 0x40) == 0);
    I2CMSST = 0x00;

    DmaRxFlag = 1;
   //触发数据读取命令
    I2CMSCR = 0x8b;
    DMA_I2C_CR = 0x01;

    DMA_I2CR_AMT = number-1;	//设置传输总字节数(低8位)：n+1
    DMA_I2CR_AMTH = 0x00;			//设置传输总字节数(高8位)：n+1
    DMA_I2C_ST1 = number-1;		//设置需要传输字节数(低8位)：number + 设备地址 + 存储地址
    DMA_I2C_ST2 = 0x00;				//设置需要传输字节数(高8位)：n+1
    DMA_I2CR_CR |= 0x40;			//bit7 1:使能 I2CT_DMA, bit5 1:开始 I2CT_DMA, bit0 1:清除 FIFO
    while(DmaRxFlag);         //DMA忙检测
    DMA_I2C_CR = 0x00;
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

//========================================================================
// 函数: void I2C_Interrupt (void) interrupt 24
// 描述: I2C 中断函数
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-3-18
// 备注: 
//========================================================================
void I2C_Interrupt() interrupt 24
{
	I2CMSST &= ~0x40;       //I2C指令发送完成状态清除

	if(DMA_I2C_CR & 0x04)   //ACKERR
	{
		DMA_I2C_CR &= ~0x04;  //发数据后收到NAK
	}
}

