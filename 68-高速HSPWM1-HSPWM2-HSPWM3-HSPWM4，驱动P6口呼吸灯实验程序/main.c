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

高速高级PWM定时器 PWM1P/PWM1N,PWM2P/PWM2N,PWM3P/PWM3N,PWM4P/PWM4N 每个通道都可独立实现PWM输出，或者两两互补对称输出.

8个通道PWM设置对应P6的8个端口.

通过P6口上连接的8个LED灯，利用PWM实现呼吸灯效果.

PWM周期和占空比可以根据需要自行设置，最高可达65535.

下载时, 选择时钟 24MHz (用户可自行修改频率).

******************************************/

#include "..\comm\STC32G.h"

#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL

/****************************** 用户定义宏 ***********************************/

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

/*****************************************************************************/

#define	HSCK_MCLK			0
#define	HSCK_PLL			1
#define	HSCK_SEL			HSCK_PLL

#define	PLL_96M				0
#define	PLL_144M			1
#define	PLL_SEL				PLL_144M

#define	CKMS			0x80
#define	HSIOCK			0x40
#define	MCK2SEL_MSK		0x0c
#define	MCK2SEL_SEL1	0x00
#define	MCK2SEL_PLL		0x04
#define	MCK2SEL_PLLD2	0x08
#define	MCK2SEL_IRC48	0x0c
#define	MCKSEL_MSK		0x03
#define	MCKSEL_HIRC		0x00
#define	MCKSEL_XOSC		0x01
#define	MCKSEL_X32K		0x02
#define	MCKSEL_IRC32K	0x03

#define	ENCKM				0x80
#define	PCKI_MSK			0x60
#define	PCKI_D1				0x00
#define	PCKI_D2				0x20
#define	PCKI_D4				0x40
#define	PCKI_D8				0x60

/*************  本地常量声明    **************/

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

#define PWM_PERIOD  1023    //设置周期值

/*************  本地变量声明    **************/

bit B_1ms;          //1ms标志

u16 PWM1_Duty;
u16 PWM2_Duty;
u16 PWM3_Duty;
u16 PWM4_Duty;

bit PWM1_Flag;
bit PWM2_Flag;
bit PWM3_Flag;
bit PWM4_Flag;

void PllConfig(void);
void HSPwmConfig(void);
void UpdatePwm(void);

/********************* 主函数 *************************/
void main(void)
{
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

    PWM1_Flag = 0;
    PWM2_Flag = 0;
    PWM3_Flag = 0;
    PWM4_Flag = 0;
    
    PWM1_Duty = 0;
    PWM2_Duty = 256;
    PWM3_Duty = 512;
    PWM4_Duty = 1024;

    //  Timer0初始化
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    P6SR = 0x00;                //IO口电平转换速度加快

    PWMA_PS = 0x00;  //高级 PWM 通道输出脚选择位
    PWMA_PS |= PWM1_2; //选择 PWM1_2 通道
    PWMA_PS |= PWM2_2; //选择 PWM2_2 通道
    PWMA_PS |= PWM3_2; //选择 PWM3_2 通道
    PWMA_PS |= PWM4_2; //选择 PWM4_2 通道

    PllConfig();
    HSPwmConfig();

    P40 = 0;	//给LED供电
    EA = 1;     //打开总中断

    while (1);
}


/********************** Timer0 1ms中断函数 ************************/
void timer0(void) interrupt 1
{
    if(!PWM1_Flag)
    {
        PWM1_Duty++;
        if(PWM1_Duty > PWM_PERIOD) PWM1_Flag = 1;
    }
    else
    {
        PWM1_Duty--;
        if(PWM1_Duty <= 0) PWM1_Flag = 0;
    }

    if(!PWM2_Flag)
    {
        PWM2_Duty++;
        if(PWM2_Duty > PWM_PERIOD) PWM2_Flag = 1;
    }
    else
    {
        PWM2_Duty--;
        if(PWM2_Duty <= 0) PWM2_Flag = 0;
    }

    if(!PWM3_Flag)
    {
        PWM3_Duty++;
        if(PWM3_Duty > PWM_PERIOD) PWM3_Flag = 1;
    }
    else
    {
        PWM3_Duty--;
        if(PWM3_Duty <= 0) PWM3_Flag = 0;
    }

    if(!PWM4_Flag)
    {
        PWM4_Duty++;
        if(PWM4_Duty > PWM_PERIOD) PWM4_Flag = 1;
    }
    else
    {
        PWM4_Duty--;
        if(PWM4_Duty <= 0) PWM4_Flag = 0;
    }
    
    UpdatePwm();
}

