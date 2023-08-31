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

/*************  功能说明    **************

本例程基于STC32G为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

UART_DMA, M2M_DMA, SPI_DMA 综合使用演示例程.

通过串口发送数据给MCU1，MCU1将接收到的数据由SPI发送给MCU2，MCU2再通过串口发送出去.

通过串口发送数据给MCU2，MCU2将接收到的数据由SPI发送给MCU1，MCU1再通过串口发送出去.

MCU1/MCU2: UART接收 -> UART Rx DMA -> M2M -> SPI Tx DMA -> SPI发送

MCU2/MCU1: SPI接收 -> SPI Rx DMA -> M2M -> UART Tx DMA -> UART发送

         MCU1                          MCU2
  |-----------------|           |-----------------|
  |            MISO |-----------| MISO            |
--| TX         MOSI |-----------| MOSI         TX |--
  |            SCLK |-----------| SCLK            |
--| RX           SS |-----------| SS           RX |--
  |-----------------|           |-----------------|


下载时, 选择时钟 22.1184MHz (用户可自行修改频率).

******************************************/

#include "..\comm\STC32G.h"
#include "intrins.h"
#include "stdio.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define     MAIN_Fosc       22118400L   //定义主时钟（精确计算115200波特率）

/*************  本地常量声明    **************/

sbit    SPI_SS  = P2^2;
sbit    SPI_SI  = P2^3;
sbit    SPI_SO  = P2^4;
sbit    SPI_SCK = P2^5;

#define Baudrate2           115200L
#define BUF_LENGTH          107			//n+1

/*************  本地变量声明    **************/

bit	SpiTxFlag;
bit	SpiRxFlag;
bit	UartTxFlag;
bit	UartRxFlag;

bit	u2sFlag;    //UART to SPI
bit	s2uFlag;    //SPI to UART

bit	SpiSendFlag;
bit	UartSendFlag;

u8 xdata UartTxBuffer[256];
u8 xdata UartRxBuffer[256];
u8 xdata SpiTxBuffer[256];
u8 xdata SpiRxBuffer[256];

void UART2_config(u8 brt);   // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
void SPI_init(void);

void SPI_DMA_Config(void);
void SPI_DMA_Master(void);
void SPI_DMA_Slave(void);
void UART_DMA_Config(void);
void UART_DMA_Tx(void);
void UART_DMA_Rx(void);
void M2M_DMA_Config(void);
void M2M_UART_SPI(void);
void M2M_SPI_UART(void);

void UartPutc(unsigned char dat)
{
	S2BUF  = dat; 
	while(S2TI == 0);
	S2TI = 0;
}
 
char putchar(char c)
{
	UartPutc(c);
	return c;
}

/******************** 主函数 **************************/
void main(void)
{
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x30;   P0M0 = 0x30;   //设置P0.4、P0.5为漏极开路(实验箱加了上拉电阻到3.3V)
    P1M1 = 0x32;   P1M0 = 0x32;   //设置P1.1、P1.4、P1.5为漏极开路(实验箱加了上拉电阻到3.3V), P1.1在PWM当DAC电路通过电阻串联到P2.3
    P2M1 = 0x3c;   P2M0 = 0x3c;   //设置P2.2~P2.5为漏极开路(实验箱加了上拉电阻到3.3V)
    P3M1 = 0x50;   P3M0 = 0x50;   //设置P3.4、P3.6为漏极开路(实验箱加了上拉电阻到3.3V)
    P4M1 = 0x3c;   P4M0 = 0x3c;   //设置P4.2~P4.5为漏极开路(实验箱加了上拉电阻到3.3V)
    P5M1 = 0x0c;   P5M0 = 0x0c;   //设置P5.2、P5.3为漏极开路(实验箱加了上拉电阻到3.3V)
    P6M1 = 0xff;   P6M0 = 0xff;   //设置为漏极开路(实验箱加了上拉电阻到3.3V)
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

	UART2_config(2);    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
	printf("STC32G UART2-DMA-SPI互为主从透传程序.\r\n");

	SPI_init();
	SPI_DMA_Config();
	UART_DMA_Config();
	M2M_DMA_Config();
	EA = 1;     //允许总中断
	
	while (1)
	{
		//UART接收 -> UART DMA -> SPI DMA -> SPI发送
		if(UartRxFlag)
		{
			UartRxFlag = 0;
			u2sFlag = 1;
			M2M_UART_SPI();			//UART Memory -> SPI Memory
		}

		if(SpiSendFlag)
		{
			SpiSendFlag = 0;
			UART_DMA_Rx();			//UART Recive Continue
			SPI_DMA_Master();		//SPI Send Memory
		}

		if(SpiTxFlag)
		{
			SpiTxFlag = 0;
			SPI_DMA_Slave();		//SPI Slave Mode
		}

		
		//SPI接收 -> SPI DMA -> UART DMA -> UART发送
		if(SpiRxFlag)
		{
			SpiRxFlag = 0;
			s2uFlag = 1;
			M2M_SPI_UART();			//SPI Memory -> UART Memory
		}

		if(UartSendFlag)
		{
			UartSendFlag = 0;
			SPI_DMA_Slave();		//SPI Slave Mode
			UART_DMA_Tx();			//UART Send Memory
		}
	}
}

