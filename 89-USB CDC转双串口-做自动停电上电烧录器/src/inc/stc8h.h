#ifndef __STC8H_H_
#define __STC8H_H_

/////////////////////////////////////////////////

//包含本头文件后,不用另外再包含"REG51.H"

sfr         P0          =           0x80;
sbit        P00         =           P0^0;
sbit        P01         =           P0^1;
sbit        P02         =           P0^2;
sbit        P03         =           P0^3;
sbit        P04         =           P0^4;
sbit        P05         =           P0^5;
sbit        P06         =           P0^6;
sbit        P07         =           P0^7;
sfr         SP          =           0x81;
sfr         DPL         =           0x82;
sfr         DPH         =           0x83;
sfr         S4CON       =           0x84;
#define     S4SM0                   0x80
#define     S4ST4                   0x40
#define     S4SM2                   0x20
#define     S4REN                   0x10
#define     S4TB8                   0x08
#define     S4RB8                   0x04
#define     S4TI                    0x02
#define     S4RI                    0x01
sfr         S4BUF       =           0x85;
sfr         PCON        =           0x87;
#define     SMOD                    0x80
#define     SMOD0                   0x40
#define     LVDF                    0x20
#define     POF                     0x10
#define     GF1                     0x08
#define     GF0                     0x04
#define     PD                      0x02
#define     IDL                     0x01
sfr         TCON        =           0x88;
sbit        TF1         =           TCON^7;
sbit        TR1         =           TCON^6;
sbit        TF0         =           TCON^5;
sbit        TR0         =           TCON^4;
sbit        IE1         =           TCON^3;
sbit        IT1         =           TCON^2;
sbit        IE0         =           TCON^1;
sbit        IT0         =           TCON^0;
sfr         TMOD        =           0x89;
#define     T1_GATE                 0x80
#define     T1_CT                   0x40
#define     T1_M1                   0x20
#define     T1_M0                   0x10
#define     T0_GATE                 0x08
#define     T0_CT                   0x04
#define     T0_M1                   0x02
#define     T0_M0                   0x01
sfr         TL0         =           0x8a;
sfr         TL1         =           0x8b;
sfr         TH0         =           0x8c;
sfr         TH1         =           0x8d;
sfr         AUXR        =           0x8e;
#define     T0x12                   0x80
#define     T1x12                   0x40
#define     UART_M0x6               0x20
#define     T2R                     0x10
#define     T2_CT                   0x08
#define     T2x12                   0x04
#define     EXTRAM                  0x02
#define     S1ST2                   0x01
sfr         INTCLKO     =           0x8f;
#define     EX4                     0x40
#define     EX3                     0x20
#define     EX2                     0x10
#define     T2CLKO                  0x04
#define     T1CLKO                  0x02
#define     T0CLKO                  0x01
sfr         P1          =           0x90;
sbit        P10         =           P1^0;
sbit        P11         =           P1^1;
sbit        P12         =           P1^2;
sbit        P13         =           P1^3;
sbit        P14         =           P1^4;
sbit        P15         =           P1^5;
sbit        P16         =           P1^6;
sbit        P17         =           P1^7;
sfr         P1M1        =           0x91;
sfr         P1M0        =           0x92;
sfr         P0M1        =           0x93;
sfr         P0M0        =           0x94;
sfr         P2M1        =           0x95;
sfr         P2M0        =           0x96;
sfr         SCON        =           0x98;
sbit        SM0         =           SCON^7;
sbit        SM1         =           SCON^6;
sbit        SM2         =           SCON^5;
sbit        REN         =           SCON^4;
sbit        TB8         =           SCON^3;
sbit        RB8         =           SCON^2;
sbit        TI          =           SCON^1;
sbit        RI          =           SCON^0;
sfr         SBUF        =           0x99;
sfr         S2CON       =           0x9a;
#define     S2SM0                   0x80
#define     S2ST4                   0x40
#define     S2SM2                   0x20
#define     S2REN                   0x10
#define     S2TB8                   0x08
#define     S2RB8                   0x04
#define     S2TI                    0x02
#define     S2RI                    0x01
sfr         S2BUF       =           0x9b;
sfr         IRCBAND     =           0x9d;
sfr         LIRTRIM     =           0x9e;
sfr         IRTRIM      =           0x9f;
sfr         P2          =           0xa0;
sbit        P20         =           P2^0;
sbit        P21         =           P2^1;
sbit        P22         =           P2^2;
sbit        P23         =           P2^3;
sbit        P24         =           P2^4;
sbit        P25         =           P2^5;
sbit        P26         =           P2^6;
sbit        P27         =           P2^7;
sfr         P_SW1       =           0xa2;
#define     UART1_S1                0x00
#define     UART1_S2                0x40
#define     UART1_S3                0x80
#define     UART1_S4                0xc0
#define     SPI_S1                  0x00
#define     SPI_S2                  0x04
#define     SPI_S3                  0x08
#define     SPI_S4                  0x0c
sfr         VRTRIM      =           0xa6;
sfr         IE          =           0xa8;
sbit        EA          =           IE^7;
sbit        ELVD        =           IE^6;
sbit        EADC        =           IE^5;
sbit        ES          =           IE^4;
sbit        ET1         =           IE^3;
sbit        EX1         =           IE^2;
sbit        ET0         =           IE^1;
sbit        EX0         =           IE^0;
sfr         SADDR       =           0xa9;
sfr         WKTCL       =           0xaa;
sfr         WKTCH       =           0xab;
#define     WKTEN                   0x80
sfr         S3CON       =           0xac;
#define     S3SM0                   0x80
#define     S3ST4                   0x40
#define     S3SM2                   0x20
#define     S3REN                   0x10
#define     S3TB8                   0x08
#define     S3RB8                   0x04
#define     S3TI                    0x02
#define     S3RI                    0x01
sfr         S3BUF       =           0xad;
sfr         TA          =           0xae;
sfr         IE2         =           0xaf;
#define     EUSB                    0x80
#define     ET4                     0x40
#define     ET3                     0x20
#define     ES4                     0x10
#define     ES3                     0x08
#define     ET2                     0x04
#define     ESPI                    0x02
#define     ES2                     0x01
sfr         P3          =           0xb0;
sbit        P30         =           P3^0;
sbit        P31         =           P3^1;
sbit        P32         =           P3^2;
sbit        P33         =           P3^3;
sbit        P34         =           P3^4;
sbit        P35         =           P3^5;
sbit        P36         =           P3^6;
sbit        P37         =           P3^7;
sfr         P3M1        =           0xb1;
sfr         P3M0        =           0xb2;
sfr         P4M1        =           0xb3;
sfr         P4M0        =           0xb4;
sfr         IP2         =           0xb5;
#define     PI2C                    0x40
#define     PCMP                    0x20
#define     PX4                     0x10
#define     PPWM2                   0x08
#define     PPWM1                   0x04
#define     PSPI                    0x02
#define     PS2                     0x01
sfr         IP2H        =           0xb6;
#define     PI2CH                   0x40
#define     PCMPH                   0x20
#define     PX4H                    0x10
#define     PPWM2H                  0x08
#define     PPWM1H                  0x04
#define     PSPIH                   0x02
#define     PS2H                    0x01
sfr         IPH         =           0xb7;
#define     PLVDH                   0x40
#define     PADCH                   0x20
#define     PSH                     0x10
#define     PT1H                    0x08
#define     PX1H                    0x04
#define     PT0H                    0x02
#define     PX0H                    0x01
sfr         IP          =           0xb8;
sbit        PLVD        =           IP^6;
sbit        PADC        =           IP^5;
sbit        PS          =           IP^4;
sbit        PT1         =           IP^3;
sbit        PX1         =           IP^2;
sbit        PT0         =           IP^1;
sbit        PX0         =           IP^0;
sfr         SADEN       =           0xb9;
sfr         P_SW2       =           0xba;
#define     EAXFR                   0x80
#define     I2C_S1                  0x00
#define     I2C_S2                  0x10
#define     I2C_S3                  0x20
#define     I2C_S4                  0x30
#define     CMPO_S1                 0x00
#define     CMPO_S2                 0x08
#define     UART4_S1                0x00
#define     UART4_S2                0x04
#define     UART3_S1                0x00
#define     UART3_S2                0x02
#define     UART2_S1                0x00
#define     UART2_S2                0x01
sfr         VOCTRL      =           0xbb;
sfr         ADC_CONTR   =           0xbc;
#define     ADC_POWER               0x80
#define     ADC_START               0x40
#define     ADC_FLAG                0x20
sfr         ADC_RES     =           0xbd;
sfr         ADC_RESL    =           0xbe;
sfr         P4          =           0xc0;
sbit        P40         =           P4^0;
sbit        P41         =           P4^1;
sbit        P42         =           P4^2;
sbit        P43         =           P4^3;
sbit        P44         =           P4^4;
sbit        P45         =           P4^5;
sbit        P46         =           P4^6;
sbit        P47         =           P4^7;
sfr         WDT_CONTR   =           0xc1;
#define     WDT_FLAG                0x80
#define     EN_WDT                  0x20
#define     CLR_WDT                 0x10
#define     IDL_WDT                 0x08
sfr         IAP_DATA    =           0xc2;
sfr         IAP_ADDRH   =           0xc3;
sfr         IAP_ADDRL   =           0xc4;
sfr         IAP_CMD     =           0xc5;
#define     IAP_IDL                 0x00
#define     IAP_READ                0x01
#define     IAP_WRITE               0x02
#define     IAP_ERASE               0x03
sfr         IAP_TRIG    =           0xc6;
sfr         IAP_CONTR   =           0xc7;
#define     IAPEN                   0x80
#define     SWBS                    0x40
#define     SWRST                   0x20
#define     CMD_FAIL                0x10
sfr         P5          =           0xc8;
sbit        P50         =           P5^0;
sbit        P51         =           P5^1;
sbit        P52         =           P5^2;
sbit        P53         =           P5^3;
sbit        P54         =           P5^4;
sbit        P55         =           P5^5;
sbit        P56         =           P5^6;
sbit        P57         =           P5^7;
sfr         P5M1        =           0xc9;
sfr         P5M0        =           0xca;
sfr         P6M1        =           0xcb;
sfr         P6M0        =           0xcc;
sfr         SPSTAT      =           0xcd;
#define     SPIF                    0x80
#define     WCOL                    0x40
sfr         SPCTL       =           0xce;
#define     SSIG                    0x80
#define     SPEN                    0x40
#define     DORD                    0x20
#define     MSTR                    0x10
#define     CPOL                    0x08
#define     CPHA                    0x04
sfr         SPDAT       =           0xcf;
sfr         PSW         =           0xd0;
sbit        CY          =           PSW^7;
sbit        AC          =           PSW^6;
sbit        F0          =           PSW^5;
sbit        RS1         =           PSW^4;
sbit        RS0         =           PSW^3;
sbit        OV          =           PSW^2;
sbit        P           =           PSW^0;
sfr         T4T3M       =           0xd1;
#define     T4R                     0x80
#define     T4_CT                   0x40
#define     T4x12                   0x20
#define     T4CLKO                  0x10
#define     T3R                     0x08
#define     T3_CT                   0x04
#define     T3x12                   0x02
#define     T3CLKO                  0x01
sfr         T4H         =           0xd2;
sfr         T4L         =           0xd3;
sfr         T3H         =           0xd4;
sfr         T3L         =           0xd5;
sfr         T2H         =           0xd6;
sfr         T2L         =           0xd7;
sfr         ADCCFG      =           0xde;
#define     ADC_RESFMT              0x20
sfr         IP3         =           0xdf;
#define     PS4                     0x02
#define     PS3                     0x01
sfr         ACC         =           0xe0;
sfr         P7M1        =           0xe1;
sfr         P7M0        =           0xe2;
sfr         DPS         =           0xe3;
sfr         DPL1        =           0xe4;
sfr         DPH1        =           0xe5;
sfr         CMPCR1      =           0xe6;
#define     CMPEN                   0x80
#define     CMPIF                   0x40
#define     PIE                     0x20
#define     NIE                     0x10
#define     PIS                     0x08
#define     NIS                     0x04
#define     CMPOE                   0x02
#define     CMPRES                  0x01
sfr         CMPCR2      =           0xe7;
#define     INVCMPO                 0x80
#define     DISFLT                  0x40
sfr         P6          =           0xe8;
sbit        P60         =           P6^0;
sbit        P61         =           P6^1;
sbit        P62         =           P6^2;
sbit        P63         =           P6^3;
sbit        P64         =           P6^4;
sbit        P65         =           P6^5;
sbit        P66         =           P6^6;
sbit        P67         =           P6^7;
sfr         IP3H        =           0xee;
#define     PS4H                    0x02
#define     PS3H                    0x01
sfr         AUXINTIF    =           0xef;
#define     INT4IF                  0x40
#define     INT3IF                  0x20
#define     INT2IF                  0x10
#define     T4IF                    0x04
#define     T3IF                    0x02
#define     T2IF                    0x01
sfr         B           =           0xf0;
sfr         IAP_TPS     =           0xf5;
sfr         P7          =           0xf8;
sbit        P70         =           P7^0;
sbit        P71         =           P7^1;
sbit        P72         =           P7^2;
sbit        P73         =           P7^3;
sbit        P74         =           P7^4;
sbit        P75         =           P7^5;
sbit        P76         =           P7^6;
sbit        P77         =           P7^7;
sfr         RSTCFG      =           0xff;
#define     ENLVR                   0x40
#define     P54RST                  0x10
#define     LVD_S1                  0x00
#define     LVD_S2                  0x01
#define     LVD_S3                  0x02
#define     LVD_S4                  0x03

