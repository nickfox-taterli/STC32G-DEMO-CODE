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

��доƬ�ڲ����ɵ�RTCģ��.

��STC��MCU��IO��ʽ����8λ����ܡ�

ʹ��Timer0��16λ�Զ���װ������1ms����,�������������������, �û��޸�MCU��ʱ��Ƶ��ʱ,�Զ���ʱ��1ms.

8λ�������ʾʱ��(Сʱ-����-��).

����ɨ�谴������Ϊ25~32.

����ֻ֧�ֵ�������, ��֧�ֶ��ͬʱ����, ���������в���Ԥ֪�Ľ��.

�����³���1���,����10��/����ٶ��ṩ�ؼ����. �û�ֻ��Ҫ���KeyCode�Ƿ��0���жϼ��Ƿ���.

����ʱ���:
����25: Сʱ+.
����26: Сʱ-.
����27: ����+.
����28: ����-.

����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

******************************************/

#include "..\..\comm\STC32G.h"

#include "stdio.h"
#include "intrins.h"

#define     MAIN_Fosc       24000000L   //������ʱ��

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

/***********************************************************/

#define DIS_DOT     0x20
#define DIS_BLACK   0x10
#define DIS_        0x11

/****************************** �û������ ***********************************/

#define PrintUart     2        //1:printf ʹ�� UART1; 2:printf ʹ�� UART2
#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

#define SleepModeSet  0        //0:��������ģʽ��ʹ���������ʾʱ���ܽ�����; 1:ʹ������ģʽ

/*****************************************************************************/


