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


;*************  ����˵��    **************

;�����̻���STC32GΪ����оƬ��ʵ������б�д���ԡ�

;ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

;edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

;�߼�PWM��ʱ�� PWM1P/PWM1N,PWM2P/PWM2N,PWM3P/PWM3N,PWM4P/PWM4N ÿ��ͨ�����ɶ���ʵ��PWM������������������Գ����.

;��ʾʹ��PWM1P,PWM1N����������SPWM.

;��ʱ��ѡ��24MHZ, PWMʱ��ѡ��1T, PWM����2400, ����12��ʱ��(0.5us).���Ҳ�����200��. 

;������Ҳ�Ƶ�� = 24000000 / 2400 / 200 = 50 HZ.

;�����������һ��SPWM����ʾ����, �û�����ͨ������ļ��㷽���޸�PWM���ں����Ҳ��ĵ����ͷ���.

;���������Ƶ�ʹ̶�, �����Ҫ��Ƶ, ���û��Լ���Ʊ�Ƶ����.

;�������P6.0(PWM1P)�����������, ��P6.1(PWM1N)�����������(����).

;����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

;******************************************/

$include (../../comm/STC32G.INC)

;/****************************** �û������ ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;��ջ��ʼ��ַ

Timer0_Reload   EQU     (65536 - Fosc_KHZ)  ; Timer 0 �ж�Ƶ��, 1000��/��


;*******************************************************************
;*******************************************************************

;*************  IO�ڶ���    **************/

;*************  ���ر�������    **************/

PWM_Index       DATA    30H     ;


;*******************************************************************
;*******************************************************************
        ORG     0000H               ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
        LJMP    F_Main

        ORG     00D3H               ;26  PWMA interrupt
        LJMP    F_PWMA_Interrupt


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
    MOV     PSW, #0
    USING   0       ;ѡ���0��R0~R7

;================= �û���ʼ������ ====================================

    LCALL   F_PWM_Init          ; PWM��ʼ��
    SETB    EA          ; �����ж�

;=====================================================
L_Main_Loop:

    LJMP    L_Main_Loop

;**********************************************/

;========================================================================
; ����: F_PWM_Init
; ����: PWM��ʼ������.
; ����: none
; ����: none.
; �汾: V1.0, 2021-3-3
;========================================================================
F_PWM_Init:
    MOV     A, #00H              ;д CCMRx ǰ���������� CCxE �ر�ͨ��
    MOV     WR6, #WORD0 PWMA_CCER1
    MOV     WR4, #WORD2 PWMA_CCER1
    MOV     @DR4, R11
    MOV     A, #068H
    MOV     WR6, #WORD0 PWMA_CCMR1
    MOV     WR4, #WORD2 PWMA_CCMR1
    MOV     @DR4, R11
    MOV     A, #05H              ;����ͨ�����ʹ�ܺͼ���
    MOV     WR6, #WORD0 PWMA_CCER1
    MOV     WR4, #WORD2 PWMA_CCER1
    MOV     @DR4, R11

    MOV     A, #9                ;��������ʱ��
    MOV     WR6, #WORD0 PWMA_ARRH
    MOV     WR4, #WORD2 PWMA_ARRH
    MOV     @DR4, R11
    MOV     A, #060H
    MOV     WR6, #WORD0 PWMA_ARRL
    MOV     WR4, #WORD2 PWMA_ARRL
    MOV     @DR4, R11

    MOV     A,#HIGH 1220        ;����ռ�ձ�ʱ��
    MOV     WR6, #WORD0 PWMA_CCR1H
    MOV     WR4, #WORD2 PWMA_CCR1H
    MOV     @DR4, R11
    MOV     A,#LOW 1220
    MOV     WR6, #WORD0 PWMA_CCR1L
    MOV     WR4, #WORD2 PWMA_CCR1L
    MOV     @DR4, R11

    MOV     A,#0CH              ;��������ʱ��
    MOV     WR6, #WORD0 PWMA_DTR
    MOV     WR4, #WORD2 PWMA_DTR
    MOV     @DR4, R11

    MOV     A,#03H              ;ʹ�� PWM1P/PWM1N ���
    MOV     WR6, #WORD0 PWMA_ENO
    MOV     WR4, #WORD2 PWMA_ENO
    MOV     @DR4, R11
    MOV     A,#02H              ;�߼� PWM ͨ�������ѡ��λ
    MOV     WR6, #WORD0 PWMA_PS
    MOV     WR4, #WORD2 PWMA_PS
    MOV     @DR4, R11
    MOV     A,#080H             ;ʹ�������
    MOV     WR6, #WORD0 PWMA_BKR
    MOV     WR4, #WORD2 PWMA_BKR
    MOV     @DR4, R11

    MOV     A,#01H              ;ʹ���ж�
    MOV     WR6, #WORD0 PWMA_IER
    MOV     WR4, #WORD2 PWMA_IER
    MOV     @DR4, R11
    MOV     WR6, #WORD0 PWMA_CR1
    MOV     WR4, #WORD2 PWMA_CR1
    MOV     R11, @DR4
    ORL     A,#081H             ;ʹ��ARRԤװ�أ���ʼ��ʱ
    MOV     @DR4, R11
    RET


