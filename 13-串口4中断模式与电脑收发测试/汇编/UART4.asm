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

;/************* ����˵��    **************

;�����̻���STC32GΪ����оƬ��ʵ������б�д���ԡ�

;ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

;edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

;����4ȫ˫���жϷ�ʽ�շ�ͨѶ����.

;��������Ϊ��115200,8,n,1.

;ͨ��PC��MCU��������, MCU�յ���ͨ�����ڰ��յ�������ԭ������.

;�ö�ʱ���������ʷ�����������ʹ��1Tģʽ(���ǵͲ�������12T)����ѡ��ɱ�������������ʱ��Ƶ�ʣ�����߾��ȡ�

;����ʱ, ѡ��ʱ�� 22.1184MHz����Ҫ�ı�, ���޸�����궨���ֵ�����±���.

;******************************************/

$include (../../comm/STC32G.INC)

;/****************************** �û������ ***********************************/

;UART4_Baudrate EQU     (-4608) ;   600bps @ 11.0592MHz
;UART4_Baudrate EQU     (-2304) ;  1200bps @ 11.0592MHz UART4_Baudrate = 65536UL - ((MAIN_Fosc / 4) / Baudrate)
;UART4_Baudrate EQU     (-1152) ;  2400bps @ 11.0592MHz
;UART4_Baudrate EQU     (-576)  ;  4800bps @ 11.0592MHz
;UART4_Baudrate EQU     (-288)  ;  9600bps @ 11.0592MHz
;UART4_Baudrate EQU     (-144)  ; 19200bps @ 11.0592MHz
;UART4_Baudrate EQU     (-72)   ; 38400bps @ 11.0592MHz
;UART4_Baudrate EQU     (-48)   ; 57600bps @ 11.0592MHz
;UART4_Baudrate EQU      (-24)   ;115200bps @ 11.0592MHz

;UART4_Baudrate EQU     (-7680) ;   600bps @ 18.432MHz
;UART4_Baudrate EQU     (-3840) ;  1200bps @ 18.432MHz
;UART4_Baudrate EQU     (-1920) ;  2400bps @ 18.432MHz
;UART4_Baudrate EQU     (-960)  ;  4800bps @ 18.432MHz
;UART4_Baudrate EQU     (-480)  ;  9600bps @ 18.432MHz
;UART4_Baudrate EQU     (-240)  ; 19200bps @ 18.432MHz
;UART4_Baudrate EQU     (-120)  ; 38400bps @ 18.432MHz
;UART4_Baudrate EQU     (-80)   ; 57600bps @ 18.432MHz
;UART4_Baudrate EQU     (-40)   ;115200bps @ 18.432MHz

;UART4_Baudrate EQU     (-9216) ;   600bps @ 22.1184MHz
;UART4_Baudrate EQU     (-4608) ;  1200bps @ 22.1184MHz
;UART4_Baudrate EQU     (-2304) ;  2400bps @ 22.1184MHz
;UART4_Baudrate EQU     (-1152) ;  4800bps @ 22.1184MHz
;UART4_Baudrate EQU     (-576)  ;  9600bps @ 22.1184MHz
;UART4_Baudrate EQU     (-288)  ; 19200bps @ 22.1184MHz
;UART4_Baudrate EQU     (-144)  ; 38400bps @ 22.1184MHz
;UART4_Baudrate EQU     (-96)   ; 57600bps @ 22.1184MHz
UART4_Baudrate  EQU    (-48)   ;115200bps @ 22.1184MHz

;UART4_Baudrate EQU     (-6912) ; 1200bps @ 33.1776MHz
;UART4_Baudrate EQU     (-3456) ; 2400bps @ 33.1776MHz
;UART4_Baudrate EQU     (-1728) ; 4800bps @ 33.1776MHz
;UART4_Baudrate EQU     (-864)  ; 9600bps @ 33.1776MHz
;UART4_Baudrate EQU     (-432)  ;19200bps @ 33.1776MHz
;UART4_Baudrate EQU     (-216)  ;38400bps @ 33.1776MHz
;UART4_Baudrate EQU     (-144)  ;57600bps @ 33.1776MHz
;UART4_Baudrate EQU     (-72)   ;115200bps @ 33.1776MHz


;*******************************************************************
;*******************************************************************

RX4_Lenth   EQU     32      ; ���ڽ��ջ��峤��

B_TX4_Busy  BIT     20H.0   ; ����æ��־
TX4_Cnt     DATA    30H     ; ���ͼ���
RX4_Cnt     DATA    31H     ; ���ռ���
RX4_Buffer  DATA    40H     ;40 ~ 5FH ���ջ���

STACK_POIRTER   EQU     0D0H    ;��ջ��ʼ��ַ

;*******************************************************************
;*******************************************************************
        ORG     0000H       ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
        LJMP    F_Main

        ORG     0093H       ;18  UART4 interrupt
        LJMP    F_UART4_Interrupt


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
    MOV     A, #1
    LCALL   F_UART4_config  ; ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
    SETB    EA      ; ����ȫ���ж�
    
    MOV     DPTR, #TestString4  ;Load string address to DPTR
    LCALL   F_SendString4       ;Send string

