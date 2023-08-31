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

;�����շ������������г�����������NEC���롣

;Ӧ�ò��ѯ B_IR_Press��־Ϊ,���ѽ��յ�һ���������IR_code��, ���������� �û��������B_IR_Press��־.

;���������4λ��ʾ�û���, ���ұ���λ��ʾ����, ��Ϊʮ������.

;�û������ں궨����ָ���û���.

;�û��ײ���򰴹̶���ʱ����(60~125us)���� "IR_RX_NEC()"����.

;����IO���м�����֧��ADC���̣�����ʾ���͡����յ��ļ�ֵ��

;����ʱ, ѡ��ʱ�� 24MHz (�û��������޸�Ƶ��).

;******************************************/

$include (../../comm/STC32G.INC)

;/****************************** �û������ ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHZ, �û�ֻ��Ҫ�Ķ����ֵ����Ӧ�Լ�ʵ�ʵ�Ƶ��

STACK_POIRTER   EQU     0D0H    ;��ջ��ʼ��ַ

Timer0_Reload   EQU     (65536 - (Fosc_KHZ/10))   ; Timer 0 �ж�Ƶ��, 10000��/��

DIS_DOT         EQU     020H
DIS_BLACK       EQU     010H
DIS_            EQU     011H

;*******************************************************************
;*******************************************************************


;*************  ���ر�������    **************/
Flag0           DATA    20H
B_1ms           BIT     Flag0.0     ;1ms��־
P_IR_RX_temp    BIT     Flag0.1     ;�û����ɲ���, Last sample
B_IR_Sync       BIT     Flag0.2     ;�û����ɲ���, ���յ�ͬ����־
B_Space         BIT     Flag0.3     ;���Ϳ���(��ʱ)��־
B_IR_Press      BIT     Flag0.4     ;�û�ʹ��, ������������

LED8            DATA    30H     ;   ��ʾ���� 30H ~ 37H
display_index   DATA    38H     ;   ��ʾλ����

cnt_1ms         DATA    39H     ;

;*************  ������ճ����������    **************
#define User_code   0xFF00      //��������û���

P_IR_TX         BIT P2.7    ;������ⷢ�Ͷ˿�
P_IR_RX         BIT P3.5    ;��������������IO��

IR_SampleCnt    DATA    3AH ;�û����ɲ���, ��������
IR_BitCnt       DATA    3BH ;�û����ɲ���, ����λ��
IR_UserH        DATA    3CH ;�û����ɲ���, �û���(��ַ)���ֽ�
IR_UserL        DATA    3DH ;�û����ɲ���, �û���(��ַ)���ֽ�
IR_data         DATA    3EH ;�û����ɲ���, ����ԭ��
IR_DataShit     DATA    3FH ;�û����ɲ���, ������λ

IR_code         DATA    40H ;�û�ʹ��, �������
UserCodeH       DATA    41H ;�û�ʹ��, �û�����ֽ�
UserCodeL       DATA    42H ;�û�ʹ��, �û�����ֽ�
msecond         DATA    43H

IO_KeyState     DATA    44H ; IO���м�״̬����
IO_KeyState1    DATA    45H
IO_KeyHoldCnt   DATA    46H ; IO�����¼�ʱ
KeyCode         DATA    47H ; ���û�ʹ�õļ���, 1~16ΪADC���� 17~32ΪIO��

/*************  ���ⷢ����ر���    **************/
tx_cnt          DATA    48H ;���ͻ���е��������(����38KHZ������������Ӧʱ��), ����Ƶ��Ϊ38KHZ, ����26.3us
TxTime          DATA    4AH ;����ʱ��


;*******************************************************************
;*******************************************************************
        ORG     0000H               ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
        LJMP    F_Main

        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt

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
    MOV     P1M1, #32H     ;����P1.4��P1.5Ϊ©����·(ʵ��������������赽3.3V), P1.1��PWM��DAC��·ͨ�����贮����P2.3
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

    MOV     display_index, #0
    MOV     R0, #LED8
    MOV     R2, #8
