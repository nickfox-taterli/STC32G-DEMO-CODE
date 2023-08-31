/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于STC32G为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

CAN总线扩展帧收发测试用例.

DCAN是一个支持CAN2.0B协议的功能单元。

收到一个扩展帧后，原样发送出去.

MCU每秒钟发送一帧固定数据.

默认波特率500KHz, 用户可自行修改.

下载时, 默认时钟 24MHz (用户可自行修改频率).

******************************************/

#include "..\comm\STC32G.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL

/****************************** 用户定义宏 ***********************************/
//CAN总线波特率=Fclk/((1+(TSG1+1)+(TSG2+1))*(BRP+1)*2)
#define TSG1    2		//0~15
#define TSG2    1		//1~7 (TSG2 不能设置为0)
#define BRP     3		//0~63
//24000000/((1+3+2)*4*2)=500KHz

#define SJW     1		//重新同步跳跃宽度

//总线波特率100KHz以上设置为 0; 100KHz以下设置为 1
#define SAM     0		//总线电平采样次数： 0:采样1次; 1:采样3次

/*****************************************************************************/


/*************  本地常量声明    **************/

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

#define	STANDARD_FRAME   0     //帧格式：标准帧
#define	EXTENDED_FRAME   1     //帧格式：扩展帧

/*************  本地变量声明    **************/

typedef struct
{
	u8	DLC:4;          //数据长度, bit0~bit3
	u8	:2;             //空数据, bit4~bit5
	u8	RTR:1;          //帧类型, bit6
	u8	FF:1;           //帧格式, bit7
	u32	ID;             //CAN ID
	u8	DataBuffer[8];  //数据缓存
}CAN_DataDef;

CAN_DataDef CAN1_Tx;
CAN_DataDef CAN1_Rx[8];

bit B_CanSend;      //CAN 发送完成标志
bit B_CanRead;      //CAN 收到数据标志
bit B_1ms;          //1ms标志
u16 msecond;

/*************  本地函数声明    **************/
void CANInit();
u8 CanReadReg(u8 addr);
u8 CanReadMsg(CAN_DataDef *CAN);
void CanSendMsg(CAN_DataDef *CAN);

/********************* 主函数 *************************/
void main(void)
{
	u8 sr,i,n;

    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x30;   P0M0 = 0x30;   //设置P0.4、P0.5为漏极开路(实验箱加了上拉电阻到3.3V)
    P1M1 = 0x32;   P1M0 = 0x32;   //设置P1.1、P1.4、P1.5为漏极开路(实验箱加了上拉电阻到3.3V), P1.1在PWM当DAC电路通过电阻串联到P2.3
    P2M1 = 0x3c;   P2M0 = 0x3c;   //设置P2.2~P2.5为漏极开路(实验箱加了上拉电阻到3.3V)
    P3M1 = 0x50;   P3M0 = 0x50;   //设置P3.4、P3.6为漏极开路(实验箱加了上拉电阻到3.3V)
    P4M1 = 0x3c;   P4M0 = 0x3c;   //设置P4.2~P4.5为漏极开路(实验箱加了上拉电阻到3.3V)
    P5M1 = 0x0c;   P5M0 = 0x0c;   //设置P5.2、P5.3为漏极开路(实验箱加了上拉电阻到3.3V)
    P6M1 = 0xff;   P6M0 = 0xff;   //设置为漏极开路(实验箱加了上拉电阻到3.3V)
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

	AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
	TH0 = (u8)(Timer0_Reload / 256);
	TL0 = (u8)(Timer0_Reload % 256);
	ET0 = 1;    //Timer0 interrupt enable
	TR0 = 1;    //Tiner0 run

	CANInit();
	
	EA = 1;                 //打开总中断

	CAN1_Tx.FF = EXTENDED_FRAME;    //扩展帧
	CAN1_Tx.RTR = 0;                //0：数据帧，1：远程帧
	CAN1_Tx.DLC = 0x08;             //数据长度
	CAN1_Tx.ID = 0x01234567;        //CAN ID
	CAN1_Tx.DataBuffer[0] = 0x21;   //数据内容
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
		if(B_1ms)   //1ms到
		{
			B_1ms = 0;
			if(++msecond >= 1000)   //1秒到
			{
				msecond = 0;
				sr = CanReadReg(SR);

				if(sr & 0x01)		//判断是否有 BS:BUS-OFF状态
				{
					CANAR = MR;
					CANDR &= ~0x04;  //清除 Reset Mode, 从BUS-OFF状态退出
				}
				else
				{
                    CanSendMsg(&CAN1_Tx);   //发送一帧数据
				}
			}
		}

		if(B_CanRead)
		{
			B_CanRead = 0;
			
            n = CanReadMsg(CAN1_Rx);    //读取接收内容
            if(n>0)
            {
                for(i=0;i<n;i++)
                {
                    CanSendMsg(&CAN1_Rx[i]);  //CAN总线原样返回
                }
            }
		}
	}
}


