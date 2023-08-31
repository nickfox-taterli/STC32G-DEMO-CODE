;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- STC 1T Series MCU Demo Programme -------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
;/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/* --- BBS: www.STCAIMCU.com ------------------------------------------*/
;/* --- QQ:  800003751 -------------------------------------------------*/
;/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
;/*---------------------------------------------------------------------*/


;*************  本程序功能说明  **************

;本例程基于STC32G为主控芯片的实验箱进行编写测试。

;使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

;edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

;通过串口2(P4.6 P4.7)对STC内部自带的EEPROM(FLASH)进行读写测试。

;对FLASH做扇区擦除、写入、读出的操作，命令指定地址。

;默认波特率:  115200,8,N,1. 
;默认主时钟:  22.1184MHz.

;串口命令设置: (命令字母不区分大小写)
;   W 0x000040 1234567890  --> 对0x000040地址写入字符1234567890.
;   R 0x000040 10          --> 对0x000040地址读出10个字节数据. 

;注意：下载时，下载界面"硬件选项"中设置用户EEPROM大小，

;并确保串口命令中的地址在EEPROM设置的大小范围之内。

;******************************************/

$include (../../comm/STC32G.INC)

;/****************************** 用户定义宏 ***********************************/
Fosc_KHZ    EQU 22118   ;22118KHZ

;UART2_Baudrate EQU     (-4608) ;   600bps @ 11.0592MHz
;UART2_Baudrate EQU     (-2304) ;  1200bps @ 11.0592MHz UART2_Baudrate = 65536UL - ((MAIN_Fosc / 4) / Baudrate)
;UART2_Baudrate EQU     (-1152) ;  2400bps @ 11.0592MHz
;UART2_Baudrate EQU     (-576)  ;  4800bps @ 11.0592MHz
;UART2_Baudrate EQU     (-288)  ;  9600bps @ 11.0592MHz
;UART2_Baudrate EQU     (-144)  ; 19200bps @ 11.0592MHz
;UART2_Baudrate EQU     (-72)   ; 38400bps @ 11.0592MHz
;UART2_Baudrate EQU     (-48)   ; 57600bps @ 11.0592MHz
;UART2_Baudrate EQU     (-24)   ;115200bps @ 11.0592MHz

;UART2_Baudrate EQU     (-7680) ;   600bps @ 18.432MHz
;UART2_Baudrate EQU     (-3840) ;  1200bps @ 18.432MHz
;UART2_Baudrate EQU     (-1920) ;  2400bps @ 18.432MHz
;UART2_Baudrate EQU     (-960)  ;  4800bps @ 18.432MHz
;UART2_Baudrate EQU     (-480)  ;  9600bps @ 18.432MHz
;UART2_Baudrate EQU     (-240)  ; 19200bps @ 18.432MHz
;UART2_Baudrate EQU     (-120)  ; 38400bps @ 18.432MHz
;UART2_Baudrate EQU     (-80)   ; 57600bps @ 18.432MHz
;UART2_Baudrate EQU     (-40)   ;115200bps @ 18.432MHz

;UART2_Baudrate EQU     (-9216) ;   600bps @ 22.1184MHz
;UART2_Baudrate EQU     (-4608) ;  1200bps @ 22.1184MHz
;UART2_Baudrate EQU     (-2304) ;  2400bps @ 22.1184MHz
;UART2_Baudrate EQU     (-1152) ;  4800bps @ 22.1184MHz
;UART2_Baudrate EQU     (-576)  ;  9600bps @ 22.1184MHz
;UART2_Baudrate EQU     (-288)  ; 19200bps @ 22.1184MHz
;UART2_Baudrate EQU     (-144)  ; 38400bps @ 22.1184MHz
;UART2_Baudrate EQU     (-96)   ; 57600bps @ 22.1184MHz
UART2_Baudrate  EQU     (-48)   ;115200bps @ 22.1184MHz