sfr         USBCLK      =           0xdc;
sfr         USBDAT      =           0xec;
sfr         USBCON      =           0xf4;
sfr         USBADR      =           0xfc;

#define     USBBASE                 0
#define     FADDR                   (USBBASE + 0)
#define     UPDATE                  0x80
#define     POWER                   (USBBASE + 1)
#define     ISOUD                   0x80
#define     USBRST                  0x08
#define     USBRSU                  0x04
#define     USBSUS                  0x02
#define     ENSUS                   0x01
#define     INTRIN1                 (USBBASE + 2)
#define     EP5INIF                 0x20
#define     EP4INIF                 0x10
#define     EP3INIF                 0x08
#define     EP2INIF                 0x04
#define     EP1INIF                 0x02
#define     EP0IF                   0x01
#define     INTROUT1                (USBBASE + 4)
#define     EP5OUTIF                0x20
#define     EP4OUTIF                0x10
#define     EP3OUTIF                0x08
#define     EP2OUTIF                0x04
#define     EP1OUTIF                0x02
#define     INTRUSB                 (USBBASE + 6)
#define     SOFIF                   0x08
#define     RSTIF                   0x04
#define     RSUIF                   0x02
#define     SUSIF                   0x01
#define     INTRIN1E                (USBBASE + 7)
#define     EP5INIE                 0x20
#define     EP4INIE                 0x10
#define     EP3INIE                 0x08
#define     EP2INIE                 0x04
#define     EP1INIE                 0x02
#define     EP0IE                   0x01
#define     INTROUT1E               (USBBASE + 9)
#define     EP5OUTIE                0x20
#define     EP4OUTIE                0x10
#define     EP3OUTIE                0x08
#define     EP2OUTIE                0x04
#define     EP1OUTIE                0x02
#define     INTRUSBE                (USBBASE + 11)
#define     SOFIE                   0x08
#define     RSTIE                   0x04
#define     RSUIE                   0x02
#define     SUSIE                   0x01
#define     FRAME1                  (USBBASE + 12)
#define     FRAME2                  (USBBASE + 13)
#define     INDEX                   (USBBASE + 14)
#define     INMAXP                  (USBBASE + 16)
#define     CSR0                    (USBBASE + 17)
#define     SSUEND                  0x80
#define     SOPRDY                  0x40
#define     SDSTL                   0x20
#define     SUEND                   0x10
#define     DATEND                  0x08
#define     STSTL                   0x04
#define     IPRDY                   0x02
#define     OPRDY                   0x01
#define     INCSR1                  (USBBASE + 17)
#define     INCLRDT                 0x40
#define     INSTSTL                 0x20
#define     INSDSTL                 0x10
#define     INFLUSH                 0x08
#define     INUNDRUN                0x04
#define     INFIFONE                0x02
#define     INIPRDY                 0x01
#define     INCSR2                  (USBBASE + 18)
#define     INAUTOSET               0x80
#define     INISO                   0x40
#define     INMODEIN                0x20
#define     INMODEOUT               0x00
#define     INENDMA                 0x10
#define     INFCDT                  0x08
#define     OUTMAXP                 (USBBASE + 19)
#define     OUTCSR1                 (USBBASE + 20)
#define     OUTCLRDT                0x80
#define     OUTSTSTL                0x40
#define     OUTSDSTL                0x20
#define     OUTFLUSH                0x10
#define     OUTDATERR               0x08
#define     OUTOVRRUN               0x04
#define     OUTFIFOFUL              0x02
#define     OUTOPRDY                0x01
#define     OUTCSR2                 (USBBASE + 21)
#define     OUTAUTOCLR              0x80
#define     OUTISO                  0x40
#define     OUTENDMA                0x20
#define     OUTDMAMD                0x10
#define     COUNT0                  (USBBASE + 22)
#define     OUTCOUNT1               (USBBASE + 22)
#define     OUTCOUNT2               (USBBASE + 23)
#define     FIFO0                   (USBBASE + 32)
#define     FIFO1                   (USBBASE + 33)
#define     FIFO2                   (USBBASE + 34)
#define     FIFO3                   (USBBASE + 35)
#define     FIFO4                   (USBBASE + 36)
#define     FIFO5                   (USBBASE + 37)
#define     UTRKCTL                 (USBBASE + 48)
#define     UTRKSTS                 (USBBASE + 49)

