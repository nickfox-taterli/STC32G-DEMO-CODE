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

ͨ�����ڶ�PM25LV040/W25X40CL/W25Q80BV���ж�д���ԡ�

��FLASH������������д�롢�����Ĳ���������ָ����ַ��

Ĭ�ϲ�����:  115200,8,N,1. ������"���ڳ�ʼ������"���޸�.

������������: (��ĸ�����ִ�Сд)
    E 0x001234              --> ��EEPROM������������������ָ��ʮ�����Ƶ�ַ.
    W 0x001234 1234567890   --> ��EEPROM��������д�������ָ��ʮ�����Ƶ�ַ������Ϊд������.
    R 0x001234 10           --> ��EEPROM������������������ָ��ʮ�����Ƶ�ַ. 
    C                       --> �����ⲻ��PM25LV040/W25X40CL/W25Q80BV, ����Cǿ���������.

ע�⣺Ϊ��ͨ�ã�����ʶ���ַ�Ƿ���Ч���û��Լ����ݾ�����ͺ���������

����ʱ, ѡ��ʱ�� 22.1184MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\comm\STC32G.h"

#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        22118400L   //������ʱ�ӣ���ȷ����115200�����ʣ�

/****************************** �û������ ***********************************/
#define     Baudrate2           115200L
#define     EE_BUF_LENGTH       50          //

#define     UART2_BUF_LENGTH    (EE_BUF_LENGTH+9)   //���ڻ��峤��

/*****************************************************************************/


/*************  ���س�������    **************/


/*************  ���ر�������    **************/

u8  tmp[EE_BUF_LENGTH];
u8  sst_byte;
u32 Flash_addr;

/*************  FLASH��ر�������   **************/
sbit    P_PM25LV040_CE  = P2^2;     //PIN1
sbit    P_PM25LV040_SO  = P2^4;     //PIN2
sbit    P_PM25LV040_SI  = P2^3;     //PIN5
sbit    P_PM25LV040_SCK = P2^5;     //PIN6

u8  B_FlashOK;                                //Flash״̬
u8  PM25LV040_ID, PM25LV040_ID1, PM25LV040_ID2;

u8  RX2_TimeOut;
u8  TX2_Cnt;    //���ͼ���
u8  RX2_Cnt;    //���ռ���
bit B_TX2_Busy; //����æ��־

u8  RX2_Buffer[UART2_BUF_LENGTH];   //���ջ���

/*************  ���غ�������    **************/
void    delay_ms(u8 ms);
void    RX2_Check(void);
void    UART2_config(u8 brt);   // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
void    PrintString2(u8 *puts);
void    UART2_TxByte(u8 dat);

void    SPI_init(void);
void    FlashCheckID(void);
u8      CheckFlashBusy(void);
void    FlashWriteEnable(void);
void    FlashChipErase(void);
void    FlashSectorErase(u32 addr);
void    SPI_Read_Nbytes( u32 addr, u8 *buffer, u16 size);
u8      SPI_Read_Compare(u32 addr, u8 *buffer, u16 size);
void    SPI_Write_Nbytes(u32 addr, u8 *buffer,  u8 size);

u8  Hex2Ascii(u8 dat)
{
    dat &= 0x0f;
    if(dat < 10)    return (dat+'0');
    return (dat-10+'A');
}

