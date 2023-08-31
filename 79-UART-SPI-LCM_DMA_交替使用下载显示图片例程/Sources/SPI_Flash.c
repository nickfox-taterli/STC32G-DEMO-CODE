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

#include "STC32G.h"
#include "system.h"
#include "intrins.h"
#include "SPI_Flash.h"
#include "stdio.h"
#include "uart.h"
#include "lcm.h"

/*************  ���س�������    **************/

//======== PLL ��ض��� =========
#define HSCK_MCLK       0
#define HSCK_PLL        1
#define HSCK_SEL        HSCK_PLL

#define PLL_96M         0       //PLLʱ�� = PLL����ʱ��*8
#define PLL_144M        1       //PLLʱ�� = PLL����ʱ��*12
#define PLL_SEL         PLL_144M

#define CKMS            0x80
#define HSIOCK          0x40
#define MCK2SEL_MSK     0x0c
#define MCK2SEL_SEL1    0x00
#define MCK2SEL_PLL     0x04
#define MCK2SEL_PLLD2   0x08
#define MCK2SEL_IRC48   0x0c
#define MCKSEL_MSK      0x03
#define MCKSEL_HIRC     0x00
#define MCKSEL_XOSC     0x01
#define MCKSEL_X32K     0x02
#define MCKSEL_IRC32K   0x03

#define ENCKM           0x80
#define PCKI_MSK        0x60
#define PCKI_D1         0x00
#define PCKI_D2         0x20
#define PCKI_D4         0x40
#define PCKI_D8         0x60

/*************  ���ر�������    **************/

u32 Flash_addr;
u16 lcdIndex;
u8 xdata DmaBuffer1[DMA_AMT_LEN];
u8 xdata DmaBuffer2[DMA_AMT_LEN];

/*************  FLASH��ر�������   **************/
sbit    SPI_CS  = P2^2;     //PIN1
sbit    SPI_MISO = P2^4;    //PIN2
sbit    SPI_MOSI = P2^3;    //PIN5
sbit    SPI_SCK = P2^5;     //PIN6

u8  B_FlashOK;                                //Flash״̬
u8  PM25LV040_ID, PM25LV040_ID1, PM25LV040_ID2;

bit SpiDmaFlag;
bit DmaBufferSW = 0;

void FlashCheckID(void);

//========================================================================
// ����: void SPI_DMA_Config(void)
// ����: SPI DMA ��������.
// ����: none.
// ����: none.
// �汾: V1.0, 2021-5-6
//========================================================================
void SPI_DMA_Config(void)
{
    //�رս���DMA���´ν��յ��������´������ʼ��ַλ�ã������´ν������ݼ����������š�
    DMA_SPI_CR = 0x00;		//bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO

	DMA_SPI_STA = 0x00;
	DMA_SPI_CFG = 0xE5;		//bit7 1:Enable Interrupt, �ṩSPI_DMA���ȼ�
	DMA_SPI_AMT = (u8)(DMA_WR_LEN-1);         //���ô������ֽ���(��8λ)��n+1
	DMA_SPI_AMTH = (u8)((DMA_WR_LEN-1) >> 8); //���ô������ֽ���(��8λ)��n+1

	DMA_SPI_TXAH = (u8)((u16)&DmaBuffer2 >> 8);	//SPI�������ݴ洢��ַ
	DMA_SPI_TXAL = (u8)((u16)&DmaBuffer2);
	DMA_SPI_RXAH = (u8)((u16)&DmaBuffer1 >> 8);	//SPI�������ݴ洢��ַ
	DMA_SPI_RXAL = (u8)((u16)&DmaBuffer1);

	DMA_SPI_CFG2 = 0x01;	//01:P2.2
	DMA_SPI_CR = 0x81;		//bit7 1:ʹ�� SPI_DMA, bit6 1:��ʼ SPI_DMA ����ģʽ, bit0 1:��� SPI_DMA FIFO
}


/******************* FLASH��س��� ************************/
#define SFC_WREN        0x06        //����Flash���
#define SFC_WRDI        0x04
#define SFC_RDSR        0x05
#define SFC_WRSR        0x01
#define SFC_READ        0x03
#define SFC_FASTREAD    0x0B
#define SFC_RDID        0xAB
#define SFC_PAGEPROG    0x02
#define SFC_RDCR        0xA1
#define SFC_WRCR        0xF1
#define SFC_SECTORER1   0xD7        //PM25LV040 ��������ָ��
#define SFC_SECTORER2   0x20        //W25Xxx ��������ָ��
#define SFC_BLOCKER     0xD8
#define SFC_CHIPER      0xC7

