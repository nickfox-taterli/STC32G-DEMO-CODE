;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- STC 1T Series MCU Demo Programme -------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
;/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/* --- Web: www.STCAIMCU.com ------------------------------------------*/
;/* --- QQ:  800003751 -------------------------------------------------*/
;/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
;/*---------------------------------------------------------------------*/


;*************  功能说明    **************

;本例程基于STC32G为主控芯片的实验箱进行编写测试。

;使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

;edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

;读写芯片内部集成的RTC模块.

;用STC的MCU的IO方式驱动8位数码管。

;使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

;8位数码管显示时间(小时-分钟-秒).

;行列扫描按键键码为25~32.

;按键只支持单键按下, 不支持多键同时按下, 那样将会有不可预知的结果.

;键按下超过1秒后,将以10键/秒的速度提供重键输出. 用户只需要检测KeyCode是否非0来判断键是否按下.

;调整时间键:
;键码25: 小时+.
;键码26: 小时-.
;键码27: 分钟+.
;键码28: 分钟-.

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

;*******************************************************************
;*******************************************************************


;*************  IO口定义    **************/


;*************  本地变量声明    **************/
Flag0           DATA    20H
B_1ms           BIT     Flag0.0 ;   1ms标志

LED8            DATA    30H     ;   显示缓冲 30H ~ 37H
display_index   DATA    38H     ;   显示位索引

usrhour         DATA    39H     ;RTC变量
usrminute       DATA    3AH
usrsecond       DATA    3BH     ;
msecond         DATA    3CH     ; 2 byte

IO_KeyState     DATA    3EH ; IO行列键状态变量
IO_KeyState1    DATA    3FH
IO_KeyHoldCnt   DATA    40H ; IO键按下计时
KeyCode         DATA    41H ; 给用户使用的键码, 1~16为ADC键， 17~32为IO键

cnt50ms         DATA    45H;


;*******************************************************************
;*******************************************************************
        ORG     0000H               ;程序复位入口，编译器自动定义到 0FF0000H 地址
        LJMP    F_Main


        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt

        ORG     0123H               ;36 RTC interrupt
        LJMP    F_RTC_Interrupt


;*******************************************************************
;*******************************************************************


;******************** 主程序 **************************/
        ORG     0200H       ;编译器自动定义到 0FF0200H 地址
F_Main:
    MOV     WTST, #00H     ;设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    MOV     CKCON,#00H     ;提高访问XRAM速度
    ORL     P_SW2,#080H    ;使能访问XFR

    MOV     P0M1, #30H     ;设置P0.4、P0.5为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P0M0, #30H
    MOV     P1M1, #32H     ;设置P1.1、P1.4、P1.5为漏极开路(实验箱加了上拉电阻到3.3V), P1.1在PWM当DAC电路通过电阻串联到P2.3
    MOV     P1M0, #32H
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
    MOV     PSW, #0     ;选择第0组R0~R7
    USING   0       ;选择第0组R0~R7

;================= 用户初始化程序 ====================================

    MOV     display_index, #0
    MOV     R0, #LED8
    MOV     R2, #8
L_ClearLoop:
    MOV     @R0, #DIS_BLACK     ;上电消隐
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

    LCALL   F_RTC_Init
    SETB    EA          ; 打开总中断

    LCALL   F_DisplayRTC

    CLR     A
    MOV     IO_KeyState, A
    MOV     IO_KeyState1, A
    MOV     IO_KeyHoldCnt, A
    MOV     KeyCode, A      ; 给用户使用的键码, 17~32有效
    MOV     cnt50ms, #50

;=====================================================

;=================== 主循环 ==================================
L_Main_Loop:
    JNB     B_1ms,  L_Main_Loop     ;1ms未到
    CLR     B_1ms
    
;=================== 检测1000ms是否到 ==================================
    MOV     WR6, msecond
    INC     WR6, #1         ;msecond + 1
    MOV     msecond, WR6
    CMP     WR6, #1000
    JC      L_Quit_Check_1000ms  ;if(msecond < 1000), jmp
    MOV     WR6, #0
    MOV     msecond, WR6    ;msecond = 0
    
