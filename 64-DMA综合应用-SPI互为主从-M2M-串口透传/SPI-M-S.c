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

UART_DMA, M2M_DMA, SPI_DMA �ۺ�ʹ����ʾ����.

ͨ�����ڷ������ݸ�MCU1��MCU1�����յ���������SPI���͸�MCU2��MCU2��ͨ�����ڷ��ͳ�ȥ.

ͨ�����ڷ������ݸ�MCU2��MCU2�����յ���������SPI���͸�MCU1��MCU1��ͨ�����ڷ��ͳ�ȥ.

MCU1/MCU2: UART���� -> UART Rx DMA -> M2M -> SPI Tx DMA -> SPI����

MCU2/MCU1: SPI���� -> SPI Rx DMA -> M2M -> UART Tx DMA -> UART����

         MCU1                          MCU2
  |-----------------|           |-----------------|
  |            MISO |-----------| MISO            |
--| TX         MOSI |-----------| MOSI         TX |--
  |            SCLK |-----------| SCLK            |
--| RX           SS |-----------| SS           RX |--
  |-----------------|           |-----------------|


����ʱ, ѡ��ʱ�� 22.1184MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\comm\STC32G.h"
#include "intrins.h"
#include "stdio.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define     MAIN_Fosc       22118400L   //������ʱ�ӣ���ȷ����115200�����ʣ�

/*************  ���س�������    **************/

sbit    SPI_SS  = P2^2;
sbit    SPI_SI  = P2^3;
sbit    SPI_SO  = P2^4;
sbit    SPI_SCK = P2^5;

#define Baudrate2           115200L
#define BUF_LENGTH          107			//n+1

/*************  ���ر�������    **************/

bit	SpiTxFlag;
bit	SpiRxFlag;
bit	UartTxFlag;
bit	UartRxFlag;

bit	u2sFlag;    //UART to SPI
bit	s2uFlag;    //SPI to UART

bit	SpiSendFlag;
bit	UartSendFlag;

u8 xdata UartTxBuffer[256];
u8 xdata UartRxBuffer[256];
u8 xdata SpiTxBuffer[256];
u8 xdata SpiRxBuffer[256];

void UART2_config(u8 brt);   // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
void SPI_init(void);

void SPI_DMA_Config(void);
void SPI_DMA_Master(void);
void SPI_DMA_Slave(void);
void UART_DMA_Config(void);
void UART_DMA_Tx(void);
void UART_DMA_Rx(void);
void M2M_DMA_Config(void);
void M2M_UART_SPI(void);
void M2M_SPI_UART(void);

void UartPutc(unsigned char dat)
{
	S2BUF  = dat; 
	while(S2TI == 0);
	S2TI = 0;
}
 
char putchar(char c)
{
	UartPutc(c);
	return c;
}

