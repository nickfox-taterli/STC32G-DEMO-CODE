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

/*************  ����˵��    **************

�����̻���STC32GΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

CAN������չ֡�շ���������.

DCAN��һ��֧��CAN2.0BЭ��Ĺ��ܵ�Ԫ��

�յ�һ����չ֡��ԭ�����ͳ�ȥ.

MCUÿ���ӷ���һ֡�̶�����.

Ĭ�ϲ�����500KHz, �û��������޸�.

����ʱ, Ĭ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\comm\STC32G.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL

/****************************** �û������ ***********************************/
//CAN���߲�����=Fclk/((1+(TSG1+1)+(TSG2+1))*(BRP+1)*2)
#define TSG1    2		//0~15
#define TSG2    1		//1~7 (TSG2 ��������Ϊ0)
#define BRP     3		//0~63
//24000000/((1+3+2)*4*2)=500KHz

#define SJW     1		//����ͬ����Ծ���

//���߲�����100KHz��������Ϊ 0; 100KHz��������Ϊ 1
#define SAM     0		//���ߵ�ƽ���������� 0:����1��; 1:����3��

/*****************************************************************************/


/*************  ���س�������    **************/

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

#define	STANDARD_FRAME   0     //֡��ʽ����׼֡
#define	EXTENDED_FRAME   1     //֡��ʽ����չ֡

/*************  ���ر�������    **************/

typedef struct
{
	u8	DLC:4;          //���ݳ���, bit0~bit3
	u8	:2;             //������, bit4~bit5
	u8	RTR:1;          //֡����, bit6
	u8	FF:1;           //֡��ʽ, bit7
	u32	ID;             //CAN ID
	u8	DataBuffer[8];  //���ݻ���
}CAN_DataDef;

CAN_DataDef CAN1_Tx;
CAN_DataDef CAN1_Rx[8];

bit B_CanSend;      //CAN ������ɱ�־
bit B_CanRead;      //CAN �յ����ݱ�־
bit B_1ms;          //1ms��־
u16 msecond;

/*************  ���غ�������    **************/
void CANInit();
u8 CanReadReg(u8 addr);
u8 CanReadMsg(CAN_DataDef *CAN);
void CanSendMsg(CAN_DataDef *CAN);

/********************* ������ *************************/
void main(void)
{
	u8 sr,i,n;

    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x30;   P0M0 = 0x30;   //����P0.4��P0.5Ϊ©����·(ʵ��������������赽3.3V)
    P1M1 = 0x32;   P1M0 = 0x32;   //����P1.1��P1.4��P1.5Ϊ©����·(ʵ��������������赽3.3V), P1.1��PWM��DAC��·ͨ�����贮����P2.3
    P2M1 = 0x3c;   P2M0 = 0x3c;   //����P2.2~P2.5Ϊ©����·(ʵ��������������赽3.3V)
    P3M1 = 0x50;   P3M0 = 0x50;   //����P3.4��P3.6Ϊ©����·(ʵ��������������赽3.3V)
    P4M1 = 0x3c;   P4M0 = 0x3c;   //����P4.2~P4.5Ϊ©����·(ʵ��������������赽3.3V)
    P5M1 = 0x0c;   P5M0 = 0x0c;   //����P5.2��P5.3Ϊ©����·(ʵ��������������赽3.3V)
    P6M1 = 0xff;   P6M0 = 0xff;   //����Ϊ©����·(ʵ��������������赽3.3V)
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���

	AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
	TH0 = (u8)(Timer0_Reload / 256);
	TL0 = (u8)(Timer0_Reload % 256);
	ET0 = 1;    //Timer0 interrupt enable
	TR0 = 1;    //Tiner0 run

	CANInit();
	
	EA = 1;                 //�����ж�

	CAN1_Tx.FF = EXTENDED_FRAME;    //��չ֡
	CAN1_Tx.RTR = 0;                //0������֡��1��Զ��֡
	CAN1_Tx.DLC = 0x08;             //���ݳ���
	CAN1_Tx.ID = 0x01234567;        //CAN ID
	CAN1_Tx.DataBuffer[0] = 0x21;   //��������
	CAN1_Tx.DataBuffer[1] = 0x22;
	CAN1_Tx.DataBuffer[2] = 0x23;
	CAN1_Tx.DataBuffer[3] = 0x24;
	CAN1_Tx.DataBuffer[4] = 0x25;
	CAN1_Tx.DataBuffer[5] = 0x26;
	CAN1_Tx.DataBuffer[6] = 0x27;
	CAN1_Tx.DataBuffer[7] = 0x28;
	
    B_CanSend = 0;
	while(1)
	{
		if(B_1ms)   //1ms��
		{
			B_1ms = 0;
			if(++msecond >= 1000)   //1�뵽
			{
				msecond = 0;
				sr = CanReadReg(SR);

				if(sr & 0x01)		//�ж��Ƿ��� BS:BUS-OFF״̬
				{
					CANAR = MR;
					CANDR &= ~0x04;  //��� Reset Mode, ��BUS-OFF״̬�˳�
				}
				else
				{
                    CanSendMsg(&CAN1_Tx);   //����һ֡����
				}
			}
		}

		if(B_CanRead)
		{
			B_CanRead = 0;
			
            n = CanReadMsg(CAN1_Rx);    //��ȡ��������
            if(n>0)
            {
                for(i=0;i<n;i++)
                {
                    CanSendMsg(&CAN1_Rx[i]);  //CAN����ԭ������
                }
            }
		}
	}
}


