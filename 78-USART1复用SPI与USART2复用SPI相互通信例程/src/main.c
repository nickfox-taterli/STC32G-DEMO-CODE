/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���STC32GΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

USART1����SPI��USART2����SPI�໥ͨ�š�

ͨ��P6��LED��ָʾͨ�Ž����P6=0x5a��ʾ���ݴ�����ȷ��

����ʱ, ѡ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#include "stc32g.h"
#include "stdio.h"

#define FOSC    24000000UL                      //ϵͳ����Ƶ��

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

BYTE xdata buffer1[256];                        //���建����
BYTE xdata buffer2[256];                        //���建����
BYTE rptr;
BYTE wptr;
bit over;

void main()
{
    int i;
    
    sys_init();                                 //ϵͳ��ʼ��
    usart1_spi_init();                          //USART1ʹ��SPI��ģʽ��ʼ��
    usart2_spi_init();                          //USART2ʹ��SPI��ģʽ��ʼ��
    EA = 1;
    
    for (i=0; i<128; i++)
    {
        buffer1[i] = i;                         //��ʼ��������
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
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�
    
    P0M1 = 0x30;   P0M0 = 0x30;   //����P0.4��P0.5Ϊ©����·(ʵ��������������赽3.3V)
    P1M1 = 0x32;   P1M0 = 0x32;   //����P1.1��P1.4��P1.5Ϊ©����·(ʵ��������������赽3.3V), P1.1��PWM��DAC��·ͨ�����贮����P2.3
    P2M1 = 0x3c;   P2M0 = 0x3c;   //����P2.2~P2.5Ϊ©����·(ʵ��������������赽3.3V)�����ÿ�©ģʽ��Ҫ�Ͽ�PWM��DAC��·�е�R2����
    P3M1 = 0x50;   P3M0 = 0x50;   //����P3.4��P3.6Ϊ©����·(ʵ��������������赽3.3V)
    P4M1 = 0x3c;   P4M0 = 0x3c;   //����P4.2~P4.5Ϊ©����·(ʵ��������������赽3.3V)
    P5M1 = 0x0c;   P5M0 = 0x0c;   //����P5.2��P5.3Ϊ©����·(ʵ��������������赽3.3V)
    P6M1 = 0xff;   P6M0 = 0xff;   //����Ϊ©����·(ʵ��������������赽3.3V)
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���
    
    P40 = 0;
    P6 = 0xff;
}

void usart1_spi_init()
{
    S1SPI_S0 = 1;                       //�л�S1SPI��P2.2/S1SS,P2.3/S1MOSI,P2.4/S1MISO,P2.5/S1SCLK
    S1SPI_S1 = 0;
    SCON = 0x10;                        //ʹ�ܽ���,��������Ϊ����ģʽ0
    
    USARTCR1 = 0x10;                    //ʹ��USART1��SPIģʽ
//  USARTCR1 |= 0x40;                   //DORD=1
    USARTCR1 &= ~0x40;                  //DORD=0
//  USARTCR1 |= 0x04;                   //�ӻ�ģʽ
    USARTCR1 &= ~0x04;                  //����ģʽ
    USARTCR1 |= 0x00;                   //CPOL=0, CPHA=0
//  USARTCR1 |= 0x01;                   //CPOL=0, CPHA=1
//  USARTCR1 |= 0x02 ;                  //CPOL=1, CPHA=0
//  USARTCR1 |= 0x03;                   //CPOL=1, CPHA=1
//  USARTCR4 = 0x00;                    //SPI�ٶ�ΪSYSCLK/4
//  USARTCR4 = 0x01;                    //SPI�ٶ�ΪSYSCLK/8
    USARTCR4 = 0x02;                    //SPI�ٶ�ΪSYSCLK/16
//  USARTCR4 = 0x03;                    //SPI�ٶ�ΪSYSCLK/2
    USARTCR1 |= 0x08;                   //ʹ��SPI����
 
    ES = 1;
}

void usart2_spi_init()
{
    S2SPI_S0 = 1;                       //�л�S2SPI��P2.2/S2SS,P2.3/S2MOSI,P2.4/S2MISO,P2.5/S2SCLK
    S2SPI_S1 = 0;
    S2CON = 0x10;                       //ʹ�ܽ���,��������Ϊ����ģʽ0
    
    USART2CR1 = 0x10;                   //ʹ��USART2��SPIģʽ
//  USART2CR1 |= 0x40;                  //DORD=1
    USART2CR1 &= ~0x40;                 //DORD=0
    USART2CR1 |= 0x04;                  //�ӻ�ģʽ
//  USART2CR1 &= ~0x04;                 //����ģʽ
    USART2CR1 |= 0x00;                  //CPOL=0, CPHA=0
//  USART2CR1 |= 0x01;                  //CPOL=0, CPHA=1
//  USART2CR1 |= 0x02 ;                 //CPOL=1, CPHA=0
//  USART2CR1 |= 0x03;                  //CPOL=1, CPHA=1
//  USART2CR4 = 0x00;                   //SPI�ٶ�ΪSYSCLK/4
//  USART2CR4 = 0x01;                   //SPI�ٶ�ΪSYSCLK/8
    USART2CR4 = 0x02;                   //SPI�ٶ�ΪSYSCLK/16
//  USART2CR4 = 0x03;                   //SPI�ٶ�ΪSYSCLK/2
    USART2CR1 |= 0x08;                  //ʹ��SPI����

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
    SBUF = buffer1[rptr++];             //�������ݴ���
    while (!over);                      //�ȴ�128�����ݴ������
    S1SS = 1;
    
    ret = 0x5a;
    for (i=0; i<128; i++)
    {
        if (buffer1[i] != buffer2[i])   //У������
        {
            ret = 0xfe;
            break;
        }
    }
    P6 = ret;                           //P6=0x5a��ʾ���ݴ�����ȷ
}

