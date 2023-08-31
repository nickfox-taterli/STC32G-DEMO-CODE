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

#include "STC32G.h"
#include "system.h"
#include "intrins.h"
#include "iap_eeprom.h"

/****************************** 用户定义宏 ***********************************/

#define     Tip_Delay   (MAIN_Fosc / 1000000)

/*****************************************************************************/


/*************  本地常量声明    **************/

//void    DisableEEPROM(void);
//void    EEPROM_read_n(u32 EE_address,u8 *DataAddress,u16 number);
//void    EEPROM_write_n(u32 EE_address,u8 *DataAddress,u16 number);
//void    EEPROM_SectorErase(u32 EE_address);

//========================================================================
// 函数: void   ISP_Disable(void)
// 描述: 禁止访问ISP/IAP.
// 参数: non.
// 返回: non.
// 版本: V1.0, 2012-10-22
//========================================================================
void DisableEEPROM(void)
{
    IAP_CONTR = 0;          //关闭 IAP 功能
    IAP_CMD = 0;            //清除命令寄存器
    IAP_TRIG = 0;           //清除触发寄存器
    IAP_ADDRE = 0xff;       //将地址设置到非 IAP 区域
    IAP_ADDRH = 0xff;       //将地址设置到非 IAP 区域
    IAP_ADDRL = 0xff;
}

//========================================================================
// 函数: void EEPROM_read_n(u32 EE_address,u8 *DataAddress,u16 number)
// 描述: 从指定EEPROM首地址读出n个字节放指定的缓冲.
// 参数: EE_address:  读出EEPROM的首地址.
//       DataAddress: 读出数据放缓冲的首地址.
//       number:      读出的字节长度.
// 返回: non.
// 版本: V1.0, 2012-10-22
//========================================================================
void EEPROM_read_n(u32 EE_address,u8 *DataAddress,u16 number)
{
    EA = 0;     //禁止中断

    IAP_CONTR = 0x80; //使能 IAP
    IAP_TPS = Tip_Delay; //设置擦除等待参数 24MHz
    IAP_CMD = 1;  //设置 IAP 读命令
    do
    {
        IAP_ADDRE = (u8)(EE_address >> 16); //送地址高字节（地址需要改变时才需重新送地址）
        IAP_ADDRH = (u8)(EE_address >> 8);  //送地址中字节（地址需要改变时才需重新送地址）
        IAP_ADDRL = (u8)EE_address;         //送地址低字节（地址需要改变时才需重新送地址）
        IAP_TRIG = 0x5a; //写触发命令(0x5a)
        IAP_TRIG = 0xa5; //写触发命令(0xa5)
        _nop_();   //由于STC32G是多级流水线的指令系统，触发命令后建议加4个NOP，保证IAP_DATA的数据完成准备
        _nop_();
        _nop_();
        _nop_();
        *DataAddress = IAP_DATA; //读 IAP 数据
        EE_address++;
        DataAddress++;
    }while(--number);
    DisableEEPROM();
    EA = 1;     //重新允许中断
}

//========================================================================
// 函数: void EEPROM_SectorErase(u32 EE_address)
// 描述: 把指定地址的EEPROM扇区擦除.
// 参数: EE_address:  要擦除的扇区EEPROM的地址.
// 返回: non.
// 版本: V1.0, 2013-5-10
//========================================================================
void EEPROM_SectorErase(u32 EE_address)
{
    EA = 0;     //禁止中断

    IAP_CONTR = 0x80; //使能 IAP
    IAP_TPS = Tip_Delay; //设置擦除等待参数 24MHz
    IAP_CMD = 3;      //设置 IAP 擦除命令
    IAP_ADDRE = (u8)(EE_address >> 16); //送扇区地址高字节（地址需要改变时才需重新送地址）
    IAP_ADDRH = (u8)(EE_address >> 8);  //送扇区地址中字节（地址需要改变时才需重新送地址）
    IAP_ADDRL = (u8)EE_address;         //送扇区地址低字节（地址需要改变时才需重新送地址）
    IAP_TRIG = 0x5a; //写触发命令(0x5a)
    IAP_TRIG = 0xa5; //写触发命令(0xa5)
    _nop_();   //由于STC32G是多级流水线的指令系统，触发命令后建议加4个NOP，保证IAP_DATA的数据完成准备
    _nop_();
    _nop_();
    _nop_();
    DisableEEPROM();
    EA = 1;     //重新允许中断
}

//========================================================================
// 函数: void EEPROM_write_n(u32 EE_address,u8 *DataAddress,u16 number)
// 描述: 把缓冲的n个字节写入指定首地址的EEPROM.
// 参数: EE_address:  写入EEPROM的首地址.
//       DataAddress: 写入源数据的缓冲的首地址.
//       number:      写入的字节长度.
// 返回: non.
// 版本: V1.0, 2012-10-22
//========================================================================
void EEPROM_write_n(u32 EE_address,u8 *DataAddress,u16 number)
{
    EA = 0;     //禁止中断

    IAP_CONTR = 0x80; //使能 IAP
    IAP_TPS = Tip_Delay; //设置擦除等待参数 24MHz
    IAP_CMD = 2;  //设置 IAP 写命令
    do
    {
        IAP_ADDRE = (u8)(EE_address >> 16); //送地址高字节（地址需要改变时才需重新送地址）
        IAP_ADDRH = (u8)(EE_address >> 8);  //送地址中字节（地址需要改变时才需重新送地址）
        IAP_ADDRL = (u8)EE_address;         //送地址低字节（地址需要改变时才需重新送地址）
        IAP_DATA = *DataAddress; //写 IAP 数据
        IAP_TRIG = 0x5a; //写触发命令(0x5a)
        IAP_TRIG = 0xa5; //写触发命令(0xa5)
        _nop_();   //由于STC32G是多级流水线的指令系统，触发命令后建议加4个NOP，保证IAP_DATA的数据完成准备
        _nop_();
        _nop_();
        _nop_();
        EE_address++;
        DataAddress++;
    }while(--number);

    DisableEEPROM();
    EA = 1;     //重新允许中断
}
