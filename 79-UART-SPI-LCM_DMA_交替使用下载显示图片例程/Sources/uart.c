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

#include "STC32G.h"
#include "system.h"
#include "stdio.h"
#include "uart.h"
#include "spi_flash.h"

/*************  本地常量声明    **************/

#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart     1        //1:printf 使用 UART1; 2:printf 使用 UART2

/*************  本地变量声明    **************/

u8  RX1_TimeOut;

bit	UartDmaTxFlag;
bit	UartDmaRxFlag;

/******************** 串口打印函数 ********************/

//void UartInit(void)
//{
//#if(PrintUart == 1)
//	SCON = (SCON & 0x3f) | 0x40; 
//	AUXR |= 0x40;		//定时器时钟1T模式
//	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
//	TL1  = TM;
//	TH1  = TM>>8;
//	TR1 = 1;				//定时器1开始计时

////	SCON = (SCON & 0x3f) | 0x40; 
////	T2L  = TM;
////	T2H  = TM>>8;
////	AUXR |= 0x15;   //串口1选择定时器2为波特率发生器
//#else
//	S2_S = 1;       //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
//    S2CFG |= 0x01;  //使用串口2时，W1位必需设置为1，否则可能会产生不可预期的错误
//	S2CON = (S2CON & 0x3f) | 0x40; 
//	T2L  = TM;
//	T2H  = TM>>8;
//	AUXR |= 0x14;	      //定时器2时钟1T模式,开始计时
//#endif
//}

void UartPutc(unsigned char dat)
{
#if(PrintUart == 1)
	SBUF = dat; 
	while(TI==0);
	TI = 0;
#else
	S2BUF  = dat; 
	while((S2CON & 2) == 0);
	S2CON &= ~2;    //Clear Tx flag
#endif
}

char putchar(char c)
{
	UartPutc(c);
	return c;
}

//========================================================================
// 函数: void UART1_DMA_Config(void)
// 描述: UART1 DMA 功能配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2021-5-6
//========================================================================
void UART1_DMA_Config(void)
{
//	DMA_UR1T_CR = 0x00;			//bit7 1:使能 UART1_DMA, bit6 1:开始 UART1_DMA 自动发送
//	DMA_UR1T_CFG = 0x80;		//bit7 1:Enable Interrupt
//	DMA_UR1T_STA = 0x00;
//	DMA_UR1T_AMT = (u8)(DMA_WR_LEN-1);         //设置传输总字节数(低8位)：n+1
//	DMA_UR1T_AMTH = (u8)((DMA_WR_LEN-1) >> 8); //设置传输总字节数(高8位)：n+1
//	DMA_UR1T_TXAH = (u8)((u16)&DmaBuffer1 >> 8);
//	DMA_UR1T_TXAL = (u8)((u16)&DmaBuffer1);
//	DMA_UR1T_CR = 0x80;			//bit7 1:使能 UART1_DMA, bit6 1:开始 UART1_DMA 自动发送

    //关闭接收DMA，下次接收的数据重新存放在起始地址位置，否则下次接收数据继续往后面存放。
    DMA_UR1R_CR = 0x00;			//bit7 1:使能 UART1_DMA, bit5 1:开始 UART1_DMA 自动接收, bit0 1:清除 FIFO
	DMA_UR1R_CFG = 0x80;		//bit7 1:Enable Interrupt
	DMA_UR1R_STA = 0x00;
	DMA_UR1R_AMT = (u8)(DMA_WR_LEN-1);         //设置传输总字节数(低8位)：n+1
	DMA_UR1R_AMTH = (u8)((DMA_WR_LEN-1) >> 8); //设置传输总字节数(高8位)：n+1
	DMA_UR1R_RXAH = (u8)((u16)&DmaBuffer1 >> 8);
	DMA_UR1R_RXAL = (u8)((u16)&DmaBuffer1);
	DMA_UR1R_CR = 0xa1;			//bit7 1:使能 UART1_DMA, bit5 1:开始 UART1_DMA 自动接收, bit0 1:清除 FIFO
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
void SetTimer2Baudraye(u16 dat)  // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
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
// 函数: void UART1_config(u8 brt)
// 描述: UART1初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART1_config(u8 brt)    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
{
	/*********** 波特率使用定时器2 *****************/
	if(brt == 2)
	{
        S1BRT = 1;	//S1 BRT Use Timer2;
        SetTimer2Baudraye((u16)(65536UL - (MAIN_Fosc / 4) / Baudrate));
	}

	/*********** 波特率使用定时器1 *****************/
	else
	{
        TR1 = 0;
        S1BRT = 0;		//S1 BRT Use Timer1;
        T1_CT = 0;		//Timer1 set As Timer
        T1x12 = 1;		//Timer1 set as 1T mode
        TMOD &= ~0x30;  //Timer1_16bitAutoReload;
        TH1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate) / 256);
        TL1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate) % 256);
        ET1 = 0;    //禁止中断
        TR1  = 1;
	}
	/*************************************************/

	SCON = (SCON & 0x3f) | 0x40;    //UART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
//  PS  = 1;    //高优先级中断
	ES  = 1;    //允许中断
	REN = 1;    //允许接收
	P_SW1 &= 0x3f;
	P_SW1 |= 0x00;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4

	RX1_TimeOut = 0;
}


//========================================================================
// 函数: void UART1_int (void) interrupt UART1_VECTOR
// 描述: UART1中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART1_int (void) interrupt 4
{
	if(RI)
	{
		RI = 0;
		RX1_TimeOut = 50;    //如果50ms没收到新的数据，判定一串数据接收完毕
	}

//	if(TI)
//	{
//		TI = 0;
//	}
}

//========================================================================
// 函数: void UART1_DMA_Interrupt (void) interrupt 50/51
// 描述: UART1 DMA中断函数
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-5-8
// 备注: 
//========================================================================
void UART1T_DMA_Interrupt(void) interrupt 50
{
	if (DMA_UR1T_STA & 0x01)	//发送完成
	{
		DMA_UR1T_STA &= ~0x01;
		UartDmaTxFlag = 1;
	}
	if (DMA_UR1T_STA & 0x04)	//数据覆盖
	{
		DMA_UR1T_STA &= ~0x04;
	}
}

void UART1R_DMA_Interrupt(void) interrupt 51
{
	if (DMA_UR1R_STA & 0x01)	//接收完成
	{
		DMA_UR1R_STA &= ~0x01;
		UartDmaRxFlag = 1;
	}
	if (DMA_UR1R_STA & 0x02)	//数据丢弃
	{
		DMA_UR1R_STA &= ~0x02;
	}
}