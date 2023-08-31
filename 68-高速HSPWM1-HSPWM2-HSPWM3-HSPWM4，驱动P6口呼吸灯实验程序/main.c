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

���ٸ߼�PWM��ʱ�� PWM1P/PWM1N,PWM2P/PWM2N,PWM3P/PWM3N,PWM4P/PWM4N ÿ��ͨ�����ɶ���ʵ��PWM������������������Գ����.

8��ͨ��PWM���ö�ӦP6��8���˿�.

ͨ��P6�������ӵ�8��LED�ƣ�����PWMʵ�ֺ�����Ч��.

PWM���ں�ռ�ձȿ��Ը�����Ҫ�������ã���߿ɴ�65535.

����ʱ, ѡ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\comm\STC32G.h"

#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL

/****************************** �û������ ***********************************/

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

/*****************************************************************************/

#define	HSCK_MCLK			0
#define	HSCK_PLL			1
#define	HSCK_SEL			HSCK_PLL

#define	PLL_96M				0
#define	PLL_144M			1
#define	PLL_SEL				PLL_144M

#define	CKMS			0x80
#define	HSIOCK			0x40
#define	MCK2SEL_MSK		0x0c
#define	MCK2SEL_SEL1	0x00
#define	MCK2SEL_PLL		0x04
#define	MCK2SEL_PLLD2	0x08
#define	MCK2SEL_IRC48	0x0c
#define	MCKSEL_MSK		0x03
#define	MCKSEL_HIRC		0x00
#define	MCKSEL_XOSC		0x01
#define	MCKSEL_X32K		0x02
#define	MCKSEL_IRC32K	0x03

#define	ENCKM				0x80
#define	PCKI_MSK			0x60
#define	PCKI_D1				0x00
#define	PCKI_D2				0x20
#define	PCKI_D4				0x40
#define	PCKI_D8				0x60

/*************  ���س�������    **************/

#define PWM1_0      0x00	//P:P1.0  N:P1.1
#define PWM1_1      0x01	//P:P2.0  N:P2.1
#define PWM1_2      0x02	//P:P6.0  N:P6.1

#define PWM2_0      0x00	//P:P1.2/P5.4  N:P1.3
#define PWM2_1      0x04	//P:P2.2  N:P2.3
#define PWM2_2      0x08	//P:P6.2  N:P6.3

#define PWM3_0      0x00	//P:P1.4  N:P1.5
#define PWM3_1      0x10	//P:P2.4  N:P2.5
#define PWM3_2      0x20	//P:P6.4  N:P6.5

#define PWM4_0      0x00	//P:P1.6  N:P1.7
#define PWM4_1      0x40	//P:P2.6  N:P2.7
#define PWM4_2      0x80	//P:P6.6  N:P6.7
#define PWM4_3      0xC0	//P:P3.4  N:P3.3

#define ENO1P       0x01
#define ENO1N       0x02
#define ENO2P       0x04
#define ENO2N       0x08
#define ENO3P       0x10
#define ENO3N       0x20
#define ENO4P       0x40
#define ENO4N       0x80

#define PWM_PERIOD  1023    //��������ֵ

/*************  ���ر�������    **************/

bit B_1ms;          //1ms��־

u16 PWM1_Duty;
u16 PWM2_Duty;
u16 PWM3_Duty;
u16 PWM4_Duty;

bit PWM1_Flag;
bit PWM2_Flag;
bit PWM3_Flag;
bit PWM4_Flag;

void PllConfig(void);
void HSPwmConfig(void);
void UpdatePwm(void);

/********************* ������ *************************/
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

    PWM1_Flag = 0;
    PWM2_Flag = 0;
    PWM3_Flag = 0;
    PWM4_Flag = 0;
    
    PWM1_Duty = 0;
    PWM2_Duty = 256;
    PWM3_Duty = 512;
    PWM4_Duty = 1024;

    //  Timer0��ʼ��
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    P6SR = 0x00;                //IO�ڵ�ƽת���ٶȼӿ�

    PWMA_PS = 0x00;  //�߼� PWM ͨ�������ѡ��λ
    PWMA_PS |= PWM1_2; //ѡ�� PWM1_2 ͨ��
    PWMA_PS |= PWM2_2; //ѡ�� PWM2_2 ͨ��
    PWMA_PS |= PWM3_2; //ѡ�� PWM3_2 ͨ��
    PWMA_PS |= PWM4_2; //ѡ�� PWM4_2 ͨ��

    PllConfig();
    HSPwmConfig();

    P40 = 0;	//��LED����
    EA = 1;     //�����ж�

    while (1);
}


