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

�����̻���STC32GΪ����оƬ��ʵ����9.6���б�д���ԡ�

9.6֮ǰʵ����汾��12864ģ��ӿڣ�P4.2��P4.4�ӿ���Ҫ���е�������ʹ��Ӳ��LCM�ӿ�ͨ��.

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

LCM�ӿ�����Һ��������

8bit M6800ģʽ, P6�ڽ�D0~D7

sbit LCD_RS = P4^5;      //����/�����л�
sbit LCD_RW = P4^2;      //��д����
sbit LCD_E = P4^4;       //ʹ��
sbit LCD_RESET = P3^4;   //��λ 

LCMָ��ͨ���жϷ�ʽ�ȴ��������

DMA���ó���16�ֽڣ�ͨ���жϷ�ʽ�жϴ������

����ʱ�� 1.2MHz (128*64��, ST7920����, �ٶ�̫����ʾ�᲻����).
(ͨ��USB���ز����Զ���ʱ�ӣ���Ҫʹ�ô��ڷ�ʽ����)

******************************************/

#include "..\..\comm\STC32G.h"
#include "stdio.h"
#include "intrins.h"
#include "LCD_IMG.h"

#define     MAIN_Fosc       1200000L   //�������ʱ�Ӳ���

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

sbit LCD_RS = P4^5;      //����/�����л�
sbit LCD_RW = P4^2;      //��д����
sbit LCD_E = P4^4;       //ʹ��
//sbit LCD_PSB = P3^5;     //PSB��Ϊ12864�Ĵ�����ͨѶ�����л�������ʹ��8λ���нӿڣ�PSB=1
sbit LCD_RESET = P3^4;   //��λ 


//IO����
#define  LCD_DataPort P6     //8λ���ݿ�

#define  USR_LCM_IF     1			//1: use LCM Interface

u16 index;
bit DmaFlag=0;
bit LcmFlag=0;

u8  code uctech[] = {"��ͨ��о΢����  "};
u8  code net[]    = {" www.stcmcu.com "};
u8  code mcu[]    = {"רҵ����51��Ƭ��"};
u8  code qq[]     = {" QQ: 800003751  "};

void delay_ms(u16 ms);
void GPIO_Init(void);
void LCM_Config(void);
void DMA_Config(void);
void LCD_Init(void);
void DisplayImage (u8 *DData);
void DisplayListChar(u8 X, u8 Y, u8 code *DData);
void Write_Cmd(unsigned char WCLCD,unsigned char BuysC);
void Write_Data(unsigned char WDLCD);
void LCDClear(void);

void main(void)
{
//    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ��죨��������Ҫ�����ٶȲ���������ʾ��
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

//	GPIO_Init();
#if USR_LCM_IF == 1	
	LCM_Config();
	DMA_Config();
	EA = 1;
#endif
	
//	delay_ms(100); //�����ȴ�����LCD���빤��״̬
	LCD_Init(); //LCM��ʼ��

	while(1)
	{
		LCDClear();
		DisplayImage(gImage_gxw);
		delay_ms(200);
		LCDClear();
		DisplayListChar(0,1,uctech);    //��ʾ�ֿ��е���������
		DisplayListChar(0,2,net);       //��ʾ�ֿ��е���������
		DisplayListChar(0,3,mcu);       //��ʾ�ֿ��е�����
		DisplayListChar(0,4,qq);        //��ʾ�ֿ��е���������
		delay_ms(200);
	}
}

//��ָ��λ����ʾһ���ַ�
void DisplayListChar(u8 X, u8 Y, u8 code *DData)
{
    u8 ListLength,X2;
    ListLength = 0;
    X2 = X;
    if(Y < 1)   Y=1;
    if(Y > 4)   Y=4;
    X &= 0x0F; //����X���ܴ���16��Y��1-4֮��
    switch(Y)
    {
        case 1: X2 |= 0X80; break;  //����������ѡ����Ӧ��ַ
        case 2: X2 |= 0X90; break;
        case 3: X2 |= 0X88; break;
        case 4: X2 |= 0X98; break;
    }
    Write_Cmd(X2, 1); //���͵�ַ��
    while (DData[ListLength] >= 0x20) //�������ִ�β���˳�
    {
        if (X <= 0x0F) //X����ӦС��0xF
        {
            Write_Data(DData[ListLength]); //
            ListLength++;
            X++;
        }
    }
}

//ͼ����ʾ122*32
void DisplayImage (u8 *DData)
{
#if USR_LCM_IF == 0
    u8 y;
#endif
    u8 x,i;
    unsigned int tmp=0;
	
    for(i=0;i<9;)       //���������ϰ������°�������Ϊ��ʼ��ַ��ͬ����Ҫ�ֿ�
    {
        for(x=0;x<32;x++)   //32��
        {
            Write_Cmd(0x34,1);
            Write_Cmd((u8)(0x80+x),1);    //�е�ַ
            Write_Cmd((u8)(0x80+i),1);    //�е�ַ���°������������е�ַ0X88
            Write_Cmd(0x30,1);
					
#if USR_LCM_IF == 1

            DMA_LCM_TXAH = (u8)((u16)&DData[tmp] >> 8);
            DMA_LCM_TXAL = (u8)((u16)&DData[tmp]);
            while(DmaFlag);
            DmaFlag = 1;
            DMA_LCM_CR = 0xa0;	//Write data

#else

            for(y=0;y<16;y++)
                Write_Data(DData[tmp+y]);//��ȡ����д��LCD

#endif
            tmp+=16;
        }
        i+=8;
    }
    Write_Cmd(0x36,1);    //���书���趨
    Write_Cmd(0x30,1);
}

