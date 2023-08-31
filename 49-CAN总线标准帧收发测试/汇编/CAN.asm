;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- STC 1T Series MCU Demo Programme -------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
;/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/* --- BBS: www.STCAIMCU.com ------------------------------------------*/
;/* --- QQ:  800003751 -------------------------------------------------*/
;/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
;/*---------------------------------------------------------------------*/


;*************  功能说明    **************

;本例程基于STC32G为主控芯片的实验箱进行编写测试。

;使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

;edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

;CAN总线收发测试用例.

;DCAN是一个支持CAN2.0B协议的功能单元。

;每秒钟发送一帧标准CAN总线数据.

;收到一个标准帧后, 替换原先的 CAN ID 与发送的数据.

;默认波特率500KHz, 用户可自行修改.

;下载时, 选择时钟 24MHZ (用户可自行修改频率).

;******************************************/

$include (../../comm/STC32G.INC)

;/****************************** 用户定义宏 ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;堆栈开始地址

Timer0_Reload   EQU     (65536 - Fosc_KHZ)  ; Timer 0 中断频率, 1000次/秒

;CAN总线波特率=Fclk/((1+(TSG1+1)+(TSG2+1))*(BRP+1)*2)
TSG1   EQU    2		;0~15
TSG2   EQU    1		;1~7 (TSG2 不能设置为0)
BRP    EQU    3		;0~63
;24000000/((1+3+2)*4*2)=500KHz

SJW    EQU    00H	;重新同步跳跃宽度： 00H/040H/080H/0C0H

;总线波特率100KHz以上设置为 0; 100KHz以下设置为 1
SAM    EQU    00H	;总线电平采样次数： 00H:采样1次; 080H:采样3次

;*******************************************************************
;*******************************************************************


;*************  IO口定义    **************/


;*************  本地变量声明    **************/
Flag0           DATA    20H
B_1ms           BIT     Flag0.0 ; 1ms标志
B_CanRead       BIT     Flag0.1 ; CAN收到数据标志

msecond         DATA    21H     ;
CAN_ID          DATA    23H     ;

RX_BUF          DATA    30H
TX_BUF          DATA    38H

TMP_BUF         DATA    40H


;*******************************************************************
;*******************************************************************
        ORG     0000H               ;程序复位入口，编译器自动定义到 0FF0000H 地址
        LJMP    F_Main


        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt

        ORG     00E3H               ;28 CAN interrupt
        LJMP    F_CAN_Interrupt


;*******************************************************************
;*******************************************************************


;******************** 主程序 **************************/
        ORG     0200H       ;编译器自动定义到 0FF0200H 地址
F_Main:
    MOV     WTST, #00H     ;设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    MOV     CKCON,#00H     ;提高访问XRAM速度
    ORL     P_SW2,#080H    ;使能访问XFR

    MOV     P0M1, #30H     ;设置P0.4、P0.5为漏极开路(实验箱加了上拉电阻到3.3V)
    MOV     P0M0, #30H
    MOV     P1M1, #32H     ;设置P1.1、P1.4、P1.5为漏极开路(实验箱加了上拉电阻到3.3V), P1.1在PWM当DAC电路通过电阻串联到P2.3
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

    CLR     TR0
    ORL     AUXR, #(1 SHL 7)    ; Timer0_1T();
    ANL     TMOD, #NOT 04H      ; Timer0_AsTimer();
    ANL     TMOD, #NOT 03H      ; Timer0_16bitAutoReload();
    MOV     TH0, #Timer0_Reload / 256   ;Timer0_Load(Timer0_Reload);
    MOV     TL0, #Timer0_Reload MOD 256
    SETB    ET0         ; Timer0_InterruptEnable();
    SETB    TR0         ; Timer0_Run();

    LCALL   F_CAN_Init  ; CAN 控制器初始化
    SETB    EA          ; 打开总中断

;=====================================================

    MOV     WR4, #0345H
    MOV     CAN_ID, WR4     ;设置默认 CAN ID
    MOV     TX_BUF+0, #11H  ;设置默认发送的CAN数据
    MOV     TX_BUF+1, #22H
    MOV     TX_BUF+2, #33H
    MOV     TX_BUF+3, #44H
    MOV     TX_BUF+4, #55H
    MOV     TX_BUF+5, #66H
    MOV     TX_BUF+6, #77H
    MOV     TX_BUF+7, #88H

;=================== 主循环 ==================================
L_Main_Loop:
    JNB     B_1ms,  L_Main_Loop     ;1ms未到
    CLR     B_1ms
    
