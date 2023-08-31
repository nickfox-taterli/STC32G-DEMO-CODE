/*Register of MCU and Pins defined here*/

#ifndef CONFIG_STC_H
#define CONFIG_STC_H

#include "stc32g.h"
//#define DEBUG

#define MAIN_Fosc       35000000L   //������ʱ��
#define BAUD            (65536 - MAIN_Fosc/4/115200)

#define VirtualDevice   1    //0: ����Ӳ��OLED;  1: �������Խӿ�OLED-12864�����豸


/* ADC����ͨ������
   ADC Channel Setup */
#define ADC_DSO ADC_CHS_14
#define ADC_BAT ADC_CHS_04

/* ��������ָʾ��
   Indicator for sampling running */
sbit P_Ready = P2 ^ 7;

/*  OLED Pin */
sbit OLED_SCLK = P2 ^ 5; //ʱ�ӣ�D0/SCLK��
sbit OLED_SDIN = P2 ^ 4; //���ݣ�D1/MOSI��
sbit OLED_RST = P2 ^ 3;  //��λ��RES��
sbit OLED_DC = P3 ^ 4;   //����/������ƣ�DC��
sbit OLED_CS = P1 ^ 1;   //Ƭѡ��CS��

#ifdef DEBUG
sbit P15 = P1 ^ 5;
#endif

#endif