//========================================================================
// 函数: void SPI_DMA_Config(void)
// 描述: SPI DMA 功能配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2021-5-6
//========================================================================
void SPI_DMA_Config(void)
{
	DMA_SPI_STA = 0x00;
	DMA_SPI_CFG = 0xe0;		//bit7 1:Enable Interrupt
	DMA_SPI_AMT = BUF_LENGTH;		//设置传输总字节数(低8位)：n+1
	DMA_SPI_AMTH = 0x00;		//设置传输总字节数(高8位)：n+1
	DMA_SPI_TXAH = (u8)((u16)&SpiTxBuffer >> 8);	//SPI发送数据存储地址
	DMA_SPI_TXAL = (u8)((u16)&SpiTxBuffer);
	DMA_SPI_RXAH = (u8)((u16)&SpiRxBuffer >> 8);	//SPI接收数据存储地址
	DMA_SPI_RXAL = (u8)((u16)&SpiRxBuffer);
	DMA_SPI_CFG2 = 0x01;	//01:SS-P2.2
	DMA_SPI_CR = 0xa1;		//bit7 1:使能 SPI_DMA, bit6 1:开始 SPI_DMA 主机模式,  bit5 1:开始 SPI_DMA 从机模式, bit0 1:清除 SPI_DMA FIFO
}

void SPI_DMA_Master(void)
{
	DMA_SPI_CR = 0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	SPI_SS = 0;
	SPCTL = 0xd2;   //使能 SPI 主机模式，忽略SS引脚功能
	DMA_SPI_CR = 0xc0;		//bit7 1:使能 SPI_DMA, bit6 1:开始 SPI_DMA 主机模式,  bit5 1:开始 SPI_DMA 从机模式, bit0 1:清除 SPI_DMA FIFO
}

void SPI_DMA_Slave(void)
{
	DMA_SPI_CR = 0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	SPCTL = 0x42;  //重新设置为从机待机
	DMA_SPI_CR = 0xa0;		//bit7 1:使能 SPI_DMA, bit6 1:开始 SPI_DMA 主机模式,  bit5 1:开始 SPI_DMA 从机模式, bit0 1:清除 SPI_DMA FIFO
}

//========================================================================
// 函数: void UART_DMA_Config(void)
// 描述: UART DMA 功能配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2021-5-6
//========================================================================
void UART_DMA_Config(void)
{
	DMA_UR2T_CFG = 0x80;		//bit7 1:Enable Interrupt
	DMA_UR2T_STA = 0x00;
	DMA_UR2T_AMT = BUF_LENGTH;	//设置传输总字节数(低8位)：n+1
	DMA_UR2T_AMTH = 0x00;		//设置传输总字节数(高8位)：n+1
	DMA_UR2T_TXAH = (u8)((u16)&UartTxBuffer >> 8);
	DMA_UR2T_TXAL = (u8)((u16)&UartTxBuffer);
	DMA_UR2T_CR = 0x80;			//bit7 1:使能 UART2_DMA, bit6 1:开始 UART2_DMA 自动发送

	DMA_UR2R_CFG = 0x80;		//bit7 1:Enable Interrupt
	DMA_UR2R_STA = 0x00;
	DMA_UR2R_AMT = BUF_LENGTH;	//设置传输总字节数(低8位)：n+1
	DMA_UR2R_AMTH = 0x00;		//设置传输总字节数(高8位)：n+1
	DMA_UR2R_RXAH = (u8)((u16)&UartRxBuffer >> 8);
	DMA_UR2R_RXAL = (u8)((u16)&UartRxBuffer);
	DMA_UR2R_CR = 0xa1;			//bit7 1:使能 UART2_DMA, bit5 1:开始 UART2_DMA 自动接收, bit0 1:清除 FIFO
}

void UART_DMA_Tx(void)
{
	DMA_UR2T_CR = 0xc0;			//bit7 1:使能 UART2_DMA, bit6 1:开始 UART2_DMA 自动发送
}

void UART_DMA_Rx(void)
{
	DMA_UR2R_CR = 0xa0;			//bit7 1:使能 UART2_DMA, bit5 1:开始 UART2_DMA 自动接收, bit0 1:清除 FIFO
}

//========================================================================
// 函数: void M2M_DMA_Config(void)
// 描述: M2M DMA 功能配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2021-5-6
//========================================================================
void M2M_DMA_Config(void)
{
	DMA_M2M_CFG = 0x80;   //r++ = t++
	DMA_M2M_STA = 0x00;
	DMA_M2M_AMT = BUF_LENGTH;		//设置传输总字节数(低8位)：n+1
	DMA_M2M_AMTH = 0x00;		//设置传输总字节数(高8位)：n+1
	DMA_M2M_TXAH = (u8)((u16)&UartRxBuffer >> 8);
	DMA_M2M_TXAL = (u8)((u16)&UartRxBuffer);
	DMA_M2M_RXAH = (u8)((u16)&SpiTxBuffer >> 8);
	DMA_M2M_RXAL = (u8)((u16)&SpiTxBuffer);
	DMA_M2M_CR = 0x80;			//bit7 1:使能 M2M_DMA, bit6 1:开始 M2M_DMA 自动接收
}

