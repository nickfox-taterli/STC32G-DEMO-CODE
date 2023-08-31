;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- STC 1T Series MCU Demo Programme -------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
;/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/* --- Web: www.STCAIMCU.com ------------------------------------------*/
;/* --- QQ:  800003751 -------------------------------------------------*/
;/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
;/*---------------------------------------------------------------------*/


;/************* 功能说明    **************

;本例程基于STC32G为主控芯片的实验箱进行编写测试。

;使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

;edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

;内部集成I2C串行总线控制器做从机模式，SCL->P3.2, SDA->P3.3;
;IO口模拟I2C做主机模式，SCL->P0.0, SDA->P0.1;
;通过外部飞线连接 P0.0->P3.2, P0.1->P3.3，实现I2C自发自收功能。

;用STC的MCU的IO方式驱动8位数码管。
;使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下,用户修改MCU主时钟频率时,自动定时于1ms.
;计数器每秒钟加1, 计数范围为0~9999.

;显示效果为: 上电后主机每秒钟发送一次计数数据，并在左边4个数码管上显示发送内容；从机接收到数据后在右边4个数码管显示。

;下载时, 选择时钟 24MHZ (用户可自行修改频率).

;******************************************/

$include (../../comm/STC32G.INC)

;/****************************** 用户定义宏 ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;堆栈开始地址

Timer0_Reload   EQU     (65536 - Fosc_KHZ)  ; Timer 0 中断频率, 1000次/秒

DIS_DOT         EQU     020H
DIS_BLACK       EQU     010H
DIS_            EQU     011H

SLAW            EQU     05AH
SLAR            EQU     05BH

SDA             BIT     P0.1
SCL             BIT     P0.0
;*******************************************************************
;*******************************************************************

;*************  IO口定义    **************/


;*************  本地变量声明    **************/
B_1ms           BIT     20H.0   ; 1ms标志
ISDA            BIT     20H.6
ISMA            BIT     20H.7

ADDR            DATA    21H

LED8            DATA    30H     ; 显示缓冲 30H ~ 37H
display_index   DATA    38H     ; 显示位索引

msecond         DATA    39H     ; 2 byte
Test_cnt        DATA    3BH     ;


;*******************************************************************
;*******************************************************************
        ORG     0000H               ;程序复位入口，编译器自动定义到 0FF0000H 地址
        LJMP    F_Main

        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt

        ORG     00C3H               ;24  I2C interrupt
        LJMP    F_I2C_Interrupt


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
    MOV     P3M1, #54H     ;设置P3.4、P3.6为漏极开路(实验箱加了上拉电阻到3.3V), SCL设置为输入口，SDA设置为准双向口
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

    MOV     display_index, #0
    MOV     R0, #LED8
    MOV     R2, #8
L_ClearLoop:
    MOV     @R0, #DIS_          ;上电全部显示-
    INC     R0
    DJNZ    R2, L_ClearLoop
    
    CLR     TR0
    ORL     AUXR, #(1 SHL 7)    ; Timer0_1T();
    ANL     TMOD, #NOT 04H      ; Timer0_AsTimer();
    ANL     TMOD, #NOT 03H      ; Timer0_16bitAutoReload();
    MOV     TH0, #Timer0_Reload / 256   ;Timer0_Load(Timer0_Reload);
    MOV     TL0, #Timer0_Reload MOD 256
    SETB    ET0         ; Timer0_InterruptEnable();
    SETB    TR0         ; Timer0_Run();

    LCALL   F_I2C_Init
    SETB    EA          ; 打开总中断

;=================== 主循环 ==================================
L_Main_Loop:
    JNB     B_1ms, L_Main_Loop     ;1ms未到
    CLR     B_1ms
    
;=================== 检测1s是否到 ==================================
    MOV     WR6, msecond
    INC     WR6, #1         ;msecond + 1
    MOV     msecond, WR6
    CMP     WR6, #1000
    JC      L_Main_Loop     ;if(msecond < 1000), jmp
    MOV     WR6, #0
    MOV     msecond, WR6    ;msecond = 0

;=================== 检测是否>= 10000 ==============================
    MOV     WR6, Test_cnt
    INC     WR6, #1         ;Test_cnt + 1
    MOV     Test_cnt, WR6
    CMP     WR6, #10000
    JC      L_LessThen10000  ;if(Test_cnt < 10000), jmp
    MOV     WR6, #0
    MOV     Test_cnt, WR6    ;Test_cnt = 0
