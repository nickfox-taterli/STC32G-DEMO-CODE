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

;��ADC�����ⲿ��ѹ��ʹ���ڲ���׼�����ѹ.

;��STC��MCU��IO��ʽ����8λ����ܡ�

;ʹ��Timer0��16λ�Զ���װ������1ms����,�������������������, �û��޸�MCU��ʱ��Ƶ��ʱ,�Զ���ʱ��1ms.

;�ұ�4λ�������ʾ�����ĵ�ѹֵ.

;�ⲿ��ѹ�Ӱ��ϲ��µ�����������, �����ѹ0~Vref, ��Ҫ����Vref�����0V. 

;ʵ����Ŀʹ���봮һ��1K�ĵ��赽ADC�����, ADC������ٲ�һ�����ݵ���.

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

msecond         DATA    39H     ;
Bandgap         DATA    3BH     ;
ADC3            DATA    3DH     ;


;*******************************************************************
;*******************************************************************
        ORG     0000H               ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
        LJMP    F_Main

        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt


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
    MOV     P1M1, #3aH     ;����P1.1��P1.4��P1.5Ϊ©����·(ʵ��������������赽3.3V), P1.1��PWM��DAC��·ͨ�����贮����P2.3,  P1.3 Ϊ ADC ��������
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
    SETB    EA          ; �����ж�

    LCALL   F_ADC_config    ; ADC��ʼ��

;=====================================================

;=====================================================
L_Main_Loop:
    JNB     B_1ms,  L_Main_Loop     ;1msδ��
    CLR     B_1ms
    
;=================== ���300ms�Ƿ� ==================================
    MOV     WR6, msecond
    INC     WR6, #1         ;msecond + 1
    MOV     msecond, WR6
    CMP     WR6, #300
    JC      L_Main_Loop     ;if(msecond < 300), jmp
    MOV     WR6, #0
    MOV     msecond, WR6    ;msecond = 0

    ;MOV     A, #15
    ;LCALL   F_Get_ADC12bitResult    ;ACC - ͨ��0~15, �ȶ�һ�β��������, ���ڲ��Ĳ������ݵĵ�ѹ��������ֵ.
    MOV     A, #15
    LCALL   F_Get_ADC12bitResult    ;���ڲ���׼ADC, ��15ͨ��, ��ѯ��ʽ��һ��ADC, ����ֵ(R6 R7)����ADC���, == 4096 Ϊ����
    MOV     Bandgap, WR6            ;����Bandgap
    
    MOV     A, #3
    LCALL   F_Get_ADC12bitResult    ; ���ⲿ��ѹADC, ��ѯ��ʽ��һ��ADC, ����ֵ(R6 R7)����ADC���, == 4096 Ϊ����
    MOV     ADC3, WR6               ;����adc

    MOV     WR2, #119       ; adc * 119 / Bandgap, �����ⲿ��ѹ, BandgapΪ1.19V, ���ѹ�ֱ���0.01V
    MUL     WR6, WR2        ;(R6,R7)* R3 -->(R4,R5,R6,R7)

    MOV     WR0, #00000H    ;����(R0,R1,R2,R3)
    MOV     WR2, Bandgap
    MOV     DMAIR, #04H     ;32λ�޷��ų��� (R4,R5,R6,R7)/(R0,R1,R2,R3)=(R4,R5,R6,R7),������(R0,R1,R2,R3)

    MOV     WR0, #00000H
    MOV     WR2, #100
    MOV     DMAIR, #04H     ;32λ�޷��ų���
    MOV     A, R7           ;��ʾ��ѹֵ
    ANL     A, #0x0F
    ADD     A, #DIS_DOT
    MOV     LED8+2, A

    MOV     WR4, WR0        ;������������
    MOV     WR6, WR2
    MOV     WR0, #00000H
    MOV     WR2, #10
    MOV     DMAIR, #04H     ;32λ�޷��ų���
    MOV     A, R7           ;��ʾ��ѹֵ
    ANL     A, #0x0F
    MOV     LED8+1, A

    MOV     A, R3           ;��ʾ��ѹֵ
    ANL     A, #0x0F
    MOV     LED8+0, A

L_Quit_Check_300ms:

;=====================================================

    LJMP    L_Main_Loop

;**********************************************/

F_ADC_config:
    MOV     A, #03FH            ; ���� ADC �ڲ�ʱ��ADC����ʱ�佨�������ֵ
    MOV     WR6, #WORD0 ADCTIM
    MOV     WR4, #WORD2 ADCTIM
    MOV     @DR4, R11

    MOV     ADCCFG, #02FH       ; ����ת������Ҷ���ģʽ�� ADC ʱ��Ϊϵͳʱ��/2/16
    MOV     ADC_CONTR, #080H    ; ʹ�� ADC ģ��
    RET

;========================================================================
; ����: F_Get_ADC12bitResult
; ����: ��ѯ����һ��ADC���.
; ����: ACC: ѡ��Ҫת����ADC.
; ����: (R6 R7) = 12λADC���.
; �汾: V1.0, 2020-11-09
;========================================================================
F_Get_ADC12bitResult:   ;ACC - ͨ��0~7, ��ѯ��ʽ��һ��ADC, ����ֵ(R6 R7)����ADC���, == 4096 Ϊ����
    MOV     R7, A           //channel
    MOV     ADC_RES,  #0;
    MOV     ADC_RESL, #0;

    MOV     A, ADC_CONTR        ;ADC_CONTR = (ADC_CONTR & 0xd0) | ADC_START | channel; 
    ANL     A, #0D0H
    ORL     A, #40H
    ORL     A, R7
    MOV     ADC_CONTR, A
    NOP
    NOP
    NOP
    NOP

L_WaitAdcLoop:
    MOV     A, ADC_CONTR
    JNB     ACC.5, L_WaitAdcLoop

    ANL     ADC_CONTR, #NOT 020H    ;�����ɱ�־

    MOV     A, ADC_RES      ;12λAD����ĸ�4λ��ADC_RES�ĵ�4λ����8λ��ADC_RESL��
    ANL     A, #0FH
    MOV     R6, A
    MOV     R7, ADC_RESL

L_QuitAdc:
    RET


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

;**************** �жϺ��� ***************************************************

F_Timer0_Interrupt: ;Timer0 1ms�жϺ���
    PUSH    PSW     ;PSW��ջ
    PUSH    ACC     ;ACC��ջ

    LCALL   F_DisplayScan   ; 1msɨ����ʾһλ
    SETB    B_1ms           ; 1ms��־

    POP     ACC     ;ACC��ջ
    POP     PSW     ;PSW��ջ
    RETI

;***************************************************************************

    END

