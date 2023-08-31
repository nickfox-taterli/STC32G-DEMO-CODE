;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- STC 1T Series MCU Demo Programme -------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
;/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/* --- Web: www.STCAIMCU.com ------------------------------------------*/
;/* --- QQ:  800003751 -------------------------------------------------*/
;/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
;/*---------------------------------------------------------------------*/


;*************  ����˵��    **************

;�����̻���STC32GΪ����оƬ��ʵ������б�д���ԡ�

;ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

;edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

;��доƬ�ڲ����ɵ�RTCģ��.

;��STC��MCU��IO��ʽ����8λ����ܡ�

;ʹ��Timer0��16λ�Զ���װ������1ms����,�������������������, �û��޸�MCU��ʱ��Ƶ��ʱ,�Զ���ʱ��1ms.

;8λ�������ʾʱ��(Сʱ-����-��).

;����ɨ�谴������Ϊ25~32.

;����ֻ֧�ֵ�������, ��֧�ֶ��ͬʱ����, ���������в���Ԥ֪�Ľ��.

;�����³���1���,����10��/����ٶ��ṩ�ؼ����. �û�ֻ��Ҫ���KeyCode�Ƿ��0���жϼ��Ƿ���.

;����ʱ���:
;����25: Сʱ+.
;����26: Сʱ-.
;����27: ����+.
;����28: ����-.

;����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

;******************************************/

$include (../../comm/STC32G.INC)

;/****************************** �û������ ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;��ջ��ʼ��ַ

Timer0_Reload   EQU     (65536 - Fosc_KHZ)  ; Timer 0 �ж�Ƶ��, 1000��/��

DIS_DOT         EQU     020H
DIS_BLACK       EQU     010H
DIS_            EQU     011H

;*******************************************************************
;*******************************************************************


;*************  IO�ڶ���    **************/


;*************  ���ر�������    **************/
Flag0           DATA    20H
B_1ms           BIT     Flag0.0 ;   1ms��־

LED8            DATA    30H     ;   ��ʾ���� 30H ~ 37H
display_index   DATA    38H     ;   ��ʾλ����

usrhour         DATA    39H     ;RTC����
usrminute       DATA    3AH
usrsecond       DATA    3BH     ;
msecond         DATA    3CH     ; 2 byte

IO_KeyState     DATA    3EH ; IO���м�״̬����
IO_KeyState1    DATA    3FH
IO_KeyHoldCnt   DATA    40H ; IO�����¼�ʱ
KeyCode         DATA    41H ; ���û�ʹ�õļ���, 1~16ΪADC���� 17~32ΪIO��

cnt50ms         DATA    45H;


;*******************************************************************
;*******************************************************************
        ORG     0000H               ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
        LJMP    F_Main


        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt

        ORG     0123H               ;36 RTC interrupt
        LJMP    F_RTC_Interrupt


;*******************************************************************
;*******************************************************************


;******************** ������ **************************/
        ORG     0200H       ;�������Զ����嵽 0FF0200H ��ַ
F_Main:
    MOV     WTST, #00H     ;���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    MOV     CKCON,#00H     ;��߷���XRAM�ٶ�
    ORL     P_SW2,#080H    ;ʹ�ܷ���XFR

    MOV     P0M1, #30H     ;����P0.4��P0.5Ϊ©����·(ʵ��������������赽3.3V)
    MOV     P0M0, #30H
    MOV     P1M1, #32H     ;����P1.1��P1.4��P1.5Ϊ©����·(ʵ��������������赽3.3V), P1.1��PWM��DAC��·ͨ�����贮����P2.3
    MOV     P1M0, #32H
    MOV     P2M1, #3cH     ;����P2.2~P2.5Ϊ©����·(ʵ��������������赽3.3V)
    MOV     P2M0, #3cH
    MOV     P3M1, #50H     ;����P3.4��P3.6Ϊ©����·(ʵ��������������赽3.3V)
    MOV     P3M0, #50H
    MOV     P4M1, #3cH     ;����P4.2~P4.5Ϊ©����·(ʵ��������������赽3.3V)
    MOV     P4M0, #3cH
    MOV     P5M1, #0cH     ;����P5.2��P5.3Ϊ©����·(ʵ��������������赽3.3V)
    MOV     P5M0, #0cH
    MOV     P6M1, #0ffH    ;����Ϊ©����·(ʵ��������������赽3.3V)
    MOV     P6M0, #0ffH
    MOV     P7M1, #00H     ;����Ϊ׼˫���
    MOV     P7M0, #00H

    MOV     SP, #STACK_POIRTER
    MOV     PSW, #0     ;ѡ���0��R0~R7
    USING   0       ;ѡ���0��R0~R7

