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


/*************  ��������˵��  **************

�����̻���STC32GΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

���ڷ�ָ��ͨ��I2C DMA��дAT24C02����.

Ĭ�ϲ�����:  115200,8,N,1. 

������������: (��ĸ�����ִ�Сд)
    W 0x12 1234567890 --> д�����  ʮ�����Ƶ�ַ  д������.
    R 0x12 10         --> ��������  ʮ�����Ƶ�ַ  �����ֽ���.

����ʱ, ѡ��ʱ�� 22.1184MHz (�û��������޸�Ƶ��).

******************************************/


#include "..\..\comm\STC32G.h"
#include "intrins.h"
#include "stdio.h"

#define     MAIN_Fosc       22118400L   //������ʱ�ӣ���ȷ����115200�����ʣ�

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;


/****************************** �û������ ***********************************/

#define Baudrate        115200L
#define TM              (65536 -(MAIN_Fosc/Baudrate/4))

/*****************************************************************************/


/*************  ���س�������    **************/

#define EE_BUF_LENGTH       255          //
#define UART2_BUF_LENGTH    (EE_BUF_LENGTH+7)   //���ڻ��峤��

#define SLAW    0xA0
#define SLAR    0xA1

/*************  ���ر�������    **************/

u8 EEPROM_addr;
u8 xdata DmaTxBuffer[256];
u8 xdata DmaRxBuffer[256];

u8  RX2_TimeOut;
u16 RX2_Cnt;    //���ռ���
bit B_TX2_Busy; //����æ��־
bit	DmaTxFlag=0;
bit	DmaRxFlag=0;

u8  RX2_Buffer[UART2_BUF_LENGTH];   //���ջ���

/*************  ���غ�������    **************/

void I2C_init(void);
void WriteNbyte(u8 addr, u8 number);
void ReadNbyte( u8 addr, u8 number);
void delay_ms(u8 ms);
void RX2_Check(void);
void DMA_Config(void);

/******************** ���ڴ�ӡ���� ********************/
void UartInit(void)
{
    S2_S  = 1;          //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
    S2CFG |= 0x01;      //ʹ�ô���2ʱ��W1λ��������Ϊ1��������ܻ��������Ԥ�ڵĴ���
	S2CON = (S2CON & 0x3f) | 0x40; 
	T2L  = TM;
	T2H  = TM>>8;
	AUXR |= 0x14;       //��ʱ��2ʱ��1Tģʽ,��ʼ��ʱ
    S2REN = 1;          //�������
    ES2   = 1;          //�����ж�
}

void UartPutc(unsigned char dat)
{
	S2BUF = dat; 
	B_TX2_Busy = 1;
	while(B_TX2_Busy);
}
 
char putchar(char c)
{
	UartPutc(c);
	return c;
}

/**********************************************/
void main(void)
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
    
    I2C_init();
    UartInit();
    DMA_Config();
    EA = 1;     //�������ж�

    printf("��������:\r\n");
    printf("W 0x12 1234567890 --> д�����  ʮ�����Ƶ�ַ  д������\r\n");
    printf("R 0x12 10         --> ��������  ʮ�����Ƶ�ַ  �����ֽ�����\r\n");

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
// ����: void delay_ms(u8 ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~255ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2021-3-9
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
u8 CheckData(u8 dat)
{
    if((dat >= '0') && (dat <= '9'))        return (dat-'0');
    if((dat >= 'A') && (dat <= 'F'))        return (dat-'A'+10);
    return 0xff;
}

/**************** ��ȡд���ַ ****************************/
u8 GetAddress(void)
{
    u8 address;
    u8  i,j;
    
    address = 0;
    if((RX2_Buffer[2] == '0') && (RX2_Buffer[3] == 'X'))
    {
        for(i=4; i<6; i++)
        {
            j = CheckData(RX2_Buffer[i]);
            if(j >= 0x10)   return 0;  //error
            address = (address << 4) + j;
        }
        return (address);
    }
    return  0; //error
}

