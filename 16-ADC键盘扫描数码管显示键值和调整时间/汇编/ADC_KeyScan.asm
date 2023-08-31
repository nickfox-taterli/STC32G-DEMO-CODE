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


;*************  ����˵��    **************

;�����̻���STC32GΪ����оƬ��ʵ������б�д���ԡ�

;ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

;edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

;��STC��MCU��IO��ʽ����8λ����ܡ�

;��ʾЧ��Ϊ: ����ʱ��.

;ʹ��Timer0��16λ�Զ���װ������1ms����,�������������������, �û��޸�MCU��ʱ��Ƶ��ʱ,�Զ���ʱ��1ms.

;���4λLED��ʾʱ��(Сʱ,����), �ұ������λ��ʾ����ֵ.

;ADC��������Ϊ1~16.

;����ֻ֧�ֵ�������, ��֧�ֶ��ͬʱ����, ���������в���Ԥ֪�Ľ��.

;�����³���1���,����10��/����ٶ��ṩ�ؼ����. �û�ֻ��Ҫ���KeyCode�Ƿ��0���жϼ��Ƿ���.

;����ʱ���:
;����1: Сʱ+.
;����2: Сʱ-.
;����3: ����+.
;����4: ����-.

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

usrhour            DATA    39H     ;RTC����
usrminute          DATA    3AH
usrsecond          DATA    3BH     ;
msecond         DATA    3CH     ; 2 byte

ADC_KeyState    DATA    3EH ; ADC��״̬����
ADC_KeyState1   DATA    3FH
ADC_KeyState2   DATA    40H
ADC_KeyState3   DATA    41H
ADC_KeyHoldCnt  DATA    42H ; ADC�����¼�ʱ

KeyCode         DATA    43H ; ���û�ʹ�õļ���, 1~16ΪADC���� 17~32ΪIO��

cnt10ms         DATA    44H;
cnt50ms         DATA    45H;


;*******************************************************************
;*******************************************************************
        ORG     0000H               ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
        JMP     F_Main


        ORG     000BH               ;1  Timer0 interrupt
        JMP     F_Timer0_Interrupt

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
    MOV     P1M1, #31H     ;����P1.4��P1.5Ϊ©����·(ʵ��������������赽3.3V), P1.0 Ϊ ADC ���������
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
    SETB    EA          ; �����ж�
    
    MOV     usrhour,   #12 ; ��ʼ��ʱ��ֵ
    MOV     usrminute, #0
    MOV     usrsecond, #0
    LCALL   F_DisplayRTC
    
    MOV     cnt10ms, #10

    CLR     A
    MOV     ADC_KeyState, A
    MOV     ADC_KeyState1, A
    MOV     ADC_KeyState2, A
    MOV     ADC_KeyState3, A    ; ��״̬
    MOV     ADC_KeyHoldCnt, A   ; �����¼�ʱ
    MOV     KeyCode, A          ; ���û�ʹ�õļ���, 1~16��Ч

    MOV     A, #03FH            ; ���� ADC �ڲ�ʱ��ADC����ʱ�佨�������ֵ
    MOV     WR6, #WORD0 ADCTIM
    MOV     WR4, #WORD2 ADCTIM
    MOV     @DR4, R11

    MOV     ADCCFG, #02FH       ; ����ת������Ҷ���ģʽ�� ADC ʱ��Ϊϵͳʱ��/2/16
    MOV     ADC_CONTR, #080H    ; ʹ�� ADC ģ��

;=====================================================

;=====================================================
L_Main_Loop:
    JNB     B_1ms,  L_Main_Loop     ;1msδ��
    CLR     B_1ms
    
    MOV     WR6, msecond
    INC     WR6, #1         ;msecond + 1
    MOV     msecond, WR6
    CMP     WR6, #1000
    JC      L_Quit_Check_1000ms  ;if(msecond < 1000), jmp
    MOV     WR6, #0
    MOV     msecond, WR6    ;msecond = 0

    LCALL   F_RTC
    LCALL   F_DisplayRTC
L_Quit_Check_1000ms:

    MOV     WR6, msecond    ;���if(msecond == 500)
    CMP     WR6, #500
    JNE     L_Quit_Display_Dot
    LCALL   F_DisplayRTC    ;Сʱ���С����������
L_Quit_Display_Dot:
;=====================================================

