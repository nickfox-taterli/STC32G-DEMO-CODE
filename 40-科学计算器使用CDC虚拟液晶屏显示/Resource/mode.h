#ifndef _MODE_H_
#define _MODE_H_

#define uchar unsigned char
#define uint unsigned int

typedef enum
{
	NORMAL,				//��ͨ����
	COMPLEX,			//�������㣨֧�ּӼ��˳�����֧�ֺ�����
	MATRIX,				//��������
	DRAW_PIC,			//����ͼ��
	SOLVE_EQUATION,		//�ⷽ��
//	TAKE_DIFFER,		//������
	STASTISTICS,		//ͳ��
	DIFFER_EQUATION,    //΢�ַ���
	MODE,				//�˵�
	SET,				//����
//	CHANGE_UNITS,	//��λת��
}PROGRAM_STATUS;

uchar mode_choose(void);

void mode_select(void);

void mode_sleep(void);

void reset(void);		//��λ


#endif