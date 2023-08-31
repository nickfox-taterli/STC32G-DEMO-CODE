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

;�ڲ�����I2C�������߿��������ӻ�ģʽ��SCL->P3.2, SDA->P3.3;
;IO��ģ��I2C������ģʽ��SCL->P0.0, SDA->P0.1;
;ͨ���ⲿ�������� P0.0->P3.2, P0.1->P3.3��ʵ��I2C�Է����չ��ܡ�

;��STC��MCU��IO��ʽ����8λ����ܡ�
;ʹ��Timer0��16λ�Զ���װ������1ms����,�������������������,�û��޸�MCU��ʱ��Ƶ��ʱ,�Զ���ʱ��1ms.
;������ÿ���Ӽ�1, ������ΧΪ0~9999.

;��ʾЧ��Ϊ: �ϵ������ÿ���ӷ���һ�μ������ݣ��������4�����������ʾ�������ݣ��ӻ����յ����ݺ����ұ�4���������ʾ��

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

SLAW            EQU     05AH
SLAR            EQU     05BH

SDA             BIT     P0.1
SCL             BIT     P0.0
;*******************************************************************
;*******************************************************************

;*************  IO�ڶ���    **************/


;*************  ���ر�������    **************/
B_1ms           BIT     20H.0   ; 1ms��־
ISDA            BIT     20H.6
ISMA            BIT     20H.7

ADDR            DATA    21H

LED8            DATA    30H     ; ��ʾ���� 30H ~ 37H
display_index   DATA    38H     ; ��ʾλ����

msecond         DATA    39H     ; 2 byte
Test_cnt        DATA    3BH     ;


;*******************************************************************
;*******************************************************************
        ORG     0000H               ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
        LJMP    F_Main

        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt

        ORG     00C3H               ;24  I2C interrupt
        LJMP    F_I2C_Interrupt


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
    MOV     P3M1, #54H     ;����P3.4��P3.6Ϊ©����·(ʵ��������������赽3.3V), SCL����Ϊ����ڣ�SDA����Ϊ׼˫���
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
    MOV     @R0, #DIS_          ;�ϵ�ȫ����ʾ-
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
    SETB    EA          ; �����ж�

;=================== ��ѭ�� ==================================
L_Main_Loop:
    JNB     B_1ms, L_Main_Loop     ;1msδ��
    CLR     B_1ms
    
;=================== ���1s�Ƿ� ==================================
    MOV     WR6, msecond
    INC     WR6, #1         ;msecond + 1
    MOV     msecond, WR6
    CMP     WR6, #1000
    JC      L_Main_Loop     ;if(msecond < 1000), jmp
    MOV     WR6, #0
    MOV     msecond, WR6    ;msecond = 0

;=================== ����Ƿ�>= 10000 ==============================
    MOV     WR6, Test_cnt
    INC     WR6, #1         ;Test_cnt + 1
    MOV     Test_cnt, WR6
    CMP     WR6, #10000
    JC      L_LessThen10000  ;if(Test_cnt < 10000), jmp
    MOV     WR6, #0
    MOV     Test_cnt, WR6    ;Test_cnt = 0
