;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- STC 1T Series MCU Demo Programme -------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
;/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/* --- Web: www.STCAIMCU.com ------------------------------------------*/
;/* --- QQ:  800003751 -------------------------------------------------*/
;/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����        */
;/*---------------------------------------------------------------------*/


;/************* ����˵��    **************

;�����̻���STC32GΪ����оƬ��ʵ������б�д���ԡ�

;ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

;edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

;�߼�PWM��ʱ�� PWM5,PWM6,PWM7,PWM8 ÿ��ͨ�����ɶ���ʵ��PWM���.

;4��ͨ��PWM������Ҫ���ö�Ӧ����ڣ���ͨ��ʾ�����۲�������ź�.

;PWM���ں�ռ�ձȿ��Ը�����Ҫ�������ã���߿ɴ�65535.

;����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

;******************************************/

$include (../../comm/STC32G.INC)

;/****************************** �û������ ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;��ջ��ʼ��ַ

Timer0_Reload   EQU     (65536 - Fosc_KHZ)  ; Timer 0 �ж�Ƶ��, 1000��/��

;*******************************************************************
;*******************************************************************


;*************  ���ر�������    **************/
PWM5_Flag       BIT     20H.0
PWM6_Flag       BIT     20H.1
PWM7_Flag       BIT     20H.2
PWM8_Flag       BIT     20H.3

PWM5_Duty       DATA    30H
PWM6_Duty       DATA    32H
PWM7_Duty       DATA    34H
PWM8_Duty       DATA    36H

;*******************************************************************
;*******************************************************************
        ORG     0000H               ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
        LJMP    F_Main

        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt

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
    CLR     PWM5_Flag
    CLR     PWM6_Flag
    CLR     PWM7_Flag
    CLR     PWM8_Flag
    MOV     WR6, #0
    MOV     PWM5_Duty, WR6
    MOV     WR6, #256
    MOV     PWM6_Duty, WR6
    MOV     WR6, #512
    MOV     PWM7_Duty, WR6
    MOV     WR6, #1024
    MOV     PWM8_Duty, WR6

    CLR     TR0
    ORL     AUXR, #(1 SHL 7)    ; Timer0_1T();
    ANL     TMOD, #NOT 04H      ; Timer0_AsTimer();
    ANL     TMOD, #NOT 03H      ; Timer0_16bitAutoReload();
    MOV     TH0, #Timer0_Reload / 256   ;Timer0_Load(Timer0_Reload);
    MOV     TL0, #Timer0_Reload MOD 256
    SETB    ET0         ; Timer0_InterruptEnable();
    SETB    TR0         ; Timer0_Run();
    SETB    EA          ; �����ж�
    
    LCALL   F_PWM_Init          ; PWM��ʼ��

;=================== ��ѭ�� ==================================
L_Main_Loop:

    LJMP    L_Main_Loop

;========================================================================
; ����: F_PWM_Init
; ����: PWM��ʼ������.
; ����: none
; ����: none.
; �汾: V1.0, 2021-3-3
;========================================================================
F_PWM_Init:
    MOV     A, #00H              ;д CCMRx ǰ���������� CCxE �ر�ͨ��
    MOV     WR6, #WORD0 PWMB_CCER1
    MOV     WR4, #WORD2 PWMB_CCER1
    MOV     @DR4, R11
    MOV     WR6, #WORD0 PWMB_CCER2
    MOV     WR4, #WORD2 PWMB_CCER2
    MOV     @DR4, R11
    MOV     A, #068H             ;�� PWMx ģʽ1 ���
    MOV     WR6, #WORD0 PWMB_CCMR1
    MOV     WR4, #WORD2 PWMB_CCMR1
    MOV     @DR4, R11
    MOV     WR6, #WORD0 PWMB_CCMR2
    MOV     WR4, #WORD2 PWMB_CCMR2
    MOV     @DR4, R11
    MOV     WR6, #WORD0 PWMB_CCMR3
    MOV     WR4, #WORD2 PWMB_CCMR3
    MOV     @DR4, R11
    MOV     WR6, #WORD0 PWMB_CCMR4
    MOV     WR4, #WORD2 PWMB_CCMR4
    MOV     @DR4, R11
    MOV     A, #033H             ;����ͨ�����ʹ�ܺͼ���
    MOV     WR6, #WORD0 PWMB_CCER1
    MOV     WR4, #WORD2 PWMB_CCER1
    MOV     @DR4, R11
    MOV     WR6, #WORD0 PWMB_CCER2
    MOV     WR4, #WORD2 PWMB_CCER2
    MOV     @DR4, R11

    MOV     A, #3                ;��������ʱ��
    MOV     WR6, #WORD0 PWMB_ARRH
    MOV     WR4, #WORD2 PWMB_ARRH
    MOV     @DR4, R11
    MOV     A, #0FFH
    MOV     WR6, #WORD0 PWMB_ARRL
    MOV     WR4, #WORD2 PWMB_ARRL
    MOV     @DR4, R11

    MOV     A, #055H             ;ʹ�� PWM5~8 ���
    MOV     WR6, #WORD0 PWMB_ENO
    MOV     WR4, #WORD2 PWMB_ENO
    MOV     @DR4, R11
    MOV     A, #00H              ;�߼� PWM ͨ�������ѡ��λ, P2
    MOV     WR6, #WORD0 PWMB_PS
    MOV     WR4, #WORD2 PWMB_PS
    MOV     @DR4, R11
    MOV     A, #080H             ;ʹ�������
    MOV     WR6, #WORD0 PWMB_BKR
    MOV     WR4, #WORD2 PWMB_BKR
    MOV     @DR4, R11

    MOV     WR6, #WORD0 PWMB_CR1
    MOV     WR4, #WORD2 PWMB_CR1
    MOV     R11, @DR4
    ORL     A,#081H             ;ʹ��ARRԤװ�أ���ʼ��ʱ
    MOV     @DR4, R11
    RET