;UART2_Baudrate EQU     (-6912) ; 1200bps @ 33.1776MHz
;UART2_Baudrate EQU     (-3456) ; 2400bps @ 33.1776MHz
;UART2_Baudrate EQU     (-1728) ; 4800bps @ 33.1776MHz
;UART2_Baudrate EQU     (-864)  ; 9600bps @ 33.1776MHz
;UART2_Baudrate EQU     (-432)  ;19200bps @ 33.1776MHz
;UART2_Baudrate EQU     (-216)  ;38400bps @ 33.1776MHz
;UART2_Baudrate EQU     (-144)  ;57600bps @ 33.1776MHz
;UART2_Baudrate EQU     (-72)   ;115200bps @ 33.1776MHz


IAP_EN          EQU     (1 SHL 7)
IAP_SWBS        EQU     (1 SHL 6)
IAP_SWRST       EQU     (1 SHL 5)
IAP_CMD_FAIL    EQU     (1 SHL 4)

TPS_WAIT        EQU     22   ;22.1184MHZ / 1000000


RX2_Lenth   EQU     32      ; 串口接收缓冲长度

B_TX2_Busy  BIT     20H.0   ; 发送忙标志

TX2_Cnt     DATA    30H     ; 发送计数
RX2_Cnt     DATA    31H     ; 接收计数
address_E   DATA    32H
address_H   DATA    33H
address_L   DATA    34H
length      DATA    35H
RX2_TimeOut DATA    36H

RX2_Buffer  DATA    40H     ; 40 ~ 5FH 接收缓冲
tmp         DATA    60H     ; 60~7F

STACK_POIRTER   EQU     0D0H    ;堆栈开始地址



;*******************************************************************
;*******************************************************************
        ORG     0000H               ;程序复位入口，编译器自动定义到 0FF0000H 地址
        LJMP    F_Main

        ORG     0043H               ;8  UART2 interrupt
        LJMP    F_UART2_Interrupt


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
    MOV     A, #2
    LCALL   F_UART2_config  ; 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
    SETB    EA      ; 允许全局中断
    
    MOV     DPTR, #TestString1  ;Load string address to DPTR
    LCALL   F_SendString2       ;Send string
    MOV     DPTR, #TestString1A ;Load string address to DPTR
    LCALL   F_SendString2       ;Send string
    MOV     DPTR, #TestString1B ;Load string address to DPTR
    LCALL   F_SendString2       ;Send string


;=================== 主循环 ==================================
L_MainLoop:
    MOV     A, #1
    LCALL   F_delay_ms
    MOV     A, RX2_TimeOut      ;超时计数
    JZ      L_MainLoop          ;为0, 循环
    DJNZ    RX2_TimeOut, L_MainLoop ;-1,不为0循环

    MOV     A, RX2_Cnt
    CLR     C
    SUBB    A, #12
    JC      L_Jump_ProcessErr   ; RX2_Cnt < 12 bytes, jump

    MOV     R0, #RX2_Buffer
L_CheckCharLoop:                ; 小写转大写
    MOV     A, @R0
    CLR     C
    SUBB    A, #'a'
    JC      L_CheckLessThan_a   ;if(RX2_Buffer[i] < 'a'), jump
    MOV     A, @R0
    CLR     C
    SUBB    A, #('z'+1)
    JNC     L_CheckLargeThan_z  ;if(RX2_Buffer[i] > 'z'), jump
    MOV     A, @R0
    ADD     A, #('A' - 'a')     ; 小写转大写
    MOV     @R0, A
L_CheckLessThan_a:
L_CheckLargeThan_z:
    INC     R0
    CJNE    R0, #(RX2_Buffer+10), L_CheckCharLoop
    SJMP    L_CalculateAddr
    
L_Jump_ProcessErr:
    LJMP    L_Rx2ProcessErr