L_LessThen10000:
    MOV     WR4, #00000H    ;被除数
    ;MOV     WR6, WR6
    MOV     WR0, #00000H    ;除数
    MOV     WR2, #1000
    MOV     DMAIR, #04H     ;32位无符号除法
    MOV     A, R7           ;显示数值
    ANL     A, #0x0F
    MOV     LED8+7, A

    MOV     WR4, WR0        ;余数做被除数
    MOV     WR6, WR2
    MOV     WR0, #00000H
    MOV     WR2, #100
    MOV     DMAIR, #04H     ;32位无符号除法
    MOV     A, R7           ;显示数值
    ANL     A, #0x0F
    MOV     LED8+6, A

    MOV     WR4, WR0        ;余数做被除数
    MOV     WR6, WR2
    MOV     WR0, #00000H
    MOV     WR2, #10
    MOV     DMAIR, #04H     ;32位无符号除法
    MOV     A, R7           ;显示数值
    ANL     A, #0x0F
    MOV     LED8+5, A

    MOV     A, R3           ;显示数值
    ANL     A, #0x0F
    MOV     LED8+4, A

    MOV     A, LED8+7           ;显示消无效0
    JNZ     L_Write_I2C
    MOV     LED8+7, #DIS_BLACK
    MOV     A, LED8+6
    JNZ     L_Write_I2C
    MOV     LED8+6, #DIS_BLACK
    MOV     A, LED8+5
    JNZ     L_Write_I2C
    MOV     LED8+5, #DIS_BLACK
L_Write_I2C:
    LCALL   WriteNbyte
    LJMP    L_Main_Loop

;========================================================================
; 函数: F_I2C_Init
; 描述: I2C初始化程序.
; 参数: none
; 返回: none.
; 版本: V1.0, 2021-3-4
;========================================================================
F_I2C_Init:
    ORL     P_SW2, #30H         ;I2C功能脚选择，00H:P1.5,P1.4; 10H:P2.5,P2.4; 30H:P3.2,P3.3

    MOV     A, #080H            ;使能I2C从机模式
    MOV     WR6, #WORD0 I2CCFG
    MOV     WR4, #WORD2 I2CCFG
    MOV     @DR4, R11

    MOV     A, #05AH            ;设置从机设备地址为5A
    MOV     WR6, #WORD0 I2CSLADR
    MOV     WR4, #WORD2 I2CSLADR
    MOV     @DR4, R11

    MOV     A, #00H
    MOV     WR6, #WORD0 I2CSLST
    MOV     WR4, #WORD2 I2CSLST
    MOV     @DR4, R11

    MOV     A, #078H            ;使能从机模式中断
    MOV     WR6, #WORD0 I2CSLCR
    MOV     WR4, #WORD2 I2CSLCR
    MOV     @DR4, R11

    SETB    ISDA
    SETB    ISMA
    CLR     A
    MOV     ADDR, A
    MOV     R0,A
    MOVX    A,@R0
    MOV     WR6,#WORD0 I2CTXD
    MOV     WR4,#WORD2 I2CTXD
    MOV     @DR4,R11
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


;//========================================================================
;// 函数: F_DisplayScan
;// 描述: 显示扫描子程序。
;// 参数: none.
;// 返回: none.
;// 版本: VER1.0
;// 日期: 2013-4-1
;// 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;//========================================================================
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

;**************** 中断函数 ***************************************************

F_Timer0_Interrupt: ;Timer0 1ms中断函数
    PUSH    PSW     ;PSW入栈
    PUSH    ACC     ;ACC入栈

    LCALL   F_DisplayScan   ; 1ms扫描显示一位
    SETB    B_1ms           ; 1ms标志
    
    POP     ACC     ;ACC出栈
    POP     PSW     ;PSW出栈
    RETI

;*******************************************************************
F_I2C_Interrupt:
    PUSH    ACC
    PUSH    PSW
    PUSH    DPL
    PUSH    DPH
    PUSH    R0
    PUSH    R4
    PUSH    R5
    PUSH    R6
    PUSH    R7

	MOV     WR6,#WORD0 I2CSLST
	MOV     WR4,#WORD2 I2CSLST
	MOV     R11,@DR4	  ;检测从机状态
    JB      ACC.6,STARTIF
    JB      ACC.5,RXIF
    JB      ACC.4,TXIF
    JB      ACC.3,STOPIF
F_I2C_EXIT:
    POP     R7
    POP     R6
    POP     R5
    POP     R4
    POP     R0
    POP     DPH
    POP     DPL
    POP     PSW
    POP     ACC
    RETI

STARTIF:
    ANL     A,#NOT 040H  ;处理 START 事件
	MOV     @DR4,R11
    JMP     F_I2C_EXIT