#define SPI_CE_High()   SPI_CS  = 1     // set CE high
#define SPI_CE_Low()    SPI_CS  = 0     // clear CE low
#define SPI_Hold()      P_SPI_Hold      = 0     // clear Hold pin
#define SPI_UnHold()    P_SPI_Hold      = 1     // set Hold pin
#define SPI_WP()        P_SPI_WP        = 0     // clear WP pin
#define SPI_UnWP()      P_SPI_WP        = 1     // set WP pin

/************************************************************************/
/*
void delay()
{
    int i;
    
    for (i=0; i<100; i++);
}

void PLL_Init(void)
{
    //ѡ��PLL���ʱ��
#if (PLL_SEL == PLL_96M)
    CLKSEL &= ~CKMS;            //ѡ��PLL��96M��ΪPLL�����ʱ��
#elif (PLL_SEL == PLL_144M)
    CLKSEL |= CKMS;             //ѡ��PLL��144M��ΪPLL�����ʱ��
#else
    CLKSEL &= ~CKMS;            //Ĭ��ѡ��PLL��96M��ΪPLL�����ʱ��
#endif
    
    //ѡ��PLL����ʱ�ӷ�Ƶ,��֤����ʱ��Ϊ12M(����Χ��8M~16M, 12M +/- 4M)
    USBCLK &= ~PCKI_MSK;
//#if (MAIN_Fosc == 12000000UL)
//    USBCLK |= PCKI_D1;          //PLL����ʱ��1��Ƶ
//#elif (MAIN_Fosc == 24000000UL)
//    USBCLK |= PCKI_D2;          //PLL����ʱ��2��Ƶ
//#elif (MAIN_Fosc == 48000000UL)
    USBCLK |= PCKI_D4;          //PLL����ʱ��4��Ƶ
//#elif (MAIN_Fosc == 96000000UL)
//    USBCLK |= PCKI_D8;          //PLL����ʱ��8��Ƶ
//#else
//    USBCLK |= PCKI_D1;          //Ĭ��PLL����ʱ��1��Ƶ
//#endif

    //����PLL
    USBCLK |= ENCKM;            //ʹ��PLL��Ƶ
    
    delay();                    //�ȴ�PLL��Ƶ

    //ѡ��HSPWM/HSSPIʱ��
#if (HSCK_SEL == HSCK_MCLK)
    CLKSEL &= ~HSIOCK;          //HSPWM/HSSPIѡ����ʱ��Ϊʱ��Դ
#elif (HSCK_SEL == HSCK_PLL)
    CLKSEL |= HSIOCK;           //HSPWM/HSSPIѡ��PLL���ʱ��Ϊʱ��Դ
#else
    CLKSEL &= ~HSIOCK;          //Ĭ��HSPWM/HSSPIѡ����ʱ��Ϊʱ��Դ
#endif

    //��Ƶ35M��PLL=35M/4*12/2=103.2MHz/2=52.5MHz
    HSCLKDIV = 2;               //HSPWM/HSSPIʱ��Դ2��Ƶ
    
    SPCTL = 0xd3;               //����SPIΪ����ģʽ,�ٶ�ΪSPIʱ��/2(52.5M/2=26.25M)
    HSSPI_CFG2 |= 0x20;         //ʹ��SPI����ģʽ

    P2SR = 0x00;                //��ƽת���ٶȿ죨����IO�ڸ��ٷ�ת�źţ�
    P2DR = 0xff;                //�˿�����������ǿ
}
*/
/************************************************************************/
void SPI_init(void)
{
    P_SW1 = (P_SW1 & ~(3<<2)) | (1<<2);     //IO���л�. 0: P1.2/P5.4 P1.3 P1.4 P1.5, 1: P2.2 P2.3 P2.4 P2.5, 2: P5.4 P4.0 P4.1 P4.3, 3: P3.5 P3.4 P3.3 P3.2

//    PLL_Init();     //SPI ʹ��PLL����ʱ���ź�(�ٶ�̫��Flashͨ�Ų�����)
    
    HSCLKDIV = 0;  //���ø���IOʱ�ӷ�ƵΪ1��Ƶ��Ĭ��2��Ƶ��ʹSPIʱ��/2
    SSIG = 1; //���� SS ���Ź��ܣ�ʹ�� MSTR ȷ���������������Ǵӻ�
    SPEN = 1; //ʹ�� SPI ����
    DORD = 0; //�ȷ���/�������ݵĸ�λ�� MSB��
    MSTR = 1; //��������ģʽ
    CPOL = 1; //SCLK ����ʱΪ�͵�ƽ��SCLK ��ǰʱ����Ϊ�����أ���ʱ����Ϊ�½���
    CPHA = 1; //���� SS �ܽ�Ϊ�͵�ƽ������һλ���ݲ��� SCLK �ĺ�ʱ���ظı�����
    SPCTL = (SPCTL & ~3) | 0;   //SPI ʱ��Ƶ��ѡ��, 0: 4T, 1: 8T,  2: 16T,  3: 2T

    SPI_SCK = 0;    // set clock to low initial state
    SPI_MOSI = 1;
    SPIF = 1;   //��SPIF��־
    WCOL = 1;   //��WCOL��־

    FlashCheckID();
    FlashCheckID();
    
    if(!B_FlashOK)  printf("δ��⵽PM25LV040/W25X40CL/W25Q80BV!\r\n");
    else
    {
        if(B_FlashOK == 1)
        {
            printf("��⵽PM25LV040!\r\n");
        }
        else if(B_FlashOK == 2)
        {
            printf("��⵽W25X40CL!\r\n");
        }
        else if(B_FlashOK == 3)
        {
            printf("��⵽W25Q80BV!\r\n");
        }
        printf("������ID1 = 0x%02X",PM25LV040_ID1);
        printf("\r\n      ID2 = 0x%02X",PM25LV040_ID2);
        printf("\r\n   �豸ID = 0x%02X\r\n",PM25LV040_ID);
    }
}

