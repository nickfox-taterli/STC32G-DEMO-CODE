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

本例程基于STC32G为主控芯片的实验箱9.6进行编写测试。

9.6之前实验箱版本，12864模块接口，P4.2与P4.4接口需要进行调换才能使用硬件LCM接口通信.

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

LCM接口驱动液晶屏程序

8bit M6800模式, P6口接D0~D7

sbit LCD_RS = P4^5;      //数据/命令切换
sbit LCD_RW = P4^2;      //读写控制
sbit LCD_E = P4^4;       //使能
sbit LCD_RESET = P3^4;   //复位 

LCM指令通过中断方式等待发送完成

DMA设置长度16字节，通过中断方式判断传输完成

设置时钟 1.2MHz (128*64屏, ST7920驱动, 速度太快显示会不正常).
(通过USB下载不能自定义时钟，需要使用串口方式下载)

******************************************/

#include "..\..\comm\STC32G.h"
#include "stdio.h"
#include "intrins.h"
#include "LCD_IMG.h"

#define     MAIN_Fosc       1200000L   //定义计算时钟参数

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

sbit LCD_RS = P4^5;      //数据/命令切换
sbit LCD_RW = P4^2;      //读写控制
sbit LCD_E = P4^4;       //使能
//sbit LCD_PSB = P3^5;     //PSB脚为12864的串、并通讯功能切换，我们使用8位并行接口，PSB=1
sbit LCD_RESET = P3^4;   //复位 


//IO连接
#define  LCD_DataPort P6     //8位数据口

#define  USR_LCM_IF     1			//1: use LCM Interface

u16 index;
bit DmaFlag=0;
bit LcmFlag=0;

u8  code uctech[] = {"南通国芯微电子  "};
u8  code net[]    = {" www.stcmcu.com "};
u8  code mcu[]    = {"专业制造51单片机"};
u8  code qq[]     = {" QQ: 800003751  "};

void delay_ms(u16 ms);
void GPIO_Init(void);
void LCM_Config(void);
void DMA_Config(void);
void LCD_Init(void);
void DisplayImage (u8 *DData);
void DisplayListChar(u8 X, u8 Y, u8 code *DData);
void Write_Cmd(unsigned char WCLCD,unsigned char BuysC);
void Write_Data(unsigned char WDLCD);
void LCDClear(void);

void main(void)
{
//    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快（本例程需要降低速度才能正常显示）
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

//	GPIO_Init();
#if USR_LCM_IF == 1	
	LCM_Config();
	DMA_Config();
	EA = 1;
#endif
	
//	delay_ms(100); //启动等待，等LCD进入工作状态
	LCD_Init(); //LCM初始化

	while(1)
	{
		LCDClear();
		DisplayImage(gImage_gxw);
		delay_ms(200);
		LCDClear();
		DisplayListChar(0,1,uctech);    //显示字库中的中文数字
		DisplayListChar(0,2,net);       //显示字库中的中文数字
		DisplayListChar(0,3,mcu);       //显示字库中的中文
		DisplayListChar(0,4,qq);        //显示字库中的中文数字
		delay_ms(200);
	}
}

//按指定位置显示一串字符
void DisplayListChar(u8 X, u8 Y, u8 code *DData)
{
    u8 ListLength,X2;
    ListLength = 0;
    X2 = X;
    if(Y < 1)   Y=1;
    if(Y > 4)   Y=4;
    X &= 0x0F; //限制X不能大于16，Y在1-4之内
    switch(Y)
    {
        case 1: X2 |= 0X80; break;  //根据行数来选择相应地址
        case 2: X2 |= 0X90; break;
        case 3: X2 |= 0X88; break;
        case 4: X2 |= 0X98; break;
    }
    Write_Cmd(X2, 1); //发送地址码
    while (DData[ListLength] >= 0x20) //若到达字串尾则退出
    {
        if (X <= 0x0F) //X坐标应小于0xF
        {
            Write_Data(DData[ListLength]); //
            ListLength++;
            X++;
        }
    }
}

//图形显示122*32
void DisplayImage (u8 *DData)
{
#if USR_LCM_IF == 0
    u8 y;
#endif
    u8 x,i;
    unsigned int tmp=0;
	
    for(i=0;i<9;)       //分两屏，上半屏和下半屏，因为起始地址不同，需要分开
    {
        for(x=0;x<32;x++)   //32行
        {
            Write_Cmd(0x34,1);
            Write_Cmd((u8)(0x80+x),1);    //列地址
            Write_Cmd((u8)(0x80+i),1);    //行地址，下半屏，即第三行地址0X88
            Write_Cmd(0x30,1);
					
#if USR_LCM_IF == 1

            DMA_LCM_TXAH = (u8)((u16)&DData[tmp] >> 8);
            DMA_LCM_TXAL = (u8)((u16)&DData[tmp]);
            while(DmaFlag);
            DmaFlag = 1;
            DMA_LCM_CR = 0xa0;	//Write data

#else

            for(y=0;y<16;y++)
                Write_Data(DData[tmp+y]);//读取数据写入LCD

#endif
            tmp+=16;
        }
        i+=8;
    }
    Write_Cmd(0x36,1);    //扩充功能设定
    Write_Cmd(0x30,1);
}

//========================================================================
// 函数: void delay_ms(u16 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注: 
//========================================================================
void delay_ms(u16 ms)
{
    u16 i;
    do{
        i = MAIN_Fosc / 6000;
        while(--i);
    }while(--ms);
}

