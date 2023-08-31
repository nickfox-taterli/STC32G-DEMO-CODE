#ifndef _USB_H_
#define _USB_H_

typedef unsigned char BYTE;
typedef unsigned int WORD;
typedef unsigned long DWORD;

#define EPIDLE 0
#define EPSTATUS 1
#define EPDATAIN 2
#define EPDATAOUT 3
#define EPSTALL -1
#define GET_STATUS 0x00
#define CLEAR_FEATURE 0x01
#define SET_FEATURE 0x03
#define SET_ADDRESS 0x05
#define GET_DESCRIPTOR 0x06
#define SET_DESCRIPTOR 0x07
#define GET_CONFIG 0x08
#define SET_CONFIG 0x09
#define GET_INTERFACE 0x0A
#define SET_INTERFACE 0x0B
#define SYNCH_FRAME 0x0C
#define GET_REPORT 0x01
#define GET_IDLE 0x02
#define GET_PROTOCOL 0x03
#define SET_REPORT 0x09
#define SET_IDLE 0x0A
#define SET_PROTOCOL 0x0B
#define DESC_DEVICE 0x01
#define DESC_CONFIG 0x02
#define DESC_STRING 0x03
#define DESC_HIDREPORT 0x22
#define STANDARD_REQUEST 0x00
#define CLASS_REQUEST 0x20
#define VENDOR_REQUEST 0x40
#define REQUEST_MASK 0x60

typedef struct
{
	BYTE bmRequestType;
	BYTE bRequest;
	BYTE wValueL;
	BYTE wValueH;
	BYTE wIndexL;
	BYTE wIndexH;
	BYTE wLengthL;
	BYTE wLengthH;
}SETUP;

typedef struct
{
	BYTE bStage;
	WORD wResidue;
	BYTE *pData;
}EP0STAGE;

char code DEVICEDESC[18];
char code CONFIGDESC[41];
char code HIDREPORTDESC[27];
char code LANGIDDESC[4];
char code MANUFACTDESC[8];
char code PRODUCTDESC[30];
SETUP Setup;
EP0STAGE Ep0Stage;
BYTE xdata HidFreature[64];
BYTE xdata HidInput[64];
BYTE xdata HidOutput[64];

#endif