/********************** Timer0 1ms中断函数 ************************/
void timer0 (void) interrupt 1
{
    B_1ms = 1;      //1ms标志
}

//========================================================================
// 函数: u8 ReadReg(u8 addr)
// 描述: CAN功能寄存器读取函数。
// 参数: CAN功能寄存器地址.
// 返回: CAN功能寄存器数据.
// 版本: VER1.0
// 日期: 2020-11-16
// 备注: 
//========================================================================
u8 CanReadReg(u8 addr)
{
	u8 dat;
	CANAR = addr;
	dat = CANDR;
	return dat;
}

//========================================================================
// 函数: void WriteReg(u8 addr, u8 dat)
// 描述: CAN功能寄存器配置函数。
// 参数: CAN功能寄存器地址, CAN功能寄存器数据.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-11-16
// 备注: 
//========================================================================
void CanWriteReg(u8 addr, u8 dat)
{
	CANAR = addr;
	CANDR = dat;
}

//========================================================================
// 函数: void CanReadFifo(CAN_DataDef *CANx)
// 描述: 读取CAN缓冲区数据函数。
// 参数: *CANx: 存放CAN总线读取数据.
// 返回: none.
// 版本: VER2.0
// 日期: 2023-01-31
// 备注: 
//========================================================================
void CanReadFifo(CAN_DataDef *CAN)
{
    u8 i;
    u8 pdat[5];
    u8 RX_Index=0;

    pdat[0] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));

    if(pdat[0] & 0x80)  //判断是标准帧还是扩展帧
    {
        pdat[1] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));   //扩展帧ID占4个字节
        pdat[2] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));
        pdat[3] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));
        pdat[4] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));
        CAN->ID = (((u32)pdat[1] << 24) + ((u32)pdat[2] << 16) + ((u32)pdat[3] << 8) + pdat[4]) >> 3;
    }
    else
    {
        pdat[1] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));   //标准帧ID占2个字节
        pdat[2] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));
        CAN->ID = ((pdat[1] << 8) + pdat[2]) >> 5;
    }
    
    CAN->FF = pdat[0] >> 7;     //帧格式
    CAN->RTR = pdat[0] >> 6;    //帧类型
    CAN->DLC = pdat[0];         //数据长度

    for(i=0;((i<CAN->DLC) && (i<8));i++)        //读取数据长度为len，最多不超过8
    {
        CAN->DataBuffer[i] = CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));   //读取有效数据
    }
    while(RX_Index&3)   //判断已读数据长度是否4的整数倍
    {
        CanReadReg((u8)(RX_BUF0 + (RX_Index++&3)));  //读取填充数据，一帧数据占据4的整数倍缓冲区空间，不足补0
    }
}

//========================================================================
// 函数: u8 CanReadMsg(void)
// 描述: CAN接收数据函数。
// 参数: *CANx: 存放CAN总线读取数据..
// 返回: 帧个数.
// 版本: VER2.0
// 日期: 2023-01-31
// 备注: 
//========================================================================
u8 CanReadMsg(CAN_DataDef *CAN)
{
    u8 i;
    u8 n=0;

    do{
        CanReadFifo(&CAN[n++]);  //读取接收缓冲区数据
        i = CanReadReg(SR);
    }while(i&0x80);     //判断接收缓冲区里是否还有数据，有的话继续读取

    return n;   //返回帧个数
}

