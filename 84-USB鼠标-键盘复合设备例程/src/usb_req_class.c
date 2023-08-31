#include "stc.h"
#include "usb.h"
#include "usb_req_class.h"

BYTE bHidIdle;
BYTE bKey1Code = 0;
BYTE bKey2Code = 0;
BYTE bKey1Debounce = 0;
BYTE bKey2Debounce = 0;
BOOL fKey1OK;
BOOL fKey2OK;

sbit LED_NUM        = P6^7;
sbit LED_CAPS       = P6^6;
sbit LED_SCROLL     = P6^5;

void usb_req_class()
{
    switch (Setup.bRequest)
    {
    case GET_REPORT:
        usb_get_report();
        break;
    case SET_REPORT:
        usb_set_report();
        break;
    case GET_IDLE:
        usb_get_idle();
        break;
    case SET_IDLE:
        usb_set_idle();
        break;
    case GET_PROTOCOL:
        usb_get_protocol();
        break;
    case SET_PROTOCOL:
        usb_set_protocol();
        break;
    default:
        usb_setup_stall();
        return;
    }
}

void usb_get_report()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (IN_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    Ep0State.pData = UsbBuffer;
    Ep0State.wSize = Setup.wLength;

    usb_setup_in();
}

void usb_set_report()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (OUT_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    Ep0State.pData = UsbBuffer;
    Ep0State.wSize = Setup.wLength;

    usb_setup_out();
}

void usb_get_idle()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (IN_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    Ep0State.pData = &bHidIdle;
    Ep0State.wSize = 1;

    usb_setup_in();
}

void usb_set_idle()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (OUT_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    bHidIdle = Setup.wValueH;

    usb_setup_status();
}

void usb_get_protocol()
{
    usb_setup_stall();
}

void usb_set_protocol()
{
    usb_setup_stall();
}

void usb_class_out()
{
    BYTE led;
    
    if (usb_bulk_intr_out(UsbBuffer, 1) == 1)
    {
        P4M0 &= ~0x01;
        P4M1 &= ~0x01;
        P6M0 &= ~0xe0;
        P6M1 &= ~0xe0;
        P40 = 0;
        
        led = UsbBuffer[0];
        LED_NUM = !(led & 0x01);
        LED_CAPS = !(led & 0x02);
        LED_SCROLL = !(led & 0x04);
    }
}

void usb_class_in()
{
    BYTE key[8];
    BYTE button[3];
    BYTE i;
    
    if (DeviceState != DEVSTATE_CONFIGURED)
        return;

    if (!Usb1InBusy && fKey1OK)
    {
        fKey1OK = 0;

        key[0] = 0;
        key[1] = 0;
        key[2] = 0;
        key[3] = 0;
        key[4] = 0;
        key[5] = 0;
        key[6] = 0;
        key[7] = 0;

        switch (bKey1Code)
        {
        case 0xfe:  key[2] = 0x1e; break;
        case 0xfd:  key[2] = 0x1f; break;
        case 0xfb:  key[2] = 0x20; break;
        case 0xf7:  key[2] = 0x21; break;
        case 0xef:  key[2] = 0x22; break;
        case 0xdf:  key[2] = 0x23; break;
        case 0xbf:  key[2] = 0x24; break;
        case 0x7f:  key[2] = 0x25; break;
        }
        
        EUSB = 0;
        Usb1InBusy = 1;
        usb_write_reg(INDEX, 1);
        for (i=0; i<8; i++)
        {
            usb_write_reg(FIFO1, key[i]);
        }
        usb_write_reg(INCSR1, INIPRDY);
        EUSB = 1;
    }

    if (!Usb2InBusy && fKey2OK)
    {
        fKey2OK = 0;

        button[0] = 0;
        button[1] = 0;
        button[2] = 0;

        switch (bKey2Code)
        {
        case 0xfe:  button[0] = 0x01; break;
        case 0xfd:  button[0] = 0x04; break;
        case 0xfb:  button[0] = 0x02; break;
        case 0xef:  button[1] = 0xfc; break;
        case 0xdf:  button[1] = 0x04; break;
        case 0xbf:  button[2] = 0xfc; break;
        case 0x7f:  button[2] = 0x04; break;
        }
        
        EUSB = 0;
        Usb2InBusy = 1;
        usb_write_reg(INDEX, 2);
        for (i=0; i<3; i++)
        {
            usb_write_reg(FIFO2, button[i]);
        }
        usb_write_reg(INCSR1, INIPRDY);
        EUSB = 1;
    }
}

void scan_keyboard()
{
    BYTE key;
    
    P0M0 = 0x00;
    P0M1 = 0x00;
    
    key = 0;
    P0 = 0xff;
    P06 = 0;
    _nop_();
    _nop_();
    key |= P0 & 0x0f;
    P06 = 1;
    P07 = 0;
    _nop_();
    _nop_();
    key |= (P0 & 0x0f) << 4;
    P07 = 1;
    
    if (!P35)               //P3.5按下时，按键为鼠标功能
        key = 0xff;
    
    if (key != bKey1Code)
    {
        bKey1Code = key;
        bKey1Debounce = 20;
    }
    else
    {
        if (bKey1Debounce)
        {
            bKey1Debounce--;
            if (bKey1Debounce == 0)
            {
                fKey1OK = 1;
            }
        }
    }
}

void scan_mouse()
{
    BYTE key;
    
    P0M0 = 0x00;
    P0M1 = 0x00;
    
    key = 0;
    P0 = 0xff;
    P06 = 0;
    _nop_();
    _nop_();
    key |= P0 & 0x0f;
    P06 = 1;
    P07 = 0;
    _nop_();
    _nop_();
    key |= (P0 & 0x0f) << 4;
    P07 = 1;
    
    if (P35)               //P3.5松开时，按键为键盘功能
        key = 0xff;
    
    if (key != bKey2Code)
    {
        bKey2Code = key;
        bKey2Debounce = 20;
    }
    else
    {
        if (bKey2Debounce)
        {
            bKey2Debounce--;
            if (bKey2Debounce == 0)
            {
                fKey2OK = 1;
            }
        }
    }
}

