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


;*************  功能说明    **************

; 本例程基于STC32G为主控芯片的实验箱进行编写测试。

; 使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

; edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

; 本程序演示5个定时器的使用, 本例程均使用16位自动重装.

; 下载时, 选择时钟 24MHZ (用户可自行修改频率).

; 定时器0做16位自动重装, 中断频率为1000HZ，中断函数从P6.7取反输出500HZ方波信号.

; 定时器1做16位自动重装, 中断频率为2000HZ，中断函数从P6.6取反输出1000HZ方波信号.

; 定时器2做16位自动重装, 中断频率为3000HZ，中断函数从P6.5取反输出1500HZ方波信号.

; 定时器3做16位自动重装, 中断频率为4000HZ，中断函数从P6.4取反输出2000HZ方波信号.

; 定时器4做16位自动重装, 中断频率为5000HZ，中断函数从P6.3取反输出2500HZ方波信号.

;******************************************/

$include (../../comm/STC32G.INC)

;/****************************** 用户定义宏 ***********************************/
Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;堆栈开始地址

;*******************************************************************
;*******************************************************************
        ORG     0000H               ;程序复位入口，编译器自动定义到 0FF0000H 地址
        LJMP    F_Main

        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt

        ORG     001BH               ;3  Timer1 interrupt
        LJMP    F_Timer1_Interrupt

        ORG     0063H               ;12  Timer2 interrupt
        LJMP    F_Timer2_Interrupt

        ORG     009BH               ;19  Timer3 interrupt
        LJMP    F_Timer3_Interrupt

        ORG     00A3H               ;20  Timer4 interrupt
        LJMP    F_Timer4_Interrupt

;*******************************************************************
;*******************************************************************


;******************** 主程序 **************************/
        ORG     0100H      ;编译器自动定义到 0FF0100H 地址
F_Main:
    MOV     WTST, #00H     ;设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    MOV     CKCON,#00H     ;提高访问XRAM速度
    ORL     P_SW2,#080H    ;使能访问XFR

    MOV     P0M1, #30H     ;设置P0.4、P0.5为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P0M0, #30H
    MOV     P1M1, #30H     ;设置P1.4、P1.5为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P1M0, #30H
    MOV     P2M1, #3cH     ;设置P2.2~P2.5为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P2M0, #3cH
    MOV     P3M1, #50H     ;设置P3.4、P3.6为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P3M0, #50H
    MOV     P4M1, #3cH     ;设置P4.2~P4.5为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P4M0, #3cH
    MOV     P5M1, #0cH     ;设置P5.2、P5.3为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P5M0, #0cH
    MOV     P6M1, #0ffH    ;设置为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P6M0, #0ffH
    MOV     P7M1, #00H     ;设置为准双向口
    MOV     P7M0, #00H

    
    MOV     SP, #STACK_POIRTER
    MOV     PSW, #0
    USING   0       ;选择第0组R0~R7

;================= 用户初始化程序 ====================================

    LCALL   F_Timer0_init
    LCALL   F_Timer1_init
    LCALL   F_Timer2_init
    LCALL   F_Timer3_init
    LCALL   F_Timer4_init
    SETB    EA
    
    CLR     P4.0        ;LED Power On

;=================== 主循环 ==================================
L_Main_Loop:

    LJMP    L_Main_Loop

;**********************************************/

;========================================================================
; 函数: F_Timer0_init
; 描述: timer0初始化函数.
; 参数: none.
; 返回: none.
; 版本: V1.0, 2015-1-12
;========================================================================
F_Timer0_init:
    CLR     TR0 ; 停止计数
    SETB    ET0 ; 允许中断
;   SETB    PT0 ; 高优先级中断
    ANL     TMOD, #NOT 0x03     ;
    ORL     TMOD, #0            ; 工作模式, 0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 16位自动重装, 不可屏蔽中断
;   ORL     TMOD, #0x04         ; 对外计数或分频
    ANL     TMOD, #NOT 0x04     ; 定时
;   ORL     INTCLKO, #0x01      ; 输出时钟
    ANL     INTCLKO, #NOT 0x01  ; 不输出时钟

;   ANL     AUXR, #NOT 0x80     ; 12T mode
    ORL     AUXR, #0x80         ; 1T mode
    MOV     TH0, #HIGH (-Fosc_KHZ) ; - 24000000 / 1000
    MOV     TL0, #LOW  (-Fosc_KHZ) ;
    SETB    TR0 ; 开始运行
    RET


;========================================================================
; 函数: F_Timer1_init
; 描述: timer1初始化函数.
; 参数: none.
; 返回: none.
; 版本: V1.0, 2015-1-12
;========================================================================
F_Timer1_init:
    CLR     TR1 ; 停止计数
    SETB    ET1 ; 允许中断
;   SETB    PT1 ; 高优先级中断
    ANL     TMOD, #NOT 0x30     ;
    ORL     TMOD, #(0 SHL 4)    ; 工作模式, 0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 16位自动重装, 不可屏蔽中断
;   ORL     TMOD, #0x40         ; 对外计数或分频
    ANL     TMOD, #NOT 0x40     ; 定时
;   ORL     INTCLKO, #0x02      ; 输出时钟
    ANL     INTCLKO, #NOT 0x02  ; 不输出时钟