;================= 1000ms到， 处理RTC ====================================
    MOV     WR6, #0
    MOV     msecond, WR6    ;msecond = 0

    LCALL   F_DisplayRTC
L_Quit_Check_1000ms:

;======================= 50ms扫描一次行列键盘 ==============================
L_Read_IO_Key:
    DJNZ    cnt50ms, L_Quit_Read_IO_Key     ; (cnt50ms - 1) != 0, jmp
    MOV     cnt50ms, #50    ;
    LCALL   F_IO_KeyScan    ;50ms扫描一次行列键盘
L_Quit_Read_IO_Key:
;=====================================================


;=====================================================
L_KeyProcess:
    MOV     A, KeyCode
    JZ      L_Quit_KeyProcess
                        ;有键按下
    MOV     A, KeyCode
    CJNE    A, #25, L_Quit_K25
    INC     usrhour        ;if(KeyCode == 17)  hour +1
    MOV     A, usrhour
    CLR     C
    SUBB    A, #24
    JC      L_K25_Display
    MOV     usrhour, #0
L_K25_Display:
    LCALL   F_WriteRTC
    LCALL   F_DisplayRTC
L_Quit_K25:

    MOV     A, KeyCode
    CJNE    A, #26, L_Quit_K26
    DEC     usrhour    ; hour - 1
    MOV     A, usrhour
    CJNE    A, #255, L_K26_Display
    MOV     usrhour, #23
L_K26_Display:
    LCALL   F_WriteRTC
    LCALL   F_DisplayRTC
L_Quit_K26:

    MOV     A, KeyCode
    CJNE    A, #27, L_Quit_K27
    INC     usrminute  ; minute + 1
    MOV     A, usrminute
    CLR     C
    SUBB    A, #60
    JC      L_K27_Display
    MOV     usrminute, #0
L_K27_Display:
    LCALL   F_WriteRTC
    LCALL   F_DisplayRTC
L_Quit_K27:

    MOV     A, KeyCode
    CJNE    A, #28, L_Quit_K28
    DEC     usrminute  ; minute - 1
    MOV     A, usrminute
    CJNE    A, #255, L_K28_Display
    MOV     usrminute, #59
L_K28_Display:
    LCALL   F_WriteRTC
    LCALL   F_DisplayRTC
L_Quit_K28:

    MOV     KeyCode, #0
L_Quit_KeyProcess:

    LJMP    L_Main_Loop

;**********************************************/

;========================================================================
; 函数: F_RTC_Init
; 描述: RTC初始化程序.
; 参数: none
; 返回: none.
; 版本: V1.0, 2021-3-4
;========================================================================
F_RTC_Init:
    ;-------时间设置------
    MOV     A,#21
    MOV     WR6, #WORD0 INIYEAR  ;Y:2021
    MOV     WR4, #WORD2 INIYEAR
    MOV     @DR4, R11

    MOV     A,#12
    MOV     WR6, #WORD0 INIMONTH ;M:12
    MOV     WR4, #WORD2 INIMONTH
    MOV     @DR4, R11

    MOV     A,#31
    MOV     WR6, #WORD0 INIDAY   ;D:31
    MOV     WR4, #WORD2 INIDAY
    MOV     @DR4, R11

    MOV     A,#23
    MOV     WR6, #WORD0 INIHOUR  ;H:23
    MOV     WR4, #WORD2 INIHOUR
    MOV     @DR4, R11

    MOV     A,#59
    MOV     WR6, #WORD0 INIMIN   ;M:59
    MOV     WR4, #WORD2 INIMIN
    MOV     @DR4, R11

    MOV     A,#50
    MOV     WR6, #WORD0 INISEC   ;S:50
    MOV     WR4, #WORD2 INISEC
    MOV     @DR4, R11

    MOV     A,#0
    MOV     WR6, #WORD0 INISSEC  ;S/128:0
    MOV     WR4, #WORD2 INISSEC
    MOV     @DR4, R11

    ;-------闹钟设置------
    MOV     A,#0
    MOV     WR6, #WORD0 ALAHOUR  ;闹钟小时
    MOV     WR4, #WORD2 ALAHOUR
    MOV     @DR4, R11

    MOV     A,#0
    MOV     WR6, #WORD0 ALAMIN   ;闹钟分钟
    MOV     WR4, #WORD2 ALAMIN
    MOV     @DR4, R11

    MOV     A,#0
    MOV     WR6, #WORD0 ALASEC   ;闹钟秒
    MOV     WR4, #WORD2 ALASEC
    MOV     @DR4, R11

    MOV     A,#0
    MOV     WR6, #WORD0 ALASSEC  ;闹钟1/128秒
    MOV     WR4, #WORD2 ALASSEC
    MOV     @DR4, R11

    ;-------时钟设置------
    ;STC32G 芯片使用内部32K时钟，休眠无法唤醒
