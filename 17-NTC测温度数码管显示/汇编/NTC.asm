/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����        */
/*---------------------------------------------------------------------*/


;*************  ����˵��    **************

; �����̻���STC32GΪ����оƬ��ʵ������б�д���ԡ�

; ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

; edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

; ��ADC�Ͳ��¶�.

; ��STC��MCU��IO��ʽ����8λ����ܡ�

; ʹ��Timer0��16λ�Զ���װ������1ms����,�������������������,�û��޸�MCU��ʱ��Ƶ��ʱ,�Զ���ʱ��1ms.

; �ұ�4λ�������ʾ�¶�ֵ, �ֱ���0.1��.

; NTCʹ��1%���ȵ�MF52 10K@25��.

; ���¶�ʱ, ʹ��12λ��ADCֵ, ʹ�öԷֲ��ұ��������, С�����һλ���������Բ岹�������.

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
B_1ms           BIT     Flag0.0 ; 1ms��־

LED8            DATA    30H     ; ��ʾ���� 30H ~ 37H
display_index   DATA    38H     ; ��ʾλ����

msecond         DATA    39H     ; 2 byte
minTemp         DATA    3BH
maxTemp         DATA    3CH


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
    MOV     P1M1, #38H     ;����P1.4��P1.5Ϊ©����·(ʵ��������������赽3.3V), P1.3 Ϊ ADC ���������
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
    
;=================== ���300ms�Ƿ� ==================================
    MOV     WR6, msecond
    INC     WR6, #1         ;msecond + 1
    MOV     msecond, WR6
    CMP     WR6, #300
    JC      L_Main_Loop     ;if(msecond < 300), jmp
    MOV     WR6, #0
    MOV     msecond, WR6    ;msecond = 0

    MOV     A, #3
    LCALL   F_Get_ADC12bitResult    ;ACC - ͨ��0~7, ��ѯ��ʽ��һ��ADC, ����ֵ(R6 R7)����ADC���, == 4096 Ϊ����

    CMP     WR6, #01000H
    JNC     L_Read_NTC_ADC_Err      ; adc >= 4096, ����

    LCALL   F_get_temperature   ;�����¶�ֵ, (R6 R7)Ϊ12λADC,  

    CMP     WR6, #400
    JC      L_Temp_LowThan_0        ; (R6 R7) < 400, jmp
    CLR     F0
    SUB     WR6, #400
    SJMP    L_DisplayTemp

L_Temp_LowThan_0:
    SETB    F0
    MOV     WR4, #400
    SUB     WR4, WR6
    MOV     WR6, WR4

L_DisplayTemp:
    MOV     WR4, #00000H    ;������
    MOV     WR0, #00000H
    MOV     WR2, #1000
    MOV     DMAIR, #04H     ;32λ�޷��ų���
    MOV     A, R7           ;��ʾ����ֵ
    ANL     A, #0x0F
    MOV     LED8+3, A

    MOV     WR4, WR0        ;������������
    MOV     WR6, WR2
    MOV     WR0, #00000H
    MOV     WR2, #100
    MOV     DMAIR, #04H     ;32λ�޷��ų���
    MOV     A, R7           ;��ʾ����ֵ
    ANL     A, #0x0F
    MOV     LED8+2, A

    MOV     WR4, WR0        ;������������
    MOV     WR6, WR2
    MOV     WR0, #00000H
    MOV     WR2, #10
    MOV     DMAIR, #04H     ;32λ�޷��ų���
    MOV     A, R7           ;��ʾ����ֵ
    ANL     A, #0x0F
    ADD     A, #DIS_DOT
    MOV     LED8+1, A

    MOV     A, R3           ;��ʾ����ֵ
    ANL     A, #0x0F
    MOV     LED8+0, A

    MOV     A, LED8+3
    JNZ     L_LED8_3_Not_0
    MOV     LED8+3, #DIS_BLACK      ;ǧλΪ0������
L_LED8_3_Not_0:
    JNB     F0, L_QuitRead_NTC_ADC
    MOV     LED8+3, #DIS_   ;���¶�, ��ʾ-
    SJMP    L_QuitRead_NTC_ADC

L_Read_NTC_ADC_Err:     ;����, ��ʾ ----
    MOV     LED8+3, #DIS_
    MOV     LED8+2, #DIS_
    MOV     LED8+1, #DIS_
    MOV     LED8, #DIS_