;==================== 10ms��һ��ADC =================================
L_Read_ADC_Key:
    DJNZ    cnt10ms, L_Quit_Read_ADC_Key    ; (cnt10ms - 1) != 0, jmp
    MOV     cnt10ms, #10    ;
    
    MOV     A, #0
    LCALL   F_Get_ADC12bitResult    ;ACC - ͨ��0~15, ��ѯ��ʽ��һ��ADC, ����ֵ(R6 R7)����ADC���, == 4096 Ϊ����
    MOV     A, R6
    ANL     A, #0F0H
    JNZ     L_Quit_Read_ADC_Key     ; adc >= 4096, ����
    LCALL   F_CalculateAdcKey       ; ���㰴��, (R6 R7) == adc, ����ֵ��R4 R5.

L_Quit_Read_ADC_Key:
;=====================================================

;=====================================================
L_KeyProcess:
    MOV     A, KeyCode
    JZ      L_Quit_KeyProcess
                            ;�м�����
    MOV     A, KeyCode
    MOV     B, #10
    DIV     AB
    MOV     LED8+1, A       ; ��ʾ����
    MOV     LED8+0, B

    MOV     A, KeyCode
    CJNE    A, #1, L_Quit_K1
    INC     usrhour    ; usrhour + 1
    MOV     A, usrhour
    CLR     C
    SUBB    A, #24
    JC      L_K1_Display
    MOV     usrhour, #0
L_K1_Display:
    LCALL   F_DisplayRTC
L_Quit_K1:

    MOV     A, KeyCode
    CJNE    A, #2, L_Quit_K2
    DEC     usrhour    ; usrhour - 1
    MOV     A, usrhour
    CJNE    A, #255, L_K2_Display
    MOV     usrhour, #23
L_K2_Display:
    LCALL   F_DisplayRTC
L_Quit_K2:

    MOV     A, KeyCode
    CJNE    A, #3, L_Quit_K3
    MOV     usrsecond, #0      ;��������ʱ�����
    INC     usrminute  ; usrminute + 1
    MOV     A, usrminute
    CLR     C
    SUBB    A, #60
    JC      L_K3_Display
    MOV     usrminute, #0
L_K3_Display:
    LCALL   F_DisplayRTC
L_Quit_K3:

    MOV     A, KeyCode
    CJNE    A, #4, L_Quit_K4
    MOV     usrsecond, #0      ;��������ʱ�����
    DEC     usrminute  ; usrminute - 1
    MOV     A, usrminute
    CJNE    A, #255, L_K4_Display
    MOV     usrminute, #59
L_K4_Display:
    LCALL   F_DisplayRTC
L_Quit_K4:

    MOV     KeyCode, #0
L_Quit_KeyProcess:

    LJMP    L_Main_Loop

;**********************************************/


;========================================================================
; ����: F_DisplayRTC
; ����: ��ʾʱ���ӳ���
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
; ��ע: ����ACCC��PSW��, ���õ���ͨ�üĴ�������ջ
;========================================================================
F_DisplayRTC:
    PUSH    B       ;B��ջ
    
    MOV     A, usrhour
    MOV     B, #10
    DIV     AB
    MOV     LED8+7, A
    MOV     LED8+6, B
    
    MOV     A, usrminute
    MOV     B, #10
    DIV     AB
    MOV     LED8+5, A;
    MOV     LED8+4, B;

    MOV     WR6, msecond
    CMP     WR6, #500
    JC      L_QuitDisplayRTC    ;if(msecond < 500), jmp
    ORL     LED8+6, #DIS_DOT    ; Сʱ���С����������
L_QuitDisplayRTC:
    POP     B       ;B��ջ
    RET

;========================================================================
; ����: F_RTC
; ����: RTC��ʾ�ӳ���
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
; ��ע: ����ACCC��PSW��, ���õ���ͨ�üĴ�������ջ
;========================================================================
F_RTC:
    INC     usrsecond      ; usrsecond + 1
    MOV     A, usrsecond
    CLR     C
    SUBB    A,#60
    JC      L_QuitRTC   ; usrsecond >= 60?
    MOV     usrsecond, #0;

    INC     usrminute      ; usrminute + 1
    MOV     A, usrminute
    CLR     C
    SUBB    A,#60
    JC      L_QuitRTC   ; usrminute >= 60?
    MOV     usrminute, #0

    INC     usrhour        ; usrhour + 1
    MOV     A, usrhour
    CLR     C
    SUBB    A,#24
    JC      L_QuitRTC   ; usrhour >= 24?
    MOV     usrhour, #0
L_QuitRTC:
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


