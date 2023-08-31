#include "rtc.h"

bit B1S_Flag;

void RTC_config(void)
{
//    INIYEAR = 21;     //Y:2021
//    INIMONTH = 12;    //M:12
//    INIDAY = 31;      //D:31
//    INIHOUR = 23;     //H:23
//    INIMIN = 59;      //M:59
//    INISEC = 50;      //S:50
//    INISSEC = 0;      //S/128:0

    ALAHOUR = 0;	//����Сʱ
    ALAMIN  = 0;	//���ӷ���
    ALASEC  = 0;	//������
    ALASSEC = 0;	//����1/128��

    //STC32G оƬʹ���ڲ�32Kʱ�ӣ������޷�����
    IRC32KCR = 0x80;   //�����ڲ�32K����.
    while (!(IRC32KCR & 1));  //�ȴ�ʱ���ȶ�
    RTCCFG = 0x03;    //ѡ���ڲ�32Kʱ��Դ������RTC�Ĵ�����ʼ��

//    X32KCR = 0x80 + 0x40;   //�����ⲿ32K����, ������+0x00, ������+0x40.
//    while (!(X32KCR & 1));  //�ȴ�ʱ���ȶ�
//    RTCCFG = 0x01;    //ѡ���ⲿ32Kʱ��Դ������RTC�Ĵ�����ʼ��

    RTCIF = 0x00;     //���жϱ�־
    RTCIEN = 0x04;    //�ж�ʹ��, 0x80:�����ж�, 0x40:���ж�, 0x20:Сʱ�ж�, 0x10:�����ж�, 0x08:���ж�, 0x04:1/2���ж�, 0x02:1/8���ж�, 0x01:1/32���ж�
    RTCCR = 0x01;     //RTCʹ��

    while(RTCCFG & 0x01);	//�ȴ���ʼ�����,��Ҫ�� "RTCʹ��" ֮���ж�. 
    //����RTCʱ����Ҫ32768Hz��1������ʱ��,��Լ30.5us. ����ͬ��, ����ʵ�ʵȴ�ʱ����0~30.5us.
    //������ȴ�������ɾ�˯��, ��RTC����������û���, ֹͣ����, ���Ѻ�ż���������ò���������.
}

void RTC_Isr() interrupt 36
{
    if(RTCIF & 0x04) //�ж��Ƿ����ж�
    {
        RTCIF &= ~0x04; //���жϱ�־
        B1S_Flag = 1;
    } 
}

//    if(B1S_Flag) {
//    B1S_Flag = 0;
//    printf("Year=20%bd ", YEAR);
//    printf("Month=%bd ", MONTH);
//    printf("Day=%bd ", DAY);
//    printf("Hour=%bd ", HOUR);
//    printf("Minute=%bd ", MIN);
//    printf("Second=%bd ", SEC);
//    printf("\r\n"); }