/********************* ������ *************************/
void main(void)
{
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x30;   P0M0 = 0x30;   //����P0.4��P0.5Ϊ©����·(ʵ��������������赽3.3V)
    P1M1 = 0x32;   P1M0 = 0x32;   //����P1.1��P1.4��P1.5Ϊ©����·(ʵ��������������赽3.3V), P1.1��PWM��DAC��·ͨ�����贮����P2.3
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x50;   P3M0 = 0x50;   //����P3.4��P3.6Ϊ©����·(ʵ��������������赽3.3V)
    P4M1 = 0x3c;   P4M0 = 0x3c;   //����P4.2~P4.5Ϊ©����·(ʵ��������������赽3.3V)
    P5M1 = 0x0c;   P5M0 = 0x0c;   //����P5.2��P5.3Ϊ©����·(ʵ��������������赽3.3V)
    P6M1 = 0xff;   P6M0 = 0xff;   //����Ϊ©����·(ʵ��������������赽3.3V)
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���

    delay_ms(10);
    UART2_config(2);    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
    
    EA = 1;                     //�����ж�
    
    PrintString2("��������:\r\n");
    PrintString2("E 0x001234            --> ��������  ʮ�����Ƶ�ַ\r\n");
    PrintString2("W 0x001234 1234567890 --> д�����  ʮ�����Ƶ�ַ  д������\r\n");
    PrintString2("R 0x001234 10         --> ��������  ʮ�����Ƶ�ַ  �����ֽ�����\r\n");
    PrintString2("C                     --> �����ⲻ��PM25LV040/W25X40CL/W25Q80BV, ����Cǿ���������.\r\n\r\n");

    SPI_init();
    FlashCheckID();
    FlashCheckID();
    
    if(!B_FlashOK)  PrintString2("δ��⵽PM25LV040/W25X40CL/W25Q80BV!\r\n");
    else
    {
        if(B_FlashOK == 1)
        {
            PrintString2("��⵽PM25LV040!\r\n");
        }
        else if(B_FlashOK == 2)
        {
            PrintString2("��⵽W25X40CL!\r\n");
        }
        else if(B_FlashOK == 3)
        {
            PrintString2("��⵽W25Q80BV!\r\n");
        }
        PrintString2("������ID1 = 0x");
        UART2_TxByte(Hex2Ascii(PM25LV040_ID1 >> 4));
        UART2_TxByte(Hex2Ascii(PM25LV040_ID1));
        PrintString2("\r\n      ID2 = 0x");
        UART2_TxByte(Hex2Ascii(PM25LV040_ID2 >> 4));
        UART2_TxByte(Hex2Ascii(PM25LV040_ID2));
        PrintString2("\r\n   �豸ID = 0x");
        UART2_TxByte(Hex2Ascii(PM25LV040_ID >> 4));
        UART2_TxByte(Hex2Ascii(PM25LV040_ID));
        PrintString2("\r\n");
    }

    while(1)
    {
        delay_ms(1);
                
        if(RX2_TimeOut > 0)
        {
            if(--RX2_TimeOut == 0)  //��ʱ,�򴮿ڽ��ս���
            {
                if(RX2_Cnt > 0)
                {
                    RX2_Check();    //����1��������
                }
                RX2_Cnt = 0;
            }
        }
    }
}


//========================================================================
// ����: void delay_ms(unsigned char ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~255ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2013-4-1
// ��ע: 
//========================================================================
void delay_ms(u8 ms)
{
     u16 i;
     do{
          i = MAIN_Fosc / 6000;
          while(--i);
     }while(--ms);
}

/**************** ASCII��תBIN ****************************/
u8  CheckData(u8 dat)
{
    if((dat >= '0') && (dat <= '9'))        return (dat-'0');
    if((dat >= 'A') && (dat <= 'F'))        return (dat-'A'+10);
    return 0xff;
}

/**************** ��ȡд���ַ ****************************/
u32 GetAddress(void)
{
    u32 address;
    u8  i,j;
    
    address = 0;
    if((RX2_Buffer[2] == '0') && (RX2_Buffer[3] == 'X'))
    {
        for(i=4; i<10; i++)
        {
            j = CheckData(RX2_Buffer[i]);
            if(j >= 0x10)   return 0x80000000;  //error
            address = (address << 4) + j;
        }
        return (address);
    }
    return  0x80000000; //error
}

/**************** ��ȡҪ�������ݵ��ֽ��� ****************************/
u8  GetDataLength(void)
{
    u8  i;
    u8  length;
    
    length = 0;
    for(i=11; i<RX2_Cnt; i++)
    {
        if(CheckData(RX2_Buffer[i]) >= 10)  break;
        length = length * 10 + CheckData(RX2_Buffer[i]);
    }
    return (length);
}

/**************** ����2������ ****************************/

