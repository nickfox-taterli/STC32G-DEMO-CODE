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

#include "stc.h"
#include "uart.h"
#include "usb.h"
#include "usb_req_class.h"
#include "util.h"
#include "timer.h"

BOOL Uart3TxBusy;
BOOL Uart2TxBusy;

BYTE Uart3RxRptr;
BYTE Uart2RxRptr;
BYTE Uart3RxWptr;
BYTE Uart2RxWptr;

BYTE Uart3TxRptr;
BYTE Uart2TxRptr;
BYTE Uart3TxWptr;
BYTE Uart2TxWptr;

BYTE xdata Uart3RxBuffer[256];
BYTE xdata Uart2RxBuffer[256];
BYTE xdata Uart3TxBuffer[256];
BYTE xdata Uart2TxBuffer[256];

void uart_init()
{
    S2CON = 0x50;
    T2L = BR(MAIN_Fosc,115200);
    T2H = BR(MAIN_Fosc,115200) >> 8;
    T2x12 = 1;
    T2R = 1;
    ES2 = 1;

    S3CON = 0x50;
    T3L = BR(MAIN_Fosc,115200);
    T3H = BR(MAIN_Fosc,115200) >> 8;
    T4T3M &= 0xf0;
    T4T3M |= 0x0a;
    ES3 = 1;

    LineCoding1.dwDTERate = REV4(115200);
    LineCoding1.bCharFormat = 0;
    LineCoding1.bParityType = 0;
    LineCoding1.bDataBits = 8;

    LineCoding2.dwDTERate = REV4(115200);
    LineCoding2.bCharFormat = 0;
    LineCoding2.bParityType = 0;
    LineCoding2.bDataBits = 8;
    
    Uart3TxBusy = 0;
    Uart2TxBusy = 0;
    Uart3RxRptr = 0;
    Uart2RxRptr = 0;
    Uart3RxWptr = 0;
    Uart2RxWptr = 0;
    Uart3TxRptr = 0;
    Uart2TxRptr = 0;
    Uart3TxWptr = 0;
    Uart2TxWptr = 0;
}

void uart2_isr() interrupt UART2_VECTOR
{
    if (S2TI)
    {
        S2TI = 0;
        Uart2TxBusy = 0;
    }

    if (S2RI)
    {
        S2RI = 0;
        Uart2TxBuffer[Uart2TxWptr++] = S2BUF;
    }
}

void uart3_isr() interrupt UART3_VECTOR
{
    if (S3TI)
    {
        S3TI = 0;
        Uart3TxBusy = 0;
    }

    if (S3RI)
    {
        S3RI = 0;
        Uart3TxBuffer[Uart3TxWptr++] = S3BUF;
    }
}

void uart_set_parity()
{
    if (Interface == 0)
    {
        switch (LineCoding1.bParityType)
        {
        default:
        case NONE_PARITY:
            S3CON = 0x50;
            break;
        case ODD_PARITY:
        case EVEN_PARITY:
        case MARK_PARITY:
            S3CON = 0xd8;
            break;
        case SPACE_PARITY:
            S3CON = 0xd0;
            break;
        }
    }
    else if (Interface == 2)
    {
        switch (LineCoding2.bParityType)
        {
        default:
        case NONE_PARITY:
            S2CON = 0x50;
            break;
        case ODD_PARITY:
        case EVEN_PARITY:
        case MARK_PARITY:
            S2CON = 0xd8;
            break;
        case SPACE_PARITY:
            S2CON = 0xd0;
            break;
        }
    }
}

#ifdef Dynamic_Frequency        //���������ʶ�̬������Ƶ���ƿ���
//========================================================================
// ����: void SetMCLK(u8 clk)
// ����: ������Ƶ
// ����: clk: ��Ƶ���
// ����: none.
// �汾: VER1.0
// ����: 2022-10-14
// ��ע: 
//========================================================================
void SetMCLK(BYTE clk) //������Ƶ
{
	switch(clk)
	{
	case 1:
        if(MAIN_Fosc == 24000000L) break;
		//ѡ��24MHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = (IRCBAND & ~0x03) | IRCBAND_27M;
		CLKDIV = 1;
		MAIN_Fosc =	24000000L;	//������ʱ��
		break;

	case 2:
        if(MAIN_Fosc == 30000000L) break;
		//ѡ��30MHz
		CLKDIV = 0x04;
		IRTRIM = T30M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = (IRCBAND & ~0x03) | IRCBAND_27M;
		CLKDIV = 1;
		MAIN_Fosc =	30000000L;	//������ʱ��
		break;

	case 3:
        if(MAIN_Fosc == 40000000L) break;
		//ѡ��40MHz
		CLKDIV = 0x04;
		IRTRIM = T40M_ADDR;
		VRTRIM = VRT44M_ADDR;
		IRCBAND = (IRCBAND & ~0x03) | IRCBAND_44M;
		CLKDIV = 1;
		MAIN_Fosc =	40000000L;	//������ʱ��
		break;

	default:
        if(MAIN_Fosc == 22118400L) break;
		//ѡ��22.1184MHz
		CLKDIV = 0x04;
		IRTRIM = T22M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = (IRCBAND & ~0x03) | IRCBAND_27M;
		CLKDIV = 1;
		MAIN_Fosc =	22118400L;	//������ʱ��
		break;
	}
}
#endif