RXIF:
    ANL     A,#NOT 020H  ;处理 RECV 事件
	MOV     @DR4,R11
	MOV     WR6,#WORD0 I2CRXD
	MOV     WR4,#WORD2 I2CRXD
	MOV     R11,@DR4
    JBC     ISDA,RXDA
    JBC     ISMA,RXMA
    MOV     R0,ADDR     ;处理 RECV 事件（RECV DATA）
    MOV     @R0,A
    INC     ADDR
    JMP     F_I2C_EXIT
RXDA:
    JMP     F_I2C_EXIT  ;处理 RECV 事件（RECV DEVICE ADDR）
RXMA:
    ADD     A,#030H     ;LED8地址
    MOV     ADDR,A      ;处理 RECV 事件（RECV MEMORY ADDR）
    MOV     R0,A
    MOV     A,@R0
	MOV     WR6,#WORD0 I2CTXD
	MOV     WR4,#WORD2 I2CTXD
	MOV     @DR4,R11
    JMP     F_I2C_EXIT
TXIF:
    ANL     A,#NOT 010H  ;处理 SEND 事件
	MOV     @DR4,R11
    JB      ACC.1,RXNAK
    INC     ADDR
    MOV     R0,ADDR
    MOVX    A,@R0
	MOV     WR6,#WORD0 I2CTXD
	MOV     WR4,#WORD2 I2CTXD
	MOV     @DR4,R11
    JMP     F_I2C_EXIT
RXNAK:
    MOV     A,#0FFH
	MOV     WR6,#WORD0 I2CTXD
	MOV     WR4,#WORD2 I2CTXD
	MOV     @DR4,R11
    JMP     F_I2C_EXIT
STOPIF:
    ANL     A,#NOT 08H  ;处理 STOP 事件
	MOV     @DR4,R11
    SETB    ISDA
    SETB    ISMA
    JMP     F_I2C_EXIT

;========================================================================
; 软件模拟I2C函数
;========================================================================
I2C_Delay:
    PUSH    0
    MOV     R0,#018H
    DJNZ    R0,$
    POP     0
    RET

I2C_Start:
    SETB    SDA
    LCALL   I2C_Delay
    SETB    SCL
    LCALL   I2C_Delay
    CLR     SDA
    LCALL   I2C_Delay
    CLR     SCL
    LCALL   I2C_Delay
    RET

I2C_Stop:
    CLR     SDA
    LCALL   I2C_Delay
    SETB    SCL
    LCALL   I2C_Delay
    SETB    SDA
    LCALL   I2C_Delay
    RET

S_ACK:
    CLR     SDA
    LCALL   I2C_Delay
    SETB    SCL
    LCALL   I2C_Delay
    CLR     SCL
    LCALL   I2C_Delay
    RET

S_NoACK:
    SETB    SDA
    LCALL   I2C_Delay
    SETB    SCL
    LCALL   I2C_Delay
    CLR     SCL
    LCALL   I2C_Delay
    RET

I2C_Check_ACK:
    SETB    SDA
    LCALL   I2C_Delay
    SETB    SCL
    LCALL   I2C_Delay
	MOV     C, SDA
    CLR     SCL
    LCALL   I2C_Delay
    RET

I2C_WriteAbyte:
    MOV     R7, #08H
TXNEXT:
    RLC     A          ;移出数据位
    MOV     SDA, C     ;数据送数据口
    SETB    SCL        ;时钟->高
    LCALL   I2C_Delay  ;延时
    CLR     SCL        ;时钟->低
    LCALL   I2C_Delay  ;延时
    DJNZ    R7, TXNEXT ;送下一位
    RET

I2C_ReadAbyte:
    MOV     R7, #08H
RXNEXT:
    SETB    SCL        ;时钟->高
    LCALL   I2C_Delay  ;延时
    MOV     C, SDA
    RLC     A
    CLR     SCL        ;时钟->低
    LCALL   I2C_Delay  ;延时
    DJNZ    R7, RXNEXT ;收下一位
    RET

WriteNbyte:
    LCALL   I2C_Start
    MOV     A, #SLAW
    LCALL   I2C_WriteAbyte
    LCALL   I2C_Check_ACK
    JC      Write_Exit
    CLR     A
    LCALL   I2C_WriteAbyte ;addr
    LCALL   I2C_Check_ACK
    JC      Write_Exit

    MOV     R6, #04H
    MOV     A, #LED8+4  ;发送 LED8[4]~LED8[7] 数据
    MOV     R0,A
Write_Loop:
    MOV     A,@R0
    LCALL   I2C_WriteAbyte
    LCALL   I2C_Check_ACK
    JC      Write_Exit
    INC     R0
    DJNZ    R6, Write_Loop

Write_Exit:
    LCALL   I2C_Stop
    RET


    END

