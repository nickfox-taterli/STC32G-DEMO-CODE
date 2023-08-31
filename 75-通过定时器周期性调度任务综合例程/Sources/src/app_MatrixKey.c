/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

#include "app_MatrixKey.h"

/*************** 功能说明 ****************

矩阵键盘扫描.

******************************************/

sbit BEEP = P5^4;

//========================================================================
//                               本地常量声明	
//========================================================================

u8 code T_KeyTable[16] = {0,1,2,0,3,0,0,0,4,0,0,0,0,0,0,0};

//========================================================================
//                               本地变量声明
//========================================================================

u8  cntms;

u8  KeyCode;    //给用户使用的键码, 1~16有效
u8  IO_KeyState, IO_KeyState1, IO_KeyHoldCnt;    //行列键盘变量
bit IO_KeyHoldFlag; //按键按压/释放标志

//========================================================================
//                               本地函数声明
//========================================================================

void MatrixKeyScan(void);

//========================================================================
//                            外部函数和变量声明
//========================================================================


/*****************************************************
    行列键扫描程序
    使用XY查找4x4键的方法，只能单键，速度快

   Y     P04      P05      P06      P07
          |        |        |        |
X         |        |        |        |
P00 ---- K00 ---- K01 ---- K02 ---- K03 ----
          |        |        |        |
P01 ---- K04 ---- K05 ---- K06 ---- K07 ----
          |        |        |        |
P02 ---- K08 ---- K09 ---- K10 ---- K11 ----
          |        |        |        |
P03 ---- K12 ---- K13 ---- K14 ---- K15 ----
          |        |        |        |
******************************************************/

void IO_KeyDelay(void)
{
    u8 i;
    i = 60;
    while(--i)  ;
}

//========================================================================
// 函数: Sample_MatrixKey
// 描述: 用户应用程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2022-05-26
//========================================================================
void Sample_MatrixKey(void)
{
    if(cntms > 0)
    {
        cntms--;
    }
    else
    {
        BEEP = 1;    //蜂鸣器关闭
    }

    MatrixKeyScan();
    if(KeyCode > 0)  //有键按下
    {
        KeyCode = 0;
        BEEP = 0;    //蜂鸣器响起
        cntms = 5;   //持续时间 5*50ms
    }
}

//========================================================================
// 函数: Sample_MatrixKey
// 描述: 用户应用程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2022-05-26
//========================================================================
void MatrixKeyScan(void)
{
    u8  j;

    P0 = 0xf0;  //X低，读Y
    IO_KeyDelay();
    IO_KeyState = P0 & 0xc0;

    P0 = 0x0f;  //Y低，读X
    IO_KeyDelay();
    IO_KeyState |= (P0 & 0x0f);
    IO_KeyState ^= 0xcf;   //取反
    
    if(IO_KeyState == IO_KeyState1)   //连续两次读相等
    {
        if(IO_KeyState != 0)    //有键按下
        {
            if(IO_KeyHoldFlag == 0)
            {
                if(IO_KeyHoldCnt >= 3)    //10ms * 5 = 50ms
                {
                    j = T_KeyTable[IO_KeyState >> 4];
                    if((j != 0) && (T_KeyTable[IO_KeyState& 0x0f] != 0)) 
                        KeyCode = (j - 1) * 4 + T_KeyTable[IO_KeyState & 0x0f] + 16;    //计算键码，17~32
                    IO_KeyHoldFlag = 1;
                }
                else if(IO_KeyHoldCnt <= 5)
                    IO_KeyHoldCnt++;
            }
        }
        else
        {
            IO_KeyHoldFlag = 0;
            IO_KeyHoldCnt = 0;
        }
    }
    else
    {
        IO_KeyHoldFlag = 0;
        IO_KeyHoldCnt = 0;
    }
    
    IO_KeyState1 = IO_KeyState;
    P0 = 0xff;
}

