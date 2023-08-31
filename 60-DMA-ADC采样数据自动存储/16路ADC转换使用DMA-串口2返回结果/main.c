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

/*************	功能说明	**************

本例程基于STC32G为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

0~7通道对应P1.0~P1.7, 8~14通道对应P0.0~P0.6, 15通道为内部1.19V基准电压做输入的ADC值.

初始化时先把要ADC转换的引脚设置为高阻输入.

设置数据批量传输(DMA)功能，所有通道一次循环采集的数据自动存放到DMA定义的xdata空间.

通过串口2(P4.6 P4.7)将DMA定义的xdata空间所收到的数据发送给上位机，波特率115200,N,8,1

下载时, 选择时钟 22.1184MHz (用户可自行修改频率).

******************************************/

#include "..\..\comm\STC32G.h"
#include "intrins.h"
#include "stdio.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

/*************	本地常量声明	**************/

#define MAIN_Fosc     22118400L	//定义主时钟
#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))

#define	ADC_SPEED	15			/* 0~15, ADC转换时间(CPU时钟数) = (n+1)*32  ADCCFG */
#define	RES_FMT		(1<<5)	/* ADC结果格式 0: 左对齐, ADC_RES: D11 D10 D9 D8 D7 D6 D5 D4, ADC_RESL: D3 D2 D1 D0 0 0 0 0 */
                          /* ADCCFG     1: 右对齐, ADC_RES: 0 0 0 0 D11 D10 D9 D8, ADC_RESL: D7 D6 D5 D4 D3 D2 D1 D0 */

#define	ADC_CH		16			/* 1~16, ADC转换通道数, 需同步修改 DMA_ADC_CHSW 转换通道 */
#define	ADC_DATA	12			/* 6~n, 每个通道ADC转换数据总数, 2*转换次数+4, 需同步修改 DMA_ADC_CFG2 转换次数 */
#define	DMA_ADDR	0x800		/* DMA数据存放地址 */

/*************	本地变量声明	**************/

bit	DmaFlag;

u8 xdata DmaBuffer[ADC_CH][ADC_DATA] _at_ DMA_ADDR;

/*************	本地函数声明	**************/

u16		Get_ADC12bitResult(u8 channel);
void	delay_ms(u8 ms);
void	ADC_convert(u8 chn);	//chn=0~7对应P1.0~P1.7, chn=8~14对应P0.0~P0.6, chn=15对应BandGap电压
void	DMA_Config(void);

/******************** 串口打印函数 ********************/
void UartInit(void)
{
	S2_S = 1;       //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
    S2CFG |= 0x01;  //使用串口2时，W1位必需设置为1，否则可能会产生不可预期的错误
	S2CON = (S2CON & 0x3f) | 0x40; 
	T2L  = TM;
	T2H  = TM>>8;
	AUXR |= 0x14;	      //定时器2时钟1T模式,开始计时
}

void UartPutc(unsigned char dat)
{
	S2BUF  = dat; 
	while(S2TI == 0);
	S2TI = 0;    //Clear Tx flag
}

char putchar(char c)
{
	UartPutc(c);
	return c;
}

/**********************************************/
void main(void)
{
	u8	i,n;

    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x7f;   P0M0 = 0x00;   //设置要做ADC的IO做高阻输入
    P1M1 = 0xfb;   P1M0 = 0x00;   //设置要做ADC的IO做高阻输入
    P2M1 = 0x3c;   P2M0 = 0x3c;   //设置P2.2~P2.5为漏极开路(实验箱加了上拉电阻到3.3V)
    P3M1 = 0x50;   P3M0 = 0x50;   //设置P3.4、P3.6为漏极开路(实验箱加了上拉电阻到3.3V)
    P4M1 = 0x3c;   P4M0 = 0x3c;   //设置P4.2~P4.5为漏极开路(实验箱加了上拉电阻到3.3V)
    P5M1 = 0x1c;   P5M0 = 0x0c;   //设置P5.2、P5.3为漏极开路(实验箱加了上拉电阻到3.3V)，设置P5.4为高阻输入
    P6M1 = 0xff;   P6M0 = 0xff;   //设置为漏极开路(实验箱加了上拉电阻到3.3V)
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口
	
	ADCTIM = 0x3f;  //设置通道选择时间、保持时间、采样时间
	ADCCFG = RES_FMT + ADC_SPEED;
	//ADC模块电源打开后，需等待1ms，MCU内部ADC电源稳定后再进行AD转换
	ADC_CONTR = 0x80 + 0;	//ADC on + channel

	UartInit();
	DMA_Config();
	EA = 1;
	printf("STC32G系列ADC DMA测试程序!\r\n");

	while (1)
	{
		delay_ms(200);
		if(DmaFlag)
		{
			DmaFlag = 0;
			for(i=0; i<ADC_CH; i++)
			{
				for(n=0; n<ADC_DATA; n++)
				{
					printf("0x%02x ",DmaBuffer[i][n]);
				}
				printf("\r\n");
			}
			printf("\r\n");
			DMA_ADC_CR = 0xc0;		//bit7 1:Enable ADC_DMA, bit6 1:Start ADC_DMA
		}
	}
}

//========================================================================
// 函数: void DMA_Config(void)
// 描述: ADC DMA 功能配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2021-5-6
//========================================================================
void DMA_Config(void)
{
	DMA_ADC_STA = 0x00;
	DMA_ADC_CFG = 0x80;		//bit7 1:Enable Interrupt
	DMA_ADC_RXAH = (u8)(DMA_ADDR >> 8);	//ADC转换数据存储地址
	DMA_ADC_RXAL = (u8)DMA_ADDR;
	DMA_ADC_CFG2 = 0x09;	//每个通道ADC转换次数:4
	DMA_ADC_CHSW0 = 0xff;	//ADC通道使能寄存器 ADC7~ADC0
	DMA_ADC_CHSW1 = 0xff;	//ADC通道使能寄存器 ADC15~ADC8
	DMA_ADC_CR = 0xc0;		//bit7 1:Enable ADC_DMA, bit6 1:Start ADC_DMA
}

//========================================================================
// 函数: void delay_ms(u8 ms)
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
	do
	{
		i = MAIN_Fosc / 6000;
		while(--i);
	}while(--ms);
}

//========================================================================
// 函数: void ADC_DMA_Interrupt (void) interrupt 48
// 描述: ADC DMA中断函数
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-5-8
// 备注: 
//========================================================================
void ADC_DMA_Interrupt(void) interrupt 13
{
	DMA_ADC_STA = 0;
	DmaFlag = 1;
}