void RX2_Check(void)
{
    u8  i,j;

    if((RX2_Cnt == 1) && (RX2_Buffer[0] == 'C'))    //����Cǿ���������
    {
        B_FlashOK = 1;
        PrintString2("ǿ���������FLASH!\r\n");
    }

    if(!B_FlashOK)
    {
        PrintString2("PM25LV040/W25X40CL/W25Q80BV������, ���ܲ���FLASH!\r\n");
        return;
    }
    
    F0 = 0;
    if((RX2_Cnt >= 10) && (RX2_Buffer[1] == ' '))   //�������Ϊ10���ֽ�
    {
        for(i=0; i<10; i++)
        {
            if((RX2_Buffer[i] >= 'a') && (RX2_Buffer[i] <= 'z'))    RX2_Buffer[i] = RX2_Buffer[i] - 'a' + 'A';//Сдת��д
        }
        Flash_addr = GetAddress();
        if(Flash_addr < 0x80000000)
        {
            if(RX2_Buffer[0] == 'E')    //����
            {
                FlashSectorErase(Flash_addr);
                PrintString2("�Ѳ�����ǰ��������!\r\n");
                F0 = 1;
            }

            else if((RX2_Buffer[0] == 'W') && (RX2_Cnt >= 12) && (RX2_Buffer[10] == ' '))   //д��N���ֽ�
            {
                j = RX2_Cnt - 11;
                for(i=0; i<j; i++)  tmp[i] = 0xff;      //���Ҫд��Ŀռ��Ƿ�Ϊ��
                i = SPI_Read_Compare(Flash_addr,tmp,j);
                if(i > 0)
                {
                    PrintString2("Ҫд��ĵ�ַΪ�ǿ�,����д��,���Ȳ���!\r\n");
                }
                else
                {
                    SPI_Write_Nbytes(Flash_addr,&RX2_Buffer[11],j);     //дN���ֽ� 
                    i = SPI_Read_Compare(Flash_addr,&RX2_Buffer[11],j); //�Ƚ�д�������
                    if(i == 0)
                    {
                        PrintString2("��д��");
                        if(j >= 100)    {UART2_TxByte((u8)(j/100+'0'));   j = j % 100;}
                        if(j >= 10)     {UART2_TxByte((u8)(j/10+'0'));    j = j % 10;}
                        UART2_TxByte((u8)(j%10+'0'));
                        PrintString2("�ֽ�����!\r\n");
                    }
                    else        PrintString2("д�����!\r\n");
                }
                F0 = 1;
            }
            else if((RX2_Buffer[0] == 'R') && (RX2_Cnt >= 12) && (RX2_Buffer[10] == ' '))   //����N���ֽ�
            {
                j = GetDataLength();
                if((j > 0) && (j < EE_BUF_LENGTH))
                {
                    SPI_Read_Nbytes(Flash_addr,tmp,j);
                    PrintString2("����");
                    if(j>=100)  UART2_TxByte((u8)(j/100+'0'));
                    UART2_TxByte((u8)(j%100/10+'0'));
                    UART2_TxByte((u8)(j%10+'0'));
                    PrintString2("���ֽ��������£�\r\n");
                    for(i=0; i<j; i++)  UART2_TxByte(tmp[i]);
                    UART2_TxByte(0x0d);
                    UART2_TxByte(0x0a);
                    F0 = 1;
                }
            }
        }
    }
    if(!F0) PrintString2("�������!\r\n");
}

//========================================================================
// ����: void UART2_TxByte(u8 dat)
// ����: ����һ���ֽ�.
// ����: ��.
// ����: ��.
// �汾: V1.0, 2014-6-30
//========================================================================

void UART2_TxByte(u8 dat)
{
    S2BUF = dat;
    B_TX2_Busy = 1;
    while(B_TX2_Busy);
}

//========================================================================
// ����: void PrintString2(u8 *puts)
// ����: ����2�����ַ���������
// ����: puts:  �ַ���ָ��.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void PrintString2(u8 *puts) //����һ���ַ���
{
    for (; *puts != 0;  puts++) UART2_TxByte(*puts);    //����ֹͣ��0����
}