//如下特殊功能寄存器位于扩展RAM区域
//访问这些寄存器,需先将P_SW2的BIT7设置为1,才可正常读写
#define     CKSEL                   (*(unsigned char volatile xdata *)0xfe00)
#define     MCK_IRC24M              0x00
#define     MCK_XOSC                0x01
#define     MCK_IRC32K              0x03
#define     CLKDIV                  (*(unsigned char volatile xdata *)0xfe01)
#define     IRC24MCR                (*(unsigned char volatile xdata *)0xfe02)
#define     ENIRC24M                0x80
#define     IRC24MST                0x01
#define     XOSCCR                  (*(unsigned char volatile xdata *)0xfe03)
#define     ENXOSC                  0x80
#define     XITYPE                  0x40
#define     XOSCST                  0x01
#define     IRC32KCR                (*(unsigned char volatile xdata *)0xfe04)
#define     ENIRC32K                0x80
#define     IRC32KST                0x01
#define     MCLKOCR                 (*(unsigned char volatile xdata *)0xfe05)
#define     MCLKO_S                 0x80
#define     IRC48MCR                (*(unsigned char volatile xdata *)0xfe07)
#define     P0PU                    (*(unsigned char volatile xdata *)0xfe10)
#define     P1PU                    (*(unsigned char volatile xdata *)0xfe11)
#define     P2PU                    (*(unsigned char volatile xdata *)0xfe12)
#define     P3PU                    (*(unsigned char volatile xdata *)0xfe13)
#define     P4PU                    (*(unsigned char volatile xdata *)0xfe14)
#define     P5PU                    (*(unsigned char volatile xdata *)0xfe15)
#define     P6PU                    (*(unsigned char volatile xdata *)0xfe16)
#define     P7PU                    (*(unsigned char volatile xdata *)0xfe17)
#define     P0NCS                   (*(unsigned char volatile xdata *)0xfe18)
#define     P1NCS                   (*(unsigned char volatile xdata *)0xfe19)
#define     P2NCS                   (*(unsigned char volatile xdata *)0xfe1a)
#define     P3NCS                   (*(unsigned char volatile xdata *)0xfe1b)
#define     P4NCS                   (*(unsigned char volatile xdata *)0xfe1c)
#define     P5NCS                   (*(unsigned char volatile xdata *)0xfe1d)
#define     P6NCS                   (*(unsigned char volatile xdata *)0xfe1e)
#define     P7NCS                   (*(unsigned char volatile xdata *)0xfe1f)
#define     P0SR                    (*(unsigned char volatile xdata *)0xfe20)
#define     P1SR                    (*(unsigned char volatile xdata *)0xfe21)
#define     P2SR                    (*(unsigned char volatile xdata *)0xfe22)
#define     P3SR                    (*(unsigned char volatile xdata *)0xfe23)
#define     P4SR                    (*(unsigned char volatile xdata *)0xfe24)
#define     P5SR                    (*(unsigned char volatile xdata *)0xfe25)
#define     P6SR                    (*(unsigned char volatile xdata *)0xfe26)
#define     P7SR                    (*(unsigned char volatile xdata *)0xfe27)
#define     P0DR                    (*(unsigned char volatile xdata *)0xfe28)
#define     P1DR                    (*(unsigned char volatile xdata *)0xfe29)
#define     P2DR                    (*(unsigned char volatile xdata *)0xfe2a)
#define     P3DR                    (*(unsigned char volatile xdata *)0xfe2b)
#define     P4DR                    (*(unsigned char volatile xdata *)0xfe2c)
#define     P5DR                    (*(unsigned char volatile xdata *)0xfe2d)
#define     P6DR                    (*(unsigned char volatile xdata *)0xfe2e)
#define     P7DR                    (*(unsigned char volatile xdata *)0xfe2f)
#define     P0IE                    (*(unsigned char volatile xdata *)0xfe30)
#define     P1IE                    (*(unsigned char volatile xdata *)0xfe31)
#define     P2IE                    (*(unsigned char volatile xdata *)0xfe32)
#define     P3IE                    (*(unsigned char volatile xdata *)0xfe33)
#define     P4IE                    (*(unsigned char volatile xdata *)0xfe34)
#define     P5IE                    (*(unsigned char volatile xdata *)0xfe35)
#define     P6IE                    (*(unsigned char volatile xdata *)0xfe36)
#define     P7IE                    (*(unsigned char volatile xdata *)0xfe37)
#define     I2CCFG                  (*(unsigned char volatile xdata *)0xfe80)
#define     ENI2C                   0x80
#define     I2CMASTER               0x40
#define     I2CSLAVE                0x00
#define     I2CMSCR                 (*(unsigned char volatile xdata *)0xfe81)
#define     EMSI                    0x80
#define     MS_IDLE                     0x00
#define     MS_START                    0x01
#define     MS_SENDDAT                  0x02
#define     MS_RECVACK                  0x03
#define     MS_RECVDAT                  0x04
#define     MS_SENDACK                  0x05
#define     MS_STOP                     0x06
#define     MS_START_SENDDAT_RECVACK    0x09
#define     MS_SENDDAT_RECVACK          0x0a
#define     MS_RECVDAT_SENDACK          0x0b
#define     MS_RECVDAT_SENDNAK          0x0c
#define     I2CMSST                 (*(unsigned char volatile xdata *)0xfe82)
#define     MSBUSY                  0x80
#define     MSIF                    0x40
#define     MSACKI                  0x02
#define     MSACKO                  0x01
#define     I2CSLCR                 (*(unsigned char volatile xdata *)0xfe83)
#define     ESTAI                   0x40
#define     ERXI                    0x20
#define     ETXI                    0x10
#define     ESTOI                   0x08
#define     SLRST                   0x01
#define     I2CSLST                 (*(unsigned char volatile xdata *)0xfe84)
#define     SLBUSY                  0x80
#define     STAIF                   0x40
#define     RXIF                    0x20
#define     TXIF                    0x10
#define     STOIF                   0x08
#define     TXING                   0x04
#define     SLACKI                  0x02
#define     SLACKO                  0x01
#define     I2CSLADR                (*(unsigned char volatile xdata *)0xfe85)
#define     I2CTXD                  (*(unsigned char volatile xdata *)0xfe86)
#define     I2CRXD                  (*(unsigned char volatile xdata *)0xfe87)
#define     I2CMSAUX                (*(unsigned char volatile xdata *)0xfe88)
#define     WDTA                    0x01
#define     TM2PS                   (*(unsigned char volatile xdata *)0xfea2)
#define     TM3PS                   (*(unsigned char volatile xdata *)0xfea3)
#define     TM4PS                   (*(unsigned char volatile xdata *)0xfea4)
#define     ADCTIM                  (*(unsigned char volatile xdata *)0xfea8)
#define     PWM1_ETRPS              (*(unsigned char volatile xdata *)0xfeb0)
#define     PWM1_BRK_S1             0x00
#define     PWM1_BRK_S2             0x04
#define     PWM1_ETR_S1             0x00
#define     PWM1_ETR_S2             0x01
#define     PWM1_ETR_S3             0x02
#define     PWM1_ETR_S4             0x03
#define     PWM1_ENO                (*(unsigned char volatile xdata *)0xfeb1)
#define     PWM1_ENO4N              0x80
#define     PWM1_ENO4P              0x40
#define     PWM1_ENO3N              0x20
#define     PWM1_ENO3P              0x10
#define     PWM1_ENO2N              0x08
#define     PWM1_ENO2P              0x04
#define     PWM1_ENO1N              0x02
#define     PWM1_ENO1P              0x01
#define     PWM1_PS                 (*(unsigned char volatile xdata *)0xfeb2)
#define     PWM1_C4_S1              0x00
#define     PWM1_C4_S2              0x40
#define     PWM1_C4_S3              0x80
#define     PWM1_C4_S4              0xc0
#define     PWM1_C3_S1              0x00
#define     PWM1_C3_S2              0x10
#define     PWM1_C3_S3              0x20
#define     PWM1_C3_S4              0x30
#define     PWM1_C2_S1              0x00
#define     PWM1_C2_S2              0x04
#define     PWM1_C2_S3              0x08
#define     PWM1_C2_S4              0x0c
#define     PWM1_C1_S1              0x00
#define     PWM1_C1_S2              0x01
#define     PWM1_C1_S3              0x02
#define     PWM1_C1_S4              0x03
#define     PWM1_IOAUX              (*(unsigned char volatile xdata *)0xfeb3)
#define     PWM1_AUX4N              0x80
#define     PWM1_AUX4P              0x40
#define     PWM1_AUX3N              0x20
#define     PWM1_AUX3P              0x10
#define     PWM1_AUX2N              0x08
#define     PWM1_AUX2P              0x04
#define     PWM1_AUX1N              0x02
#define     PWM1_AUX1P              0x01
#define     PWM2_ETRPS              (*(unsigned char volatile xdata *)0xfeb4)
#define     PWm2_BRK_S1             0x00
#define     PWm2_BRK_S2             0x04
#define     PWm2_ETR_S1             0x00
#define     PWm2_ETR_S2             0x01
#define     PWm2_ETR_S3             0x02
#define     PWm2_ETR_S4             0x03
#define     PWM2_ENO                (*(unsigned char volatile xdata *)0xfeb5)
#define     PWM2_ENO8P              0x40
#define     PWM2_ENO7P              0x10
#define     PWM2_ENO6P              0x04
#define     PWM2_ENO5P              0x01
#define     PWM2_PS                 (*(unsigned char volatile xdata *)0xfeb6)
#define     PWM2_C8_S1              0x00
#define     PWM2_C8_S2              0x40
#define     PWM2_C8_S3              0x80
#define     PWM2_C8_S4              0xc0
#define     PWM2_C7_S1              0x00
#define     PWM2_C7_S2              0x10
#define     PWM2_C7_S3              0x20
#define     PWM2_C7_S4              0x30
#define     PWM2_C6_S1              0x00
#define     PWM2_C6_S2              0x04
#define     PWM2_C6_S3              0x08
#define     PWM2_C6_S4              0x0c
#define     PWM2_C5_S1              0x00
#define     PWM2_C5_S2              0x01
#define     PWM2_C5_S3              0x02
#define     PWM2_C5_S4              0x03
#define     PWM2_IOAUX              (*(unsigned char volatile xdata *)0xfeb7)
#define     PWM2_AUX8P              0x40
#define     PWM2_AUX7P              0x10
#define     PWM2_AUX6P              0x04
#define     PWM2_AUX5P              0x01
#define     PWM1_CR1                (*(unsigned char volatile xdata *)0xfec0)
#define     PWM1_ARPE               0x80
#define     PWM1_CMS0               0x00
#define     PWM1_CMS1               0x20
#define     PWM1_CMS2               0x40
#define     PWM1_CMS3               0x60
#define     PWM1_UP                 0x00
#define     PWM1_DOWN               0x10
#define     PWM1_OPM                0x08
#define     PWM1_URS                0x04
#define     PWM1_UDIS               0x02
#define     PWM1_CEN                0x01
#define     PWM1_CR2                (*(unsigned char volatile xdata *)0xfec1)
#define     PWM1_TI1S               0x80
#define     PWM1_MMS0               0x00
#define     PWM1_MMS1               0x10
#define     PWM1_MMS2               0x20
#define     PWM1_MMS3               0x30
#define     PWM1_MMS4               0x40
#define     PWM1_MMS5               0x50
#define     PWM1_MMS6               0x60
#define     PWM1_MMS7               0x70
#define     PWM1_COMS               0x04
#define     PWM1_CCPS               0x01
#define     PWM1_SMCR               (*(unsigned char volatile xdata *)0xfec2)
#define     PWM1_MSM                0x80
#define     PWM1_TS0                0x00
#define     PWM1_TS1                0x10
#define     PWM1_TS2                0x20
#define     PWM1_TS3                0x30
#define     PWM1_TS4                0x40
#define     PWM1_TS5                0x50
#define     PWM1_TS6                0x60
#define     PWM1_TS7                0x70
#define     PWM1_SMS0               0x00
#define     PWM1_SMS1               0x01
#define     PWM1_SMS2               0x02
#define     PWM1_SMS3               0x03
#define     PWM1_SMS4               0x04
#define     PWM1_SMS5               0x05
#define     PWM1_SMS6               0x06
#define     PWM1_SMS7               0x07
#define     PWM1_ETR                (*(unsigned char volatile xdata *)0xfec3)
#define     PWM1_ETP                0x80
#define     PWM1_ECE                0x40
#define     PWM1_ETPS0              0x00
#define     PWM1_ETPS1              0x10
#define     PWM1_ETPS2              0x20
#define     PWM1_ETPS3              0x30
#define     PWM1_IER                (*(unsigned char volatile xdata *)0xfec4)
#define     PWM1_BIE                0x80
#define     PWM1_TIE                0x40
#define     PWM1_COMIE              0x20
#define     PWM1_CC4IE              0x10
#define     PWM1_CC3IE              0x08
#define     PWM1_CC2IE              0x04
#define     PWM1_CC1IE              0x02
#define     PWM1_UIE                0x01
#define     PWM1_SR1                (*(unsigned char volatile xdata *)0xfec5)
#define     PWM1_BIF                0x80
#define     PWM1_TIF                0x40
#define     PWM1_COMIF              0x20
#define     PWM1_CC4IF              0x10
#define     PWM1_CC3IF              0x08
#define     PWM1_CC2IF              0x04
#define     PWM1_CC1IF              0x02
#define     PWM1_UIF                0x01
#define     PWM1_SR2                (*(unsigned char volatile xdata *)0xfec6)
#define     PWM1_CC4OF              0x10
#define     PWM1_CC3OF              0x08
#define     PWM1_CC2OF              0x04
#define     PWM1_CC1OF              0x02
#define     PWM1_EGR                (*(unsigned char volatile xdata *)0xfec7)
#define     PWM1_BG                 0x80
#define     PWM1_TG                 0x40
#define     PWM1_COMG               0x20
#define     PWM1_CC4G               0x10
#define     PWM1_CC3G               0x08
#define     PWM1_CC2G               0x04
#define     PWM1_CC1G               0x02
#define     PWM1_UG                 0x01
#define     PWM1_CCMR1              (*(unsigned char volatile xdata *)0xfec8)
#define     PWM1_OC1CE              0x80
#define     PWM1_OC1M0              0x00
#define     PWM1_OC1M1              0x10
#define     PWM1_OC1M2              0x20
#define     PWM1_OC1M3              0x30
#define     PWM1_OC1M4              0x40
#define     PWM1_OC1M5              0x50
#define     PWM1_OC1M6              0x60
#define     PWM1_OC1M7              0x70
#define     PWM1_OC1PE              0x08
#define     PWM1_OC1FE              0x04
#define     PWM1_CC1S0              0x00
#define     PWM1_CC1S1              0x01
#define     PWM1_CC1S2              0x02
#define     PWM1_CC1S3              0x03
#define     PWM1_IC1F0              0x00
#define     PWM1_IC1F1              0x01
#define     PWM1_IC1F2              0x02
#define     PWM1_IC1F3              0x03
#define     PWM1_IC1F4              0x04
#define     PWM1_IC1F5              0x05
#define     PWM1_IC1F6              0x06
#define     PWM1_IC1F7              0x07
#define     PWM1_IC1F8              0x08
#define     PWM1_IC1F9              0x09
#define     PWM1_IC1F10             0x0a
#define     PWM1_IC1F11             0x0b
#define     PWM1_IC1F12             0x0c
#define     PWM1_IC1F13             0x0d
#define     PWM1_IC1F14             0x0e
#define     PWM1_IC1F15             0x0f
#define     PWM1_CCMR2              (*(unsigned char volatile xdata *)0xfec9)
#define     PWM1_OC2CE              0x80
#define     PWM1_OC2M0              0x00
#define     PWM1_OC2M1              0x10
#define     PWM1_OC2M2              0x20
#define     PWM1_OC2M3              0x30
#define     PWM1_OC2M4              0x40
#define     PWM1_OC2M5              0x50
#define     PWM1_OC2M6              0x60
#define     PWM1_OC2M7              0x70
#define     PWM1_OC2PE              0x08
#define     PWM1_OC2FE              0x04
#define     PWM1_CC2S0              0x00
#define     PWM1_CC2S1              0x01
#define     PWM1_CC2S2              0x02
#define     PWM1_CC2S3              0x03
#define     PWM1_IC2F0              0x00
#define     PWM1_IC2F1              0x01
#define     PWM1_IC2F2              0x02
#define     PWM1_IC2F3              0x03
#define     PWM1_IC2F4              0x04
#define     PWM1_IC2F5              0x05
#define     PWM1_IC2F6              0x06
#define     PWM1_IC2F7              0x07
#define     PWM1_IC2F8              0x08
#define     PWM1_IC2F9              0x09
#define     PWM1_IC2F10             0x0a
#define     PWM1_IC2F11             0x0b
#define     PWM1_IC2F12             0x0c
#define     PWM1_IC2F13             0x0d
#define     PWM1_IC2F14             0x0e
#define     PWM1_IC2F15             0x0f
#define     PWM1_CCMR3              (*(unsigned char volatile xdata *)0xfeca)
#define     PWM1_OC3CE              0x80
#define     PWM1_OC3M0              0x00
#define     PWM1_OC3M1              0x10
#define     PWM1_OC3M2              0x20
#define     PWM1_OC3M3              0x30
#define     PWM1_OC3M4              0x40
#define     PWM1_OC3M5              0x50
#define     PWM1_OC3M6              0x60
#define     PWM1_OC3M7              0x70
#define     PWM1_OC3PE              0x08
#define     PWM1_OC3FE              0x04
#define     PWM1_CC3S0              0x00
#define     PWM1_CC3S1              0x01
#define     PWM1_CC3S2              0x02
#define     PWM1_CC3S3              0x03
#define     PWM1_IC3F0              0x00
#define     PWM1_IC3F1              0x01
#define     PWM1_IC3F2              0x02
#define     PWM1_IC3F3              0x03
#define     PWM1_IC3F4              0x04
#define     PWM1_IC3F5              0x05
#define     PWM1_IC3F6              0x06
#define     PWM1_IC3F7              0x07
#define     PWM1_IC3F8              0x08
#define     PWM1_IC3F9              0x09
#define     PWM1_IC3F10             0x0a
#define     PWM1_IC3F11             0x0b
#define     PWM1_IC3F12             0x0c
#define     PWM1_IC3F13             0x0d
#define     PWM1_IC3F14             0x0e
#define     PWM1_IC3F15             0x0f
#define     PWM1_CCMR4              (*(unsigned char volatile xdata *)0xfecb)
#define     PWM1_OC4CE              0x80
#define     PWM1_OC4M0              0x00
#define     PWM1_OC4M1              0x10
#define     PWM1_OC4M2              0x20
#define     PWM1_OC4M3              0x30
#define     PWM1_OC4M4              0x40
#define     PWM1_OC4M5              0x50
#define     PWM1_OC4M6              0x60
#define     PWM1_OC4M7              0x70
#define     PWM1_OC4PE              0x08
#define     PWM1_OC4FE              0x04
#define     PWM1_CC4S0              0x00
#define     PWM1_CC4S1              0x01
#define     PWM1_CC4S2              0x02
#define     PWM1_CC4S3              0x03
#define     PWM1_IC4F0              0x00
#define     PWM1_IC4F1              0x01
#define     PWM1_IC4F2              0x02
#define     PWM1_IC4F3              0x03
#define     PWM1_IC4F4              0x04
#define     PWM1_IC4F5              0x05
#define     PWM1_IC4F6              0x06
#define     PWM1_IC4F7              0x07
#define     PWM1_IC4F8              0x08
#define     PWM1_IC4F9              0x09
#define     PWM1_IC4F10             0x0a
#define     PWM1_IC4F11             0x0b
#define     PWM1_IC4F12             0x0c
#define     PWM1_IC4F13             0x0d
#define     PWM1_IC4F14             0x0e
#define     PWM1_IC4F15             0x0f
#define     PWM1_CCER1              (*(unsigned char volatile xdata *)0xfecc)
#define     PWM1_CC2NP              0x80
#define     PWM1_CC2NE              0x40
#define     PWM1_CC2P               0x20
#define     PWM1_CC2E               0x10
#define     PWM1_CC1NP              0x08
#define     PWM1_CC1NE              0x04
#define     PWM1_CC1P               0x02
#define     PWM1_CC1E               0x01
#define     PWM1_CCER2              (*(unsigned char volatile xdata *)0xfecd)
#define     PWM1_CC4NP              0x80
#define     PWM1_CC4NE              0x40
#define     PWM1_CC4P               0x20
#define     PWM1_CC4E               0x10
#define     PWM1_CC3NP              0x08
#define     PWM1_CC3NE              0x04
#define     PWM1_CC3P               0x02
#define     PWM1_CC3E               0x01
#define     PWM1_CNTR               (*(unsigned int volatile xdata *)0xfece)
#define     PWM1_CNTRH              (*(unsigned char volatile xdata *)0xfece)
#define     PWM1_CNTRL              (*(unsigned char volatile xdata *)0xfecf)
#define     PWM1_PSCR               (*(unsigned int volatile xdata *)0xfed0)
#define     PWM1_PSCRH              (*(unsigned char volatile xdata *)0xfed0)
#define     PWM1_PSCRL              (*(unsigned char volatile xdata *)0xfed1)
#define     PWM1_ARR                (*(unsigned int volatile xdata *)0xfed2)
#define     PWM1_ARRH               (*(unsigned char volatile xdata *)0xfed2)
#define     PWM1_ARRL               (*(unsigned char volatile xdata *)0xfed3)
#define     PWM1_RCR                (*(unsigned char volatile xdata *)0xfed4)
#define     PWM1_CCR1               (*(unsigned int volatile xdata *)0xfed5)
#define     PWM1_CCR1H              (*(unsigned char volatile xdata *)0xfed5)
#define     PWM1_CCR1L              (*(unsigned char volatile xdata *)0xfed6)
#define     PWM1_CCR2               (*(unsigned int volatile xdata *)0xfed7)
#define     PWM1_CCR2H              (*(unsigned char volatile xdata *)0xfed7)
#define     PWM1_CCR2L              (*(unsigned char volatile xdata *)0xfed8)
#define     PWM1_CCR3               (*(unsigned int volatile xdata *)0xfed9)
#define     PWM1_CCR3H              (*(unsigned char volatile xdata *)0xfed9)
#define     PWM1_CCR3L              (*(unsigned char volatile xdata *)0xfeda)
#define     PWM1_CCR4               (*(unsigned int volatile xdata *)0xfedb)
#define     PWM1_CCR4H              (*(unsigned char volatile xdata *)0xfedb)
#define     PWM1_CCR4L              (*(unsigned char volatile xdata *)0xfedc)
#define     PWM1_BKR                (*(unsigned char volatile xdata *)0xfedd)
#define     PWM1_MOE                0x80
#define     PWM1_AOE                0x40
#define     PWM1_BKP                0x20
#define     PWM1_BKE                0x10
#define     PWM1_OSSR               0x08
#define     PWM1_OSSI               0x04
#define     PWM1_LOCK0              0x00
#define     PWM1_LOCK1              0x01
#define     PWM1_LOCK2              0x02
#define     PWM1_LOCK3              0x03
#define     PWM1_DTR                (*(unsigned char volatile xdata *)0xfede)
#define     PWM1_OISR               (*(unsigned char volatile xdata *)0xfedf)
#define     PWM1_OIS4N              0x80
#define     PWM1_OIS4               0x40
#define     PWM1_OIS3N              0x20
#define     PWM1_OIS3               0x10
#define     PWM1_OIS2N              0x08
#define     PWM1_OIS2               0x04
#define     PWM1_OIS1N              0x02
#define     PWM1_OIS1               0x01
#define     PWM2_CR1                (*(unsigned char volatile xdata *)0xfee0)
#define     PWM2_ARPE               0x80
#define     PWM2_CMS0               0x00
#define     PWM2_CMS1               0x20
#define     PWM2_CMS2               0x40
#define     PWM2_CMS3               0x60
#define     PWM2_UP                 0x00
#define     PWM2_DOWN               0x10
#define     PWM2_OPM                0x08
#define     PWM2_URS                0x04
#define     PWM2_UDIS               0x02
#define     PWM2_CEN                0x01
#define     PWM2_CR2                (*(unsigned char volatile xdata *)0xfee1)
#define     PWM2_TI2S               0x80
#define     PWM2_MMS0               0x00
#define     PWM2_MMS1               0x10
#define     PWM2_MMS2               0x20
#define     PWM2_MMS3               0x30
#define     PWM2_MMS4               0x40
#define     PWM2_MMS5               0x50
#define     PWM2_MMS6               0x60
#define     PWM2_MMS7               0x70
#define     PWM2_COMS               0x04
#define     PWM2_CCPS               0x01
#define     PWM2_SMCR               (*(unsigned char volatile xdata *)0xfee2)
#define     PWM2_MSM                0x80
#define     PWM2_TS0                0x00
#define     PWM2_TS1                0x10
#define     PWM2_TS2                0x20
#define     PWM2_TS3                0x30
#define     PWM2_TS4                0x40
#define     PWM2_TS5                0x50
#define     PWM2_TS6                0x60
#define     PWM2_TS7                0x70
#define     PWM2_SMS0               0x00
#define     PWM2_SMS1               0x01
#define     PWM2_SMS2               0x02
#define     PWM2_SMS3               0x03
#define     PWM2_SMS4               0x04
#define     PWM2_SMS5               0x05
#define     PWM2_SMS6               0x06
#define     PWM2_SMS7               0x07
#define     PWM2_ETR                (*(unsigned char volatile xdata *)0xfee3)
#define     PWM2_ETP                0x80
#define     PWM2_ECE                0x40
#define     PWM2_ETPS0              0x00
#define     PWM2_ETPS1              0x10
#define     PWM2_ETPS2              0x20
#define     PWM2_ETPS3              0x30
#define     PWM2_IER                (*(unsigned char volatile xdata *)0xfee4)
#define     PWM2_BIE                0x80
#define     PWM2_TIE                0x40
#define     PWM2_COMIE              0x20
#define     PWM2_CC8IE              0x10
#define     PWM2_CC7IE              0x08
#define     PWM2_CC6IE              0x04
#define     PWM2_CC5IE              0x02
#define     PWM2_UIE                0x01
#define     PWM2_SR1                (*(unsigned char volatile xdata *)0xfee5)
#define     PWM2_BIF                0x80
#define     PWM2_TIF                0x40
#define     PWM2_COMIF              0x20
#define     PWM2_CC8IF              0x10
#define     PWM2_CC7IF              0x08
#define     PWM2_CC6IF              0x04
#define     PWM2_CC5IF              0x02
#define     PWM2_UIF                0x01
#define     PWM2_SR2                (*(unsigned char volatile xdata *)0xfee6)
#define     PWM2_CC8OF              0x10
#define     PWM2_CC7OF              0x08
#define     PWM2_CC6OF              0x04
#define     PWM2_CC5OF              0x02
#define     PWM2_EGR                (*(unsigned char volatile xdata *)0xfee7)
#define     PWM2_BG                 0x80
#define     PWM2_TG                 0x40
#define     PWM2_COMG               0x20
#define     PWM2_CC8G               0x10
#define     PWM2_CC7G               0x08
#define     PWM2_CC6G               0x04
#define     PWM2_CC5G               0x02
#define     PWM2_UG                 0x01
#define     PWM2_CCMR1              (*(unsigned char volatile xdata *)0xfee8)
#define     PWM2_OC5CE              0x80
#define     PWM2_OC5M0              0x00
#define     PWM2_OC5M1              0x10
#define     PWM2_OC5M2              0x20
#define     PWM2_OC5M3              0x30
#define     PWM2_OC5M4              0x40
#define     PWM2_OC5M5              0x50
#define     PWM2_OC5M6              0x60
#define     PWM2_OC5M7              0x70
#define     PWM2_OC5PE              0x08
#define     PWM2_OC5FE              0x04
#define     PWM2_CC5S0              0x00
#define     PWM2_CC5S1              0x01
#define     PWM2_CC5S2              0x02
#define     PWM2_CC5S3              0x03
#define     PWM2_IC5F0              0x00
#define     PWM2_IC5F1              0x01
#define     PWM2_IC5F2              0x02
#define     PWM2_IC5F3              0x03
#define     PWM2_IC5F4              0x04
#define     PWM2_IC5F5              0x05
#define     PWM2_IC5F6              0x06
#define     PWM2_IC5F7              0x07
#define     PWM2_IC5F8              0x08
#define     PWM2_IC5F9              0x09
#define     PWM2_IC5F10             0x0a
#define     PWM2_IC5F11             0x0b
#define     PWM2_IC5F12             0x0c
#define     PWM2_IC5F13             0x0d
#define     PWM2_IC5F14             0x0e
#define     PWM2_IC5F15             0x0f
#define     PWM2_CCMR2              (*(unsigned char volatile xdata *)0xfee9)
#define     PWM2_OC6CE              0x80
#define     PWM2_OC6M0              0x00
#define     PWM2_OC6M1              0x10
#define     PWM2_OC6M2              0x20
#define     PWM2_OC6M3              0x30
#define     PWM2_OC6M4              0x40
#define     PWM2_OC6M5              0x50
#define     PWM2_OC6M6              0x60
#define     PWM2_OC6M7              0x70
#define     PWM2_OC6PE              0x08
#define     PWM2_OC6FE              0x04
#define     PWM2_CC6S0              0x00
#define     PWM2_CC6S1              0x01
#define     PWM2_CC6S2              0x02
#define     PWM2_CC6S3              0x03
#define     PWM2_IC6F0              0x00
#define     PWM2_IC6F1              0x01
#define     PWM2_IC6F2              0x02
#define     PWM2_IC6F3              0x03
#define     PWM2_IC6F4              0x04
#define     PWM2_IC6F5              0x05
#define     PWM2_IC6F6              0x06
#define     PWM2_IC6F7              0x07
#define     PWM2_IC6F8              0x08
#define     PWM2_IC6F9              0x09
#define     PWM2_IC6F10             0x0a
#define     PWM2_IC6F11             0x0b
#define     PWM2_IC6F12             0x0c
#define     PWM2_IC6F13             0x0d
#define     PWM2_IC6F14             0x0e
#define     PWM2_IC6F15             0x0f
#define     PWM2_CCMR3              (*(unsigned char volatile xdata *)0xfeea)
#define     PWM2_OC7CE              0x80
#define     PWM2_OC7M0              0x00
#define     PWM2_OC7M1              0x10
#define     PWM2_OC7M2              0x20
#define     PWM2_OC7M3              0x30
#define     PWM2_OC7M4              0x40
#define     PWM2_OC7M5              0x50
#define     PWM2_OC7M6              0x60
#define     PWM2_OC7M7              0x70
#define     PWM2_OC7PE              0x08
#define     PWM2_OC7FE              0x04
#define     PWM2_CC7S0              0x00
#define     PWM2_CC7S1              0x01
#define     PWM2_CC7S2              0x02
#define     PWM2_CC7S3              0x03
#define     PWM2_IC7F0              0x00
#define     PWM2_IC7F1              0x01
#define     PWM2_IC7F2              0x02
#define     PWM2_IC7F3              0x03
#define     PWM2_IC7F4              0x04
#define     PWM2_IC7F5              0x05
#define     PWM2_IC7F6              0x06
#define     PWM2_IC7F7              0x07
#define     PWM2_IC7F8              0x08
#define     PWM2_IC7F9              0x09
#define     PWM2_IC7F10             0x0a
#define     PWM2_IC7F11             0x0b
#define     PWM2_IC7F12             0x0c
#define     PWM2_IC7F13             0x0d
#define     PWM2_IC7F14             0x0e
#define     PWM2_IC7F15             0x0f
#define     PWM2_CCMR4              (*(unsigned char volatile xdata *)0xfeeb)
#define     PWM2_OC8CE              0x80
#define     PWM2_OC8M0              0x00
#define     PWM2_OC8M1              0x10
#define     PWM2_OC8M2              0x20
#define     PWM2_OC8M3              0x30
#define     PWM2_OC8M4              0x40
#define     PWM2_OC8M5              0x50
#define     PWM2_OC8M6              0x60
#define     PWM2_OC8M7              0x70
#define     PWM2_OC8PE              0x08
#define     PWM2_OC8FE              0x04
#define     PWM2_CC8S0              0x00
#define     PWM2_CC8S1              0x01
#define     PWM2_CC8S2              0x02
#define     PWM2_CC8S3              0x03
#define     PWM2_IC8F0              0x00
#define     PWM2_IC8F1              0x01
#define     PWM2_IC8F2              0x02
#define     PWM2_IC8F3              0x03
#define     PWM2_IC8F4              0x04
#define     PWM2_IC8F5              0x05
#define     PWM2_IC8F6              0x06
#define     PWM2_IC8F7              0x07
#define     PWM2_IC8F8              0x08
#define     PWM2_IC8F9              0x09
#define     PWM2_IC8F10             0x0a
#define     PWM2_IC8F11             0x0b
#define     PWM2_IC8F12             0x0c
#define     PWM2_IC8F13             0x0d
#define     PWM2_IC8F14             0x0e
#define     PWM2_IC8F15             0x0f
#define     PWM2_CCER1              (*(unsigned char volatile xdata *)0xfeec)
#define     PWM2_CC6P               0x20
#define     PWM2_CC6E               0x10
#define     PWM2_CC5P               0x02
#define     PWM2_CC5E               0x01
#define     PWM2_CCER2              (*(unsigned char volatile xdata *)0xfeed)
#define     PWM2_CC8P               0x20
#define     PWM2_CC8E               0x10
#define     PWM2_CC7P               0x02
#define     PWM2_CC7E               0x01
#define     PWM2_CNTR               (*(unsigned int volatile xdata *)0xfeee)
#define     PWM2_CNTRH              (*(unsigned char volatile xdata *)0xfeee)
#define     PWM2_CNTRL              (*(unsigned char volatile xdata *)0xfeef)
#define     PWM2_PSCR               (*(unsigned int volatile xdata *)0xfef0)
#define     PWM2_PSCRH              (*(unsigned char volatile xdata *)0xfef0)
#define     PWM2_PSCRL              (*(unsigned char volatile xdata *)0xfef1)
#define     PWM2_ARR                (*(unsigned int volatile xdata *)0xfef2)
#define     PWM2_ARRH               (*(unsigned char volatile xdata *)0xfef2)
#define     PWM2_ARRL               (*(unsigned char volatile xdata *)0xfef3)
#define     PWM2_RCR                (*(unsigned char volatile xdata *)0xfef4)
#define     PWM2_CCR1               (*(unsigned int volatile xdata *)0xfef5)
#define     PWM2_CCR1H              (*(unsigned char volatile xdata *)0xfef5)
#define     PWM2_CCR1L              (*(unsigned char volatile xdata *)0xfef6)
#define     PWM2_CCR2               (*(unsigned int volatile xdata *)0xfef7)
#define     PWM2_CCR2H              (*(unsigned char volatile xdata *)0xfef7)
#define     PWM2_CCR2L              (*(unsigned char volatile xdata *)0xfef8)
#define     PWM2_CCR3               (*(unsigned int volatile xdata *)0xfef9)
#define     PWM2_CCR3H              (*(unsigned char volatile xdata *)0xfef9)
#define     PWM2_CCR3L              (*(unsigned char volatile xdata *)0xfefa)
#define     PWM2_CCR4               (*(unsigned int volatile xdata *)0xfefb)
#define     PWM2_CCR4H              (*(unsigned char volatile xdata *)0xfefb)
#define     PWM2_CCR4L              (*(unsigned char volatile xdata *)0xfefc)
#define     PWM2_BKR                (*(unsigned char volatile xdata *)0xfefd)
#define     PWM2_MOE                0x80
#define     PWM2_AOE                0x40
#define     PWM2_BKP                0x20
#define     PWM2_BKE                0x10
#define     PWM2_OSSR               0x08
#define     PWM2_OSSI               0x04
#define     PWM2_LOCK0              0x00
#define     PWM2_LOCK1              0x01
#define     PWM2_LOCK2              0x02
#define     PWM2_LOCK3              0x03
#define     PWM2_DTR                (*(unsigned char volatile xdata *)0xfefe)
#define     PWM2_OISR               (*(unsigned char volatile xdata *)0xfeff)
#define     PWM2_OIS8               0x40
#define     PWM2_OIS7               0x10
#define     PWM2_OIS6               0x04
#define     PWM2_OIS5               0x01