;================= �û���ʼ������ ====================================

    MOV     display_index, #0
    MOV     R0, #LED8
    MOV     R2, #8
L_ClearLoop:
    MOV     @R0, #DIS_BLACK     ;�ϵ�����
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
    SETB    EA          ; �����ж�

    LCALL   F_DisplayRTC

    CLR     A
    MOV     IO_KeyState, A
    MOV     IO_KeyState1, A
    MOV     IO_KeyHoldCnt, A
    MOV     KeyCode, A      ; ���û�ʹ�õļ���, 17~32��Ч
    MOV     cnt50ms, #50

;=====================================================

;=================== ��ѭ�� ==================================
L_Main_Loop:
    JNB     B_1ms,  L_Main_Loop     ;1msδ��
    CLR     B_1ms
    
;=================== ���1000ms�Ƿ� ==================================
    MOV     WR6, msecond
    INC     WR6, #1         ;msecond + 1
    MOV     msecond, WR6
    CMP     WR6, #1000
    JC      L_Quit_Check_1000ms  ;if(msecond < 1000), jmp
    MOV     WR6, #0
    MOV     msecond, WR6    ;msecond = 0
    
;================= 1000ms���� ����RTC ====================================
    MOV     WR6, #0
    MOV     msecond, WR6    ;msecond = 0

    LCALL   F_DisplayRTC
L_Quit_Check_1000ms:

;======================= 50msɨ��һ�����м��� ==============================
L_Read_IO_Key:
    DJNZ    cnt50ms, L_Quit_Read_IO_Key     ; (cnt50ms - 1) != 0, jmp
    MOV     cnt50ms, #50    ;
    LCALL   F_IO_KeyScan    ;50msɨ��һ�����м���
L_Quit_Read_IO_Key:
;=====================================================


;=====================================================
L_KeyProcess:
    MOV     A, KeyCode
    JZ      L_Quit_KeyProcess
                        ;�м�����
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
; ����: F_RTC_Init
; ����: RTC��ʼ������.
; ����: none
; ����: none.
; �汾: V1.0, 2021-3-4
;========================================================================
F_RTC_Init:
    ;-------ʱ������------
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

    ;-------��������------
    MOV     A,#0
    MOV     WR6, #WORD0 ALAHOUR  ;����Сʱ
    MOV     WR4, #WORD2 ALAHOUR
    MOV     @DR4, R11

    MOV     A,#0
    MOV     WR6, #WORD0 ALAMIN   ;���ӷ���
    MOV     WR4, #WORD2 ALAMIN
    MOV     @DR4, R11

    MOV     A,#0
    MOV     WR6, #WORD0 ALASEC   ;������
    MOV     WR4, #WORD2 ALASEC
    MOV     @DR4, R11

    MOV     A,#0
    MOV     WR6, #WORD0 ALASSEC  ;����1/128��
    MOV     WR4, #WORD2 ALASSEC
    MOV     @DR4, R11

    ;-------ʱ������------
    ;STC32G оƬʹ���ڲ�32Kʱ�ӣ������޷�����
;    MOV     A,#80H	            ;�����ڲ�32K IRC
;    MOV     WR6, #WORD0 IRC32KCR
;    MOV     WR4, #WORD2 IRC32KCR
;    MOV     @DR4, R11
;    MOV     R11,@DR4
;    JNB     ACC.0,$-1	        ;�ȴ�ʱ���ȶ�
;    MOV     A,#1
;    MOV     A,#3
;    MOV     WR6, #WORD0 RTCCFG  ;ѡ���ڲ�32Kʱ��Դ������RTC�Ĵ�����ʼ��
;    MOV     WR4, #WORD2 RTCCFG
;    MOV     @DR4, R11

    MOV     A,#0C0H
    MOV     WR6, #WORD0 X32KCR  ;�����ⲿ32K����, ������+0x00, ������+0x40.
    MOV     WR4, #WORD2 X32KCR
    MOV     @DR4, R11
    MOV     R11, @DR4
    JNB     ACC.0,$-1	        ;�ȴ�ʱ���ȶ�
    MOV     A,#1
    MOV     WR6, #WORD0 RTCCFG  ;ѡ���ⲿ32Kʱ��Դ������RTC�Ĵ�����ʼ��
    MOV     WR4, #WORD2 RTCCFG
    MOV     @DR4, R11

    ;-------�ж�����------
    MOV     A,#0
    MOV     WR6, #WORD0 RTCIF   ;���жϱ�־
    MOV     WR4, #WORD2 RTCIF
    MOV     @DR4, R11

    MOV     A,#088H
    MOV     WR6, #WORD0 RTCIEN  ;�ж�ʹ��, 0x80:�����ж�, 0x40:���ж�, 0x20:Сʱ�ж�, 0x10:�����ж�, 0x08:���ж�, 0x04:1/2���ж�, 0x02:1/8���ж�, 0x01:1/32���ж�
    MOV     WR4, #WORD2 RTCIEN
    MOV     @DR4, R11

    ;-------RTCʹ��-------
    MOV     A,#1
    MOV     WR6, #WORD0 RTCCR   ;RTCʹ��
    MOV     WR4, #WORD2 RTCCR
    MOV     @DR4, R11

    MOV     WR6, #WORD0 RTCCFG
    MOV     WR4, #WORD2 RTCCFG
    MOV     R11,@DR4
    JNB     ACC.0,$-1	        ;�ȴ���ʼ�����,��Ҫ�� "RTCʹ��" ֮���ж�. 
    ;����RTCʱ����Ҫ32768Hz��1������ʱ��,��Լ30.5us. ����ͬ��, ����ʵ�ʵȴ�ʱ����0~30.5us.
    ;������ȴ�������ɾ�˯��, ��RTC����������û���, ֹͣ����, ���Ѻ�ż���������ò���������.
    RET


