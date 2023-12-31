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

/*************  功能说明    **************

本例程基于STC32G为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

基于HID协议的USB键盘范例

将代码下载到试验箱后即可实现USB键盘的基本功能
跑马灯中的LED17为NumLock灯、LED16为CapsLock灯、LED15为ScrollLock灯
矩阵按键中的KEY0~KEY7分别为键盘中的1~8

下载时, 设置IRC工作频率为 24MHz.

******************************************/

#include "stc.h"
#include "usb.h"
#include "usb_req_class.h"
#include "timer.h"

void main()
{
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    usb_init();
    timer_init();

    EA = 1;
    
    while (1)
    {
        usb_class_in();
        
        if (f1ms)
        {
            f1ms = 0;
            scan_key();
        }
    }
}
