/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
/*---------------------------------------------------------------------*/

#include "stc.h"
#include "timer.h"

//#define T1MS                (65536 - ((MAIN_Fosc) / 12 / 1000))

BOOL f1ms;
//BOOL f10ms;

//static BYTE cnt10;

void timer_init()
{
    unsigned long T1MS;

    T1MS = (65536 - ((MAIN_Fosc) / 12 / 1000));
    TMOD &= ~0x0f;
    TL0 = T1MS;
    TH0 = T1MS >> 8;
    TR0 = 1;
    ET0 = 1;
    
//    cnt10 = 10;
}

void tm0_isr() interrupt 1
{
    f1ms = 1;
    
//    if (!--cnt10)
//    {
//        cnt10 = 10;
//        f10ms = 1;
//    }
}