L_CalculateAddr:    
    LCALL   F_GetAddress            ; 计算地址
    JB      F0, L_Jump_ProcessErr   ; 地址错误

    MOV     R0, #RX2_Buffer+10
    CJNE    @R0, #' ', L_Jump_ProcessErr    ;检查命令是否正确   RX2_Buffer[10] = ' '

    MOV     R0, #RX2_Buffer
    CJNE    @R0, #'R', L_CMD_Not_R  ;检查命令是读命令   RX2_Buffer[0] = 'R'
    LCALL   F_GetReadDataLength
    JB      F0, L_Jump_ProcessErr   ;长度错误
    MOV     A, length
    JZ      L_Jump_ProcessErr   ; 长度错误
    CLR     C
    SUBB    A, #RX2_Lenth+1
    JNC     L_Jump_ProcessErr   ; 长度错误
    
    MOV     R0, #RX2_Buffer
    LCALL   F_EEPROM_read_n     ;address_H,address_L,R0,length
    MOV     DPTR, #TestString2  ;"读出"
    LCALL   F_SendString2
    MOV     A, length
    MOV     B, #100
    DIV     AB
    ADD     A, #'0'
    LCALL   F_SendByte      ;发送长度 百位
    MOV     A, #10
    XCH     A, B
    DIV     AB
    ADD     A, #'0'
    LCALL   F_SendByte      ;发送长度 十位
    XCH     A,B
    ADD     A, #'0'
    LCALL   F_SendByte      ;发送长度 个位
    MOV     DPTR, #TestString3  ;"个字节数据如下：\r\n"
    LCALL   F_SendString2
    MOV     R2, length
    MOV     R0, #RX2_Buffer
L_TxDataLoop:
    MOV     A, @R0
    LCALL   F_SendByte      ;发送数据
    INC     R0
    DJNZ    R2, L_TxDataLoop
    MOV     A, #0DH
    LCALL   F_SendByte  ;回车
    MOV     A, #0AH
    LCALL   F_SendByte  ;换行
    LJMP    L_QuitRx2Process

L_CMD_Not_R:
    MOV     R0, #RX2_Buffer
    CJNE    @R0, #'W', L_CMD_Not_W  ;检查命令是写命令   RX2_Buffer[0] = 'W'
    MOV     A, RX2_Cnt
    CLR     C
    SUBB    A, #11
    MOV     length, A
    MOV     R0, #RX2_Buffer+11
    LCALL   F_EEPROM_SectorErase    ; 擦除一个扇区
    LCALL   F_EEPROM_write_n        ; 写N个字节并校验   ;address_H,address_L,R0,length
    JB      F0, L_WriteError        ; 写入错误, 转
    MOV     DPTR, #TestString4      ; "已写入"
    LCALL   F_SendString2
    MOV     A, length
    MOV     B, #100
    DIV     AB
    ADD     A, #'0'
    LCALL   F_SendByte      ;写入长度 百位
    MOV     A, #10
    XCH     A, B
    DIV     AB
    ADD     A, #'0'
    LCALL   F_SendByte      ;写入长度 十位
    XCH     A,B
    ADD     A, #'0'
    LCALL   F_SendByte      ;写入长度 个位
    MOV     DPTR, #TestString5      ;"字节数据!\r\n"
    LCALL   F_SendString2
    SJMP    L_QuitRx2Process

L_WriteError:
    MOV     DPTR, #TestString6      ;"写入错误!\r\n"
    LCALL   F_SendString2
    SJMP    L_QuitRx2Process

L_AddrNotEmpty:
    MOV     DPTR, #TestString7      ;"要写入的地址为非空,不能写入,请先擦除!\r\n"
    LCALL   F_SendString2
    SJMP    L_QuitRx2Process

L_CMD_Not_W:
L_Rx2ProcessErr:
    MOV     DPTR, #TestString7      ;"命令错误!\r\n"
    LCALL   F_SendString2

L_QuitRx2Process:
    MOV     RX2_Cnt, #0     ;   //清除字节数

    LJMP    L_MainLoop
;===================================================================


;**********************************************

;========================================================================
; 函数: F_CheckData
; 描述: ASCII的数字'0'~'9','A'~'F'转成十六进制的数字 0~9, A~F.
; 参数: ACC: 要转换的数据。
; 返回: F0 = 0 正确,  F0 = 1 错误.
; 版本: V1.0, 2014-1-22
;========================================================================
F_CheckData:
    CLR     F0  ;0--正确, 1--错误
    MOV     R7, A
    CLR     C
    SUBB    A, #'0'
    JC      L_CheckDataErr  ; < '0'
    MOV     A, R7
    CLR     C
    SUBB    A, #'9'+1
    JNC     L_CheckDataChar ; > '9'
    MOV     A, R7
    CLR     C
    SUBB    A, #'0'
    RET
