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
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

/****************************************����˵��****************************************

�����̻���STC32G���б�д���ԣ�ʵ���䣬���������������Ⱦ��ɲ��ԡ�
ͨ��USB CDC(Communication Device Class)Э��ʶ��Ϊ�����豸����STC-ISP(V6.90�Ժ�汾);

1.��ISP����У�ѡ��󶨵�CDC���ڣ��������ⰴ����OLED12864�����ⴰ�ڣ��򿪴���
2.������ѧ����������.kpc���ļ�����������̣����ɿ�ʼ���ԡ�

���������ʾ���ָ�λ����USB����ģʽ�ķ�����

1. ͨ��ÿ1����ִ��һ�Ρ�KeyResetScan��������ʵ�ֳ���P3.2�ڰ�������MCU��λ������USB����ģʽ��
   (�����ϣ����λ����USB����ģʽ�Ļ������ڸ�λ�����ｫ IAP_CONTR ��bit6��0��ѡ��λ���û�������)
2. ͨ�����ء�stc_usb_cdc_32.lib���⺯����ʵ��ʹ��STC-ISP�������ָ���MCU��λ������USB����ģʽ���Զ����ء�
   (ע�⣺ʹ��CDC�ӿڴ���MCU��λ���Զ����ع��ܣ���Ҫ��ѡ�˿����ã��´�ǿ��ʹ�á�STC USB Writer (HID)������ISP����)
   
������ʾ��Ƶ�� www.stcai.com ��Ƶ��ʾר����

����ʱ, ѡ��ʱ���м�ʱ��ѡ��24MHZ��

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
char *USER_STCISPCMD = "@STCISP#";                      //�����Զ���λ��ISP�����û��ӿ�����

//P3.2�ڰ�����λ�������
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
    while(DeviceState != DEVSTATE_CONFIGURED);  //�ȴ�USB�������

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
        KeyResetScan();   //����P3.2�ڰ������������λ������USB����ģʽ������Ҫ�˹��ܿ�ɾ�����д���

//        if(DeviceState != DEVSTATE_CONFIGURED)  //�ж�USB�豸�Ƿ��������
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
                USB_SendData(UsbOutBuffer,OutNumber);    //�������ݻ�����������
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

/********************* INT1�жϺ��� *************************/
void INT2_int (void) interrupt 10     //���ж�ʱ�Ѿ������־
{
    //INT2_cnt++; //�ж�+1
}

void sys_init()
{
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x00;   P0M0 = 0x00;   //����Ϊ׼˫���
    P1M1 = 0x00;   P1M0 = 0x00;   //����Ϊ׼˫���
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;   //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���
}

//========================================================================
// ����: void KeyResetScan(void)
// ����: P3.2�ڰ�������1�봥�������λ������USB����ģʽ��
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-6-11
// ��ע: 
//========================================================================
void KeyResetScan(void)
{
    if(!P32)
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 1000)		//����1000ms��Ч�������
            {
                Key_Flag = 1;		//���ð���״̬����ֹ�ظ�����

                USBCON = 0x00;      //���USB����
                USBCLK = 0x00;
                IRC48MCR = 0x00;
                
                delay_ms(10);
                IAP_CONTR = 0x60;   //���������λ����ISP��ʼִ��
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