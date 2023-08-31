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

Lin从机总线收发测试用例，默认LIN脚位选择，P0.2,P0.3.

收到一个非本机应答的完整帧后通过串口2输出, 并保存到数据缓存.

收到一个本机应答的帧头后, 发送缓存数据进行应答.

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

/*****************************************************************************/

sbit SLP_N  = P5^3;     //0: Sleep

/*************  本地常量声明    **************/

#define LIN_MODE      1    //0: LIN2.1;  1: LIN1.3
#define FRAME_LEN     8    //数据长度: 8 字节

#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart     2        //1:printf 使用 UART1; 2:printf 使用 UART2

/*************  本地变量声明    **************/

u8 Lin_ID;
u8 TX_BUF[8];
u16 msSecond;

bit RxFlag;

/*************  本地函数声明    **************/
void LinInit();
void LinReadMsg(u8 *pdat);
void ResponseRxCmd(void);
u8 WaitLinReady(void);
u8 GetLinError(void);
u8 LinReadReg(u8 addr);
void LinTxResponse(u8 *pdat);
void delay_ms(u8 ms);

/******************** 串口打印函数 ********************/
void UartInit(void)
{
#if(PrintUart == 1)
	SCON = (SCON & 0x3f) | 0x40; 
	T1x12 = 1;          //定时器时钟1T模式
	S1BRT = 0;          //串口1选择定时器1为波特率发生器
	TL1  = TM;
	TH1  = TM>>8;
	TR1 = 1;			//定时器1开始计时

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
	while(TI == 0);
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
/********************* 主函数 *************************/
void main(void)
{
	u8 isr;
	
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

	P0PU = 0x0c;          //LIN_TX, LIN_RX 脚开启内部上拉

	Lin_ID = 0x32;
	LinInit();
	UartInit();
	
	EA = 1;                 //打开总中断

	SLP_N = 1;
	TX_BUF[0] = 0x81;
	TX_BUF[1] = 0x22;
	TX_BUF[2] = 0x33;
	TX_BUF[3] = 0x44;
	TX_BUF[4] = 0x55;
	TX_BUF[5] = 0x66;
	TX_BUF[6] = 0x77;
	TX_BUF[7] = 0x88;
	
	UartPutc(TX_BUF[0]);
	UartPutc(TX_BUF[1]);
	UartPutc(TX_BUF[2]);
	UartPutc(TX_BUF[3]);
	UartPutc(TX_BUF[4]);
	UartPutc(TX_BUF[5]);
	UartPutc(TX_BUF[6]);
	UartPutc(TX_BUF[7]);

	while(1)
	{
        delay_ms(1);    //延时1ms
        msSecond++;
        if(msSecond >= 3000) //3秒钟没收到数据进休眠
        {
            msSecond = 0;
            P46 = 1;    //LED9 OFF
            PD = 1;     //Sleep
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            P46 = 0;    //LED9 ON
        }
        
		if(RxFlag == 1)
		{
            msSecond = 0;
			RxFlag = 0;
			isr = LinReadReg(LID);
			if(isr == 0x12)		//判断是否从机响应ID
			{
				LinTxResponse(TX_BUF);	//返回响应数据
			}
			else
			{
				ResponseRxCmd();		//RX response
				WaitLinReady();			//等待ready状态
				GetLinError();			//读取清除错误寄存器

				LinReadMsg(TX_BUF);		//接收Lin总线数据
			}
		}
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

//========================================================================
// 函数: u8 ReadReg(u8 addr)
// 描述: Lin功能寄存器读取函数。
// 参数: Lin功能寄存器地址.
// 返回: Lin功能寄存器数据.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
u8 LinReadReg(u8 addr)
{
	u8 dat;
	LINAR = addr;
	dat = LINDR;
	return dat;
}

//========================================================================
// 函数: void WriteReg(u8 addr, u8 dat)
// 描述: Lin功能寄存器配置函数。
// 参数: Lin功能寄存器地址, Lin功能寄存器数据.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinWriteReg(u8 addr, u8 dat)
{
	LINAR = addr;
	LINDR = dat;
}

//========================================================================
// 函数: void LinReadMsg(u8 *pdat)
// 描述: Lin发送数据函数。
// 参数: *pdat: 接收数据缓冲区.
// 返回: Lin ID.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinReadMsg(u8 *pdat)
{
	u8 i;

	LinWriteReg(LSEL,0x80);	//地址自增，从0开始

	for(i=0;i<FRAME_LEN;i++)
	{
		pdat[i] = LinReadReg(LBUF);
		UartPutc(pdat[i]);		//从串口输出收到的数据
	}
   printf("\r\n");
}

//========================================================================
// 函数: u16 LinSetMsg(u8 *pdat)
// 描述: Lin设置数据函数。
// 参数: *pdat: 设置数据缓冲区.
// 返回: Lin ID.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinSetMsg(u8 *pdat)
{
	u8 i;

	LinWriteReg(LSEL,0x80);		//地址自增，从0开始
	for(i=0;i<FRAME_LEN;i++)
	{
		LinWriteReg(LBUF,pdat[i]);
	}
}

//========================================================================
// 函数: void LinSetID(u8 lid)
// 描述: 设置LIN ID函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinSetID(u8 lid)
{
	LinWriteReg(LID,lid);			//设置总线ID
}

//========================================================================
// 函数: u8 GetLinError(void)
// 描述: 获取LIN总线错误寄存器状态。
// 参数: none.
// 返回: 错误寄存器状态.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
u8 GetLinError(void)
{
	u8 sta;
	sta = LinReadReg(LER);		//读取清除错误寄存器
	return sta;
}

//========================================================================
// 函数: u8 WaitLinReady(void)
// 描述: 等待LIN总线就绪。
// 参数: none.
// 返回: LIN总线状态.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
u8 WaitLinReady(void)
{
	u8 lsr;
	do{
		lsr = LinReadReg(LSR);
	}while(!(lsr & 0x02));		//判断ready状态
	return lsr;
}

//========================================================================
// 函数: void SendAbortCmd(void)
// 描述: 主模式发送Lin总线Abort函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void SendAbortCmd(void)
{
	LinWriteReg(LCR,0x80);		//主模式 Send Abort
}

//========================================================================
// 函数: void SendHeadCmd(void)
// 描述: 主模式发送Lin总线Header函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void SendHeadCmd(void)
{
	LinWriteReg(LCR,0x81);		//主模式 Send Header
}

//========================================================================
// 函数: void SendDatCmd(void)
// 描述: 主模式发送Lin总线数据函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void SendDatCmd(void)
{
	u8 lcr_val;
	lcr_val = 0x82+(LIN_MODE<<6)+(FRAME_LEN<<2);
	LinWriteReg(LCR,lcr_val);
}

//========================================================================
// 函数: void ResponseTxCmd(void)
// 描述: 从模式发送Lin总线Tx Response函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void ResponseTxCmd(void)
{
	u8 lcr_val;
	lcr_val = 0x02+(LIN_MODE<<6)+(FRAME_LEN<<2);
	LinWriteReg(LCR,lcr_val);
}

//========================================================================
// 函数: void ResponseRxCmd(void)
// 描述: 从模式发送Lin总线Rx Response函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void ResponseRxCmd(void)
{
	u8 lcr_val;
	lcr_val = 0x03+(LIN_MODE<<6)+(FRAME_LEN<<2);
	LinWriteReg(LCR,lcr_val);
}

//========================================================================
// 函数: void LinTxResponse(u8 *pdat)
// 描述: Lin从机发送应答数据，跟主机发送的Header拼成一个完整的帧。
// 参数: *pdat: 发送数据缓冲区.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinTxResponse(u8 *pdat)
{
	LinSetMsg(pdat);
	ResponseTxCmd();					//TX response
	WaitLinReady();						//等待ready状态
	GetLinError();						//读取清除错误寄存器
}

//========================================================================
// 函数: void LinSetBaudrate(u16 brt)
// 描述: Lin总线波特率设置函数。
// 参数: brt: 波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinSetBaudrate(u16 brt)
{
	u16 tmp;
	tmp = (MAIN_Fosc >> 4) / brt;
	LinWriteReg(DLH,(u8)(tmp>>8));
	LinWriteReg(DLL,(u8)tmp);
}

//========================================================================
// 函数: void LinSetHeadDelay(u8 base_ms, u8 prescaler)
// 描述: Lin总线设置帧头延时函数。
// 参数: base_ms:延时计数, prescaler:延时分频.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinSetHeadDelay(u8 base_ms, u8 prescaler)
{
	u16 tmp;
	tmp = (MAIN_Fosc * base_ms) / 1000;
	LinWriteReg(HDRH,(u8)(tmp>>8));
	LinWriteReg(HDRL,(u8)tmp);		//设置帧头延时计数

	LinWriteReg(HDP,prescaler);		//设置帧头延时分频
}

//========================================================================
// 函数: void LinInit()
// 描述: Lin初始化函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinInit()
{
    P_SW1 = (P_SW1 & 0xfc) | 0x00;    //LIN脚位选择，0x00: P0.2,P0.3, 0x01: P5.2,P5.3, 0x02: P4.6,P4.7, 0x03: P7.2,P7.3
	LINICR = 0x02;		//LIN模块中断使能
	LINEN = 1;		    //LIN模块被使能

	GetLinError();				//读取清除错误寄存器
	LinWriteReg(LIE,0x0F);		//LIR中断使能寄存器
	LinSetBaudrate(9600);		//设置波特率
	LinSetHeadDelay(0x02,0x03);	//设置帧头延时
}

//========================================================================
// 函数: void LinBUS_Interrupt(void) interrupt LIN_VECTOR
// 描述: Lin总线中断函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-01-05
// 备注: 
//========================================================================
void LinBUS_Interrupt(void) interrupt LIN_VECTOR
{
	u8 isr;
	u8 arTemp;
	arTemp = LINAR;     //LINAR 现场保存，避免主循环里写完 LINAR 后产生中断，在中断里修改了 LINAR 内容
	
	isr = LinReadReg(LSR);
	if((isr & 0x03) == 0x03)
	{
		isr = LinReadReg(LER);
		if(isr == 0x00)		//No Error
		{
			RxFlag = 1;
		}
	}
	else
	{
		isr = LinReadReg(LER);	//读取清除错误寄存器
	}

	LINAR = arTemp;    //LINAR 现场恢复
}
