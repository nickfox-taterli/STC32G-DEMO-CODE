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

高级PWM定时器 PWM1P/PWM1N,PWM2P/PWM2N,PWM3P/PWM3N,PWM4P/PWM4N 每个通道都可独立实现PWM输出，或者两两互补对称输出.

演示使用PWM1P,PWM1N产生互补的SPWM.

主时钟选择24MHZ, PWM时钟选择1T, PWM周期2400, 死区12个时钟(0.5us).正弦波表用200点. 

输出正弦波频率 = 24000000 / 2400 / 200 = 50 HZ.

本程序仅仅是一个SPWM的演示程序, 用户可以通过上面的计算方法修改PWM周期和正弦波的点数和幅度.

本程序输出频率固定, 如果需要变频, 请用户自己设计变频方案.

本程序从P6.0(PWM1P)输出正相脉冲, 从P6.1(PWM1N)输出反相脉冲(互补).

下载时, 选择时钟 24MHz (用户可自行修改频率).

******************************************/

#include "..\..\comm\STC32G.h"

#include "stdio.h"
#include "intrins.h"
#include "T_SineTable.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL

/****************************** 用户定义宏 ***********************************/
#define PWM1_1      0x00	//P:P1.0  N:P1.1
#define PWM1_2      0x01	//P:P2.0  N:P2.1
#define PWM1_3      0x02	//P:P6.0  N:P6.1

#define PWM2_1      0x00	//P:P1.2/P5.4  N:P1.3
#define PWM2_2      0x04	//P:P2.2  N:P2.3
#define PWM2_3      0x08	//P:P6.2  N:P6.3

#define PWM3_1      0x00	//P:P1.4  N:P1.5
#define PWM3_2      0x10	//P:P2.4  N:P2.5
#define PWM3_3      0x20	//P:P6.4  N:P6.5

#define PWM4_1      0x00	//P:P1.6  N:P1.7
#define PWM4_2      0x40	//P:P2.6  N:P2.7
#define PWM4_3      0x80	//P:P6.6  N:P6.7
#define PWM4_4      0xC0	//P:P3.4  N:P3.3

#define ENO1P       0x01
#define ENO1N       0x02
#define ENO2P       0x04
#define ENO2N       0x08
#define ENO3P       0x10
#define ENO3N       0x20
#define ENO4P       0x40
#define ENO4N       0x80
/*****************************************************************************/

/*************  本地常量声明    **************/


/*************  本地变量声明    **************/
u16 PWM1_Duty;
u8	PWM_Index;	//SPWM查表索引

/*************  本地函数声明    **************/


/********************* 主函数 *************************/
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

    PWM1_Duty = 1220;

    PWMA_CCER1 = 0x00; //写 CCMRx 前必须先清零 CCxE 关闭通道
    PWMA_CCER2 = 0x00;
    PWMA_CCMR1 = 0x68; //通道模式配置
//    PWMA_CCMR2 = 0x68;
//    PWMA_CCMR3 = 0x68;
//    PWMA_CCMR4 = 0x68;
    PWMA_CCER1 = 0x05; //配置通道输出使能和极性
//    PWMA_CCER2 = 0x55;

    PWMA_ARRH = 0x09; //设置周期时间
    PWMA_ARRL = 0x60;

    PWMA_CCR1H = (u8)(PWM1_Duty >> 8); //设置占空比时间
    PWMA_CCR1L = (u8)(PWM1_Duty);

    PWMA_DTR = 0x0C;   //设置死区时间

    PWMA_ENO = 0x00;
    PWMA_ENO |= ENO1P; //使能输出
    PWMA_ENO |= ENO1N; //使能输出
//    PWMA_ENO |= ENO2P; //使能输出
//    PWMA_ENO |= ENO2N; //使能输出
//    PWMA_ENO |= ENO3P; //使能输出
//    PWMA_ENO |= ENO3N; //使能输出
//    PWMA_ENO |= ENO4P; //使能输出
//    PWMA_ENO |= ENO4N; //使能输出

    //高级PWM输出脚会自动设置为推挽输出模式
    PWMA_PS = 0x00;  //高级 PWM 通道输出脚选择位
    PWMA_PS |= PWM1_3; //选择 PWM1_3 通道
//    PWMA_PS |= PWM2_3; //选择 PWM2_3 通道
//    PWMA_PS |= PWM3_3; //选择 PWM3_3 通道
//    PWMA_PS |= PWM4_3; //选择 PWM4_3 通道

    PWMA_BKR = 0x80; //使能主输出
    PWMA_IER = 0x01; //使能中断
    PWMA_CR1 |= 0x81; //使能ARR预装载，开始计时

    EA = 1;     //打开总中断

    while(1)
    {
    }
}


/******************** 中断函数 **************************/
void PWMA_ISR() interrupt PWMA_VECTOR 
{ 
    if(PWMA_SR1 & 0X01)
    {
        PWMA_SR1 &=~0X01; 
        PWM1_Duty = T_SinTable[PWM_Index];
        if(++PWM_Index >= 200)	PWM_Index = 0;

        PWMA_CCR1H = (u8)(PWM1_Duty >> 8); //设置占空比时间
        PWMA_CCR1L = (u8)(PWM1_Duty);
    }
    PWMA_SR1 = 0;
}
