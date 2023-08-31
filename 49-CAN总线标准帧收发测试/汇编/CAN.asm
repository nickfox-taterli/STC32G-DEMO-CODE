;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- STC 1T Series MCU Demo Programme -------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
;/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/* --- BBS: www.STCAIMCU.com ------------------------------------------*/
;/* --- QQ:  800003751 -------------------------------------------------*/
;/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
;/*---------------------------------------------------------------------*/


;*************  ����˵��    **************

;�����̻���STC32GΪ����оƬ��ʵ������б�д���ԡ�

;ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

;edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

;CAN�����շ���������.

;DCAN��һ��֧��CAN2.0BЭ��Ĺ��ܵ�Ԫ��

;ÿ���ӷ���һ֡��׼CAN��������.

;�յ�һ����׼֡��, �滻ԭ�ȵ� CAN ID �뷢�͵�����.

;Ĭ�ϲ�����500KHz, �û��������޸�.

;����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

;******************************************/

$include (../../comm/STC32G.INC)

;/****************************** �û������ ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;��ջ��ʼ��ַ

Timer0_Reload   EQU     (65536 - Fosc_KHZ)  ; Timer 0 �ж�Ƶ��, 1000��/��

;CAN���߲�����=Fclk/((1+(TSG1+1)+(TSG2+1))*(BRP+1)*2)
TSG1   EQU    2		;0~15
TSG2   EQU    1		;1~7 (TSG2 ��������Ϊ0)
BRP    EQU    3		;0~63
;24000000/((1+3+2)*4*2)=500KHz

SJW    EQU    00H	;����ͬ����Ծ��ȣ� 00H/040H/080H/0C0H

;���߲�����100KHz��������Ϊ 0; 100KHz��������Ϊ 1
SAM    EQU    00H	;���ߵ�ƽ���������� 00H:����1��; 080H:����3��

;*******************************************************************
;*******************************************************************


;*************  IO�ڶ���    **************/


;*************  ���ر�������    **************/
Flag0           DATA    20H
B_1ms           BIT     Flag0.0 ; 1ms��־
B_CanRead       BIT     Flag0.1 ; CAN�յ����ݱ�־

msecond         DATA    21H     ;
CAN_ID          DATA    23H     ;

RX_BUF          DATA    30H
TX_BUF          DATA    38H

TMP_BUF         DATA    40H


;*******************************************************************
;*******************************************************************
        ORG     0000H               ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
        LJMP    F_Main


        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt

        ORG     00E3H               ;28 CAN interrupt
        LJMP    F_CAN_Interrupt


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

    CLR     TR0
    ORL     AUXR, #(1 SHL 7)    ; Timer0_1T();
    ANL     TMOD, #NOT 04H      ; Timer0_AsTimer();
    ANL     TMOD, #NOT 03H      ; Timer0_16bitAutoReload();
    MOV     TH0, #Timer0_Reload / 256   ;Timer0_Load(Timer0_Reload);
    MOV     TL0, #Timer0_Reload MOD 256
    SETB    ET0         ; Timer0_InterruptEnable();
    SETB    TR0         ; Timer0_Run();

    LCALL   F_CAN_Init  ; CAN ��������ʼ��
    SETB    EA          ; �����ж�

;=====================================================

    MOV     WR4, #0345H
    MOV     CAN_ID, WR4     ;����Ĭ�� CAN ID
    MOV     TX_BUF+0, #11H  ;����Ĭ�Ϸ��͵�CAN����
    MOV     TX_BUF+1, #22H
    MOV     TX_BUF+2, #33H
    MOV     TX_BUF+3, #44H
    MOV     TX_BUF+4, #55H
    MOV     TX_BUF+5, #66H
    MOV     TX_BUF+6, #77H
    MOV     TX_BUF+7, #88H

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

    MOV     A, #SR
    LCALL   F_CAN_ReadReg
    JB      ACC.0,L_CAN_BUSOFF
    LCALL   F_CAN_SendMsg
    LJMP    L_Quit_Check_1000ms