L_LessThen10000:
    MOV     WR4, #00000H    ;������
    ;MOV     WR6, WR6
    MOV     WR0, #00000H    ;����
    MOV     WR2, #1000
    MOV     DMAIR, #04H     ;32λ�޷��ų���
    MOV     A, R7           ;��ʾ��ֵ
    ANL     A, #0x0F
    MOV     LED8+7, A

    MOV     WR4, WR0        ;������������
    MOV     WR6, WR2
    MOV     WR0, #00000H
    MOV     WR2, #100
    MOV     DMAIR, #04H     ;32λ�޷��ų���
    MOV     A, R7           ;��ʾ��ֵ
    ANL     A, #0x0F
    MOV     LED8+6, A

    MOV     WR4, WR0        ;������������
    MOV     WR6, WR2
    MOV     WR0, #00000H
    MOV     WR2, #10
    MOV     DMAIR, #04H     ;32λ�޷��ų���
    MOV     A, R7           ;��ʾ��ֵ
    ANL     A, #0x0F
    MOV     LED8+5, A

    MOV     A, R3           ;��ʾ��ֵ
    ANL     A, #0x0F
    MOV     LED8+4, A

    MOV     A, LED8+7           ;��ʾ����Ч0
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
; ����: F_I2C_Init
; ����: I2C��ʼ������.
; ����: none
; ����: none.
; �汾: V1.0, 2021-3-4
;========================================================================
F_I2C_Init:
    ORL     P_SW2, #30H         ;I2C���ܽ�ѡ��00H:P1.5,P1.4; 10H:P2.5,P2.4; 30H:P3.2,P3.3

    MOV     A, #080H            ;ʹ��I2C�ӻ�ģʽ
    MOV     WR6, #WORD0 I2CCFG
    MOV     WR4, #WORD2 I2CCFG
    MOV     @DR4, R11

    MOV     A, #05AH            ;���ôӻ��豸��ַΪ5A
    MOV     WR6, #WORD0 I2CSLADR
    MOV     WR4, #WORD2 I2CSLADR
    MOV     @DR4, R11

    MOV     A, #00H
    MOV     WR6, #WORD0 I2CSLST
    MOV     WR4, #WORD2 I2CSLST
    MOV     @DR4, R11

    MOV     A, #078H            ;ʹ�ܴӻ�ģʽ�ж�
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
	MOV     R11,@DR4	  ;���ӻ�״̬
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
    ANL     A,#NOT 040H  ;���� START �¼�
	MOV     @DR4,R11
    JMP     F_I2C_EXIT
RXIF:
    ANL     A,#NOT 020H  ;���� RECV �¼�
	MOV     @DR4,R11
	MOV     WR6,#WORD0 I2CRXD
	MOV     WR4,#WORD2 I2CRXD
	MOV     R11,@DR4
    JBC     ISDA,RXDA
    JBC     ISMA,RXMA
    MOV     R0,ADDR     ;���� RECV �¼���RECV DATA��
    MOV     @R0,A
    INC     ADDR
    JMP     F_I2C_EXIT
RXDA:
    JMP     F_I2C_EXIT  ;���� RECV �¼���RECV DEVICE ADDR��
RXMA:
    ADD     A,#030H     ;LED8��ַ
    MOV     ADDR,A      ;���� RECV �¼���RECV MEMORY ADDR��
    MOV     R0,A
    MOV     A,@R0
	MOV     WR6,#WORD0 I2CTXD
	MOV     WR4,#WORD2 I2CTXD
	MOV     @DR4,R11
    JMP     F_I2C_EXIT
TXIF:
    ANL     A,#NOT 010H  ;���� SEND �¼�
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
    ANL     A,#NOT 08H  ;���� STOP �¼�
	MOV     @DR4,R11
    SETB    ISDA
    SETB    ISMA
    JMP     F_I2C_EXIT

;========================================================================
; ���ģ��I2C����
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
    RLC     A          ;�Ƴ�����λ
    MOV     SDA, C     ;���������ݿ�
    SETB    SCL        ;ʱ��->��
    LCALL   I2C_Delay  ;��ʱ
    CLR     SCL        ;ʱ��->��
    LCALL   I2C_Delay  ;��ʱ
    DJNZ    R7, TXNEXT ;����һλ
    RET

I2C_ReadAbyte:
    MOV     R7, #08H
RXNEXT:
    SETB    SCL        ;ʱ��->��
    LCALL   I2C_Delay  ;��ʱ
    MOV     C, SDA
    RLC     A
    CLR     SCL        ;ʱ��->��
    LCALL   I2C_Delay  ;��ʱ
    DJNZ    R7, RXNEXT ;����һλ
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
    MOV     A, #LED8+4  ;���� LED8[4]~LED8[7] ����
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

