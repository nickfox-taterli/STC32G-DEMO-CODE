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

;用STC的MCU的IO方式驱动8位数码管。

;使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

;通过硬件I2C接口读取AT24C02前8个字节数据，并在数码管显示.

;将读取的数据加1后写回AT24C02前8个字节.

;重新读取AT24C02前8个字节数据，并在数码管显示.

;MCU上电后执行1次以上动作，可重复断电/上电测试AT24C02前8个字节的数据内容.

;下载时, 选择时钟 24MHZ (用户可自行修改频率).

;******************************************/

$include (../../comm/STC32G.INC)

;/****************************** 用户定义宏 ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;堆栈开始地址

Timer0_Reload   EQU     (65536 - Fosc_KHZ)  ; Timer 0 中断频率, 1000次/秒

DIS_DOT         EQU     020H
DIS_BLACK       EQU     010H
DIS_            EQU     011H

;*******************************************************************

SLAW    EQU     0xA2
SLAR    EQU     0xA3

;*******************************************************************
;*************  IO口定义    **************/


;*************  本地变量声明    **************/
Flag0           DATA    20H
B_1ms           BIT     Flag0.0 ; 1ms标志

display_index   DATA    21H     ; 显示位索引
LED8            DATA    30H     ; 显示缓冲 30H ~ 37H
EEPROM           DATA   38H     ; 存储缓冲 38H ~ 3FH


;*******************************************************************
;*******************************************************************
        ORG     0000H               ;程序复位入口，编译器自动定义到 0FF0000H 地址
        LJMP    F_Main


        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt


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
    MOV     PSW, #0     ;选择第0组R0~R7
    USING   0       ;选择第0组R0~R7

;================= 用户初始化程序 ====================================

    MOV     display_index, #0
    MOV     R0, #LED8
    MOV     R2, #8
L_ClearLoop:
    MOV     @R0, #DIS_BLACK     ;上电消隐
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
    SETB    EA          ; 打开总中断

    LCALL   F_ReadEEP   ;读24Cxx
    LCALL   F_WriteEEP  ;写24Cxx
    MOV     A, #250
	LCALL   F_delay_ms
    MOV     A, #250
	LCALL   F_delay_ms
    LCALL   F_ReadEEP   ;读24Cxx

;=================== 主循环 ==================================
L_Main_Loop:

    LJMP    L_Main_Loop

;**********************************************/

;========================================================================
; 函数: F_I2C_Init
; 描述: I2C初始化程序.
; 参数: none
; 返回: none.
; 版本: V1.0, 2021-3-4
;========================================================================
F_I2C_Init:
    ORL     P_SW2, #010H        ;I2C功能脚选择，00H:P1.5,P1.4; 10H:P2.5,P2.4; 30H:P3.2,P3.3

    MOV     A, #0E0H            ;使能I2C主机模式
    MOV     WR6, #WORD0 I2CCFG
    MOV     WR4, #WORD2 I2CCFG
    MOV     @DR4, R11

    MOV     A, #00H
    MOV     WR6, #WORD0 I2CMSST
    MOV     WR4, #WORD2 I2CMSST
    MOV     @DR4, R11
    RET


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
    PUSH    00H     ;R0 入栈
    
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
    POP     00H     ;R0 出栈
    POP     DPL     ;DPL出栈
    POP     DPH     ;DPH出栈
    RET


;*******************************************************************
;**************** 中断函数 ***************************************************

F_Timer0_Interrupt: ;Timer0 1ms中断函数
    PUSH    PSW     ;PSW入栈
    PUSH    ACC     ;ACC入栈

    LCALL   F_DisplayScan   ; 1ms扫描显示一位
    SETB    B_1ms           ; 1ms标志

    POP     ACC     ;ACC出栈
    POP     PSW     ;PSW出栈
    RETI
    

;************ I2C相关函数 ****************
;========================================================================
; 函数: Wait
; 描述: I2C访问延时.
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
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
; 函数: Start
; 描述: 启动I2C. 
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
;========================================================================
Start:
    MOV     A, #01H
    MOV     WR6, #WORD0 I2CMSCR  ;发送START命令
    MOV     WR4, #WORD2 I2CMSCR
    MOV     @DR4, R11
    LCALL   Wait
    RET


;========================================================================
; 函数: Stop
; 描述: 停止I2C. 
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
;========================================================================
Stop:
    MOV     A, #06H
    MOV     WR6, #WORD0 I2CMSCR  ;发送STOP命令
    MOV     WR4, #WORD2 I2CMSCR
    MOV     @DR4, R11
    LCALL   Wait
    RET