L_CAN_BUSOFF:
    MOV     A, #MR
    MOV     WR6, #WORD0 CANAR
    MOV     WR4, #WORD2 CANAR
    MOV     @DR4, R11

    MOV     WR6, #WORD0 CANDR
    MOV     WR4, #WORD2 CANDR
    MOV     R11, @DR4
    ANL     A,#NOT 04H  ;��� Reset Mode, ��BUS-OFF״̬�˳�
    MOV     @DR4, R11

L_Quit_Check_1000ms:

    JNB     B_CanRead,  L_Main_Loop     ;δ���յ�CAN��������
    CLR     B_CanRead
    LCALL   F_CAN_ReadMsg     ;����CAN�������ݣ��滻ԭ�ȵ�CAN_ID�뷢�͵�����
;=====================================================
    LJMP    L_Main_Loop

;**********************************************/

;========================================================================
; ����: F_CAN_ReadReg
; ����: CAN���ܼĴ�����ȡ����.
; ����: A: Addr.
; ����: A: Data.
; �汾: V1.0, 2022-04-06
;========================================================================
F_CAN_ReadReg:
    MOV     WR6, #WORD0 CANAR
    MOV     WR4, #WORD2 CANAR
    MOV     @DR4, R11

    MOV     WR6, #WORD0 CANDR
    MOV     WR4, #WORD2 CANDR
    MOV     R11,@DR4
    RET

;========================================================================
; ����: F_CAN_WriteReg
; ����: CAN���ܼĴ������ú���.
; ����: A: Addr, B: Data.
; ����: none.
; �汾: V1.0, 2022-04-06
;========================================================================
F_CAN_WriteReg:
    MOV     WR6, #WORD0 CANAR
    MOV     WR4, #WORD2 CANAR
    MOV     @DR4, R11

    MOV     WR6, #WORD0 CANDR
    MOV     WR4, #WORD2 CANDR
    MOV     @DR4, R10
    RET

;========================================================================
; ����: F_CAN_ReadMsg
; ����: CAN�������ݺ���.
; ����: none.
; ����: none.
; �汾: V1.0, 2022-04-06
;========================================================================
F_CAN_ReadMsg:
    MOV     A, #RX_BUF0
    CALL    F_CAN_ReadReg
    MOV     TMP_BUF+0, A

    MOV     A, #RX_BUF1
    CALL    F_CAN_ReadReg
    MOV     TMP_BUF+1, A

    MOV     A, #RX_BUF2
    CALL    F_CAN_ReadReg
    MOV     TMP_BUF+2, A

    MOV     A, #RX_BUF3
    CALL    F_CAN_ReadReg
    MOV     TX_BUF+0, A   ;RX_BUF+0

    MOV     A, #RX_BUF0
    CALL    F_CAN_ReadReg
    MOV     TX_BUF+1, A   ;RX_BUF+1

    MOV     A, #RX_BUF1
    CALL    F_CAN_ReadReg
    MOV     TX_BUF+2, A   ;RX_BUF+2

    MOV     A, #RX_BUF2
    CALL    F_CAN_ReadReg
    MOV     TX_BUF+3, A   ;RX_BUF+3

    MOV     A, #RX_BUF3
    CALL    F_CAN_ReadReg
    MOV     TX_BUF+4, A   ;RX_BUF+4

    MOV     A, #RX_BUF0
    CALL    F_CAN_ReadReg
    MOV     TX_BUF+5, A   ;RX_BUF+5

    MOV     A, #RX_BUF1
    CALL    F_CAN_ReadReg
    MOV     TX_BUF+6, A   ;RX_BUF+6

    MOV     A, #RX_BUF2
    CALL    F_CAN_ReadReg
    MOV     TX_BUF+7, A   ;RX_BUF+7

    MOV     A, #RX_BUF3
    CALL    F_CAN_ReadReg

    MOV     A, #RX_BUF0
    CALL    F_CAN_ReadReg

    MOV     A, #RX_BUF1
    CALL    F_CAN_ReadReg

    MOV     A, #RX_BUF2
    CALL    F_CAN_ReadReg

    MOV     A, #RX_BUF3
    CALL    F_CAN_ReadReg

;	CanID = ((buffer[1] << 8) + buffer[2]) >> 5;
    MOV     R6, TMP_BUF+1
    MOV     R7, TMP_BUF+2
	SRL     WR6
	SRL     WR6
	SRL     WR6
	SRL     WR6
	SRL     WR6
    ANL     WR6, #07FFH
    MOV     CAN_ID, WR6  ;���� CAN ID

    RET