/********************** Timer0 1ms�жϺ��� ************************/
void timer0 (void) interrupt 1
{
    B_1ms = 1;      //1ms��־
}

//========================================================================
// ����: u8 ReadReg(u8 addr)
// ����: CAN���ܼĴ�����ȡ������
// ����: CAN���ܼĴ�����ַ.
// ����: CAN���ܼĴ�������.
// �汾: VER1.0
// ����: 2020-11-16
// ��ע: 
//========================================================================
u8 CanReadReg(u8 addr)
{
	u8 dat;
	CANAR = addr;
	dat = CANDR;
	return dat;
}

//========================================================================
// ����: void WriteReg(u8 addr, u8 dat)
// ����: CAN���ܼĴ������ú�����
// ����: CAN���ܼĴ�����ַ, CAN���ܼĴ�������.
// ����: none.
// �汾: VER1.0
// ����: 2020-11-16
// ��ע: 
//========================================================================
void CanWriteReg(u8 addr, u8 dat)
{
	CANAR = addr;
	CANDR = dat;
}

//========================================================================
// ����: void CanReadFifo(CAN_DataDef *CANx)
// ����: ��ȡCAN���������ݺ�����
// ����: *CANx: ���CAN���߶�ȡ����.
// ����: none.
// �汾: VER2.0
// ����: 2023-01-31
// ��ע: 
//========================================================================
void CanReadFifo(CAN_DataDef *CAN)
{
    u8 i;
    u8 pdat[5];
    u8 RX_Index=0;

    pdat[0] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));

    if(pdat[0] & 0x80)  //�ж��Ǳ�׼֡������չ֡
    {
        pdat[1] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));   //��չ֡IDռ4���ֽ�
        pdat[2] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));
        pdat[3] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));
        pdat[4] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));
        CAN->ID = (((u32)pdat[1] << 24) + ((u32)pdat[2] << 16) + ((u32)pdat[3] << 8) + pdat[4]) >> 3;
    }
    else
    {
        pdat[1] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));   //��׼֡IDռ2���ֽ�
        pdat[2] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));
        CAN->ID = ((pdat[1] << 8) + pdat[2]) >> 5;
    }
    
    CAN->FF = pdat[0] >> 7;     //֡��ʽ
    CAN->RTR = pdat[0] >> 6;    //֡����
    CAN->DLC = pdat[0];         //���ݳ���

    for(i=0;((i<CAN->DLC) && (i<8));i++)        //��ȡ���ݳ���Ϊlen����಻����8
    {
        CAN->DataBuffer[i] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));   //��ȡ��Ч����
    }
    while(RX_Index&3)   //�ж��Ѷ����ݳ����Ƿ�4��������
    {
        CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));  //��ȡ������ݣ�һ֡����ռ��4���������������ռ䣬���㲹0
    }
}

