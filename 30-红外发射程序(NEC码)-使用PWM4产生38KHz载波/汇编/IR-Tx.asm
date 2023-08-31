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

;�û������ں궨���иı�MCU��ʱ��Ƶ��. ��Χ 8MHZ ~ 33MHZ.

;���ⷢ�����ģ���г�����������NEC�ı��롣

;�û������ں궨����ָ���û���.

;ʹ��PWM4����38KHZ�ز�, 1/3ռ�ձ�, ÿ��38KHZ���ڷ���ܷ���9us,�ر�16.3us.

;ʹ�ÿ������ϵ�16��IOɨ�谴��, MCU��˯��, ����ɨ�谴��.

;��������, ��һ֡Ϊ����, �����֡Ϊ�ظ�֡,��������, ���嶨�������вο�NEC�ı�������.

;���ͷź�, ֹͣ����.

;******************************************/

$include (../../comm/STC32G.INC)

;/****************************** �û������ ***********************************/

STACK_POIRTER   EQU     0D0H    ;��ջ��ʼ��ַ
Fosc_KHZ        EQU     24000   ;24000KHZ

;*******************************************************************
;*******************************************************************


;*************  IO�ڶ���    **************/

P_IR_TX   BIT P2.7  ;������ⷢ�Ͷ˿�

;*************  ���ر�������    **************/

IO_KeyState     DATA    30H ; IO���м�״̬����
IO_KeyState1    DATA    31H
IO_KeyHoldCnt   DATA    32H ; IO�����¼�ʱ
KeyCode         DATA    33H ; ���û�ʹ�õļ���, 1~16ΪADC���� 17~32ΪIO��


/*************  ���ⷢ����ر���    **************/
#define User_code   0xFF00      //��������û���

FLAG0   DATA    0x20
B_Space BIT FLAG0.0 ;���Ϳ���(��ʱ)��־

tx_cnt      DATA    0x36    ;���ͻ���е��������(����38KHZ������������Ӧʱ��), ����Ƶ��Ϊ38KHZ, ����26.3us
TxTime      DATA    0x38    ;����ʱ��


;*******************************************************************
;*******************************************************************
        ORG     0000H               ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
        LJMP    F_Main

        ORG     00D3H               ;26  PWMA interrupt
        LJMP    F_PWMA_Interrupt


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

    LCALL   F_PWM_Init      ;��ʼ��PWM
    SETB    P_IR_TX
    
    SETB    EA              ;�����ж�
    
    MOV     KeyCode, #0

;=================== ��ѭ�� ==================================
L_Main_Loop:
    MOV     A, #30
    LCALL   F_delay_ms      ;��ʱ30ms
    LCALL   F_IO_KeyScan    ;ɨ�����

    MOV     A, KeyCode
    JZ      L_Main_Loop     ;�޼�ѭ��
    MOV     TxTime, #0      ;
                            ;һ֡������С���� = 9 + 4.5 + 0.5625 + 24 * 1.125 + 8 * 2.25 = 59.0625 ms
                            ;һ֡������󳤶� = 9 + 4.5 + 0.5625 + 8 * 1.125 + 24 * 2.25 = 77.0625 ms
    MOV     WR6, #342       ;��Ӧ9ms��ͬ��ͷ        9ms
    LCALL   F_IR_TxPulse

    MOV     WR6, #171       ;��Ӧ4.5ms��ͬ��ͷ���  4.5ms
    LCALL   F_IR_TxSpace

    MOV     WR6, #21        ;��������           0.5625ms
    LCALL   F_IR_TxPulse

    MOV     A, #LOW  User_code  ;���û�����ֽ�
    LCALL   F_IR_TxByte
    MOV     A, #HIGH User_code  ;���û�����ֽ�
    LCALL   F_IR_TxByte
    MOV     A, KeyCode          ;������
    LCALL   F_IR_TxByte
    MOV     A, KeyCode          ;�����ݷ���
    CPL     A
    LCALL   F_IR_TxByte

    MOV     A, TxTime
    CMP     R11, #56      ;һ֡�����77ms����, �����Ļ�,����ʱ��      108ms
    JNC     L_ADJ_Time

    CLR     c
    MOV     A, #56
    SUBB    A, TxTime
    MOV     TxTime, A
    RRC     A
    RRC     A
    RRC     A
    ANL     A, #0x1F
    ADD     A, TxTime
    LCALL   F_delay_ms      ;TxTime = 56 - TxTime;  TxTime = TxTime + TxTime / 8;   delay_ms(TxTime);
L_ADJ_Time:
    MOV     A, #31          ;delay_ms(31)
    LCALL   F_delay_ms
        