L_QuitRead_NTC_ADC:

;=====================================================

    LJMP    L_Main_Loop

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

//    MOV     A, ADC_RES      ;12λAD����ĸ�4λ��ADC_RES�ĵ�4λ����8λ��ADC_RESL��
//    ANL     A, #0FH
    MOV     R6, ADC_RES      ;12λAD����ĸ�4λ��ADC_RES�ĵ�4λ����8λ��ADC_RESL��
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
    

;   MF52E 10K at 25, B = 3950, ADC = 12 bits
temp_table:
    DW  140     ;-40    0
    DW  149     ;-39    1
    DW  159     ;-38    2
    DW  168     ;-37    3
    DW  178     ;-36    4
    DW  188     ;-35    5
    DW  199     ;-34    6
    DW  210     ;-33    7
    DW  222     ;-32    8
    DW  233     ;-31    9
    DW  246     ;-30    10
    DW  259     ;-29    11
    DW  272     ;-28    12
    DW  286     ;-27    13
    DW  301     ;-26    14
    DW  317     ;-25    15
    DW  333     ;-24    16
    DW  349     ;-23    17
    DW  367     ;-22    18
    DW  385     ;-21    19
    DW  403     ;-20    20
    DW  423     ;-19    21
    DW  443     ;-18    22
    DW  464     ;-17    23
    DW  486     ;-16    24
    DW  509     ;-15    25
    DW  533     ;-14    26
    DW  558     ;-13    27
    DW  583     ;-12    28
    DW  610     ;-11    29
    DW  638     ;-10    30
    DW  667     ;-9 31
    DW  696     ;-8 32
    DW  727     ;-7 33
    DW  758     ;-6 34
    DW  791     ;-5 35
    DW  824     ;-4 36
    DW  858     ;-3 37
    DW  893     ;-2 38
    DW  929     ;-1 39
    DW  965     ;0  40
    DW  1003    ;1  41
    DW  1041    ;2  42
    DW  1080    ;3  43
    DW  1119    ;4  44
    DW  1160    ;5  45
    DW  1201    ;6  46
    DW  1243    ;7  47
    DW  1285    ;8  48
    DW  1328    ;9  49
    DW  1371    ;10 50
    DW  1414    ;11 51
    DW  1459    ;12 52
    DW  1503    ;13 53
    DW  1548    ;14 54
    DW  1593    ;15 55
    DW  1638    ;16 56
    DW  1684    ;17 57
    DW  1730    ;18 58
    DW  1775    ;19 59
    DW  1821    ;20 60
    DW  1867    ;21 61
    DW  1912    ;22 62
    DW  1958    ;23 63
    DW  2003    ;24 64
    DW  2048    ;25 65
    DW  2093    ;26 66
    DW  2137    ;27 67
    DW  2182    ;28 68
    DW  2225    ;29 69
    DW  2269    ;30 70
    DW  2312    ;31 71
    DW  2354    ;32 72
    DW  2397    ;33 73
    DW  2438    ;34 74
    DW  2479    ;35 75
    DW  2519    ;36 76
    DW  2559    ;37 77
    DW  2598    ;38 78
    DW  2637    ;39 79
    DW  2675    ;40 80
    DW  2712    ;41 81
    DW  2748    ;42 82
    DW  2784    ;43 83
    DW  2819    ;44 84
    DW  2853    ;45 85
    DW  2887    ;46 86
    DW  2920    ;47 87
    DW  2952    ;48 88
    DW  2984    ;49 89
    DW  3014    ;50 90
    DW  3044    ;51 91
    DW  3073    ;52 92
    DW  3102    ;53 93
    DW  3130    ;54 94
    DW  3157    ;55 95
    DW  3183    ;56 96
    DW  3209    ;57 97
    DW  3234    ;58 98
    DW  3259    ;59 99
    DW  3283    ;60 100
    DW  3306    ;61 101
    DW  3328    ;62 102
    DW  3351    ;63 103
    DW  3372    ;64 104
    DW  3393    ;65 105
    DW  3413    ;66 106
    DW  3432    ;67 107
    DW  3452    ;68 108
    DW  3470    ;69 109
    DW  3488    ;70 110
    DW  3506    ;71 111
    DW  3523    ;72 112
    DW  3539    ;73 113
    DW  3555    ;74 114
    DW  3571    ;75 115
    DW  3586    ;76 116
    DW  3601    ;77 117
    DW  3615    ;78 118
    DW  3628    ;79 119
    DW  3642    ;80 120
    DW  3655    ;81 121
    DW  3667    ;82 122
    DW  3679    ;83 123
    DW  3691    ;84 124
    DW  3702    ;85 125
    DW  3714    ;86 126
    DW  3724    ;87 127
    DW  3735    ;88 128
    DW  3745    ;89 129
    DW  3754    ;90 130
    DW  3764    ;91 131
    DW  3773    ;92 132
    DW  3782    ;93 133
    DW  3791    ;94 134
    DW  3799    ;95 135
    DW  3807    ;96 136
    DW  3815    ;97 137
    DW  3822    ;98 138
    DW  3830    ;99 139
    DW  3837    ;100    140
    DW  3844    ;101    141
    DW  3850    ;102    142
    DW  3857    ;103    143
    DW  3863    ;104    144
    DW  3869    ;105    145
    DW  3875    ;106    146
    DW  3881    ;107    147
    DW  3887    ;108    148
    DW  3892    ;109    149
    DW  3897    ;110    150
    DW  3902    ;111    151
    DW  3907    ;112    152
    DW  3912    ;113    153
    DW  3917    ;114    154
    DW  3921    ;115    155
    DW  3926    ;116    156
    DW  3930    ;117    157
    DW  3934    ;118    158
    DW  3938    ;119    159
    DW  3942    ;120    160