;=================== 检测1000ms是否到 ==================================
    MOV     WR6, msecond
    INC     WR6, #1         ;msecond + 1
    MOV     msecond, WR6
    CMP     WR6, #1000
    JC      L_Quit_Check_1000ms  ;if(msecond < 1000), jmp
    MOV     WR6, #0
    MOV     msecond, WR6    ;msecond = 0
    
;================= 1000ms到， 处理RTC ====================================
    MOV     WR6, #0
    MOV     msecond, WR6    ;msecond = 0

    MOV     A, #SR
    LCALL   F_CAN_ReadReg
    JB      ACC.0,L_CAN_BUSOFF
    LCALL   F_CAN_SendMsg
    LJMP    L_Quit_Check_1000ms
L_CAN_BUSOFF:
    MOV     A, #MR
    MOV     WR6, #WORD0 CANAR
    MOV     WR4, #WORD2 CANAR
    MOV     @DR4, R11

    MOV     WR6, #WORD0 CANDR
    MOV     WR4, #WORD2 CANDR
    MOV     R11, @DR4
    ANL     A,#NOT 04H  ;清除 Reset Mode, 从BUS-OFF状态退出
    MOV     @DR4, R11

L_Quit_Check_1000ms:

    JNB     B_CanRead,  L_Main_Loop     ;未接收到CAN总线数据
    CLR     B_CanRead
    LCALL   F_CAN_ReadMsg     ;接收CAN总线数据，替换原先的CAN_ID与发送的数据
;=====================================================
    LJMP    L_Main_Loop

;**********************************************/

;========================================================================
; 函数: F_CAN_ReadReg
; 描述: CAN功能寄存器读取函数.
; 参数: A: Addr.
; 返回: A: Data.
; 版本: V1.0, 2022-04-06
;========================================================================
F_CAN_ReadReg:
    MOV     WR6, #WORD0 CANAR
    MOV     WR4, #WORD2 CANAR
    MOV     @DR4, R11

    MOV     WR6, #WORD0 CANDR
    MOV     WR4, #WORD2 CANDR
    MOV     R11,@DR4
    RET

;========================================================================
; 函数: F_CAN_WriteReg
; 描述: CAN功能寄存器配置函数.
; 参数: A: Addr, B: Data.
; 返回: none.
; 版本: V1.0, 2022-04-06
;========================================================================
F_CAN_WriteReg:
    MOV     WR6, #WORD0 CANAR
    MOV     WR4, #WORD2 CANAR
    MOV     @DR4, R11

    MOV     WR6, #WORD0 CANDR
    MOV     WR4, #WORD2 CANDR
    MOV     @DR4, R10
    RET

;========================================================================
; 函数: F_CAN_ReadMsg
; 描述: CAN接收数据函数.
; 参数: none.
; 返回: none.
; 版本: V1.0, 2022-04-06
;========================================================================
F_CAN_ReadMsg:
    MOV     A, #RX_BUF0
    CALL    F_CAN_ReadReg
    MOV     TMP_BUF+0, A

    MOV     A, #RX_BUF1
    CALL    F_CAN_ReadReg
    MOV     TMP_BUF+1, A

    MOV     A, #RX_BUF2
    CALL    F_CAN_ReadReg
    MOV     TMP_BUF+2, A

    MOV     A, #RX_BUF3
    CALL    F_CAN_ReadReg
    MOV     TX_BUF+0, A   ;RX_BUF+0

    MOV     A, #RX_BUF0
    CALL    F_CAN_ReadReg
    MOV     TX_BUF+1, A   ;RX_BUF+1

    MOV     A, #RX_BUF1
    CALL    F_CAN_ReadReg
    MOV     TX_BUF+2, A   ;RX_BUF+2

    MOV     A, #RX_BUF2
    CALL    F_CAN_ReadReg
    MOV     TX_BUF+3, A   ;RX_BUF+3

    MOV     A, #RX_BUF3
    CALL    F_CAN_ReadReg
    MOV     TX_BUF+4, A   ;RX_BUF+4

    MOV     A, #RX_BUF0
    CALL    F_CAN_ReadReg
    MOV     TX_BUF+5, A   ;RX_BUF+5

    MOV     A, #RX_BUF1
    CALL    F_CAN_ReadReg
    MOV     TX_BUF+6, A   ;RX_BUF+6

    MOV     A, #RX_BUF2
    CALL    F_CAN_ReadReg
    MOV     TX_BUF+7, A   ;RX_BUF+7

    MOV     A, #RX_BUF3
    CALL    F_CAN_ReadReg

    MOV     A, #RX_BUF0
    CALL    F_CAN_ReadReg

    MOV     A, #RX_BUF1
    CALL    F_CAN_ReadReg

    MOV     A, #RX_BUF2
    CALL    F_CAN_ReadReg

    MOV     A, #RX_BUF3
    CALL    F_CAN_ReadReg

