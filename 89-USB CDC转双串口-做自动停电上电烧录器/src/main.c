/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于STC32G12K128为主控芯片的实验箱进行编写测试.

使用USB线连接核心板USB接口与电脑;

MCU通过USB CDC(Communication Device Class)协议识别为2个串口设备;

其中CDC1虚拟串口，通过TXD3_2(P5.1), RXD3_2(P5.0)接口收发; CDC2虚拟串口，通过TXD2_2(P4.7), RXD2_2(P4.6)接口收发。
短接实验箱上的J7、J8跳线即可进行USB转双串口间相互通信。

按P3.2口按键手动断开输出供电(输出电源从实验箱Q11三极管C极取电)，松开按键恢复供电。

使用本工具CDC1串口对其它芯片进行ISP下载时通过“config.h”文件里面的“ISPPWRCTRL”定义选择输出电源自动控制开关
使能此功能时,会侦测STC-ISP下载并对输出供电进行自动停电再上电
若不需要此功能只需要注释“ISPPWRCTRL”定义即可

下载时, 选择时钟 24MHZ (用户可自行修改频率)。

******************************************/

#include "stc.h"
#include "usb.h"
#include "uart.h"
#include "timer.h"

bit Key_Flag;
u8  Key_cnt;

void sys_init();

void check_init();
void check_isp();
void KeyResetScan();

void main()
{
    sys_init();
    timer_init();
    uart_init();
    usb_init();
    
#ifdef ISPPWRCTRL
    check_init();
#endif

    EA = 1;
    
    while (1)
    {
        uart_polling();
        
        if (f10ms)
        {
            f10ms = 0;
            KeyResetScan();
            
#ifdef ISPPWRCTRL
            check_isp();
#endif
        }
    }
}

void sys_init()
{
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口
    
    S3_S = 1;       //UART3(RxD3_2/P5.0, TxD3_2/P5.1)
    S2_S = 1;       //UART2(RxD2_2/P4.6, TxD2_2/P4.7)
    
    LED_POWER = 0;      //输出指示灯(P6.7)
}

#define SVCC_ON()           SVCC_E = 0; P5M1 &= ~0x03   //P5.0,P5.1设置准双向模式
#define SVCC_OFF()          SVCC_E = 1; P5M1 |= 0x03   //P5.0,P5.1设置高阻输入模式
    
#ifdef ISPPWRCTRL
    #define ISPSTG_IDLE         0
    #define ISPSTG_CNT7F        1
    #define ISPSTG_PWROFF       2

    #define CNT7F_MAX           16          //连续检测到7F的个数
    #define TIMEOUT_DETECT      2000        //检测7F的超时时间(2000ms)
    #define TIME_PWROFF         500         //自动停电的时间(500ms)

    BOOL IspUartSet;
    BYTE IspStage;
    BYTE IspWaitX10ms;
    BYTE IspCount7F;
    
    void check_init()
    {
        IspUartSet = 0;
        IspStage = ISPSTG_IDLE;
        IspWaitX10ms = 0;
        IspCount7F = 0;
        
        SVCC_ON();
    }
    
    void check_isp()
    {
        switch (IspStage)
        {
        default:
        case ISPSTG_IDLE:
            if(!Key_Flag)
                SVCC_ON();
            if (IspUartSet)
            {
                IspUartSet = 0;
                IspWaitX10ms = 0;
                IspCount7F = 0;
                IspStage = ISPSTG_CNT7F;
            }
            break;
        case ISPSTG_CNT7F:
            if (IspWaitX10ms >= (TIMEOUT_DETECT/10))
            {
                IspStage = ISPSTG_IDLE;
            }
            else
            {
                IspWaitX10ms++;
                if (IspCount7F >= CNT7F_MAX)
                {
                    SVCC_OFF();
                    IspWaitX10ms = 0;
                    IspStage = ISPSTG_PWROFF;
                }
            }
            break;
        case ISPSTG_PWROFF:
            if (IspWaitX10ms >= (TIME_PWROFF/10))
            {
                IspStage = ISPSTG_IDLE;
            }
            else
            {
                IspWaitX10ms++;
            }
            break;
        }
    }
#endif

//========================================================================
// 函数: void KeyResetScan(void)
// 描述: P3.2口按键触发S-VCC断电。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-12-06
// 备注: 
//========================================================================
void KeyResetScan(void)
{
    if(!P32)
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 5)		//连续50ms有效按键检测
            {
                Key_Flag = 1;		//设置按键状态，防止重复触发

                SVCC_OFF();
            }
        }
    }
    else
    {
        Key_cnt = 0;
        if(Key_Flag)
        {
            SVCC_ON();
            Key_Flag = 0;
        }
    }
}
