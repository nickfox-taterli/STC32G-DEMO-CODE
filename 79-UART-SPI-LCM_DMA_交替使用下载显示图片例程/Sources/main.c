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

UART DMA + SPII DMA + LCM DMA����Һ��������

����ͼƬʱͨ������DMA����ͼƬ���ݣ�SPI DMA���ֵ�Flash�

��ʾʱSPI DMA��ȡFlashͼƬ���ݣ�LCM DMA��ʾͼƬ��

����ʹ�õ�SPI Flashһ�����ֻ��д��256�ֽ����ݣ�UART����SPIд��Flash��DMA���ݳ���ֻ����256�ֽڡ�

SPI��ȡFlash��LCM��ʾDMA�����ݳ�������2048�ֽڡ�

����DMA�����������DMA����������ʹ�á�

8bit I8080ģʽ, P6�ڽ�������

LCD_RS = P4^5      //����/�����л�
LCD_WR = P4^2      //д����
LCD_RD = P4^4      //������
LCD_CS = P3^4      //Ƭѡ
LCD_RESET = P4^3   //��λ

SPI Flash �ӿڣ�
SPI_SS = P2^2;
SPI_MOSI = P2^3;
SPI_MISO = P2^4;
SPI_SCLK = P2^5;

UART �ӿڣ�
RX = P3^0
TX = P3^1

����ʱ, ѡ��ʱ�� 35MHz (�û��������޸�Ƶ��).

******************************************/

#include "STC32G.h"
#include "system.h"
#include "spi_flash.h"
#include "uart.h"
#include "lcm.h"
#include "stdio.h"
#include "iap_eeprom.h"

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

#define KEY_TIMER 30        //�����������ʱ��(ms)

sbit KEY1 = P3 ^ 2;
sbit KEY2 = P3 ^ 3;

u16 Key1_cnt;
u16 Key2_cnt;
bit Key1_Flag;
bit Key2_Flag;
bit Key1_Short_Flag;
bit Key2_Short_Flag;

bit B_1ms;          //1ms��־
bit Mode_Flag;
bit AutoDisplayFlag;
u32 Max_addr;

u16 MSecond;

void GPIO_Init(void);
void Timer0_Init(void);
void KeyScan(void);
void UART_SPI_DMA_BufSwitch(void);
void LCM_SPI_DMA_BufSwitch(void);

void main(void)
{
    u8 temp[4];
    
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

	GPIO_Init();
    Timer0_Init();
    
	LCM_Config();
	LCM_DMA_Config();
    
    UART1_config(1);  // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
//    UART1_DMA_Config();
	EA = 1;

    SPI_init();
    SPI_DMA_Config();
	
	LCD_Init(); //LCM��ʼ��
    
    EEPROM_read_n(EE_ADDRESS,temp,4);    //����4�ֽ�
    Max_addr = ((u32)temp[0] << 24) | ((u32)temp[1] << 16) | ((u32)temp[2] << 8) | (u32)temp[3];
    if(Max_addr == 0xffffffff)
    {
        Max_addr = 0;
    }

	while(1)
	{
        if(Mode_Flag == 0)
        {
            if(!SpiDmaFlag && !LcmDmaFlag && (lcdIndex > 0))
            {
                LCM_SPI_DMA_BufSwitch();
                LcmDmaFlag = 1;
                DMA_LCM_CR = 0xa0;	//LCM DMA Write dat

                Flash_addr += DMA_AMT_LEN;
                SPI_Read_Nbytes(Flash_addr,DMA_AMT_LEN);

    //            printf("LCD index=%u\r\n",lcdIndex);
            }
        }
        else if(Mode_Flag == 1)
        {
            if(!SpiDmaFlag && UartDmaRxFlag)
            {
                UartDmaRxFlag = 0;
                UART_SPI_DMA_BufSwitch();
                SPI_Write_Nbytes(Flash_addr,DMA_WR_LEN);
                Flash_addr += DMA_WR_LEN;
                DMA_UR1R_CR = 0xa0;			//bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO
            }
        }
        
		if(B_1ms)   //1ms��
		{
			B_1ms = 0;
            KeyScan();
            
            if(AutoDisplayFlag)
            {
                MSecond++;
                if(MSecond >= 1000)
                {
                    MSecond = 0;

                    Mode_Flag = 0;
                    if(Flash_addr >= Max_addr)
                    {
                        Flash_addr = 0;
                        DmaBufferSW = 0;
                    }
                    
                    lcdIndex = 75;  //75 * 2048 = 320 * 240 * 2
                    LCD_Display();
                    SPI_DMA_Config();
                    LCM_DMA_Config();

                    SPI_Read_Nbytes(Flash_addr,DMA_AMT_LEN);
                }
            }
            
			if(RX1_TimeOut > 0)     //��ʱ����
			{
				if(--RX1_TimeOut == 0)
				{
                    UART_SPI_DMA_BufSwitch();
                    SPI_Write_Nbytes(Flash_addr,(((u16)DMA_UR1R_DONEH<<8)+DMA_UR1R_DONE));
                    Flash_addr += (((u16)DMA_UR1R_DONEH<<8)+DMA_UR1R_DONE);
                    Max_addr = Flash_addr;
                    
                    //�رս���DMA���´ν��յ��������´������ʼ��ַλ�ã������´ν������ݼ����������š�
					DMA_UR1R_CR = 0x00;			//bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO
					printf("\r\nUART1 Timeout receive end!\r\n");  //UART1����һ���ַ���

                    temp[0] = (u8)(Max_addr >> 24);
                    temp[1] = (u8)(Max_addr >> 16);
                    temp[2] = (u8)(Max_addr >> 8);
                    temp[3] = (u8)Max_addr;
                    EEPROM_SectorErase(EE_ADDRESS); //��������
                    EEPROM_write_n(EE_ADDRESS,temp,4);
				}
			}
        }
	}
}