//========================================================================
// 函数: void CanSendMsg(CAN_DataDef *CAN)
// 描述: CAN发送标准帧函数。
// 参数: *CANx: 存放CAN总线发送数据..
// 返回: none.
// 版本: VER1.0
// 日期: 2020-11-19
// 备注: 
//========================================================================
void CanSendMsg(CAN_DataDef *CAN)
{
	u32 CanID;
    u8 RX_Index,i;

    while(B_CanSend);   //等待发送完成

    if(CAN->FF)     //判断是否扩展帧
    {
        CanID = CAN->ID << 3;
        CanWriteReg(TX_BUF0,CAN->DLC|((u8)CAN->RTR<<6)|0x80);	//bit7: 标准帧(0)/扩展帧(1), bit6: 数据帧(0)/远程帧(1), bit3~bit0: 数据长度(DLC)
        CanWriteReg(TX_BUF1,(u8)(CanID>>24));
        CanWriteReg(TX_BUF2,(u8)(CanID>>16));
        CanWriteReg(TX_BUF3,(u8)(CanID>>8));

        CanWriteReg(TX_BUF0,(u8)CanID);

        RX_Index = 1;
        for(i=0;((i<CAN->DLC) && (i<8));i++)        //数据长度为DLC，最多不超过8
        {
            CanWriteReg((u8)(TX_BUF0 + (RX_Index++&3)),CAN->DataBuffer[i]);   //写入有效数据
        }
        while(RX_Index&3)   //判断已读数据长度是否4的整数倍
        {
            CanWriteReg((u8)(TX_BUF0 + (RX_Index++&3)),0x00);  //写入填充数据，一帧数据占据4的整数倍缓冲区空间，不足补0
        }
    }
    else    //发送标准帧
    {
        CanID = (u16)(CAN->ID << 5);
        CanWriteReg(TX_BUF0,CAN->DLC|((u8)CAN->RTR<<6));  //bit7: 标准帧(0)/扩展帧(1), bit6: 数据帧(0)/远程帧(1), bit3~bit0: 数据长度(DLC)
        CanWriteReg(TX_BUF1,(u8)(CanID>>8));
        CanWriteReg(TX_BUF2,(u8)CanID);

        RX_Index = 3;
        for(i=0;((i<CAN->DLC) && (i<8));i++)        //数据长度为DLC，最多不超过8
        {
            CanWriteReg((u8)(TX_BUF0 + (RX_Index++&3)),CAN->DataBuffer[i]);   //写入有效数据
        }
        while(RX_Index&3)   //判断已读数据长度是否4的整数倍
        {
            CanWriteReg((u8)(TX_BUF0 + (RX_Index++&3)),0x00);  //写入填充数据，一帧数据占据4的整数倍缓冲区空间，不足补0
        }
    }
	CanWriteReg(CMR ,0x04);		//发起一次帧传输
    B_CanSend = 1;     //设置发送忙标志
}

//========================================================================
// 函数: void CANSetBaudrate()
// 描述: CAN总线波特率设置函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-11-19
// 备注: 
//========================================================================
void CANSetBaudrate()
{
	CanWriteReg(BTR0,(SJW << 6) + BRP);
	CanWriteReg(BTR1,(SAM << 7) + (TSG2 << 4) + TSG1);
}

//========================================================================
// 函数: void CANInit()
// 描述: CAN初始化函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-11-19
// 备注: 
//========================================================================
void CANInit()
{
	CANEN = 1;          //CAN1模块使能
	CanWriteReg(MR  ,0x05);  //使能 Reset Mode, 采用单滤波设置

	CANSetBaudrate();	//设置波特率
	
	//设置过滤ID为：xxF8xxxx 的帧才接收
//	CanWriteReg(ACR0,0x07);		//总线验收代码寄存器
//	CanWriteReg(ACR1,0xc0);
//	CanWriteReg(ACR2,0x00);
//	CanWriteReg(ACR3,0x00);
//	CanWriteReg(AMR0,0xF8);		//总线验收屏蔽寄存器
//	CanWriteReg(AMR1,0x07);
//	CanWriteReg(AMR2,0xFF);
//	CanWriteReg(AMR3,0xFF);

	//取消过滤ID，所有帧都接收
	CanWriteReg(ACR0,0x00);		//总线验收代码寄存器
	CanWriteReg(ACR1,0x00);
	CanWriteReg(ACR2,0x00);
	CanWriteReg(ACR3,0x00);
	CanWriteReg(AMR0,0xFF);		//总线验收屏蔽寄存器
	CanWriteReg(AMR1,0xFF);
	CanWriteReg(AMR2,0xFF);
	CanWriteReg(AMR3,0xFF);

	CanWriteReg(IMR ,0xff);		//中断寄存器
	CanWriteReg(ISR ,0xff);		//清中断标志
	CanWriteReg(MR  ,0x01);		//退出 Reset Mode, 采用单滤波设置(设置过滤器后注意选择滤波模式)
//	CanWriteReg(MR  ,0x00);		//退出 Reset Mode, 采用双滤波设置(设置过滤器后注意选择滤波模式)

	P_SW1 = (P_SW1 & ~(3<<4)) | (0<<4); //端口切换(CAN_Rx,CAN_Tx) 0x00:P0.0,P0.1  0x10:P5.0,P5.1  0x20:P4.2,P4.5  0x30:P7.0,P7.1
	CANICR = 0x02;		//CAN中断使能
}

//========================================================================
// 函数: void CANBUS_Interrupt(void) interrupt CAN1_VECTOR
// 描述: CAN总线中断函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-11-19
// 备注: 
//========================================================================
void CANBUS_Interrupt(void) interrupt CAN1_VECTOR
{
	u8 isr;
	u8 arTemp;
	arTemp = CANAR;     //CANAR现场保存，避免主循环里写完 CANAR 后产生中断，在中断里修改了 CANAR 内容
	
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

	CANAR = arTemp;    //CANAR现场恢复
}
