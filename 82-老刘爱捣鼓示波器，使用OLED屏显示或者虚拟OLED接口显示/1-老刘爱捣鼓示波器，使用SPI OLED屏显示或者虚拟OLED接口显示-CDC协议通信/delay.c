/* 延时函数，由STC-ISP软件生成
   Dealy Functions. Generated by STC-ISP Software*/

#include "delay.h"

void delay_ms(unsigned int ms)
{
    unsigned int i;
    do{
        i = MAIN_Fosc / 6000;
        while(--i);   //6T per loop
    }while(--ms);
}

//void Delay5ms() //@27.000MHz
//{
//    unsigned char i, j;

//    _nop_();
//    i = 176;
//    j = 80;

//    do
//    {
//        while (--j)
//            ;
//    } while (--i);
//}

//void Delay50ms() //@27.000MHz
//{
//    unsigned char i, j, k;

//    _nop_();
//    i = 7;
//    j = 218;
//    k = 55;

//    do
//    {
//        do
//        {
//            while (--k)
//                ;
//        } while (--j);
//    } while (--i);
//}

//void Delay1000ms() //@27.000MHz
//{
//    unsigned char i, j, k;

//    _nop_();
//    i = 137;
//    j = 249;
//    k = 145;

//    do
//    {
//        do
//        {
//            while (--k)
//                ;
//        } while (--j);
//    } while (--i);
//}

void Delay3us() //@35.000MHz
{
    unsigned char i;

    i = 17;

    while (--i)
        ;
}

//For scaleH 500ms with ADC_SPEED_512 20ms-26us-3us
void Delay19971us() //@35.000MHz
{
    unsigned char i, j, k;

    i = 2;
    j = 195;
    k = 200;

    do
    {
        do
        {
            while (--k)
                ;
        } while (--j);
    } while (--i);
}

//For scaleH 200ms with ADC_SPEED_512 8ms-26us-3us
void Delay7971us() //@35.000MHz
{
    unsigned char i, j, k;

    _nop_();
    i = 1;
    j = 180;
    k = 250;

    do
    {
        do
        {
            while (--k)
                ;
        } while (--j);
    } while (--i);
}

//For scaleH 100ms with ADC_SPEED_512 4ms-26us-3us
void Delay3971us() //@35.000MHz
{
    unsigned char i, j;

    _nop_();
    i = 90;
    j = 59;

    do
    {
        while (--j)
            ;
    } while (--i);
}

//For scaleH 50ms with ADC_SPEED_512 2ms-26us-3us
void Delay1971us() //@35.000MHz
{
    unsigned char i, j;

    i = 45;
    j = 60;

    do
    {
        while (--j)
            ;
    } while (--i);
}

//For scaleH 20ms with ADC_SPEED_512 800us-26us-3us
void Delay771us() //@35.000MHz
{
    unsigned char i, j;

    i = 18;
    j = 80;

    do
    {
        while (--j)
            ;
    } while (--i);
}

//For scaleH 10ms with ADC_SPEED_512 400us-26us-3us
void Delay371us() //@35.000MHz
{
    unsigned char i, j;

    i = 9;
    j = 105;

    do
    {
        while (--j)
            ;
    } while (--i);
}

//For scaleH 5ms with ADC_SPEED_512 200us-26us-3us
void Delay171us() //@35.000MHz
{
    unsigned char i, j;

    _nop_();
    _nop_();
    i = 4;
    j = 220;

    do
    {
        while (--j)
            ;
    } while (--i);
}

//For scaleH 2ms with ADC_SPEED_512 80us-26us-3us
void Delay51us() //@35.000MHz
{
    unsigned char i, j;

    _nop_();
    i = 2;
    j = 40;

    do
    {
        while (--j)
            ;
    } while (--i);
}

//For scaleH 1ms with ADC_SPEED_352 40us-19us-3us
void Delay18us() //@35.000MHz
{
    unsigned char i;

    i = 104;

    while (--i)
        ;
}

//For scaleH 500us with ADC_SPEED_192 20us-11us-3us
void Delay6us() //@35.000MHz
{
    unsigned char i;

    i = 34; //微调/Fine tuning 52->44

    while (--i)
        ;
}