//========================================================================
// 函数: delay(void)
// 描述: 延时函数. 
// 参数: none.
// 返回: none.
// 版本: V1.0, 2022-03-16
//========================================================================
void delay(void)
{
	u8	i;
	for	(i=0; i<100; i++);
}

//========================================================================
// 函数: ReadPWMA(void)
// 描述: 异步读取PWMA特殊功能寄存器函数. 
// 参数: addr: 读取特殊功能寄存器地址.
// 返回: dat:  读取特殊功能寄存器内容.
// 版本: V1.0, 2022-03-16
//========================================================================
//u8 ReadPWMA(u8 addr)
//{
//	u8 dat;
//	
//	while (HSPWMA_ADR &	0x80); //等待前一个异步读写完成
//	HSPWMA_ADR = addr |	0x80;  //设置间接访问地址,只需要设置原XFR地址的低7位
//								//HSPWMA_ADDR寄存器的最高位写1,表示读数据
//	while (HSPWMA_ADR &	0x80); //等待当前异步读取完成
//	dat	= HSPWMA_DAT;		   //读取异步数据
//	
//	return dat;
//}

//========================================================================
// 函数: WritePWMA(void)
// 描述: 异步读取PWMA特殊功能寄存器函数. 
// 参数: addr: 写入特殊功能寄存器地址.
// 参数: dat:  写入特殊功能寄存器内容.
// 返回: none.
// 版本: V1.0, 2022-03-16
//========================================================================
void WritePWMA(u8	addr, u8 dat)
{
	while (HSPWMA_ADR &	0x80); //等待前一个异步读写完成
	HSPWMA_DAT = dat;		   //准备需要写入的数据
	HSPWMA_ADR = addr &	0x7f;  //设置间接访问地址,只需要设置原XFR地址的低7位
								//HSPWMA_ADDR寄存器的最高位写0,表示写数据
}

//========================================================================
// 函数: PllConfig(void)
// 描述: PWM时钟初始化函数. 
// 参数: none.
// 返回: none.
// 版本: V1.0, 2022-03-16
//========================================================================
void PllConfig(void)
{
	//选择PLL输出时钟
#if	(PLL_SEL ==	PLL_96M)
	CLKSEL &= ~CKMS;			//选择PLL的96M作为PLL的输出时钟
#elif (PLL_SEL == PLL_144M)
	CLKSEL |= CKMS;				//选择PLL的144M作为PLL的输出时钟
#else
	CLKSEL &= ~CKMS;			//默认选择PLL的96M作为PLL的输出时钟
#endif
	
	//选择PLL输入时钟分频,保证输入时钟为12M
	USBCLK &= ~PCKI_MSK;
#if	(MAIN_Fosc == 12000000UL)
	USBCLK |= PCKI_D1;			//PLL输入时钟1分频
#elif (MAIN_Fosc	== 24000000UL)
	USBCLK |= PCKI_D2;			//PLL输入时钟2分频
#elif (MAIN_Fosc	== 48000000UL)
	USBCLK |= PCKI_D4;			//PLL输入时钟4分频
#elif (MAIN_Fosc	== 96000000UL)
	USBCLK |= PCKI_D8;			//PLL输入时钟8分频
#else
	USBCLK |= PCKI_D1;			//默认PLL输入时钟1分频
#endif

	//启动PLL
	USBCLK |= ENCKM;			//使能PLL倍频
	
	delay();					//等待PLL锁频

	//选择HSPWM/HSSPI时钟
#if	(HSCK_SEL == HSCK_MCLK)
	CLKSEL &= ~HSIOCK;			//HSPWM/HSSPI选择主时钟为时钟源
#elif (HSCK_SEL	== HSCK_PLL)
	CLKSEL |= HSIOCK;			//HSPWM/HSSPI选择PLL输出时钟为时钟源
#else
	CLKSEL &= ~HSIOCK;			//默认HSPWM/HSSPI选择主时钟为时钟源
#endif

	HSCLKDIV = 0;				//HSPWM/HSSPI时钟源不分频
}