L_WaitKeyRelease:
    MOV     A, IO_KeyState
    JZ      L_ClearKeyCode

                            ;��δ�ͷ�, �����ظ�֡(������)
    MOV     WR6, #342       ;��Ӧ9ms��ͬ��ͷ        9ms
    LCALL   F_IR_TxPulse

    MOV     WR6, #86        ;��Ӧ2.25ms��ͬ��ͷ��� 2.25ms
    LCALL   F_IR_TxSpace

    MOV     WR6, #21        ;��������           0.5625ms
    LCALL   F_IR_TxPulse

    MOV     A, #96          ;delay_ms(96)
    LCALL   F_delay_ms
    LCALL   F_IO_KeyScan    ;ɨ�����
    SJMP    L_WaitKeyRelease

L_ClearKeyCode:
    MOV     KeyCode, #0

    LJMP    L_Main_Loop
;===================================================================


;========================================================================
; ����: F_delay_ms
; ����: ��ʱ�ӳ���
; ����: ACC: ��ʱms��.
; ����: none.
; �汾: VER1.0
; ����: 2021-3-16
; ��ע: ����ACCC��PSW��, ���õ���ͨ�üĴ�������ջ����, �˳�ʱ�ָ�ԭ�����ݲ��ı�.
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


;========================================================================
; ����: F_IO_KeyDelay
; ����: ���м�ɨ�����.
; ����: none
; ����: ��������, KeyCodeΪ��0����.
; �汾: V1.0, 2013-11-22
;========================================================================
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

F_IO_KeyScan:
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
    RET

;========================================================================
; ����: F_IR_TxPulse
; ����: �������庯��.
; ����: tx_cntH, tx_cntL: Ҫ���͵�38K������
; ����: none.
; �汾: V1.0, 2013-11-22
;========================================================================
F_IR_TxPulse:
    MOV     tx_cnt, WR6
    CLR     B_Space

    MOV     A, #00                 ;д CCMRx ǰ���������� CCxE �ر�ͨ��
    MOV     WR6, #WORD0 PWMA_CCER2
    MOV     WR4, #WORD2 PWMA_CCER2
    MOV     @DR4, R11
    MOV     A, #068H               ;���� PWM4 ģʽ1 ���
    MOV     WR6, #WORD0 PWMA_CCMR4
    MOV     WR4, #WORD2 PWMA_CCMR4
    MOV     @DR4, R11
    MOV     A, #070H               ;ʹ�� CC4NE ͨ��, �͵�ƽ��Ч
    MOV     WR6, #WORD0 PWMA_CCER2
    MOV     WR4, #WORD2 PWMA_CCER2
    MOV     @DR4, R11
    MOV     A, #010H               ;ʹ�ܲ���/�Ƚ� 4 �ж�
    MOV     WR6, #WORD0 PWMA_IER
    MOV     WR4, #WORD2 PWMA_IER
    MOV     @DR4, R11

    JNB     B_Space, $   ;�ȴ�����
    RET

;========================================================================
; ����: F_IR_TxSpace
; ����: ���Ϳ��к���.
; ����: tx_cntH, tx_cntL: Ҫ���͵�38K������
; ����: none.
; �汾: V1.0, 2013-11-22
;========================================================================
F_IR_TxSpace:
    MOV     tx_cnt, WR6
    CLR     B_Space

    MOV     A, #00                 ;д CCMRx ǰ���������� CCxE �ر�ͨ��
    MOV     WR6, #WORD0 PWMA_CCER2
    MOV     WR4, #WORD2 PWMA_CCER2
    MOV     @DR4, R11
    MOV     A, #040H               ;���� PWM4 ǿ��Ϊ��Ч��ƽ
    MOV     WR6, #WORD0 PWMA_CCMR4
    MOV     WR4, #WORD2 PWMA_CCMR4
    MOV     @DR4, R11
    MOV     A, #070H               ;ʹ�� CC4NE ͨ��, �͵�ƽ��Ч
    MOV     WR6, #WORD0 PWMA_CCER2
    MOV     WR4, #WORD2 PWMA_CCER2
    MOV     @DR4, R11
    MOV     A, #010H               ;ʹ�ܲ���/�Ƚ� 4 �ж�
    MOV     WR6, #WORD0 PWMA_IER
    MOV     WR4, #WORD2 PWMA_IER
    MOV     @DR4, R11

    JNB     B_Space, $   ;�ȴ�����
    RET

;========================================================================
; ����: F_IR_TxByte
; ����: ����һ���ֽں���.
; ����: ACC: Ҫ���͵��ֽ�
; ����: none.
; �汾: V1.0, 2013-11-22
;========================================================================
F_IR_TxByte:
    PUSH    AR2
    PUSH    AR3

    MOV     R2, #8
    MOV     R3, A
L_IR_TxByteLoop:
    MOV     A, R3
    JNB     ACC.0, L_IR_TxByte_0
    MOV     WR6, #63        ;��������1
    LCALL   F_IR_TxSpace
    INC     TxTime          ;TxTime += 2;   //����1��Ӧ 1.6875 + 0.5625 ms
    INC     TxTime
    SJMP    L_IR_TxByte_Pause