#if USR_LCM_IF == 0
void LCD_delay(void)
{
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
}
#endif

//读状态
void ReadStatusLCD(void)
{
#if USR_LCM_IF == 1

    do{
        LcmFlag = 1;
        LCMIFCR = 0x86;		//Enable interface, Read command
        while(LcmFlag);
        LCD_E = 0;
    }while(LCMIFDATL & 0x80);
	
#else
	
	LCD_RS = 0;
	LCD_RW = 1; 
	LCD_delay();
	LCD_E = 1;
	LCD_delay();
	while (LCD_DataPort & 0x80); //检测忙信号
	LCD_E = 0;

#endif
}

void Write_Cmd(unsigned char WCLCD,unsigned char BuysC)
{
#if USR_LCM_IF == 1
	
	if (BuysC) ReadStatusLCD(); //根据需要检测忙 
	LCMIFDATL = WCLCD;
	LcmFlag = 1;
	LCMIFCR = 0x84;		//Enable interface, write command out
	while(LcmFlag);
	
#else
	
	if (BuysC) ReadStatusLCD(); //根据需要检测忙 
	LCD_RS = 0;
	LCD_RW = 0; 
	LCD_DataPort = WCLCD;
	LCD_delay();
	LCD_E = 1; 
	LCD_delay();
	LCD_E = 0;  
	
#endif
}

void Write_Data(unsigned char WDLCD)
{
#if USR_LCM_IF == 1
	
	LCMIFDATL = WDLCD;
	LcmFlag = 1;
	LCMIFCR = 0x85;		//Enable interface, write data out
	while(LcmFlag);
	
#else
	
	ReadStatusLCD(); //检测忙 
	LCD_RS = 1;
	LCD_RW = 0;
	LCD_DataPort = WDLCD;
	LCD_delay();
	LCD_E = 1;
	LCD_delay();
	LCD_E = 0;
	
#endif
}

/*****************************************************************************
 * @name       :void GPIO_Init(void)
 * @date       :2018-11-13 
 * @function   :Set the gpio to push-pull mode
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
//void GPIO_Init(void)
//{
//	//P6口设置成推挽输出
//	P6M0 |= 0xff;
//	P6M1 &= 0x00;

//	//P3.4口设置成推挽输出
//	P3M0 |= 0x10;
//	P3M1 &= ~0x10;

//	//P4.2,P4.4,P4.5口设置成推挽输出
//	P4M0 |= 0x34;
//	P4M1 &= ~0x34;
//}

#if USR_LCM_IF == 1	
/*****************************************************************************
 * @name       :void LCM_Config(void)
 * @date       :2018-11-13 
 * @function   :Config LCM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCM_Config(void)
{
	LCMIFCFG = 0x85;   //bit7 1:Enable Interrupt, bit1 0:8bit mode; bit0 0:8080,1:6800
	LCMIFCFG2 = 0x1f;  //RS:P45,RD:P44,WR:P42; Setup Time,HOLD Time
	LCMIFSTA = 0x00;
}

/*****************************************************************************
 * @name       :void DMA_Config(void)
 * @date       :2020-12-09 
 * @function   :Config DMA
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void DMA_Config(void)
{
	DMA_LCM_AMT = 0x0f;		//设置传输总字节数(低8位)：n+1
	DMA_LCM_AMTH = 0x00;	//设置传输总字节数(高8位)：n+1
	DMA_LCM_TXAH = (u8)((u16)&gImage_gxw >> 8);
	DMA_LCM_TXAL = (u8)((u16)&gImage_gxw);
	DMA_LCM_STA = 0x00;
	DMA_LCM_CFG = 0x82;
	DMA_LCM_CR = 0x80;
}
#endif

/*****************************************************************************
 * @name       :void LCDReset(void)
 * @date       :2018-08-09 
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCDReset(void)
{
//	LCD_PSB = 1;    //并口
	delay_ms(10);
	LCD_RESET = 0;
	delay_ms(10);
	LCD_RESET = 1;
	delay_ms(100);
}

/*****************************************************************************
 * @name       :void LCD_Init(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 	 
void LCD_Init(void)
{
	LCDReset(); //初始化之前复位
//*************LCD 初始化**********//	
	Write_Cmd(0x30,1); //显示模式设置,开始要求每次检测忙信号
	Write_Cmd(0x01,1); //显示清屏
	Write_Cmd(0x06,1); // 显示光标移动设置
	Write_Cmd(0x0C,1); // 显示开及光标设置
}

void LCDClear(void) //清屏
{
	Write_Cmd(0x01,1); //显示清屏
	Write_Cmd(0x34,1); // 显示光标移动设置
	Write_Cmd(0x30,1); // 显示开及光标设置
}

/*****************************************************************************
 * @name       :void LCM_Interrupt(void)
 * @date       :2018-11-13 
 * @function   :None
 * @parameters :None
 * @retvalue   :
******************************************************************************/ 
void LCMIF_DMA_Interrupt(void) interrupt 13
{
	if(LCMIFSTA & 0x01)
	{
		LCMIFSTA = 0x00;
		LcmFlag = 0;
	}
	
	if(DMA_LCM_STA & 0x01)
	{
		DmaFlag = 0;
		DMA_LCM_CR = 0;
		DMA_LCM_STA = 0;
	}
}