/**************** ��ȡҪ�������ݵ��ֽ��� ****************************/
u8  GetDataLength(void)
{
    u8  i;
    u8  length;
    
    length = 0;
    for(i=7; i<RX2_Cnt; i++)
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

    F0 = 0;
    if((RX2_Cnt >= 8) && (RX2_Buffer[1] == ' '))   //�������Ϊ8���ֽ�
    {
        for(i=0; i<6; i++)
        {
            if((RX2_Buffer[i] >= 'a') && (RX2_Buffer[i] <= 'z'))    RX2_Buffer[i] = RX2_Buffer[i] - 'a' + 'A';//Сдת��д
        }
        EEPROM_addr = GetAddress();
        if(EEPROM_addr <= 255)
        {
            if((RX2_Buffer[0] == 'W') && (RX2_Cnt >= 8) && (RX2_Buffer[6] == ' '))   //д��N���ֽ�
            {
                j = RX2_Cnt - 7;
                
                for(i=0; i<j; i++)  DmaTxBuffer[i+2] = RX2_Buffer[i+7];
                WriteNbyte(EEPROM_addr, j);     //дN���ֽ� 
                printf("��д��%d�ֽ�����!\r\n",j);
                delay_ms(5);

                ReadNbyte(EEPROM_addr, j);
                printf("����%d���ֽ��������£�\r\n",j);
                for(i=0; i<j; i++)    printf("%c", DmaRxBuffer[i]);
                printf("\r\n");
                
                F0 = 1;
            }
            else if((RX2_Buffer[0] == 'R') && (RX2_Cnt >= 8) && (RX2_Buffer[6] == ' '))   //����N���ֽ�
            {
                j = GetDataLength();
                if((j > 0) && (j <= EE_BUF_LENGTH))
                {
                    ReadNbyte(EEPROM_addr, j);
                    printf("����%d���ֽ��������£�\r\n",j);
                    for(i=0; i<j; i++)    printf("%c", DmaRxBuffer[i]);
                    printf("\r\n");
                    F0 = 1;
                }
            }
        }
    }
    if(!F0) printf("�������!\r\n");
}

