#ifndef __TIM_H
#define __TIM_H

#include "stc.h"

extern u32 xdata Sys_Time_10ms ;
extern u32 xdata Time_sleep;
#define Timer0_Reload   (MAIN_Fosc / 100)      //Timer 0 �ж�Ƶ��, 10��/�� 100hz���ٶ�
#define Timer1_Reload   (MAIN_Fosc / 100)      //Timer 1 �ж�Ƶ��, 2000��/��

void Timer0_init(void);
    
#endif

