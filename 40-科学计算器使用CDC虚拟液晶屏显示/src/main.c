/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

/****************************************功能说明****************************************

本例程基于STC32G进行编写测试，实验箱，屠龙刀，降龙棍等均可测试。
通过USB CDC(Communication Device Class)协议识别为串口设备连接STC-ISP(V6.90以后版本);

1.在ISP软件中，选择绑定到CDC串口，随后打开虚拟按键和OLED12864的虚拟窗口，打开串口
2.将“科学计算器键盘.kpc”文件导入虚拟键盘，即可开始测试。

此外程序演示两种复位进入USB下载模式的方法：

1. 通过每1毫秒执行一次“KeyResetScan”函数，实现长按P3.2口按键触发MCU复位，进入USB下载模式。
   (如果不希望复位进入USB下载模式的话，可在复位代码里将 IAP_CONTR 的bit6清0，选择复位进用户程序区)
2. 通过加载“stc_usb_cdc_32.lib”库函数，实现使用STC-ISP软件发送指令触发MCU复位，进入USB下载模式并自动下载。
   (注意：使用CDC接口触发MCU复位并自动下载功能，需要勾选端口设置：下次强制使用”STC USB Writer (HID)”进行ISP下载)
   
早期演示视频见 www.stcai.com 视频演示专区。

下载时, 选择时钟切记时钟选择24MHZ。

****************************************************************************************/

#include "stc.h"
#include "usb.h"
#include "vk.h"
#include "middle.h"
#include "menu.h"
#include "nor_cal.h"
#include "set.h"
#include "rtc.h"
#include "spi.h"

char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //设置自动复位到ISP区的用户接口命令

//P3.2口按键复位所需变量
bit Key_Flag;
u16 Key_cnt;
u8 xdata daf[10]={1,2,3,4,5,6,7,8,9,10};

void sys_init();
void KeyResetScan(void);

void main()
{
    u32 Time_sleep_last=0;
    bit Init_flag;
    u8 Key_Num=0;
    
    sys_init();
    usb_init();

	RTC_config();
    Timer0_init();
    EA = 1; 
    while(DeviceState != DEVSTATE_CONFIGURED);  //等待USB配置完成

    HAL_SPI_Init();
    Menu_Init();    
    OLED_Init();
    Screen_Clear();
    Menu_Display();
    SetParm_Read();
    Screen_Showlight(Parma[0]);
    
    while (1)
    {
        delay_ms(1);
        KeyResetScan();   //长按P3.2口按键触发软件复位，进入USB下载模式，不需要此功能可删除本行代码

//        if(DeviceState != DEVSTATE_CONFIGURED)  //判断USB设备是否配置完成
//            continue;

        if (bUsbOutReady)
        {
            if ((UsbOutBuffer[0] == 'K') &&(UsbOutBuffer[1] == 'E') &&(UsbOutBuffer[2] == 'Y') &&(UsbOutBuffer[3] == 'P'))
            {
                if( Init_flag==0 )
                {
                    Init_flag = 1;
                    OLED_LightSet(255);
                    OLED12864_ScrollStop();
                    OLED_DisplayOn();
                    Menu_Display();
                    Screen_Clear();
                } 
                
                Key_Num = CDC_KeyRead( UsbOutBuffer[5] );
                if( Time_sleep>=(Parma[2]*60) )
                {
                    Screen_Clear();
                    Key_Num  =0;
                }
                
                Time_sleep = 0;
            }
            else
            {
                USB_SendData(UsbOutBuffer,OutNumber);    //发送数据缓冲区，长度
            }
            usb_OUT_done();
        }
        
        if( Key_Num>0 )
        {
            if( Time_sleep>=(Parma[2]*60) )
            {
                Menu_Init(); 
                Screen_Clear();
                Menu_Display();
                Key_Num  =0;
            }            
             Time_sleep = 0;
        }
        delay_ms(9);

        if( Time_sleep< (Parma[2]*60) )
        {
            Menu_Deal(&Key_Num);
            //Time_sleep = 0;
           Time_sleep_last = 0;
        }
        else
        {
            if(Time_sleep>Time_sleep_last)
            {
                if( Time_sleep_last==0 )
                    Screen_Clear();
                Time_sleep_last = Time_sleep;
                Show_time();
            }            
        }

    }
}

/********************* INT1中断函数 *************************/
void INT2_int (void) interrupt 10     //进中断时已经清除标志
{
    //INT2_cnt++; //中断+1
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
}

//========================================================================
// 函数: void KeyResetScan(void)
// 描述: P3.2口按键长按1秒触发软件复位，进入USB下载模式。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-11
// 备注: 
//========================================================================
void KeyResetScan(void)
{
    if(!P32)
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 1000)		//连续1000ms有效按键检测
            {
                Key_Flag = 1;		//设置按键状态，防止重复触发

                USBCON = 0x00;      //清除USB设置
                USBCLK = 0x00;
                IRC48MCR = 0x00;
                
                delay_ms(10);
                IAP_CONTR = 0x60;   //触发软件复位，从ISP开始执行
                while (1);
            }
        }
    }
    else
    {
        Key_cnt = 0;
        Key_Flag = 0;
    }
}