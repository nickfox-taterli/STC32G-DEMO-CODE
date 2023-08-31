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

USART1的SPI模式，使用DMA和寄存器方式访问实验箱上的外挂flash.

串口2(P4.6,P4.7)输出调试结果，默认波特率:  115200,8,N,1. 

下载时, 选择时钟 24MHz (用户可自行修改频率).

******************************************/

#include "stc32g.h"
#include "stdio.h"

#define FOSC    24000000UL                      //系统工作频率
#define BAUD    (65536 - FOSC/4/115200)         //调试串口波特率

typedef bit BOOL;
typedef unsigned char BYTE;
typedef unsigned int WORD;
typedef unsigned long DWORD;

sbit S1SS       =   P2^2;
sbit S1MOSI     =   P2^3;
sbit S1MISO     =   P2^4;
sbit S1SCLK     =   P2^5;

void sys_init();
void usart1_spi_init();
void usart1_tx_dma(WORD size, BYTE xdata *pdat);
void usart1_rx_dma(WORD size, BYTE xdata *pdat);
BOOL flash_is_busy();
void flash_read_id();
void flash_read_data(DWORD addr, WORD size, BYTE xdata *pdat);
void flash_write_enable();
void flash_write_data(DWORD addr, WORD size, BYTE xdata *pdat);
void flash_erase_sector(DWORD addr);

BYTE xdata buffer1[256];                        //定义缓冲区
BYTE xdata buffer2[256];                        //注意:如果需要使用DMA发送数据,则缓冲区必须定义在xdata区域内

void main()
{
    int i;
    
    sys_init();                                 //系统初始化
    usart1_spi_init();                          //USART1使能SPI模式初始化
    
    printf("\r\nUSART_SPI_DMA test !\r\n");
    flash_read_id();
    flash_read_data(0x0000, 0x80, buffer1);     //测试使用USART1的SPI模式读取外挂FLASH的数据
    flash_erase_sector(0x0000);                 //测试使用USART1的SPI模式擦除外挂FLASH的一个扇区
    flash_read_data(0x0000, 0x80, buffer1);
    for (i=0; i<128; i++)
        buffer2[i] = i;
    flash_write_data(0x0000, 0x80, buffer2);    //测试使用USART1的SPI模式写数据到外挂FLASH
    flash_read_data(0x0000, 0x80, buffer1);
    
    while (1);
}

char putchar(char dat)
{
    while (!S2TI);
    S2TI = 0;
    S2BUF = dat;
    
    return dat;
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
    
    //初始化串口2,用于调试程序
    S2_S = 1;       //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
    S2CFG |= 0x01;  //使用串口2时，W1位必需设置为1，否则可能会产生不可预期的错误
    S2CON = 0x52;
    T2L = BAUD;
    T2H = BAUD >> 8;
    T2x12 = 1;
    T2R = 1;
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
}

BYTE usart1_spi_shift(BYTE dat)
{
    TI = 0;
    SBUF = dat;                         //发送数据
    while (!TI);                        //TI标志是主机模式发送/接收数据完成标志
    
    return SBUF;                        //读取接收的数据
}

BOOL flash_is_busy()
{
    BYTE dat;

    S1SS = 0;
    
    usart1_spi_shift(0x05);             //发送读取状态寄存器命令
    dat = usart1_spi_shift(0);          //读取状态寄存器
    
    S1SS = 1;

    return (dat & 0x01);                //检测FLASH的忙标志
}

void flash_read_id()
{
    BYTE id[3];
    
    S1SS = 0;
    
    usart1_spi_shift(0x9f);             //发送读取FLASH ID命令
    id[0] = usart1_spi_shift(0);       //读取ID1
    id[1] = usart1_spi_shift(0);       //读取ID2
    id[2] = usart1_spi_shift(0);       //读取ID3
    
    S1SS = 1;

    printf("ReadID : ");
    printf("%02bx", id[0]);
    printf("%02bx", id[1]);
    printf("%02bx\r\n", id[2]);
}