void uart_set_baud()
{
    DWORD baud,baud2;
    WORD temp,temp2;
    
    if (Interface == 0)
    {
        baud = LineCoding1.dwDTERate;
        baud2 = LineCoding2.dwDTERate;
    }
    else if (Interface == 2)
    {
        baud = LineCoding2.dwDTERate;
        baud2 = LineCoding1.dwDTERate;
    }
    
    baud = reverse4(baud);

#ifdef Dynamic_Frequency        //���������ʶ�̬������Ƶ���ƿ���
    baud2 = reverse4(baud2);

    switch (baud)
    {
        case 14400:
        case 19200:
        case 28800:
        case 38400:
        case 57600:
        case 115200:
        case 230400:
        case 460800:
        case 921600:    //�ɱ� 22.1184M ����
            SetMCLK(0); //��ʱ�� 22.1184MHz
            break;
        case 1500000:   //�ɱ� 30M ����
            SetMCLK(2); //��ʱ�� 30MHz
            break;
        default:
            SetMCLK(1); //��ʱ�� 24MHz
            break;
    }
    timer_init();
#endif
    temp = (WORD)BR(MAIN_Fosc,baud);
    temp2 = (WORD)BR(MAIN_Fosc,baud2);
    
    if (Interface == 0)
    {
        T3L = temp;
        T3H = temp >> 8;
        #ifdef Dynamic_Frequency        //���������ʶ�̬������Ƶ���ƿ���
        T2L = temp2;
        T2H = temp2 >> 8;
        #endif
    }
    else if (Interface == 2)
    {
        T2L = temp;
        T2H = temp >> 8;
        #ifdef Dynamic_Frequency        //���������ʶ�̬������Ƶ���ƿ���
        T3L = temp2;
        T3H = temp2 >> 8;
        #endif
    }
}

void uart_polling()
{
    BYTE dat;
    BYTE cnt;

    if (DeviceState != DEVSTATE_CONFIGURED)
        return;

    if (!Ep4InBusy && (Uart3TxRptr != Uart3TxWptr))
    {
        EUSB = 0;
        Ep4InBusy = 1;
        usb_write_reg(INDEX, 4);
        cnt = 0;
        while (Uart3TxRptr != Uart3TxWptr)
        {
            usb_write_reg(FIFO4, Uart3TxBuffer[Uart3TxRptr++]);
            cnt++;
            if (cnt == EP4IN_SIZE) break;
        }
        usb_write_reg(INCSR1, INIPRDY);
        EUSB = 1;
    }

    if (!Ep5InBusy && (Uart2TxRptr != Uart2TxWptr))
    {
        EUSB = 0;
        Ep5InBusy = 1;
        usb_write_reg(INDEX, 5);
        cnt = 0;
        while (Uart2TxRptr != Uart2TxWptr)
        {
            usb_write_reg(FIFO5, Uart2TxBuffer[Uart2TxRptr++]);
            cnt++;
            if (cnt == EP5IN_SIZE) break;
        }
        usb_write_reg(INCSR1, INIPRDY);
        EUSB = 1;
    }

    if (!Uart3TxBusy && (Uart3RxRptr != Uart3RxWptr))
    {
        dat = Uart3RxBuffer[Uart3RxRptr++];
        Uart3TxBusy = 1;
        switch (LineCoding1.bParityType)
        {
        case NONE_PARITY:
        case SPACE_PARITY:
            S3TB8 = 0;
            break;
        case ODD_PARITY:
            ACC = dat;
            if (!P)
            {
                S3TB8 = 1;
            }
            else
            {
                S3TB8 = 0;
            }
            break;
        case EVEN_PARITY:
            ACC = dat;
            if (P)
            {
                S3TB8 = 1;
            }
            else
            {
                S3TB8 = 0;
            }
            break;
        case MARK_PARITY:
            S3TB8 = 1;
            break;
        }
        S3BUF = dat;
//        while (Uart3TxBusy);
    }

    if (!Uart2TxBusy && (Uart2RxRptr != Uart2RxWptr))
    {
        dat = Uart2RxBuffer[Uart2RxRptr++];
        Uart2TxBusy = 1;
        switch (LineCoding2.bParityType)
        {
        case NONE_PARITY:
        case SPACE_PARITY:
            S2TB8 = 0;
            break;
        case ODD_PARITY:
            ACC = dat;
            if (!P)
            {
                S2TB8 = 1;
            }
            else
            {
                S2TB8 = 0;
            }
            break;
        case EVEN_PARITY:
            ACC = dat;
            if (P)
            {
                S2TB8 = 1;
            }
            else
            {
                S2TB8 = 0;
            }
            break;
        case MARK_PARITY:
            S2TB8 = 1;
            break;
        }
        S2BUF = dat;

//        while (Uart2Busy);
    }

    if (Ep4OutBusy)
    {
        EUSB = 0;
        if ((BYTE)(Uart3RxWptr - Uart3RxRptr) < (BYTE)(256 - EP4OUT_SIZE))
        {
            Ep4OutBusy = 0;
            usb_write_reg(INDEX, 4);
            usb_write_reg(OUTCSR1, 0);
        }
        EUSB = 1;
    }

    if (Ep5OutBusy)
    {
        EUSB = 0;
        if ((BYTE)(Uart2RxWptr - Uart2RxRptr) < (BYTE)(256 - EP5OUT_SIZE))
        {
            Ep5OutBusy = 0;
            usb_write_reg(INDEX, 5);
            usb_write_reg(OUTCSR1, 0);
        }
        EUSB = 1;
    }
}