;========================================================================
; ����: F_CAN_SendMsg
; ����: CAN�������ݺ���.
; ����: none.
; ����: none.
; �汾: V1.0, 2022-04-06
;========================================================================
F_CAN_SendMsg:
    MOV     A, #TX_BUF0
    MOV     B, #08H        ;bit7: ��׼֡(0)/��չ֡(1), bit6: ����֡(0)/Զ��֡(1), bit3~bit0: ���ݳ���(DLC)
    CALL    F_CAN_WriteReg

    MOV     WR2, CAN_ID
	SLL     WR2
	SLL     WR2
	SLL     WR2
	SLL     WR2
	SLL     WR2
    MOV     A, #TX_BUF1
    MOV     B, R2
    CALL    F_CAN_WriteReg
    MOV     A, #TX_BUF2
    MOV     B, R3
    CALL    F_CAN_WriteReg
    MOV     A, #TX_BUF3
    MOV     B, TX_BUF+0
    CALL    F_CAN_WriteReg

    MOV     A, #TX_BUF0
    MOV     B, TX_BUF+1
    CALL    F_CAN_WriteReg
    MOV     A, #TX_BUF1
    MOV     B, TX_BUF+2
    CALL    F_CAN_WriteReg
    MOV     A, #TX_BUF2
    MOV     B, TX_BUF+3
    CALL    F_CAN_WriteReg
    MOV     A, #TX_BUF3
    MOV     B, TX_BUF+4
    CALL    F_CAN_WriteReg

    MOV     A, #TX_BUF0
    MOV     B, TX_BUF+5
    CALL    F_CAN_WriteReg
    MOV     A, #TX_BUF1
    MOV     B, TX_BUF+6
    CALL    F_CAN_WriteReg
    MOV     A, #TX_BUF2
    MOV     B, TX_BUF+7
    CALL    F_CAN_WriteReg
    MOV     A, #TX_BUF3
    MOV     B, #0
    CALL    F_CAN_WriteReg

    MOV     A, #CMR
    MOV     B, #04H        ;����һ��֡����
    CALL    F_CAN_WriteReg
    RET

;========================================================================
; ����: F_CAN_Init
; ����: CAN��ʼ������.
; ����: none
; ����: none.
; �汾: V1.0, 2022-04-06
;========================================================================
F_CAN_Init:
    ORL     AUXR2, #02H       ;CAN1ģ��ʹ��
    ANL     AUXR2, #NOT 08H   ;ѡ��CAN1ģ��
    MOV     P_SW1, #0         ;�˿��л�(CAN_Rx,CAN_Tx) 0x00:P0.0,P0.1  0x10:P5.0,P5.1  0x20:P4.2,P4.5  0x30:P7.0,P7.1

    MOV     A, #MR
    MOV     B, #04H           ;ʹ�� Reset Mode
    CALL    F_CAN_WriteReg

    ;���ò�����
    MOV     A, #SJW
	ADD     A, #BRP
    MOV     B, A
    MOV     A, #BTR0
    CALL    F_CAN_WriteReg

    MOV     A, #TSG2
    SWAP    A
	ADD     A, #TSG1
	ADD     A, #SAM
    MOV     B, A
    MOV     A, #BTR1
    CALL    F_CAN_WriteReg

    ;�������մ���Ĵ���
    MOV     A, #ACR0
    MOV     B, #00H
    CALL    F_CAN_WriteReg
    MOV     A, #ACR1
    MOV     B, #00H
    CALL    F_CAN_WriteReg
    MOV     A, #ACR2
    MOV     B, #00H
    CALL    F_CAN_WriteReg
    MOV     A, #ACR3
    MOV     B, #00H
    CALL    F_CAN_WriteReg
    ;�����������μĴ���
    MOV     A, #AMR0
    MOV     B, #0FFH
    CALL    F_CAN_WriteReg
    MOV     A, #AMR1
    MOV     B, #0FFH
    CALL    F_CAN_WriteReg
    MOV     A, #AMR2
    MOV     B, #0FFH
    CALL    F_CAN_WriteReg
    MOV     A, #AMR3
    MOV     B, #0FFH
    CALL    F_CAN_WriteReg

    MOV     A, #IMR
    MOV     B, #0FFH       ;�жϼĴ���
    CALL    F_CAN_WriteReg
    MOV     A, #ISR
    MOV     B, #0FFH       ;���жϱ�־
    CALL    F_CAN_WriteReg
    MOV     A, #MR
    MOV     B, #00H        ;�˳� Reset Mode
    CALL    F_CAN_WriteReg

    MOV     CANICR, #02H   ;CAN�ж�ʹ��
    RET