;   ANL     AUXR, #NOT 0x40     ; 12T mode
    ORL     AUXR, #0x40         ; 1T mode
    MOV     TH1, #HIGH (-(Fosc_KHZ / 2)) ; - 24000000 / 2000
    MOV     TL1, #LOW  (-(Fosc_KHZ / 2)) ;
    SETB    TR1 ; 开始运行
    RET

;========================================================================
; 函数: F_Timer2_init
; 描述: timer2初始化函数.
; 参数: none.
; 返回: none.
; 版本: V1.0, 2015-1-12
;========================================================================
F_Timer2_init:
    ANL     AUXR, #NOT 0x1c     ; 停止计数, 定时模式, 12T模式

;   ANL     IE2, #NOT (1 SHL 2) ; 禁止中断
    ORL     IE2, #(1 SHL 2)     ; 允许中断
;   ORL     INTCLKO, #0x04      ; 输出时钟
    ANL     INTCLKO, #NOT 0x04  ; 不输出时钟

;   ORL     AUXR, #(1 SHL 3)    ; 对外计数或分频
;   ORL     INTCLKO, #0x02      ; 输出时钟
    ANL     INTCLKO, #NOT 0x02  ; 不输出时钟

    ORL     AUXR, #(1 SHL 2)    ; 1T mode
    MOV     T2H, #HIGH (-(Fosc_KHZ / 3))  ; - 24000000 / 3000
    MOV     T2L, #LOW  (-(Fosc_KHZ / 3))  ;

    ORL     AUXR, #(1 SHL 4)    ; 开始运行
    RET

;========================================================================
; 函数: F_Timer3_init
; 描述: timer3初始化函数.
; 参数: none.
; 返回: none.
; 版本: V1.0, 2015-1-12
;========================================================================
F_Timer3_init:
    ANL     T4T3M, #NOT 0x0F    ; 停止计数, 定时模式, 12T模式

;   ANL     IE2, #NOT (1 SHL 5) ; 禁止中断
    ORL     IE2, #(1 SHL 5)     ; 允许中断
;   ORL     T4T3M, #0x01        ; 输出时钟
    ANL     T4T3M, #NOT 0x01    ; 不输出时钟

;   ORL     T4T3M, #(1 SHL 2)   ; 对外计数或分频
;   ORL     INTCLKO, #0x02      ; 输出时钟
    ANL     INTCLKO, #NOT 0x02  ; 不输出时钟

    ORL     T4T3M, #(1 SHL 1)   ; 1T mode
    MOV     T3H, #HIGH (-(Fosc_KHZ / 4))  ; - 24000000 / 4000
    MOV     T3L, #LOW  (-(Fosc_KHZ / 4))  ;

    ORL     T4T3M, #(1 SHL 3)   ; 开始运行
    RET

;========================================================================
; 函数: F_Timer4_init
; 描述: timer4初始化函数.
; 参数: none.
; 返回: none.
; 版本: V1.0, 2015-1-12
;========================================================================
F_Timer4_init:
    ANL     T4T3M, #NOT 0xF0    ; 停止计数, 定时模式, 12T模式

;   ANL     IE2, #NOT (1 SHL 6) ; 禁止中断
    ORL     IE2, #(1 SHL 6)     ; 允许中断
;   ORL     T4T3M, #0x10        ; 输出时钟
    ANL     T4T3M, #NOT 0x10    ; 不输出时钟

;   ORL     T4T3M, #(1 SHL 6)   ; 对外计数或分频
;   ORL     INTCLKO, #0x02      ; 输出时钟
    ANL     INTCLKO, #NOT 0x02  ; 不输出时钟

    ORL     T4T3M, #(1 SHL 5)   ; 1T mode
    MOV     T4H, #HIGH (-(Fosc_KHZ / 5))  ; - 24000000 / 5000
    MOV     T4L, #LOW  (-(Fosc_KHZ / 5))  ;

    ORL     T4T3M, #(1 SHL 7)   ; 开始运行
    RET


;**************** 中断函数 ***************************************************
F_Timer0_Interrupt: ;Timer0 中断函数
    PUSH    PSW     ;PSW入栈
    PUSH    ACC     ;ACC入栈

    CPL     P6.7

    POP     ACC     ;ACC出栈
    POP     PSW     ;PSW出栈
    RETI
    
F_Timer1_Interrupt: ;Timer1 中断函数
    PUSH    PSW     ;PSW入栈
    PUSH    ACC     ;ACC入栈

    CPL     P6.6

    POP     ACC     ;ACC出栈
    POP     PSW     ;PSW出栈
    RETI
    
F_Timer2_Interrupt: ;Timer2 中断函数
    PUSH    PSW     ;PSW入栈
    PUSH    ACC     ;ACC入栈

    CPL     P6.5

    POP     ACC     ;ACC出栈
    POP     PSW     ;PSW出栈
    RETI
    
F_Timer3_Interrupt: ;Timer3 中断函数
    PUSH    PSW     ;PSW入栈
    PUSH    ACC     ;ACC入栈

    CPL     P6.4

    POP     ACC     ;ACC出栈
    POP     PSW     ;PSW出栈
    RETI
    
F_Timer4_Interrupt: ;Timer4 中断函数
    PUSH    PSW     ;PSW入栈
    PUSH    ACC     ;ACC入栈

    CPL     P6.3

    POP     ACC     ;ACC出栈
    POP     PSW     ;PSW出栈
    RETI
    


    END