L_IR_TxByte_0:
    MOV     WR6, #21        ;��������0
    LCALL   F_IR_TxSpace
    INC     TxTime          ;����0��Ӧ 0.5625 + 0.5625 ms
L_IR_TxByte_Pause:
    MOV     WR6, #21        ;��������
    LCALL   F_IR_TxPulse    ;���嶼��0.5625ms
    MOV     A, R3
    RR      A               ;��һ��λ
    MOV     R3, A
    DJNZ    R2, L_IR_TxByteLoop
    POP     AR3
    POP     AR2
    
    RET

;========================================================================
; ����: F_PWM_Init
; ����: PWM��ʼ������.
; ����: none
; ����: none.
; �汾: V1.0, 2013-11-22
;========================================================================
F_PWM_Init:
    MOV     A, #00H              ;д CCMRx ǰ���������� CCxE �ر�ͨ��
    MOV     WR6, #WORD0 PWMA_CCER2
    MOV     WR4, #WORD2 PWMA_CCER2
    MOV     @DR4, R11
    MOV     A, #068H             ;���� PWM4 ģʽ1 ���
    MOV     WR6, #WORD0 PWMA_CCMR4
    MOV     WR4, #WORD2 PWMA_CCMR4
    MOV     @DR4, R11

    MOV     A, #2                ;��������ʱ��
    MOV     WR6, #WORD0 PWMA_ARRH
    MOV     WR4, #WORD2 PWMA_ARRH
    MOV     @DR4, R11
    MOV     A, #077H
    MOV     WR6, #WORD0 PWMA_ARRL
    MOV     WR4, #WORD2 PWMA_ARRL
    MOV     @DR4, R11
    MOV     A, #0                ;����ռ�ձ�ʱ��
    MOV     WR6, #WORD0 PWMA_CCR4H
    MOV     WR4, #WORD2 PWMA_CCR4H
    MOV     @DR4, R11
    MOV     A, #210
    MOV     WR6, #WORD0 PWMA_CCR4L
    MOV     WR4, #WORD2 PWMA_CCR4L
    MOV     @DR4, R11

    MOV     A, #080H             ;ʹ�� PWM4N ���
    MOV     WR6, #WORD0 PWMA_ENO
    MOV     WR4, #WORD2 PWMA_ENO
    MOV     @DR4, R11
    MOV     A, #040H             ;�߼� PWM ͨ�� 4N �����ѡ��λ, 0x00:P1.7, 0x40:P2.7, 0x80:P6.7, 0xC0:P3.3
    MOV     WR6, #WORD0 PWMA_PS
    MOV     WR4, #WORD2 PWMA_PS
    MOV     @DR4, R11
    MOV     A, #080H             ;ʹ�������
    MOV     WR6, #WORD0 PWMA_BKR
    MOV     WR4, #WORD2 PWMA_BKR
    MOV     @DR4, R11

    MOV     WR6, #WORD0 PWMA_CR1
    MOV     WR4, #WORD2 PWMA_CR1
    MOV     R11, @DR4
    ORL     A,#081H              ;ʹ��ARRԤװ�أ���ʼ��ʱ
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
    PUSH    R4
    PUSH    R5
    PUSH    R6
    PUSH    R7

    MOV     WR6, #WORD0 PWMA_SR1 ;���ӻ�״̬
    MOV     WR4, #WORD2 PWMA_SR1
    MOV     R11, @DR4
    JNB     ACC.4,F_PWMA_QuitInt
    CLR     A
    MOV     @DR4, R11

    MOV     WR6, tx_cnt
    DEC     WR6, #1       ;tx_cnt - 1
    MOV     tx_cnt, WR6
    JNE     F_PWMA_QuitInt

    MOV     A, #00                 ;д CCMRx ǰ���������� CCxE �ر�ͨ��
    MOV     WR6, #WORD0 PWMA_CCER2
    MOV     WR4, #WORD2 PWMA_CCER2
    MOV     @DR4, R11
    MOV     A, #040H               ;���� PWM4 ǿ��Ϊ��Ч��ƽ
    MOV     WR6, #WORD0 PWMA_CCMR4
    MOV     WR4, #WORD2 PWMA_CCMR4
    MOV     @DR4, R11
    MOV     A, #070H               ;ʹ�� CC4NE ͨ��, �͵�ƽ��Ч
    MOV     WR6, #WORD0 PWMA_CCER2
    MOV     WR4, #WORD2 PWMA_CCER2
    MOV     @DR4, R11
    MOV     A, #00H                ;�ر��ж�
    MOV     WR6, #WORD0 PWMA_IER
    MOV     WR4, #WORD2 PWMA_IER
    MOV     @DR4, R11

    SETB    B_Space        ;���ý�����־

F_PWMA_QuitInt:
    POP     R7
    POP     R6
    POP     R5
    POP     R4
    POP     ACC
    POP     PSW
    RETI

    END

