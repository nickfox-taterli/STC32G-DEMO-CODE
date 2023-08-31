#include "stc.h"
#include "timer.h"

#define TM_1MS      (65536 - FOSC/1000)

BOOL f1ms;

void timer_init()
{
    f1ms = 0;
    T0x12 = 1;
    TMOD &= ~0x0f;
    TL0 = TM_1MS;
    TH0 = TM_1MS >> 8;
    TR0 = 1;
    ET0 = 1;
}

void timer0_isr() interrupt 1
{
    f1ms = 1;
}