; ��ȡ���ݱ���˫�ֽ����ݵ��׵�ַ
; ����: ACC
; ���: DPTR.   DPTR = #temp_table + ACC * 2

F_GetFirstAddress:  ;DPTR = #temp_table + ACC * 2
    MOV     DPTR, #temp_table
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


;/********************  �����¶� ***********************************************/
;// ������: 0��Ӧ-40.0��, 400��Ӧ0��, 625��Ӧ25.0��, ���1600��Ӧ120.0��. 
;// Ϊ��ͨ��, ADC����Ϊ12bit��ADCֵ.
;// ��·������㷨���: Coody
;/**********************************************/

D_SCALE     EQU     10      //����Ŵ���, �Ŵ�10�����Ǳ���һλС��
F_get_temperature:    ;(R6 R7)Ϊ12λADC,  �����¶�Ϊ(R6 R7), 0��Ӧ-40.0��, 400��Ӧ0��, 625��Ӧ25.0��, ���1600��Ӧ120.0��.

    MOV     WR4, #4096      ;adc = 4096 - adc;  //Rt�ӵ�ʱҪ���������
    SUB     WR4, WR6
    MOV     WR6, WR4

    MOV     WR8, #temp_table
    MOV     WR14, WR8
    MOV     WR12, #00FFH
    MOV     WR10, @DR12
    CMP     WR10, WR6
    JLE     L_MoreThanTable_0   ; adc >= table[0], jmp
    MOV     WR6, #0FFFEH
    RET

L_MoreThanTable_0:
    MOV     WR14, WR8
    ADD     WR14, #320           ; 160 * 2 
    MOV     WR12, #00FFH
    MOV     WR10, @DR12
    CMP     WR10, WR6
    JNC     L_LessThanTable_160   ; adc <= table[160], jmp
    MOV     WR6, #0FFFFH
    RET
L_LessThanTable_160:
    
    MOV     minTemp, #0     ; -40��
    MOV     maxTemp, #160   ; 120��

    MOV     R0, #5
L_NTC_CheckTableLoop1:  ;�Էֲ��
    MOV     A, minTemp
    CLR     C
    PUSH    ACC
    MOV     A, maxTemp
    CLR     C
    MOV     R1, A
    POP     ACC
    ADD     A, R1
    MOV     R1, A   ;R1 = minTemp + maxTemp

    MOVZ    WR14, R1
    ADD     WR14, #temp_table
    MOV     WR12, #00FFH
    MOV     WR10, @DR12
    CMP     WR10, WR6
    JC      L_MoreThanTable_R1  ; adc > table[R1], jmp
    MOV     maxTemp, R1             ; if(adc <= table[R1])  maxTemp = R1
    SJMP    L_NTC_CheckLoopEnd
