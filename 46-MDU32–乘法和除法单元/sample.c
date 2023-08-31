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

通过汇编代码实现STC32硬件乘除法，替换标准算法库算法

串口2(115200,N,8,1)打印计算结果

可通过屏蔽"STC32_MDU32_V1.x.LIB"文件，用示波器测量IO口低电平时间，来对比STC32硬件乘除法单元与标准算法库的计算效率

测量计算时间时可将串口打印指令屏蔽，方便查看每条公式的计算时间

下载时, 默认时钟 24MHz (用户可自行修改频率).

******************************************/

#include "..\comm\STC32G.h"
#include "intrins.h"
#include "stdio.h"

#define MAIN_Fosc        24000000UL

volatile  unsigned long int near uint1, uint2;
volatile unsigned long int near xuint;

volatile long int sint1, sint2;
volatile long int xsint;

unsigned long ultest;
long ltest;

/*****************************************************************************/

sbit TPIN  =  P5^0;

/*****************************************************************************/

#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart     2        //1:printf 使用 UART1; 2:printf 使用 UART2

/******************** 串口打印函数 ********************/
void UartInit(void)
{
#if(PrintUart == 1)
	SCON = (SCON & 0x3f) | 0x40; 
	T1x12 = 1;          //定时器时钟1T模式
	S1BRT = 0;          //串口1选择定时器1为波特率发生器
	TL1  = TM;
	TH1  = TM>>8;
	TR1 = 1;		//定时器1开始计时

//	SCON = (SCON & 0x3f) | 0x40; 
//	T2L  = TM;
//	T2H  = TM>>8;
//	AUXR |= 0x15;   //串口1选择定时器2为波特率发生器
#else
	S2_S = 1;       //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
    S2CFG |= 0x01;  //使用串口2时，W1位必需设置为1，否则可能会产生不可预期的错误
	S2CON = (S2CON & 0x3f) | 0x40; 
	T2L  = TM;
	T2H  = TM>>8;
	AUXR |= 0x14;	    //定时器2时钟1T模式,开始计时
#endif
}

void UartPutc(unsigned char dat)
{
#if(PrintUart == 1)
	SBUF = dat; 
	while(TI==0);
	TI = 0;
#else
	S2BUF  = dat; 
	while(S2TI == 0);
	S2TI = 0;    //Clear Tx flag
#endif
}

char putchar(char c)
{
	UartPutc(c);
	return c;
}

void delay(unsigned char ms)
{
	 while(--ms);
}

/*****************************************************************************/
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

	UartInit();

	printf("STC32G MDU32 Test.\r\n");
	
	TPIN = 0;  //计算开始同步信号
	delay(2);
	TPIN = 1;
	delay(2);
	TPIN = 0;
	delay(2);
	TPIN = 1;
	delay(2);
	
	ultest = 12345678UL;
	ltest = 12345678;
	ultest = ultest / 12;
	ltest = ltest / 12;

	sint1 = 0x31030F05;
	sint2 = 0x00401350;
	TPIN = 0;
	xsint = sint1 * sint2;
	TPIN = 1;
	printf("Result1=0x%x",xsint>>16);
	printf("%x\r\n",xsint);

	uint1 =  5;
	uint2 =  50;
	TPIN = 0;
	xuint = uint1 * uint2;
	TPIN = 1;
	printf("Result2=%d\r\n",xuint);

	uint1 = 654689;
	uint2 = 528;
	TPIN = 0;
	xuint = uint1 / uint2;
	TPIN = 1;
	printf("Result3=%u\r\n",xuint);

	sint1 = 2134135177;
	sint2 = 20000;
	TPIN = 0;
	xsint = sint1 / sint2;
	TPIN = 1;
	printf("Result4=0x%x",xsint>>16);
	printf("%x\r\n",xsint);

	sint1 = -2134135177;
	sint2 = -20000;
	TPIN = 0;
	xsint = sint1 / sint2;
	TPIN = 1;
	printf("Result5=0x%x",xsint>>16);
	printf("%x\r\n",xsint);

	sint1 = 2134135177;
	sint2 = -20000;
	TPIN = 0;
	xsint = sint1 / sint2;
	TPIN = 1;
	printf("Result6=0x%x",xsint>>16);
	printf("%x\r\n",xsint);

	while(1);
}
/*****************************************************************************/
