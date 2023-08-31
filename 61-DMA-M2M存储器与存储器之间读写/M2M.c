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

设置2个存储器空间，一个发送一个接收，分别初始化2个存储器空间内容。

设置M2M DMA，上电后自动将发送存储里的内容写入到接收存储器空间.

根据不同的读取顺序、写入顺序，接收到不同的数据结果.

通过串口2(P4.6 P4.7)打印接收存储器数据(上电打印一次).

下载时, 选择时钟 22.1184MHz (用户可自行修改频率).

******************************************/

#include "..\comm\STC32G.h"
#include "intrins.h"
#include "stdio.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc     22118400L   //定义主时钟（精确计算115200波特率）
#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))

bit	DmaFlag;

u8 xdata DmaTxBuffer[256];
u8 xdata DmaRxBuffer[256];

void DMA_Config(void);

/******************** 串口打印函数 ********************/
void UartInit(void)
{
	S2_S = 1;       //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
    S2CFG |= 0x01;  //使用串口2时，W1位必需设置为1，否则可能会产生不可预期的错误
	S2CON = (S2CON & 0x3f) | 0x40; 
	T2L  = TM;
	T2H  = TM>>8;
	AUXR |= 0x14;   //定时器2时钟1T模式,开始计时
}

void UartPutc(unsigned char dat)
{
	S2BUF  = dat; 
	while(S2TI == 0);
	S2TI = 0;    //Clear Tx flag
}

char putchar(char c)
{
	UartPutc(c);
	return c;
}

//========================================================================
// 函数: void main(void)
// 描述: 主函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void main(void)
{
	u16 i;
	
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

	for(i=0; i<256; i++)
	{
		DmaTxBuffer[i] = i;
		DmaRxBuffer[i] = 0;
	}

	UartInit();
	printf("STC32G Memory to Memory DMA Test Programme!\r\n");  //UART1发送一个字符串

	DMA_Config();
	EA = 1; //允许总中断

	DmaFlag = 0;

	while (1)
	{
		if(DmaFlag)
		{
			DmaFlag = 0;

			for(i=0; i<256; i++)
			{
				printf("%02X ", DmaRxBuffer[i]);
				if((i & 0x0f) == 0x0f)
					printf("\r\n");
			}
		}
	}
}

//========================================================================
// 函数: void DMA_Config(void)
// 描述: UART DMA 功能配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2021-5-6
//========================================================================
void DMA_Config(void)
{
	DMA_M2M_CFG = 0x80;     //r++ = t++
	DMA_M2M_STA = 0x00;
	DMA_M2M_AMT = 0x7f;		//设置传输总字节数：n+1
	DMA_M2M_TXAH = (u8)((u16)&DmaTxBuffer >> 8);
	DMA_M2M_TXAL = (u8)((u16)&DmaTxBuffer);
	DMA_M2M_RXAH = (u8)((u16)&DmaRxBuffer >> 8);
	DMA_M2M_RXAL = (u8)((u16)&DmaRxBuffer);

//	DMA_M2M_CFG = 0xa0;     //r++ = t--
//	DMA_M2M_STA = 0x00;
//	DMA_M2M_AMT = 0x7f;		//设置传输总字节数：n+1
//	DMA_M2M_TXAH = (u8)((u16)&DmaTxBuffer[255] >> 8);
//	DMA_M2M_TXAL = (u8)((u16)&DmaTxBuffer[255]);
//	DMA_M2M_RXAH = (u8)((u16)&DmaRxBuffer >> 8);
//	DMA_M2M_RXAL = (u8)((u16)&DmaRxBuffer);

//	DMA_M2M_CFG = 0x90;     //r-- = t++
//	DMA_M2M_STA = 0x00;
//	DMA_M2M_AMT = 0x7f;		//设置传输总字节数：n+1
//	DMA_M2M_TXAH = (u8)((u16)&DmaTxBuffer >> 8);
//	DMA_M2M_TXAL = (u8)((u16)&DmaTxBuffer);
//	DMA_M2M_RXAH = (u8)((u16)&DmaRxBuffer[255] >> 8);
//	DMA_M2M_RXAL = (u8)((u16)&DmaRxBuffer[255]);

//	DMA_M2M_CFG = 0xb0;     //r-- = t--
//	DMA_M2M_STA = 0x00;
//	DMA_M2M_AMT = 0x7f;		//设置传输总字节数：n+1
//	DMA_M2M_TXAH = (u8)((u16)&DmaTxBuffer[255] >> 8);
//	DMA_M2M_TXAL = (u8)((u16)&DmaTxBuffer[255]);
//	DMA_M2M_RXAH = (u8)((u16)&DmaRxBuffer[255] >> 8);
//	DMA_M2M_RXAL = (u8)((u16)&DmaRxBuffer[255]);
	
//	DMA_M2M_CFG |= 0x0c;    //设置中断优先级
	DMA_M2M_CR = 0xc0;		//bit7 1:使能 M2M_DMA, bit6 1:开始 M2M_DMA 自动接收
}

//========================================================================
// 函数: void M2M_DMA_Interrupt (void) interrupt 47
// 描述: UART1 DMA中断函数
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-5-8
// 备注: 
//========================================================================
void M2M_DMA_Interrupt(void) interrupt 13
{
	if (DMA_M2M_STA & 0x01)	//发送完成
	{
		DMA_M2M_STA &= ~0x01;
		DmaFlag = 1;
	}
}
