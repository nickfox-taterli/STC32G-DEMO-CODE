#ifndef __MENU_H
#define __MENU_H

#include "middle.h"


#define Null ((void *)0)
	
struct MenuItem     
{  
    char MenuCount;                 //�ṹ�������Ԫ�ظ��� 
    unsigned char DisplayString[20];//��ǰLCD��ʾ����Ϣ 
    void (*Subs)();                 //ִ�еĺ�����ָ��. 
    struct MenuItem *Childrenms;    //ָ���ӽڵ��ָ�� 
    struct MenuItem *Parentms;      //ָ�򸸽ڵ��ָ�� 
}; 

struct MenuIndex
{
	char Menu_Depth;					//��ǰ�˵����
	char Menu_Start[10];				//��ǰ��һ����ʾ���Ǳ�ż�
    char Menu_Pilot[10];				//�˵���������ǰѡ�е�Ϊ�ڼ���
	char Menu_KeyFunc;					//0:�˵��л�    1:�˵���Ĳ�������
};

extern struct MenuItem *P_menu;     //��ǰ�˵��ڵ��ָ��
extern struct MenuIndex P_Index;	 //��ǰ�˵��ĵ�����Ϣ
extern void  (*function)(u8 num);			//ϵͳ��ָ�뺯��

void Menu_Init(void);
void Menu_Display(void);
void Menu_Deal(char* key);

void Menu_MainFunc1(void);
void Menu_MainFunc2(void);
void Menu_MainFunc31(void);
void Menu_MainFunc32(void);
void Menu_MainFunc33(void);
void Menu_MainFunc4(void);
void Menu_MainFunc5(void);
void Menu_MainFunc6(void);
void Menu_MainFunc61(void);
void Menu_MainFunc7(void);
void Menu_MainFunc8(void);

#endif