/******************** ������ **************************/
void main(void)
{
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x30;   P0M0 = 0x30;   //����P0.4��P0.5Ϊ©����·(ʵ��������������赽3.3V)
    P1M1 = 0x32;   P1M0 = 0x32;   //����P1.1��P1.4��P1.5Ϊ©����·(ʵ��������������赽3.3V), P1.1��PWM��DAC��·ͨ�����贮����P2.3
    P2M1 = 0x3c;   P2M0 = 0x3c;   //����P2.2~P2.5Ϊ©����·(ʵ��������������赽3.3V)
    P3M1 = 0x50;   P3M0 = 0x50;   //����P3.4��P3.6Ϊ©����·(ʵ��������������赽3.3V)
    P4M1 = 0x3c;   P4M0 = 0x3c;   //����P4.2~P4.5Ϊ©����·(ʵ��������������赽3.3V)
    P5M1 = 0x0c;   P5M0 = 0x0c;   //����P5.2��P5.3Ϊ©����·(ʵ��������������赽3.3V)
    P6M1 = 0xff;   P6M0 = 0xff;   //����Ϊ©����·(ʵ��������������赽3.3V)
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���

	UART2_config(2);    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
	printf("STC32G UART2-DMA-SPI��Ϊ����͸������.\r\n");

	SPI_init();
	SPI_DMA_Config();
	UART_DMA_Config();
	M2M_DMA_Config();
	EA = 1;     //�������ж�
	
	while (1)
	{
		//UART���� -> UART DMA -> SPI DMA -> SPI����
		if(UartRxFlag)
		{
			UartRxFlag = 0;
			u2sFlag = 1;
			M2M_UART_SPI();			//UART Memory -> SPI Memory
		}

		if(SpiSendFlag)
		{
			SpiSendFlag = 0;
			UART_DMA_Rx();			//UART Recive Continue
			SPI_DMA_Master();		//SPI Send Memory
		}

		if(SpiTxFlag)
		{
			SpiTxFlag = 0;
			SPI_DMA_Slave();		//SPI Slave Mode
		}

		
		//SPI���� -> SPI DMA -> UART DMA -> UART����
		if(SpiRxFlag)
		{
			SpiRxFlag = 0;
			s2uFlag = 1;
			M2M_SPI_UART();			//SPI Memory -> UART Memory
		}

		if(UartSendFlag)
		{
			UartSendFlag = 0;
			SPI_DMA_Slave();		//SPI Slave Mode
			UART_DMA_Tx();			//UART Send Memory
		}
	}
}

//========================================================================
// ����: void SPI_DMA_Config(void)
// ����: SPI DMA ��������.
// ����: none.
// ����: none.
// �汾: V1.0, 2021-5-6
//========================================================================
void SPI_DMA_Config(void)
{
	DMA_SPI_STA = 0x00;
	DMA_SPI_CFG = 0xe0;		//bit7 1:Enable Interrupt
	DMA_SPI_AMT = BUF_LENGTH;		//���ô������ֽ���(��8λ)��n+1
	DMA_SPI_AMTH = 0x00;		//���ô������ֽ���(��8λ)��n+1
	DMA_SPI_TXAH = (u8)((u16)&SpiTxBuffer >> 8);	//SPI�������ݴ洢��ַ
	DMA_SPI_TXAL = (u8)((u16)&SpiTxBuffer);
	DMA_SPI_RXAH = (u8)((u16)&SpiRxBuffer >> 8);	//SPI�������ݴ洢��ַ
	DMA_SPI_RXAL = (u8)((u16)&SpiRxBuffer);
	DMA_SPI_CFG2 = 0x01;	//01:SS-P2.2
	DMA_SPI_CR = 0xa1;		//bit7 1:ʹ�� SPI_DMA, bit6 1:��ʼ SPI_DMA ����ģʽ,  bit5 1:��ʼ SPI_DMA �ӻ�ģʽ, bit0 1:��� SPI_DMA FIFO
}

void SPI_DMA_Master(void)
{
	DMA_SPI_CR = 0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	SPI_SS = 0;
	SPCTL = 0xd2;   //ʹ�� SPI ����ģʽ������SS���Ź���
	DMA_SPI_CR = 0xc0;		//bit7 1:ʹ�� SPI_DMA, bit6 1:��ʼ SPI_DMA ����ģʽ,  bit5 1:��ʼ SPI_DMA �ӻ�ģʽ, bit0 1:��� SPI_DMA FIFO
}

void SPI_DMA_Slave(void)
{
	DMA_SPI_CR = 0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	SPCTL = 0x42;  //��������Ϊ�ӻ�����
	DMA_SPI_CR = 0xa0;		//bit7 1:ʹ�� SPI_DMA, bit6 1:��ʼ SPI_DMA ����ģʽ,  bit5 1:��ʼ SPI_DMA �ӻ�ģʽ, bit0 1:��� SPI_DMA FIFO
}

