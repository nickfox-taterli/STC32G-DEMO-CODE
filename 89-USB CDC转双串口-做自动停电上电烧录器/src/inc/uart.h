#ifndef __UART_H__
#define __UART_H__

#define BR(n)                   (65536 - ((FOSC) / (n) + 2)/ 4)

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