L_CheckDataChar:
    MOV     A,R7
    CLR     C
    SUBB    A, #'A'
    JC      L_CheckDataErr  ; < 'A'
    MOV     A, R7
    CLR     C
    SUBB    A, #'F'+1
    JNC     L_CheckDataChar ; > 'F'
    MOV     A, R7
    CLR     C
    SUBB    A, #'A'-10
    RET

L_CheckDataErr:
    SETB    F0  ;0--正确, 1--错误
    RET

;========================================================================
; 函数: F_GetAddress
; 描述: 获取要操作的EEPROM的首地址.
; 参数: non.
; 返回: F0 = 0 正确,  F0 = 1 错误.
; 版本: V1.0, 2014-1-22
;========================================================================
F_GetAddress:
    MOV     address_E, #0
    MOV     address_H, #0
    MOV     address_L, #0

    MOV     R0, #RX2_Buffer+2
    CJNE    @R0, #'0', L_AddrError  ;检查地址是否0X开头
    INC     R0
    CJNE    @R0, #'X', L_AddrError

    MOV     R0, #RX2_Buffer+4
    MOV     A, @R0
    LCALL   F_CheckData
    JB      F0, L_AddrError
    SWAP    A
    MOV     address_E, A
    INC     R0
    MOV     A, @R0
    LCALL   F_CheckData
    JB      F0, L_AddrError
    ORL     A, address_E
    MOV     address_E, A

    INC     R0
    MOV     A, @R0
    LCALL   F_CheckData
    JB      F0, L_AddrError
    SWAP    A
    MOV     address_H, A

    INC     R0
    MOV     A, @R0
    LCALL   F_CheckData
    JB      F0, L_AddrError
    ORL     A, address_H
    MOV     address_H, A

    INC     R0
    MOV     A, @R0
    LCALL   F_CheckData
    JB      F0, L_AddrError
    SWAP    A
    MOV     address_L, A

    INC     R0
    MOV     A, @R0
    LCALL   F_CheckData
    JB      F0, L_AddrError
    ORL     A, address_L
    MOV     address_L, A
    CLR     F0
    RET
L_AddrError:
    SETB    F0
    RET

;========================================================================
; 函数: F_CheckNumber
; 描述: 获取要读出数据的字节数.
; 参数: non.
; 返回: F0 = 0 正确,  F0 = 1 错误.
; 版本: V1.0, 2014-1-22
;========================================================================
F_CheckNumber:
    CLR     F0  ;0--正确, 1--错误
    MOV     R7, A
    CLR     C
    SUBB    A, #'0'
    JC      L_CheckNumberErr    ; < '0'
    MOV     A, R7
    CLR     C
    SUBB    A, #'9'+1
    JNC     L_CheckNumberErr    ; > '9'
    MOV     A, R7
    CLR     C
    SUBB    A, #'0'
    RET
L_CheckNumberErr:
    SETB    F0  ;0--正确, 1--错误
    RET

F_GetReadDataLength:
    CLR     F0  ;0--正确, 1--错误
    
    MOV     length, #0
    MOV     R2, #11
    MOV     R0, #RX2_Buffer+11
L_GetReadDataLengthLoop:    
    MOV     A, length
    MOV     B, #10
    MUL     AB
    MOV     length, A
    MOV     A, @R0
    LCALL   F_CheckNumber
    JB      F0, L_GetReadDataLengthErr
    ADD     A, length
    MOV     length, A
    INC     R0
    INC     R2
    MOV     A, R2
    CJNE    A, RX2_Cnt, L_GetReadDataLengthLoop
    RET
L_GetReadDataLengthErr:
    RET


TestString1:
    DB  "STC32G系列单片机EEPROM测试程序!",0DH,0AH,0
TestString1A:
    DB  "W 0x000040 1234567890  --> 对0x000040地址写入字符1234567890.",0DH,0AH,0
TestString1B:
    DB  "R 0x000040 10          --> 对0x000040地址读出10个字节内容.",0DH,0AH,0
TestString2:
    DB  "读出",0
TestString3:
    DB  "个字节内容如下:",0DH,0AH,0
TestString4:
    DB  "已写入",0
TestString5:
    DB  "字节内容!",0DH,0AH,0