//========================================================================
// 函数: HSPwmConfig(void)
// 描述: PWM初始化函数. 
// 参数: none.
// 返回: none.
// 版本: V1.0, 2022-03-16
//========================================================================
void HSPwmConfig(void)
{
	HSPWMA_CFG = 0x03;			//使能PWMA相关寄存器异步访问功能
	
	//通过异步方式设置PWMA的相关寄存器
	WritePWMA((u8)&PWMA_CCER1, 0x00);
	WritePWMA((u8)&PWMA_CCER2, 0x00);
	WritePWMA((u8)&PWMA_CCMR1, 0x68);			//通道模式配置
	WritePWMA((u8)&PWMA_CCMR2, 0x68);
	WritePWMA((u8)&PWMA_CCMR3, 0x68);
	WritePWMA((u8)&PWMA_CCMR4, 0x68);
	WritePWMA((u8)&PWMA_CCER1, 0x55);			//配置通道输出使能和极性
	WritePWMA((u8)&PWMA_CCER2, 0x55);
	WritePWMA((u8)&PWMA_ENO, ENO1P|ENO1N|ENO2P|ENO2N|ENO3P|ENO3N|ENO4P|ENO4N);	//使能PWM信号输出端口
	WritePWMA((u8)&PWMA_CCR1H, (u8)(PWM1_Duty >> 8));	//设置输出PWM的占空比
	WritePWMA((u8)&PWMA_CCR1L, (u8)PWM1_Duty);
	WritePWMA((u8)&PWMA_CCR2H, (u8)(PWM2_Duty >> 8));	//设置输出PWM的占空比
	WritePWMA((u8)&PWMA_CCR2L, (u8)PWM2_Duty);
	WritePWMA((u8)&PWMA_CCR3H, (u8)(PWM3_Duty >> 8));	//设置输出PWM的占空比
	WritePWMA((u8)&PWMA_CCR3L, (u8)PWM3_Duty);
	WritePWMA((u8)&PWMA_CCR4H, (u8)(PWM4_Duty >> 8));	//设置输出PWM的占空比
	WritePWMA((u8)&PWMA_CCR4L, (u8)PWM4_Duty);
	WritePWMA((u8)&PWMA_ARRH,  (u8)(PWM_PERIOD >> 8));	//设置输出PWM的周期
	WritePWMA((u8)&PWMA_ARRL,  (u8)PWM_PERIOD);
	WritePWMA((u8)&PWMA_DTR, 10);					//设置互补对称输出PWM的死区
	WritePWMA((u8)&PWMA_BKR, 0x80);				//使能主输出
	WritePWMA((u8)&PWMA_CR1, 0x81);				//使能ARR预装载，开始PWM计数
}

//========================================================================
// 函数: UpdatePwm(void)
// 描述: 更新PWM占空比. 
// 参数: none.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void UpdatePwm(void)
{
	WritePWMA((u8)&PWMA_CCR1H, (u8)(PWM1_Duty >> 8));	//设置输出PWM的占空比
	WritePWMA((u8)&PWMA_CCR1L, (u8)PWM1_Duty);
	WritePWMA((u8)&PWMA_CCR2H, (u8)(PWM2_Duty >> 8));	//设置输出PWM的占空比
	WritePWMA((u8)&PWMA_CCR2L, (u8)PWM2_Duty);
	WritePWMA((u8)&PWMA_CCR3H, (u8)(PWM3_Duty >> 8));	//设置输出PWM的占空比
	WritePWMA((u8)&PWMA_CCR3L, (u8)PWM3_Duty);
	WritePWMA((u8)&PWMA_CCR4H, (u8)(PWM4_Duty >> 8));	//设置输出PWM的占空比
	WritePWMA((u8)&PWMA_CCR4L, (u8)PWM4_Duty);
}
