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

USART类型Lin从机总线收发测试用例，默认通信脚：P4.6 P4.7.

收到一个非本机应答的完整帧后通过串口1输出, 并保存到数据缓存.

收到一个本机应答的帧头后(例如：ID=0x12), 发送据进行应答.

3秒内没收到新的数据，MCU进入休眠状态，通过LIN总线唤醒.

默认传输速率：9600波特率, 用户可自行修改.

下载时, 默认时钟 24MHz (用户可自行修改频率).

******************************************/

#include "..\comm\STC32G.h"
#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL
#define Baudrate         9600UL
#define TM               (65536 -(MAIN_Fosc/Baudrate/4))
#define Timer0_Reload    (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

/*****************************************************************************/

sbit SLP_N  = P5^0;     //0: Sleep

/*************  本地常量声明    **************/

#define FRAME_LEN           8    //数据长度: 8 字节
#define UART2_BUF_LENGTH    16

/*************  本地变量声明    **************/

u8 StaCR;
u8 Lin_ID;
u8 TX_BUF[8];

bit B_1ms;      //1ms标志
u16 msSecond;

bit B_RX2_Flag; //接收标志
bit B_TX2_Busy; //发送忙标志
u8  TX2_Cnt;    //发送计数
u8  RX2_Cnt;    //接收计数
u8 RX2_TimeOut;

u8  RX2_Buffer[UART2_BUF_LENGTH]; //接收缓冲

/*************  本地函数声明    **************/

void LinInit(void);
void LinStatusRead(void);
void LinSendData(u8 *pdat);
void LINSendChecksum(u8 *dat);

/******************** 串口打印函数 ********************/
void UartInit(void)
{
	P_SW1 = (P_SW1 & 0x3f) | 0x00;    //USART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
	SCON = (SCON & 0x3f) | 0x40; 
	T1x12 = 1;          //定时器时钟1T模式
	S1BRT = 0;          //串口1选择定时器1为波特率发生器
	TL1 = TM;
	TH1 = TM>>8;
	TR1 = 1;			//定时器1开始计时
}

void UartPutc(unsigned char dat)
{
	SBUF = dat; 
	while(TI==0);
	TI = 0;
}

char putchar(char c)
{
	UartPutc(c);
	return c;
}

/********************* 主函数 *************************/
void main(void)
{
	u8 i;
	
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

	AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
	TH0 = (u8)(Timer0_Reload / 256);
	TL0 = (u8)(Timer0_Reload % 256);
	ET0 = 1;    //Timer0 interrupt enable
	TR0 = 1;    //Tiner0 run

	Lin_ID = 0x32;
	UartInit();
	LinInit();
	
	EA = 1;                 //打开总中断

	printf("STC32G USART2 to LIN Test Programme!\r\n");

	SLP_N = 1;
	TX_BUF[0] = 0x81;
	TX_BUF[1] = 0x22;
	TX_BUF[2] = 0x33;
	TX_BUF[3] = 0x44;
	TX_BUF[4] = 0x55;
	TX_BUF[5] = 0x66;
	TX_BUF[6] = 0x77;
	TX_BUF[7] = 0x88;
	
    P40 = 0;  //LED Power on
	while(1)
	{
		if((B_RX2_Flag) && (RX2_Cnt >= 2))
		{
			B_RX2_Flag = 0;

			if((RX2_Buffer[0] == 0x55) && ((RX2_Buffer[1] & 0x3f) == 0x12)) //PID -> ID
			{
				LinSendData(TX_BUF);
				LINSendChecksum(TX_BUF);
			}
		}

		if(B_1ms)   //1ms到
		{
			B_1ms = 0;
            msSecond++;
            if(msSecond >= 3000) //3秒钟没收到数据进休眠
            {
                msSecond = 0;
                P67 = 1;    //LED17 OFF
                PD = 1;     //Sleep
                _nop_();
                _nop_();
                _nop_();
                _nop_();
                _nop_();
                _nop_();
                _nop_();
                P67 = 0;    //LED17 ON
            }

			if(RX2_TimeOut > 0)     //超时计数
			{
				if(--RX2_TimeOut == 0)
				{
                    msSecond = 0;
					printf("Read Cnt = %d.\r\n",RX2_Cnt);
					for(i=0; i<RX2_Cnt; i++)    printf("0x%02x ",RX2_Buffer[i]);    //从串口输出收到的数据
					RX2_Cnt  = 0;   //清除字节数
					printf("\r\n");
				}
			}
		}
	}
}

