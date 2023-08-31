;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- STC 1T Series MCU Demo Programme -------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
;/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/* --- Web: www.STCAIMCU.com ------------------------------------------*/
;/* --- QQ:  800003751 -------------------------------------------------*/
;/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
;/*---------------------------------------------------------------------*/


;*************  功能说明    **************

;本例程基于STC32G为主控芯片的实验箱进行编写测试。

;使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

;edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

;红外收发程序。适用于市场上用量最大的NEC编码。

;应用层查询 B_IR_Press标志为,则已接收到一个键码放在IR_code中, 处理完键码后， 用户程序清除B_IR_Press标志.

;数码管左起4位显示用户码, 最右边两位显示数据, 均为十六进制.

;用户可以在宏定义中指定用户码.

;用户底层程序按固定的时间间隔(60~125us)调用 "IR_RX_NEC()"函数.

;按下IO行列键（不支持ADC键盘），显示发送、接收到的键值。

;下载时, 选择时钟 24MHz (用户可自行修改频率).

;******************************************/

$include (../../comm/STC32G.INC)

;/****************************** 用户定义宏 ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHZ, 用户只需要改动这个值以适应自己实际的频率

STACK_POIRTER   EQU     0D0H    ;堆栈开始地址

Timer0_Reload   EQU     (65536 - (Fosc_KHZ/10))   ; Timer 0 中断频率, 10000次/秒

DIS_DOT         EQU     020H
DIS_BLACK       EQU     010H
DIS_            EQU     011H

;*******************************************************************
;*******************************************************************


;*************  本地变量声明    **************/
Flag0           DATA    20H
B_1ms           BIT     Flag0.0     ;1ms标志
P_IR_RX_temp    BIT     Flag0.1     ;用户不可操作, Last sample
B_IR_Sync       BIT     Flag0.2     ;用户不可操作, 已收到同步标志
B_Space         BIT     Flag0.3     ;发送空闲(延时)标志
B_IR_Press      BIT     Flag0.4     ;用户使用, 按键动作发生

LED8            DATA    30H     ;   显示缓冲 30H ~ 37H
display_index   DATA    38H     ;   显示位索引

cnt_1ms         DATA    39H     ;

;*************  红外接收程序变量声明    **************
#define User_code   0xFF00      //定义红外用户码

P_IR_TX         BIT P2.7    ;定义红外发送端口
P_IR_RX         BIT P3.5    ;定义红外接收输入IO口

IR_SampleCnt    DATA    3AH ;用户不可操作, 采样计数
IR_BitCnt       DATA    3BH ;用户不可操作, 编码位数
IR_UserH        DATA    3CH ;用户不可操作, 用户码(地址)高字节
IR_UserL        DATA    3DH ;用户不可操作, 用户码(地址)低字节
IR_data         DATA    3EH ;用户不可操作, 数据原码
IR_DataShit     DATA    3FH ;用户不可操作, 数据移位

IR_code         DATA    40H ;用户使用, 红外键码
UserCodeH       DATA    41H ;用户使用, 用户码高字节
UserCodeL       DATA    42H ;用户使用, 用户码低字节
msecond         DATA    43H

IO_KeyState     DATA    44H ; IO行列键状态变量
IO_KeyState1    DATA    45H
IO_KeyHoldCnt   DATA    46H ; IO键按下计时
KeyCode         DATA    47H ; 给用户使用的键码, 1~16为ADC键， 17~32为IO键

/*************  红外发送相关变量    **************/
tx_cnt          DATA    48H ;发送或空闲的脉冲计数(等于38KHZ的脉冲数，对应时间), 红外频率为38KHZ, 周期26.3us
TxTime          DATA    4AH ;发送时间


;*******************************************************************
;*******************************************************************
        ORG     0000H               ;程序复位入口，编译器自动定义到 0FF0000H 地址
        LJMP    F_Main

        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt

        ORG     00D3H               ;26  PWMA interrupt
        LJMP    F_PWMA_Interrupt

