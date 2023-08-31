/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����        */
/*---------------------------------------------------------------------*/


/*************  ����˵��    **************

�����̻���STC32G12K128Ϊ����оƬ��ʵ����9���б�д����.

���ø߼�PWMA/PWMB�ж�ʵ��8����ʱ������.

PWMA��ʱ��Ƶ��Ϊ SYSclk/2
PWMAͨ��1����ʱ���� 1ms
PWMAͨ��2����ʱ���� 2ms
PWMAͨ��3����ʱ���� 4ms
PWMAͨ��4����ʱ���� 5ms

PWMB��ʱ��Ƶ��Ϊ SYSclk/10000
PWMBͨ��1����ʱ���� 1s
PWMBͨ��2����ʱ���� 2s
PWMBͨ��3����ʱ���� 3s
PWMBͨ��4����ʱ���� 4s

����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

******************************************/

#include "..\comm\STC32G.h"
#include "intrins.h"

#define     MAIN_Fosc       24000000L   //������ʱ��

#define     PSCRA           2
#define     PWMA_T1         MAIN_Fosc/PSCRA/1000    //��ʱ���� 1ms(1KHz)
#define     PWMA_T2         MAIN_Fosc/PSCRA/500     //��ʱ���� 2ms(500Hz)
#define     PWMA_T3         MAIN_Fosc/PSCRA/250     //��ʱ���� 4ms(250Hz)
#define     PWMA_T4         MAIN_Fosc/PSCRA/200     //��ʱ���� 5ms(200Hz)

#define     PSCRB           10000
#define     PWMB_T5         MAIN_Fosc/PSCRB*1       //��ʱ���� 1s
#define     PWMB_T6         MAIN_Fosc/PSCRB*2       //��ʱ���� 2s
#define     PWMB_T7         MAIN_Fosc/PSCRB*3       //��ʱ���� 3s
#define     PWMB_T8         MAIN_Fosc/PSCRB*4       //��ʱ���� 4s

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

/*****************************************************************************/

void PWMA_Timer();
void PWMB_Timer();

u16 cnt1;
u16 cnt2;
u16 cnt3;
u16 cnt4;
u16 cnt5;
u16 cnt6;
u16 cnt7;
u16 cnt8;

/******************** ������ **************************/
void main(void)
{
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x00;   P0M0 = 0x00;     //����Ϊ׼˫���
    P1M1 = 0x00;   P1M0 = 0x00;     //����Ϊ׼˫���
    P2M1 = 0x00;   P2M0 = 0x00;     //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;     //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;     //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;     //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;     //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;     //����Ϊ׼˫���

    PWMA_Timer();
    PWMB_Timer();

    P40 = 0;		                //��LED����
    EA = 1;                         //�����ж�

    while (1);
}

//========================================================================
// ����: void PWMA_Timer()
// ����: PWMA���ú���.
// ����: None
// ����: none.
// �汾: V1.0, 2021-02-08
//========================================================================
void PWMA_Timer()
{
    PWMA_PSCRH = (u8)((PSCRA-1) >> 8);  //����Ԥ��Ƶ��
    PWMA_PSCRL = (u8)(PSCRA-1);
    PWMA_ARRH = 0xff; //��������ʱ��
    PWMA_ARRL = 0xff;
    PWMA_CCER1 = 0x00;
    PWMA_CCMR1 = 0x00;
    PWMA_CCMR2 = 0x00;
    PWMA_CCMR3 = 0x00;
    PWMA_CCMR4 = 0x00;
    cnt1 = PWMA_T1;
    cnt2 = PWMA_T2;
    cnt3 = PWMA_T3;
    cnt4 = PWMA_T4;
    PWMA_CCR1H = (u8)(cnt1 >> 8);
    PWMA_CCR1L = (u8)(cnt1);
    PWMA_CCR2H = (u8)(cnt2 >> 8);
    PWMA_CCR2L = (u8)(cnt2);
    PWMA_CCR3H = (u8)(cnt3 >> 8);
    PWMA_CCR3L = (u8)(cnt3);
    PWMA_CCR4H = (u8)(cnt4 >> 8);
    PWMA_CCR4L = (u8)(cnt4);
    PWMA_IER = 0x1e;            // ʹ���ж�
    PWMA_CR1 |= 0x81;           //ʹ��ARRԤװ�أ���ʼ��ʱ

    cnt1 += PWMA_T1;
    cnt2 += PWMA_T2;
    cnt3 += PWMA_T3;
    cnt4 += PWMA_T4;
}

