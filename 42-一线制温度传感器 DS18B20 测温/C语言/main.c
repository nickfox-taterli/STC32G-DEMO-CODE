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

ͨ��һ��IO�ڻ�ȡһ�����¶ȴ����� DS18B20 �¶�ֵ.

ʹ��Timer0��16λ�Զ���װ������1ms����,�������������������, �û��޸�MCU��ʱ��Ƶ��ʱ,�Զ���ʱ��1ms.

��STC��MCU��IO��ʽ����8λ����ܣ�ͨ���������ʾ�������¶�ֵ.

����ʱ, ѡ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\..\comm\STC32G.h"

#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL

/****************************** �û������ ***********************************/
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

#if (MAIN_Fosc >= 40000000L)
    #define		usrNOP()    _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_()
#elif (MAIN_Fosc >= 36000000L)
    #define		usrNOP()    _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_()
#elif (MAIN_Fosc >= 30000000L)
    #define		usrNOP()    _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_()
#elif (MAIN_Fosc >= 24000000L)
    #define		usrNOP()    _nop_();_nop_();_nop_();_nop_();_nop_();_nop_()
#elif (MAIN_Fosc >= 20000000L)
    #define		usrNOP()    _nop_();_nop_();_nop_();_nop_();_nop_()
#elif (MAIN_Fosc >= 18000000L)
    #define		usrNOP()    _nop_();_nop_();_nop_();_nop_()
#elif (MAIN_Fosc >= 12000000L)
    #define		usrNOP()    _nop_();_nop_();_nop_()
#elif (MAIN_Fosc >= 6000000L)
    #define		usrNOP()    _nop_();_nop_()
#else
    #define		usrNOP()    _nop_()
#endif

#define DIS_DOT     0x20
#define DIS_BLACK   0x10
#define DIS_        0x11

/*****************************************************************************/
sbit DQ = P3^3;                   //DS18B20�����ݿ�

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
u16 msecond;
u8 MinusFlag;       //������־��0��������1������

/*************  ���غ�������    **************/
void DS18B20_Reset();
void DS18B20_WriteByte(u8 dat);
u8 DS18B20_ReadByte();

void delay_us(u8 us);
u16 ReadTemperature();
void DisplayTemperature(u16 temp);

/********************* ������ *************************/
void main(void)
{
    u8  i;
    u16 Temp;

    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x30;   P0M0 = 0x30;   //����P0.4��P0.5Ϊ©����·(ʵ��������������赽3.3V)
    P1M1 = 0x3a;   P1M0 = 0x32;   //����P1.1��P1.4��P1.5Ϊ©����·(ʵ��������������赽3.3V), P1.1��PWM��DAC��·ͨ�����贮����P2.3,  P1.3 Ϊ ADC ��������
    P2M1 = 0x3c;   P2M0 = 0x3c;   //����P2.2~P2.5Ϊ©����·(ʵ��������������赽3.3V)
    P3M1 = 0x50;   P3M0 = 0x50;   //����P3.4��P3.6Ϊ©����·(ʵ��������������赽3.3V)
    P4M1 = 0x3c;   P4M0 = 0x3c;   //����P4.2~P4.5Ϊ©����·(ʵ��������������赽3.3V)
    P5M1 = 0x0c;   P5M0 = 0x0c;   //����P5.2��P5.3Ϊ©����·(ʵ��������������赽3.3V)
    P6M1 = 0xff;   P6M0 = 0xff;   //����Ϊ©����·(ʵ��������������赽3.3V)
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���

    display_index = 0;
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
    EA = 1;     //�����ж�
    
    for(i=0; i<8; i++)  LED8[i] = 0x10; //�ϵ�����

    while(1)
    {
        if(B_1ms)   //1ms��
        {
            B_1ms = 0;
            if(++msecond >= 300)    //300ms��
            {
                msecond = 0;
                Temp = ReadTemperature();
                DisplayTemperature(Temp);
            }
        }
    }
}


