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

/*************	����˵��	**************

�����̻���STC32GΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

0~7ͨ����ӦP1.0~P1.7, 8~14ͨ����ӦP0.0~P0.6, 15ͨ��Ϊ�ڲ�1.19V��׼��ѹ�������ADCֵ.

��ʼ��ʱ�Ȱ�ҪADCת������������Ϊ��������.

����������������(DMA)���ܣ�����ͨ��һ��ѭ���ɼ��������Զ���ŵ�DMA�����xdata�ռ�.

ͨ������2(P4.6 P4.7)��DMA�����xdata�ռ����յ������ݷ��͸���λ����������115200,N,8,1

����ʱ, ѡ��ʱ�� 22.1184MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\..\comm\STC32G.h"
#include "intrins.h"
#include "stdio.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

/*************	���س�������	**************/

#define MAIN_Fosc     22118400L	//������ʱ��
#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))

#define	ADC_SPEED	15			/* 0~15, ADCת��ʱ��(CPUʱ����) = (n+1)*32  ADCCFG */
#define	RES_FMT		(1<<5)	/* ADC�����ʽ 0: �����, ADC_RES: D11 D10 D9 D8 D7 D6 D5 D4, ADC_RESL: D3 D2 D1 D0 0 0 0 0 */
                          /* ADCCFG     1: �Ҷ���, ADC_RES: 0 0 0 0 D11 D10 D9 D8, ADC_RESL: D7 D6 D5 D4 D3 D2 D1 D0 */

#define	ADC_CH		16			/* 1~16, ADCת��ͨ����, ��ͬ���޸� DMA_ADC_CHSW ת��ͨ�� */
#define	ADC_DATA	12			/* 6~n, ÿ��ͨ��ADCת����������, 2*ת������+4, ��ͬ���޸� DMA_ADC_CFG2 ת������ */
#define	DMA_ADDR	0x800		/* DMA���ݴ�ŵ�ַ */

/*************	���ر�������	**************/

bit	DmaFlag;

u8 xdata DmaBuffer[ADC_CH][ADC_DATA] _at_ DMA_ADDR;

/*************	���غ�������	**************/

u16		Get_ADC12bitResult(u8 channel);
void	delay_ms(u8 ms);
void	ADC_convert(u8 chn);	//chn=0~7��ӦP1.0~P1.7, chn=8~14��ӦP0.0~P0.6, chn=15��ӦBandGap��ѹ
void	DMA_Config(void);

/******************** ���ڴ�ӡ���� ********************/
void UartInit(void)
{
	S2_S = 1;       //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
    S2CFG |= 0x01;  //ʹ�ô���2ʱ��W1λ��������Ϊ1��������ܻ��������Ԥ�ڵĴ���
	S2CON = (S2CON & 0x3f) | 0x40; 
	T2L  = TM;
	T2H  = TM>>8;
	AUXR |= 0x14;	      //��ʱ��2ʱ��1Tģʽ,��ʼ��ʱ
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

/**********************************************/
void main(void)
{
	u8	i,n;

    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x7f;   P0M0 = 0x00;   //����Ҫ��ADC��IO����������
    P1M1 = 0xfb;   P1M0 = 0x00;   //����Ҫ��ADC��IO����������
    P2M1 = 0x3c;   P2M0 = 0x3c;   //����P2.2~P2.5Ϊ©����·(ʵ��������������赽3.3V)
    P3M1 = 0x50;   P3M0 = 0x50;   //����P3.4��P3.6Ϊ©����·(ʵ��������������赽3.3V)
    P4M1 = 0x3c;   P4M0 = 0x3c;   //����P4.2~P4.5Ϊ©����·(ʵ��������������赽3.3V)
    P5M1 = 0x1c;   P5M0 = 0x0c;   //����P5.2��P5.3Ϊ©����·(ʵ��������������赽3.3V)������P5.4Ϊ��������
    P6M1 = 0xff;   P6M0 = 0xff;   //����Ϊ©����·(ʵ��������������赽3.3V)
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���
	
	ADCTIM = 0x3f;  //����ͨ��ѡ��ʱ�䡢����ʱ�䡢����ʱ��
	ADCCFG = RES_FMT + ADC_SPEED;
	//ADCģ���Դ�򿪺���ȴ�1ms��MCU�ڲ�ADC��Դ�ȶ����ٽ���ADת��
	ADC_CONTR = 0x80 + 0;	//ADC on + channel

	UartInit();
	DMA_Config();
	EA = 1;
	printf("STC32Gϵ��ADC DMA���Գ���!\r\n");

	while (1)
	{
		delay_ms(200);
		if(DmaFlag)
		{
			DmaFlag = 0;
			for(i=0; i<ADC_CH; i++)
			{
				for(n=0; n<ADC_DATA; n++)
				{
					printf("0x%02x ",DmaBuffer[i][n]);
				}
				printf("\r\n");
			}
			printf("\r\n");
			DMA_ADC_CR = 0xc0;		//bit7 1:Enable ADC_DMA, bit6 1:Start ADC_DMA
		}
	}
}

//========================================================================
// ����: void DMA_Config(void)
// ����: ADC DMA ��������.
// ����: none.
// ����: none.
// �汾: V1.0, 2021-5-6
//========================================================================
void DMA_Config(void)
{
	DMA_ADC_STA = 0x00;
	DMA_ADC_CFG = 0x80;		//bit7 1:Enable Interrupt
	DMA_ADC_RXAH = (u8)(DMA_ADDR >> 8);	//ADCת�����ݴ洢��ַ
	DMA_ADC_RXAL = (u8)DMA_ADDR;
	DMA_ADC_CFG2 = 0x09;	//ÿ��ͨ��ADCת������:4
	DMA_ADC_CHSW0 = 0xff;	//ADCͨ��ʹ�ܼĴ��� ADC7~ADC0
	DMA_ADC_CHSW1 = 0xff;	//ADCͨ��ʹ�ܼĴ��� ADC15~ADC8
	DMA_ADC_CR = 0xc0;		//bit7 1:Enable ADC_DMA, bit6 1:Start ADC_DMA
}

//========================================================================
// ����: void delay_ms(u8 ms)
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
	do
	{
		i = MAIN_Fosc / 6000;
		while(--i);
	}while(--ms);
}

//========================================================================
// ����: void ADC_DMA_Interrupt (void) interrupt 48
// ����: ADC DMA�жϺ���
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2021-5-8
// ��ע: 
//========================================================================
void ADC_DMA_Interrupt(void) interrupt 13
{
	DMA_ADC_STA = 0;
	DmaFlag = 1;
}