;*******************************************************************
;*******************************************************************


;******************** 主程序 **************************/
        ORG     0100H       ;编译器自动定义到 0FF0100H 地址
F_Main:
    MOV     WTST, #00H     ;设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    MOV     CKCON,#00H     ;提高访问XRAM速度
    ORL     P_SW2,#080H    ;使能访问XFR

    MOV     P0M1, #30H     ;设置P0.4、P0.5为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P0M0, #30H
    MOV     P1M1, #32H     ;设置P1.4、P1.5为漏极开路(实验箱加了上拉电阻到3.3V), P1.1在PWM当DAC电路通过电阻串联到P2.3
    MOV     P1M0, #32H
    MOV     P2M1, #3cH     ;设置P2.2~P2.5为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P2M0, #3cH
    MOV     P3M1, #50H     ;设置P3.4、P3.6为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P3M0, #50H
    MOV     P4M1, #3cH     ;设置P4.2~P4.5为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P4M0, #3cH
    MOV     P5M1, #0cH     ;设置P5.2、P5.3为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P5M0, #0cH
    MOV     P6M1, #0ffH    ;设置为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P6M0, #0ffH
    MOV     P7M1, #00H     ;设置为准双向口
    MOV     P7M0, #00H

    MOV     SP, #STACK_POIRTER
    MOV     PSW, #0     ;选择第0组R0~R7
    USING   0       ;选择第0组R0~R7

;================= 用户初始化程序 ====================================

    MOV     display_index, #0
    MOV     R0, #LED8
    MOV     R2, #8
L_ClearLoop:
    MOV     @R0, #DIS_
    INC     R0
    DJNZ    R2, L_ClearLoop

    MOV     R0, #LED8+2
    MOV     @R0, #DIS_BLACK     ;上电消隐
    INC     R0
    MOV     @R0, #DIS_BLACK     ;上电消隐
    
    CLR     TR0
    ORL     AUXR, #(1 SHL 7)    ; Timer0_1T();
    ANL     TMOD, #NOT 04H      ; Timer0_AsTimer();
    ANL     TMOD, #NOT 03H      ; Timer0_16bitAutoReload();
    MOV     TH0, #Timer0_Reload / 256   ;Timer0_Load(Timer0_Reload);
    MOV     TL0, #Timer0_Reload MOD 256
    SETB    ET0         ; Timer0_InterruptEnable();
    SETB    TR0         ; Timer0_Run();
    
    LCALL   F_PWM_Init      ;初始化PWM
    SETB    P_IR_TX
    SETB    EA              ;打开总中断
    
    MOV     KeyCode, #0
    MOV     cnt_1ms, #10

;=====================================================

;=====================================================
L_Main_Loop:

    JNB     B_1ms,  L_Main_Loop     ;1ms未到
    CLR     B_1ms
    
    JNB     B_IR_Press, L_Main_KeyScan ;未检测到收到红外键码

    CLR     B_IR_Press      ;检测到收到红外键码
    MOV     A, UserCodeH
    SWAP    A
    ANL     A, #0FH
    MOV     LED8+7, A       ;用户码高字节的高半字节
    MOV     A, UserCodeH
    ANL     A, #0FH
    MOV     LED8+6, A       ;用户码高字节的低半字节

    MOV     A, UserCodeL
    SWAP    A
    ANL     A, #0FH
    MOV     LED8+5, A       ;用户码低字节的高半字节
    MOV     A, UserCodeL
    ANL     A, #0FH
    MOV     LED8+4, A       ;用户码低字节的低半字节
    
    MOV     A, IR_code
    SWAP    A
    ANL     A, #0FH
    MOV     LED8+1, A       ;遥控数据的高半字节
    MOV     A, IR_code
    ANL     A, #0FH
    MOV     LED8+0, A       ;遥控数据的低半字节