;/***************** ADC���̼������ *****************************
;��·������㷨���: Coody
;��ADC���̷����ںܶ�ʵ�ʲ�Ʒ�����, ��֤�����ȶ��ɿ�, ��ʹ����ʹ�õ���Ĥ,���ܿɿ�.
;16����,�����ϸ�������Ӧ��ADCֵΪ (4096 / 16) * k = 256 * k, k = 1 ~ 16, �ر��, k=16ʱ,��Ӧ��ADCֵ��4095.
;����ʵ�ʻ���ƫ��,���ж�ʱ�������ƫ��, ADC_OFFSETΪ+-ƫ��, ��ADCֵ�� (256*k-ADC_OFFSET) �� (256*k+ADC_OFFSET)֮��Ϊ����Ч.
;���һ����ʱ��,�Ͳ���һ��ADC,����10ms.
;Ϊ�˱���żȻ��ADCֵ����, ���߱���ADC���������½�ʱ����, ʹ������3��ADCֵ����ƫ�Χ��ʱ, ADCֵ����Ϊ��Ч.
;�����㷨, �ܱ�֤�����ǳ��ɿ�.
;**********************************************/
ADC_OFFSET  EQU 64
F_CalculateAdcKey:      //R6 R7Ϊ�����ADCֵ (u16 adc)
    PUSH    02H     ;R2��ջ
    PUSH    03H     ;R3��ջ
    PUSH    04H     ;R4��ջ
    PUSH    05H     ;R5��ջ
    PUSH    DPH     ;DPH��ջ
    PUSH    DPL     ;DPL��ջ
    
    CMP     WR6, #(256 - ADC_OFFSET)
    JNC     L_ADC_MoreThanOffset
    MOV     ADC_KeyState, #0    ;   if(adc < (256-ADC_OFFSET)), ��״̬��0
    MOV     ADC_KeyHoldCnt, #0  ;

L_ADC_MoreThanOffset:
    MOV     WR4, #0100H      ;j = 256
    MOV     R10, #1
L_CheckAdcKeyLoop:
    MOV     WR2, WR4
    SUB     WR2, #ADC_OFFSET
    CMP     WR2, WR6
    JG      L_CheckNextAdcKey ;adc < (j - ADC_OFFSET)
    MOV     WR2, WR4
    ADD     WR2, #ADC_OFFSET
    CMP     WR2, WR6
    JNC     L_CheckAdcKeyEnd ;(adc >= (j - ADC_OFFSET)) && (adc <= (j + ADC_OFFSET))
L_CheckNextAdcKey:
    ADD     WR4, #0100H      ;j = j + 256

    INC     R10, #1
    CMP     R10, #010H
    JLE     L_CheckAdcKeyLoop

L_CheckAdcKeyEnd:
    MOV     ADC_KeyState3, ADC_KeyState2
    MOV     ADC_KeyState2, ADC_KeyState1
    MOV     R11, R10
    CLR     C
    SUBB    A, #17
    JC      L_AdcKeyIsOk
    MOV     ADC_KeyState1, #0   ;if(i > 16) KeyState1 = 0;  //����Ч
    LJMP    L_QuitCheckAdcKey

L_AdcKeyIsOk:       ;����Ч
    MOV     ADC_KeyState1, R10
    MOV     A, ADC_KeyState3
    CJNE    A, ADC_KeyState2, L_QuitCheckAdcKey     ;if (KeyState3 != KeyState2)
    CJNE    A, ADC_KeyState1, L_QuitCheckAdcKey     ;if (KeyState3 != KeyState1)
    JZ      L_QuitCheckAdcKey                   ;if (KeyState3 == 0)

    MOV     A, ADC_KeyState
    JNZ     L_NotFirstCheck ;���ǵ�һ�μ�⵽, jmp
    MOV     KeyCode,  R10    ; �������
    MOV     ADC_KeyState, R10   ; �����״̬
    MOV     ADC_KeyHoldCnt, #0  ; ��������ŵ�ʱ�����
L_NotFirstCheck:

    MOV     A, ADC_KeyState
    XRL     R11, R10
    JNZ     L_NotCheckAdcKeySame
                ;if(KeyState == i)  //������⵽ͬһ������
    INC     ADC_KeyHoldCnt
    MOV     A, ADC_KeyHoldCnt
    CJNE    A, #100, L_QuitCheckAdcKey  ;if(++KeyHoldCnt >= 100)    //����1���,��10��ÿ����ٶ�Repeat Key
    MOV     ADC_KeyHoldCnt, #90     ;����1���,��10��ÿ����ٶ�Repeat Key
    MOV     KeyCode, R10         ; �������
    SJMP    L_QuitCheckAdcKey
L_NotCheckAdcKeySame:
    MOV     ADC_KeyHoldCnt, #0  ;   ����ʱ�������0

L_QuitCheckAdcKey:
    POP     DPL     ;DPL��ջ
    POP     DPH     ;DPH��ջ
    POP     05H     ;R5��ջ
    POP     04H     ;R4��ջ
    POP     03H     ;R3��ջ
    POP     02H     ;R2��ջ
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
    


    END

