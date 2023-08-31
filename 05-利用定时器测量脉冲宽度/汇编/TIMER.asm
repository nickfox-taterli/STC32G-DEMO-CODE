;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- STC 1T Series MCU Demo Programme -------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
;/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/* --- BBS: www.STCAIMCU.com ------------------------------------------*/
;/* --- QQ:  800003751 -------------------------------------------------*/
;/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����        */
;/*---------------------------------------------------------------------*/


;*************  ����˵��    **************

; �����̻���STC32GΪ����оƬ��ʵ������б�д���ԡ�

; ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

; edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

; ��ʱ��0��16λ�Զ���װ, �ж�Ƶ��Ϊ1000HZ����Ϊ�����ɨ����ʾ.

; ��ʱ��1��16λ�Զ���װ, �ж�Ƶ��Ϊ10000HZ����Ϊ�����źſ�ȼ��.

; ��STC��MCU��IO��ʽ����8λ����ܡ�

; P33�ڲ����͵�ƽ(�û����Զ����޸Ķ˿ڸ�����ƽ)ʱ���м�ʱ����ƽ�仯��ֹͣ��ʱ��

; �������ʾ�����źſ�ȣ���λ0.1ms��������Χ0.1ms~6553.5ms.

; ����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

;******************************************/

$include (../../comm/STC32G.INC)

;/****************************** �û������ ***********************************/


Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;��ջ��ʼ��ַ

DIS_DOT         EQU     020H
DIS_BLACK       EQU     010H
DIS_            EQU     011H

;*******************************************************************
;*******************************************************************

;*************  ���ر�������    **************/
Flag0           DATA    20H
B_1ms           BIT     Flag0.0 ; 1ms��־

LED8            DATA    30H     ; ��ʾ���� 30H ~ 37H
display_index   DATA    38H     ; ��ʾλ����

Test_cnt        DATA    39H     ; ��ʾ�õļ�������
Temp_cnt        DATA    3BH     ; �����õļ�������
msecond         DATA    3DH     ;

;*******************************************************************
;*******************************************************************
        ORG     0000H               ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
        LJMP    F_Main

        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt

        ORG     001BH               ;3  Timer1 interrupt
        LJMP    F_Timer1_Interrupt

;*******************************************************************
;*******************************************************************


;******************** ������ **************************/
        ORG     0100H       ;�������Զ����嵽 0FF0100H ��ַ
F_Main:
    MOV     WTST, #00H     ;���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    MOV     CKCON,#00H     ;��߷���XRAM�ٶ�
    ORL     P_SW2,#080H    ;ʹ�ܷ���XFR

    MOV     P0M1, #30H     ;����P0.4��P0.5Ϊ©����·(ʵ��������������赽3.3V)
    MOV     P0M0, #30H
    MOV     P1M1, #30H     ;����P1.4��P1.5Ϊ©����·(ʵ��������������赽3.3V)
    MOV     P1M0, #30H
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
    MOV     PSW, #0
    USING   0       ;ѡ���0��R0~R7

;================= �û���ʼ������ ====================================

    MOV     WR6, #0
    MOV     Test_cnt, WR6
    MOV     Temp_cnt, WR6
    MOV     display_index, #0
    MOV     R0, #LED8
    MOV     R2, #8
L_ClearLoop:
    MOV     @R0, #DIS_BLACK     ;�ϵ�����
    INC     R0
    DJNZ    R2, L_ClearLoop
    
    LCALL   F_Timer0_init
    LCALL   F_Timer1_init
    SETB    EA
    
;=================== ��ѭ�� ==================================
L_Main_Loop:
    JNB     B_1ms,  L_Main_Loop     ;1msδ��
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
; ����: F_Timer0_init
; ����: timer0��ʼ������.
; ����: none.
; ����: none.
; �汾: V1.0, 2015-1-12
;========================================================================
F_Timer0_init:
    CLR     TR0 ; ֹͣ����
    SETB    ET0 ; �����ж�
;   SETB    PT0 ; �����ȼ��ж�
    ANL     TMOD, #NOT 0x03     ;
    ORL     TMOD, #0            ; ����ģʽ, 0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ, 3: 16λ�Զ���װ, ���������ж�
;   ORL     TMOD, #0x04         ; ����������Ƶ
    ANL     TMOD, #NOT 0x04     ; ��ʱ
;   ORL     INTCLKO, #0x01      ; ���ʱ��
    ANL     INTCLKO, #NOT 0x01  ; �����ʱ��

