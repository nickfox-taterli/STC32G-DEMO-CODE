#include "tim.h"

u32 xdata Sys_Time_10ms = 0;
u32 xdata Time_sleep=0;
//========================================================================
// ����: void Timer0_init(void)
// ����: timer0��ʼ������.
// ����: none.
// ����: none.
// �汾: V1.0, 2015-1-12
//========================================================================
void Timer0_init(void)
{
        TR0 = 0;    //ֹͣ����

    #if (Timer0_Reload < 64)    // ����û�����ֵ�����ʣ� ��������ʱ��
        #error "Timer0���õ��жϹ���!"

    #elif ((Timer0_Reload/12) < 65536UL)    // ����û�����ֵ�����ʣ� ��������ʱ��
        ET0 = 1;    //�����ж�
    //  PT0 = 1;    //�����ȼ��ж�
        TMOD &= ~0x03;
        TMOD |= 0;  //����ģʽ, 0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ, 3: 16λ�Զ���װ, ���������ж�
    //  T0_CT = 1;  //����
        T0_CT = 0;  //��ʱ
    //  T0CLKO = 1; //���ʱ��
        T0CLKO = 0; //�����ʱ��

        #if (Timer0_Reload < 65536UL)
            T0x12 = 1;  //1T mode
            TH0 = (u8)((65536UL - Timer0_Reload) / 256);
            TL0 = (u8)((65536UL - Timer0_Reload) % 256);
        #else
            T0x12 = 0;  //12T mode
            TH0 = (u8)((65536UL - Timer0_Reload/12) / 256);
            TL0 = (u8)((65536UL - Timer0_Reload/12) % 256);
        #endif

        TR0 = 1;    //��ʼ����

    #else
        #error "Timer0���õ��жϹ���!"
    #endif
}


//========================================================================
// ����: void Timer1_init(void)
// ����: timer1��ʼ������.
// ����: none.
// ����: none.
// �汾: V1.0, 2015-1-12
//========================================================================
void Timer1_init(void)
{
        TR1 = 0;    //ֹͣ����

    #if (Timer1_Reload < 64)    // ����û�����ֵ�����ʣ� ��������ʱ��
        #error "Timer1���õ��жϹ���!"

    #elif ((Timer1_Reload/12) < 65536UL)    // ����û�����ֵ�����ʣ� ��������ʱ��
        ET1 = 1;    //�����ж�
    //  PT1 = 1;    //�����ȼ��ж�
        TMOD &= ~0x30;
        TMOD |= (0 << 4);   //����ģʽ, 0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ
    //  T1_CT = 1;  //����
        T1_CT = 0;  //��ʱ
    //  T1CLKO = 1; //���ʱ��
        T1CLKO = 0; //�����ʱ��

        #if (Timer1_Reload < 65536UL)
            T1x12 = 1;  //1T mode
            TH1 = (u8)((65536UL - Timer1_Reload) / 256);
            TL1 = (u8)((65536UL - Timer1_Reload) % 256);
        #else
            T1x12 = 0;  //12T mode
            TH1 = (u8)((65536UL - Timer1_Reload/12) / 256);
            TL1 = (u8)((65536UL - Timer1_Reload/12) % 256);
        #endif

        TR1 = 1;    //��ʼ����

    #else
        #error "Timer1���õ��жϹ���!"
    #endif
}

//========================================================================
// ����: void timer0_int (void) interrupt TIMER0_VECTOR
// ����:  timer0�жϺ���.
// ����: none.
// ����: none.
// �汾: V1.0, 2015-1-12
//========================================================================
void timer0_int (void) interrupt 1
{
    static u8 count = 0;
    TR0=0;
    
    Sys_Time_10ms++;
//	flash_count++;
	TR0=1;
    if( count++ >= 100 )
    {
        count= 0;
        if( Time_sleep<100000 )
            Time_sleep++;
    }
}


//========================================================================
// ����: void timer1_int (void) interrupt TIMER1_VECTOR
// ����:  timer1�жϺ���.
// ����: none.
// ����: none.
// �汾: V1.0, 2015-1-12
//========================================================================
void timer1_int (void) interrupt 3
{
    TR1 = 0;
//	if(pwm_current > pwm_target)
//	{
//		pwm_current --;
//		CCAP1H = pwm_current;// ��PWM
//	}
//	else if(pwm_current < pwm_target)
//	{
//		pwm_current ++;
//		CCAP1H = pwm_current;// ��PWM
//	}
	TR1 = 1;
	
//	if(time_out<60000)
//		time_out++;    
}