L_Main_KeyScan:
;=================== 检测30ms是否到 ==================================
    INC     msecond       ;msecond + 1
    CLR     C
    MOV     A, msecond    ;msecond - 30
    SUBB    A, #30
    JC      L_Main_Loop     ;if(msecond < 30), jmp
    
;================= 30ms到 ====================================
    MOV     msecond, #0

    LCALL   F_IO_KeyScan    ;扫描键盘

    MOV     A, KeyCode
    JZ      L_Main_Loop     ;无键循环
    MOV     TxTime, #0      ;
                            ;一帧数据最小长度 = 9 + 4.5 + 0.5625 + 24 * 1.125 + 8 * 2.25 = 59.0625 ms
                            ;一帧数据最大长度 = 9 + 4.5 + 0.5625 + 8 * 1.125 + 24 * 2.25 = 77.0625 ms
    MOV     WR6, #342       ;对应9ms，同步头        9ms
    LCALL   F_IR_TxPulse

    MOV     WR6, #171       ;对应4.5ms，同步头间隔  4.5ms
    LCALL   F_IR_TxSpace

    MOV     WR6, #21        ;发送脉冲           0.5625ms
    LCALL   F_IR_TxPulse

    MOV     A, #LOW  User_code  ;发用户码低字节
    LCALL   F_IR_TxByte
    MOV     A, #HIGH User_code  ;发用户码高字节
    LCALL   F_IR_TxByte
    MOV     A, KeyCode          ;发数据
    LCALL   F_IR_TxByte
    MOV     A, KeyCode          ;发数据反码
    CPL     A
    LCALL   F_IR_TxByte

L_ClearKeyCode:
    MOV KeyCode, #0

    LJMP    L_Main_Loop

;**********************************************/


; *********************** 显示相关程序 ****************************************
T_Display:                      ;标准字库
;    0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
DB  03FH,006H,05BH,04FH,066H,06DH,07DH,007H,07FH,06FH,077H,07CH,039H,05EH,079H,071H
;  black  -    H    J    K    L    N    o    P    U    t    G    Q    r    M    y
DB  000H,040H,076H,01EH,070H,038H,037H,05CH,073H,03EH,078H,03dH,067H,050H,037H,06EH
;    0.   1.   2.   3.   4.   5.   6.   7.   8.   9.   -1
DB  0BFH,086H,0DBH,0CFH,0E6H,0EDH,0FDH,087H,0FFH,0EFH,046H

T_COM:
DB  001H,002H,004H,008H,010H,020H,040H,080H     ;   位码


;//========================================================================
;// 函数: F_DisplayScan
;// 描述: 显示扫描子程序。
;// 参数: none.
;// 返回: none.
;// 版本: VER1.0
;// 日期: 2013-4-1
;// 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;//========================================================================
F_DisplayScan:
    PUSH    DPH     ;DPH入栈
    PUSH    DPL     ;DPL入栈
    PUSH    AR0     ;R0 入栈
    
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
    MOV     display_index, #0;  ;8位结束回0
L_QuitDisplayScan:
    POP     AR0     ;R0 出栈
    POP     DPL     ;DPL出栈
    POP     DPH     ;DPH出栈
    RET


;*******************************************************************
;**************** 中断函数 ***************************************************

F_Timer0_Interrupt: ;Timer0 1ms中断函数
    PUSH    PSW     ;PSW入栈
    PUSH    ACC     ;ACC入栈
    PUSH    AR7     ;SampleTime

    LCALL   F_IR_RX_NEC

    DJNZ    cnt_1ms, L_Quit_1ms
    MOV     cnt_1ms, #10
    LCALL   F_DisplayScan   ; 1ms扫描显示一位
    SETB    B_1ms           ; 1ms标志
L_Quit_1ms:

    POP     AR7
    POP     ACC     ;ACC出栈
    POP     PSW     ;PSW出栈
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

