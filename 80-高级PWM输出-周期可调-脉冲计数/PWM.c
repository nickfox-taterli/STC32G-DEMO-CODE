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

高级PWM定时器实现高速PWM脉冲输出.

周期/占空比可调, 通过比较/捕获中断进行脉冲个数计数.

通过P6口演示输出,每隔10ms输出一次PWM,计数10个脉冲后停止输出.

使用单脉冲模式配合重复计数寄存器，纯硬件控制脉冲个数.

定时器每1ms调整PWM周期.

下载时, 选择时钟 24MHZ (用户可自行修改频率).

******************************************/

#include "stc32g.h"
#include "intrins.h"

#define MAIN_Fosc			24000000L	//定义主时钟

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;    //1:printf 使用 UART1; 2:printf 使用 UART2

/****************************** 用户定义宏 ***********************************/

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

#define PWM1_0      0x00	//P:P1.0  N:P1.1
#define PWM1_1      0x01	//P:P2.0  N:P2.1
#define PWM1_2      0x02	//P:P6.0  N:P6.1

#define PWM2_0      0x00	//P:P1.2/P5.4  N:P1.3
#define PWM2_1      0x04	//P:P2.2  N:P2.3
#define PWM2_2      0x08	//P:P6.2  N:P6.3

#define PWM3_0      0x00	//P:P1.4  N:P1.5
#define PWM3_1      0x10	//P:P2.4  N:P2.5
#define PWM3_2      0x20	//P:P6.4  N:P6.5

#define PWM4_0      0x00	//P:P1.6  N:P1.7
#define PWM4_1      0x40	//P:P2.6  N:P2.7
#define PWM4_2      0x80	//P:P6.6  N:P6.7
#define PWM4_3      0xC0	//P:P3.4  N:P3.3

#define ENO1P       0x01
#define ENO1N       0x02
#define ENO2P       0x04
#define ENO2N       0x08
#define ENO3P       0x10
#define ENO3N       0x20
#define ENO4P       0x40
#define ENO4N       0x80

/*****************************************************************************/

/*************  本地变量声明    **************/
bit B_1ms;          //1ms标志
bit PWM1_Flag;

u16 Period;
u8 msSecond;

void UpdatePwm(void);
void TxPulse(u8 rep);

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

    PWM1_Flag = 0;
    Period = 0x1000;

    //  Timer0初始化
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    PWMA_ENO = 0x00;
    PWMA_ENO |= ENO1P; //使能输出

    PWMA_CCER1 = 0x00; //写 CCMRx 前必须先清零 CCxE 关闭通道
    PWMA_CCMR1 = 0x68; //设置 PWM1 模式1 输出
//    PWMA_CCER1 = 0x01; //使能 CC1E 通道, 高电平有效
    PWMA_CCER1 = 0x03; //使能 CC1E 通道, 低电平有效

    PWMA_PS = 0x00;    //高级 PWM 通道输出脚选择位
    PWMA_PS |= PWM1_2; //选择 PWM1_2 通道

    UpdatePwm();
    PWMA_BKR = 0x80;   //使能主输出
//    PWMA_CR1 |= 0x89;  //使能ARR预装载，单脉冲模式，开始计时

    P40 = 0;    //给LED供电
    EA = 1;     //打开总中断

    while (1)
    {
        if(B_1ms)
        {
            B_1ms = 0;
            msSecond++;
            if(msSecond >= 10)  //10ms启动一次PWM输出
            {
                msSecond = 0;
                TxPulse(10);    //输出10个脉冲
            }
        }
    }
}

/************* 发送脉冲函数 **************/
void TxPulse(u8 rep)
{
    if(rep == 0) return;
    rep -= 1;

    PWMA_RCR = rep;    //重复计数寄存器，计数 rep 个脉冲后产生更新事件
    PWMA_CR1 |= 0x89;  //使能ARR预装载，单脉冲模式，开始计时
}

/********************** Timer0 1ms中断函数 ************************/
void timer0(void) interrupt 1
{
    B_1ms = 1;
    if(PWM1_Flag)
    {
        Period++;  //周期递增
        if(Period >= 0x1000) PWM1_Flag = 0;
    }
    else
    {
        Period--;  //周期递减
        if(Period <= 0x0100) PWM1_Flag = 1;
    }
    UpdatePwm();	//设置周期、占空比
}

/******************* PWM中断函数 ********************/
//void PWMA_ISR() interrupt 26
//{
//    if(PWMA_SR1 & 0X01)
//    {
//        PWMA_SR1 &=~0X01;    //清标志位
//    }
//}

//========================================================================
// 函数: UpdatePwm(void)
// 描述: 更新PWM周期占空比.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void UpdatePwm(void)
{
    PWMA_ARRH = (u8)(Period >> 8); //设置周期时间
    PWMA_ARRL = (u8)Period;
    PWMA_CCR1H = (u8)((Period >> 1) >> 8); //设置占空比时间: Period/2
    PWMA_CCR1L = (u8)((Period >> 1));
}
