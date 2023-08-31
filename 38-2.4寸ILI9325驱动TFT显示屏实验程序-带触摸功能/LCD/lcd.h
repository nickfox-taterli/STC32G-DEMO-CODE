/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* Һ������������ο�wiki������վ�ṩ�Ŀ�ԴԴ�룬����ѧϰʹ��          */
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����        */
/*---------------------------------------------------------------------*/

//=========================================��Դ����================================================//
//VDD��DC 3.3V��Դ
//GND�ӵ�
//=======================================Һ���������߽���==========================================//
//��ģ��������������Ϊ8λ����
//Һ����ģ��               STC32G
// LCD_D0~D7     ��       P60~P67        //Һ����8λ������
//=======================================Һ���������߽���==========================================//
//Һ����ģ��               STC32G
// LCD_RS        ��        P45           //����/����ѡ������ź�
// LCD_RD        ��        P44           //�������ź�
// LCD_RST       ��        P43           //��λ�ź�
// LCD_WR        ��        P42           //д�����ź�
// LCD_CS        ��        P34           //Ƭѡ�����ź�
//===========================================����������============================================//
//��ʹ�ô�������ģ�鱾������������ɲ�����
//������ʹ�õ�������������ΪSPI
//Һ����ģ��               STC32G
//  T_CS         ��        P52          //������Ƭѡ�����ź�
//  T_CLK        ��        P25          //������SPIʱ���ź�
//  T_DO         ��        P24          //������SPI���ź�
//  T_DIN        ��        P23          //������SPIд�ź�
//  T_IRQ        ��        P36          //��������Ӧ����ź�
//================================================================================================//
// ��ʵ�����ʹ�õ�2.4��320*240�Ĳ��ڲ����������Ա��� https://kcer001.taobao.com/ �ṩ����Ʒ�����ϲ���ͨ����
//**************************************************************************************************/
#ifndef __LCD_H
#define __LCD_H		
#include "sys.h"

/////////////////////////////////////�û�������///////////////////////////////////	 
//֧�ֺ��������ٶ����л�
#define USE_HORIZONTAL  	  1   //����Һ������ʾ���� 	0-������1-����
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////	  
//����LCD�ĳߴ�
#define LCD_W 240
#define LCD_H 320

#define WINDOW_XADDR_START	0x0050 // Horizontal Start Address Set
#define WINDOW_XADDR_END		0x0051 // Horizontal End Address Set
#define WINDOW_YADDR_START	0x0052 // Vertical Start Address Set
#define WINDOW_YADDR_END	0x0053 // Vertical End Address Set
#define GRAM_XADDR		    0x0020 // GRAM Horizontal Address Set
#define GRAM_YADDR		    0x0021 // GRAM Vertical Address Set
#define GRAMWR 			    0x0022 // memory write

//IO����
#define  LCD_DataPort P6     //8λ���ݿ�

sbit LCD_RS = P4^5;      //����/�����л�
sbit LCD_WR = P4^2;      //д����
sbit LCD_RD = P4^4;      //������
sbit LCD_CS = P3^4;      //Ƭѡ	
sbit LCD_RESET = P4^3;   //��λ 

//LCD�Ļ�����ɫ�ͱ���ɫ	   
extern u16  POINT_COLOR;//Ĭ�Ϻ�ɫ    
extern u16  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ
//LCD��Ҫ������
typedef struct  
{										    
	u16 width;			//LCD ���
	u16 height;			//LCD �߶�
	u16 id;					//LCD ID
	u8  dir;				//���������������ƣ�0��������1��������	
	u16	 wramcmd;		//��ʼдgramָ��
	u16  rramcmd;   //��ʼ��gramָ��
	u16  setxcmd;		//����x����ָ��
	u16  setycmd;		//����y����ָ��	 
}_lcd_dev; 	

//LCD����
extern _lcd_dev lcddev;	//����LCD��Ҫ����
void LCD_Init(void); 
void LCD_Clear(u16 Color);
void LCD_write(u8 VAL);
u16 LCD_read(void);
void LCD_WR_DATA(u8 Data);
u16 LCD_RD_DATA(void);
void LCD_WR_REG(u8 Reg);
void LCD_SetCursor(u16 Xpos, u16 Ypos);//���ù��λ��
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);//������ʾ����
void LCD_DrawPoint(u16 x,u16 y);//����
u16 LCD_ReadPoint(u16 x,u16 y);
void LCD_direction(u8 direction );
void LCD_WR_DATA_16Bit(u16 Data);
u16 Lcd_RD_DATA_16Bit(void);
//void LCD_WriteReg(u8 LCD_Reg, u8 LCD_RegValue);
void LCD_ReadReg(u8 LCD_Reg,u8 *Rval,int n);
u16 LCD_Read_ID(void);
u16 Color_To_565(u8 r, u8 g, u8 b);


//������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE             0x001F  
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN            0XBC40 //��ɫ
#define BRRED            0XFC07 //�غ�ɫ
#define GRAY             0X8430 //��ɫ
//GUI��ɫ

#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
//������ɫΪPANEL����ɫ 
 
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
#define LGRAY            0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)


					  		 
#endif  
	 
	 