//========================================================================
// ����: void delay_ms(u16 ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2013-4-1
// ��ע: 
//========================================================================
void delay_ms(u16 ms)
{
    u16 i;
    do{
        i = MAIN_Fosc / 6000;
        while(--i);
    }while(--ms);
}

#if USR_LCM_IF == 0
void LCD_delay(void)
{
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
}
#endif

//��״̬
void ReadStatusLCD(void)
{
#if USR_LCM_IF == 1

    do{
        LcmFlag = 1;
        LCMIFCR = 0x86;		//Enable interface, Read command
        while(LcmFlag);
        LCD_E = 0;
    }while(LCMIFDATL & 0x80);
	
#else
	
	LCD_RS = 0;
	LCD_RW = 1; 
	LCD_delay();
	LCD_E = 1;
	LCD_delay();
	while (LCD_DataPort & 0x80); //���æ�ź�
	LCD_E = 0;

#endif
}

void Write_Cmd(unsigned char WCLCD,unsigned char BuysC)
{
#if USR_LCM_IF == 1
	
	if (BuysC) ReadStatusLCD(); //������Ҫ���æ 
	LCMIFDATL = WCLCD;
	LcmFlag = 1;
	LCMIFCR = 0x84;		//Enable interface, write command out
	while(LcmFlag);
	
#else
	
	if (BuysC) ReadStatusLCD(); //������Ҫ���æ 
	LCD_RS = 0;
	LCD_RW = 0; 
	LCD_DataPort = WCLCD;
	LCD_delay();
	LCD_E = 1; 
	LCD_delay();
	LCD_E = 0;  
	
#endif
}

void Write_Data(unsigned char WDLCD)
{
#if USR_LCM_IF == 1
	
	LCMIFDATL = WDLCD;
	LcmFlag = 1;
	LCMIFCR = 0x85;		//Enable interface, write data out
	while(LcmFlag);
	
#else
	
	ReadStatusLCD(); //���æ 
	LCD_RS = 1;
	LCD_RW = 0;
	LCD_DataPort = WDLCD;
	LCD_delay();
	LCD_E = 1;
	LCD_delay();
	LCD_E = 0;
	
#endif
}

/*****************************************************************************
 * @name       :void GPIO_Init(void)
 * @date       :2018-11-13 
 * @function   :Set the gpio to push-pull mode
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
//void GPIO_Init(void)
//{
//	//P6�����ó��������
//	P6M0 |= 0xff;
//	P6M1 &= 0x00;

//	//P3.4�����ó��������
//	P3M0 |= 0x10;
//	P3M1 &= ~0x10;

//	//P4.2,P4.4,P4.5�����ó��������
//	P4M0 |= 0x34;
//	P4M1 &= ~0x34;
//}

#if USR_LCM_IF == 1	
/*****************************************************************************
 * @name       :void LCM_Config(void)
 * @date       :2018-11-13 
 * @function   :Config LCM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCM_Config(void)
{
	LCMIFCFG = 0x85;   //bit7 1:Enable Interrupt, bit1 0:8bit mode; bit0 0:8080,1:6800
	LCMIFCFG2 = 0x1f;  //RS:P45,RD:P44,WR:P42; Setup Time,HOLD Time
	LCMIFSTA = 0x00;
}

/*****************************************************************************
 * @name       :void DMA_Config(void)
 * @date       :2020-12-09 
 * @function   :Config DMA
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void DMA_Config(void)
{
	DMA_LCM_AMT = 0x0f;		//���ô������ֽ���(��8λ)��n+1
	DMA_LCM_AMTH = 0x00;	//���ô������ֽ���(��8λ)��n+1
	DMA_LCM_TXAH = (u8)((u16)&gImage_gxw >> 8);
	DMA_LCM_TXAL = (u8)((u16)&gImage_gxw);
	DMA_LCM_STA = 0x00;
	DMA_LCM_CFG = 0x82;
	DMA_LCM_CR = 0x80;
}
#endif

/*****************************************************************************
 * @name       :void LCDReset(void)
 * @date       :2018-08-09 
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCDReset(void)
{
//	LCD_PSB = 1;    //����
	delay_ms(10);
	LCD_RESET = 0;
	delay_ms(10);
	LCD_RESET = 1;
	delay_ms(100);
}

/*****************************************************************************
 * @name       :void LCD_Init(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 	 
void LCD_Init(void)
{
	LCDReset(); //��ʼ��֮ǰ��λ
//*************LCD ��ʼ��**********//	
	Write_Cmd(0x30,1); //��ʾģʽ����,��ʼҪ��ÿ�μ��æ�ź�
	Write_Cmd(0x01,1); //��ʾ����
	Write_Cmd(0x06,1); // ��ʾ����ƶ�����
	Write_Cmd(0x0C,1); // ��ʾ�����������
}

void LCDClear(void) //����
{
	Write_Cmd(0x01,1); //��ʾ����
	Write_Cmd(0x34,1); // ��ʾ����ƶ�����
	Write_Cmd(0x30,1); // ��ʾ�����������
}

/*****************************************************************************
 * @name       :void LCM_Interrupt(void)
 * @date       :2018-11-13 
 * @function   :None
 * @parameters :None
 * @retvalue   :
******************************************************************************/ 
void LCMIF_DMA_Interrupt(void) interrupt 13
{
	if(LCMIFSTA & 0x01)
	{
		LCMIFSTA = 0x00;
		LcmFlag = 0;
	}
	
	if(DMA_LCM_STA & 0x01)
	{
		DmaFlag = 0;
		DMA_LCM_CR = 0;
		DMA_LCM_STA = 0;
	}
}
