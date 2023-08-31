#ifndef __CONFIG_H__
#define __CONFIG_H__

#define FOSC                    24000000UL

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

#define ISPPWRCTRL                      //ISP下载时SVCC电源自动控制开关
                                        //使能此功能时,会侦测STC-ISP下载并对SVCC进行自动停电再上电
                                        //若不需要此功能只需要注释此行即可

#endif
