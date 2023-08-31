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

;��STC��MCU��IO��ʽ����8λ����ܡ�

;ʹ��Timer0��16λ�Զ���װ������1ms����,�������������������, �û��޸�MCU��ʱ��Ƶ��ʱ,�Զ���ʱ��1ms.

;ͨ��Ӳ��I2C�ӿڶ�ȡAT24C02ǰ8���ֽ����ݣ������������ʾ.

;����ȡ�����ݼ�1��д��AT24C02ǰ8���ֽ�.

;���¶�ȡAT24C02ǰ8���ֽ����ݣ������������ʾ.

;MCU�ϵ��ִ��1�����϶��������ظ��ϵ�/�ϵ����AT24C02ǰ8���ֽڵ���������.

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

SLAW    EQU     0xA2
SLAR    EQU     0xA3

;*******************************************************************
;*************  IO�ڶ���    **************/


;*************  ���ر�������    **************/
Flag0           DATA    20H
B_1ms           BIT     Flag0.0 ; 1ms��־

display_index   DATA    21H     ; ��ʾλ����
LED8            DATA    30H     ; ��ʾ���� 30H ~ 37H
EEPROM           DATA   38H     ; �洢���� 38H ~ 3FH


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

    LCALL   F_I2C_Init
    SETB    EA          ; �����ж�

    LCALL   F_ReadEEP   ;��24Cxx
    LCALL   F_WriteEEP  ;д24Cxx
    MOV     A, #250
	LCALL   F_delay_ms
    MOV     A, #250
	LCALL   F_delay_ms
    LCALL   F_ReadEEP   ;��24Cxx

;=================== ��ѭ�� ==================================
L_Main_Loop:

    LJMP    L_Main_Loop

;**********************************************/

;========================================================================
; ����: F_I2C_Init
; ����: I2C��ʼ������.
; ����: none
; ����: none.
; �汾: V1.0, 2021-3-4
;========================================================================
F_I2C_Init:
    ORL     P_SW2, #010H        ;I2C���ܽ�ѡ��00H:P1.5,P1.4; 10H:P2.5,P2.4; 30H:P3.2,P3.3

    MOV     A, #0E0H            ;ʹ��I2C����ģʽ
    MOV     WR6, #WORD0 I2CCFG
    MOV     WR4, #WORD2 I2CCFG
    MOV     @DR4, R11

    MOV     A, #00H
    MOV     WR6, #WORD0 I2CMSST
    MOV     WR4, #WORD2 I2CMSST
    MOV     @DR4, R11
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


;*******************************************************************
;**************** �жϺ��� ***************************************************

F_Timer0_Interrupt: ;Timer0 1ms�жϺ���
    PUSH    PSW     ;PSW��ջ
    PUSH    ACC     ;ACC��ջ

    LCALL   F_DisplayScan   ; 1msɨ����ʾһλ
    SETB    B_1ms           ; 1ms��־

    POP     ACC     ;ACC��ջ
    POP     PSW     ;PSW��ջ
    RETI
    

;************ I2C��غ��� ****************
;========================================================================
; ����: Wait
; ����: I2C������ʱ.
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
;========================================================================
Wait:
    MOV     WR6, #WORD0 I2CMSST
    MOV     WR4, #WORD2 I2CMSST
    MOV     R11, @DR4
    JNB     ACC.6,Wait
    ANL     A,#NOT 40H
	MOV     @DR4,R11
    RET

;========================================================================
; ����: Start
; ����: ����I2C. 
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
;========================================================================
Start:
    MOV     A, #01H
    MOV     WR6, #WORD0 I2CMSCR  ;����START����
    MOV     WR4, #WORD2 I2CMSCR
    MOV     @DR4, R11
    LCALL   Wait
    RET


;========================================================================
; ����: Stop
; ����: ֹͣI2C. 
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
;========================================================================
Stop:
    MOV     A, #06H
    MOV     WR6, #WORD0 I2CMSCR  ;����STOP����
    MOV     WR4, #WORD2 I2CMSCR
    MOV     @DR4, R11
    LCALL   Wait
    RET