void M2M_UART_SPI(void)
{
	DMA_M2M_STA = 0x00;
	DMA_M2M_TXAH = (u8)((u16)&UartRxBuffer >> 8);
	DMA_M2M_TXAL = (u8)((u16)&UartRxBuffer);
	DMA_M2M_RXAH = (u8)((u16)&SpiTxBuffer >> 8);
	DMA_M2M_RXAL = (u8)((u16)&SpiTxBuffer);
	DMA_M2M_CR = 0xc0;			//bit7 1:使能 M2M_DMA, bit6 1:开始 M2M_DMA 自动接收
}

void M2M_SPI_UART(void)
{
	DMA_M2M_STA = 0x00;
	DMA_M2M_TXAH = (u8)((u16)&SpiRxBuffer >> 8);
	DMA_M2M_TXAL = (u8)((u16)&SpiRxBuffer);
	DMA_M2M_RXAH = (u8)((u16)&UartTxBuffer >> 8);
	DMA_M2M_RXAL = (u8)((u16)&UartTxBuffer);
	DMA_M2M_CR = 0xc0;			//bit7 1:使能 M2M_DMA, bit6 1:开始 M2M_DMA 自动接收
}

//========================================================================
// 函数: SetTimer2Baudraye(u16 dat)
// 描述: 设置Timer2做波特率发生器。
// 参数: dat: Timer2的重装值.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void SetTimer2Baudraye(u16 dat)  // 使用Timer2做波特率.
{
    T2R = 0;	//Timer stop
    T2_CT = 0;	//Timer2 set As Timer
    T2x12 = 1;	//Timer2 set as 1T mode
    T2H = dat / 256;
    T2L = dat % 256;
    ET2 = 0;    //禁止中断
    T2R = 1;	//Timer run enable
}

//========================================================================
// 函数: void UART2_config(u8 brt)
// 描述: UART2初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART2_config(u8 brt)    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
{
    /*********** 波特率固定使用定时器2 *****************/
    if(brt == 2)
    {
        SetTimer2Baudraye((u16)(65536UL - (MAIN_Fosc / 4) / Baudrate2));

        S2CFG |= 0x01;     //使用串口2时，W1位必需设置为1，否则可能会产生不可预期的错误
        S2CON = (S2CON & 0x3f) | 0x40; 
//        ES2   = 1;         //允许中断
        S2REN = 1;         //允许接收
        S2_S  = 1;         //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
    }
}

//========================================================================
// 函数: void SPI_init(void)
// 描述: SPI
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void SPI_init(void)
{
	SPI_SS = 1;
	SPCTL = 0x42;         //使能 SPI 从机模式进行待机
	P_SW1 = (P_SW1 & ~(3<<2)) | (1<<2);  //IO口切换. 0: P1.2/P5.4 P1.3 P1.4 P1.5, 1: P2.2 P2.3 P2.4 P2.5, 2: P5.4 P4.0 P4.1 P4.3, 3: P3.5 P3.4 P3.3 P3.2
}

//========================================================================
// 函数: void DMA_Interrupt (void) interrupt 13
// 描述: DMA中断函数
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-5-8
// 备注: 
//========================================================================
void DMA_Interrupt(void) interrupt 13
{
	if(DMA_SPI_STA & 0x01)	//通信完成
	{
		DMA_SPI_STA &= ~0x01;
		if(SPCTL & 0x10) 
		{ //主机模式
			SpiTxFlag = 1;
			SPI_SS = 1;
		}
		else 
		{ //从机模式
			SpiRxFlag = 1;
		}
	}
	if(DMA_SPI_STA & 0x02)	//数据丢弃
	{
		DMA_SPI_STA &= ~0x02;
	}
	if(DMA_SPI_STA & 0x04)	//数据覆盖
	{
		DMA_SPI_STA &= ~0x04;
	}

	if(DMA_UR2T_STA & 0x01)	//发送完成
	{
		DMA_UR2T_STA &= ~0x01;
		UartTxFlag = 1;
	}
	if(DMA_UR2T_STA & 0x04)	//数据覆盖
	{
		DMA_UR2T_STA &= ~0x04;
	}
	
	if(DMA_UR2R_STA & 0x01)	//接收完成
	{
		DMA_UR2R_STA &= ~0x01;
		UartRxFlag = 1;
	}
	if(DMA_UR2R_STA & 0x02)	//数据丢弃
	{
		DMA_UR2R_STA &= ~0x02;
	}

	if(DMA_M2M_STA & 0x01)	//发送完成
	{
		DMA_M2M_STA &= ~0x01;
		if(u2sFlag)
		{
			u2sFlag = 0;
			SpiSendFlag = 1;
		}
		if(s2uFlag)
		{
			s2uFlag = 0;
			UartSendFlag = 1;
		}
	}
}
