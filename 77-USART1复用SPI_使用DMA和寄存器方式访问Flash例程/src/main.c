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

USART1��SPIģʽ��ʹ��DMA�ͼĴ�����ʽ����ʵ�����ϵ����flash.

����2(P4.6,P4.7)������Խ����Ĭ�ϲ�����:  115200,8,N,1. 

����ʱ, ѡ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#include "stc32g.h"
#include "stdio.h"

#define FOSC    24000000UL                      //ϵͳ����Ƶ��
#define BAUD    (65536 - FOSC/4/115200)         //���Դ��ڲ�����

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

BYTE xdata buffer1[256];                        //���建����
BYTE xdata buffer2[256];                        //ע��:�����Ҫʹ��DMA��������,�򻺳������붨����xdata������

void main()
{
    int i;
    
    sys_init();                                 //ϵͳ��ʼ��
    usart1_spi_init();                          //USART1ʹ��SPIģʽ��ʼ��
    
    printf("\r\nUSART_SPI_DMA test !\r\n");
    flash_read_id();
    flash_read_data(0x0000, 0x80, buffer1);     //����ʹ��USART1��SPIģʽ��ȡ���FLASH������
    flash_erase_sector(0x0000);                 //����ʹ��USART1��SPIģʽ�������FLASH��һ������
    flash_read_data(0x0000, 0x80, buffer1);
    for (i=0; i<128; i++)
        buffer2[i] = i;
    flash_write_data(0x0000, 0x80, buffer2);    //����ʹ��USART1��SPIģʽд���ݵ����FLASH
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
    
    //��ʼ������2,���ڵ��Գ���
    S2_S = 1;       //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
    S2CFG |= 0x01;  //ʹ�ô���2ʱ��W1λ��������Ϊ1��������ܻ��������Ԥ�ڵĴ���
    S2CON = 0x52;
    T2L = BAUD;
    T2H = BAUD >> 8;
    T2x12 = 1;
    T2R = 1;
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
}

BYTE usart1_spi_shift(BYTE dat)
{
    TI = 0;
    SBUF = dat;                         //��������
    while (!TI);                        //TI��־������ģʽ����/����������ɱ�־
    
    return SBUF;                        //��ȡ���յ�����
}

BOOL flash_is_busy()
{
    BYTE dat;

    S1SS = 0;
    
    usart1_spi_shift(0x05);             //���Ͷ�ȡ״̬�Ĵ�������
    dat = usart1_spi_shift(0);          //��ȡ״̬�Ĵ���
    
    S1SS = 1;

    return (dat & 0x01);                //���FLASH��æ��־
}

void flash_read_id()
{
    BYTE id[3];
    
    S1SS = 0;
    
    usart1_spi_shift(0x9f);             //���Ͷ�ȡFLASH ID����
    id[0] = usart1_spi_shift(0);       //��ȡID1
    id[1] = usart1_spi_shift(0);       //��ȡID2
    id[2] = usart1_spi_shift(0);       //��ȡID3
    
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
    
    usart1_spi_shift(0x03);             //���Ͷ�ȡFLASH��������
    usart1_spi_shift((BYTE)(addr >> 16));
    usart1_spi_shift((BYTE)(addr >> 8));
    usart1_spi_shift((BYTE)(addr));     //����Ŀ���ַ
    
//  sz = size;
//  ptr = pdat;
//  while (sz--)
//      *ptr++ = usart1_spi_shift(0);   //�Ĵ�����ʽ������
      
    usart1_rx_dma(size, pdat);          //DMA��ʽ������

    S1SS = 1;

    printf("ReadData : ");
    sz = size;
    ptr = pdat;
    for (sz=0; sz<size; sz++)
    {
        printf("%02bx ", *ptr++);        //�����������ݷ��͵�����,����ʹ��
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
    
    usart1_spi_shift(0x06);             //����дʹ������
    
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
        
        usart1_spi_shift(0x02);         //����д��������
        usart1_spi_shift((BYTE)(addr >> 16));
        usart1_spi_shift((BYTE)(addr >> 8));
        usart1_spi_shift((BYTE)(addr));
        
//      do
//      {
//          usart1_spi_shift(*pdat++);  //�Ĵ�����ʽд����
//          addr++;
//
//          if ((BYTE)(addr) == 0x00)
//              break;
//      } while (--sz);

        usart1_tx_dma(sz, pdat);        //DMA��ʽд����(ע��:���ݱ�����һ��page֮��)
        sz = 0;
        
        S1SS = 1;
    }

    printf("Program !\r\n");
}

void flash_erase_sector(DWORD addr)
{
    flash_write_enable();

    S1SS = 0;
    usart1_spi_shift(0x20);             //���Ͳ�������
    usart1_spi_shift((BYTE)(addr >> 16));
    usart1_spi_shift((BYTE)(addr >> 8));
    usart1_spi_shift((BYTE)(addr));
    S1SS = 1;

    printf("Erase Sector !\r\n");
}

void usart1_tx_dma(WORD size, BYTE xdata *pdat)
{
    size--;                             //DMA�����ֽ�����ʵ����1
    
    DMA_UR1T_CFG = 0x00;                //�ر�DMA�ж�
    DMA_UR1T_STA = 0x00;                //���DMA״̬
    DMA_UR1T_AMT = size;                //����DMA�����ֽ���
    DMA_UR1T_AMTH = size >> 8;
    DMA_UR1T_TXAL = (BYTE)pdat;         //���û�������ַ(ע��:������������xdata����)
    DMA_UR1T_TXAH = (WORD)pdat >> 8;
    DMA_UR1T_CR = 0xc0;                 //ʹ��DMA,��������1��������
    
    while (!(DMA_UR1T_STA & 0x01));     //�ȴ�DMA���ݴ������
    DMA_UR1T_STA = 0x00;                //���DMA״̬
    DMA_UR1T_CR = 0x00;                 //�ر�DMA
}

void usart1_rx_dma(WORD size, BYTE xdata *pdat)
{
    size--;                             //DMA�����ֽ�����ʵ����1
    
    DMA_UR1R_CFG = 0x00;                //�ر�DMA�ж�
    DMA_UR1R_STA = 0x00;                //���DMA״̬
    DMA_UR1R_AMT = size;                //����DMA�����ֽ���
    DMA_UR1R_AMTH = size >> 8;
    DMA_UR1R_RXAL = (BYTE)pdat;         //���û�������ַ(ע��:������������xdata����)
    DMA_UR1R_RXAH = (WORD)pdat >> 8;
    DMA_UR1R_CR = 0xa1;                 //ʹ��DMA,��ս���FIFO,��������1��������
    
                                        //!!!!!!!!!!!!!
    usart1_tx_dma(size+1, pdat);        //ע��:��������ʱ����ͬʱ��������DMA
                                        //!!!!!!!!!!!!!
    
    while (!(DMA_UR1R_STA & 0x01));     //�ȴ�DMA���ݴ������
    DMA_UR1R_STA = 0x00;                //���DMA״̬
    DMA_UR1R_CR = 0x00;                 //�ر�DMA
}