;	CanID = ((buffer[1] << 8) + buffer[2]) >> 5;
    MOV     R6, TMP_BUF+1
    MOV     R7, TMP_BUF+2
	SRL     WR6
	SRL     WR6
	SRL     WR6
	SRL     WR6
	SRL     WR6
    ANL     WR6, #07FFH
    MOV     CAN_ID, WR6  ;解析 CAN ID

    RET

;========================================================================
; 函数: F_CAN_SendMsg
; 描述: CAN发送数据函数.
; 参数: none.
; 返回: none.
; 版本: V1.0, 2022-04-06
;========================================================================
F_CAN_SendMsg:
    MOV     A, #TX_BUF0
    MOV     B, #08H        ;bit7: 标准帧(0)/扩展帧(1), bit6: 数据帧(0)/远程帧(1), bit3~bit0: 数据长度(DLC)
    CALL    F_CAN_WriteReg

    MOV     WR2, CAN_ID
	SLL     WR2
	SLL     WR2
	SLL     WR2
	SLL     WR2
	SLL     WR2
    MOV     A, #TX_BUF1
    MOV     B, R2
    CALL    F_CAN_WriteReg
    MOV     A, #TX_BUF2
    MOV     B, R3
    CALL    F_CAN_WriteReg
    MOV     A, #TX_BUF3
    MOV     B, TX_BUF+0
    CALL    F_CAN_WriteReg

    MOV     A, #TX_BUF0
    MOV     B, TX_BUF+1
    CALL    F_CAN_WriteReg
    MOV     A, #TX_BUF1
    MOV     B, TX_BUF+2
    CALL    F_CAN_WriteReg
    MOV     A, #TX_BUF2
    MOV     B, TX_BUF+3
    CALL    F_CAN_WriteReg
    MOV     A, #TX_BUF3
    MOV     B, TX_BUF+4
    CALL    F_CAN_WriteReg

    MOV     A, #TX_BUF0
    MOV     B, TX_BUF+5
    CALL    F_CAN_WriteReg
    MOV     A, #TX_BUF1
    MOV     B, TX_BUF+6
    CALL    F_CAN_WriteReg
    MOV     A, #TX_BUF2
    MOV     B, TX_BUF+7
    CALL    F_CAN_WriteReg
    MOV     A, #TX_BUF3
    MOV     B, #0
    CALL    F_CAN_WriteReg

    MOV     A, #CMR
    MOV     B, #04H        ;发起一次帧传输
    CALL    F_CAN_WriteReg
    RET

;========================================================================
; 函数: F_CAN_Init
; 描述: CAN初始化程序.
; 参数: none
; 返回: none.
; 版本: V1.0, 2022-04-06
;========================================================================
F_CAN_Init:
    ORL     AUXR2, #02H       ;CAN1模块使能
    ANL     AUXR2, #NOT 08H   ;选择CAN1模块
    MOV     P_SW1, #0         ;端口切换(CAN_Rx,CAN_Tx) 0x00:P0.0,P0.1  0x10:P5.0,P5.1  0x20:P4.2,P4.5  0x30:P7.0,P7.1

    MOV     A, #MR
    MOV     B, #04H           ;使能 Reset Mode
    CALL    F_CAN_WriteReg

    ;设置波特率
    MOV     A, #SJW
	ADD     A, #BRP
    MOV     B, A
    MOV     A, #BTR0
    CALL    F_CAN_WriteReg

    MOV     A, #TSG2
    SWAP    A
	ADD     A, #TSG1
	ADD     A, #SAM
    MOV     B, A
    MOV     A, #BTR1
    CALL    F_CAN_WriteReg

    ;总线验收代码寄存器
    MOV     A, #ACR0
    MOV     B, #00H
    CALL    F_CAN_WriteReg
    MOV     A, #ACR1
    MOV     B, #00H
    CALL    F_CAN_WriteReg
    MOV     A, #ACR2
    MOV     B, #00H
    CALL    F_CAN_WriteReg
    MOV     A, #ACR3
    MOV     B, #00H
    CALL    F_CAN_WriteReg
    ;总线验收屏蔽寄存器
    MOV     A, #AMR0
    MOV     B, #0FFH
    CALL    F_CAN_WriteReg
    MOV     A, #AMR1
    MOV     B, #0FFH
    CALL    F_CAN_WriteReg
    MOV     A, #AMR2
    MOV     B, #0FFH
    CALL    F_CAN_WriteReg
    MOV     A, #AMR3
    MOV     B, #0FFH
    CALL    F_CAN_WriteReg

    MOV     A, #IMR
    MOV     B, #0FFH       ;中断寄存器
    CALL    F_CAN_WriteReg
    MOV     A, #ISR
    MOV     B, #0FFH       ;清中断标志
    CALL    F_CAN_WriteReg
    MOV     A, #MR
    MOV     B, #00H        ;退出 Reset Mode
    CALL    F_CAN_WriteReg

    MOV     CANICR, #02H   ;CAN中断使能
    RET