;========================================================================
; ����: SendData
; ����: ��������.
; ����: I2CTXD -> A.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
;========================================================================
SendData:
	MOV     WR6,#WORD0 I2CTXD   ;д���ݵ����ݻ�����
	MOV     WR4,#WORD2 I2CTXD
	MOV     @DR4,R11
	MOV     A,#00000010B        ;����SEND����
	MOV     WR6,#WORD0 I2CMSCR
	MOV     WR4,#WORD2 I2CMSCR
	MOV     @DR4,R11
    LCALL   Wait
    RET

;========================================================================
; ����: RecvACK
; ����: ���Ӧ��.
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
;========================================================================
RecvACK:
    MOV     A,#03H
	MOV     WR6,#WORD0 I2CMSCR  ;���Ͷ�ACK����
	MOV     WR4,#WORD2 I2CMSCR
	MOV     @DR4,R11
    LCALL   Wait
    CLR     C
	MOV     WR6,#WORD0 I2CMSST  ;���Ͷ�ACK����
	MOV     WR4,#WORD2 I2CMSST
	MOV     R11,@DR4
    JNB     ACC.1,$+4
    SETB    C
    RET
        
;========================================================================
; ����: RecvData
; ����: ��������.
; ����: none.
; ����: I2CRXD -> A.
; �汾: VER1.0
; ����: 2013-4-1
;========================================================================
RecvData:
    MOV     A,#04H
	MOV     WR6,#WORD0 I2CMSCR  ;����RECV����
	MOV     WR4,#WORD2 I2CMSCR
	MOV     @DR4,R11
    LCALL   Wait
	MOV     WR6,#WORD0 I2CRXD
	MOV     WR4,#WORD2 I2CRXD
	MOV     R11,@DR4	;�����ݻ�������ȡ����
    RET

;========================================================================
; ����: SendACK
; ����: ����Ӧ��.
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
;========================================================================
SendACK:
    MOV     A,#00H
	MOV     WR6,#WORD0 I2CMSST  ;����ACK�ź�
	MOV     WR4,#WORD2 I2CMSST
	MOV     @DR4,R11
    MOV     A,#05H
	MOV     WR6,#WORD0 I2CMSCR  ;����ACK����
	MOV     WR4,#WORD2 I2CMSCR
	MOV     @DR4,R11
    LCALL   Wait
    RET

;========================================================================
; ����: SendNAK
; ����: ���ͷ�Ӧ��.
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
;========================================================================
SendNAK:
    MOV     A,#01H
	MOV     WR6,#WORD0 I2CMSST  ;����NAK�ź�
	MOV     WR4,#WORD2 I2CMSST
	MOV     @DR4,R11
    MOV     A,#05H
	MOV     WR6,#WORD0 I2CMSCR  ;����ACK����
	MOV     WR4,#WORD2 I2CMSCR
	MOV     @DR4,R11
    LCALL   Wait
    RET

;========================================================================
; ����: F_WriteNbyte
; ����: дN���ֽ��ӳ���
; ����: R2: дI2C�����׵�ַ,  R0: д�����ݴ���׵�ַ,  R3: д���ֽ���
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
; ��ע: ����ACCC��PSW��, ���õ���ͨ�üĴ�������ջ
;========================================================================
F_WriteNbyte:
    LCALL   Start
    MOV     A, #SLAW
    LCALL   SendData
    LCALL   RecvACK
    JC      L_WriteN_StopI2C

    MOV     A, R2
    LCALL   SendData
    LCALL   RecvACK
    JC      L_WriteN_StopI2C

L_WriteNbyteLoop:
    MOV     A, @R0
    LCALL   SendData
    INC     R0
    LCALL   RecvACK
    JC      L_WriteN_StopI2C
    DJNZ    R3, L_WriteNbyteLoop 
L_WriteN_StopI2C:
    LCALL   Stop
    RET