//========================================================================
// ����: void UART_SPI_DMA_BufSwitch(void)
// ����: UART��SPI��DMA�������л�������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-8-26
// ��ע: 
//========================================================================
void UART_SPI_DMA_BufSwitch(void)
{
    if(DmaBufferSW)
    {
        DmaBufferSW = 0;
        DMA_UR1R_RXAH = (u8)((u16)&DmaBuffer1 >> 8);
        DMA_UR1R_RXAL = (u8)((u16)&DmaBuffer1);
        DMA_SPI_TXAH = (u8)((u16)&DmaBuffer2 >> 8);	//SPI�������ݴ洢��ַ
        DMA_SPI_TXAL = (u8)((u16)&DmaBuffer2);
    }
    else
    {
        DmaBufferSW = 1;
        DMA_UR1R_RXAH = (u8)((u16)&DmaBuffer2 >> 8);
        DMA_UR1R_RXAL = (u8)((u16)&DmaBuffer2);
        DMA_SPI_TXAH = (u8)((u16)&DmaBuffer1 >> 8);	//SPI�������ݴ洢��ַ
        DMA_SPI_TXAL = (u8)((u16)&DmaBuffer1);
    }
}

//========================================================================
// ����: void LCM_SPI_DMA_BufSwitch(void)
// ����: LCM��SPI��DMA�������л�������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-8-26
// ��ע: 
//========================================================================
void LCM_SPI_DMA_BufSwitch(void)
{
    if(DmaBufferSW)
    {
        DmaBufferSW = 0;
        DMA_LCM_TXAH = (u8)((u16)&DmaBuffer2 >> 8);
        DMA_LCM_TXAL = (u8)((u16)&DmaBuffer2);
        DMA_SPI_RXAH = (u8)((u16)&DmaBuffer1 >> 8);	//SPI�������ݴ洢��ַ
        DMA_SPI_RXAL = (u8)((u16)&DmaBuffer1);
    }
    else
    {
        DmaBufferSW = 1;
        DMA_LCM_TXAH = (u8)((u16)&DmaBuffer1 >> 8);
        DMA_LCM_TXAL = (u8)((u16)&DmaBuffer1);
        DMA_SPI_RXAH = (u8)((u16)&DmaBuffer2 >> 8);	//SPI�������ݴ洢��ַ
        DMA_SPI_RXAL = (u8)((u16)&DmaBuffer2);
    }
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

//========================================================================
// ����: void GPIO_Init(void)
// ����: IO�����ú�����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-8-24
// ��ע: 
//========================================================================
void GPIO_Init(void)
{
    //©����·+��������ģʽ�ź������ٶ�������Ҫ���ٲ�������ͨ�ţ�Ӱ����ʾ�ٶ�
//    P0M1 = 0x30;   P0M0 = 0x30;   //����P0.4��P0.5Ϊ©����·(ʵ��������������赽3.3V)
//    P1M1 = 0x32;   P1M0 = 0x32;   //����P1.1��P1.4��P1.5Ϊ©����·(ʵ��������������赽3.3V), P1.1��PWM��DAC��·ͨ�����贮����P2.3
//    P2M1 = 0x3c;   P2M0 = 0x3c;   //����P2.2~P2.5Ϊ©����·(ʵ��������������赽3.3V)
//    P3M1 = 0x50;   P3M0 = 0x50;   //����P3.4��P3.6Ϊ©����·(ʵ��������������赽3.3V)
//    P4M1 = 0x3c;   P4M0 = 0x3c;   //����P4.2~P4.5Ϊ©����·(ʵ��������������赽3.3V)
//    P5M1 = 0x0c;   P5M0 = 0x0c;   //����P5.2��P5.3Ϊ©����·(ʵ��������������赽3.3V)
//    P6M1 = 0xff;   P6M0 = 0xff;   //����Ϊ©����·(ʵ��������������赽3.3V)
//    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���

    P0M1 = 0x00;   P0M0 = 0x00;   //����Ϊ׼˫���
    P1M1 = 0x00;   P1M0 = 0x00;   //����Ϊ׼˫���
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;   //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���
    
	//P6�����ó��������
	P6M0=0xff;
	P6M1=0x00;

	//P3.4�����ó��������
	P3M0=0x10;
	P3M1=0x00;

	//P4.2,P4.3,P4.4,P4.5�����ó��������
	P4M0=0x3c;
	P4M1=0x00;
}

//========================================================================
// ����: void Timer0_Init(void)
// ����: ��ʱ��0���ú�����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-8-24
// ��ע: 
//========================================================================
void Timer0_Init(void)
{
	AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
	TH0 = (u8)(Timer0_Reload / 256);
	TL0 = (u8)(Timer0_Reload % 256);
	ET0 = 1;    //Timer0 interrupt enable
	TR0 = 1;    //Tiner0 run
}

//========================================================================
// ����: void timer0_Interrupt(void) interrupt 1
// ����: ��ʱ��0�жϺ�����
// ����: nine.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void timer0_Interrupt(void) interrupt 1
{
    B_1ms = 1;      //1ms��־
}

//========================================================================
// ����: void KeyScan(void)
// ����: ����ɨ�躯����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2013-4-1
// ��ע: 
//========================================================================
void KeyScan(void)
{
	if(!KEY1)
	{
		if(!Key1_Flag)
		{
			Key1_cnt++;
			if(Key1_cnt >= 1000)		//����1s
			{
				Key1_Short_Flag = 0;	//����̰���־
				Key1_Flag = 1;			//���ð���״̬����ֹ�ظ�����

                printf("LCM auto display...\r\n");

                DMA_SPI_CR = 0x00;		//bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO
                DMA_SPI_STA = 0x00;
                DMA_UR1R_CR = 0x00;			//bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO
                DMA_UR1R_STA = 0x00;
                
                AutoDisplayFlag = 1;
                Flash_addr = 0;
                DmaBufferSW = 0;

                lcdIndex = 75;  //75 * 2048 = 320 * 240 * 2
                LCD_Display();
//                printf("Start LCD display...\r\n");
                SPI_DMA_Config();
                LCM_DMA_Config();

                SPI_Read_Nbytes(Flash_addr,DMA_AMT_LEN);
            }
			else if(Key1_cnt >= KEY_TIMER)	//30ms����
			{
				Key1_Short_Flag = 1;		//���ö̰���־
			}
		}
	}
	else
	{
		if(Key1_Short_Flag)			//�ж��Ƿ�̰�
		{
			Key1_Short_Flag = 0;	//����̰���־

            DMA_SPI_CR = 0x00;		//bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO
            DMA_SPI_STA = 0x00;
            DMA_UR1R_CR = 0x00;			//bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO
            DMA_UR1R_STA = 0x00;
            
            Mode_Flag = 0;
            AutoDisplayFlag = 0;
            printf("Max_addr = %lu, Flash_addr = %lu\r\n",Max_addr,Flash_addr);
            if(Flash_addr >= Max_addr)
            {
                Flash_addr = 0;
                DmaBufferSW = 0;
            }
            
            lcdIndex = 75;  //75 * 2048 = 320 * 240 * 2
            LCD_Display();
//          printf("Start LCD display...\r\n");
            SPI_DMA_Config();
            LCM_DMA_Config();

            SPI_Read_Nbytes(Flash_addr,DMA_AMT_LEN);
		}
		Key1_cnt = 0;
		Key1_Flag = 0;
	}

	if(!KEY2)
	{
		if(!Key2_Flag)
		{
			Key2_cnt++;

			if(Key2_cnt >= 1000)		//����1s
			{
				Key2_Short_Flag = 0;	//����̰���־
				Key2_Flag = 1;			//���ð���״̬����ֹ�ظ�����

                DMA_LCM_CR = 0x00;
                DMA_LCM_STA = 0x00;
                DMA_SPI_CR = 0x00;		//bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO
                DMA_SPI_STA = 0x00;
                DMA_UR1R_CR = 0x00;		//bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO
                DMA_UR1R_STA = 0x00;
                
                Mode_Flag = 1;
                DmaBufferSW = 0;
                Flash_addr = 0;
                Max_addr = 0;
                printf("Start Flash Chip Erase...\r\n");
                FlashChipErase();
                printf("Flash Chip Erase OK!\r\n");
            }
			else if(Key2_cnt >= KEY_TIMER)	//30ms����
			{
				Key2_Short_Flag = 1;		//���ö̰���־
			}
		}
	}
	else
	{
		if(Key2_Short_Flag)			//�ж��Ƿ�̰�
		{
			Key2_Short_Flag = 0;	//����̰���־

            Mode_Flag = 1;
            Flash_addr = Max_addr;  //�������ͼƬ
            printf("Ready to receive data...\r\n");
            UART1_DMA_Config();
            SPI_DMA_Config();
		}
		Key2_cnt = 0;
		Key2_Flag = 0;	//�����ͷ�
	}
}