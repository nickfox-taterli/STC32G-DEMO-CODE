#include "stc.h"
#include "usb_desc.h"

char code DEVICEDESC[18] =
{
    0x12,                   //bLength(18);
    0x01,                   //bDescriptorType(Device);
    0x00,0x02,              //bcdUSB(2.00);
    0x00,                   //bDeviceClass(0);
    0x00,                   //bDeviceSubClass0);
    0x00,                   //bDeviceProtocol(0);
    0x40,                   //bMaxPacketSize0(64);
    0xbf,0x34,              //idVendor(34bf);
    0x0b,0xff,              //idProduct(ff0b);
    0x00,0x01,              //bcdDevice(1.00);
    0x01,                   //iManufacturer(1);
    0x02,                   //iProduct(2);
    0x00,                   //iSerialNumber(0);
    0x01,                   //bNumConfigurations(1);
};

char code CONFIGDESC[66] =
{
    0x09,                   //bLength(9);
    0x02,                   //bDescriptorType(Configuration);
    0x42,0x00,              //wTotalLength(66);
    0x02,                   //bNumInterfaces(2);
    0x01,                   //bConfigurationValue(1);
    0x00,                   //iConfiguration(0);
    0x80,                   //bmAttributes(BUSPower);
    0x32,                   //MaxPower(100mA);

    0x09,                   //bLength(9);
    0x04,                   //bDescriptorType(Interface);
    0x00,                   //bInterfaceNumber(0);
    0x00,                   //bAlternateSetting(0);
    0x02,                   //bNumEndpoints(2);
    0x03,                   //bInterfaceClass(HID);
    0x01,                   //bInterfaceSubClass(Boot);
    0x01,                   //bInterfaceProtocol(Keyboard);
    0x00,                   //iInterface(0);

    0x09,                   //bLength(9);
    0x21,                   //bDescriptorType(HID);
    0x01,0x01,              //bcdHID(1.01);
    0x00,                   //bCountryCode(0);
    0x01,                   //bNumDescriptors(1);
    0x22,                   //bDescriptorType(HID Report);
    0x41,0x00,              //wDescriptorLength(65);

    0x07,                   //bLength(7);
    0x05,                   //bDescriptorType(Endpoint);
    0x81,                   //bEndpointAddress(EndPoint1 as IN);
    0x03,                   //bmAttributes(Interrupt);
    0x08,0x00,              //wMaxPacketSize(8);
    0x0a,                   //bInterval(10ms);

    0x07,                   //bLength(7);
    0x05,                   //bDescriptorType(Endpoint);
    0x01,                   //bEndpointAddress(EndPoint1 as OUT);
    0x03,                   //bmAttributes(Interrupt);
    0x01,0x00,              //wMaxPacketSize(1);
    0x0a,                   //bInterval(10ms);

    0x09,                   //bLength(9);
    0x04,                   //bDescriptorType(Interface);
    0x01,                   //bInterfaceNumber(1);
    0x00,                   //bAlternateSetting(0);
    0x01,                   //bNumEndpoints(1);
    0x03,                   //bInterfaceClass(HID);
    0x01,                   //bInterfaceSubClass(Boot);
    0x02,                   //bInterfaceProtocol(Mouse);
    0x00,                   //iInterface(0);

    0x09,                   //bLength(9);
    0x21,                   //bDescriptorType(HID);
    0x01,0x01,              //bcdHID(1.01);
    0x00,                   //bCountryCode(0);
    0x01,                   //bNumDescriptors(1);
    0x22,                   //bDescriptorType(HID Report);
    0x32,0x00,              //wDescriptorLength(50);

    0x07,                   //bLength(7);
    0x05,                   //bDescriptorType(Endpoint);
    0x82,                   //bEndpointAddress(EndPoint2 as IN);
    0x03,                   //bmAttributes(Interrupt);
    0x03,0x00,              //wMaxPacketSize(3);
    0x0a,                   //bInterval(10ms);
};