L_ClearLoop:
    MOV     @R0, #DIS_
    INC     R0
    DJNZ    R2, L_ClearLoop

    MOV     R0, #LED8+2
    MOV     @R0, #DIS_BLACK     ;�ϵ�����
    INC     R0
    MOV     @R0, #DIS_BLACK     ;�ϵ�����
    
    CLR     TR0
    ORL     AUXR, #(1 SHL 7)    ; Timer0_1T();
    ANL     TMOD, #NOT 04H      ; Timer0_AsTimer();
    ANL     TMOD, #NOT 03H      ; Timer0_16bitAutoReload();
    MOV     TH0, #Timer0_Reload / 256   ;Timer0_Load(Timer0_Reload);
    MOV     TL0, #Timer0_Reload MOD 256
    SETB    ET0         ; Timer0_InterruptEnable();
    SETB    TR0         ; Timer0_Run();
    
    LCALL   F_PWM_Init      ;��ʼ��PWM
    SETB    P_IR_TX
    SETB    EA              ;�����ж�
    
    MOV     KeyCode, #0
    MOV     cnt_1ms, #10

;=====================================================

;=====================================================
L_Main_Loop:

    JNB     B_1ms,  L_Main_Loop     ;1msδ��
    CLR     B_1ms
    
    JNB     B_IR_Press, L_Main_KeyScan ;δ��⵽�յ��������

    CLR     B_IR_Press      ;��⵽�յ��������
    MOV     A, UserCodeH
    SWAP    A
    ANL     A, #0FH
    MOV     LED8+7, A       ;�û�����ֽڵĸ߰��ֽ�
    MOV     A, UserCodeH
    ANL     A, #0FH
    MOV     LED8+6, A       ;�û�����ֽڵĵͰ��ֽ�

    MOV     A, UserCodeL
    SWAP    A
    ANL     A, #0FH
    MOV     LED8+5, A       ;�û�����ֽڵĸ߰��ֽ�
    MOV     A, UserCodeL
    ANL     A, #0FH
    MOV     LED8+4, A       ;�û�����ֽڵĵͰ��ֽ�
    
    MOV     A, IR_code
    SWAP    A
    ANL     A, #0FH
    MOV     LED8+1, A       ;ң�����ݵĸ߰��ֽ�
    MOV     A, IR_code
    ANL     A, #0FH
    MOV     LED8+0, A       ;ң�����ݵĵͰ��ֽ�

L_Main_KeyScan:
;=================== ���30ms�Ƿ� ==================================
    INC     msecond       ;msecond + 1
    CLR     C
    MOV     A, msecond    ;msecond - 30
    SUBB    A, #30
    JC      L_Main_Loop     ;if(msecond < 30), jmp
    
;================= 30ms�� ====================================
    MOV     msecond, #0

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

L_ClearKeyCode:
    MOV KeyCode, #0

    LJMP    L_Main_Loop

;**********************************************/


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
    PUSH    AR0     ;R0 ��ջ
    
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
    POP     AR0     ;R0 ��ջ
    POP     DPL     ;DPL��ջ
    POP     DPH     ;DPH��ջ
    RET


;*******************************************************************
;**************** �жϺ��� ***************************************************

F_Timer0_Interrupt: ;Timer0 1ms�жϺ���
    PUSH    PSW     ;PSW��ջ
    PUSH    ACC     ;ACC��ջ
    PUSH    AR7     ;SampleTime

    LCALL   F_IR_RX_NEC

    DJNZ    cnt_1ms, L_Quit_1ms
    MOV     cnt_1ms, #10
    LCALL   F_DisplayScan   ; 1msɨ����ʾһλ
    SETB    B_1ms           ; 1ms��־
L_Quit_1ms:

    POP     AR7
    POP     ACC     ;ACC��ջ
    POP     PSW     ;PSW��ջ
    RETI
    

;*******************************************************************
;*********************** IR Remote Module **************************
;*********************** By  (Coody) 2002-8-24 *********************
;*********************** IR Remote Module **************************
;this programme is used for Receive IR Remote (NEC Code).

;data format: Synchro, AddressH, AddressL, data, /data, (total 32 bit).

;send a frame(85ms), pause 23ms, send synchro of continue frame, pause 94ms

;data rate: 108ms/Frame


;Synchro: low=9ms, high=4.5 / 2.25ms, low=0.5626ms
;Bit0: high=0.5626ms, low=0.5626ms
;Bit1: high=1.6879ms, low=0.5626ms
;frame rate = 108ms ( pause 23 ms or 96 ms)

;******************** �������ʱ��궨��, �û���Ҫ�����޸�  *******************

