;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- STC 1T Series MCU Demo Programme -------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
;/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/* --- BBS: www.STCAIMCU.com ------------------------------------------*/
;/* --- QQ:  800003751 -------------------------------------------------*/
;/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
;/*---------------------------------------------------------------------*/

$include (../../comm/STC32G.INC)

;/************* 功能说明    **************

;本例程基于STC32G为主控芯片的实验箱进行编写测试。

;使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

;edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

;单纯的IO口初始化，端口模式配置参考程序.

;******************************************/

    ORG     0000H               ;程序复位入口，编译器自动定义到 0FF0000H 地址
    LJMP    MAIN

    ORG     0100H      ;编译器自动定义到 0FF0100H 地址
MAIN:
    MOV     WTST, #00H     ;设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    MOV     CKCON,#00H     ;提高访问XRAM速度
    ORL     P_SW2,#080H    ;使能访问XFR

    MOV     SP,#3FH

    MOV     P0M0,#00H                   ;设置P0.0~P0.7为双向口模式
    MOV     P0M1,#00H

    MOV     P1M0,#0FFH                  ;设置P1.0~P1.7为推挽输出模式
    MOV     P1M1,#00H

    MOV     P2M0,#00H                   ;设置P2.0~P2.7为高阻输入模式
    MOV     P2M1,#0FFH

    MOV     P3M0,#0FFH                  ;设置P3.0~P3.7为开漏模式
    MOV     P3M1,#0FFH

    JMP     $

    END