;**************** 中断函数 ***********************************************

F_Timer0_Interrupt: ;Timer0 1ms中断函数
    PUSH    PSW     ;PSW入栈
    PUSH    ACC     ;ACC入栈

    SETB    B_1ms           ; 1ms标志

    POP     ACC     ;ACC出栈
    POP     PSW     ;PSW出栈
    RETI

;========================================================================
F_CAN_Interrupt:    ;CAN中断函数
    PUSH    PSW     ;PSW入栈
    PUSH    ACC     ;ACC入栈
    PUSH    R4      ;R4入栈
    PUSH    R5      ;R5入栈
    PUSH    R6      ;R6入栈
    PUSH    R7      ;R7入栈
    PUSH    B       ;B入栈

    MOV     WR6, #WORD0 CANAR   ;CANAR现场保存，避免主循环里写完 CANAR 后产生中断，在中断里修改了 CANAR 内容
    MOV     WR4, #WORD2 CANAR
    MOV     R10,@DR4            ;存入 B 寄存器

    MOV     A, #ISR
;    MOV     WR6, #WORD0 CANAR
;    MOV     WR4, #WORD2 CANAR
    MOV     @DR4, R11

    MOV     WR6, #WORD0 CANDR
    MOV     WR4, #WORD2 CANDR
    MOV     R11,@DR4
    MOV     @DR4, R11  ;清标志位，写 1 清除

    JB      ACC.0,CAN_DOIIF
ISRTEST1:
    JB      ACC.1,CAN_BEIIF
ISRTEST2:
    JB      ACC.2,CAN_TIIF
ISRTEST3:
    JB      ACC.3,CAN_RIIF
ISRTEST4:
    JB      ACC.4,CAN_EPIIF
ISRTEST5:
    JB      ACC.5,CAN_EWIIF
ISRTEST6:
    JB      ACC.6,CAN_ALIIF

ISREXIT:
    MOV     WR6, #WORD0 CANAR   ;CANAR现场恢复
    MOV     WR4, #WORD2 CANAR
    MOV     @DR4, R10

    POP     B       ;B出栈
    POP     R7      ;R7出栈
    POP     R6      ;R6出栈
    POP     R5      ;R5出栈
    POP     R4      ;R4出栈
    POP     ACC     ;ACC出栈
    POP     PSW     ;PSW出栈
    RETI

CAN_DOIIF:
;    ORL     A,#01H  ;清标志位，写 1 清除
;    MOV     @DR4, R11
    JMP     ISRTEST1

CAN_BEIIF:
;    ORL     A,#02H  ;清标志位，写 1 清除
;    MOV     @DR4, R11
    JMP     ISRTEST2

CAN_TIIF:
;    ORL     A,#04H  ;清标志位，写 1 清除
;    MOV     @DR4, R11
    JMP     ISRTEST3

CAN_RIIF:
;    ORL     A,#08H  ;清标志位，写 1 清除
;    MOV     @DR4, R11
    SETB    B_CanRead
    JMP     ISRTEST4

CAN_EPIIF:
;    ORL     A,#010H ;清标志位，写 1 清除
;    MOV     @DR4, R11
    JMP     ISRTEST5

CAN_EWIIF:
;    ORL     A,#020H ;清标志位，写 1 清除
;    MOV     @DR4, R11
    JMP     ISRTEST6

CAN_ALIIF:
;    ORL     A,#040H ;清标志位，写 1 清除
;    MOV     @DR4, R11
    JMP     ISREXIT

;========================================================================
    END