L_MoreThanTable_R1:
    MOV     minTemp, R1             ; if(adc > table[R1])   minTemp = R1
L_NTC_CheckLoopEnd:
    DJNZ    R0, L_NTC_CheckTableLoop1

L_CheckEQU_Min:     ;����ǹ��պ� adc == table[minTemp]
    MOVZ    WR14, R1
    ADD     WR14, #temp_table
    MOV     WR12, #00FFH
    MOV     WR10, @DR12
    CMP     WR10, WR6
    JNE     L_CheckEQU_Max
    MOV     A, minTemp
    MOV     R7, #D_SCALE
    MUL     R7, R11         ;if(adc == table[minTemp])  (R6 R7) = minTemp * D_SCALE
    RET
L_QuitCheckEQU_Min:

L_CheckEQU_Max:     ;����ǹ��պ� adc == table[maxTemp]
    MOV     R1, maxTemp
    MOVZ    WR14, R1
    ADD     WR14, #temp_table
    MOV     WR12, #00FFH
    MOV     WR10, @DR12
    CMP     WR10, WR6
    JNE     L_QuitCheckEQU_Max
    MOV     A, maxTemp
    MOV     R7, #D_SCALE
    MUL     R7, R11         ;if(adc == table[maxTemp])  (R6 R7) = maxTemp * D_SCALE
    RET
L_QuitCheckEQU_Max:


L_Calculate_Loop:           ; table[minTemp] < adc < table[maxTemp], ����С������, ���Բ岹
    MOV     A, minTemp
    SETB    C
    SUBB    A, maxTemp          ; minTemp - maxTemp - 1
    JC      L_Calculate_1   ; while(minTemp <= maxTemp)
    RET
L_Calculate_1:
    INC     minTemp     ; minTemp++
    MOV     A, minTemp
    LCALL   F_GetFirstAddress   ; DPTR = #temp_table + ACC * 2
    MOV     A, #1
    MOVC    A, @A+DPTR
    CJNE    A, AR7, L_AdcNotEquTableMin     ; AR7 == R7
    CLR     A
    MOVC    A, @A+DPTR
    CJNE    A, AR6, L_AdcNotEquTableMin     ; AR6 == R6

    MOV     A, minTemp
    MOV     R7, #D_SCALE
    MUL     R7, R11         ;if(adc == table[minTemp])  (R6 R7) = minTemp * D_SCALE
    RET

L_AdcNotEquTableMin:    ; adc != table[minTemp]
    MOV     A, #1
    SETB    C
    MOVC    A, @A+DPTR
    SUBB    A, R7
    CLR     A
    MOVC    A, @A+DPTR
    SUBB    A, R6
    JC      L_Calculate_Loop
                            ;   if(adc < table[minTemp])
    DEC     minTemp     ;minTemp--
    MOV     A, minTemp
    LCALL   F_GetFirstAddress   ; DPTR = #temp_table + ACC * 2
    CLR     A
    MOVC    A, @A+DPTR
    MOV     R0, A        ; (R0 R1) = table[minTemp]
    MOV     A, #1
    MOVC    A, @A+DPTR
    MOV     R1, A

    MOV     A, minTemp
    INC     A
    LCALL   F_GetFirstAddress   ; DPTR = #temp_table + ACC * 2
    MOV     A, #1
    MOVC    A, @A+DPTR
    CLR     C
    SUBB    A, R1        ; (R4 R5) = table[minTemp+1] - table[minTemp]
    MOV     R5, A
    CLR     A
    MOVC    A, @A+DPTR
    SUBB    A, R0
    MOV     R4, A
    
    SUB     WR6, WR0     ; (R0 R1) = adc - table[minTemp]
    MOV     R1, #D_SCALE
    MUL     R7, R1       ;(R0 R1) = (adc - table[minTemp]) * D_SCALE

    DIV     WR6, WR4     ;(R6,R7)/R5 = (R6,R7)
                         ;(R6 R7) = ((adc - table[minTemp]) * D_SCALE) / (table[minTemp+1] - table[minTemp])
    MOV     A, minTemp
    MOV     B, #D_SCALE
    MUL     AB           ;(B ACC) = minTemp * D_SCALE
    
    ADD     WR6, WR10    ;(R6 R7) = (minTemp * D_SCALE) + R7
    RET


    END