/************************************************************************/
void SPI_WriteByte(u8 out)
{
    SPDAT = out;
    while(SPIF == 0);
    SPIF = 1;   //��SPIF��־
    WCOL = 1;   //��WCOL��־
}

/************************************************************************/
u8 SPI_ReadByte(void)
{
    SPDAT = 0xff;
    while(SPIF == 0);
    SPIF = 1;   //��SPIF��־
    WCOL = 1;   //��WCOL��־
    return (SPDAT);
}

/************************************************
���Flash�Ƿ�׼������
��ڲ���: ��
���ڲ���:
    0 : û�м�⵽��ȷ��Flash
    1 : Flash׼������
************************************************/
void FlashCheckID(void)
{
    SPI_CE_Low();
    SPI_WriteByte(SFC_RDID);        //���Ͷ�ȡID����
    SPI_WriteByte(0x00);            //�ն�3���ֽ�
    SPI_WriteByte(0x00);
    SPI_WriteByte(0x00);
    PM25LV040_ID1 = SPI_ReadByte();         //��ȡ������ID1
    PM25LV040_ID  = SPI_ReadByte();         //��ȡ�豸ID
    PM25LV040_ID2 = SPI_ReadByte();         //��ȡ������ID2
    SPI_CE_High();

//	printf("ID1=%x\r\n",PM25LV040_ID1);
//	printf("ID=%x\r\n",PM25LV040_ID);
//	printf("ID2=%x\r\n",PM25LV040_ID2);
	
    if((PM25LV040_ID1 == 0x9d) && (PM25LV040_ID2 == 0x7f))  B_FlashOK = 1;  //����Ƿ�ΪPM25LVxxϵ�е�Flash
    else if(PM25LV040_ID == 0x12)  B_FlashOK = 2;                           //����Ƿ�ΪW25X4xϵ�е�Flash
    else if(PM25LV040_ID == 0x13)  B_FlashOK = 3;                           //����Ƿ�ΪW25X8xϵ�е�Flash
    else                                                    B_FlashOK = 0;
}

/************************************************
���Flash��æ״̬
��ڲ���: ��
���ڲ���:
    0 : Flash���ڿ���״̬
    1 : Flash����æ״̬
************************************************/
u8 CheckFlashBusy(void)
{
    u8  dat;

    SPI_CE_Low();
    SPI_WriteByte(SFC_RDSR);        //���Ͷ�ȡ״̬����
    dat = SPI_ReadByte();           //��ȡ״̬
    SPI_CE_High();

    return (dat);                   //״ֵ̬��Bit0��Ϊæ��־
}