/********************** Timer0 1ms中断函数 ************************/
void timer0 (void) interrupt 1
{
    B_1ms = 1;      //1ms标志
}

//========================================================================
// 函数: void LinSendByte(u8 dat)
// 描述: 发送一个字节函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-10-28
// 备注: 
//========================================================================
void LinSendByte(u8 dat)
{
	B_TX2_Busy = 1;
	S2BUF = dat;
	while(B_TX2_Busy);
}

//========================================================================
// 函数: u16 LinSendData(u8 *pdat)
// 描述: Lin发送数据函数。
// 参数: *pdat: 设置数据缓冲区.
// 返回: Lin ID.
// 版本: VER1.0
// 日期: 2021-10-28
// 备注: 
//========================================================================
void LinSendData(u8 *pdat)
{
	u8 i;

	for(i=0;i<FRAME_LEN;i++)
	{
		LinSendByte(pdat[i]);
	}
}

//========================================================================
// 函数: void LINSendChecksum(u8 *dat)
// 描述: 计算校验码并发送。
// 参数: 数据场传输的数据.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-12-2
// 备注: 
//========================================================================
void LINSendChecksum(u8 *dat)
{
    u16 sum = 0;
    u8 i;

    for(i = 0; i < FRAME_LEN; i++)
    {
        sum += dat[i];
        if(sum & 0xFF00)
        {
            sum = (sum & 0x00FF) + 1;
        }
    }
    sum ^= 0x00FF;
    LinSendByte((u8)sum);
}

//========================================================================
// 函数: void LinSetBaudrate(u16 brt)
// 描述: Lin总线波特率设置函数。
// 参数: brt: 波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-10-28
// 备注: 
//========================================================================
void LinSetBaudrate(u16 brt)
{
	u16 tmp;
	
	tmp = (MAIN_Fosc >> 4) / brt;
	USART2BRH = (u8)(tmp>>8);
	USART2BRL = (u8)tmp;
}

//========================================================================
// 函数: void LinInit(void)
// 描述: Lin初始化函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinInit(void)
{
	S2_S  = 1;      //USART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7

	S2CON = (S2CON & 0x3f) | 0x50;  //8位数据,可变波特率, 允许接收
	AUXR |= 0x14;   //定时器2时钟1T模式,开始计时
	ES2 = 1;        //允许中断

	LinSetBaudrate(Baudrate);	//设置波特率
	S2CFG |= 0x80;			//S2MOD = 1
	USART2CR1 |= 0x80;		//LIN Mode Enable
	USART2CR5 |= 0x20;		//LIN Slave Mode

	USART2CR5 |= 0x10;		//AutoSync
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

        StaCR = USART2CR5;
        if(StaCR & 0x02)     //if LIN header is detected
        {
            B_RX2_Flag = 1;
        }

        if(StaCR & 0xc0)     //if LIN break is detected / LIN header error is detected
        {
            RX2_Cnt = 0;
        }

        RX2_Buffer[RX2_Cnt++] = S2BUF;
        if(RX2_Cnt >= UART2_BUF_LENGTH)   RX2_Cnt = 0;
        RX2_TimeOut = 20;

        USART2CR5 &= ~0xcb;    //Clear flag
    }

    if(S2TI)
    {
        S2TI = 0;    //Clear Tx flag
        B_TX2_Busy = 0;
    }
}
