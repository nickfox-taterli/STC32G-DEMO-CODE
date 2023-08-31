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

程序使用P6口做跑马灯，演示系统时钟源切换效果。

跑马灯每跑一轮切换一次时钟源，分别是默认IRC主频，主频12分频，PLL 96M 16分频，内部32K IRC。

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

	
/*****************************************************************************/

/*************  本地常量声明    **************/

#define CKMS            0x80
#define HSIOCK          0x40
#define MCK2SEL_MSK     0x0c
#define MCK2SEL_SEL1    0x00
#define MCK2SEL_PLL     0x04
#define MCK2SEL_PLLD2   0x08
#define MCK2SEL_IRC48   0x0c
#define MCKSEL_MSK      0x03
#define MCKSEL_HIRC     0x00
#define MCKSEL_XOSC     0x01
#define MCKSEL_X32K     0x02
#define MCKSEL_IRC32K   0x03

#define ENCKM           0x80
#define PCKI_MSK        0x60
#define PCKI_D1         0x00
#define PCKI_D2         0x20
#define PCKI_D4         0x40
#define PCKI_D8         0x60

/*************  本地变量声明    **************/
u8 ledIndex;
u8 code ledNum[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
u8 mode = 1;

/*************  本地函数声明    **************/
void  delay_ms(u8 ms);
void  MCLK_Sel(void);

/********************* 主函数 *************************/
void main(void)
{
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x30;   P0M0 = 0x30;   //设置P0.4、P0.5为漏极开路(实验箱加了上拉电阻到3.3V)
    P1M1 = 0x3a;   P1M0 = 0x32;   //设置P1.1、P1.4、P1.5为漏极开路(实验箱加了上拉电阻到3.3V), P1.1在PWM当DAC电路通过电阻串联到P2.3,  P1.3 为 ADC 高阻输入
    P2M1 = 0x3c;   P2M0 = 0x3c;   //设置P2.2~P2.5为漏极开路(实验箱加了上拉电阻到3.3V)
    P3M1 = 0x50;   P3M0 = 0x50;   //设置P3.4、P3.6为漏极开路(实验箱加了上拉电阻到3.3V)
    P4M1 = 0x3c;   P4M0 = 0x3c;   //设置P4.2~P4.5为漏极开路(实验箱加了上拉电阻到3.3V)
    P5M1 = 0x0c;   P5M0 = 0x0c;   //设置P5.2、P5.3为漏极开路(实验箱加了上拉电阻到3.3V)
    P6M1 = 0xff;   P6M0 = 0xff;   //设置为漏极开路(实验箱加了上拉电阻到3.3V)
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

    P40 = 0;		//LED Power On
    ledIndex = 0;

    while(1)
    {
        P6 = ~ledNum[ledIndex];	//输出低驱动
        ledIndex++;
        if(ledIndex > 7)
        {
            ledIndex = 0;
            MCLK_Sel();
        }
        delay_ms(10);
    }
}

//========================================================================
// 函数: void MCLK_Sel(void)
// 描述: 系统时钟设置函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-7-29
// 备注: 
//========================================================================
void MCLK_Sel(void)
{
    if(mode == 0)
    {
        mode++;
        HIRCCR = 0x80; //启动内部 IRC
        while (!(HIRCCR & 1)); //等待时钟稳定
        CLKDIV = 0;
        CLKSEL = 0x00; //选择内部 IRC ( 默认 )
    }
    else if(mode == 1)
    {
        mode++;
        HIRCCR = 0x80; //启动内部 IRC
        while (!(HIRCCR & 1)); //等待时钟稳定
        CLKDIV = 24;   //MCLK/24
        CLKSEL = 0x00; //选择内部 IRC ( 默认 )
    }
    else if(mode == 2)
    {
        mode++;
        CLKSEL &= ~CKMS;            //选择PLL的96M作为PLL的输出时钟
        USBCLK |= PCKI_D2;          //输入时钟2分频(选择PLL输入时钟分频,保证输入时钟为12M)
        //启动PLL
        USBCLK |= ENCKM;            //使能PLL倍频
        delay_ms(1);                //等待PLL锁频
        CLKDIV = 16;                //主时钟选择高速频率前,必须先设置分频系数,否则程序会当掉

        CLKSEL &= ~MCKSEL_MSK;
        CLKSEL &= ~MCK2SEL_MSK;
        CLKSEL |= MCKSEL_HIRC;      //选择内部高速IRC作为主时钟
        CLKSEL |= MCK2SEL_PLLD2;    //选择PLL输出时钟2分频后的时钟作为主时钟
    }
    else if(mode == 3)
    {
        mode = 0;
        IRC32KCR = 0x80; //启动内部 32K IRC
        while (!(IRC32KCR & 1)); //等待时钟稳定
        CLKDIV = 0x00; //时钟不分频
        CLKSEL = 0x03; //选择内部 32K
    }
//    else
//    {
//        mode = 0;
//        XOSCCR = 0xc0; //启动外部晶振
//        while (!(XOSCCR & 1)); //等待时钟稳定
//        CLKDIV = 0x00; //时钟不分频
//        CLKSEL = 0x01; //选择外部晶振
//    }
}

//========================================================================
// 函数: void  delay_ms(u8 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注: 
//========================================================================
void  delay_ms(u8 ms)
{
    u16 i;
    do{
        i = MAIN_Fosc / 6000;
        while(--i);
    }while(--ms);
}

