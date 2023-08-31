#ifndef _KEYBOARD_DEVICE_H_
#define _KEYBOARD_DEVICE_H_

#include "stc.h"

#define KEY_X P3		/*����x�����P2*/
#define KEY_Y P2		/*����y�����P0*/

#define KEY_X_IS_HIGH	/*y����ڸ�5λ*/
//#define LEY_X_IS_LOW	/*y����ڵ�5λ*/


/** =========== ������̬ɨ�� =========== */
uchar keyboard(void);

/** =========== ������̬���� =========== */
uchar keyboard_wake(void);

/** =========== ����ɨ��ֵ���� =========== */
//���� uchar number0,���Բ�����ݷ���ֵ���ж�Ӧ���� 
uchar keyboard_output();

//����
void about();

void delay_2s(void);	//��ʱ2s

#endif