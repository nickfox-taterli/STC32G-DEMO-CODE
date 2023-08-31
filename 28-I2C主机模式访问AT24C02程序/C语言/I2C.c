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

��STC��MCU��IO��ʽ����8λ�����.

ʹ��Timer0��16λ�Զ���װ������1ms���ģ�������������������£��û��޸�MCU��ʱ��Ƶ��ʱ,�Զ���ʱ��1ms.

ͨ��Ӳ��I2C�ӿڶ�ȡAT24C02ǰ8���ֽ����ݣ������������ʾ.

����ȡ�����ݼ�1��д��AT24C02ǰ8���ֽ�.

���¶�ȡAT24C02ǰ8���ֽ����ݣ������������ʾ.

MCU�ϵ��ִ��1�����϶��������ظ��ϵ�/�ϵ����AT24C02ǰ8���ֽڵ���������.

����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

******************************************/


#include "..\..\comm\STC32G.h"
#include "intrins.h"

#define     MAIN_Fosc       24000000L   //������ʱ��

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;


sbit SDA = P2^4;
sbit SCL = P2^5;

/***********************************************************/

#define DIS_DOT     0x20
#define DIS_BLACK   0x10
#define DIS_        0x11

#define SLAW    0xA0
#define SLAR    0xA1


/****************************** �û������ ***********************************/

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

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

/*************  ���غ�������    **************/
void WriteNbyte(u8 addr, u8 *p, u8 number);
void ReadNbyte( u8 addr, u8 *p, u8 number);
void delay_ms(u8 ms);

/**********************************************/
void main(void)
{
    u8  i;
    u8  tmp[8];

    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x30;   P0M0 = 0x30;   //����P0.4��P0.5Ϊ©����·(ʵ��������������赽3.3V)
    P1M1 = 0x30;   P1M0 = 0x30;   //����P1.4��P1.5Ϊ©����·(ʵ��������������赽3.3V)
    P2M1 = 0x3c;   P2M0 = 0x3c;   //����P2.2~P2.5Ϊ©����·(ʵ��������������赽3.3V)
    P3M1 = 0x50;   P3M0 = 0x50;   //����P3.4��P3.6Ϊ©����·(ʵ��������������赽3.3V)
    P4M1 = 0x3c;   P4M0 = 0x3c;   //����P4.2~P4.5Ϊ©����·(ʵ��������������赽3.3V)
    P5M1 = 0x0c;   P5M0 = 0x0c;   //����P5.2��P5.3Ϊ©����·(ʵ��������������赽3.3V)
    P6M1 = 0xff;   P6M0 = 0xff;   //����Ϊ©����·(ʵ��������������赽3.3V)
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���
    
    display_index = 0;
    
    for(i=0; i<8; i++)  LED8[i] = 0x10; //�ϵ�����
    
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    I2C_S1 =0;   //I2C���ܽ�ѡ��00:P1.5,P1.4; 01:P2.5,P2.4; 11:P3.2,P3.3
    I2C_S0 =1;
    I2CCFG = 0xe0;              //ʹ��I2C����ģʽ
    I2CMSST = 0x00;

    EA = 1;     //�����ж�
    
    ReadNbyte(0, tmp, 8);
    for(i=0; i<8; i++)  LED8[i] = tmp[i] & 0x0f;
    
    for(i=0; i<8; i++)
    {
        tmp[i] += 1;
    }
    WriteNbyte(0, tmp, 8);
    delay_ms(250);
    delay_ms(250);

    ReadNbyte(0, tmp, 8);
    for(i=0; i<8; i++)  LED8[i] = tmp[i] & 0x0f;

    while(1);
} 

/********************** ��ʾɨ�躯�� ************************/
void DisplayScan(void)
{   
    P7 = ~T_COM[7-display_index];
    P6 = ~t_display[LED8[display_index]];
    if(++display_index >= 8)    display_index = 0;  //8λ������0
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

/********************** Timer0 1ms�жϺ��� ************************/
void timer0 (void) interrupt 1
{
    DisplayScan();  //1msɨ����ʾһλ
    B_1ms = 1;      //1ms��־
}


void Wait()
{
    while (!(I2CMSST & 0x40));
    I2CMSST &= ~0x40;
}

void Start()
{
    I2CMSCR = 0x01;                         //����START����
    Wait();
}

void SendData(char dat)
{
    I2CTXD = dat;                           //д���ݵ����ݻ�����
    I2CMSCR = 0x02;                         //����SEND����
    Wait();
}

void RecvACK()
{
    I2CMSCR = 0x03;                         //���Ͷ�ACK����
    Wait();
}

char RecvData()
{
    I2CMSCR = 0x04;                         //����RECV����
    Wait();
    return I2CRXD;
}

void SendACK()
{
    I2CMSST = 0x00;                         //����ACK�ź�
    I2CMSCR = 0x05;                         //����ACK����
    Wait();
}

void SendNAK()
{
    I2CMSST = 0x01;                         //����NAK�ź�
    I2CMSCR = 0x05;                         //����ACK����
    Wait();
}

void Stop()
{
    I2CMSCR = 0x06;                         //����STOP����
    Wait();
}

void WriteNbyte(u8 addr, u8 *p, u8 number)  /*  WordAddress,First Data Address,Byte lenth   */
{
    Start();                                //������ʼ����
    SendData(SLAW);                         //�����豸��ַ+д����
    RecvACK();
    SendData(addr);                         //���ʹ洢��ַ
    RecvACK();
    do
    {
        SendData(*p++);
        RecvACK();
    }
    while(--number);
    Stop();                                 //����ֹͣ����
}

void ReadNbyte(u8 addr, u8 *p, u8 number)   /*  WordAddress,First Data Address,Byte lenth   */
{
    Start();                                //������ʼ����
    SendData(SLAW);                         //�����豸��ַ+д����
    RecvACK();
    SendData(addr);                         //���ʹ洢��ַ
    RecvACK();
    Start();                                //������ʼ����
    SendData(SLAR);                         //�����豸��ַ+������
    RecvACK();
    do
    {
        *p = RecvData();
        p++;
        if(number != 1) SendACK();          //send ACK
    }
    while(--number);
    SendNAK();                              //send no ACK	
    Stop();                                 //����ֹͣ����
}
/****************************/

