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

USART����Lin�ӻ������շ�����������Ĭ��ͨ�Žţ�P4.3 P4.4.

�յ�һ���Ǳ���Ӧ�������֡��ͨ������2���, �����浽���ݻ���.

�յ�һ������Ӧ���֡ͷ��(���磺ID=0x12), ���;ݽ���Ӧ��.

3����û�յ��µ����ݣ�MCU��������״̬��ͨ��LIN���߻���.

Ĭ�ϴ������ʣ�9600������, �û��������޸�.

����ʱ, Ĭ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\comm\STC32G.h"
#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL
#define Baudrate         9600UL
#define TM               (65536 -(MAIN_Fosc/Baudrate/4))
#define Timer0_Reload    (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

/*****************************************************************************/

sbit SLP_N  = P4^1;     //0: Sleep

/*************  ���س�������    **************/

#define FRAME_LEN           8    //���ݳ���: 8 �ֽ�
#define UART1_BUF_LENGTH    64

/*************  ���ر�������    **************/

u8 Lin_ID;
u8 TX_BUF[8];

bit B_1ms;      //1ms��־
u16 msSecond;

bit B_RX1_Flag; //���ձ�־
bit B_TX1_Busy; //����æ��־
u8  TX1_Cnt;    //���ͼ���
u8  RX1_Cnt;    //���ռ���
u8  RX1_TimeOut;

u8  RX1_Buffer[UART1_BUF_LENGTH]; //���ջ���
u8  tmp_Buffer[16]; //���ջ���

/*************  ���غ�������    **************/

void LinInit();
void LinSendData(u8 *pdat);
void LINSendChecksum(u8 *dat);
void LinSetBaudrate(u16 brt);

/******************** ���ڴ�ӡ���� ********************/
void UartInit(void)
{
	S2_S = 1;       //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
    S2CFG |= 0x01;  //ʹ�ô���2ʱ��W1λ��������Ϊ1��������ܻ��������Ԥ�ڵĴ���
	S2CON = (S2CON & 0x3f) | 0x40;
	T2L  = TM;
	T2H  = TM>>8;
	AUXR |= 0x14;   //��ʱ��2ʱ��1Tģʽ,��ʼ��ʱ
}

void UartPutc(unsigned char dat)
{
	S2BUF  = dat; 
	while(S2TI == 0);
	S2TI = 0;    //Clear Tx flag
}

char putchar(char c)
{
	UartPutc(c);
	return c;
}

/********************* ������ *************************/
void main(void)
{
	u8 i;
	
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

	AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
	TH0 = (u8)(Timer0_Reload / 256);
	TL0 = (u8)(Timer0_Reload % 256);
	ET0 = 1;    //Timer0 interrupt enable
	TR0 = 1;    //Tiner0 run

	Lin_ID = 0x32;
	UartInit();
	LinInit();
	
	EA = 1;                 //�����ж�

	printf("STC32G USART to LIN Test Programme!\r\n");

	SLP_N = 1;
	TX_BUF[0] = 0x81;
	TX_BUF[1] = 0x22;
	TX_BUF[2] = 0x33;
	TX_BUF[3] = 0x44;
	TX_BUF[4] = 0x55;
	TX_BUF[5] = 0x66;
	TX_BUF[6] = 0x77;
	TX_BUF[7] = 0x88;
			
	while(1)
	{
		if((B_RX1_Flag) && (RX1_Cnt >= 2))
		{
			B_RX1_Flag = 0;

			if((RX1_Buffer[0] == 0x55) && ((RX1_Buffer[1] & 0x3f) == 0x12)) //PID -> ID
			{
				LinSendData(TX_BUF);
				LINSendChecksum(TX_BUF);
			}
		}

		if(B_1ms)   //1ms��
		{
			B_1ms = 0;
            msSecond++;
            if(msSecond >= 3000) //3����û�յ����ݽ�����
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

			if(RX1_TimeOut > 0)     //��ʱ����
			{
				if(--RX1_TimeOut == 0)
				{
                    msSecond = 0;
					printf("Read Cnt = %d.\r\n",RX1_Cnt);
					for(i=0; i<RX1_Cnt; i++)    printf("0x%02x ",RX1_Buffer[i]);    //�Ӵ�������յ��Ĵӻ�����
					RX1_Cnt  = 0;   //����ֽ���
					printf("\r\n");
				}
			}
		}
	}
}

