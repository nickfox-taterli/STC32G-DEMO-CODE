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

;/************* 功能说明    **************

;本例程基于STC32G为主控芯片的实验箱进行编写测试。

;使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

;edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

;用户可以在宏定义中改变MCU主时钟频率. 范围 8MHZ ~ 33MHZ.

;红外发射程序。模拟市场上用量最大的NEC的编码。

;用户可以在宏定义中指定用户码.

;使用PWM4产生38KHZ载波, 1/3占空比, 每个38KHZ周期发射管发射9us,关闭16.3us.

;使用开发板上的16个IO扫描按键, MCU不睡眠, 连续扫描按键.

;当键按下, 第一帧为数据, 后面的帧为重复帧,不带数据, 具体定义请自行参考NEC的编码资料.

;键释放后, 停止发送.

;******************************************/

$include (../../comm/STC32G.INC)

;/****************************** 用户定义宏 ***********************************/

STACK_POIRTER   EQU     0D0H    ;堆栈开始地址
Fosc_KHZ        EQU     24000   ;24000KHZ

;*******************************************************************
;*******************************************************************


;*************  IO口定义    **************/

P_IR_TX   BIT P2.7  ;定义红外发送端口

;*************  本地变量声明    **************/

IO_KeyState     DATA    30H ; IO行列键状态变量
IO_KeyState1    DATA    31H
IO_KeyHoldCnt   DATA    32H ; IO键按下计时
KeyCode         DATA    33H ; 给用户使用的键码, 1~16为ADC键， 17~32为IO键


/*************  红外发送相关变量    **************/
#define User_code   0xFF00      //定义红外用户码

FLAG0   DATA    0x20
B_Space BIT FLAG0.0 ;发送空闲(延时)标志

tx_cnt      DATA    0x36    ;发送或空闲的脉冲计数(等于38KHZ的脉冲数，对应时间), 红外频率为38KHZ, 周期26.3us
TxTime      DATA    0x38    ;发送时间


;*******************************************************************
;*******************************************************************
        ORG     0000H               ;程序复位入口，编译器自动定义到 0FF0000H 地址
        LJMP    F_Main

        ORG     00D3H               ;26  PWMA interrupt
        LJMP    F_PWMA_Interrupt


;******************** 主程序 **************************/
        ORG     0100H       ;编译器自动定义到 0FF0100H 地址
F_Main:
    MOV     WTST, #00H     ;设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    MOV     CKCON,#00H     ;提高访问XRAM速度
    ORL     P_SW2,#080H    ;使能访问XFR

    MOV     P0M1, #30H     ;设置P0.4、P0.5为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P0M0, #30H
    MOV     P1M1, #30H     ;设置P1.4、P1.5为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P1M0, #30H
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
    MOV     PSW, #0
    USING   0       ;选择第0组R0~R7

;================= 用户初始化程序 ====================================

    LCALL   F_PWM_Init      ;初始化PWM
    SETB    P_IR_TX
    
    SETB    EA              ;打开总中断
    
    MOV     KeyCode, #0

;=================== 主循环 ==================================
L_Main_Loop:
    MOV     A, #30
    LCALL   F_delay_ms      ;延时30ms
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

    MOV     A, TxTime
    CMP     R11, #56      ;一帧按最大77ms发送, 不够的话,补偿时间      108ms
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

                            ;键未释放, 发送重复帧(无数据)
    MOV     WR6, #342       ;对应9ms，同步头        9ms
    LCALL   F_IR_TxPulse

    MOV     WR6, #86        ;对应2.25ms，同步头间隔 2.25ms
    LCALL   F_IR_TxSpace

    MOV     WR6, #21        ;发送脉冲           0.5625ms
    LCALL   F_IR_TxPulse

    MOV     A, #96          ;delay_ms(96)
    LCALL   F_delay_ms
    LCALL   F_IO_KeyScan    ;扫描键盘
    SJMP    L_WaitKeyRelease

L_ClearKeyCode:
    MOV     KeyCode, #0

    LJMP    L_Main_Loop
;===================================================================


;========================================================================
; 函数: F_delay_ms
; 描述: 延时子程序。
; 参数: ACC: 延时ms数.
; 返回: none.
; 版本: VER1.0
; 日期: 2021-3-16
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈保护, 退出时恢复原来数据不改变.
;========================================================================
F_delay_ms:
    PUSH    02H     ;入栈R2
    PUSH    03H     ;入栈R3
    PUSH    04H     ;入栈R4

    MOV     R4,A
L_delay_ms_1:
    MOV     WR2, #(Fosc_KHZ / 4)
L_delay_ms_2:
    DEC     WR2, #1         ;1T
    JNE     L_delay_ms_2    ;3T
    DJNZ    R4, L_delay_ms_1

    POP     04H     ;出栈R2
    POP     03H     ;出栈R3
    POP     02H     ;出栈R4
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

