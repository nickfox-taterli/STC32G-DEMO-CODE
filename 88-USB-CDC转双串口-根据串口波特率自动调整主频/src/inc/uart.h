#ifndef __UART_H__
#define __UART_H__

#define BR(clk,n)                   (65536 - ((clk) / (n) + 2)/ 4)

#define CPUIDBASE   0xfde0

#define T22M_ADDR   (*(unsigned char volatile xdata *)(CPUIDBASE + 0x0b))   //22.1184MHz
#define T24M_ADDR   (*(unsigned char volatile xdata *)(CPUIDBASE + 0x0c))   //24MHz
#define T27M_ADDR   (*(unsigned char volatile xdata *)(CPUIDBASE + 0x0d))   //27MHz
#define T30M_ADDR   (*(unsigned char volatile xdata *)(CPUIDBASE + 0x0e))   //30MHz
#define T33M_ADDR   (*(unsigned char volatile xdata *)(CPUIDBASE + 0x0f))   //33.1776MHz
#define T35M_ADDR   (*(unsigned char volatile xdata *)(CPUIDBASE + 0x10))   //35MHz
#define T36M_ADDR   (*(unsigned char volatile xdata *)(CPUIDBASE + 0x11))   //36.864MHz
#define T40M_ADDR   (*(unsigned char volatile xdata *)(CPUIDBASE + 0x12))   //40MHz
#define T44M_ADDR   (*(unsigned char volatile xdata *)(CPUIDBASE + 0x13))   //44.2368MHz
#define T48M_ADDR   (*(unsigned char volatile xdata *)(CPUIDBASE + 0x14))   //48MHz

#define VRT6M_ADDR  (*(unsigned char volatile xdata *)(CPUIDBASE + 0x15))   //VRTRIM_6M
#define VRT10M_ADDR (*(unsigned char volatile xdata *)(CPUIDBASE + 0x16))   //VRTRIM_10M
#define VRT27M_ADDR (*(unsigned char volatile xdata *)(CPUIDBASE + 0x17))   //VRTRIM_27M
#define VRT44M_ADDR (*(unsigned char volatile xdata *)(CPUIDBASE + 0x18))   //VRTRIM_44M

typedef enum
{
	IRCBAND_6M = 0,		/*0*/
	IRCBAND_10M,
	IRCBAND_27M,
	IRCBAND_44M,
}IRCBAND_SelType;


void uart_init();
void uart_set_parity();
void uart_set_baud();
void uart_polling();

extern BOOL Uart3TxBusy;
extern BOOL Uart2TxBusy;

extern BYTE Uart3RxRptr;
extern BYTE Uart2RxRptr;
extern BYTE Uart3RxWptr;
extern BYTE Uart2RxWptr;

extern BYTE Uart3TxRptr;
extern BYTE Uart2TxRptr;
extern BYTE Uart3TxWptr;
extern BYTE Uart2TxWptr;

extern BYTE xdata Uart3RxBuffer[256];
extern BYTE xdata Uart2RxBuffer[256];
extern BYTE xdata Uart3TxBuffer[256];
extern BYTE xdata Uart2TxBuffer[256];

#endif