//========================================================================
// ����: void UART_DMA_Config(void)
// ����: UART DMA ��������.
// ����: none.
// ����: none.
// �汾: V1.0, 2021-5-6
//========================================================================
void UART_DMA_Config(void)
{
	DMA_UR2T_CFG = 0x80;		//bit7 1:Enable Interrupt
	DMA_UR2T_STA = 0x00;
	DMA_UR2T_AMT = BUF_LENGTH;	//���ô������ֽ���(��8λ)��n+1
	DMA_UR2T_AMTH = 0x00;		//���ô������ֽ���(��8λ)��n+1
	DMA_UR2T_TXAH = (u8)((u16)&UartTxBuffer >> 8);
	DMA_UR2T_TXAL = (u8)((u16)&UartTxBuffer);
	DMA_UR2T_CR = 0x80;			//bit7 1:ʹ�� UART2_DMA, bit6 1:��ʼ UART2_DMA �Զ�����

	DMA_UR2R_CFG = 0x80;		//bit7 1:Enable Interrupt
	DMA_UR2R_STA = 0x00;
	DMA_UR2R_AMT = BUF_LENGTH;	//���ô������ֽ���(��8λ)��n+1
	DMA_UR2R_AMTH = 0x00;		//���ô������ֽ���(��8λ)��n+1
	DMA_UR2R_RXAH = (u8)((u16)&UartRxBuffer >> 8);
	DMA_UR2R_RXAL = (u8)((u16)&UartRxBuffer);
	DMA_UR2R_CR = 0xa1;			//bit7 1:ʹ�� UART2_DMA, bit5 1:��ʼ UART2_DMA �Զ�����, bit0 1:��� FIFO
}

void UART_DMA_Tx(void)
{
	DMA_UR2T_CR = 0xc0;			//bit7 1:ʹ�� UART2_DMA, bit6 1:��ʼ UART2_DMA �Զ�����
}

void UART_DMA_Rx(void)
{
	DMA_UR2R_CR = 0xa0;			//bit7 1:ʹ�� UART2_DMA, bit5 1:��ʼ UART2_DMA �Զ�����, bit0 1:��� FIFO
}

//========================================================================
// ����: void M2M_DMA_Config(void)
// ����: M2M DMA ��������.
// ����: none.
// ����: none.
// �汾: V1.0, 2021-5-6
//========================================================================
void M2M_DMA_Config(void)
{
	DMA_M2M_CFG = 0x80;   //r++ = t++
	DMA_M2M_STA = 0x00;
	DMA_M2M_AMT = BUF_LENGTH;		//���ô������ֽ���(��8λ)��n+1
	DMA_M2M_AMTH = 0x00;		//���ô������ֽ���(��8λ)��n+1
	DMA_M2M_TXAH = (u8)((u16)&UartRxBuffer >> 8);
	DMA_M2M_TXAL = (u8)((u16)&UartRxBuffer);
	DMA_M2M_RXAH = (u8)((u16)&SpiTxBuffer >> 8);
	DMA_M2M_RXAL = (u8)((u16)&SpiTxBuffer);
	DMA_M2M_CR = 0x80;			//bit7 1:ʹ�� M2M_DMA, bit6 1:��ʼ M2M_DMA �Զ�����
}

void M2M_UART_SPI(void)
{
	DMA_M2M_STA = 0x00;
	DMA_M2M_TXAH = (u8)((u16)&UartRxBuffer >> 8);
	DMA_M2M_TXAL = (u8)((u16)&UartRxBuffer);
	DMA_M2M_RXAH = (u8)((u16)&SpiTxBuffer >> 8);
	DMA_M2M_RXAL = (u8)((u16)&SpiTxBuffer);
	DMA_M2M_CR = 0xc0;			//bit7 1:ʹ�� M2M_DMA, bit6 1:��ʼ M2M_DMA �Զ�����
}