;   ANL     AUXR, #NOT 0x80     ; 12T mode
    ORL     AUXR, #0x80         ; 1T mode
    MOV     TH0, #HIGH (-Fosc_KHZ) ; - 24000000 / 1000
    MOV     TL0, #LOW  (-Fosc_KHZ) ;
    SETB    TR0 ; ��ʼ����
    RET


;========================================================================
; ����: F_Timer1_init
; ����: timer1��ʼ������.
; ����: none.
; ����: none.
; �汾: V1.0, 2015-1-12
;========================================================================
F_Timer1_init:
    CLR     TR1 ; ֹͣ����
    SETB    ET1 ; �����ж�
;   SETB    PT1 ; �����ȼ��ж�
    ANL     TMOD, #NOT 0x30     ;
    ORL     TMOD, #(0 SHL 4)    ; ����ģʽ, 0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ, 3: 16λ�Զ���װ, ���������ж�
;   ORL     TMOD, #0x40         ; ����������Ƶ
    ANL     TMOD, #NOT 0x40     ; ��ʱ
;   ORL     INTCLKO, #0x02      ; ���ʱ��
    ANL     INTCLKO, #NOT 0x02  ; �����ʱ��

;   ANL     AUXR, #NOT 0x40     ; 12T mode
    ORL     AUXR, #0x40         ; 1T mode
    MOV     TH1, #HIGH (-(Fosc_KHZ / 10)) ; - 24000000 / 10000
    MOV     TL1, #LOW  (-(Fosc_KHZ / 10)) ;
    SETB    TR1 ; ��ʼ����
    RET


;**************** �жϺ��� ***************************************************
F_Timer0_Interrupt: ;Timer0 �жϺ���
    PUSH    PSW     ;PSW��ջ
    PUSH    ACC     ;ACC��ջ

    LCALL   F_DisplayScan   ; 1msɨ����ʾһλ
    SETB    B_1ms           ; 1ms��־

    POP     ACC     ;ACC��ջ
    POP     PSW     ;PSW��ջ
    RETI
    
F_Timer1_Interrupt: ;Timer1 �жϺ���
    PUSH    PSW     ;PSW��ջ
    PUSH    ACC     ;ACC��ջ
    PUSH    R0      ;R0��ջ
    PUSH    R1      ;R1��ջ

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
    POP     R1      ;R1��ջ
    POP     R0      ;R0��ջ
    POP     ACC     ;ACC��ջ
    POP     PSW     ;PSW��ջ
    RETI

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


;========================================================================
; ����: F_DisplayScan
; ����: ��ʾɨ���ӳ���
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
; ��ע: ����ACCC��PSW��, ���õ���ͨ�üĴ�������ջ
;========================================================================
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

;========================================================================
; ����: F_Display
; ����: ��ʾ��������.
; ����: none.
; ����: none.
; �汾: V1.0, 2021-3-1
;========================================================================
F_Display:
    MOV     WR4, #00000H    ;������
    MOV     WR6, Test_cnt
    MOV     WR0, #00000H    ;����
    MOV     WR2, #10000
    MOV     DMAIR, #04H     ;32λ�޷��ų���
    MOV     A, R7           ;��ʾ����ֵ
    ANL     A, #0x0F
    MOV     LED8+4, A

    MOV     WR4, WR0        ;������������
    MOV     WR6, WR2
    MOV     WR0, #00000H
    MOV     WR2, #1000
    MOV     DMAIR, #04H     ;32λ�޷��ų���
    MOV     A, R7           ;��ʾ����ֵ
    ANL     A, #0x0F
    MOV     LED8+3, A

    MOV     WR4, WR0        ;������������
    MOV     WR6, WR2
    MOV     WR0, #00000H
    MOV     WR2, #100
    MOV     DMAIR, #04H     ;32λ�޷��ų���
    MOV     A, R7           ;��ʾ����ֵ
    ANL     A, #0x0F
    MOV     LED8+2, A

    MOV     WR4, WR0        ;������������
    MOV     WR6, WR2
    MOV     WR0, #00000H
    MOV     WR2, #10
    MOV     DMAIR, #04H     ;32λ�޷��ų���
    MOV     A, R7           ;��ʾ����ֵ
    ANL     A, #0x0F
    MOV     LED8+1, A

    MOV     A, R3           ;��ʾ����ֵ
    ANL     A, #0x0F
    MOV     LED8+0, A

    MOV     A, LED8+4       ;��ʾ����Ч0
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

