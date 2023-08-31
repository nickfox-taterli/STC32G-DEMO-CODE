#ifndef __OLED_H
#define __OLED_H


#define Drive_Func  0  //0:CDC���⴮��   1��Ӳ��SPI������ʵ�֣�

extern  unsigned char xdata ShowBUFF[8][128];

void OLED_Init(void);
void OLED_DisplayOn(void);                                          //����ʾ
void OLED_DisplayOff(void);                                         //����ʾ
void OLED_LightSet(unsigned char num);     //��������
    
void OLED_ShowChar(unsigned char x,unsigned char y,char asc);       //OLED��ʾһ��8*16�ַ�
void OLED_ShowGBK(unsigned char x,unsigned char y,char *gbk);       //OLED��ʾһ��16*16�ַ�
void OLED_ShowString(unsigned char x,unsigned char y,char *s);

    
void OLED_BuffClear(void);     //����(CDCδʵ��)
void OLED_BuffShow(void)     ;
void OLED_BuffShowChar(unsigned char x,unsigned char y,char asc);     //OLED��ʾһ��8*16�ַ�
void OLED_BuffShowGBK(unsigned char x,unsigned char y,char *gbk);     //OLED��ʾһ��16*16�ַ�
void OLED_BuffShowString(unsigned char x,unsigned char y,char *s);
void OLED_BuffShowPoint(unsigned char x,unsigned char y);     //OLED��ʾһ��8*16�ַ�
void OLED_BuffShowLine( unsigned char x1, unsigned char y1, unsigned char x2,unsigned char y2);
void OLED_BuffShowAscll32(unsigned char  x,unsigned char  y,unsigned char num);
void OLED_BuffShowCharOpp(unsigned char x,unsigned char y,char asc) ;
void Cursor_Blink( unsigned char x,unsigned char y,unsigned char count );
#endif