/************************************************
ʹ��Flashд����
��ڲ���: ��
���ڲ���: ��
************************************************/
void FlashWriteEnable(void)
{
    while(CheckFlashBusy() > 0);    //Flashæ���
    SPI_CE_Low();
    SPI_WriteByte(SFC_WREN);        //����дʹ������
    SPI_CE_High();
}

/************************************************
������ƬFlash
��ڲ���: ��
���ڲ���: ��
************************************************/
void FlashChipErase(void)
{
    if(B_FlashOK)
    {
        FlashWriteEnable();             //ʹ��Flashд����
        SPI_CE_Low();
        SPI_WriteByte(SFC_CHIPER);      //����Ƭ��������
        SPI_CE_High();
    }
}

/************************************************
��������, һ������4KB
��ڲ���: ��
���ڲ���: ��
************************************************/
//void FlashSectorErase(u32 addr)
//{
//    if(B_FlashOK)
//    {
//        FlashWriteEnable();             //ʹ��Flashд����
//        SPI_CE_Low();
//        if(B_FlashOK == 1)
//        {
//            SPI_WriteByte(SFC_SECTORER1);    //����������������
//        }
//        else
//        {
//            SPI_WriteByte(SFC_SECTORER2);    //����������������
//        }
//        SPI_WriteByte(((u8 *)&addr)[1]);     //������ʼ��ַ
//        SPI_WriteByte(((u8 *)&addr)[2]);
//        SPI_WriteByte(((u8 *)&addr)[3]);
//        SPI_CE_High();
//    }
//}

/************************************************
��Flash�ж�ȡ����
��ڲ���:
    addr   : ��ַ����
    buffer : �����Flash�ж�ȡ������
    size   : ���ݿ��С
���ڲ���:
    ��
************************************************/
void SPI_Read_Nbytes(u32 addr, u16 len)
{
    if(len == 0)   return;
    if(!B_FlashOK)  return;
    while(SpiDmaFlag);                     //DMAæ���
    while(CheckFlashBusy() > 0);        //Flashæ���

    SPI_CE_Low();                       //enable device
    SPI_WriteByte(SFC_READ);            //read command

    SPI_WriteByte(((u8 *)&addr)[1]);    //������ʼ��ַ
    SPI_WriteByte(((u8 *)&addr)[2]);
    SPI_WriteByte(((u8 *)&addr)[3]);

    SpiDmaFlag = 1;
	DMA_SPI_AMT = (u8)(len-1);         //���ô������ֽ���(��8λ)��n+1
	DMA_SPI_AMTH = (u8)((len-1) >> 8); //���ô������ֽ���(��8λ)��n+1
    DMA_SPI_CR |= 0x40;     //��ʼSPI_DMA��ģʽ����
}

/************************************************
д���ݵ�Flash��
��ڲ���:
    addr   : ��ַ����
    size   : ���ݿ��С
���ڲ���: ��
************************************************/
void SPI_Write_Nbytes(u32 addr, u16 len)
{
    if(len == 0)   return;
    if(!B_FlashOK)  return;
    while(SpiDmaFlag);                     //DMAæ���
    while(CheckFlashBusy() > 0);        //Flashæ���

    FlashWriteEnable();                 //ʹ��Flashд����

    SPI_CE_Low();                       // enable device
    SPI_WriteByte(SFC_PAGEPROG);        // ����ҳ�������
    SPI_WriteByte(((u8 *)&addr)[1]);    //������ʼ��ַ
    SPI_WriteByte(((u8 *)&addr)[2]);
    SPI_WriteByte(((u8 *)&addr)[3]);

    SpiDmaFlag = 1;
	DMA_SPI_AMT = (u8)(len-1);         //���ô������ֽ���(��8λ)��n+1
	DMA_SPI_AMTH = (u8)((len-1) >> 8); //���ô������ֽ���(��8λ)��n+1
    DMA_SPI_CR |= 0x40;     //��ʼSPI_DMA��ģʽ����
}

//========================================================================
// ����: void SPI_DMA_Interrupt (void) interrupt 49
// ����: SPI DMA�жϺ���
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2021-5-8
// ��ע: 
//========================================================================
void SPI_DMA_Interrupt(void) interrupt 49
{
	DMA_SPI_STA = 0;
	SpiDmaFlag = 0;
	SPI_CE_High();
}
