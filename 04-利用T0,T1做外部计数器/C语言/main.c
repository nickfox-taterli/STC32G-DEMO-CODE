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

本例程基于STC32G为主控芯片的实验箱进行编写测试.

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

用STC的MCU的IO方式驱动8位数码管。

显示效果为: 左边为T0(SW21)外部中断计数, 右边为T1(SW22)外部中断计数, 计数范围为0~255.

由于按键是机械按键, 按下有抖动, 而本例程没有去抖动处理, 所以按一次有多个计数也是正常的.

下载时, 选择时钟 24MHZ (用户可自行修改频率).

******************************************/

#include "..\..\comm\STC32G.h"

#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL

//==========================================================================

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

#define DIS_DOT     0x20
#define DIS_BLACK   0x10
#define DIS_        0x11

/*************  本地常量声明    **************/
u8 code t_display[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

u8 code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //位码

/*************  IO口定义    **************/

/*************  本地变量声明    **************/

u8  LED8[8];        //显示缓冲
u8  display_index;  //显示位索引

u8  T0_cnt, T1_cnt; //测试用的计数变量


/*************  本地函数声明    **************/

void delay_ms(u8 ms);
void DisplayScan(void);


/****************  外部函数声明和外部变量声明 *****************/


//========================================================================
// 函数: void timer0_int (void) interrupt TIMER0_VECTOR
// 描述:  timer0中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void TM0_Isr() interrupt 1
{
    P47 = !P47;  //测试端口
    T0_cnt++;    //计数+1
}

//========================================================================
// 函数: void timer1_int (void) interrupt TIMER1_VECTOR
// 描述:  timer1中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void TM1_Isr (void) interrupt 3
{
    P46 = !P46;  //测试端口
    T1_cnt++;    //计数+1
}

/******************** 主函数 **************************/
void main(void)
{
    u8  i;
    
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x30;   P0M0 = 0x30;   //设置P0.4、P0.5为漏极开路(实验箱加了上拉电阻到3.3V)
    P1M1 = 0x30;   P1M0 = 0x30;   //设置P1.4、P1.5为漏极开路(实验箱加了上拉电阻到3.3V)
    P2M1 = 0x3c;   P2M0 = 0x3c;   //设置P2.2~P2.5为漏极开路(实验箱加了上拉电阻到3.3V)
    P3M1 = 0x70;   P3M0 = 0x40;   //设置P3.6为漏极开路(实验箱加了上拉电阻到3.3V), P3.4,P3.5设置为输入口
    P4M1 = 0x3c;   P4M0 = 0x3c;   //设置P4.2~P4.5为漏极开路(实验箱加了上拉电阻到3.3V)
    P5M1 = 0x0c;   P5M0 = 0x0c;   //设置P5.2、P5.3为漏极开路(实验箱加了上拉电阻到3.3V)
    P6M1 = 0xff;   P6M0 = 0xff;   //设置为漏极开路(实验箱加了上拉电阻到3.3V)
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

    P3PU = 0x20;            //P3.5使能内部4.1K上拉电阻

    TMOD = 0;
    T0_CT = 1;              //使能T0外部计数模式
    T1_CT = 1;              //使能T1外部计数模式
    TL0 = 0xff;
    TH0 = 0xff;
    TL1 = 0xff;
    TH1 = 0xff;
    TR0 = 1;                //启动定时器T0
    TR1 = 1;                //启动定时器T1
    ET0 = 1;                //使能定时器中断T0
    ET1 = 1;                //使能定时器中断T1

    T0CLKO = 0;             //T0不输出时钟
    T1CLKO = 0;             //T1不输出时钟

    display_index = 0;
    for(i=0; i<8; i++)  LED8[i] = DIS_BLACK;    //全部消隐
    T0_cnt = 0;
    T1_cnt = 0;

    EA = 1;

    while(1)
    {
        delay_ms(1);    //延时1ms
        DisplayScan();
    }
}

//========================================================================
// 函数: void delay_ms(unsigned char ms)
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

/********************** 显示扫描函数 ************************/
void DisplayScan(void)
{   
		P7 = ~T_COM[7-display_index];
		P6 = ~t_display[LED8[display_index]];
    if(++display_index >= 8)
    {
        display_index = 0;  //8位结束回0
        LED8[0] = T0_cnt / 100;
        LED8[1] = (T0_cnt % 100)/10;
        LED8[2] = T0_cnt % 10;
        LED8[3] = DIS_BLACK;
        LED8[4] = DIS_BLACK;
        LED8[5] = T1_cnt / 100;
        LED8[6] = (T1_cnt % 100)/10;
        LED8[7] = T1_cnt % 10;
    }
}
