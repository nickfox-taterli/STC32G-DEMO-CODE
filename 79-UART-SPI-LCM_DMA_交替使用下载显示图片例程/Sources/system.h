/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

#ifndef	__SYSTEM_H
#define	__SYSTEM_H

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc     35000000L   //������ʱ��

#define DMA_WR_LEN    256      //���� DMA �����ֽ�����SPI Flash һ�������д��256�ֽڣ�
#define DMA_AMT_LEN   2048     //���� DMA �����ֽ�����LCD�����������ɱ�2048������320*240*2/2048=75��

extern bit Mode_Flag;
extern bit DmaBufferSW;
extern u16 lcdIndex;

extern u8 xdata DmaBuffer1[DMA_AMT_LEN];
extern u8 xdata DmaBuffer2[DMA_AMT_LEN];

void delay_ms(u16 ms);

#endif