;========================================================================
; ����: F_PWMA_Interrupt
; ����: PWMA�жϴ������.
; ����: None
; ����: none.
; �汾: V1.0, 2012-11-22
;========================================================================
F_PWMA_Interrupt:
    PUSH    PSW
    PUSH    ACC

    MOV     WR6, #WORD0 PWMA_SR1 ;����жϱ�־
    MOV     WR4, #WORD2 PWMA_SR1
    MOV     R11, @DR4
    JNB     ACC.1,F_PWMA_QuitInt
    CLR     A
    MOV     @DR4, R11

    MOV     A, PWM_Index
    LCALL   F_GetFirstAddress   ; DPTR = #sin_table + ACC * 2
    CLR     A
    MOVC    A, @A+DPTR
    MOV     WR6, #WORD0 PWMA_CCR1H
    MOV     WR4, #WORD2 PWMA_CCR1H
    MOV     @DR4, R11

    MOV     A, #1
    MOVC    A, @A+DPTR
    MOV     WR6, #WORD0 PWMA_CCR1L
    MOV     WR4, #WORD2 PWMA_CCR1L
    MOV     @DR4, R11

    INC     PWM_Index       ;PWM_Index++
    CLR     C
    MOV     A, PWM_Index    ;PWM_Index - 200
    SUBB    A, #200
    JC      F_PWMA_QuitInt  ;if(PWM_Index < 200), jmp
    MOV     PWM_Index, #0   ;if(PWM_Index >= 200)

F_PWMA_QuitInt:
    POP     ACC
    POP     PSW
    RETI


; ��ȡ���ݱ���˫�ֽ����ݵ��׵�ַ
; ����: ACC
; ���: DPTR.   DPTR = #sin_table + ACC * 2

F_GetFirstAddress:  ;DPTR = #sin_table + ACC * 2
    MOV     DPTR, #sin_table
    PUSH    01H     ; R1��ջ
    MOV     R1, A
    ADD     A, DPL
    MOV     DPL, A
    CLR     A
    ADDC    A, DPH
    MOV     DPH, A
    
    MOV     A, R1
    ADD     A, DPL
    MOV     DPL, A
    CLR     A
    ADDC    A, DPH
    MOV     DPH, A
    POP     01H     ;R1 ��ջ
    RET


sin_table:
    DW  1220
    DW  1256
    DW  1292
    DW  1328
    DW  1364
    DW  1400
    DW  1435
    DW  1471
    DW  1506
    DW  1541
    DW  1575
    DW  1610
    DW  1643
    DW  1677
    DW  1710
    DW  1742
    DW  1774
    DW  1805
    DW  1836
    DW  1866
    DW  1896
    DW  1925
    DW  1953
    DW  1981
    DW  2007
    DW  2033
    DW  2058
    DW  2083
    DW  2106
    DW  2129
    DW  2150
    DW  2171
    DW  2191
    DW  2210
    DW  2228
    DW  2245
    DW  2261
    DW  2275
    DW  2289
    DW  2302
    DW  2314
    DW  2324
    DW  2334
    DW  2342
    DW  2350
    DW  2356
    DW  2361
    DW  2365
    DW  2368
    DW  2369
    DW  2370
    DW  2369
    DW  2368
    DW  2365
    DW  2361
    DW  2356
    DW  2350
    DW  2342
    DW  2334
    DW  2324
    DW  2314
    DW  2302
    DW  2289
    DW  2275
    DW  2261
    DW  2245
    DW  2228
    DW  2210
    DW  2191
    DW  2171
    DW  2150
    DW  2129
    DW  2106
    DW  2083
    DW  2058
    DW  2033
    DW  2007
    DW  1981
    DW  1953
    DW  1925
    DW  1896
    DW  1866
    DW  1836
    DW  1805
    DW  1774
    DW  1742
    DW  1710
    DW  1677
    DW  1643
    DW  1610
    DW  1575
    DW  1541
    DW  1506
    DW  1471
    DW  1435
    DW  1400
    DW  1364
    DW  1328
    DW  1292
    DW  1256
    DW  1220
    DW  1184
    DW  1148
    DW  1112
    DW  1076
    DW  1040
    DW  1005
    DW  969
    DW  934
    DW  899
    DW  865
    DW  830
    DW  797
    DW  763
    DW  730
    DW  698
    DW  666
    DW  635
    DW  604
    DW  574
    DW  544
    DW  515
    DW  487
    DW  459
    DW  433
    DW  407
    DW  382
    DW  357
    DW  334
    DW  311
    DW  290
    DW  269
    DW  249
    DW  230
    DW  212
    DW  195
    DW  179
    DW  165
    DW  151
    DW  138
    DW  126
    DW  116
    DW  106
    DW  98
    DW  90
    DW  84
    DW  79
    DW  75
    DW  72
    DW  71
    DW  70
    DW  71
    DW  72
    DW  75
    DW  79
    DW  84
    DW  90
    DW  98
    DW  106
    DW  116
    DW  126
    DW  138
    DW  151
    DW  165
    DW  179
    DW  195
    DW  212
    DW  230
    DW  249
    DW  269
    DW  290
    DW  311
    DW  334
    DW  357
    DW  382
    DW  407
    DW  433
    DW  459
    DW  487
    DW  515
    DW  544
    DW  574
    DW  604
    DW  635
    DW  666
    DW  698
    DW  730
    DW  763
    DW  797
    DW  830
    DW  865
    DW  899
    DW  934
    DW  969
    DW  1005
    DW  1040
    DW  1076
    DW  1112
    DW  1148
    DW  1184

    END