/*************  ���س�������    **************/
u8 code t_display[]={                       //��׼�ֿ�
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

u8 code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //λ��


/*************  ���ر�������    **************/

u8  LED8[8];        //��ʾ����
u8  display_index;  //��ʾλ����
bit B_1ms;          //1ms��־
bit B_1s;
bit B_Alarm;        //���ӱ�־

u8 IO_KeyState, IO_KeyState1, IO_KeyHoldCnt;    //���м��̱���
u8  KeyHoldCnt; //�����¼�ʱ
u8  KeyCode;    //���û�ʹ�õļ���
u8  cnt50ms;

u8  hour,minute; //RTC����
u16 msecond;


/*************  ���غ�������    **************/
void IO_KeyScan(void);   //50ms call
void DisplayRTC(void);
void WriteRTC(void);

void RTC_config(void);

/******************** ���ڴ�ӡ���� ********************/
void UartInit(void)
{
#if(PrintUart == 1)
	SCON = (SCON & 0x3f) | 0x40; 
	T1x12 = 1;          //��ʱ��ʱ��1Tģʽ
	S1BRT = 0;          //����1ѡ��ʱ��1Ϊ�����ʷ�����
	TL1  = TM;
	TH1  = TM>>8;
	TR1 = 1;				//��ʱ��1��ʼ��ʱ

//	SCON = (SCON & 0x3f) | 0x40; 
//	T2L  = TM;
//	T2H  = TM>>8;
//	AUXR |= 0x15;   //����1ѡ��ʱ��2Ϊ�����ʷ�����
#else
	S2_S = 1;       //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
    S2CFG |= 0x01;  //ʹ�ô���2ʱ��W1λ��������Ϊ1��������ܻ��������Ԥ�ڵĴ���
	S2CON = (SCON & 0x3f) | 0x40; 
	T2L  = TM;
	T2H  = TM>>8;
	AUXR |= 0x14;	      //��ʱ��2ʱ��1Tģʽ,��ʼ��ʱ
#endif
}

void UartPutc(unsigned char dat)
{
#if(PrintUart == 1)
	SBUF = dat; 
	while(TI == 0);
	TI = 0;
#else
	S2BUF  = dat; 
	while(S2TI == 0);
	S2TI = 0;    //Clear Tx flag
#endif
}

char putchar(char c)
{
	UartPutc(c);
	return c;
}

/**********************************************/
void main(void)
{
    u8  i;

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
    
    display_index = 0;
    
    for(i=0; i<8; i++)  LED8[i] = DIS_BLACK; //�ϵ�����
    
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    UartInit();
    RTC_config();
    EA = 1;     //�����ж�
    
    DisplayRTC();
    LED8[2] = DIS_;
    LED8[5] = DIS_;

    KeyHoldCnt = 0; //�����¼�ʱ
    KeyCode = 0;    //���û�ʹ�õļ���

    IO_KeyState = 0;
    IO_KeyState1 = 0;
    IO_KeyHoldCnt = 0;
    cnt50ms = 0;

    while(1)
    {
        if(B_1s)
        {
            B_1s = 0;
            DisplayRTC();
            printf("Year=20%d,Month=%d,Day=%d,Hour=%d,Minute=%d,Second=%d\r\n",YEAR,MONTH,DAY,HOUR,MIN,SEC);
        }

        if(B_Alarm)
        {
            B_Alarm = 0;
            printf("RTC Alarm!\r\n");
        }

#if(SleepModeSet == 1)
        _nop_();
        _nop_();
        PCON = 0x02;  //STC32G оƬʹ���ڲ�32Kʱ�ӣ������޷�����
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
#else
        if(B_1ms)   //1ms��
        {
            B_1ms = 0;

            if(++cnt50ms >= 50)     //50msɨ��һ�����м���
            {
                cnt50ms = 0;
                IO_KeyScan();
            }
            
            if(KeyCode != 0)        //�м�����
            {
                if(KeyCode == 25)   //hour +1
                {
                    if(++hour >= 24)    hour = 0;
                    WriteRTC();
                    DisplayRTC();
                }
                if(KeyCode == 26)   //hour -1
                {
                    if(--hour >= 24)    hour = 23;
                    WriteRTC();
                    DisplayRTC();
                }
                if(KeyCode == 27)   //minute +1
                {
                    if(++minute >= 60)  minute = 0;
                    WriteRTC();
                    DisplayRTC();
                }
                if(KeyCode == 28)   //minute -1
                {
                    if(--minute >= 60)  minute = 59;
                    WriteRTC();
                    DisplayRTC();
                }

                KeyCode = 0;
            }

        }
#endif
    }
} 

/********************** ��ʾʱ�Ӻ��� ************************/
void DisplayRTC(void)
{
    hour = HOUR;
    minute = MIN;

    if(HOUR >= 10)  LED8[0] = HOUR / 10;
    else            LED8[0] = DIS_BLACK;
    LED8[1] = HOUR % 10;
    LED8[2] = DIS_;
    LED8[3] = MIN / 10;
    LED8[4] = MIN % 10;
    LED8[6] = SEC / 10;
    LED8[7] = SEC % 10;
}

/********************** дRTC���� ************************/
void WriteRTC(void)
{
    INIYEAR = YEAR;   //�̳е�ǰ������
    INIMONTH = MONTH;
    INIDAY = DAY;

    INIHOUR = hour;   //�޸�ʱ����
    INIMIN = minute;
    INISEC = 0;
    INISSEC = 0;
    RTCCFG |= 0x01;   //����RTC�Ĵ�����ʼ��
}

/*****************************************************
    ���м�ɨ�����
    ʹ��XY����4x4���ķ�����ֻ�ܵ������ٶȿ�

   Y     P04      P05      P06      P07
          |        |        |        |
X         |        |        |        |
P00 ---- K00 ---- K01 ---- K02 ---- K03 ----
          |        |        |        |
P01 ---- K04 ---- K05 ---- K06 ---- K07 ----
          |        |        |        |
P02 ---- K08 ---- K09 ---- K10 ---- K11 ----
          |        |        |        |
P03 ---- K12 ---- K13 ---- K14 ---- K15 ----
          |        |        |        |
******************************************************/

u8 code T_KeyTable[16] = {0,1,2,0,3,0,0,0,4,0,0,0,0,0,0,0};

void IO_KeyDelay(void)
{
    u8 i;
    i = 60;
    while(--i)  ;
}

void IO_KeyScan(void)    //50ms call
{
    u8  j;

    j = IO_KeyState1;   //������һ��״̬

    P0 = 0xf0;  //X�ͣ���Y
    IO_KeyDelay();
    IO_KeyState1 = P0 & 0xf0;

    P0 = 0x0f;  //Y�ͣ���X
    IO_KeyDelay();
    IO_KeyState1 |= (P0 & 0x0f);
    IO_KeyState1 ^= 0xff;   //ȡ��
    
    if(j == IO_KeyState1)   //�������ζ����
    {
        j = IO_KeyState;
        IO_KeyState = IO_KeyState1;
        if(IO_KeyState != 0)    //�м�����
        {
            F0 = 0;
            if(j == 0)  F0 = 1; //��һ�ΰ���
            else if(j == IO_KeyState)
            {
                if(++IO_KeyHoldCnt >= 20)   //1����ؼ�
                {
                    IO_KeyHoldCnt = 18;
                    F0 = 1;
                }
            }
            if(F0)
            {
                j = T_KeyTable[IO_KeyState >> 4];
                if((j != 0) && (T_KeyTable[IO_KeyState& 0x0f] != 0)) 
                    KeyCode = (j - 1) * 4 + T_KeyTable[IO_KeyState & 0x0f] + 16;    //������룬17~32
            }
        }
        else    IO_KeyHoldCnt = 0;
    }
    P0 = 0xff;
}

/********************** ��ʾɨ�躯�� ************************/
void DisplayScan(void)
{   
    P7 = ~T_COM[7-display_index];
    P6 = ~t_display[LED8[display_index]];
    if(++display_index >= 8)    display_index = 0;  //8λ������0
}

/******************** RTC�жϺ��� *********************/
void RTC_Isr() interrupt 13
{
	if(RTCIF & 0x80)    //�����ж�
	{
//		P01 = !P01;
		RTCIF &= ~0x80;
		B_Alarm = 1;
	}

	if(RTCIF & 0x08)    //���ж�
	{
//		P00 = !P00;
		RTCIF &= ~0x08;
		B_1s = 1;
	}
}

/********************** Timer0 1ms�жϺ��� ************************/
void timer0 (void) interrupt 1
{
#if(SleepModeSet == 0)
    DisplayScan();  //1msɨ����ʾһλ
#endif
    B_1ms = 1;      //1ms��־
}

//========================================================================
// ����: void RTC_config(void)
// ����: RTC��ʼ��������
// ����: ��.
// ����: ��.
// �汾: V1.0, 2020-6-10
//========================================================================
void RTC_config(void)
{
    INIYEAR = 21;     //Y:2021
    INIMONTH = 12;    //M:12
    INIDAY = 31;      //D:31
    INIHOUR = 23;     //H:23
    INIMIN = 59;      //M:59
    INISEC = 50;      //S:50
    INISSEC = 0;      //S/128:0

    ALAHOUR = 0;	//����Сʱ
    ALAMIN  = 0;	//���ӷ���
    ALASEC  = 0;	//������
    ALASSEC = 0;	//����1/128��

    //STC32G оƬʹ���ڲ�32Kʱ�ӣ������޷�����
//    IRC32KCR = 0x80;   //�����ڲ�32K����.
//    while (!(IRC32KCR & 1));  //�ȴ�ʱ���ȶ�
//    RTCCFG = 0x03;    //ѡ���ڲ�32Kʱ��Դ������RTC�Ĵ�����ʼ��

    X32KCR = 0x80 + 0x40;   //�����ⲿ32K����, ������+0x00, ������+0x40.
    while (!(X32KCR & 1));  //�ȴ�ʱ���ȶ�
    RTCCFG = 0x01;    //ѡ���ⲿ32Kʱ��Դ������RTC�Ĵ�����ʼ��

    RTCIF = 0x00;     //���жϱ�־
    RTCIEN = 0x88;    //�ж�ʹ��, 0x80:�����ж�, 0x40:���ж�, 0x20:Сʱ�ж�, 0x10:�����ж�, 0x08:���ж�, 0x04:1/2���ж�, 0x02:1/8���ж�, 0x01:1/32���ж�
    RTCCR = 0x01;     //RTCʹ��

    while(RTCCFG & 0x01);	//�ȴ���ʼ�����,��Ҫ�� "RTCʹ��" ֮���ж�. 
    //����RTCʱ����Ҫ32768Hz��1������ʱ��,��Լ30.5us. ����ͬ��, ����ʵ�ʵȴ�ʱ����0~30.5us.
    //������ȴ�������ɾ�˯��, ��RTC����������û���, ֹͣ����, ���Ѻ�ż���������ò���������.
}