TestString6:
    DB  "写入错误!",0DH,0AH,0
TestString7:
    DB  "命令错误!",0DH,0AH,0

F_SendByte:
    SETB    B_TX2_Busy      ;标志发送忙
    MOV     S2BUF, A        ;发送一个字节
    JB      B_TX2_Busy, $   ;等待发送完成
    RET

;========================================================================
; 函数: F_SendString2
; 描述: 串口2发送字符串函数。
; 参数: DPTR: 字符串首地址.
; 返回: none.
; 版本: VER1.0
; 日期: 2014-11-28
; 备注: 
;========================================================================
F_SendString2:
    CLR     A
    MOVC    A, @A+DPTR      ;Get current char
    JZ      L_SendEnd       ;Check the end of the string
    LCALL   F_SendByte      ;发送一个字节
    INC     DPTR            ;increment string ptr
    SJMP    F_SendString2       ;Check next
L_SendEnd:
    RET

;========================================================================
; 函数: F_SetTimer2Baudraye
; 描述: 设置Timer2做波特率发生器。
; 参数: DPTR: Timer2的重装值.
; 返回: none.
; 版本: VER1.0
; 日期: 2014-11-28
; 备注: 
;========================================================================
F_SetTimer2Baudraye:    ; 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
    ANL     AUXR, #NOT (1 SHL 4)    ; Timer stop    波特率使用Timer2产生
    ANL     AUXR, #NOT (1 SHL 3)    ; Timer2 set As Timer
    ORL     AUXR, #(1 SHL 2)        ; Timer2 set as 1T mode
    MOV     T2H, DPH
    MOV     T2L, DPL
    ANL     IE2, #NOT (1 SHL 2)     ; 禁止中断
    ORL     AUXR, #(1 SHL 4)        ; Timer run enable
    RET

;========================================================================
; 函数: F_UART2_config
; 描述: UART2初始化函数。
; 参数: ACC: 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
; 返回: none.
; 版本: VER1.0
; 日期: 2014-11-28
; 备注: 
;========================================================================
F_UART2_config:
    CJNE    A, #2, L_Uart2NotUseTimer2
    MOV     DPTR, #UART2_Baudrate
    LCALL   F_SetTimer2Baudraye

    MOV     A, #0X01            ; 使用串口2时，W1位必需设置为1，否则可能会产生不可预期的错误
    MOV     WR6, #WORD0 S2CFG
    MOV     WR4, #WORD2 S2CFG
    MOV     @DR4, R11

    ANL     S2CON, #0x3f
    ORL     S2CON, #0x40     ; UART2模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
    ORL     IE2,   #0X01        ; 允许中断
    ORL     S2CON, #(1 SHL 4)   ; 允许接收
;   ANL     P_SW2, #NOT 0x01    ; UART2 switch to P1.0 P1.1
    ORL     P_SW2, #0x01        ; UART2 switch to P4.6 P4.7
    CLR     B_TX2_Busy
    MOV     RX2_Cnt, #0;
    MOV     TX2_Cnt, #0;
    MOV     RX2_TimeOut, #0
L_Uart2NotUseTimer2:
    RET

;========================================================================
; 函数: F_UART2_Interrupt
; 描述: UART2中断函数。
; 参数: nine.
; 返回: none.
; 版本: VER1.0
; 日期: 2014-11-28
; 备注: 
;========================================================================
F_UART2_Interrupt:
    PUSH    PSW
    PUSH    ACC
    PUSH    AR0
    
    MOV     A, S2CON
    JNB     ACC.0, L_QuitUartRx
    ANL     S2CON, #NOT 1;
    MOV     RX2_TimeOut, #5
    MOV     A, #RX2_Buffer
    ADD     A, RX2_Cnt
    MOV     R0, A
    MOV     @R0, S2BUF  ;保存一个字节
    INC     RX2_Cnt
    MOV     A, RX2_Cnt
    CJNE    A, #RX2_Lenth, L_QuitUartRx
    MOV     RX2_Cnt, #0     ; 避免溢出处理
L_QuitUartRx:

    MOV     A, S2CON
    JNB     ACC.1, L_QuitUartTx
    ANL     S2CON, #NOT 2;
    CLR     B_TX2_Busy      ; 清除发送忙标志