/*
Input Report:
0   Modifierkeys (D0:LCtrl D1:LShift D2:LAlt D3:LGui D4:RCtrl D5:RShift D6:RAlt D7:RGui)
1	Reserved
2	Keycode 1
3	Keycode 2
4	Keycode 3
5	Keycode 4
6	Keycode 5
7	Keycode 6
Output Report:
0   LEDs (D0:NumLock D1:CapLock D2:ScrollLock)
*/
char code KEYBOARDREPORTDESC[65] =
{
    0x05,0x01,              //USAGE_PAGE(Generic Desktop);
    0x09,0x06,              //USAGE(Keyboard);
    0xa1,0x01,              //COLLECTION(Application);
    0x05,0x07,              //  USAGE_PAGE(Keyboard);
    0x19,0xe0,              //  USAGE_MINIMUM(224);
    0x29,0xe7,              //  USAGE_MAXIMUM(255);
    0x15,0x00,              //  LOGICAL_MINIMUM(0);
    0x25,0x01,              //  LOGICAL_MAXIMUM(1);
    0x75,0x01,              //  REPORT_SIZE(1);
    0x95,0x08,              //  REPORT_COUNT(8);
    0x81,0x02,              //  INPUT(Data,Variable,Absolute);
    0x75,0x08,              //  REPORT_SIZE(8);
    0x95,0x01,              //  REPORT_COUNT(1);
    0x81,0x01,              //  INPUT(Constant);
    0x19,0x00,              //  USAGE_MINIMUM(0);
    0x29,0x65,              //  USAGE_MAXIMUM(101);
    0x15,0x00,              //  LOGICAL_MINIMUM(0);
    0x25,0x65,              //  LOGICAL_MAXIMUM(101);
    0x75,0x08,              //  REPORT_SIZE(8);
    0x95,0x06,              //  REPORT_COUNT(6);
    0x81,0x00,              //  INPUT(Data,Array);
    0x05,0x08,              //  USAGE_PAGE(LEDs);
    0x19,0x01,              //  USAGE_MINIMUM(1);
    0x29,0x03,              //  USAGE_MAXIMUM(3);
    0x15,0x00,              //  LOGICAL_MINIMUM(0);
    0x25,0x01,              //  LOGICAL_MAXIMUM(1);
    0x75,0x01,              //  REPORT_SIZE(1);
    0x95,0x03,              //  REPORT_COUNT(3);
    0x91,0x02,              //  OUTPUT(Data,Variable,Absolute);
    0x75,0x05,              //  REPORT_SIZE(5);
    0x95,0x01,              //  REPORT_COUNT(1);
    0x91,0x01,              //  OUTPUT(Constant);
    0xc0,                   //END_COLLECTION;
};

/*
Input Report:
0   Buttons (D0:LButton D1:RButton D2:MButton [D3:D7]:Pad)
1	X displacement (>0:right; <0:left)
2	Y displacement (>0:down; <0:up)
*/
char code MOUSEREPORTDESC[50] =
{
    0x05,0x01,              //USAGE_PAGE(Generic Desktop);
    0x09,0x02,              //USAGE(Mouse);
    0xa1,0x01,              //COLLECTION(Application);
    0x09,0x01,              //  USAGE(Pointer);
    0xa1,0x00,              //  COLLECTION(Physical);
    0x05,0x09,              //    USAGE_PAGE(Buttons);
    0x19,0x01,              //    USAGE_MINIMUM(1);
    0x29,0x03,              //    USAGE_MAXIMUM(3);
    0x15,0x00,              //    LOGICAL_MINIMUM(0);
    0x25,0x01,              //    LOGICAL_MAXIMUM(1);
    0x75,0x01,              //    REPORT_SIZE(1);
    0x95,0x03,              //    REPORT_COUNT(3);
    0x81,0x02,              //    INPUT(Data,Variable,Absolute);
    0x75,0x05,              //    REPORT_SIZE(5);
    0x95,0x01,              //    REPORT_COUNT(1);
    0x81,0x01,              //    INPUT(Constant);
    0x05,0x01,              //    USAGE_PAGE(Generic Desktop);
    0x09,0x30,              //    USAGE(X);
    0x09,0x31,              //    USAGE(Y);
    0x15,0x81,              //    LOGICAL_MINIMUM(-127);
    0x25,0x7f,              //    LOGICAL_MAXIMUM(127);
    0x75,0x08,              //    REPORT_SIZE(8);
    0x95,0x02,              //    REPORT_COUNT(2);
    0x81,0x06,              //    INPUT(Data, Variable, Relative);
    0xc0,                   //  END_COLLECTION;
    0xc0,                   //END_COLLECTION;
};

char code LANGIDDESC[4] =
{
    0x04,0x03,
    0x09,0x04,
};

char code MANUFACTDESC[8] =
{
    0x08,0x03,
    'S',0,
    'T',0,
    'C',0,
};

char code PRODUCTDESC[70] =
{
    0x46,0x03,
    'S',0,
    'T',0,
    'C',0,
    ' ',0,
    'U',0,
    'S',0,
    'B',0,
    ' ',0,
    'C',0,
    'O',0,
    'M',0,
    'P',0,
    'O',0,
    'S',0,
    'I',0,
    'T',0,
    'E',0,
    ' ',0,
    'K',0,
    'e',0,
    'y',0,
    'b',0,
    'o',0,
    'a',0,
    'r',0,
    'd',0,
    ' ',0,
    '&',0,
    ' ',0,
    'M',0,
    'o',0,
    'u',0,
    's',0,
    'e',0,
};

char code PACKET0[2] =
{
    0, 0,
};

char code PACKET1[2] =
{
    1, 0,
};