;    MOV     A,#80H	            ;启动内部32K IRC
;    MOV     WR6, #WORD0 IRC32KCR
;    MOV     WR4, #WORD2 IRC32KCR
;    MOV     @DR4, R11
;    MOV     R11,@DR4
;    JNB     ACC.0,$-1	        ;等待时钟稳定
;    MOV     A,#1
;    MOV     A,#3
;    MOV     WR6, #WORD0 RTCCFG  ;选择内部32K时钟源，触发RTC寄存器初始化
;    MOV     WR4, #WORD2 RTCCFG
;    MOV     @DR4, R11

    MOV     A,#0C0H
    MOV     WR6, #WORD0 X32KCR  ;启动外部32K晶振, 低增益+0x00, 高增益+0x40.
    MOV     WR4, #WORD2 X32KCR
    MOV     @DR4, R11
    MOV     R11, @DR4
    JNB     ACC.0,$-1	        ;等待时钟稳定
    MOV     A,#1
    MOV     WR6, #WORD0 RTCCFG  ;选择外部32K时钟源，触发RTC寄存器初始化
    MOV     WR4, #WORD2 RTCCFG
    MOV     @DR4, R11

    ;-------中断设置------
    MOV     A,#0
    MOV     WR6, #WORD0 RTCIF   ;清中断标志
    MOV     WR4, #WORD2 RTCIF
    MOV     @DR4, R11

    MOV     A,#088H
    MOV     WR6, #WORD0 RTCIEN  ;中断使能, 0x80:闹钟中断, 0x40:日中断, 0x20:小时中断, 0x10:分钟中断, 0x08:秒中断, 0x04:1/2秒中断, 0x02:1/8秒中断, 0x01:1/32秒中断
    MOV     WR4, #WORD2 RTCIEN
    MOV     @DR4, R11

    ;-------RTC使能-------
    MOV     A,#1
    MOV     WR6, #WORD0 RTCCR   ;RTC使能
    MOV     WR4, #WORD2 RTCCR
    MOV     @DR4, R11

    MOV     WR6, #WORD0 RTCCFG
    MOV     WR4, #WORD2 RTCCFG
    MOV     R11,@DR4
    JNB     ACC.0,$-1	        ;等待初始化完成,需要在 "RTC使能" 之后判断. 
    ;设置RTC时间需要32768Hz的1个周期时间,大约30.5us. 由于同步, 所以实际等待时间是0~30.5us.
    ;如果不等待设置完成就睡眠, 则RTC会由于设置没完成, 停止计数, 唤醒后才继续完成设置并继续计数.
    RET


;/*****************************************************
;   行列键扫描程序
;   使用XY查找4x4键的方法，只能单键，速度快
;
;   Y     P04      P05      P06      P07
;          |        |        |        |
;X         |        |        |        |
;P00 ---- K00 ---- K01 ---- K02 ---- K03 ----
;          |        |        |        |
;P01 ---- K04 ---- K05 ---- K06 ---- K07 ----
;          |        |        |        |
;P02 ---- K08 ---- K09 ---- K10 ---- K11 ----
;          |        |        |        |
;P03 ---- K12 ---- K13 ---- K14 ---- K15 ----
;          |        |        |        |
;******************************************************/