void flash_read_data(DWORD addr, WORD size, BYTE xdata *pdat)
{
    WORD sz;
    BYTE *ptr;

    while (flash_is_busy());

    S1SS = 0;
    
    usart1_spi_shift(0x03);             //发送读取FLASH数据命令
    usart1_spi_shift((BYTE)(addr >> 16));
    usart1_spi_shift((BYTE)(addr >> 8));
    usart1_spi_shift((BYTE)(addr));     //设置目标地址
    
//  sz = size;
//  ptr = pdat;
//  while (sz--)
//      *ptr++ = usart1_spi_shift(0);   //寄存器方式读数据
      
    usart1_rx_dma(size, pdat);          //DMA方式读数据

    S1SS = 1;

    printf("ReadData : ");
    sz = size;
    ptr = pdat;
    for (sz=0; sz<size; sz++)
    {
        printf("%02bx ", *ptr++);        //将读到的数据发送到串口,调试使用
        if ((sz % 16) == 15)
        {
            printf("\r\n           ");
        }
    }
    printf("\r\n");
}

void flash_write_enable()
{
    while (flash_is_busy());

    S1SS = 0;
    
    usart1_spi_shift(0x06);             //发送写使能命令
    
    S1SS = 1;
}

void flash_write_data(DWORD addr, WORD size, BYTE xdata *pdat)
{
    WORD sz;

    sz = size;
    while (sz)
    {
        flash_write_enable();

        S1SS = 0;
        
        usart1_spi_shift(0x02);         //发送写数据命令
        usart1_spi_shift((BYTE)(addr >> 16));
        usart1_spi_shift((BYTE)(addr >> 8));
        usart1_spi_shift((BYTE)(addr));
        
//      do
//      {
//          usart1_spi_shift(*pdat++);  //寄存器方式写数据
//          addr++;
//
//          if ((BYTE)(addr) == 0x00)
//              break;
//      } while (--sz);

        usart1_tx_dma(sz, pdat);        //DMA方式写数据(注意:数据必须在一个page之内)
        sz = 0;
        
        S1SS = 1;
    }

    printf("Program !\r\n");
}

void flash_erase_sector(DWORD addr)
{
    flash_write_enable();

    S1SS = 0;
    usart1_spi_shift(0x20);             //发送擦除命令
    usart1_spi_shift((BYTE)(addr >> 16));
    usart1_spi_shift((BYTE)(addr >> 8));
    usart1_spi_shift((BYTE)(addr));
    S1SS = 1;

    printf("Erase Sector !\r\n");
}

void usart1_tx_dma(WORD size, BYTE xdata *pdat)
{
    size--;                             //DMA传输字节数比实际少1
    
    DMA_UR1T_CFG = 0x00;                //关闭DMA中断
    DMA_UR1T_STA = 0x00;                //清除DMA状态
    DMA_UR1T_AMT = size;                //设置DMA传输字节数
    DMA_UR1T_AMTH = size >> 8;
    DMA_UR1T_TXAL = (BYTE)pdat;         //设置缓冲区地址(注意:缓冲区必须是xdata类型)
    DMA_UR1T_TXAH = (WORD)pdat >> 8;
    DMA_UR1T_CR = 0xc0;                 //使能DMA,触发串口1发送数据
    
    while (!(DMA_UR1T_STA & 0x01));     //等待DMA数据传输完成
    DMA_UR1T_STA = 0x00;                //清除DMA状态
    DMA_UR1T_CR = 0x00;                 //关闭DMA
}

void usart1_rx_dma(WORD size, BYTE xdata *pdat)
{
    size--;                             //DMA传输字节数比实际少1
    
    DMA_UR1R_CFG = 0x00;                //关闭DMA中断
    DMA_UR1R_STA = 0x00;                //清除DMA状态
    DMA_UR1R_AMT = size;                //设置DMA传输字节数
    DMA_UR1R_AMTH = size >> 8;
    DMA_UR1R_RXAL = (BYTE)pdat;         //设置缓冲区地址(注意:缓冲区必须是xdata类型)
    DMA_UR1R_RXAH = (WORD)pdat >> 8;
    DMA_UR1R_CR = 0xa1;                 //使能DMA,清空接收FIFO,触发串口1接收数据
    
                                        //!!!!!!!!!!!!!
    usart1_tx_dma(size+1, pdat);        //注意:接收数据时必须同时启动发送DMA
                                        //!!!!!!!!!!!!!
    
    while (!(DMA_UR1R_STA & 0x01));     //等待DMA数据传输完成
    DMA_UR1R_STA = 0x00;                //清除DMA状态
    DMA_UR1R_CR = 0x00;                 //关闭DMA
}