;========================================================================
; ����: F_UpdatePwm
; ����: ����PWMռ�ձ�ֵ. 
; ����: [PWMn_Duty_H PWMn_Duty_L]: pwmռ�ձ�ֵ.
; ����: none.
; �汾: V1.0, 2021-3-3
;========================================================================
F_UpdatePwm:
    MOV     WR2, PWM5_Duty      ;����ռ�ձ�ʱ��
    MOV     WR6, #WORD0 PWMB_CCR5H
    MOV     WR4, #WORD2 PWMB_CCR5H
    MOV     @DR4, R2
    MOV     WR6, #WORD0 PWMB_CCR5L
    MOV     @DR4, R3

    MOV     WR2, PWM6_Duty      ;����ռ�ձ�ʱ��
    MOV     WR6, #WORD0 PWMB_CCR6H
    MOV     WR4, #WORD2 PWMB_CCR6H
    MOV     @DR4, R2
    MOV     WR6, #WORD0 PWMB_CCR6L
    MOV     @DR4, R3

    MOV     WR2, PWM7_Duty      ;����ռ�ձ�ʱ��
    MOV     WR6, #WORD0 PWMB_CCR7H
    MOV     WR4, #WORD2 PWMB_CCR7H
    MOV     @DR4, R2
    MOV     WR6, #WORD0 PWMB_CCR7L
    MOV     @DR4, R3

    MOV     WR2, PWM8_Duty      ;����ռ�ձ�ʱ��
    MOV     WR6, #WORD0 PWMB_CCR8H
    MOV     WR4, #WORD2 PWMB_CCR8H
    MOV     @DR4, R2
    MOV     WR6, #WORD0 PWMB_CCR8L
    MOV     @DR4, R3
    RET

;**************** �жϺ��� ***************************************************
F_Timer0_Interrupt: ;Timer0 1ms�жϺ���
    PUSH    PSW     ;PSW��ջ
    PUSH    ACC     ;ACC��ջ
    PUSH    R1      ;R1��ջ
    PUSH    R0      ;R0��ջ

    JB      PWM5_Flag, T0_PWM5_SUB
    MOV     WR0, PWM5_Duty
    INC     WR0, #1
    MOV     PWM5_Duty, WR0   ;PWM5_Duty + 1
    CMP     WR0, #1023
    JC      T0_PWM6_Start
    SETB    PWM5_Flag
    SJMP    T0_PWM6_Start
T0_PWM5_SUB:
    MOV     WR0, PWM5_Duty
    DEC     WR0, #1
    MOV     PWM5_Duty, WR0   ;PWM5_Duty - 1
    CMP     WR0, #0
    JG      T0_PWM6_Start
    CLR     PWM5_Flag

T0_PWM6_Start:
    JB      PWM6_Flag, T0_PWM6_SUB
    MOV     WR0, PWM6_Duty
    INC     WR0, #1
    MOV     PWM6_Duty, WR0   ;PWM6_Duty + 1
    CMP     WR0, #1023
    JC      T0_PWM7_Start
    SETB    PWM6_Flag
    SJMP    T0_PWM7_Start
T0_PWM6_SUB:
    MOV     WR0, PWM6_Duty
    DEC     WR0, #1
    MOV     PWM6_Duty, WR0   ;PWM6_Duty - 1
    CMP     WR0, #0
    JG      T0_PWM7_Start
    CLR     PWM6_Flag

T0_PWM7_Start:
    JB      PWM7_Flag, T0_PWM7_SUB
    MOV     WR0, PWM7_Duty
    INC     WR0, #1
    MOV     PWM7_Duty, WR0   ;PWM7_Duty + 1
    CMP     WR0, #1023
    JC      T0_PWM8_Start
    SETB    PWM7_Flag
    SJMP    T0_PWM8_Start
T0_PWM7_SUB:
    MOV     WR0, PWM7_Duty
    DEC     WR0, #1
    MOV     PWM7_Duty, WR0   ;PWM7_Duty - 1
    CMP     WR0, #0
    JG      T0_PWM8_Start
    CLR     PWM7_Flag

T0_PWM8_Start:
    JB      PWM8_Flag, T0_PWM8_SUB
    MOV     WR0, PWM8_Duty
    INC     WR0, #1
    MOV     PWM8_Duty, WR0   ;PWM8_Duty + 1
    CMP     WR0, #1023
    JC      F_QuitTimer0
    SETB    PWM8_Flag
    SJMP    F_QuitTimer0
T0_PWM8_SUB:
    MOV     WR0, PWM8_Duty
    DEC     WR0, #1
    MOV     PWM8_Duty, WR0   ;PWM8_Duty - 1
    CMP     WR0, #0
    JG      F_QuitTimer0
    CLR     PWM8_Flag

F_QuitTimer0:
    LCALL   F_UpdatePwm

    POP     R0      ;R0��ջ
    POP     R1      ;R1��ջ
    POP     ACC     ;ACC��ջ
    POP     PSW     ;PSW��ջ
    RETI

;======================================================================

    END

