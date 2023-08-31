/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����        */
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���STC32GΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

ͨ��������ʵ��STC32Ӳ���˳������滻��׼�㷨���㷨

����2(115200,N,8,1)��ӡ������

��ͨ������"STC32_MDU32_V1.x.LIB"�ļ�����ʾ��������IO�ڵ͵�ƽʱ�䣬���Ա�STC32Ӳ���˳�����Ԫ���׼�㷨��ļ���Ч��

��������ʱ��ʱ�ɽ����ڴ�ӡָ�����Σ�����鿴ÿ����ʽ�ļ���ʱ��

����ʱ, Ĭ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\comm\STC32G.h"
#include "intrins.h"
#include "stdio.h"

#define MAIN_Fosc        24000000UL

volatile  unsigned long int near uint1, uint2;
volatile unsigned long int near xuint;

volatile long int sint1, sint2;
volatile long int xsint;

unsigned long ultest;
long ltest;

/*****************************************************************************/

sbit TPIN  =  P5^0;

/*****************************************************************************/

#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart     2        //1:printf ʹ�� UART1; 2:printf ʹ�� UART2

/******************** ���ڴ�ӡ���� ********************/
void UartInit(void)
{
#if(PrintUart == 1)
	SCON = (SCON & 0x3f) | 0x40; 
	T1x12 = 1;          //��ʱ��ʱ��1Tģʽ
	S1BRT = 0;          //����1ѡ��ʱ��1Ϊ�����ʷ�����
	TL1  = TM;
	TH1  = TM>>8;
	TR1 = 1;		//��ʱ��1��ʼ��ʱ

//	SCON = (SCON & 0x3f) | 0x40; 
//	T2L  = TM;
//	T2H  = TM>>8;
//	AUXR |= 0x15;   //����1ѡ��ʱ��2Ϊ�����ʷ�����
#else
	S2_S = 1;       //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
    S2CFG |= 0x01;  //ʹ�ô���2ʱ��W1λ��������Ϊ1��������ܻ��������Ԥ�ڵĴ���
	S2CON = (S2CON & 0x3f) | 0x40; 
	T2L  = TM;
	T2H  = TM>>8;
	AUXR |= 0x14;	    //��ʱ��2ʱ��1Tģʽ,��ʼ��ʱ
#endif
}

void UartPutc(unsigned char dat)
{
#if(PrintUart == 1)
	SBUF = dat; 
	while(TI==0);
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

void delay(unsigned char ms)
{
	 while(--ms);
}

/*****************************************************************************/
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

	UartInit();

	printf("STC32G MDU32 Test.\r\n");
	
	TPIN = 0;  //���㿪ʼͬ���ź�
	delay(2);
	TPIN = 1;
	delay(2);
	TPIN = 0;
	delay(2);
	TPIN = 1;
	delay(2);
	
	ultest = 12345678UL;
	ltest = 12345678;
	ultest = ultest / 12;
	ltest = ltest / 12;

	sint1 = 0x31030F05;
	sint2 = 0x00401350;
	TPIN = 0;
	xsint = sint1 * sint2;
	TPIN = 1;
	printf("Result1=0x%x",xsint>>16);
	printf("%x\r\n",xsint);

	uint1 =  5;
	uint2 =  50;
	TPIN = 0;
	xuint = uint1 * uint2;
	TPIN = 1;
	printf("Result2=%d\r\n",xuint);

	uint1 = 654689;
	uint2 = 528;
	TPIN = 0;
	xuint = uint1 / uint2;
	TPIN = 1;
	printf("Result3=%u\r\n",xuint);

	sint1 = 2134135177;
	sint2 = 20000;
	TPIN = 0;
	xsint = sint1 / sint2;
	TPIN = 1;
	printf("Result4=0x%x",xsint>>16);
	printf("%x\r\n",xsint);

	sint1 = -2134135177;
	sint2 = -20000;
	TPIN = 0;
	xsint = sint1 / sint2;
	TPIN = 1;
	printf("Result5=0x%x",xsint>>16);
	printf("%x\r\n",xsint);

	sint1 = 2134135177;
	sint2 = -20000;
	TPIN = 0;
	xsint = sint1 / sint2;
	TPIN = 1;
	printf("Result6=0x%x",xsint>>16);
	printf("%x\r\n",xsint);

	while(1);
}
/*****************************************************************************/
