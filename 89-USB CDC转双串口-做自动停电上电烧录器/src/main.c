/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����        */
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���STC32G12K128Ϊ����оƬ��ʵ������б�д����.

ʹ��USB�����Ӻ��İ�USB�ӿ������;

MCUͨ��USB CDC(Communication Device Class)Э��ʶ��Ϊ2�������豸;

����CDC1���⴮�ڣ�ͨ��TXD3_2(P5.1), RXD3_2(P5.0)�ӿ��շ�; CDC2���⴮�ڣ�ͨ��TXD2_2(P4.7), RXD2_2(P4.6)�ӿ��շ���
�̽�ʵ�����ϵ�J7��J8���߼��ɽ���USBת˫���ڼ��໥ͨ�š�

��P3.2�ڰ����ֶ��Ͽ��������(�����Դ��ʵ����Q11������C��ȡ��)���ɿ������ָ����硣

ʹ�ñ�����CDC1���ڶ�����оƬ����ISP����ʱͨ����config.h���ļ�����ġ�ISPPWRCTRL������ѡ�������Դ�Զ����ƿ���
ʹ�ܴ˹���ʱ,�����STC-ISP���ز��������������Զ�ͣ�����ϵ�
������Ҫ�˹���ֻ��Ҫע�͡�ISPPWRCTRL�����弴��

����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��)��

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
    
    S3_S = 1;       //UART3(RxD3_2/P5.0, TxD3_2/P5.1)
    S2_S = 1;       //UART2(RxD2_2/P4.6, TxD2_2/P4.7)
    
    LED_POWER = 0;      //���ָʾ��(P6.7)
}

#define SVCC_ON()           SVCC_E = 0; P5M1 &= ~0x03   //P5.0,P5.1����׼˫��ģʽ
#define SVCC_OFF()          SVCC_E = 1; P5M1 |= 0x03   //P5.0,P5.1���ø�������ģʽ
    
#ifdef ISPPWRCTRL
    #define ISPSTG_IDLE         0
    #define ISPSTG_CNT7F        1
    #define ISPSTG_PWROFF       2

    #define CNT7F_MAX           16          //������⵽7F�ĸ���
    #define TIMEOUT_DETECT      2000        //���7F�ĳ�ʱʱ��(2000ms)
    #define TIME_PWROFF         500         //�Զ�ͣ���ʱ��(500ms)

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
// ����: void KeyResetScan(void)
// ����: P3.2�ڰ�������S-VCC�ϵ硣
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-12-06
// ��ע: 
//========================================================================
void KeyResetScan(void)
{
    if(!P32)
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 5)		//����50ms��Ч�������
            {
                Key_Flag = 1;		//���ð���״̬����ֹ�ظ�����

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