T_KeyTable:  DB 0,1,2,0,3,0,0,0,4,0,0,0,0,0,0,0

F_IO_KeyDelay:
    PUSH    03H     ;R3入栈
    MOV     R3, #60
    DJNZ    R3, $   ; (n * 4) T
    POP     03H     ;R3出栈
    RET

F_IO_KeyScan:   ;50ms call
    PUSH    06H     ;R6入栈
    PUSH    07H     ;R7入栈

    MOV     R6, IO_KeyState1    ; 保存上一次状态

    MOV     P0, #0F0H       ;X低，读Y
    LCALL   F_IO_KeyDelay       ;delay about 250T
    MOV     A, P0
    ANL     A, #0F0H
    MOV     IO_KeyState1, A     ; IO_KeyState1 = P0 & 0xf0

    MOV     P0, #0FH        ;Y低，读X
    LCALL   F_IO_KeyDelay       ;delay about 250T
    MOV     A, P0
    ANL     A, #0FH
    ORL     A, IO_KeyState1         ; IO_KeyState1 |= (P0 & 0x0f)
    MOV     IO_KeyState1, A
    XRL     IO_KeyState1, #0FFH     ; IO_KeyState1 ^= 0xff; 取反

    MOV     A, R6                   ;if(j == IO_KeyState1), 连续两次读相等
    CJNE    A, IO_KeyState1, L_QuitCheckIoKey   ;不相等, jmp
    
    MOV     R6, IO_KeyState     ;暂存IO_KeyState
    MOV     IO_KeyState, IO_KeyState1
    MOV     A, IO_KeyState
    JZ      L_NoIoKeyPress      ; if(IO_KeyState != 0), 有键按下

    MOV     A, R6   
    JZ      L_CalculateIoKey    ;if(R6 == 0)    F0 = 1; 第一次按下
    MOV     A, R6   
    CJNE    A, IO_KeyState, L_QuitCheckIoKey    ; if(j != IO_KeyState), jmp
    
    INC     IO_KeyHoldCnt   ; if(++IO_KeyHoldCnt >= 20),    1秒后重键
    MOV     A, IO_KeyHoldCnt
    CJNE    A, #20, L_QuitCheckIoKey
    MOV     IO_KeyHoldCnt, #18;
L_CalculateIoKey:
    MOV     A, IO_KeyState
    SWAP    A       ;R6 = T_KeyTable[IO_KeyState >> 4];
    ANL     A, #0x0F
    MOV     DPTR, #T_KeyTable
    MOVC    A, @A+DPTR
    MOV     R6, A
    
    JZ      L_QuitCheckIoKey    ; if(R6 == 0)
    MOV     A, IO_KeyState
    ANL     A, #0x0F
    MOVC    A, @A+DPTR
    MOV     R7, A
    JZ      L_QuitCheckIoKey    ; if(T_KeyTable[IO_KeyState& 0x0f] == 0)
    
    MOV     A, R6       ;KeyCode = (j - 1) * 4 + T_KeyTable[IO_KeyState & 0x0f] + 16;   //计算键码，17~32
    ADD     A, ACC
    ADD     A, ACC
    MOV     R6, A
    MOV     A, R7
    ADD     A, R6
    ADD     A, #12
    MOV     KeyCode, A
    SJMP    L_QuitCheckIoKey
    
L_NoIoKeyPress:
    MOV     IO_KeyHoldCnt, #0

L_QuitCheckIoKey:
    MOV     P0, #0xFF
    POP     07H     ;R7出栈
    POP     06H     ;R6出栈
    REt



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

;**************** 中断函数 ***********************************************

F_Timer0_Interrupt: ;Timer0 1ms中断函数
    PUSH    PSW     ;PSW入栈
    PUSH    ACC     ;ACC入栈

    LCALL   F_DisplayScan   ; 1ms扫描显示一位
    SETB    B_1ms           ; 1ms标志

    POP     ACC     ;ACC出栈
    POP     PSW     ;PSW出栈
    RETI