//========================================================================
// ����: void DMA_Config(void)
// ����: I2C DMA ��������.
// ����: none.
// ����: none.
// �汾: V1.0, 2021-5-6
//========================================================================
void DMA_Config(void)
{
	DMA_I2CT_STA = 0x00;
	DMA_I2CT_CFG = 0x80;	//bit7 1:Enable Interrupt
	DMA_I2CT_AMT = 0xff;	//���ô������ֽ���(��8λ)��n+1
	DMA_I2CT_AMTH = 0x00;	//���ô������ֽ���(��8λ)��n+1
	DMA_I2CT_TXAH = (u8)((u16)&DmaTxBuffer >> 8);	//I2C�������ݴ洢��ַ
	DMA_I2CT_TXAL = (u8)((u16)&DmaTxBuffer);
	DMA_I2CT_CR = 0x80;		//bit7 1:ʹ�� I2CT_DMA, bit6 1:��ʼ I2CT_DMA

	DMA_I2CR_STA = 0x00;
	DMA_I2CR_CFG = 0x80;	//bit7 1:Enable Interrupt
	DMA_I2CR_AMT = 0xff;	//���ô������ֽ���(��8λ)��n+1
	DMA_I2CR_AMTH = 0x00;	//���ô������ֽ���(��8λ)��n+1
	DMA_I2CR_RXAH = (u8)((u16)&DmaRxBuffer >> 8);	//I2C�������ݴ洢��ַ
	DMA_I2CR_RXAL = (u8)((u16)&DmaRxBuffer);
	DMA_I2CR_CR = 0x81;		//bit7 1:ʹ�� I2CT_DMA, bit5 1:��ʼ I2CT_DMA, bit0 1:��� FIFO

	DMA_I2C_ST1 = 0xff;		//������Ҫ�����ֽ���(��8λ)��n+1
	DMA_I2C_ST2 = 0x00;		//������Ҫ�����ֽ���(��8λ)��n+1
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

/********************** I2C���� ************************/
void I2C_init(void)
{
    P_SW2 = (P_SW2 & ~(3<<4)) | (1<<4);     //IO���л�. 0: P1.4 P1.5, 1: P2.4 P2.5, 3: P3.3 P3.2
    I2CCFG = 0xe0;              //ʹ��I2C����ģʽ
    I2CMSST = 0x00;
}

void WriteNbyte(u8 addr, u8 number)  /*  WordAddress,First Data Address,Byte lenth   */
{
    while(I2CMSST & 0x80);    //���I2C������æµ״̬

    DmaTxFlag = 1;
    DmaTxBuffer[0] = SLAW;
    DmaTxBuffer[1] = addr;

    I2CMSST = 0x00;
    I2CMSCR = 0x89;             //set cmd //write_start_combo
    DMA_I2C_CR = 0x01;
    DMA_I2CT_AMT = number+1;	//���ô������ֽ���(��8λ)��number + �豸��ַ + �洢��ַ
    DMA_I2CT_AMTH = 0x00;		//���ô������ֽ���(��8λ)��n+1
    DMA_I2C_ST1 = number+1;		//������Ҫ�����ֽ���(��8λ)��number + �豸��ַ + �洢��ַ
    DMA_I2C_ST2 = 0x00;			//������Ҫ�����ֽ���(��8λ)��n+1
    DMA_I2CT_CR |= 0x40;		//bit7 1:ʹ�� I2CT_DMA, bit6 1:��ʼ I2CT_DMA

    while(DmaTxFlag);         //DMAæ���
    DMA_I2C_CR = 0x00;
}

void ReadNbyte(u8 addr, u8 number)   /*  WordAddress,First Data Address,Byte lenth   */
{
    while(I2CMSST & 0x80);    //���I2C������æµ״̬
    DMA_I2C_CR = 0x00;
    I2CMSST = 0x00;

    //������ʼ�ź�+�豸��ַ+д�ź�
    I2CTXD = SLAW;
    I2CMSCR = 0x09;
    while ((I2CMSST & 0x40) == 0);
    I2CMSST = 0x00;

    //���ʹ洢����ַ
    I2CTXD = addr;
    I2CMSCR = 0x0a;
    while ((I2CMSST & 0x40) == 0);
    I2CMSST = 0x00;
    
    //������ʼ�ź�+�豸��ַ+���ź�
    I2CTXD = SLAR;
    I2CMSCR = 0x09;
    while ((I2CMSST & 0x40) == 0);
    I2CMSST = 0x00;

    DmaRxFlag = 1;
   //�������ݶ�ȡ����
    I2CMSCR = 0x8b;
    DMA_I2C_CR = 0x01;

    DMA_I2CR_AMT = number-1;	//���ô������ֽ���(��8λ)��n+1
    DMA_I2CR_AMTH = 0x00;			//���ô������ֽ���(��8λ)��n+1
    DMA_I2C_ST1 = number-1;		//������Ҫ�����ֽ���(��8λ)��number + �豸��ַ + �洢��ַ
    DMA_I2C_ST2 = 0x00;				//������Ҫ�����ֽ���(��8λ)��n+1
    DMA_I2CR_CR |= 0x40;			//bit7 1:ʹ�� I2CT_DMA, bit5 1:��ʼ I2CT_DMA, bit0 1:��� FIFO
    while(DmaRxFlag);         //DMAæ���
    DMA_I2C_CR = 0x00;
}

//========================================================================
// ����: void I2C_DMA_Interrupt (void) interrupt 60/61
// ����: I2C DMA�жϺ���
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2021-5-8
// ��ע: 
//========================================================================
void I2C_DMA_Interrupt(void) interrupt 13
{
	if(DMA_I2CT_STA & 0x01)   //�������
	{
		DMA_I2CT_STA &= ~0x01;  //�����־λ
		DmaTxFlag = 0;
	}
	if(DMA_I2CT_STA & 0x04)   //���ݸ���
	{
		DMA_I2CT_STA &= ~0x04;  //�����־λ
	}
	
	if(DMA_I2CR_STA & 0x01)   //�������
	{
		DMA_I2CR_STA &= ~0x01;  //�����־λ
		DmaRxFlag = 0;
	}
	if(DMA_I2CR_STA & 0x02)   //���ݶ���
	{
		DMA_I2CR_STA &= ~0x02;  //�����־λ
	}
}

//========================================================================
// ����: void I2C_Interrupt (void) interrupt 24
// ����: I2C �жϺ���
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-3-18
// ��ע: 
//========================================================================
void I2C_Interrupt() interrupt 24
{
	I2CMSST &= ~0x40;       //I2Cָ������״̬���

	if(DMA_I2C_CR & 0x04)   //ACKERR
	{
		DMA_I2C_CR &= ~0x04;  //�����ݺ��յ�NAK
	}
}

