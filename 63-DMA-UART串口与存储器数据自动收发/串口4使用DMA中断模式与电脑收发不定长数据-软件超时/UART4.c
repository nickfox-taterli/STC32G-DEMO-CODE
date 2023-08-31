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

串口4全双工中断方式收发通讯程序。

通过PC向MCU发送数据, MCU将收到的数据自动存入DMA空间.

当DMA空间存满设置大小的内容后，通过串口4的DMA自动发送功能把存储空间的数据原样返回.

利用串口接收中断进行超时判断，超时没有收到新的数据，表示一串数据已经接收完毕，将收到的数据原样返回.

用定时器做波特率发生器，建议使用1T模式(除非低波特率用12T)，并选择可被波特率整除的时钟频率，以提高精度。

下载时, 选择时钟 22.1184MHz (用户可自行修改频率).

******************************************/

#include "..\..\comm\STC32G.h"

#include "stdio.h"
#include "intrins.h"
#include <string.h>

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc   22118400L   //定义主时钟（精确计算115200波特率）
#define Baudrate4   115200L

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

bit B_1ms;          //1ms标志
bit Tx4SendFlag;
bit	DmaTx4Flag;
bit	DmaRx4Flag;
u8  RX4_TimeOut;
u16 RX4_Cnt;    //接收计数
u8 xdata DmaBuffer[256];

u8 xdata TemBuffer[256];

void UART4_config(u8 brt);   // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
void DMA_Config(void);
void UART4_DMA_Transmit(u8 *pData, u16 Size);
void UART4_DMA_Receive(u8 *pData, u16 Size);