;========================================================================
; 函数: SendData
; 描述: 发送数据.
; 参数: I2CTXD -> A.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
;========================================================================
SendData:
	MOV     WR6,#WORD0 I2CTXD   ;写数据到数据缓冲区
	MOV     WR4,#WORD2 I2CTXD
	MOV     @DR4,R11
	MOV     A,#00000010B        ;发送SEND命令
	MOV     WR6,#WORD0 I2CMSCR
	MOV     WR4,#WORD2 I2CMSCR
	MOV     @DR4,R11
    LCALL   Wait
    RET

;========================================================================
; 函数: RecvACK
; 描述: 检测应答.
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
;========================================================================
RecvACK:
    MOV     A,#03H
	MOV     WR6,#WORD0 I2CMSCR  ;发送读ACK命令
	MOV     WR4,#WORD2 I2CMSCR
	MOV     @DR4,R11
    LCALL   Wait
    CLR     C
	MOV     WR6,#WORD0 I2CMSST  ;发送读ACK命令
	MOV     WR4,#WORD2 I2CMSST
	MOV     R11,@DR4
    JNB     ACC.1,$+4
    SETB    C
    RET
        
;========================================================================
; 函数: RecvData
; 描述: 接收数据.
; 参数: none.
; 返回: I2CRXD -> A.
; 版本: VER1.0
; 日期: 2013-4-1
;========================================================================
RecvData:
    MOV     A,#04H
	MOV     WR6,#WORD0 I2CMSCR  ;发送RECV命令
	MOV     WR4,#WORD2 I2CMSCR
	MOV     @DR4,R11
    LCALL   Wait
	MOV     WR6,#WORD0 I2CRXD
	MOV     WR4,#WORD2 I2CRXD
	MOV     R11,@DR4	;从数据缓冲区读取数据
    RET

;========================================================================
; 函数: SendACK
; 描述: 发送应答.
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
;========================================================================
SendACK:
    MOV     A,#00H
	MOV     WR6,#WORD0 I2CMSST  ;设置ACK信号
	MOV     WR4,#WORD2 I2CMSST
	MOV     @DR4,R11
    MOV     A,#05H
	MOV     WR6,#WORD0 I2CMSCR  ;发送ACK命令
	MOV     WR4,#WORD2 I2CMSCR
	MOV     @DR4,R11
    LCALL   Wait
    RET

;========================================================================
; 函数: SendNAK
; 描述: 发送非应答.
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
;========================================================================
SendNAK:
    MOV     A,#01H
	MOV     WR6,#WORD0 I2CMSST  ;设置NAK信号
	MOV     WR4,#WORD2 I2CMSST
	MOV     @DR4,R11
    MOV     A,#05H
	MOV     WR6,#WORD0 I2CMSCR  ;发送ACK命令
	MOV     WR4,#WORD2 I2CMSCR
	MOV     @DR4,R11
    LCALL   Wait
    RET

;========================================================================
; 函数: F_WriteNbyte
; 描述: 写N个字节子程序。
; 参数: R2: 写I2C数据首地址,  R0: 写入数据存放首地址,  R3: 写入字节数
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
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
; 函数: F_ReadNbyte
; 描述: 读N个字节子程序。
; 参数: R2: 读I2C数据首地址,  R0: 读出数据存放首地址,  R3: 读出字节数
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
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
    ANL     A, #0xfe    ;判断是否大于1
    JZ      L_ReadLastByte
    DEC     R3          ;大于1字节, 则-1
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
; 函数: F_ReadEEP
; 描述: 读24Cxx函数。
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 
;========================================================================
F_ReadEEP:

    MOV     R2, #0      ; 读I2C数据首地址
    MOV     R0, #EEPROM ; 读出数据存放首地址
    MOV     R3, #8      ; 读出字节数
    LCALL   F_ReadNbyte ; R2: 读I2C数据首地址,  R0: 读出数据存放首地址,  R3: 读出字节数

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
; 函数: F_WriteEEP
; 描述: 写24Cxx函数。
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 
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

    MOV     R2, #0      ;写I2C数据首地址
    MOV     R0, #EEPROM ;写入数据存放首地址
    MOV     R3, #8      ;写入字节数
    LCALL   F_WriteNbyte    ;
    RET

;========================================================================
; 函数: F_delay_ms
; 描述: 延时子程序。
; 参数: ACC: 延时ms数.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
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



    END