//========================================================================
// ����: void PWMB_Timer()
// ����: PWMB���ú���.
// ����: None
// ����: none.
// �汾: V1.0, 2021-02-08
//========================================================================
void PWMB_Timer()
{
    PWMB_PSCRH = (u8)((PSCRB-1) >> 8);  //����Ԥ��Ƶ��
    PWMB_PSCRL = (u8)(PSCRB-1);
    PWMB_ARRH = 0xff; //��������ʱ��
    PWMB_ARRL = 0xff;
    PWMB_CCER1 = 0x00;
    PWMB_CCMR1 = 0x00;      //���ö���ģʽ
    PWMB_CCMR2 = 0x00;      //���ö���ģʽ
    PWMB_CCMR3 = 0x00;      //���ö���ģʽ
    PWMB_CCMR4 = 0x00;      //���ö���ģʽ
    cnt5 = PWMB_T5;
    cnt6 = PWMB_T6;
    cnt7 = PWMB_T7;
    cnt8 = PWMB_T8;
    PWMB_CCR5H = (u8)(cnt5 >> 8);
    PWMB_CCR5L = (u8)(cnt5);
    PWMB_CCR6H = (u8)(cnt6 >> 8);
    PWMB_CCR6L = (u8)(cnt6);
    PWMB_CCR7H = (u8)(cnt7 >> 8);
    PWMB_CCR7L = (u8)(cnt7);
    PWMB_CCR8H = (u8)(cnt8 >> 8);
    PWMB_CCR8L = (u8)(cnt8);
    PWMB_IER = 0x1e;        // ʹ���ж�
    PWMB_CR1 |= 0x81;       //ʹ��ARRԤװ�أ���ʼ��ʱ

    cnt5 += PWMB_T5;
    cnt6 += PWMB_T6;
    cnt7 += PWMB_T7;
    cnt8 += PWMB_T8;
}

//========================================================================
// ����: PWMA_ISR() interrupt 26
// ����: PWMA�жϺ���.
// ����: None
// ����: none.
// �汾: V1.0, 2020-12-04
//========================================================================
void PWMA_ISR() interrupt 26
{
    u8 sr;
    
    sr = PWMA_SR1;
    PWMA_SR1 = 0;
    
    if (sr & 0x02)
    {
        PWMA_CCR1H = (u8)(cnt1 >> 8);
        PWMA_CCR1L = (u8)(cnt1);
        cnt1 += PWMA_T1;
        P20 = ~P20;
        P60 = ~P60;
    }
    if (sr & 0x04)
    {
        PWMA_CCR2H = (u8)(cnt2 >> 8);
        PWMA_CCR2L = (u8)(cnt2);
        cnt2 += PWMA_T2;
        P21 = ~P21;
        P61 = ~P61;
    }
    if (sr & 0x08)
    {
        PWMA_CCR3H = (u8)(cnt3 >> 8);
        PWMA_CCR3L = (u8)(cnt3);
        cnt3 += PWMA_T3;
        P22 = ~P22;
        P62 = ~P62;
    }
    if (sr & 0x10)
    {
        PWMA_CCR4H = (u8)(cnt4 >> 8);
        PWMA_CCR4L = (u8)(cnt4);
        cnt4 += PWMA_T4;
        P23 = ~P23;
        P63 = ~P63;
    }
}

//========================================================================
// ����: PWMB_ISR() interrupt 27
// ����: PWMB�жϺ���.
// ����: None
// ����: none.
// �汾: V1.0, 2020-12-04
//========================================================================
void PWMB_ISR() interrupt 27
{
    u8 sr;
    
    sr = PWMB_SR1;
    PWMB_SR1 = 0;
    
    if (sr & 0x02)
    {
        PWMB_CCR5H = (u8)(cnt5 >> 8);
        PWMB_CCR5L = (u8)(cnt5);
        cnt5 += PWMB_T5;
        P24 = ~P24;
        P64 = ~P64;
    }
    if (sr & 0x04)
    {
        PWMB_CCR6H = (u8)(cnt6 >> 8);
        PWMB_CCR6L = (u8)(cnt6);
        cnt6 += PWMB_T6;
        P25 = ~P25;
        P65 = ~P65;
    }
    if (sr & 0x08)
    {
        PWMB_CCR7H = (u8)(cnt7 >> 8);
        PWMB_CCR7L = (u8)(cnt7);
        cnt7 += PWMB_T7;
        P26 = ~P26;
        P66 = ~P66;
    }
    if (sr & 0x10)
    {
        PWMB_CCR8H = (u8)(cnt8 >> 8);
        PWMB_CCR8L = (u8)(cnt8);
        cnt8 += PWMB_T8;
        P27 = ~P27;
        P67 = ~P67;
    }
}