//========================================================================
// ����: u8 CanReadMsg(void)
// ����: CAN�������ݺ�����
// ����: *CANx: ���CAN���߶�ȡ����..
// ����: ֡����.
// �汾: VER2.0
// ����: 2023-01-31
// ��ע: 
//========================================================================
u8 CanReadMsg(CAN_DataDef *CAN)
{
    u8 i;
    u8 n=0;

    do{
        CanReadFifo(&CAN[n++]);  //��ȡ���ջ���������
        i = CanReadReg(SR);
    }while(i&0x80);     //�жϽ��ջ��������Ƿ������ݣ��еĻ�������ȡ

    return n;   //����֡����
}

//========================================================================
// ����: void CanSendMsg(CAN_DataDef *CAN)
// ����: CAN���ͱ�׼֡������
// ����: *CANx: ���CAN���߷�������..
// ����: none.
// �汾: VER1.0
// ����: 2020-11-19
// ��ע: 
//========================================================================
void CanSendMsg(CAN_DataDef *CAN)
{
	u32 CanID;
    u8 RX_Index,i;

    while(B_CanSend);   //�ȴ��������

    if(CAN->FF)     //�ж��Ƿ���չ֡
    {
        CanID = CAN->ID << 3;
        CanWriteReg(TX_BUF0,CAN->DLC|((u8)CAN->RTR<<6)|0x80);	//bit7: ��׼֡(0)/��չ֡(1), bit6: ����֡(0)/Զ��֡(1), bit3~bit0: ���ݳ���(DLC)
        CanWriteReg(TX_BUF1,(u8)(CanID>>24));
        CanWriteReg(TX_BUF2,(u8)(CanID>>16));
        CanWriteReg(TX_BUF3,(u8)(CanID>>8));

        CanWriteReg(TX_BUF0,(u8)CanID);

        RX_Index = 1;
        for(i=0;((i<CAN->DLC) && (i<8));i++)        //���ݳ���ΪDLC����಻����8
        {
            CanWriteReg((u8)(TX_BUF0 + (RX_Index++&3)),CAN->DataBuffer[i]);   //д����Ч����
        }
        while(RX_Index&3)   //�ж��Ѷ����ݳ����Ƿ�4��������
        {
            CanWriteReg((u8)(TX_BUF0 + (RX_Index++&3)),0x00);  //д��������ݣ�һ֡����ռ��4���������������ռ䣬���㲹0
        }
    }
    else    //���ͱ�׼֡
    {
        CanID = (u16)(CAN->ID << 5);
        CanWriteReg(TX_BUF0,CAN->DLC|((u8)CAN->RTR<<6));  //bit7: ��׼֡(0)/��չ֡(1), bit6: ����֡(0)/Զ��֡(1), bit3~bit0: ���ݳ���(DLC)
        CanWriteReg(TX_BUF1,(u8)(CanID>>8));
        CanWriteReg(TX_BUF2,(u8)CanID);

        RX_Index = 3;
        for(i=0;((i<CAN->DLC) && (i<8));i++)        //���ݳ���ΪDLC����಻����8
        {
            CanWriteReg((u8)(TX_BUF0 + (RX_Index++&3)),CAN->DataBuffer[i]);   //д����Ч����
        }
        while(RX_Index&3)   //�ж��Ѷ����ݳ����Ƿ�4��������
        {
            CanWriteReg((u8)(TX_BUF0 + (RX_Index++&3)),0x00);  //д��������ݣ�һ֡����ռ��4���������������ռ䣬���㲹0
        }
    }
	CanWriteReg(CMR ,0x04);		//����һ��֡����
    B_CanSend = 1;     //���÷���æ��־
}

//========================================================================
// ����: void CANSetBaudrate()
// ����: CAN���߲��������ú�����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2020-11-19
// ��ע: 
//========================================================================
void CANSetBaudrate()
{
	CanWriteReg(BTR0,(SJW << 6) + BRP);
	CanWriteReg(BTR1,(SAM << 7) + (TSG2 << 4) + TSG1);
}