typedef struct TAG_PWM_STRUCT
{
    unsigned char CR1;
    unsigned char CR2;
    unsigned char SMCR;
    unsigned char ETR;
    unsigned char IER;
    unsigned char SR1;
    unsigned char SR2;
    unsigned char EGR;
    unsigned char CCMR1;
    unsigned char CCMR2;
    unsigned char CCMR3;
    unsigned char CCMR4;
    unsigned char CCER1;
    unsigned char CCER2;
    unsigned int CNTR;
    unsigned int PSCR;
    unsigned int ARR;
    unsigned char RCR;
    unsigned int CCR1;
    unsigned int CCR2;
    unsigned int CCR3;
    unsigned int CCR4;
    unsigned char BKR;
    unsigned char DTR;
    unsigned char OISR;
} PWM_STRUCT;

#define PWM1        ((PWM_STRUCT volatile xdata *)0xfec0)
#define PWM2        ((PWM_STRUCT volatile xdata *)0xfee0)

/////////////////////////////////////////////////

#define INT0_VECTOR         0       //0003H
#define TMR0_VECTOR         1       //000BH
#define INT1_VECTOR         2       //0013H
#define TMR1_VECTOR         3       //001BH
#define UART1_VECTOR        4       //0023H
#define ADC_VECTOR          5       //002BH
#define LVD_VECTOR          6       //0033H
#define PCA_VECTOR          7       //003BH
#define UART2_VECTOR        8       //0043H
#define SPI_VECTOR          9       //004BH
#define INT2_VECTOR         10      //0053H
#define INT3_VECTOR         11      //005BH
#define TMR2_VECTOR         12      //0063H
#define INT4_VECTOR         16      //0083H
#define UART3_VECTOR        17      //008BH
#define UART4_VECTOR        18      //0093H
#define TMR3_VECTOR         19      //009BH
#define TMR4_VECTOR         20      //00A3H
#define CMP_VECTOR          21      //00ABH
#define I2C_VECTOR          24      //00C3H
#define USB_VECTOR          25      //00CBH
#define PWM1_VECTOR         26      //00D3H
#define PWM2_VECTOR         27      //00DBH

/////////////////////////////////////////////////

#endif