void M2M_SPI_UART(void)
{
	DMA_M2M_STA = 0x00;
	DMA_M2M_TXAH = (u8)((u16)&SpiRxBuffer >> 8);
	DMA_M2M_TXAL = (u8)((u16)&SpiRxBuffer);
	DMA_M2M_RXAH = (u8)((u16)&UartTxBuffer >> 8);
	DMA_M2M_RXAL = (u8)((u16)&UartTxBuffer);
	DMA_M2M_CR = 0xc0;			//bit7 1:ʹ�� M2M_DMA, bit6 1:��ʼ M2M_DMA �Զ�����
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
void SetTimer2Baudraye(u16 dat)  // ʹ��Timer2��������.
{
    T2R = 0;	//Timer stop
    T2_CT = 0;	//Timer2 set As Timer
    T2x12 = 1;	//Timer2 set as 1T mode
    T2H = dat / 256;
    T2L = dat % 256;
    ET2 = 0;    //��ֹ�ж�
    T2R = 1;	//Timer run enable
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
    /*********** �����ʹ̶�ʹ�ö�ʱ��2 *****************/
    if(brt == 2)
    {
        SetTimer2Baudraye((u16)(65536UL - (MAIN_Fosc / 4) / Baudrate2));

        S2CFG |= 0x01;     //ʹ�ô���2ʱ��W1λ��������Ϊ1��������ܻ��������Ԥ�ڵĴ���
        S2CON = (S2CON & 0x3f) | 0x40; 
//        ES2   = 1;         //�����ж�
        S2REN = 1;         //�������
        S2_S  = 1;         //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
    }
}

//========================================================================
// ����: void SPI_init(void)
// ����: SPI
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void SPI_init(void)
{
	SPI_SS = 1;
	SPCTL = 0x42;         //ʹ�� SPI �ӻ�ģʽ���д���
	P_SW1 = (P_SW1 & ~(3<<2)) | (1<<2);  //IO���л�. 0: P1.2/P5.4 P1.3 P1.4 P1.5, 1: P2.2 P2.3 P2.4 P2.5, 2: P5.4 P4.0 P4.1 P4.3, 3: P3.5 P3.4 P3.3 P3.2
}

//========================================================================
// ����: void DMA_Interrupt (void) interrupt 13
// ����: DMA�жϺ���
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2021-5-8
// ��ע: 
//========================================================================
void DMA_Interrupt(void) interrupt 13
{
	if(DMA_SPI_STA & 0x01)	//ͨ�����
	{
		DMA_SPI_STA &= ~0x01;
		if(SPCTL & 0x10) 
		{ //����ģʽ
			SpiTxFlag = 1;
			SPI_SS = 1;
		}
		else 
		{ //�ӻ�ģʽ
			SpiRxFlag = 1;
		}
	}
	if(DMA_SPI_STA & 0x02)	//���ݶ���
	{
		DMA_SPI_STA &= ~0x02;
	}
	if(DMA_SPI_STA & 0x04)	//���ݸ���
	{
		DMA_SPI_STA &= ~0x04;
	}

	if(DMA_UR2T_STA & 0x01)	//�������
	{
		DMA_UR2T_STA &= ~0x01;
		UartTxFlag = 1;
	}
	if(DMA_UR2T_STA & 0x04)	//���ݸ���
	{
		DMA_UR2T_STA &= ~0x04;
	}
	
	if(DMA_UR2R_STA & 0x01)	//�������
	{
		DMA_UR2R_STA &= ~0x01;
		UartRxFlag = 1;
	}
	if(DMA_UR2R_STA & 0x02)	//���ݶ���
	{
		DMA_UR2R_STA &= ~0x02;
	}

	if(DMA_M2M_STA & 0x01)	//�������
	{
		DMA_M2M_STA &= ~0x01;
		if(u2sFlag)
		{
			u2sFlag = 0;
			SpiSendFlag = 1;
		}
		if(s2uFlag)
		{
			s2uFlag = 0;
			UartSendFlag = 1;
		}
	}
}