;******************** 红外采样时间宏定义, 用户不要随意修改  *******************

D_IR_sample         EQU 100                 ;查询时间间隔, 100us, 红外接收要求在60us~250us之间
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
    MOV     A, IR_DataShit      ;if(~IR_DataShit == IR_data)        //判断数据正反码
    CPL     A
    XRL     A, IR_data
    JNZ     L_QuitIrRx
    
    MOV     UserCodeH, IR_UserH
    MOV     UserCodeL, IR_UserL
    MOV     IR_code, IR_data
    SETB    B_IR_Press          ;数据有效
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
; 函数: F_IO_KeyDelay
; 描述: 行列键扫描程序.
; 参数: none
; 返回: 读到按键, KeyCode为非0键码.
; 版本: V1.0, 2013-11-22
;========================================================================
;/*****************************************************
;   行列键扫描程序
;   使用XY查找4x4键的方法，只能单键，速度快
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
    PUSH    03H     ;R3入栈
    MOV     R3, #60
    DJNZ    R3, $   ; (n * 4) T
    POP     03H     ;R3出栈
    RET

F_IO_KeyScan:
    PUSH    06H     ;R6入栈
    PUSH    07H     ;R7入栈

    MOV     R6, IO_KeyState1    ; 保存上一次状态

    MOV     P0, #0F0H       ;X低，读Y
    LCALL   F_IO_KeyDelay       ;delay about 250T
    MOV     A, P0
    ANL     A, #0F0H
    MOV     IO_KeyState1, A     ; IO_KeyState1 = P0 & 0xf0

    MOV     P0, #0FH        ;Y低，读X
    LCALL   F_IO_KeyDelay       ;delay about 250T
    MOV     A, P0
    ANL     A, #0FH
    ORL     A, IO_KeyState1         ; IO_KeyState1 |= (P0 & 0x0f)
    MOV     IO_KeyState1, A
    XRL     IO_KeyState1, #0FFH     ; IO_KeyState1 ^= 0xff; 取反

    MOV     A, R6                   ;if(j == IO_KeyState1), 连续两次读相等
    CJNE    A, IO_KeyState1, L_QuitCheckIoKey   ;不相等, jmp
    
    MOV     R6, IO_KeyState     ;暂存IO_KeyState
    MOV     IO_KeyState, IO_KeyState1
    MOV     A, IO_KeyState
    JZ      L_NoIoKeyPress      ; if(IO_KeyState != 0), 有键按下

    MOV     A, R6   
    JZ      L_CalculateIoKey    ;if(R6 == 0)    F0 = 1; 第一次按下
    MOV     A, R6   
    CJNE    A, IO_KeyState, L_QuitCheckIoKey    ; if(j != IO_KeyState), jmp
    
    INC     IO_KeyHoldCnt   ; if(++IO_KeyHoldCnt >= 20),    1秒后重键
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
    
    MOV     A, R6       ;KeyCode = (j - 1) * 4 + T_KeyTable[IO_KeyState & 0x0f] + 16;   //计算键码，17~32
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
    POP     07H     ;R7出栈
    POP     06H     ;R6出栈
    RET

;========================================================================
; 函数: F_IR_TxPulse
; 描述: 发送脉冲函数.
; 参数: tx_cntH, tx_cntL: 要发送的38K周期数
; 返回: none.
; 版本: V1.0, 2013-11-22
;========================================================================
F_IR_TxPulse:
    MOV     tx_cnt, WR6
    CLR     B_Space

    MOV     A, #00                 ;写 CCMRx 前必须先清零 CCxE 关闭通道
    MOV     WR6, #WORD0 PWMA_CCER2
    MOV     WR4, #WORD2 PWMA_CCER2
    MOV     @DR4, R11
    MOV     A, #068H               ;设置 PWM4 模式1 输出
    MOV     WR6, #WORD0 PWMA_CCMR4
    MOV     WR4, #WORD2 PWMA_CCMR4
    MOV     @DR4, R11
    MOV     A, #070H               ;使能 CC4NE 通道, 低电平有效
    MOV     WR6, #WORD0 PWMA_CCER2
    MOV     WR4, #WORD2 PWMA_CCER2
    MOV     @DR4, R11
    MOV     A, #010H               ;使能捕获/比较 4 中断
    MOV     WR6, #WORD0 PWMA_IER
    MOV     WR4, #WORD2 PWMA_IER
    MOV     @DR4, R11

    JNB     B_Space, $   ;等待结束
    RET

;========================================================================
; 函数: F_IR_TxSpace
; 描述: 发送空闲函数.
; 参数: tx_cntH, tx_cntL: 要发送的38K周期数
; 返回: none.
; 版本: V1.0, 2013-11-22
;========================================================================
F_IR_TxSpace:
    MOV     tx_cnt, WR6
    CLR     B_Space

    MOV     A, #00                 ;写 CCMRx 前必须先清零 CCxE 关闭通道
    MOV     WR6, #WORD0 PWMA_CCER2
    MOV     WR4, #WORD2 PWMA_CCER2
    MOV     @DR4, R11
    MOV     A, #040H               ;设置 PWM4 强制为无效电平
    MOV     WR6, #WORD0 PWMA_CCMR4
    MOV     WR4, #WORD2 PWMA_CCMR4
    MOV     @DR4, R11
    MOV     A, #070H               ;使能 CC4NE 通道, 低电平有效
    MOV     WR6, #WORD0 PWMA_CCER2
    MOV     WR4, #WORD2 PWMA_CCER2
    MOV     @DR4, R11
    MOV     A, #010H               ;使能捕获/比较 4 中断
    MOV     WR6, #WORD0 PWMA_IER
    MOV     WR4, #WORD2 PWMA_IER
    MOV     @DR4, R11

    JNB     B_Space, $   ;等待结束
    RET

;========================================================================
; 函数: F_IR_TxByte
; 描述: 发送一个字节函数.
; 参数: ACC: 要发送的字节
; 返回: none.
; 版本: V1.0, 2013-11-22
;========================================================================
F_IR_TxByte:
    PUSH    AR2
    PUSH    AR3

    MOV     R2, #8
    MOV     R3, A
L_IR_TxByteLoop:
    MOV     A, R3
    JNB     ACC.0, L_IR_TxByte_0
    MOV     WR6, #63        ;发送数据1
    LCALL   F_IR_TxSpace
    INC     TxTime          ;TxTime += 2;   //数据1对应 1.6875 + 0.5625 ms
    INC     TxTime
    SJMP    L_IR_TxByte_Pause
L_IR_TxByte_0:
    MOV     WR6, #21        ;发送数据0
    LCALL   F_IR_TxSpace
    INC     TxTime          ;数据0对应 0.5625 + 0.5625 ms
L_IR_TxByte_Pause:
    MOV     WR6, #21        ;发送脉冲
    LCALL   F_IR_TxPulse    ;脉冲都是0.5625ms
    MOV     A, R3
    RR      A               ;下一个位
    MOV     R3, A
    DJNZ    R2, L_IR_TxByteLoop
    POP     AR3
    POP     AR2
    
    RET

;========================================================================
; 函数: F_PWM_Init
; 描述: PWM初始化程序.
; 参数: none
; 返回: none.
; 版本: V1.0, 2013-11-22
;========================================================================
F_PWM_Init:
    MOV     A, #00H              ;写 CCMRx 前必须先清零 CCxE 关闭通道
    MOV     WR6, #WORD0 PWMA_CCER2
    MOV     WR4, #WORD2 PWMA_CCER2
    MOV     @DR4, R11
    MOV     A, #068H             ;设置 PWM4 模式1 输出
    MOV     WR6, #WORD0 PWMA_CCMR4
    MOV     WR4, #WORD2 PWMA_CCMR4
    MOV     @DR4, R11

    MOV     A, #2                ;设置周期时间
    MOV     WR6, #WORD0 PWMA_ARRH
    MOV     WR4, #WORD2 PWMA_ARRH
    MOV     @DR4, R11
    MOV     A, #077H
    MOV     WR6, #WORD0 PWMA_ARRL
    MOV     WR4, #WORD2 PWMA_ARRL
    MOV     @DR4, R11
    MOV     A, #0                ;设置占空比时间
    MOV     WR6, #WORD0 PWMA_CCR4H
    MOV     WR4, #WORD2 PWMA_CCR4H
    MOV     @DR4, R11
    MOV     A, #210
    MOV     WR6, #WORD0 PWMA_CCR4L
    MOV     WR4, #WORD2 PWMA_CCR4L
    MOV     @DR4, R11

    MOV     A, #080H             ;使能 PWM4N 输出
    MOV     WR6, #WORD0 PWMA_ENO
    MOV     WR4, #WORD2 PWMA_ENO
    MOV     @DR4, R11
    MOV     A, #040H             ;高级 PWM 通道 4N 输出脚选择位, 0x00:P1.7, 0x40:P2.7, 0x80:P6.7, 0xC0:P3.3
    MOV     WR6, #WORD0 PWMA_PS
    MOV     WR4, #WORD2 PWMA_PS
    MOV     @DR4, R11
    MOV     A, #080H             ;使能主输出
    MOV     WR6, #WORD0 PWMA_BKR
    MOV     WR4, #WORD2 PWMA_BKR
    MOV     @DR4, R11

    MOV     WR6, #WORD0 PWMA_CR1
    MOV     WR4, #WORD2 PWMA_CR1
    MOV     R11, @DR4
    ORL     A,#081H              ;使能ARR预装载，开始计时
    MOV     @DR4, R11
    RET

;========================================================================
; 函数: F_PWMA_Interrupt
; 描述: PWMA中断处理程序.
; 参数: None
; 返回: none.
; 版本: V1.0, 2012-11-22
;========================================================================
F_PWMA_Interrupt:
    PUSH    PSW
    PUSH    ACC
    PUSH    R4
    PUSH    R5
    PUSH    R6
    PUSH    R7

    MOV     WR6, #WORD0 PWMA_SR1 ;检测从机状态
    MOV     WR4, #WORD2 PWMA_SR1
    MOV     R11, @DR4
    JNB     ACC.4,F_PWMA_QuitInt
    CLR     A
    MOV     @DR4, R11

    MOV     WR6, tx_cnt
    DEC     WR6, #1       ;tx_cnt - 1
    MOV     tx_cnt, WR6
    JNE     F_PWMA_QuitInt

    MOV     A, #00                 ;写 CCMRx 前必须先清零 CCxE 关闭通道
    MOV     WR6, #WORD0 PWMA_CCER2
    MOV     WR4, #WORD2 PWMA_CCER2
    MOV     @DR4, R11
    MOV     A, #040H               ;设置 PWM4 强制为无效电平
    MOV     WR6, #WORD0 PWMA_CCMR4
    MOV     WR4, #WORD2 PWMA_CCMR4
    MOV     @DR4, R11
    MOV     A, #070H               ;使能 CC4NE 通道, 低电平有效
    MOV     WR6, #WORD0 PWMA_CCER2
    MOV     WR4, #WORD2 PWMA_CCER2
    MOV     @DR4, R11
    MOV     A, #00H                ;关闭中断
    MOV     WR6, #WORD0 PWMA_IER
    MOV     WR4, #WORD2 PWMA_IER
    MOV     @DR4, R11

    SETB    B_Space        ;设置结束标志

F_PWMA_QuitInt:
    POP     R7
    POP     R6
    POP     R5
    POP     R4
    POP     ACC
    POP     PSW
    RETI


    END