void UartPutc(unsigned char dat)
{
    Tx4SendFlag = 1;
    S4BUF = dat; 
    while(Tx4SendFlag); 
//	while(S4TI == 0);
//	S4TI = 0;
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
		DmaBuffer[i] = i;
	}

	AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
	TH0 = (u8)(Timer0_Reload / 256);
	TL0 = (u8)(Timer0_Reload % 256);
	ET0 = 1;    //Timer0 interrupt enable
	TR0 = 1;    //Tiner0 run
	
	UART4_config(0);    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.

	DMA_Config();
	EA = 1; //允许全局中断

	DmaTx4Flag = 0;
	DmaRx4Flag = 0;
	printf("STC32G UART4 Test Programme!\r\n");  //UART4发送一个字符串
	
	while (1)
	{
		if((DmaTx4Flag) && (DmaRx4Flag))
		{
			DmaTx4Flag = 0;
			DMA_UR4T_CR = 0xc0;			//bit7 1:使能 UART4_DMA, bit6 1:开始 UART4_DMA 自动发送
			DmaRx4Flag = 0;
			DMA_UR4R_CR = 0xa1;			//bit7 1:使能 UART4_DMA, bit5 1:开始 UART4_DMA 自动接收, bit0 1:清除 FIFO
		}

        if(B_1ms)   //1ms到
        {
            B_1ms = 0;
            if(RX4_TimeOut > 0)     //超时计数
            {
                if(--RX4_TimeOut == 0)
                {
                    //关闭接收DMA，下次接收的数据重新存放在起始地址位置，否则下次接收数据继续往后面存放。
                    DMA_UR4R_CR = 0x00;			//bit7 1:使能 UART1_DMA, bit5 1:开始 UART1_DMA 自动接收, bit0 1:清除 FIFO

                    printf("\r\nUART4 Timeout!\r\n");  //UART4发送一个字符串
                    memcpy(TemBuffer,DmaBuffer,RX4_Cnt);
                    UART4_DMA_Transmit(TemBuffer,RX4_Cnt);  //设置DMA发送缓冲区，数据长度，并启动发送
                    RX4_Cnt = 0;
                    DmaTx4Flag = 0;
                    DmaRx4Flag = 0;
                    UART4_DMA_Receive(DmaBuffer,256);  //设置DMA接收缓冲区，数据长度，并启动接收
                }
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
	DMA_UR4T_CFG = 0x80;		//bit7 1:Enable Interrupt
	DMA_UR4T_STA = 0x00;
	DMA_UR4T_AMT = 0xff;		//设置传输总字节数(低8位)：n+1
	DMA_UR4T_AMTH = 0x00;		//设置传输总字节数(高8位)：n+1
	DMA_UR4T_TXAH = (u8)((u16)&DmaBuffer >> 8);
	DMA_UR4T_TXAL = (u8)((u16)&DmaBuffer);
	DMA_UR4T_CR = 0xc0;			//bit7 1:使能 UART4_DMA, bit6 1:开始 UART4_DMA 自动发送

	DMA_UR4R_CFG = 0x80;		//bit7 1:Enable Interrupt
	DMA_UR4R_STA = 0x00;
	DMA_UR4R_AMT = 0xff;		//设置传输总字节数(低8位)：n+1
	DMA_UR4R_AMTH = 0x00;		//设置传输总字节数(高8位)：n+1
	DMA_UR4R_RXAH = (u8)((u16)&DmaBuffer >> 8);
	DMA_UR4R_RXAL = (u8)((u16)&DmaBuffer);
	DMA_UR4R_CR = 0xa1;			//bit7 1:使能 UART4_DMA, bit5 1:开始 UART4_DMA 自动接收, bit0 1:清除 FIFO
}

void UART4_DMA_Transmit(u8 *pData, u16 Size)
{
    if(Size == 0) return;
    Size -= 1;
	DMA_UR4T_AMTH = (u8)(Size >> 8);    //设置传输总字节数(低8位)：n+1
	DMA_UR4T_AMT = (u8)Size;            //设置传输总字节数(高8位)：n+1
	DMA_UR4T_TXAH = (u8)((u16)pData >> 8);
	DMA_UR4T_TXAL = (u8)((u16)pData);
    DMA_UR4T_CR = 0xc0;     //bit7 1:使能 UART4_DMA, bit6 1:开始 UART1_DMA 自动发送
}

void UART4_DMA_Receive(u8 *pData, u16 Size)
{
    if(Size == 0) return;
    Size -= 1;
	DMA_UR4R_AMTH = (u8)(Size >> 8);    //设置传输总字节数(低8位)：n+1
	DMA_UR4R_AMT = (u8)Size;            //设置传输总字节数(高8位)：n+1
	DMA_UR4R_RXAH = (u8)((u16)pData >> 8);
	DMA_UR4R_RXAL = (u8)((u16)pData);
    DMA_UR4R_CR = 0xa1;     //bit7 1:使能 UART4_DMA, bit5 1:开始 UART4_DMA 自动接收, bit0 1:清除 FIFO
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
// 函数: void UART4_config(u8 brt)
// 描述: UART4初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART4_config(u8 brt)    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
{
	if(brt == 2)
	{
		SetTimer2Baudraye((u16)(65536UL - (MAIN_Fosc / 4) / Baudrate4));
		S4CON = 0x10;       //8位数据, 使用Timer2做波特率发生器, 允许接收
	}
	else
	{
        T4R = 0;	//Timer stop
		S4CON = 0x50;       //8位数据, 使用Timer4做波特率发生器, 允许接收
		T4H = (65536UL - (MAIN_Fosc / 4) / Baudrate4) / 256;
		T4L = (65536UL - (MAIN_Fosc / 4) / Baudrate4) % 256;
        T4_CT = 0;	//Timer3 set As Timer
        T4x12 = 1;	//Timer3 set as 1T mode
        T4R = 1;	//Timer run enable
	}
    ES4  = 1;       //允许UART4中断
    S4_S = 1;       //UART4 switch bit2 to: 0: P0.2 P0.3, 1: P5.2 P5.3

    RX4_Cnt = 0;
    RX4_TimeOut = 0;
}

//========================================================================
// 函数: void timer0 (void) interrupt 1
// 描述: 定时器0中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void timer0 (void) interrupt 1
{
    B_1ms = 1;      //1ms标志
}

//========================================================================
// 函数: void UART4_int (void) interrupt UART4_VECTOR
// 描述: UART4中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART4_int (void) interrupt 18
{
    if(S4RI)
    {
        S4RI = 0;    //Clear Rx flag
        RX4_Cnt++;
        RX4_TimeOut = 5;
    }

    if(S4TI)
    {
        S4TI = 0;    //Clear Tx flag
        Tx4SendFlag = 0;
    }
}

//========================================================================
// 函数: void UART4_DMA_Interrupt (void) interrupt 56/57
// 描述: UART4 DMA中断函数
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-5-8
// 备注: 
//========================================================================
void UART4_DMA_Interrupt(void) interrupt 13
{
	if (DMA_UR4T_STA & 0x01)	//发送完成
	{
		DMA_UR4T_STA &= ~0x01;
		DmaTx4Flag = 1;
	}
	if (DMA_UR4T_STA & 0x04)	//数据覆盖
	{
		DMA_UR4T_STA &= ~0x04;
	}
	
	if (DMA_UR4R_STA & 0x01)	//接收完成
	{
		DMA_UR4R_STA &= ~0x01;
		DmaRx4Flag = 1;
	}
	if (DMA_UR4R_STA & 0x02)	//数据丢弃
	{
		DMA_UR4R_STA &= ~0x02;
	}
}
