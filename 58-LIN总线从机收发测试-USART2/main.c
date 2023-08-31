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

USART����Lin�ӻ������շ�����������Ĭ��ͨ�Žţ�P4.6 P4.7.

�յ�һ���Ǳ���Ӧ�������֡��ͨ������1���, �����浽���ݻ���.

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

sbit SLP_N  = P5^0;     //0: Sleep

/*************  ���س�������    **************/

#define FRAME_LEN           8    //���ݳ���: 8 �ֽ�
#define UART2_BUF_LENGTH    16

/*************  ���ر�������    **************/

u8 StaCR;
u8 Lin_ID;
u8 TX_BUF[8];

bit B_1ms;      //1ms��־
u16 msSecond;

bit B_RX2_Flag; //���ձ�־
bit B_TX2_Busy; //����æ��־
u8  TX2_Cnt;    //���ͼ���
u8  RX2_Cnt;    //���ռ���
u8 RX2_TimeOut;

u8  RX2_Buffer[UART2_BUF_LENGTH]; //���ջ���

/*************  ���غ�������    **************/

void LinInit(void);
void LinStatusRead(void);
void LinSendData(u8 *pdat);
void LINSendChecksum(u8 *dat);

/******************** ���ڴ�ӡ���� ********************/
void UartInit(void)
{
	P_SW1 = (P_SW1 & 0x3f) | 0x00;    //USART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
	SCON = (SCON & 0x3f) | 0x40; 
	T1x12 = 1;          //��ʱ��ʱ��1Tģʽ
	S1BRT = 0;          //����1ѡ��ʱ��1Ϊ�����ʷ�����
	TL1 = TM;
	TH1 = TM>>8;
	TR1 = 1;			//��ʱ��1��ʼ��ʱ
}

void UartPutc(unsigned char dat)
{
	SBUF = dat; 
	while(TI==0);
	TI = 0;
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

	printf("STC32G USART2 to LIN Test Programme!\r\n");

	SLP_N = 1;
	TX_BUF[0] = 0x81;
	TX_BUF[1] = 0x22;
	TX_BUF[2] = 0x33;
	TX_BUF[3] = 0x44;
	TX_BUF[4] = 0x55;
	TX_BUF[5] = 0x66;
	TX_BUF[6] = 0x77;
	TX_BUF[7] = 0x88;
	
    P40 = 0;  //LED Power on
	while(1)
	{
		if((B_RX2_Flag) && (RX2_Cnt >= 2))
		{
			B_RX2_Flag = 0;

			if((RX2_Buffer[0] == 0x55) && ((RX2_Buffer[1] & 0x3f) == 0x12)) //PID -> ID
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
                P67 = 1;    //LED17 OFF
                PD = 1;     //Sleep
                _nop_();
                _nop_();
                _nop_();
                _nop_();
                _nop_();
                _nop_();
                _nop_();
                P67 = 0;    //LED17 ON
            }

			if(RX2_TimeOut > 0)     //��ʱ����
			{
				if(--RX2_TimeOut == 0)
				{
                    msSecond = 0;
					printf("Read Cnt = %d.\r\n",RX2_Cnt);
					for(i=0; i<RX2_Cnt; i++)    printf("0x%02x ",RX2_Buffer[i]);    //�Ӵ�������յ�������
					RX2_Cnt  = 0;   //����ֽ���
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
	B_TX2_Busy = 1;
	S2BUF = dat;
	while(B_TX2_Busy);
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
	USART2BRH = (u8)(tmp>>8);
	USART2BRL = (u8)tmp;
}

//========================================================================
// ����: void LinInit(void)
// ����: Lin��ʼ��������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2021-01-05
// ��ע: 
//========================================================================
void LinInit(void)
{
	S2_S  = 1;      //USART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7

	S2CON = (S2CON & 0x3f) | 0x50;  //8λ����,�ɱ䲨����, �������
	AUXR |= 0x14;   //��ʱ��2ʱ��1Tģʽ,��ʼ��ʱ
	ES2 = 1;        //�����ж�

	LinSetBaudrate(Baudrate);	//���ò�����
	S2CFG |= 0x80;			//S2MOD = 1
	USART2CR1 |= 0x80;		//LIN Mode Enable
	USART2CR5 |= 0x20;		//LIN Slave Mode

	USART2CR5 |= 0x10;		//AutoSync
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

        StaCR = USART2CR5;
        if(StaCR & 0x02)     //if LIN header is detected
        {
            B_RX2_Flag = 1;
        }

        if(StaCR & 0xc0)     //if LIN break is detected / LIN header error is detected
        {
            RX2_Cnt = 0;
        }

        RX2_Buffer[RX2_Cnt++] = S2BUF;
        if(RX2_Cnt >= UART2_BUF_LENGTH)   RX2_Cnt = 0;
        RX2_TimeOut = 20;

        USART2CR5 &= ~0xcb;    //Clear flag
    }

    if(S2TI)
    {
        S2TI = 0;    //Clear Tx flag
        B_TX2_Busy = 0;
    }
}