//========================================================================
// ����: u16 ReadTemperature()
// ����: ��ȡ�¶Ⱥ�����
// ����: none.
// ����: �¶�ֵ.
// �汾: VER1.0
// ����: 2020-7-30
// ��ע: 
//========================================================================
u16 ReadTemperature()
{
    u16 TempH, TempL, Temperature;
	
    DS18B20_Reset();                //�豸��λ
    DS18B20_WriteByte(0xCC);        //����ROM����
    DS18B20_WriteByte(0x44);        //��ʼת������
    while (!DQ);                    //�ȴ�ת�����

    DS18B20_Reset();                //�豸��λ
    DS18B20_WriteByte(0xCC);        //����ROM����
    DS18B20_WriteByte(0xBE);        //���ݴ�洢������
    TempL = DS18B20_ReadByte();     //���¶ȵ��ֽ�
    TempH = DS18B20_ReadByte();     //���¶ȸ��ֽ�
	
    if(TempH & 0xf8)    //�ж��Ƿ�λ����
    {
        MinusFlag = 1;  //���ø�����־
        Temperature = (TempH<<8) | TempL;
        Temperature = ~Temperature + 1;
        Temperature *= 0.625;       //0.0625 * 10������1λС����
    }
    else
    {
        MinusFlag = 0;  //���������־
        Temperature = (((TempH<<8) | TempL) * 0.625); //0.0625 * 10������1λС����
    }

    return Temperature;
}

//========================================================================
// ����: void DisplayTemperature(u16 temp)
// ����: ��ʾ�¶Ⱥ�����
// ����: none.
// ����: �¶�ֵ.
// �汾: VER1.0
// ����: 2020-7-30
// ��ע: 
//========================================================================
void DisplayTemperature(u16 temp)
{
    if(MinusFlag)
    {
        if(temp > 999)
        {
            LED8[3] = DIS_;
            LED8[4] = temp / 1000;
        }
        else
        {
            LED8[3] = DIS_BLACK;
            LED8[4] = DIS_;
        }
    }
    else
    {
        LED8[3] = DIS_BLACK;
        if(temp > 999)
        {
            LED8[4] = temp / 1000;
        }
        else
        {
            LED8[4] = DIS_BLACK;
        }
    }
    LED8[5] = (temp % 1000) / 100;
    LED8[6] = ((temp % 100) / 10) + DIS_DOT;
    LED8[7] = temp % 10;
}

//========================================================================
// ����: void  delay_us(u8 us)
// ����: ��ʱ������
// ����: us,Ҫ��ʱ��us��, ����ֻ֧��1~255us. 
// ����: none.
// �汾: VER1.0
// ����: 2013-4-1
// ��ע: 
//========================================================================
void delay_us(u8 us)
{
    do{
        usrNOP();
        usrNOP();
    }while(--us);
}

/**************************************
��λDS18B20,������豸�Ƿ����
**************************************/
void DS18B20_Reset()
{
    CY = 1;
    while (CY)
    {
        DQ = 0;                     //�ͳ��͵�ƽ��λ�ź�
        delay_us(240);              //��ʱ����480us
        delay_us(240);
        DQ = 1;                     //�ͷ�������
        delay_us(60);               //�ȴ�60us
        CY = DQ;                    //����������
        delay_us(240);              //�ȴ��豸�ͷ�������
        delay_us(180);
    }
}

/**************************************
��DS18B20��1�ֽ�����
**************************************/
u8 DS18B20_ReadByte()
{
    u8 i;
    u8 dat = 0;

    for (i=0; i<8; i++)             //8λ������
    {
        dat >>= 1;
        DQ = 0;                     //��ʼʱ��Ƭ
        delay_us(1);                //��ʱ�ȴ�
        DQ = 1;                     //׼������
        delay_us(1);                //������ʱ
        if (DQ) dat |= 0x80;        //��ȡ����
        delay_us(60);               //�ȴ�ʱ��Ƭ����
    }

    return dat;
}

/**************************************
��DS18B20д1�ֽ�����
**************************************/
void DS18B20_WriteByte(u8 dat)
{
    char i;

    for (i=0; i<8; i++)             //8λ������
    {
        DQ = 0;                     //��ʼʱ��Ƭ
        delay_us(1);                //��ʱ�ȴ�
        dat >>= 1;                  //�ͳ�����
        DQ = CY;
        delay_us(60);               //�ȴ�ʱ��Ƭ����
        DQ = 1;                     //�ָ�������
        delay_us(1);                //�ָ���ʱ
    }
}

/********************** ��ʾɨ�躯�� ************************/
void DisplayScan(void)
{   
    P7 = ~T_COM[7-display_index];
    P6 = ~t_display[LED8[display_index]];
    if(++display_index >= 8)    display_index = 0;  //8λ������0
}

/********************** Timer0 1ms�жϺ��� ************************/
void timer0 (void) interrupt 1
{
    DisplayScan();  //1msɨ����ʾһλ
    B_1ms = 1;      //1ms��־
}