;========================================================================
; ����: F_ReadNbyte
; ����: ��N���ֽ��ӳ���
; ����: R2: ��I2C�����׵�ַ,  R0: �������ݴ���׵�ַ,  R3: �����ֽ���
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
; ��ע: ����ACCC��PSW��, ���õ���ͨ�üĴ�������ջ
;========================================================================
F_ReadNbyte:
    LCALL   Start
    MOV     A, #SLAW
    LCALL   SendData
    LCALL   RecvACK
    JC      L_ReadN_StopI2C

    MOV     A, R2
    LCALL   SendData
    LCALL   RecvACK
    JC      L_ReadN_StopI2C

    LCALL   Start
    MOV     A, #SLAR
    LCALL   SendData
    LCALL   RecvACK
    JC      L_ReadN_StopI2C

    MOV     A, R3
    ANL     A, #0xfe    ;�ж��Ƿ����1
    JZ      L_ReadLastByte
    DEC     R3          ;����1�ֽ�, ��-1
L_ReadNbyteLoop:
    LCALL   RecvData    ;*p = I2C_ReadAbyte();  p++;
    MOV     @R0, A
    INC     R0
    LCALL   SendACK     ;send ACK
    DJNZ    R3, L_ReadNbyteLoop 
L_ReadLastByte:
    LCALL   RecvData    ;*p = I2C_ReadAbyte()
    MOV     @R0, A
    LCALL   SendNAK     ;send no ACK
L_ReadN_StopI2C:
    LCALL   Stop
    RET


;========================================================================
; ����: F_ReadEEP
; ����: ��24Cxx������
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
; ��ע: 
;========================================================================
F_ReadEEP:

    MOV     R2, #0      ; ��I2C�����׵�ַ
    MOV     R0, #EEPROM ; �������ݴ���׵�ַ
    MOV     R3, #8      ; �����ֽ���
    LCALL   F_ReadNbyte ; R2: ��I2C�����׵�ַ,  R0: �������ݴ���׵�ַ,  R3: �����ֽ���

    MOV     LED8+0, EEPROM+0
    MOV     LED8+1, EEPROM+1
    MOV     LED8+2, EEPROM+2
    MOV     LED8+3, EEPROM+3
    MOV     LED8+4, EEPROM+4
    MOV     LED8+5, EEPROM+5
    MOV     LED8+6, EEPROM+6
    MOV     LED8+7, EEPROM+7
    RET

;========================================================================
; ����: F_WriteEEP
; ����: д24Cxx������
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
; ��ע: 
;========================================================================
F_WriteEEP:
    INC     EEPROM+0
    INC     EEPROM+1
    INC     EEPROM+2
    INC     EEPROM+3
    INC     EEPROM+4
    INC     EEPROM+5
    INC     EEPROM+6
    INC     EEPROM+7

    ANL     EEPROM+0, #0FH
    ANL     EEPROM+1, #0FH
    ANL     EEPROM+2, #0FH
    ANL     EEPROM+3, #0FH
    ANL     EEPROM+4, #0FH
    ANL     EEPROM+5, #0FH
    ANL     EEPROM+6, #0FH
    ANL     EEPROM+7, #0FH

    MOV     R2, #0      ;дI2C�����׵�ַ
    MOV     R0, #EEPROM ;д�����ݴ���׵�ַ
    MOV     R3, #8      ;д���ֽ���
    LCALL   F_WriteNbyte    ;
    RET

;========================================================================
; ����: F_delay_ms
; ����: ��ʱ�ӳ���
; ����: ACC: ��ʱms��.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
; ��ע: ����ACCC��PSW��, ���õ���ͨ�üĴ�������ջ
;========================================================================
F_delay_ms:
    PUSH    02H     ;��ջR2
    PUSH    03H     ;��ջR3
    PUSH    04H     ;��ջR4

    MOV     R4,A

L_delay_ms_1:
    MOV     WR2, #(Fosc_KHZ / 4)
    
L_delay_ms_2:
    DEC     WR2, #1         ;1T
    JNE     L_delay_ms_2    ;3T

    DJNZ    R4, L_delay_ms_1

    POP     04H     ;��ջR2
    POP     03H     ;��ջR3
    POP     02H     ;��ջR4
    RET



    END