/********************** Timer0 1ms�жϺ��� ************************/
void timer0(void) interrupt 1
{
    if(!PWM1_Flag)
    {
        PWM1_Duty++;
        if(PWM1_Duty > PWM_PERIOD) PWM1_Flag = 1;
    }
    else
    {
        PWM1_Duty--;
        if(PWM1_Duty <= 0) PWM1_Flag = 0;
    }

    if(!PWM2_Flag)
    {
        PWM2_Duty++;
        if(PWM2_Duty > PWM_PERIOD) PWM2_Flag = 1;
    }
    else
    {
        PWM2_Duty--;
        if(PWM2_Duty <= 0) PWM2_Flag = 0;
    }

    if(!PWM3_Flag)
    {
        PWM3_Duty++;
        if(PWM3_Duty > PWM_PERIOD) PWM3_Flag = 1;
    }
    else
    {
        PWM3_Duty--;
        if(PWM3_Duty <= 0) PWM3_Flag = 0;
    }

    if(!PWM4_Flag)
    {
        PWM4_Duty++;
        if(PWM4_Duty > PWM_PERIOD) PWM4_Flag = 1;
    }
    else
    {
        PWM4_Duty--;
        if(PWM4_Duty <= 0) PWM4_Flag = 0;
    }
    
    UpdatePwm();
}

//========================================================================
// ����: delay(void)
// ����: ��ʱ����. 
// ����: none.
// ����: none.
// �汾: V1.0, 2022-03-16
//========================================================================
void delay(void)
{
	u8	i;
	for	(i=0; i<100; i++);
}

//========================================================================
// ����: ReadPWMA(void)
// ����: �첽��ȡPWMA���⹦�ܼĴ�������. 
// ����: addr: ��ȡ���⹦�ܼĴ�����ַ.
// ����: dat:  ��ȡ���⹦�ܼĴ�������.
// �汾: V1.0, 2022-03-16
//========================================================================
//u8 ReadPWMA(u8 addr)
//{
//	u8 dat;
//	
//	while (HSPWMA_ADR &	0x80); //�ȴ�ǰһ���첽��д���
//	HSPWMA_ADR = addr |	0x80;  //���ü�ӷ��ʵ�ַ,ֻ��Ҫ����ԭXFR��ַ�ĵ�7λ
//								//HSPWMA_ADDR�Ĵ��������λд1,��ʾ������
//	while (HSPWMA_ADR &	0x80); //�ȴ���ǰ�첽��ȡ���
//	dat	= HSPWMA_DAT;		   //��ȡ�첽����
//	
//	return dat;
//}

//========================================================================
// ����: WritePWMA(void)
// ����: �첽��ȡPWMA���⹦�ܼĴ�������. 
// ����: addr: д�����⹦�ܼĴ�����ַ.
// ����: dat:  д�����⹦�ܼĴ�������.
// ����: none.
// �汾: V1.0, 2022-03-16
//========================================================================
void WritePWMA(u8	addr, u8 dat)
{
	while (HSPWMA_ADR &	0x80); //�ȴ�ǰһ���첽��д���
	HSPWMA_DAT = dat;		   //׼����Ҫд�������
	HSPWMA_ADR = addr &	0x7f;  //���ü�ӷ��ʵ�ַ,ֻ��Ҫ����ԭXFR��ַ�ĵ�7λ
								//HSPWMA_ADDR�Ĵ��������λд0,��ʾд����
}

//========================================================================
// ����: PllConfig(void)
// ����: PWMʱ�ӳ�ʼ������. 
// ����: none.
// ����: none.
// �汾: V1.0, 2022-03-16
//========================================================================
void PllConfig(void)
{
	//ѡ��PLL���ʱ��
#if	(PLL_SEL ==	PLL_96M)
	CLKSEL &= ~CKMS;			//ѡ��PLL��96M��ΪPLL�����ʱ��
#elif (PLL_SEL == PLL_144M)
	CLKSEL |= CKMS;				//ѡ��PLL��144M��ΪPLL�����ʱ��
#else
	CLKSEL &= ~CKMS;			//Ĭ��ѡ��PLL��96M��ΪPLL�����ʱ��
#endif
	
	//ѡ��PLL����ʱ�ӷ�Ƶ,��֤����ʱ��Ϊ12M
	USBCLK &= ~PCKI_MSK;
#if	(MAIN_Fosc == 12000000UL)
	USBCLK |= PCKI_D1;			//PLL����ʱ��1��Ƶ
#elif (MAIN_Fosc	== 24000000UL)
	USBCLK |= PCKI_D2;			//PLL����ʱ��2��Ƶ
#elif (MAIN_Fosc	== 48000000UL)
	USBCLK |= PCKI_D4;			//PLL����ʱ��4��Ƶ
#elif (MAIN_Fosc	== 96000000UL)
	USBCLK |= PCKI_D8;			//PLL����ʱ��8��Ƶ
#else
	USBCLK |= PCKI_D1;			//Ĭ��PLL����ʱ��1��Ƶ
#endif

	//����PLL
	USBCLK |= ENCKM;			//ʹ��PLL��Ƶ
	
	delay();					//�ȴ�PLL��Ƶ

	//ѡ��HSPWM/HSSPIʱ��
#if	(HSCK_SEL == HSCK_MCLK)
	CLKSEL &= ~HSIOCK;			//HSPWM/HSSPIѡ����ʱ��Ϊʱ��Դ
#elif (HSCK_SEL	== HSCK_PLL)
	CLKSEL |= HSIOCK;			//HSPWM/HSSPIѡ��PLL���ʱ��Ϊʱ��Դ
#else
	CLKSEL &= ~HSIOCK;			//Ĭ��HSPWM/HSSPIѡ����ʱ��Ϊʱ��Դ
#endif

	HSCLKDIV = 0;				//HSPWM/HSSPIʱ��Դ����Ƶ
}