;/*****************************************************
;   ���м�ɨ�����
;   ʹ��XY����4x4���ķ�����ֻ�ܵ������ٶȿ�
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
    PUSH    03H     ;R3��ջ
    MOV     R3, #60
    DJNZ    R3, $   ; (n * 4) T
    POP     03H     ;R3��ջ
    RET

F_IO_KeyScan:   ;50ms call
    PUSH    06H     ;R6��ջ
    PUSH    07H     ;R7��ջ

    MOV     R6, IO_KeyState1    ; ������һ��״̬

    MOV     P0, #0F0H       ;X�ͣ���Y
    LCALL   F_IO_KeyDelay       ;delay about 250T
    MOV     A, P0
    ANL     A, #0F0H
    MOV     IO_KeyState1, A     ; IO_KeyState1 = P0 & 0xf0

    MOV     P0, #0FH        ;Y�ͣ���X
    LCALL   F_IO_KeyDelay       ;delay about 250T
    MOV     A, P0
    ANL     A, #0FH
    ORL     A, IO_KeyState1         ; IO_KeyState1 |= (P0 & 0x0f)
    MOV     IO_KeyState1, A
    XRL     IO_KeyState1, #0FFH     ; IO_KeyState1 ^= 0xff; ȡ��

    MOV     A, R6                   ;if(j == IO_KeyState1), �������ζ����
    CJNE    A, IO_KeyState1, L_QuitCheckIoKey   ;�����, jmp
    
    MOV     R6, IO_KeyState     ;�ݴ�IO_KeyState
    MOV     IO_KeyState, IO_KeyState1
    MOV     A, IO_KeyState
    JZ      L_NoIoKeyPress      ; if(IO_KeyState != 0), �м�����

    MOV     A, R6   
    JZ      L_CalculateIoKey    ;if(R6 == 0)    F0 = 1; ��һ�ΰ���
    MOV     A, R6   
    CJNE    A, IO_KeyState, L_QuitCheckIoKey    ; if(j != IO_KeyState), jmp
    
    INC     IO_KeyHoldCnt   ; if(++IO_KeyHoldCnt >= 20),    1����ؼ�
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
    
    MOV     A, R6       ;KeyCode = (j - 1) * 4 + T_KeyTable[IO_KeyState & 0x0f] + 16;   //������룬17~32
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
    POP     07H     ;R7��ջ
    POP     06H     ;R6��ջ
    REt



; *********************** ��ʾ��س��� ****************************************
T_Display:                      ;��׼�ֿ�
;    0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
DB  03FH,006H,05BH,04FH,066H,06DH,07DH,007H,07FH,06FH,077H,07CH,039H,05EH,079H,071H
;  black  -    H    J    K    L    N    o    P    U    t    G    Q    r    M    y
DB  000H,040H,076H,01EH,070H,038H,037H,05CH,073H,03EH,078H,03dH,067H,050H,037H,06EH
;    0.   1.   2.   3.   4.   5.   6.   7.   8.   9.   -1
DB  0BFH,086H,0DBH,0CFH,0E6H,0EDH,0FDH,087H,0FFH,0EFH,046H

T_COM:
DB  001H,002H,004H,008H,010H,020H,040H,080H     ;   λ��


;//========================================================================
;// ����: F_DisplayScan
;// ����: ��ʾɨ���ӳ���
;// ����: none.
;// ����: none.
;// �汾: VER1.0
;// ����: 2013-4-1
;// ��ע: ����ACCC��PSW��, ���õ���ͨ�üĴ�������ջ
;//========================================================================
F_DisplayScan:
    PUSH    DPH     ;DPH��ջ
    PUSH    DPL     ;DPL��ջ
    PUSH    00H     ;R0 ��ջ
    
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
    MOV     display_index, #0;  ;8λ������0
L_QuitDisplayScan:
    POP     00H     ;R0 ��ջ
    POP     DPL     ;DPL��ջ
    POP     DPH     ;DPH��ջ
    RET

;**************** �жϺ��� ***********************************************

F_Timer0_Interrupt: ;Timer0 1ms�жϺ���
    PUSH    PSW     ;PSW��ջ
    PUSH    ACC     ;ACC��ջ

    LCALL   F_DisplayScan   ; 1msɨ����ʾһλ
    SETB    B_1ms           ; 1ms��־

    POP     ACC     ;ACC��ջ
    POP     PSW     ;PSW��ջ
    RETI

;========================================================================
F_RTC_Interrupt:    ;RTC�жϺ���
    PUSH    PSW     ;PSW��ջ
    PUSH    ACC     ;ACC��ջ

    MOV     WR6, #WORD0 RTCIF ;���ӻ�״̬
    MOV     WR4, #WORD2 RTCIF
    MOV     R11, @DR4
    JB      ACC.7,ALARMIF
    JB      ACC.3,SECONDIF
    CLR     A
    MOVX    @DPTR,A
    JMP     ISREXIT

ALARMIF:
    ANL     A,#NOT 080H       ;���жϱ�־
    MOV     @DR4, R11
    CPL     P4.6              ;���Զ˿�
    JMP     ISREXIT

SECONDIF:
    ANL     A,#NOT 08H        ;���жϱ�־
    MOV     @DR4, R11
    CPL     P4.7              ;���Զ˿�
    JMP     ISREXIT

ISREXIT:
    POP     ACC     ;ACC��ջ
    POP     PSW     ;PSW��ջ
    RETI

;========================================================================
; ����: F_DisplayRTC
; ����: ��ʾʱ���ӳ���
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
; ��ע: ����ACCC��PSW��, ���õ���ͨ�üĴ�������ջ
;========================================================================
F_DisplayRTC:
    PUSH    B       ;B��ջ

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
    POP     B       ;B��ջ
    RET

;========================================================================
; ����: F_WriteRTC
; ����: дRTC������
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
; ��ע: 
;========================================================================
F_WriteRTC:
    MOV     WR6, #WORD0 YEAR
    MOV     WR4, #WORD2 YEAR
    MOV     R11, @DR4
    MOV     WR6, #WORD0 INIYEAR  ;�̳е�ǰ��
    MOV     WR4, #WORD2 INIYEAR
    MOV     @DR4, R11

    MOV     WR6, #WORD0 MONTH
    MOV     WR4, #WORD2 MONTH
    MOV     R11, @DR4
    MOV     WR6, #WORD0 INIMONTH ;�̳е�ǰ��
    MOV     WR4, #WORD2 INIMONTH
    MOV     @DR4, R11

    MOV     WR6, #WORD0 DAY
    MOV     WR4, #WORD2 DAY
    MOV     R11, @DR4
    MOV     WR6, #WORD0 INIDAY   ;�̳е�ǰ��
    MOV     WR4, #WORD2 INIDAY
    MOV     @DR4, R11

    MOV     A,usrhour
    MOV     WR6, #WORD0 INIHOUR  ;�޸�ʱ
    MOV     WR4, #WORD2 INIHOUR
    MOV     @DR4, R11

    MOV     A,usrminute
    MOV     WR6, #WORD0 INIMIN   ;�޸ķ�
    MOV     WR4, #WORD2 INIMIN
    MOV     @DR4, R11

    MOV     A,#0
    MOV     WR6, #WORD0 INISEC   ;�޸���
    MOV     WR4, #WORD2 INISEC
    MOV     @DR4, R11

    MOV     A,#0
    MOV     WR6, #WORD0 INISSEC  ;�޸�1/128��
    MOV     WR4, #WORD2 INISSEC
    MOV     @DR4, R11

    MOV     WR6, #WORD0 RTCCFG
    MOV     WR4, #WORD2 RTCCFG
    MOV     R11, @DR4
    ORL     A,#01H
    MOV     @DR4, R11
    RET



    END