L_QuitUartTx:

    POP     AR0
    POP     ACC
    POP     PSW
    RETI


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


;========================================================================
; 函数: F_DisableEEPROM
; 描述: 禁止访问ISP/IAP.
; 参数: non.
; 返回: non.
; 版本: V1.0, 2012-10-22
;========================================================================
F_DisableEEPROM:
    MOV     IAP_CONTR, #0       ; 禁止ISP/IAP操作
    MOV     IAP_CMD,  #0        ; 去除ISP/IAP命令
    MOV     IAP_TRIG, #0        ; 防止ISP/IAP命令误触发
    MOV     IAP_ADDRE, #0FFH    ; 清0地址高字节
    MOV     IAP_ADDRH, #0FFH    ; 清0地址中字节
    MOV     IAP_ADDRL, #0FFH    ; 清0地址低字节，指向非EEPROM区，防止误操作
    RET

;========================================================================
; 函数: F_EEPROM_Trig
; 描述: 触发EEPROM操作.
; 参数: none.
; 返回: none.
; 版本: V1.0, 2014-6-30
;========================================================================
F_EEPROM_Trig:
    MOV     C, EA
    MOV     F0, C           ;保存全局中断
    CLR     EA              ;禁止中断, 避免触发命令无效
    MOV     IAP_TRIG, #0x5A ;先送5AH，再送A5H到ISP/IAP触发寄存器，每次都需要如此
                            ;送完A5H后，ISP/IAP命令立即被触发启动.
                            ;CPU等待IAP完成后，才会继续执行程序.
    MOV     IAP_TRIG, #0xA5
    NOP     ;由于STC32G是多级流水线的指令系统，触发命令后建议加4个NOP，保证IAP_DATA的数据完成准备
    NOP
    NOP
    NOP
    MOV     C, F0
    MOV     EA, C       ;恢复全局中断
    RET

;========================================================================
; 函数: F_EEPROM_read_n
; 描述: 从指定EEPROM首地址读出n个字节放指定的缓冲.
; 参数: address_H,address_L:  读出EEPROM的首地址.
;       R0:                   读出数据放缓冲的首地址.
;       length:               读出的字节长度.
; 返回: non.
; 版本: V1.0, 2012-10-22
;========================================================================

F_EEPROM_read_n:
    PUSH    AR2

    MOV     R2, length
    MOV     IAP_ADDRE, address_E    ; 送地址高字节（地址需要改变时才需重新送地址）
    MOV     IAP_ADDRH, address_H    ; 送地址高字节（地址需要改变时才需重新送地址）
    MOV     IAP_ADDRL, address_L    ; 送地址低字节（地址需要改变时才需重新送地址）
    MOV     IAP_CONTR, #IAP_EN      ; 允许ISP/IAP操作，送一次就够
	MOV     IAP_TPS, #TPS_WAIT      ; 设置等待时间，
    MOV     IAP_CMD, #1             ; ISP读出命令送字节读命令，命令不需改变时，不需重新送命令
L_EE_Read_Loop:
    LCALL   F_EEPROM_Trig           ; 触发EEPROM操作
    MOV     @R0, IAP_DATA

    MOV     A, IAP_ADDRL            ; 地址 +1
    ADD     A, #1
    MOV     IAP_ADDRL, A
    MOV     A, IAP_ADDRH
    ADDC    A, #0
    MOV     IAP_ADDRH, A
    MOV     A, IAP_ADDRE
    ADDC    A, #0
    MOV     IAP_ADDRE, A
    INC     R0
    DJNZ    R2, L_EE_Read_Loop

    LCALL   F_DisableEEPROM
    POP     AR2
    RET