//========================================================================
// ����: SetTimer2Baudraye(u16 dat)
// ����: ����Timer2�������ʷ�������
// ����: dat: Timer2����װֵ.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void SetTimer2Baudraye(u16 dat)  // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
{
    T2R = 0;		//Timer stop
    T2_CT = 0;	//Timer2 set As Timer
    T2x12 = 1;	//Timer2 set as 1T mode
    T2H = (u8)(dat / 256);
    T2L = (u8)(dat % 256);
    ET2 = 0;    //��ֹ�ж�
    T2R = 1;		//Timer run enable
}

//========================================================================
// ����: void UART2_config(u8 brt)
// ����: UART2��ʼ��������
// ����: brt: ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void UART2_config(u8 brt)    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
{
    if(brt == 2)
    {
        SetTimer2Baudraye((u16)(65536UL - (MAIN_Fosc / 4) / Baudrate2));

        S2CFG |= 0x01;     //ʹ�ô���2ʱ��W1λ��������Ϊ1��������ܻ��������Ԥ�ڵĴ���
        S2CON = (S2CON & 0x3f) | 0x40;    //UART2ģʽ, 0x00: ͬ����λ���, 0x40: 8λ����,�ɱ䲨����, 0x80: 9λ����,�̶�������, 0xc0: 9λ����,�ɱ䲨����
        ES2   = 1;         //�����ж�
        S2REN = 1;         //�������
        S2_S  = 1;         //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7

        B_TX2_Busy = 0;
        TX2_Cnt = 0;
        RX2_Cnt = 0;
    }
}