/********************** Timer0 1ms�жϺ��� ************************/
void timer0 (void) interrupt 1
{
    B_1ms = 1;      //1ms��־
}

//========================================================================
// ����: void LinSendByte(u8 dat)
// ����: ����һ���ֽں�����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2021-10-28
// ��ע: 
//========================================================================
void LinSendByte(u8 dat)
{
	B_TX1_Busy = 1;
	SBUF = dat;
	while(B_TX1_Busy);
}

//========================================================================
// ����: u16 LinSendData(u8 *pdat)
// ����: Lin�������ݺ�����
// ����: *pdat: �������ݻ�����.
// ����: Lin ID.
// �汾: VER1.0
// ����: 2021-10-28
// ��ע: 
//========================================================================
void LinSendData(u8 *pdat)
{
	u8 i;

	for(i=0;i<FRAME_LEN;i++)
	{
		LinSendByte(pdat[i]);
	}
}

//========================================================================
// ����: void LINSendChecksum(u8 *dat)
// ����: ����У���벢���͡�
// ����: ���ݳ����������.
// ����: none.
// �汾: VER1.0
// ����: 2020-12-2
// ��ע: 
//========================================================================
void LINSendChecksum(u8 *dat)
{
    u16 sum = 0;
    u8 i;

    for(i = 0; i < FRAME_LEN; i++)
    {
        sum += dat[i];
        if(sum & 0xFF00)
        {
            sum = (sum & 0x00FF) + 1;
        }
    }
    sum ^= 0x00FF;
	LinSendByte((u8)sum);
}

//========================================================================
// ����: void LinSetBaudrate(u16 brt)
// ����: Lin���߲��������ú�����
// ����: brt: ������.
// ����: none.
// �汾: VER1.0
// ����: 2021-10-28
// ��ע: 
//========================================================================
void LinSetBaudrate(u16 brt)
{
	u16 tmp;
	
	tmp = (MAIN_Fosc >> 4) / brt;
	USARTBRH = (u8)(tmp>>8);
	USARTBRL = (u8)tmp;
}

//========================================================================
// ����: void LinInit()
// ����: Lin��ʼ��������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2021-01-05
// ��ע: 
//========================================================================
void LinInit()
{
    P_SW1 = (P_SW1 & 0x3f) | 0xc0;  //USART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4

	SCON = (SCON & 0x3f) | 0x40;    //USART1ģʽ, 0x00: ͬ����λ���, 0x40: 8λ����,�ɱ䲨����, 0x80: 9λ����,�̶�������, 0xc0: 9λ����,�ɱ䲨����
	TI = 0;
	REN = 1;    //�������
	ES  = 1;    //�����ж�

	LinSetBaudrate(9600);	//���ò�����
	SMOD = 1;				//����1�����ʷ���
	USARTCR1 |= 0x80;		//LIN Mode Enable
	USARTCR5 |= 0x20;		//LIN Slave Mode

	USARTCR5 |= 0x10;		//AutoSync
}

//========================================================================
// ����: void USART1_int (void) interrupt UART1_VECTOR
// ����: USART1�жϺ�����
// ����: nine.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void USART1_int (void) interrupt 4
{
	u8 Status;

	if(RI)
	{
		RI = 0;    //Clear Rx flag

		Status = USARTCR5;
		if(Status & 0x02)     //if LIN header is detected
		{
			B_RX1_Flag = 1;
		}

		if(Status & 0xc0)     //if LIN break is detected / LIN header error is detected
		{
			RX1_Cnt = 0;
		}

		USARTCR5 &= ~0xcb;    //Clear flag
		RX1_Buffer[RX1_Cnt++] = SBUF;
		if(RX1_Cnt >= UART1_BUF_LENGTH)   RX1_Cnt = 0;
		RX1_TimeOut = 5;
	}

	if(TI)
	{
		TI = 0;    //Clear Tx flag
		B_TX1_Busy = 0;
	}
}