;========================================================================
; 函数: F_EEPROM_SectorErase
; 描述: 把指定地址的EEPROM扇区擦除.
; 参数: address_H,address_L:  要擦除的扇区EEPROM的地址.
; 返回: non.
; 版本: V1.0, 2013-5-10
;========================================================================
F_EEPROM_SectorErase:
                                            ;只有扇区擦除，没有字节擦除，512字节/扇区。
                                            ;扇区中任意一个字节地址都是扇区地址。
    MOV     IAP_ADDRE, address_E    ; 送地址高字节（地址需要改变时才需重新送地址）
    MOV     IAP_ADDRH, address_H    ; 送地址高字节（地址需要改变时才需重新送地址）
    MOV     IAP_ADDRL, address_L    ; 送地址低字节（地址需要改变时才需重新送地址）
    MOV     IAP_CONTR, #IAP_EN      ; 允许ISP/IAP操作，送一次就够
	MOV     IAP_TPS, #TPS_WAIT      ; 设置等待时间，
    MOV     IAP_CMD, #3             ;送扇区擦除命令
    LCALL   F_EEPROM_Trig           ; 触发EEPROM操作
    LCALL   F_DisableEEPROM         ; 禁止EEPROM操作
    RET

;========================================================================
; 函数: F_EEPROM_write_n
; 描述: 把缓冲的n个字节写入指定首地址的EEPROM, 并且校验.
; 参数: address_H,address_L:    写入EEPROM的首地址.
;       R0:                     写入源数据的缓冲的首地址.
;       length:                 写入的字节长度.
; 返回: F0 == 0, 写入正确,  F0 == 1, 写入错误.
; 版本: V1.0, 2014-1-22
;========================================================================
F_EEPROM_write_n:
    PUSH    AR2
    PUSH    AR0
    MOV     R2, length
    MOV     IAP_ADDRE, address_E    ; 送地址高字节（地址需要改变时才需重新送地址）
    MOV     IAP_ADDRH, address_H    ; 送地址高字节（地址需要改变时才需重新送地址）
    MOV     IAP_ADDRL, address_L    ; 送地址低字节（地址需要改变时才需重新送地址）
    MOV     IAP_CONTR, #IAP_EN      ; 允许ISP/IAP操作，送一次就够
	MOV     IAP_TPS, #TPS_WAIT      ; 设置等待时间，
    MOV     IAP_CMD, #2             ;送字节写命令，命令不需改变时，不需重新送命令
L_EE_W_Loop:
    MOV     IAP_DATA, @R0           ; 送数据到IAP_DATA，只有数据改变时才需重新送
    LCALL   F_EEPROM_Trig           ; 触发EEPROM操作
    MOV     A, IAP_ADDRL            ;地址 +1
    ADD     A, #1
    MOV     IAP_ADDRL, A
    MOV     A, IAP_ADDRH
    ADDC    A, #0
    MOV     IAP_ADDRH, A
    MOV     A, IAP_ADDRE
    ADDC    A, #0
    MOV     IAP_ADDRE, A
    INC     R0
    DJNZ    R2, L_EE_W_Loop

    POP     AR0
    MOV     R2, length              ; 写入后比较
    MOV     IAP_ADDRE, address_E    ; 送地址高字节（地址需要改变时才需重新送地址）
    MOV     IAP_ADDRH, address_H    ; 送地址高字节（地址需要改变时才需重新送地址）
    MOV     IAP_ADDRL, address_L    ; 送地址低字节（地址需要改变时才需重新送地址）
    MOV     IAP_CONTR, #IAP_EN      ; 允许ISP/IAP操作，送一次就够
	MOV     IAP_TPS, #TPS_WAIT      ; 设置等待时间，
    MOV     IAP_CMD, #1     ; ISP读出命令送字节读命令，命令不需改变时，不需重新送命令
L_EE_Compare_Loop:
    LCALL   F_EEPROM_Trig           ; 触发EEPROM操作
    MOV     A, IAP_DATA
    XRL     A, @R0
    JNZ     L_EE_CompareErr

    MOV     A, IAP_ADDRL    ;
    ADD     A, #1
    MOV     IAP_ADDRL, A
    MOV     A, IAP_ADDRH
    ADDC    A, #0
    MOV     IAP_ADDRH, A
    MOV     A, IAP_ADDRE
    ADDC    A, #0
    MOV     IAP_ADDRE, A
    INC     R0
    DJNZ    R2, L_EE_Compare_Loop

    LCALL   F_DisableEEPROM
    CLR     F0
    POP     AR2
    RET

L_EE_CompareErr:
    LCALL   F_DisableEEPROM
    SETB    F0
    POP     AR2
    RET


    END

