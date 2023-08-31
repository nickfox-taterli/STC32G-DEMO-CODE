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

; 定时器0做16位自动重装, 中断频率为1000HZ，作为数码管扫描显示.

; 定时器1做16位自动重装, 中断频率为10000HZ，作为脉冲信号宽度检测.

; 用STC的MCU的IO方式驱动8位数码管。

; P33口产生低电平(用户可自定义修改端口跟检测电平)时进行计时，电平变化后停止计时。

; 数码管显示脉冲信号宽度，单位0.1ms，测量范围0.1ms~6553.5ms.

; 下载时, 选择时钟 24MHZ (用户可自行修改频率).

;******************************************/

$include (../../comm/STC32G.INC)

;/****************************** 用户定义宏 ***********************************/


Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;堆栈开始地址

DIS_DOT         EQU     020H
DIS_BLACK       EQU     010H
DIS_            EQU     011H

;*******************************************************************
;*******************************************************************

;*************  本地变量声明    **************/
Flag0           DATA    20H
B_1ms           BIT     Flag0.0 ; 1ms标志

LED8            DATA    30H     ; 显示缓冲 30H ~ 37H
display_index   DATA    38H     ; 显示位索引

Test_cnt        DATA    39H     ; 显示用的计数变量
Temp_cnt        DATA    3BH     ; 测量用的计数变量
msecond         DATA    3DH     ;

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

    MOV     WR6, #0
    MOV     Test_cnt, WR6
    MOV     Temp_cnt, WR6
    MOV     display_index, #0
    MOV     R0, #LED8
    MOV     R2, #8
L_ClearLoop:
    MOV     @R0, #DIS_BLACK     ;上电消隐
    INC     R0
    DJNZ    R2, L_ClearLoop
    
    LCALL   F_Timer0_init
    LCALL   F_Timer1_init
    SETB    EA
    
;=================== 主循环 ==================================
L_Main_Loop:
    JNB     B_1ms,  L_Main_Loop     ;1ms未到
    CLR     B_1ms

    MOV     WR6, msecond
    INC     WR6, #1         ;msecond + 1
    MOV     msecond, WR6
    CMP     WR6, #1000
    JC      L_Main_Loop     ;if(msecond < 1000), jmp
    MOV     WR6, #0
    MOV     msecond, WR6    ;msecond = 0

    LCALL   F_Display
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
    MOV     TH1, #HIGH (-(Fosc_KHZ / 10)) ; - 24000000 / 10000
    MOV     TL1, #LOW  (-(Fosc_KHZ / 10)) ;
    SETB    TR1 ; 开始运行
    RET


;**************** 中断函数 ***************************************************
F_Timer0_Interrupt: ;Timer0 中断函数
    PUSH    PSW     ;PSW入栈
    PUSH    ACC     ;ACC入栈

    LCALL   F_DisplayScan   ; 1ms扫描显示一位
    SETB    B_1ms           ; 1ms标志

    POP     ACC     ;ACC出栈
    POP     PSW     ;PSW出栈
    RETI
    
F_Timer1_Interrupt: ;Timer1 中断函数
    PUSH    PSW     ;PSW入栈
    PUSH    ACC     ;ACC入栈
    PUSH    R0      ;R0入栈
    PUSH    R1      ;R1入栈

    JB      P3.3, F_Timer1_Next

    MOV     WR0, Temp_cnt
    INC     WR0, #1         ;Temp_cnt + 1
    MOV     Temp_cnt, WR0
    LJMP    F_Timer1_Exit

F_Timer1_Next:
    MOV     WR0, Temp_cnt
    CMP     WR0, #10
    JC      F_Timer1_Clear  ;if(Temp_cnt < 10), jmp
    MOV     WR0, Temp_cnt
    MOV     Test_cnt, WR0

F_Timer1_Clear:
    MOV     WR0, #0
    MOV     Temp_cnt, WR0   ;Temp_cnt = 0

F_Timer1_Exit:
    POP     R1      ;R1出栈
    POP     R0      ;R0出栈
    POP     ACC     ;ACC出栈
    POP     PSW     ;PSW出栈
    RETI

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
; 日期: 2013-4-1
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
L_QuitDisplayScan:
    POP     00H     ;R0 出栈
    POP     DPL     ;DPL出栈
    POP     DPH     ;DPH出栈
    RET

;========================================================================
; 函数: F_Display
; 描述: 显示计数函数.
; 参数: none.
; 返回: none.
; 版本: V1.0, 2021-3-1
;========================================================================
F_Display:
    MOV     WR4, #00000H    ;被除数
    MOV     WR6, Test_cnt
    MOV     WR0, #00000H    ;除数
    MOV     WR2, #10000
    MOV     DMAIR, #04H     ;32位无符号除法
    MOV     A, R7           ;显示计数值
    ANL     A, #0x0F
    MOV     LED8+4, A

    MOV     WR4, WR0        ;余数做被除数
    MOV     WR6, WR2
    MOV     WR0, #00000H
    MOV     WR2, #1000
    MOV     DMAIR, #04H     ;32位无符号除法
    MOV     A, R7           ;显示计数值
    ANL     A, #0x0F
    MOV     LED8+3, A

    MOV     WR4, WR0        ;余数做被除数
    MOV     WR6, WR2
    MOV     WR0, #00000H
    MOV     WR2, #100
    MOV     DMAIR, #04H     ;32位无符号除法
    MOV     A, R7           ;显示计数值
    ANL     A, #0x0F
    MOV     LED8+2, A

    MOV     WR4, WR0        ;余数做被除数
    MOV     WR6, WR2
    MOV     WR0, #00000H
    MOV     WR2, #10
    MOV     DMAIR, #04H     ;32位无符号除法
    MOV     A, R7           ;显示计数值
    ANL     A, #0x0F
    MOV     LED8+1, A

    MOV     A, R3           ;显示计数值
    ANL     A, #0x0F
    MOV     LED8+0, A

    MOV     A, LED8+4       ;显示消无效0
    JNZ     F_QuitDisplay
    MOV     LED8+4, #DIS_BLACK
    MOV     A, LED8+3
    JNZ     F_QuitDisplay
    MOV     LED8+3, #DIS_BLACK
    MOV     A, LED8+2
    JNZ     F_QuitDisplay
    MOV     LED8+2, #DIS_BLACK
    MOV     A, LED8+1
    JNZ     F_QuitDisplay
    MOV     LED8+1, #DIS_BLACK
F_QuitDisplay:
    RET




    END