;========================================================================
F_RTC_Interrupt:    ;RTC中断函数
    PUSH    PSW     ;PSW入栈
    PUSH    ACC     ;ACC入栈

    MOV     WR6, #WORD0 RTCIF ;检测从机状态
    MOV     WR4, #WORD2 RTCIF
    MOV     R11, @DR4
    JB      ACC.7,ALARMIF
    JB      ACC.3,SECONDIF
    CLR     A
    MOVX    @DPTR,A
    JMP     ISREXIT

ALARMIF:
    ANL     A,#NOT 080H       ;清中断标志
    MOV     @DR4, R11
    CPL     P4.6              ;测试端口
    JMP     ISREXIT

SECONDIF:
    ANL     A,#NOT 08H        ;清中断标志
    MOV     @DR4, R11
    CPL     P4.7              ;测试端口
    JMP     ISREXIT

ISREXIT:
    POP     ACC     ;ACC出栈
    POP     PSW     ;PSW出栈
    RETI

;========================================================================
; 函数: F_DisplayRTC
; 描述: 显示时钟子程序。
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;========================================================================
F_DisplayRTC:
    PUSH    B       ;B入栈

    MOV     WR6, #WORD0 HOUR
    MOV     WR4, #WORD2 HOUR
    MOV     R11, @DR4
    MOV     usrhour, A
    MOV     B, #10
    DIV     AB
    MOV     LED8+7, A
    MOV     LED8+6, B

    MOV     LED8+5, #DIS_

    MOV     WR6, #WORD0 MIN
    MOV     WR4, #WORD2 MIN
    MOV     R11, @DR4
    MOV     usrminute, A
    MOV     B, #10
    DIV     AB
    MOV     LED8+4, A;
    MOV     LED8+3, B;

    MOV     LED8+2, #DIS_

    MOV     WR6, #WORD0 SEC
    MOV     WR4, #WORD2 SEC
    MOV     R11, @DR4
    MOV     B, #10
    DIV     AB
    MOV     LED8+1, A;
    MOV     LED8+0, B;
L_QuitDisplayRTC:
    POP     B       ;B出栈
    RET

;========================================================================
; 函数: F_WriteRTC
; 描述: 写RTC函数。
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 
;========================================================================
F_WriteRTC:
    MOV     WR6, #WORD0 YEAR
    MOV     WR4, #WORD2 YEAR
    MOV     R11, @DR4
    MOV     WR6, #WORD0 INIYEAR  ;继承当前年
    MOV     WR4, #WORD2 INIYEAR
    MOV     @DR4, R11

    MOV     WR6, #WORD0 MONTH
    MOV     WR4, #WORD2 MONTH
    MOV     R11, @DR4
    MOV     WR6, #WORD0 INIMONTH ;继承当前月
    MOV     WR4, #WORD2 INIMONTH
    MOV     @DR4, R11

    MOV     WR6, #WORD0 DAY
    MOV     WR4, #WORD2 DAY
    MOV     R11, @DR4
    MOV     WR6, #WORD0 INIDAY   ;继承当前日
    MOV     WR4, #WORD2 INIDAY
    MOV     @DR4, R11

    MOV     A,usrhour
    MOV     WR6, #WORD0 INIHOUR  ;修改时
    MOV     WR4, #WORD2 INIHOUR
    MOV     @DR4, R11

    MOV     A,usrminute
    MOV     WR6, #WORD0 INIMIN   ;修改分
    MOV     WR4, #WORD2 INIMIN
    MOV     @DR4, R11

    MOV     A,#0
    MOV     WR6, #WORD0 INISEC   ;修改秒
    MOV     WR4, #WORD2 INISEC
    MOV     @DR4, R11

    MOV     A,#0
    MOV     WR6, #WORD0 INISSEC  ;修改1/128秒
    MOV     WR4, #WORD2 INISSEC
    MOV     @DR4, R11

    MOV     WR6, #WORD0 RTCCFG
    MOV     WR4, #WORD2 RTCCFG
    MOV     R11, @DR4
    ORL     A,#01H
    MOV     @DR4, R11
    RET



    END

