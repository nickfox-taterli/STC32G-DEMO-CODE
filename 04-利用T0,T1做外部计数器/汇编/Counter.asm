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

;本例程基于STC32G为主控芯片的实验箱进行编写测试。

;使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

;edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

;用STC的MCU的IO方式驱动8位数码管。

;显示效果为: 左边为T0(SW21)外部中断计数, 右边为T1(SW22)外部中断计数, 计数范围为0~255.

;由于按键是机械按键, 按下有抖动, 而本例程没有去抖动处理, 所以按一次有多个计数也是正常的.

;下载时, 选择时钟 24MHZ (用户可自行修改频率).

;******************************************

$include (../../comm/STC32G.INC)

;****************************** 用户定义宏 ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;堆栈开始地址

DIS_DOT         EQU     020H
DIS_BLACK       EQU     010H
DIS_            EQU     011H

;*******************************************************************
;*******************************************************************


;*************  IO口定义    **************/


;*************  本地变量声明    **************/
LED8            DATA    30H     ; 显示缓冲 30H ~ 37H
display_index   DATA    38H     ; 显示位索引

T0_cnt        DATA    39H     ; 测试用的计数变量
T1_cnt        DATA    3AH     ;


;*******************************************************************
;*******************************************************************
        ORG     0000H               ;程序复位入口，编译器自动定义到 0FF0000H 地址
        LJMP    F_Main

        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt

        ORG     001BH               ;3  Timer1 interrupt
        LJMP    F_Timer1_Interrupt


;*******************************************************************
;*******************************************************************


;******************** 主程序 **************************/
        ORG     0100H       ;编译器自动定义到 0FF0100H 地址
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
    MOV     P3M1, #70H     ;设置P3.6为漏极开路(实验箱加了上拉电阻到3.3V), P3.4,P3.5设置为输入口
    MOV     P3M0, #40H
    MOV     P4M1, #3cH     ;设置P4.2~P4.5为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P4M0, #3cH
    MOV     P5M1, #0cH     ;设置P5.2、P5.3为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P5M0, #0cH
    MOV     P6M1, #0ffH    ;设置为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P6M0, #0ffH
    MOV     P7M1, #00H     ;设置为准双向口
    MOV     P7M0, #00H

	MOV     A, #020H         ; P3.5使能内部4.1K上拉电阻
	MOV     WR6, #WORD0 P3PU
	MOV     WR4, #WORD2 P3PU
	MOV     @DR4, R11

    MOV     SP, #STACK_POIRTER
    MOV     PSW, #0
    USING   0       ;选择第0组R0~R7

;================= 用户初始化程序 ====================================

    MOV     display_index, #0
    MOV     R0, #LED8
    MOV     R2, #8
L_ClearLoop:
    MOV     @R0, #DIS_BLACK     ; 上电消隐
    INC     R0
    DJNZ    R2, L_ClearLoop

    CLR     A
	MOV     TMOD,A
    ORL     TMOD, #(1 SHL 2)    ; 使能T0外部计数模式
    ORL     TMOD, #(1 SHL 6)    ; 使能T1外部计数模式
    MOV     TH0, #0FFH
    MOV     TL0, #0FFH
    MOV     TH1, #0FFH
    MOV     TL1, #0FFH
    SETB    TR0 ; 启动定时器T0
    SETB    TR1 ; 启动定时器T1
    SETB    ET0 ; 使能定时器中断T0
    SETB    ET1 ; 使能定时器中断T1

    ANL     INTCLKO, #NOT 03H ; T0,T1不输出时钟

    SETB    EA      ;允许总中断

;=================== 主循环 ==================================
L_MainLoop:
    MOV     A, #1           ;延时1ms
    LCALL   F_delay_ms  
    LCALL   F_DisplayScan
    LJMP    L_MainLoop

;**********************************************/


;========================================================================
; 函数: F_delay_ms
; 描述: 延时子程序。
; 参数: ACC: 延时ms数.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;========================================================================
F_delay_ms:
    PUSH    02H     ;入栈R2
    PUSH    03H     ;入栈R3
    PUSH    04H     ;入栈R4

    MOV     R4,A
L_delay_ms_1:
    MOV     WR2, #(Fosc_KHZ / 4)
L_delay_ms_2:
    DEC     WR2, #1         ;1T
    JNE     L_delay_ms_2    ;3T
    DJNZ    R4, L_delay_ms_1

    POP     04H     ;出栈R2
    POP     03H     ;出栈R3
    POP     02H     ;出栈R4
    RET



; *********************** 显示相关程序 ****************************************
T_Display:                      ;标准字库
;    0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
DB  03FH,006H,05BH,04FH,066H,06DH,07DH,007H,07FH,06FH,077H,07CH,039H,05EH,079H,071H
;  black  -    H    J    K    L    N    o    P    U    t    G    Q    r    M    y
DB  000H,040H,076H,01EH,070H,038H,037H,05CH,073H,03EH,078H,03dH,067H,050H,037H,06EH
;    0.   1.   2.   3.   4.   5.   6.   7.   8.   9.   -1
DB  0BFH,086H,0DBH,0CFH,0E6H,0EDH,0FDH,087H,0FFH,0EFH,046H

T_COM:
DB  001H,002H,004H,008H,010H,020H,040H,080H     ;   位码


;========================================================================
; 函数: F_DisplayScan
; 描述: 显示扫描子程序。
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2020-11-4
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;========================================================================
F_DisplayScan:
    PUSH    DPH     ;DPH入栈
    PUSH    DPL     ;DPL入栈
    PUSH    00H     ;R0 入栈
    
    MOV     DPTR, #T_COM
    MOV     A, display_index
    MOVC    A, @A+DPTR
    CPL     A
	MOV     P7,A
    
    MOV     DPTR, #T_Display
    MOV     A, display_index
    ADD     A, #LED8
    MOV     R0, A
    MOV     A, @R0
    MOVC    A, @A+DPTR
    CPL     A
	MOV     P6,A

    INC     display_index
    MOV     A, display_index
    ANL     A, #0F8H            ; if(display_index >= 8)
    JZ      L_QuitDisplayScan
    MOV     display_index, #0;  ;8位结束回0
    
    MOV     A, T0_cnt
    MOV     B, #100
    DIV     AB
    MOV     LED8+7, A
    MOV     A, #10
    XCH     A, B
    DIV     AB
    MOV     LED8+6, A
    MOV     LED8+5, B
    MOV     LED8+4, #DIS_BLACK;

    MOV     LED8+3, #DIS_BLACK;
    MOV     A, T1_cnt
    MOV     B, #100
    DIV     AB
    MOV     LED8+2, A
    MOV     A, #10
    XCH     A, B
    DIV     AB
    MOV     LED8+1, A
    MOV     LED8+0, B
L_QuitDisplayScan:
    POP     00H     ;R0 出栈
    POP     DPL     ;DPL出栈
    POP     DPH     ;DPH出栈
    RET


;========================================================================
; 函数: F_Timer0_Interrupt
; 描述: Timer0中断函数.
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2020-11-4
; 备注: 所用到的通用寄存器都入栈保护, 退出时恢复原来数据不改变.
;========================================================================
F_Timer0_Interrupt:
    INC     T0_cnt    ; 中断+1
    RETI
    
;========================================================================
; 函数: F_Timer1_Interrupt
; 描述: Timer1中断函数.
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2020-11-4
; 备注: 所用到的通用寄存器都入栈保护, 退出时恢复原来数据不改变.
;========================================================================
F_Timer1_Interrupt:
    INC     T1_cnt    ; 中断+1
    RETI


    END

