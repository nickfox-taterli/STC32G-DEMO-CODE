#ifndef _KEYBOARD_INPUT_H_
#define _KEYBOARD_INPUT_H_

#include "stc.h"

typedef enum
{
	INPUT_NORMAL,
	INPUT_AC,
	INPUT_EQ,
	INPUT_UP,
	INPUT_DOWN,
//	INPUT_COPY,
	INPUT_VACANT	//����
}INPUT_FLAG;

uchar input_to_str(uchar, char *,uchar num1);  //�ڵڼ�����ʾ y~��0,3��
void t0_init(void);						//��ʱ����ʼ��
void close_cursor(void);		//�ع��
void open_cursor(void);			//�����

#endif
