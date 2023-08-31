//#include "UART.h"
//#include "STC12C5A60S2.h"
//
//#define uchar unsigned char
//#define uint unsigned int
//
//#define FOSC 11059200L      //System frequency
//#define BAUD 9600           //UART baudrate
//
//bit busy;
//extern char UART_sbuf[20];
//extern char count;
//
//void UART_init(void)
//{
//	PCON &= 0x7F;			//�����ʲ�����
//    SCON = 0x50;            //8-bit variable UART
//	AUXR &= 0xBF;			//��ʱ��1ʱ��ΪFosc/12,��12T
//	AUXR &= 0xFE;			//����1ѡ��ʱ��1Ϊ�����ʷ�����
//    TMOD &= 0x0F;			//�����ʱ��1ģʽλ
//	TMOD |= 0x20;			//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
//    TH1 = TL1 = -(FOSC/12/32/BAUD); //Set auto-reload vaule
//	ET1 = 0;				//��ֹ��ʱ��1�ж�
//    TR1 = 1;                //������ʱ��1
//    ES = 1;                 //�򿪴����ж�
//    EA = 1;                 //���ж�����
//}
//
//
///*----------------------------
//UART interrupt service routine
//----------------------------*/
//void Uart_Isr() interrupt 4
//{
//    if (RI)
//    {
//        ES=0;					//��ֹ�ж�
//		count += 1;
//		UART_sbuf[count] = SBUF;
// 		RI=0; 					//���жϱ�־
// 		ES=1;					//�������ж�
//    }
//    if (TI)
//    {
//        TI = 0;             //Clear transmit interrupt flag
//        busy = 0;           //Clear transmit busy flag
//    }
//}
//
///*----------------------------
//Send a byte data to UART
//Input: dat (data to be sent)
//Output:None
//----------------------------*/
//void UART_write_byte(uchar dat)
//{
//    while (busy);           //Wait for the completion of the previous data is sent
//    busy = 1;
//    SBUF = dat;             //Send data to UART buffer
//}
//
///*----------------------------
//Send a string to UART
//Input: s (address of string)
//Output:None
//----------------------------*/
//void UART_write_str(char *s)
//{
//    while (*s)              //Check the end of the string
//    {
//        UART_write_byte(*s++);     //Send current char and increment string ptr
//    }
//}
//