;=================== ��ѭ�� ==================================
L_MainLoop:
    MOV     A, TX4_Cnt
    CJNE    A, RX4_Cnt, L_ReturnData
    SJMP    L_QuitCheckRx4
L_ReturnData:                   ; �յ�������
    JB      B_TX4_Busy, L_QuitCheckRx4      ; ����æ�� �˳�
    SETB    B_TX4_Busy          ; ��־����æ
    MOV     A, #RX4_Buffer
    ADD     A, TX4_Cnt
    MOV     R0, A
    MOV     S4BUF, @R0       ; ��һ���ֽ�
    INC     TX4_Cnt
    MOV     A, TX4_Cnt
    CJNE    A, #RX4_Lenth, L_QuitCheckRx4
    MOV     TX4_Cnt, #0     ; �����������
L_QuitCheckRx4:
    LJMP    L_MainLoop
;===================================================================


TestString4:                    ;Test string
    DB  "STC32G UART4 Test Programme!",0DH,0AH,0

;========================================================================
; ����: F_SendString4
; ����: ����4�����ַ���������
; ����: DPTR: �ַ����׵�ַ.
; ����: none.
; �汾: VER1.0
; ����: 2020-11-05
; ��ע: 
;========================================================================
F_SendString4:
    CLR     A
    MOVC    A, @A+DPTR      ;Get current char
    JZ      L_SendEnd4      ;Check the end of the string
    SETB    B_TX4_Busy      ;��־����æ
    MOV     S4BUF, A         ;����һ���ֽ�
    JB      B_TX4_Busy, $   ;�ȴ��������
    INC     DPTR            ;increment string ptr
    SJMP    F_SendString4   ;Check next
L_SendEnd4:
    RET

;========================================================================
; ����: F_SetTimer2Baudraye
; ����: ����Timer2�������ʷ�������
; ����: DPTR: Timer2����װֵ.
; ����: none.
; �汾: VER1.0
; ����: 2014-11-28
; ��ע: 
;========================================================================
F_SetTimer2Baudraye:    ; ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
    ANL     AUXR, #NOT (1 SHL 4)    ; Timer stop    ������ʹ��Timer2����
    ANL     AUXR, #NOT (1 SHL 3)    ; Timer2 set As Timer
    ORL     AUXR, #(1 SHL 2)        ; Timer2 set as 1T mode
    MOV     T2H, DPH
    MOV     T2L, DPL
    ANL     IE2, #NOT (1 SHL 2)     ; ��ֹ�ж�
    ORL     AUXR, #(1 SHL 4)        ; Timer run enable
    RET

;========================================================================
; ����: F_UART4_config
; ����: UART4��ʼ��������
; ����: ACC: ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer4��������.
; ����: none.
; �汾: VER1.0
; ����: 2020-11-05
; ��ע: 
;========================================================================
F_UART4_config:
    CJNE    A, #2, L_Uart1NotUseTimer2
    ORL     S4CON, #0x10     ; S4 BRT Use Timer2;
    MOV     DPTR, #UART4_Baudrate
    LCALL   F_SetTimer2Baudraye
    SJMP    L_SetupUart1

L_Uart1NotUseTimer2:
    MOV     S4CON, #0x50     ; 8λ����, ʹ��Timer4�������ʷ�����, �������
    MOV     T4H, #HIGH UART4_Baudrate
    MOV     T4L, #LOW  UART4_Baudrate
    MOV     T4T3M, #0xa0

L_SetupUart1:
    ANL     P_SW2, #NOT 0x04  ; UART4 switch to P0.2 P0.3
;    ORL     P_SW2, #0x04     ; UART4 switch to P5.2 P5.3
    ORL     IE2, #0x10        ; ����UART4�ж�

    CLR     B_TX4_Busy
    MOV     RX4_Cnt, #0;
    MOV     TX4_Cnt, #0;
    RET


;========================================================================
; ����: F_UART4_Interrupt
; ����: UART4�жϺ�����
; ����: nine.
; ����: none.
; �汾: VER1.0
; ����: 2020-11-05
; ��ע: 
;========================================================================
F_UART4_Interrupt:
    PUSH    PSW
    PUSH    ACC
    PUSH    AR0
    
    MOV     A, S4CON
    JNB     ACC.0, L_QuitUartRx
    ANL     S4CON, #NOT 1;
    MOV     A, #RX4_Buffer
    ADD     A, RX4_Cnt
    MOV     R0, A
    MOV     @R0, S4BUF   ;����һ���ֽ�
    INC     RX4_Cnt
    MOV     A, RX4_Cnt
    CJNE    A, #RX4_Lenth, L_QuitUartRx
    MOV     RX4_Cnt, #0     ; �����������
L_QuitUartRx:

    MOV     A, S4CON
    JNB     ACC.1, L_QuitUartTx
    ANL     S4CON, #NOT 2;
    CLR     B_TX4_Busy      ; �������æ��־
L_QuitUartTx:

    POP     AR0
    POP     ACC
    POP     PSW
    RETI

    END

