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

USART1复用SPI与USART2复用SPI相互通信。

通过P6口LED灯指示通信结果，P6=0x5a表示数据传输正确。

下载时, 选择时钟 24MHz (用户可自行修改频率).

******************************************/

#include "stc32g.h"
#include "stdio.h"

#define FOSC    24000000UL                      //系统工作频率

typedef bit BOOL;
typedef unsigned char BYTE;
typedef unsigned int WORD;
typedef unsigned long DWORD;

sbit S1SS       =   P2^2;
sbit S1MOSI     =   P2^3;
sbit S1MISO     =   P2^4;
sbit S1SCLK     =   P2^5;

sbit S2SS       =   P2^2;
sbit S2MOSI     =   P2^3;
sbit S2MISO     =   P2^4;
sbit S2SCLK     =   P2^5;

void sys_init();
void usart1_spi_init();
void usart2_spi_init();
void test();

BYTE xdata buffer1[256];                        //定义缓冲区
BYTE xdata buffer2[256];                        //定义缓冲区
BYTE rptr;
BYTE wptr;
bit over;

void main()
{
    int i;
    
    sys_init();                                 //系统初始化
    usart1_spi_init();                          //USART1使能SPI主模式初始化
    usart2_spi_init();                          //USART2使能SPI从模式初始化
    EA = 1;
    
    for (i=0; i<128; i++)
    {
        buffer1[i] = i;                         //初始化缓冲区
        buffer2[i] = 0;
    }
    test();
    
    while (1);
}

void uart1_isr() interrupt UART1_VECTOR
{
    if (TI)
    {
        TI = 0;
        
        if (rptr < 128)
        {
            SBUF = buffer1[rptr++];
        }
        else
        {
            over = 1;
        }
    }
    
    if (RI)
    {
        RI = 0;
    }
}

void uart2_isr() interrupt UART2_VECTOR
{
    if (S2TI)
    {
        S2TI = 0;
    }
    
    if (S2RI)
    {
        S2RI = 0;
        buffer2[wptr++] = S2BUF;
    }
}

void sys_init()
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
    
    P40 = 0;
    P6 = 0xff;
}

void usart1_spi_init()
{
    S1SPI_S0 = 1;                       //切换S1SPI到P2.2/S1SS,P2.3/S1MOSI,P2.4/S1MISO,P2.5/S1SCLK
    S1SPI_S1 = 0;
    SCON = 0x10;                        //使能接收,必须设置为串口模式0
    
    USARTCR1 = 0x10;                    //使能USART1的SPI模式
//  USARTCR1 |= 0x40;                   //DORD=1
    USARTCR1 &= ~0x40;                  //DORD=0
//  USARTCR1 |= 0x04;                   //从机模式
    USARTCR1 &= ~0x04;                  //主机模式
    USARTCR1 |= 0x00;                   //CPOL=0, CPHA=0
//  USARTCR1 |= 0x01;                   //CPOL=0, CPHA=1
//  USARTCR1 |= 0x02 ;                  //CPOL=1, CPHA=0
//  USARTCR1 |= 0x03;                   //CPOL=1, CPHA=1
//  USARTCR4 = 0x00;                    //SPI速度为SYSCLK/4
//  USARTCR4 = 0x01;                    //SPI速度为SYSCLK/8
    USARTCR4 = 0x02;                    //SPI速度为SYSCLK/16
//  USARTCR4 = 0x03;                    //SPI速度为SYSCLK/2
    USARTCR1 |= 0x08;                   //使能SPI功能
 
    ES = 1;
}

void usart2_spi_init()
{
    S2SPI_S0 = 1;                       //切换S2SPI到P2.2/S2SS,P2.3/S2MOSI,P2.4/S2MISO,P2.5/S2SCLK
    S2SPI_S1 = 0;
    S2CON = 0x10;                       //使能接收,必须设置为串口模式0
    
    USART2CR1 = 0x10;                   //使能USART2的SPI模式
//  USART2CR1 |= 0x40;                  //DORD=1
    USART2CR1 &= ~0x40;                 //DORD=0
    USART2CR1 |= 0x04;                  //从机模式
//  USART2CR1 &= ~0x04;                 //主机模式
    USART2CR1 |= 0x00;                  //CPOL=0, CPHA=0
//  USART2CR1 |= 0x01;                  //CPOL=0, CPHA=1
//  USART2CR1 |= 0x02 ;                 //CPOL=1, CPHA=0
//  USART2CR1 |= 0x03;                  //CPOL=1, CPHA=1
//  USART2CR4 = 0x00;                   //SPI速度为SYSCLK/4
//  USART2CR4 = 0x01;                   //SPI速度为SYSCLK/8
    USART2CR4 = 0x02;                   //SPI速度为SYSCLK/16
//  USART2CR4 = 0x03;                   //SPI速度为SYSCLK/2
    USART2CR1 |= 0x08;                  //使能SPI功能

    ES2 = 1;
}

void test()
{
    BYTE i;
    BYTE ret;
    
    wptr = 0;
    rptr = 0;
    over = 0;
    
    S1SS = 0;
    SBUF = buffer1[rptr++];             //启动数据传输
    while (!over);                      //等待128个数据传输完成
    S1SS = 1;
    
    ret = 0x5a;
    for (i=0; i<128; i++)
    {
        if (buffer1[i] != buffer2[i])   //校验数据
        {
            ret = 0xfe;
            break;
        }
    }
    P6 = ret;                           //P6=0x5a表示数据传输正确
}