;**************** �жϺ��� ***********************************************

F_Timer0_Interrupt: ;Timer0 1ms�жϺ���
    PUSH    PSW     ;PSW��ջ
    PUSH    ACC     ;ACC��ջ

    SETB    B_1ms           ; 1ms��־

    POP     ACC     ;ACC��ջ
    POP     PSW     ;PSW��ջ
    RETI

;========================================================================
F_CAN_Interrupt:    ;CAN�жϺ���
    PUSH    PSW     ;PSW��ջ
    PUSH    ACC     ;ACC��ջ
    PUSH    R4      ;R4��ջ
    PUSH    R5      ;R5��ջ
    PUSH    R6      ;R6��ջ
    PUSH    R7      ;R7��ջ
    PUSH    B       ;B��ջ

    MOV     WR6, #WORD0 CANAR   ;CANAR�ֳ����棬������ѭ����д�� CANAR ������жϣ����ж����޸��� CANAR ����
    MOV     WR4, #WORD2 CANAR
    MOV     R10,@DR4            ;���� B �Ĵ���

    MOV     A, #ISR
;    MOV     WR6, #WORD0 CANAR
;    MOV     WR4, #WORD2 CANAR
    MOV     @DR4, R11

    MOV     WR6, #WORD0 CANDR
    MOV     WR4, #WORD2 CANDR
    MOV     R11,@DR4
    MOV     @DR4, R11  ;���־λ��д 1 ���

    JB      ACC.0,CAN_DOIIF
ISRTEST1:
    JB      ACC.1,CAN_BEIIF
ISRTEST2:
    JB      ACC.2,CAN_TIIF
ISRTEST3:
    JB      ACC.3,CAN_RIIF
ISRTEST4:
    JB      ACC.4,CAN_EPIIF
ISRTEST5:
    JB      ACC.5,CAN_EWIIF
ISRTEST6:
    JB      ACC.6,CAN_ALIIF

ISREXIT:
    MOV     WR6, #WORD0 CANAR   ;CANAR�ֳ��ָ�
    MOV     WR4, #WORD2 CANAR
    MOV     @DR4, R10

    POP     B       ;B��ջ
    POP     R7      ;R7��ջ
    POP     R6      ;R6��ջ
    POP     R5      ;R5��ջ
    POP     R4      ;R4��ջ
    POP     ACC     ;ACC��ջ
    POP     PSW     ;PSW��ջ
    RETI

CAN_DOIIF:
;    ORL     A,#01H  ;���־λ��д 1 ���
;    MOV     @DR4, R11
    JMP     ISRTEST1

CAN_BEIIF:
;    ORL     A,#02H  ;���־λ��д 1 ���
;    MOV     @DR4, R11
    JMP     ISRTEST2

CAN_TIIF:
;    ORL     A,#04H  ;���־λ��д 1 ���
;    MOV     @DR4, R11
    JMP     ISRTEST3

CAN_RIIF:
;    ORL     A,#08H  ;���־λ��д 1 ���
;    MOV     @DR4, R11
    SETB    B_CanRead
    JMP     ISRTEST4

CAN_EPIIF:
;    ORL     A,#010H ;���־λ��д 1 ���
;    MOV     @DR4, R11
    JMP     ISRTEST5

CAN_EWIIF:
;    ORL     A,#020H ;���־λ��д 1 ���
;    MOV     @DR4, R11
    JMP     ISRTEST6

CAN_ALIIF:
;    ORL     A,#040H ;���־λ��д 1 ���
;    MOV     @DR4, R11
    JMP     ISREXIT

;========================================================================
    END

