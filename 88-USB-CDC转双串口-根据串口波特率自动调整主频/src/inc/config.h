#ifndef __CONFIG_H__
#define __CONFIG_H__

extern unsigned long MAIN_Fosc;

#define Dynamic_Frequency         //根据波特率动态调整主频控制开关
                                  //使能后可根据串口波特率动态调整主频以提高串口通信精度
                                  //不过一个通道波特率调整引发主频变化后，另一个通道的波特率计算就会受影响
                                  //不需要此功能可屏蔽这个定义


#define EP0_SIZE                64
#define EP1IN_SIZE              64
#define EP2IN_SIZE              64
#define EP3IN_SIZE              64
#define EP4IN_SIZE              64
#define EP5IN_SIZE              64
#define EP1OUT_SIZE             64
#define EP2OUT_SIZE             64
#define EP3OUT_SIZE             64
#define EP4OUT_SIZE             64
#define EP5OUT_SIZE             64

#define EN_EP2IN                        //串口1的数据IN端点
#define EN_EP3IN                        //串口2的控制IN端点
#define EN_EP4IN                        //串口1的数据IN端点
#define EN_EP5IN                        //串口2的控制IN端点
#define EN_EP4OUT                       //串口1的数据OUT端点
#define EN_EP5OUT                       //串口2的数据OUT端点

#endif