D_IR_sample         EQU 100                 ;��ѯʱ����, 100us, �������Ҫ����60us~250us֮��
D_IR_SYNC_MAX       EQU (15000/D_IR_sample) ;SYNC max time
D_IR_SYNC_MIN       EQU (9700 /D_IR_sample) ;SYNC min time
D_IR_SYNC_DIVIDE    EQU (12375/D_IR_sample) ;decide data 0 or 1
D_IR_DATA_MAX       EQU (3000 /D_IR_sample) ;data max time
D_IR_DATA_MIN       EQU (600  /D_IR_sample) ;data min time
D_IR_DATA_DIVIDE    EQU (1687 /D_IR_sample) ;decide data 0 or 1
D_IR_BIT_NUMBER     EQU 32                  ;bit number

;*******************************************************************************************
;**************************** IR RECEIVE MODULE ********************************************

F_IR_RX_NEC:
    INC     IR_SampleCnt        ;Sample + 1

    MOV     C, P_IR_RX_temp     ;Save Last sample status
    MOV     F0, C
    MOV     C, P_IR_RX          ;Read current status
    MOV     P_IR_RX_temp, C

    JNB     F0, L_QuitIrRx              ;Pre-sample is high
    JB      P_IR_RX_temp, L_QuitIrRx    ;and current sample is low, so is fall edge

    MOV     R7, IR_SampleCnt            ;get the sample time
    MOV     IR_SampleCnt, #0            ;Clear the sample counter

    MOV     A, R7       ; if(SampleTime > D_IR_SYNC_MAX)    B_IR_Sync = 0
    SETB    C
    SUBB    A, #D_IR_SYNC_MAX
    JC      L_IR_RX_1
    CLR     B_IR_Sync       ;large than the Maxim SYNC time, then error
    RET
    
L_IR_RX_1:
    MOV     A, R7       ; else if(SampleTime >= D_IR_SYNC_MIN)
    CLR     C
    SUBB    A, #D_IR_SYNC_MIN
    JC      L_IR_RX_2

    MOV     A, R7       ; else if(SampleTime >= D_IR_SYNC_MIN)
    SUBB    A, #D_IR_SYNC_DIVIDE
    JC      L_QuitIrRx
    SETB    B_IR_Sync           ;has received SYNC
    MOV     IR_BitCnt, #D_IR_BIT_NUMBER     ;Load bit number
    RET

L_IR_RX_2:
    JNB     B_IR_Sync, L_QuitIrRx   ;else if(B_IR_Sync), has received SYNC
    MOV     A, R7       ; if(SampleTime > D_IR_DATA_MAX)    B_IR_Sync = 0, data samlpe time too large
    SETB    C
    SUBB    A, #D_IR_DATA_MAX
    JC      L_IR_RX_3
    CLR     B_IR_Sync       ;data samlpe time too large
    RET

L_IR_RX_3:
    MOV     A, IR_DataShit      ;data shift right 1 bit
    CLR     C
    RRC     A
    MOV     IR_DataShit, A
    
    MOV     A, R7
    CLR     C
    SUBB    A, #D_IR_DATA_DIVIDE
    JC      L_IR_RX_4
    ORL     IR_DataShit, #080H  ;if(SampleTime >= D_IR_DATA_DIVIDE) IR_DataShit |= 0x80;    //devide data 0 or 1
L_IR_RX_4:
    DEC     IR_BitCnt
    MOV     A, IR_BitCnt
    JNZ     L_IR_RX_5           ;bit number is over?
    CLR     B_IR_Sync           ;Clear SYNC
    MOV     A, IR_DataShit      ;if(~IR_DataShit == IR_data)        //�ж�����������
    CPL     A
    XRL     A, IR_data
    JNZ     L_QuitIrRx
    
    MOV     UserCodeH, IR_UserH
    MOV     UserCodeL, IR_UserL
    MOV     IR_code, IR_data
    SETB    B_IR_Press          ;������Ч
    RET

L_IR_RX_5:
    MOV     A, IR_BitCnt        ;else if((IR_BitCnt & 7)== 0)   one byte receive
    ANL     A, #07H
    JNZ     L_QuitIrRx
    MOV     IR_UserL, IR_UserH      ;Save the User code high byte
    MOV     IR_UserH, IR_data       ;Save the User code low byte
    MOV     IR_data,  IR_DataShit   ;Save the IR data byte
L_QuitIrRx:
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

