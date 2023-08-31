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

;ͨ��һ��IO�ڻ�ȡһ�����¶ȴ����� DS18B20 �¶�ֵ.

;ʹ��Timer0��16λ�Զ���װ������1ms����,�������������������, �û��޸�MCU��ʱ��Ƶ��ʱ,�Զ���ʱ��1ms.

;��STC��MCU��IO��ʽ����8λ����ܣ�ͨ���������ʾ�������¶�ֵ.

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
DQ  BIT P3.3                ;DS18B20�����ݿ�λP3.3

;*************  ���ر�������    **************/
Flag0           DATA    20H
B_1ms           BIT     Flag0.0 ;   1ms��־

LED8            DATA    30H     ;   ��ʾ���� 30H ~ 37H
display_index   DATA    38H     ;   ��ʾλ����

msecond         DATA    39H     ;

;*******************************************************************
;*******************************************************************

        ORG     0000H               ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
        LJMP    F_Main

        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt

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

    CALL    DS18B20_Reset       ;�豸��λ
    MOV     A,#0CCH             ;����ROM����
    CALL    DS18B20_WriteByte   ;�ͳ�����
    MOV     A,#044H             ;��ʼת��
    CALL    DS18B20_WriteByte   ;�ͳ�����
    JNB     DQ,$                ;�ȴ�ת�����

    CALL    DS18B20_Reset       ;�豸��λ
    MOV     A,#0CCH             ;����ROM����
    CALL    DS18B20_WriteByte   ;�ͳ�����
    MOV     A,#0BEH             ;���ݴ�洢��
    CALL    DS18B20_WriteByte   ;�ͳ�����
    CALL    DS18B20_ReadByte    ;���¶ȵ��ֽ�
    MOV     R7,A                ;�洢����
    CALL    DS18B20_ReadByte    ;���¶ȸ��ֽ�
    MOV     R6,A                ;�洢����

    MOV     WR2, #5         ;0.0625 * 10������1λС����
    MUL     WR6, WR2        ;(R6,R7)* R3 -->(R4,R5,R6,R7)
    MOV     WR0, #00000H    ;����(R0,R1,R2,R3)
    MOV     WR2, #8          ; �¶� * 0.625 = �¶� * 5/8
    MOV     DMAIR, #04H     ;32λ�޷��ų��� (R4,R5,R6,R7)/(R0,R1,R2,R3)=(R4,R5,R6,R7),������(R0,R1,R2,R3)

    MOV     WR0, #00000H
    MOV     WR2, #1000
    MOV     DMAIR, #04H     ;32λ�޷��ų���
    MOV     A, R7           ;��ʾ�¶�ֵ
    ANL     A, #0x0F
    MOV     LED8+3, A

    MOV     WR4, WR0        ;������������
    MOV     WR6, WR2
    MOV     WR0, #00000H
    MOV     WR2, #100
    MOV     DMAIR, #04H     ;32λ�޷��ų���
    MOV     A, R7           ;��ʾ�¶�ֵ
    ANL     A, #0x0F
    MOV     LED8+2, A

    MOV     WR4, WR0        ;������������
    MOV     WR6, WR2
    MOV     WR0, #00000H
    MOV     WR2, #10
    MOV     DMAIR, #04H     ;32λ�޷��ų���
    MOV     A, R7           ;��ʾ�¶�ֵ
    ANL     A, #0x0F
    ADD     A, #DIS_DOT
    MOV     LED8+1, A

    MOV     A, R3           ;��ʾ�¶�ֵ
    ANL     A, #0x0F
    MOV     LED8+0, A

    MOV     A, LED8+3
    JNZ     L_LED8_3_Not_0
    MOV     LED8+3, #DIS_BLACK      ;ǧλΪ0������
L_LED8_3_Not_0:
    JNB     F0, L_QuitRead_Temp
    MOV     LED8+3, #DIS_   ;���¶�, ��ʾ-
L_QuitRead_Temp:

    LJMP    L_Main_Loop


;**************************************
;��ʱX΢��(12M)
;��ͬ�Ĺ�������,��Ҫ�����˺���
;��ڲ���:R7
;���ڲ���:��
;**************************************
DelayXus:                   ;6 ����ʱ������ʹ��1T��ָ�����ڽ��м���,�봫ͳ��12T��MCU��ͬ
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    DJNZ R1,DelayXus        ;4
    RET                     ;4

;**************************************
;��λDS18B20,������豸�Ƿ����
;��ڲ���:��
;���ڲ���:��
;**************************************
DS18B20_Reset:
    PUSH    1
    CLR     DQ                 ;�ͳ��͵�ƽ��λ�ź�
    MOV     R1,#240            ;��ʱ����480us
    CALL    DelayXus
    MOV     R1,#240
    CALL    DelayXus
    SETB    DQ                 ;�ͷ�������
    MOV     R1,#60             ;�ȴ�60us
    CALL    DelayXus
    MOV     C,DQ               ;����������
    MOV     R1,#240            ;�ȴ��豸�ͷ�������
    CALL    DelayXus
    MOV     R1,#180
    CALL    DelayXus
    JC      DS18B20_Reset      ;����豸������,������ȴ�
    POP     1
    RET

;**************************************
;��DS18B20��1�ֽ�����
;��ڲ���:��
;���ڲ���:ACC
;**************************************
DS18B20_ReadByte:
    CLR     A
    PUSH    0
    PUSH    1
    MOV     0,#8               ;8λ������
ReadNext:
    CLR     DQ                 ;��ʼʱ��Ƭ
    MOV     R1,#1              ;��ʱ�ȴ�
    CALL    DelayXus
    SETB    DQ                 ;׼������
    MOV     R1,#1
    CALL    DelayXus
    MOV     C,DQ               ;��ȡ����
    RRC     A
    MOV     R1,#60             ;�ȴ�ʱ��Ƭ����
    CALL    DelayXus
    DJNZ    0,ReadNext
    POP     1
    POP     0
    RET

;**************************************
;��DS18B20д1�ֽ�����
;��ڲ���:ACC
;���ڲ���:��
;**************************************
DS18B20_WriteByte:
    PUSH    0
    PUSH    1
    MOV     0,#8               ;8λ������
WriteNext:
    CLR     DQ                 ;��ʼʱ��Ƭ
    MOV     R1,#1              ;��ʱ�ȴ�
    CALL    DelayXus
    RRC     A                  ;�������
    MOV     DQ,C
    MOV     R1,#60             ;�ȴ�ʱ��Ƭ����
    CALL    DelayXus
    SETB    DQ                 ;׼���ͳ���һλ����
    MOV     R1,#1
    CALL    DelayXus
    DJNZ    0,WriteNext
    POP     1
    POP     0
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

    END