//========================================================================
// ����: void CANInit()
// ����: CAN��ʼ��������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2020-11-19
// ��ע: 
//========================================================================
void CANInit()
{
	CANEN = 1;          //CAN1ģ��ʹ��
	CanWriteReg(MR  ,0x05);  //ʹ�� Reset Mode, ���õ��˲�����

	CANSetBaudrate();	//���ò�����
	
	//���ù���IDΪ��xxF8xxxx ��֡�Ž���
//	CanWriteReg(ACR0,0x07);		//�������մ���Ĵ���
//	CanWriteReg(ACR1,0xc0);
//	CanWriteReg(ACR2,0x00);
//	CanWriteReg(ACR3,0x00);
//	CanWriteReg(AMR0,0xF8);		//�����������μĴ���
//	CanWriteReg(AMR1,0x07);
//	CanWriteReg(AMR2,0xFF);
//	CanWriteReg(AMR3,0xFF);

	//ȡ������ID������֡������
	CanWriteReg(ACR0,0x00);		//�������մ���Ĵ���
	CanWriteReg(ACR1,0x00);
	CanWriteReg(ACR2,0x00);
	CanWriteReg(ACR3,0x00);
	CanWriteReg(AMR0,0xFF);		//�����������μĴ���
	CanWriteReg(AMR1,0xFF);
	CanWriteReg(AMR2,0xFF);
	CanWriteReg(AMR3,0xFF);

	CanWriteReg(IMR ,0xff);		//�жϼĴ���
	CanWriteReg(ISR ,0xff);		//���жϱ�־
	CanWriteReg(MR  ,0x01);		//�˳� Reset Mode, ���õ��˲�����(���ù�������ע��ѡ���˲�ģʽ)
//	CanWriteReg(MR  ,0x00);		//�˳� Reset Mode, ����˫�˲�����(���ù�������ע��ѡ���˲�ģʽ)

	P_SW1 = (P_SW1 & ~(3<<4)) | (0<<4); //�˿��л�(CAN_Rx,CAN_Tx) 0x00:P0.0,P0.1  0x10:P5.0,P5.1  0x20:P4.2,P4.5  0x30:P7.0,P7.1
	CANICR = 0x02;		//CAN�ж�ʹ��
}

//========================================================================
// ����: void CANBUS_Interrupt(void) interrupt CAN1_VECTOR
// ����: CAN�����жϺ�����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2020-11-19
// ��ע: 
//========================================================================
void CANBUS_Interrupt(void) interrupt CAN1_VECTOR
{
	u8 isr;
	u8 arTemp;
	arTemp = CANAR;     //CANAR�ֳ����棬������ѭ����д�� CANAR ������жϣ����ж����޸��� CANAR ����
	
	isr = CanReadReg(ISR);
	if((isr & 0x04) == 0x04)  //TI
    {
		CANAR = ISR;
		CANDR = 0x04;    //CLR FLAG

        B_CanSend = 0;
    }	
	if((isr & 0x08) == 0x08)  //RI
    {
		CANAR = ISR;
		CANDR = 0x08;    //CLR FLAG

		B_CanRead = 1;
    }

	if((isr & 0x40) == 0x40)  //ALI
    {
		CANAR = ISR;
		CANDR = 0x40;    //CLR FLAG
    }	

	if((isr & 0x20) == 0x20)  //EWI
    {
		CANAR = ISR;
		CANDR = 0x20;    //CLR FLAG
    }	

	if((isr & 0x10) == 0x10)  //EPI
    {
		CANAR = ISR;
		CANDR = 0x10;    //CLR FLAG
    }	

	if((isr & 0x02) == 0x02)  //BEI
    {
		CANAR = ISR;
		CANDR = 0x02;    //CLR FLAG
    }	

	if((isr & 0x01) == 0x01)  //DOI
    {
		CANAR = ISR;
		CANDR = 0x01;    //CLR FLAG
    }	

	CANAR = arTemp;    //CANAR�ֳ��ָ�
}
