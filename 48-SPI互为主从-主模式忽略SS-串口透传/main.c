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

设置开漏模式需要断开PWM当DAC电路中的R2电阻。

通过串口发送数据给MCU1，MCU1将接收到的数据由SPI发送给MCU2，MCU2再通过串口发送出去.

设置方法 2：
两个设备初始化时都设置 SSIG 为 0，MSTR 设置为0，此时两个设备都是不忽略 SS 的从机模式。
当其中一个设备需要启动传输时，先检测 SS 管脚的电平，如果时候高电平，
就将自己设置成忽略 SS 的主模式，自己的 SS 脚输出低电平，拉低对方的 SS 脚，即可进行数据传输。

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

#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        22118400L   //定义主时钟（精确计算115200波特率）

/****************************** 用户定义宏 ***********************************/
#define Baudrate2           115200L
#define BUF_LENGTH          128

#define UART2_BUF_LENGTH    (BUF_LENGTH)   //串口缓冲长度
#define SPI_BUF_LENGTH      (BUF_LENGTH)   //SPI缓冲长度
/*****************************************************************************/

/*************  本地常量声明    **************/
sbit    SPI_SS  = P2^2;
sbit    SPI_SI  = P2^3;
sbit    SPI_SO  = P2^4;
sbit    SPI_SCK = P2^5;

/*************  本地变量声明    **************/
u8  RX2_TimeOut;
u8  SPI_TimeOut;
u8  SPI_Cnt;    //SPI计数
u8  RX2_Cnt;    //UART计数
bit B_TX2_Busy; //发送忙标志
bit B_SPI_Busy; //发送忙标志
bit B_SPI_Send; //发送标志

u8  RX2_Buffer[UART2_BUF_LENGTH];   //接收缓冲
u8  SPI_Buffer[SPI_BUF_LENGTH];     //接收缓冲

/*************  本地函数声明    **************/
void    delay_ms(u8 ms);
void    UART2_config(u8 brt);   // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
void    PrintString2(u8 *puts);
void    UART2_TxByte(u8 dat);

void    SPI_init(void);
void    SPI_WriteByte(u8 out);

/********************* 主函数 *************************/
void main(void)
{
    u8 i;

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

    UART2_config(2);    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
    SPI_init();
    EA = 1;     //允许总中断
    
    PrintString2("UART2与SPI透传程序-SPI互为主从设置方法2.\r\n");

    while (1)
    {
        delay_ms(1);

        if(RX2_TimeOut > 0)
        {
            if(--RX2_TimeOut == 0)  //超时,则串口接收结束
            {
                if(RX2_Cnt > 0)
                {
                    B_SPI_Send = 1;
                }
            }
        }
        if((B_SPI_Send) && (SPI_SS))
        {
            B_SPI_Send = 0;
            SPI_SS = 0;     //拉低从机 SS 管脚
            SPCTL = 0xd4;   //使能 SPI 主机模式，忽略SS引脚功能
            for(i=0;i<RX2_Cnt;i++)
            {
                SPI_WriteByte(RX2_Buffer[i]); //发送串口数据
            }
            SPI_SS = 1;    //拉高从机的 SS 管脚
            SPCTL = 0x44;  //重新设置为从机待机
            RX2_Cnt = 0;
        }
				
        if(SPI_TimeOut > 0)
        {
            if(--SPI_TimeOut == 0)  //超时,则SPI接收结束
            {
                if(SPI_Cnt > 0)
                {
                    for(i=0;i<SPI_Cnt;i++)
                    {
                        UART2_TxByte(SPI_Buffer[i]); //发送SPI数据
                    }
                    SPI_Cnt = 0;
                }
            }
        }
    }
}

//========================================================================
// 函数: void  delay_ms(unsigned char ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
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

//========================================================================
// 函数: void UART2_TxByte(u8 dat)
// 描述: 发送一个字节.
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2014-6-30
//========================================================================
void UART2_TxByte(u8 dat)
{
    S2BUF = dat;
    B_TX2_Busy = 1;
    while(B_TX2_Busy);
}

//========================================================================
// 函数: void PrintString2(u8 *puts)
// 描述: 串口2发送字符串函数。
// 参数: puts:  字符串指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void PrintString2(u8 *puts) //发送一个字符串
{
    for (; *puts != 0;  puts++) UART2_TxByte(*puts);    //遇到停止符0结束
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
void SetTimer2Baudraye(u16 dat)  // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
{
    T2R = 0;	//Timer stop
    T2_CT = 0;	//Timer2 set As Timer
    T2x12 = 1;	//Timer2 set as 1T mode
    T2H = (u8)(dat / 256);
    T2L = (u8)(dat % 256);
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
    if(brt == 2)
    {
        SetTimer2Baudraye((u16)(65536UL - (MAIN_Fosc / 4) / Baudrate2));

        S2CFG |= 0x01;     //使用串口2时，W1位必需设置为1，否则可能会产生不可预期的错误
        S2CON = (S2CON & 0x3f) | 0x40;    //UART2模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
        ES2   = 1;         //允许中断
        S2REN = 1;         //允许接收
        S2_S  = 1;         //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7

        RX2_TimeOut = 0;
        B_TX2_Busy = 0;
        RX2_Cnt = 0;
    }
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

/************************************************************************/

void SPI_Isr() interrupt 9 
{ 
    SPSTAT = 0xc0; //清中断标志
    if (MSTR) 
    { //主机模式
        B_SPI_Busy = 0;
    }
    else 
    { //从机模式
        if(SPI_Cnt >= SPI_BUF_LENGTH) SPI_Cnt = 0;
        SPI_Buffer[SPI_Cnt] = SPDAT;
        SPI_Cnt++;
        SPI_TimeOut = 10;
    }
}

/************************************************************************/
void SPI_WriteByte(u8 out)
{
    SPDAT = out;
    B_SPI_Busy = 1;
    while(B_SPI_Busy);
}

/************************************************************************/
void SPI_init(void)
{
    SPI_SS = 1;
    SPCTL = 0x44;         //使能 SPI 从机模式进行待机
    P_SW1 = (P_SW1 & ~(3<<2)) | (1<<2);  //IO口切换. 0: P1.2/P5.4 P1.3 P1.4 P1.5, 1: P2.2 P2.3 P2.4 P2.5, 2: P5.4 P4.0 P4.1 P4.3, 3: P3.5 P3.4 P3.3 P3.2
    SPIF = 1;               //写1清除 SPIF标志
    WCOL = 1;               //写1清除 WCOL标志
    ESPI = 1;               //使能 SPI 中断

    HSCLKDIV = 0x06;        //高速时钟6分频，默认2分频，采用漏极开路+上拉电阻方式驱动，需要调低SPI频率信号才能正常
    
    SPI_TimeOut = 0;
    B_SPI_Busy = 0;
    B_SPI_Send = 0;
    SPI_Cnt = 0;
}