//========================================================================
// ����: void UART2_int (void) interrupt UART2_VECTOR
// ����: UART2�жϺ�����
// ����: nine.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void UART2_int (void) interrupt 8
{
    if(S2RI)
    {
        S2RI = 0;    //Clear Rx flag
        RX2_Buffer[RX2_Cnt] = S2BUF;
        if(++RX2_Cnt >= UART2_BUF_LENGTH)   RX2_Cnt = 0;
        RX2_TimeOut = 5;
    }

    if(S2TI)
    {
        S2TI = 0;    //Clear Tx flag
        B_TX2_Busy = 0;
    }
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

#define SPI_CE_High()   P_PM25LV040_CE  = 1     // set CE high
#define SPI_CE_Low()    P_PM25LV040_CE  = 0     // clear CE low
#define SPI_Hold()      P_SPI_Hold      = 0     // clear Hold pin
#define SPI_UnHold()    P_SPI_Hold      = 1     // set Hold pin
#define SPI_WP()        P_SPI_WP        = 0     // clear WP pin
#define SPI_UnWP()      P_SPI_WP        = 1     // set WP pin

/************************************************************************/
void SPI_Delay(u8 cnt)
{
    while(cnt--);
}

/************************************************************************/
void SPI_init(void)
{
    SPI_CE_High();
    P_PM25LV040_SCK = 0;    // set clock to low initial state
    P_PM25LV040_SI = 1;
}

/************************************************************************/
void SPI_WriteByte(u8 out)
{
    u8 i;
    i = 8;
    do{
        out <<= 1;
        P_PM25LV040_SI  = CY;
        P_PM25LV040_SCK = 1;
        SPI_Delay(3);
        P_PM25LV040_SCK = 0;
    }while(--i);
    P_PM25LV040_SI = 1;
}

/************************************************************************/
u8 SPI_ReadByte(void)
{
    u8 i, in;
    i = 8;
    do{
        in <<= 1;
        if (P_PM25LV040_SO) in++;
        P_PM25LV040_SCK = 1;
        SPI_Delay(3);
        P_PM25LV040_SCK = 0;
    }while(--i);
    return in;
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

    if((PM25LV040_ID1 == 0x9d) && (PM25LV040_ID2 == 0x7f))  B_FlashOK = 1;  //����Ƿ�ΪPM25LVxxϵ�е�Flash
    else if(PM25LV040_ID == 0x12)  B_FlashOK = 2;                           //����Ƿ�ΪW25X4xϵ�е�Flash
    else if(PM25LV040_ID == 0x13)  B_FlashOK = 3;                           //����Ƿ�ΪW25X8xϵ�е�Flash
    else B_FlashOK = 0;
}

/************************************************
���Flash��æ״̬
��ڲ���: ��
���ڲ���:
    0 : Flash���ڿ���״̬
    1 : Flash����æ״̬
************************************************/
u8  CheckFlashBusy(void)
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
/*
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
*/

/************************************************
��������, һ������4KB
��ڲ���: ��
���ڲ���: ��
************************************************/
void FlashSectorErase(u32 addr)
{
    if(B_FlashOK)
    {
        FlashWriteEnable();             //ʹ��Flashд����
        SPI_CE_Low();
        if(B_FlashOK == 1)
        {
            SPI_WriteByte(SFC_SECTORER1);    //����������������
        }
        else
        {
            SPI_WriteByte(SFC_SECTORER2);    //����������������
        }
        SPI_WriteByte(((u8 *)&addr)[1]);           //������ʼ��ַ
        SPI_WriteByte(((u8 *)&addr)[2]);
        SPI_WriteByte(((u8 *)&addr)[3]);
        SPI_CE_High();
    }
}

/************************************************
��Flash�ж�ȡ����
��ڲ���:
    addr   : ��ַ����
    buffer : �����Flash�ж�ȡ������
    size   : ���ݿ��С
���ڲ���:
    ��
************************************************/
void SPI_Read_Nbytes(u32 addr, u8 *buffer, u16 size)
{
    if(size == 0)   return;
    if(!B_FlashOK)  return;
    while(CheckFlashBusy() > 0);        //Flashæ���

    SPI_CE_Low();                       //enable device
    SPI_WriteByte(SFC_READ);            //read command

    SPI_WriteByte(((u8 *)&addr)[1]);    //������ʼ��ַ
    SPI_WriteByte(((u8 *)&addr)[2]);
    SPI_WriteByte(((u8 *)&addr)[3]);

    do{
        *buffer = SPI_ReadByte();       //receive byte and store at buffer
        buffer++;
    }while(--size);                     //read until no_bytes is reached
    SPI_CE_High();                      //disable device
}

/************************************************************************
����n���ֽ�,��ָ�������ݽ��бȽ�, ���󷵻�1,��ȷ����0
************************************************************************/
u8  SPI_Read_Compare(u32 addr, u8 *buffer, u16 size)
{
    u8  j;
    if(size == 0)   return 2;
    if(!B_FlashOK)  return 2;
    while(CheckFlashBusy() > 0);            //Flashæ���

    j = 0;
    SPI_CE_Low();                           //enable device
    SPI_WriteByte(SFC_READ);                //read command
    SPI_WriteByte(((u8 *)&addr)[1]);        //������ʼ��ַ
    SPI_WriteByte(((u8 *)&addr)[2]);
    SPI_WriteByte(((u8 *)&addr)[3]);
    do
    {
        if(*buffer != SPI_ReadByte())       //receive byte and store at buffer
        {
            j = 1;
            break;
        }
        buffer++;
    }while(--size);         //read until no_bytes is reached
    SPI_CE_High();          //disable device
    return j;
}

/************************************************
д���ݵ�Flash��
��ڲ���:
    addr   : ��ַ����
    buffer : ������Ҫд��Flash������
    size   : ���ݿ��С
���ڲ���: ��
************************************************/
void SPI_Write_Nbytes(u32 addr, u8 *buffer, u8 size)
{
    if(size == 0)   return;
    if(!B_FlashOK)  return;
    while(CheckFlashBusy() > 0);        //Flashæ���

    FlashWriteEnable();                 //ʹ��Flashд����

    SPI_CE_Low();                       // enable device
    SPI_WriteByte(SFC_PAGEPROG);        // ����ҳ�������
    SPI_WriteByte(((u8 *)&addr)[1]);    //������ʼ��ַ
    SPI_WriteByte(((u8 *)&addr)[2]);
    SPI_WriteByte(((u8 *)&addr)[3]);
    do{
        SPI_WriteByte(*buffer++);       //����ҳ��д
        addr++;
        if ((addr & 0xff) == 0) break;
    }while(--size);
    SPI_CE_High();                      // disable device
}