//========================================================================
// ����: HSPwmConfig(void)
// ����: PWM��ʼ������. 
// ����: none.
// ����: none.
// �汾: V1.0, 2022-03-16
//========================================================================
void HSPwmConfig(void)
{
	HSPWMA_CFG = 0x03;			//ʹ��PWMA��ؼĴ����첽���ʹ���
	
	//ͨ���첽��ʽ����PWMA����ؼĴ���
	WritePWMA((u8)&PWMA_CCER1, 0x00);
	WritePWMA((u8)&PWMA_CCER2, 0x00);
	WritePWMA((u8)&PWMA_CCMR1, 0x68);			//ͨ��ģʽ����
	WritePWMA((u8)&PWMA_CCMR2, 0x68);
	WritePWMA((u8)&PWMA_CCMR3, 0x68);
	WritePWMA((u8)&PWMA_CCMR4, 0x68);
	WritePWMA((u8)&PWMA_CCER1, 0x55);			//����ͨ�����ʹ�ܺͼ���
	WritePWMA((u8)&PWMA_CCER2, 0x55);
	WritePWMA((u8)&PWMA_ENO, ENO1P|ENO1N|ENO2P|ENO2N|ENO3P|ENO3N|ENO4P|ENO4N);	//ʹ��PWM�ź�����˿�
	WritePWMA((u8)&PWMA_CCR1H, (u8)(PWM1_Duty >> 8));	//�������PWM��ռ�ձ�
	WritePWMA((u8)&PWMA_CCR1L, (u8)PWM1_Duty);
	WritePWMA((u8)&PWMA_CCR2H, (u8)(PWM2_Duty >> 8));	//�������PWM��ռ�ձ�
	WritePWMA((u8)&PWMA_CCR2L, (u8)PWM2_Duty);
	WritePWMA((u8)&PWMA_CCR3H, (u8)(PWM3_Duty >> 8));	//�������PWM��ռ�ձ�
	WritePWMA((u8)&PWMA_CCR3L, (u8)PWM3_Duty);
	WritePWMA((u8)&PWMA_CCR4H, (u8)(PWM4_Duty >> 8));	//�������PWM��ռ�ձ�
	WritePWMA((u8)&PWMA_CCR4L, (u8)PWM4_Duty);
	WritePWMA((u8)&PWMA_ARRH,  (u8)(PWM_PERIOD >> 8));	//�������PWM������
	WritePWMA((u8)&PWMA_ARRL,  (u8)PWM_PERIOD);
	WritePWMA((u8)&PWMA_DTR, 10);					//���û����Գ����PWM������
	WritePWMA((u8)&PWMA_BKR, 0x80);				//ʹ�������
	WritePWMA((u8)&PWMA_CR1, 0x81);				//ʹ��ARRԤװ�أ���ʼPWM����
}

//========================================================================
// ����: UpdatePwm(void)
// ����: ����PWMռ�ձ�. 
// ����: none.
// ����: none.
// �汾: V1.0, 2012-11-22
//========================================================================
void UpdatePwm(void)
{
	WritePWMA((u8)&PWMA_CCR1H, (u8)(PWM1_Duty >> 8));	//�������PWM��ռ�ձ�
	WritePWMA((u8)&PWMA_CCR1L, (u8)PWM1_Duty);
	WritePWMA((u8)&PWMA_CCR2H, (u8)(PWM2_Duty >> 8));	//�������PWM��ռ�ձ�
	WritePWMA((u8)&PWMA_CCR2L, (u8)PWM2_Duty);
	WritePWMA((u8)&PWMA_CCR3H, (u8)(PWM3_Duty >> 8));	//�������PWM��ռ�ձ�
	WritePWMA((u8)&PWMA_CCR3L, (u8)PWM3_Duty);
	WritePWMA((u8)&PWMA_CCR4H, (u8)(PWM4_Duty >> 8));	//�������PWM��ռ�ձ�
	WritePWMA((u8)&PWMA_CCR4L, (u8)PWM4_Duty);
}
